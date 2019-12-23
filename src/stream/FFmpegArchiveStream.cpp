/*
 *      Copyright (C) 2016 Arne Morten Kvarving
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "FFmpegArchiveStream.h"

#include "threads/SingleLock.h"
#include "../utils/Log.h"

#ifdef TARGET_POSIX
#include "platform/posix/XTimeUtils.h"
#endif

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#ifdef TARGET_POSIX
#include <stdint.h>
#endif

extern "C" {
#include <libavutil/dict.h>
#include <libavutil/opt.h>
}

#include "platform/posix/XTimeUtils.h"

#include <p8-platform/util/StringUtils.h>

/***********************************************************
* InputSteam Client AddOn specific public library functions
***********************************************************/

FFmpegArchiveStream::FFmpegArchiveStream(IManageDemuxPacket* demuxPacketManager)
  : FFmpegStream(demuxPacketManager), m_bIsOpening(false), m_seekOffset(0)
{
}

FFmpegArchiveStream::~FFmpegArchiveStream()
{

}

bool FFmpegArchiveStream::Open(const std::string& streamUrl, const std::string& mimeType, bool isRealTimeStream, const std::string& programProperty)
{
  m_bIsOpening = true;
  bool ret = FFmpegStream::Open(streamUrl, mimeType, isRealTimeStream, programProperty);
  m_bIsOpening = false;
  return ret;
}

bool FFmpegArchiveStream::DemuxSeekTime(double time, bool backwards, double& startpts)
{
  if (/*!m_pInput ||*/ time < 0)
    return false;

  int whence = m_bIsOpening ? SEEK_CUR : SEEK_SET;
  int64_t seekResult = SeekStream(static_cast<int64_t>(time), whence);
  if (seekResult >= 0)
  {
    {
      CSingleLock lock(m_critSection);
      m_seekOffset = seekResult;
    }

    Log(LOGLEVEL_DEBUG, "Seek successful. m_seekOffset = %f, m_currentPts = %f, time = %f, backwards = %d, startptr = %f",
      m_seekOffset, m_currentPts, time, backwards, startpts);

    if (!m_bIsOpening)
      DemuxReset();

    return true;
    //return m_bIsOpening ? true : DemuxReset();
  }

  Log(LOGLEVEL_DEBUG, "Seek failed. m_currentPts = %f, time = %f, backwards = %d, startptr = %f",
    m_currentPts, time, backwards, startpts);
  return false;
}

DemuxPacket* FFmpegArchiveStream::DemuxRead()
{
  DemuxPacket* pPacket = FFmpegStream::DemuxRead();
  if (pPacket)
  {
    CSingleLock lock(m_critSection);
    pPacket->pts += m_seekOffset;
    pPacket->dts += m_seekOffset;
  }

  return pPacket;
}

void FFmpegArchiveStream::GetCapabilities(INPUTSTREAM_CAPABILITIES& caps)
{
  kodi::Log(ADDON_LOG_DEBUG, "GetCapabilities()");
  caps.m_mask = INPUTSTREAM_CAPABILITIES::SUPPORTS_IDEMUX |
    INPUTSTREAM_CAPABILITIES::SUPPORTS_IDISPLAYTIME |
    INPUTSTREAM_CAPABILITIES::SUPPORTS_ITIME |
    INPUTSTREAM_CAPABILITIES::SUPPORTS_IPOSTIME |
    INPUTSTREAM_CAPABILITIES::SUPPORTS_SEEK |
    INPUTSTREAM_CAPABILITIES::SUPPORTS_PAUSE;
// #if INPUTSTREAM_VERSION_LEVEL > 1
//   caps.m_mask |= INPUTSTREAM_CAPABILITIES::SUPPORTS_ICHAPTER;
// #endif
}

int64_t FFmpegArchiveStream::SeekStream(int64_t position, int whence /* SEEK_SET */)
{
  long long ret = -1;
  if (m_catchupStartTime > 0)
  {
    Log(LOGLEVEL_NOTICE, "SeekLiveStream - iPosition = %lld, iWhence = %d", position, whence);
    const time_t timeNow = time(0);
    switch (whence)
    {
      case SEEK_SET:
      {
        Log(LOGLEVEL_NOTICE, "SeekLiveStream - SeekSet");
        position += 500;
        position /= 1000;
        if (m_catchupStartTime + position < timeNow - 10)
        {
          ret = position;
          m_timeshiftBufferOffset = position;
        }
        else
        {
          ret = timeNow - m_catchupStartTime;
          m_timeshiftBufferOffset = ret;
        }
        ret *= DVD_TIME_BASE;
      }
      break;
      case SEEK_CUR:
      {
        long long offset = m_timeshiftBufferOffset;
        //Log(LOGLEVEL_NOTICE, "SeekLiveStream - timeNow = %d, startTime = %d, iTvgShift = %d, offset = %d", timeNow, m_catchupStartTime, m_programmeChannelTvgShift, offset);
        ret = offset * DVD_TIME_BASE;
      }
      break;
      default:
        Log(LOGLEVEL_NOTICE, "SeekLiveStream - Unsupported SEEK command (%d)", whence);
      break;
    }
  }
  return ret;
}

int64_t FFmpegArchiveStream::LengthStream()
{
  long long ret = -1;
  Log(LOGLEVEL_NOTICE, "LengthLiveStream");
  if (m_catchupStartTime > 0 && m_catchupEndTime >= m_catchupStartTime)
    ret = (m_catchupEndTime - m_catchupStartTime) * DVD_TIME_BASE;
  return ret;
}

bool FFmpegArchiveStream::GetTimes(INPUTSTREAM_TIMES& times)
{
  if (m_timeshiftBufferStartTime == 0)
    return false;

  times = {0};
  const time_t dateTimeNow = time(0);

  times.startTime = m_timeshiftBufferStartTime;
  if (m_playbackIsVideo)
    times.ptsEnd = static_cast<int64_t>(std::min(dateTimeNow, m_catchupEndTime) - times.startTime) * DVD_TIME_BASE;
  else // it's live!
    times.ptsEnd = static_cast<int64_t>(dateTimeNow - times.startTime) * DVD_TIME_BASE;

  // Log(LOGLEVEL_NOTICE, "GetStreamTimes - Ch = %u \tTitle = \"%s\" \tepgTag->startTime = %ld \tepgTag->endTime = %ld",
  //           m_programmeUniqueChannelId, m_programmeTitle.c_str(), m_catchupStartTime, m_catchupEndTime);
  Log(LOGLEVEL_NOTICE, "GetStreamTimes - startTime = %ld \tptsStart = %lld \tptsBegin = %lld \tptsEnd = %lld",
            times.startTime, times.ptsStart, times.ptsBegin, times.ptsEnd);
  return true;
}

void FFmpegArchiveStream::UpdateCurrentPTS()
{
  FFmpegStream::UpdateCurrentPTS();
  if (m_currentPts != DVD_NOPTS_VALUE)
    m_currentPts += m_seekOffset;
}
