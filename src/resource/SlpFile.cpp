/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Armin Preiml
    Copyright (C) 2015 - 2021  Mikko "Tapsa" P

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
  version = readString(4);
  if (version[3] != 'P')//4.2P
  {
    num_frames_ = read<uint32_t>();
    comment = readString(24);

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
    std::vector<uint8_t> slp_data(original_size);
    char *slp_data_ptr = reinterpret_cast<char *>(slp_data.data());
    // There's probably better way to do this.
    {
      std::vector<uint8_t> input(std::istreambuf_iterator<char>(*getIStream()), {});
      int32_t size = input.size();
      unpack_count = LZ4_decompress_safe(reinterpret_cast<const char *>(input.data()),
        slp_data_ptr, size, original_size);
    }
    // Set the stream
    if (unpack_count == original_size)
    {
      IMemoryStream slp_stream(slp_data_ptr, slp_data_ptr + original_size);
      setIStream(slp_stream);
      return loadFile();
    }
    else
    {
      num_frames_ = 0;
    }
  }

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
    frames_[i]->load();
  }

  loaded_ = true;
}

//------------------------------------------------------------------------------
void SlpFile::saveFile()
{
#ifndef NDEBUG
  std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();
#endif
  writeString(version, 4);
  serializeSize<uint32_t>(num_frames_, frames_.size());
  writeString(comment, 24);

  uint32_t slp_offset = 32 + 32 * num_frames_;

  std::vector<SlpSaveData> save_data(num_frames_);

  // Write frame headers
  for (uint32_t i = 0; i < num_frames_; ++i)
  {
    frames_[i]->buildSaveData(*getOStream(), slp_offset, save_data[i]);
    frames_[i]->serializeHeader();
  }

  // Write frame content
  for (uint32_t i = 0; i < num_frames_; ++i)
  {
    frames_[i]->save(save_data[i]);
  }
#ifndef NDEBUG
  std::chrono::time_point<std::chrono::system_clock> endTime = std::chrono::system_clock::now();
  log.debug("SLP (%u bytes) saving took [%u] milliseconds", slp_offset, std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count());
#endif
}

//------------------------------------------------------------------------------
void SlpFile::loadAndRelease(const char *fileName)
{
  load(fileName);
  freelock();
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

}
