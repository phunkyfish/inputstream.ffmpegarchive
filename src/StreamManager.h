/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "stream/FFmpegArchiveStream.h"
#include "stream/FFmpegStream.h"
#include "stream/IManageDemuxPacket.h"

#include <memory>
#include <string>

#include <kodi/addon-instance/Inputstream.h>

static const std::string MIMETYPE = "inputstream.ffmpegarchive.mimetype";
static const std::string PROGRAM_PROP = "inputstream.ffmpegarchive.property";
static const std::string ISREALTIMESTREAM = "inputstream.ffmpegarchive.isrealtimestream";

class CInputStreamArchive
  : public kodi::addon::CInstanceInputStream, IManageDemuxPacket
{
public:
  CInputStreamArchive(KODI_HANDLE instance);
  ~CInputStreamArchive();

  virtual bool Open(INPUTSTREAM& props) override;
  virtual void Close() override;
  virtual void GetCapabilities(INPUTSTREAM_CAPABILITIES& caps) override;
  virtual INPUTSTREAM_IDS GetStreamIds() override;
  virtual INPUTSTREAM_INFO GetStream(int streamid) override;
  virtual void EnableStream(int streamid, bool enable) override;
  virtual bool OpenStream(int streamid) override;

  virtual void DemuxReset() override;
  virtual void DemuxAbort() override;
  virtual void DemuxFlush() override;
  virtual DemuxPacket* DemuxRead() override;
  virtual bool DemuxSeekTime(double time, bool backwards, double& startpts) override;
  virtual void DemuxSetSpeed(int speed) override;
  virtual void SetVideoResolution(int width, int height) override;

  virtual int GetTotalTime() override;
  virtual int GetTime() override;
  virtual bool GetTimes(INPUTSTREAM_TIMES& times) override;
  virtual bool PosTime(int ms) override;

  virtual int GetChapter() override;
  virtual int GetChapterCount() override;
  virtual const char* GetChapterName(int ch) override;
  virtual int64_t GetChapterPos(int ch) override;
  virtual bool SeekChapter(int ch) override;

  virtual bool CanPauseStream() override;
  virtual bool CanSeekStream() override;
  virtual int ReadStream(uint8_t* buffer, unsigned int bufferSize) override;
  virtual int64_t SeekStream(int64_t position, int whence = SEEK_SET) override;
  virtual int64_t PositionStream() override;
  virtual int64_t LengthStream() override;
  virtual void PauseStream(double time) override;
  virtual bool IsRealTimeStream() override; // { return true; }

  DemuxPacket* AllocateDemuxPacketFromInputStreamAPI(int dataSize) override;
  DemuxPacket* AllocateEncryptedDemuxPacketFromInputStreamAPI(int dataSize, unsigned int encryptedSubsampleCount) override;
  void FreeDemuxPacketFromInputStreamAPI(DemuxPacket* packet) override;

protected:

private:
  std::string m_streamUrl;
  std::string m_mimeType;
  std::string m_programProperty;
  bool m_isRealTimeStream;
  bool m_opened;
  bool m_isArchiveStream = false;

  int m_videoWidth;
  int m_videoHeight;

  std::shared_ptr<BaseStream> m_stream;
};