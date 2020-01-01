/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "FFmpegStream.h"

class FFmpegArchiveStream : public FFmpegStream
{
public:
  FFmpegArchiveStream(IManageDemuxPacket* demuxPacketManager);
  ~FFmpegArchiveStream();

  virtual bool Open(const std::string& streamUrl, const std::string& mimeType, bool isRealTimeStream, const std::string& programProperty) override;
  virtual bool DemuxSeekTime(double time, bool backwards, double& startpts) override;
  virtual DemuxPacket* DemuxRead() override;
  virtual void GetCapabilities(INPUTSTREAM_CAPABILITIES& caps) override;

  virtual int64_t SeekStream(int64_t position, int whence = SEEK_SET) override;
  virtual int64_t LengthStream() override;
  virtual bool GetTimes(INPUTSTREAM_TIMES& times) override;

  virtual bool CanPauseStream() override;
  virtual bool CanSeekStream() override;

protected:
  void UpdateCurrentPTS() override;

  time_t m_catchupStartTime = 0;
  time_t m_catchupEndTime = 0;
  time_t m_timeshiftBufferStartTime = 0;
  long long m_timeshiftBufferOffset = 0;
  bool m_resetCatchupState = false;
  bool m_playbackIsVideo = false;
  bool m_fromEpgTag = false;

  bool m_bIsOpening;
  double m_seekOffset;
};