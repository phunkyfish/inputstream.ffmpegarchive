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

#include "StreamManager.h"

#include "utils/Log.h"

#include <p8-platform/util/StringUtils.h>

/***********************************************************
* InputSteam Client AddOn specific public library functions
***********************************************************/

void Log(const LogLevel loglevel, const char* format, ...)
{
  char buffer[16384];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);
  ::kodi::addon::CAddonBase::m_interface->toKodi->addon_log_msg(::kodi::addon::CAddonBase::m_interface->toKodi->kodiBase, loglevel, buffer);
}

CInputStreamArchive::CInputStreamArchive(KODI_HANDLE instance)
  : CInstanceInputStream(instance)
{
}

CInputStreamArchive::~CInputStreamArchive()
{

}

bool CInputStreamArchive::Open(INPUTSTREAM& props)
{
  kodi::Log(ADDON_LOG_NOTICE, "InputStream.ffmpegarchive: OpenStream()");
  std::string tempString;

  for (size_t i = 0; i < props.m_nCountInfoValues; ++i)
  {
    kodi::Log(ADDON_LOG_NOTICE, "InputStream.ffmpegarchive property: %s = %s", props.m_ListItemProperties[i].m_strKey, props.m_ListItemProperties[i].m_strValue);

    if (MIME_TYPE == props.m_ListItemProperties[i].m_strKey)
    {
      m_mimeType = props.m_ListItemProperties[i].m_strValue;
    }
    else if (PROGRAM_NUMBER == props.m_ListItemProperties[i].m_strKey)
    {
      m_programProperty = props.m_ListItemProperties[i].m_strValue;
    }
    else if (IS_REALTIME_STREAM == props.m_ListItemProperties[i].m_strKey)
    {
      m_isRealTimeStream = StringUtils::EqualsNoCase(props.m_ListItemProperties[i].m_strValue, "true");
    }
    else if (IS_ARCHIVE_STREAM == props.m_ListItemProperties[i].m_strKey)
    {
      m_isArchiveStream = StringUtils::EqualsNoCase(props.m_ListItemProperties[i].m_strValue, "true");
    }
    else if (PLAYBACK_AS_LIVE == props.m_ListItemProperties[i].m_strKey)
    {
      m_playbackAsLive = StringUtils::EqualsNoCase(props.m_ListItemProperties[i].m_strValue, "true");
    }
    else if (CATCHUP_START_TIME == props.m_ListItemProperties[i].m_strKey)
    {
      tempString = props.m_ListItemProperties[i].m_strValue;
      m_catchupStartTime = static_cast<time_t>(std::stoll(tempString));
    }
    else if (CATCHUP_END_TIME == props.m_ListItemProperties[i].m_strKey)
    {
      tempString = props.m_ListItemProperties[i].m_strValue;
      m_catchupEndTime = static_cast<time_t>(std::stoll(tempString));
    }
    else if (TIMESHIFT_BUFFER_START_TIME == props.m_ListItemProperties[i].m_strKey)
    {
      tempString = props.m_ListItemProperties[i].m_strValue;
      m_timeshiftBufferStartTime = static_cast<time_t>(std::stoll(tempString));
    }
    else if (TIMESHIFT_BUFFER_OFFSET == props.m_ListItemProperties[i].m_strKey)
    {
      tempString = props.m_ListItemProperties[i].m_strValue;
      m_timeshiftBufferOffset = std::stoll(tempString);
    }
  }

  m_streamUrl = props.m_strURL;

  if (m_isArchiveStream)
    m_stream = std::make_shared<FFmpegArchiveStream>(static_cast<IManageDemuxPacket*>(this), m_playbackAsLive, m_catchupStartTime, m_catchupEndTime, m_timeshiftBufferStartTime, m_timeshiftBufferOffset);
  else
    m_stream = std::make_shared<FFmpegStream>(static_cast<IManageDemuxPacket*>(this));

  m_stream->SetVideoResolution(m_videoWidth, m_videoHeight);

  m_opened = m_stream->Open(m_streamUrl, m_mimeType, m_isRealTimeStream, m_programProperty);

  return m_opened;
}

void CInputStreamArchive::Close()
{
  m_opened = false;

  m_stream->Close();
}

void CInputStreamArchive::GetCapabilities(INPUTSTREAM_CAPABILITIES &caps)
{
  kodi::Log(ADDON_LOG_DEBUG, "GetCapabilities()");
  m_stream->GetCapabilities(caps);
}

INPUTSTREAM_IDS CInputStreamArchive::GetStreamIds()
{
  Log(LOGLEVEL_DEBUG, "GetStreamIds()");
  return m_stream->GetStreamIds();
}

