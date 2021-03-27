/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2021  Mikko "Tapsa" P

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

#include "genie/resource/SmxFile.h"

#include <stdexcept>
#include <chrono>

#include "genie/resource/SmxFrame.h"
#include "genie/resource/PalFile.h"

namespace genie
{

Logger& SmxFile::log = Logger::getLogger("genie.SmxFile");

//------------------------------------------------------------------------------
SmxFile::SmxFile() : IFile()
{
}

//------------------------------------------------------------------------------
SmxFile::~SmxFile()
{
}

//------------------------------------------------------------------------------
void SmxFile::serializeObject(void)
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
void SmxFile::loadFile()
{
  serializeHeader();

  if (version < 3)
  {
    num_unique_frames = num_frames_;
  }
  frames_.resize(num_frames_);

  // Load frame headers and content
  for (uint16_t i = 0; i < num_frames_; ++i)
  {
    frames_[i] = SmxFramePtr(new SmxFrame());
    frames_[i]->load(*getIStream());
  }

  loaded_ = true;
}

//------------------------------------------------------------------------------
void SmxFile::saveFile()
{
}

//------------------------------------------------------------------------------
void SmxFile::unload(void)
{
  if (!loaded_)
    log.warn("Trying to unload a not loaded smxfile!");

  frames_.clear();
  num_frames_ = 0;

  loaded_ = false;
}

//------------------------------------------------------------------------------
bool SmxFile::isLoaded(void) const
{
  return loaded_;
}

//------------------------------------------------------------------------------
uint16_t SmxFile::getFrameCount(void)
{
  return static_cast<uint16_t>(frames_.size());
}

//------------------------------------------------------------------------------
void SmxFile::setFrameCount(uint16_t count)
{
  frames_.resize(count);
  num_frames_ = count;
}

//------------------------------------------------------------------------------
SmxFramePtr SmxFile::getFrame(uint16_t frame)
{
  if (frame >= frames_.size())
  {
    if (!loaded_)
    {
#ifndef NDEBUG
      log.debug("Reloading SMX, seeking frame [%u]", frame);
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
void SmxFile::setFrame(uint16_t frame, SmxFramePtr data)
{
  if (frame < frames_.size())
  {
    frames_[frame] = data;
  }
}

//------------------------------------------------------------------------------
void SmxFile::serializeHeader()
{
  serialize(signature, 4);
  serialize<uint16_t>(version);
  serializeSize<uint16_t>(num_frames_, frames_.size());
  serialize<uint32_t>(size);
  serialize<uint32_t>(original_size);
  serialize<uint32_t>(checksum);
  serialize<uint32_t>(odd_size);
  serialize<uint32_t>(odd_original_size);
  serialize<uint32_t>(odd_checksum);
}

}
