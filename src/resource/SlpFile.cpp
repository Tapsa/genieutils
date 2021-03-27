/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Armin Preiml
    Copyright (C) 2015 - 2016  Mikko "Tapsa" P

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "genie/resource/SlpFile.h"

#include <stdexcept>
#include <chrono>

#include "genie/resource/SlpFrame.h"
#include "genie/resource/PalFile.h"

#include "lz4hc.h"

namespace genie
{

Logger& SlpFile::log = Logger::getLogger("genie.SlpFile");

//------------------------------------------------------------------------------
SlpFile::SlpFile() : IFile()
{
}

//------------------------------------------------------------------------------
SlpFile::~SlpFile()
{
  delete slp_stream_;
  delete[] slp_data_;
}

//------------------------------------------------------------------------------
void SlpFile::serializeObject(void)
{
  if (isOperation(OP_READ) && !loaded_)
  {
    loadFile();
  }
  else if (isOperation(OP_WRITE))// && loaded_)
  {
    saveFile();
  }
}

//------------------------------------------------------------------------------
void SlpFile::loadFile()
{
  serializeHeader();

  frames_.resize(num_frames_);

  // Load frame headers
  for (uint32_t i = 0; i < num_frames_; ++i)
  {
    frames_[i] = SlpFramePtr(new SlpFrame());
    frames_[i]->setLoadParams(*getIStream());
    frames_[i]->serializeHeader();
    frames_[i]->setSlpFilePos(getInitialReadPosition());
  }

  // Load frame content
  for (uint32_t i = 0; i < num_frames_; ++i)
  {
    frames_[i]->load(*getIStream());
  }

  // In case file content was decompressed
  delete slp_stream_;
  delete[] slp_data_;
  slp_stream_ = nullptr;
  slp_data_ = nullptr;
  loaded_ = true;
}

//------------------------------------------------------------------------------
void SlpFile::saveFile()
{
#ifndef NDEBUG
  std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();
#endif
  serializeHeader();
  slp_offset_ = 32 + 32 * num_frames_;

  // Write frame headers
  for (uint32_t i = 0; i < num_frames_; ++i)
  {
    frames_[i]->setSaveParams(*getOStream(), slp_offset_);
    frames_[i]->serializeHeader();
  }

  // Write frame content
  for (uint32_t i = 0; i < num_frames_; ++i)
  {
    frames_[i]->save(*getOStream());
  }
#ifndef NDEBUG
  std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
  log.debug("SLP (%u bytes) saving took [%u] milliseconds", slp_offset_, std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
#endif
}

//------------------------------------------------------------------------------
void SlpFile::unload(void)
{
  if (!loaded_)
    log.warn("Trying to unload a not loaded slpfile!");

  frames_.clear();
  num_frames_ = 0;

  loaded_ = false;
}

//------------------------------------------------------------------------------
bool SlpFile::isLoaded(void) const
{
  return loaded_;
}

//------------------------------------------------------------------------------
uint32_t SlpFile::getFrameCount(void)
{
  return frames_.size();
}

//------------------------------------------------------------------------------
void SlpFile::setFrameCount(uint32_t count)
{
  frames_.resize(count);
  num_frames_ = count;
}

//------------------------------------------------------------------------------
SlpFramePtr SlpFile::getFrame(uint32_t frame)
{
  if (frame >= frames_.size())
  {
    if (!loaded_)
    {
#ifndef NDEBUG
      log.debug("Reloading SLP, seeking frame [%u]", frame);
#endif
      readObject(*getIStream());
      return getFrame(frame);
    }
    log.error("Trying to get frame [%u] from index out of range!", frame);
    throw std::out_of_range("getFrame()");
  }

  return frames_[frame];
}

//------------------------------------------------------------------------------
void SlpFile::setFrame(uint32_t frame, SlpFramePtr data)
{
  if (frame < frames_.size())
  {
    frames_[frame] = data;
  }
}

//------------------------------------------------------------------------------
void SlpFile::serializeHeader()
{
  serialize(version, 4);
  if (version[3] != 'P')//4.2P
  {
    serializeSize<uint32_t>(num_frames_, frames_.size());
    serialize(comment, 24);

    // Avoid crash
    if (num_frames_ > 4000)
    {
      num_frames_ = 0;
    }
  }
  else
  {
    // Decompress rest of the file
    int32_t original_size = read<int32_t>();
    int32_t unpack_count;
    slp_data_ = new char[original_size];
    // There's probably better way to do this.
    {
      std::vector<uint8_t> input(std::istreambuf_iterator<char>(*getIStream()), {});
      int32_t size = input.size();
      unpack_count = LZ4_decompress_safe(reinterpret_cast<const char*>(input.data()),
        slp_data_, size, original_size);
    }
    // Set the stream
    if (unpack_count == original_size)
    {
      slp_stream_ = new IMemoryStream(slp_data_, slp_data_ + original_size);
      setIStream(*slp_stream_);
      serializeHeader();
    }
    else
    {
      num_frames_ = 0;
      delete[] slp_data_;
      slp_data_ = nullptr;
    }
  }
}

}