INPUTSTREAM_INFO CInputStreamArchive::GetStream(int streamid)
{
  return m_stream->GetStream(streamid);
}

void CInputStreamArchive::EnableStream(int streamid, bool enable)
{
  m_stream->EnableStream(streamid, enable);
}

bool CInputStreamArchive::OpenStream(int streamid)
{
  return m_stream->OpenStream(streamid);
}

void CInputStreamArchive::DemuxReset()
{
  m_stream->DemuxReset();
}

void CInputStreamArchive::DemuxAbort()
{
  m_stream->DemuxAbort();
}

void CInputStreamArchive::DemuxFlush()
{
  m_stream->DemuxFlush();
}

DemuxPacket* CInputStreamArchive::DemuxRead()
{
  return m_stream->DemuxRead();
}

bool CInputStreamArchive::DemuxSeekTime(double time, bool backwards, double& startpts)
{
  return m_stream->DemuxSeekTime(time, backwards, startpts);
}

void CInputStreamArchive::DemuxSetSpeed(int speed)
{
  m_stream->DemuxSetSpeed(speed);
}

void CInputStreamArchive::SetVideoResolution(int width, int height)
{
    kodi::Log(ADDON_LOG_NOTICE, "InputStream.ffmpegarchive: SetVideoResolution()");

    m_videoWidth = width;
    m_videoHeight = height;

    //TODO, store and pass as arguments to open stream.
  //m_stream->SetVideoResolution(width, height);
}

int CInputStreamArchive::GetTotalTime()
{
  return m_stream->GetTotalTime();
}

int CInputStreamArchive::GetTime()
{
  return m_stream->GetTime();
}

bool CInputStreamArchive::GetTimes(INPUTSTREAM_TIMES& times)
{
  return m_stream->GetTimes(times);
}

bool CInputStreamArchive::PosTime(int ms)
{
  return m_stream->PosTime(ms);
}

int CInputStreamArchive::GetChapter()
{
  return m_stream->GetChapter();
}

int CInputStreamArchive::GetChapterCount()
{
  return m_stream->GetChapterCount();
}

const char* CInputStreamArchive::GetChapterName(int ch)
{
  return m_stream->GetChapterName(ch);
}

int64_t CInputStreamArchive::GetChapterPos(int ch)
{
  return m_stream->GetChapterPos(ch);
}

bool CInputStreamArchive::SeekChapter(int ch)
{
  return m_stream->SeekChapter(ch);
}

bool CInputStreamArchive::CanPauseStream()
{
  return m_stream->CanPauseStream();
}

bool CInputStreamArchive::CanSeekStream()
{
  return m_stream->CanSeekStream();
}

int CInputStreamArchive::ReadStream(uint8_t* buf, unsigned int size)
{
  return m_stream->ReadStream(buf, size);
}

int64_t CInputStreamArchive::SeekStream(int64_t position, int whence /* SEEK_SET */)
{
  return m_stream->SeekStream(position, whence);
}

int64_t CInputStreamArchive::PositionStream()
{
  return m_stream->PositionStream();
}

int64_t CInputStreamArchive::LengthStream()
{
  return m_stream->LengthStream();
}

void CInputStreamArchive::PauseStream(double time)
{
  return m_stream->PauseStream(time);
}

bool CInputStreamArchive::IsRealTimeStream()
{
  return m_stream->IsRealTimeStream();
}

/*****************************************************************************************************/

DemuxPacket* CInputStreamArchive::AllocateDemuxPacketFromInputStreamAPI(int dataSize)
{
  return AllocateDemuxPacket(dataSize);
}

DemuxPacket* CInputStreamArchive::AllocateEncryptedDemuxPacketFromInputStreamAPI(int dataSize, unsigned int encryptedSubsampleCount)
{
  return AllocateEncryptedDemuxPacket(dataSize, encryptedSubsampleCount);
}

void CInputStreamArchive::FreeDemuxPacketFromInputStreamAPI(DemuxPacket* packet)
{
  return FreeDemuxPacket(packet);
}

/*****************************************************************************************************/

class CMyAddon
  : public kodi::addon::CAddonBase
{
public:
  CMyAddon() { }
  virtual ADDON_STATUS CreateInstance(int instanceType, std::string instanceID, KODI_HANDLE instance, KODI_HANDLE& addonInstance) override
  {
    if (instanceType == ADDON_INSTANCE_INPUTSTREAM)
    {
      addonInstance = new CInputStreamArchive(instance);
      return ADDON_STATUS_OK;
    }
    return ADDON_STATUS_NOT_IMPLEMENTED;
  }
};

ADDONCREATOR(CMyAddon)
