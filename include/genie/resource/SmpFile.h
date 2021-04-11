/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2018 - 2021  Mikko "Tapsa" P

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

#ifndef GENIE_SMPFILE_H
#define GENIE_SMPFILE_H

#include <istream>
#include <vector>

#include "genie/file/IFile.h"
#include "genie/util/Logger.h"
#include "PalFile.h"
#include "SmpFrame.h"
#include "SpriteFile.h"

namespace genie
{

//------------------------------------------------------------------------------
/// This format is superseded by smx.
/// A smp file stores one or several images encoded using simple commands.
/// The image is stored as 10 bits per pixel, that means only the index of a
/// color in a palette is saved.
//
class SmpFile : public SpriteFile, public IFile
{

public:
  //----------------------------------------------------------------------------
  /// Constructor
  //
  SmpFile();

  //----------------------------------------------------------------------------
  /// Destructor
  //
  virtual ~SmpFile();

  inline bool isSMP(void) const override { return true; }

  //----------------------------------------------------------------------------
  /// Loads contents of a sprite file and then unlocks the file for others.
  //
  void loadAndRelease(const char *fileName) override;

  //----------------------------------------------------------------------------
  /// Frees all content of a smp file.
  //
  void unload(void) override;

  //----------------------------------------------------------------------------
  /// Check whether the file's content is loaded or not.
  //
  inline bool isLoaded(void) const { return loaded_; }

  //----------------------------------------------------------------------------
  /// Return number of frames stored in the file. Available after load.
  ///
  /// @return number of frames
  //
  uint16_t getFrameCount(void) const override;
  void setFrameCount(uint16_t);

  //----------------------------------------------------------------------------
  /// Returns the smp frame at given frame index.
  ///
  /// @param frame frame index
  /// @return SmpFrame
  //
  SmpFramePtr getFrame(uint16_t frame = 0);
  void setFrame(uint16_t, SmpFramePtr);

  std::string signature;
  std::string comment;
  uint32_t version;
  uint32_t num_animations;
  uint32_t num_frames_per_animation;
  uint32_t checksum;
  uint32_t size;
  uint32_t resource_id;

private:
  static Logger &log;

  bool loaded_ = false;

  uint32_t num_frames_ = 0;

  typedef std::vector<SmpFramePtr> FrameVector;
  FrameVector frames_;

  // Used to calculate offsets when saving the SMP.
  uint32_t smp_offset_;

  //----------------------------------------------------------------------------
  virtual void serializeObject(void);

  //----------------------------------------------------------------------------
  /// Loads the file and its frames.
  //
  void loadFile(void);

  //----------------------------------------------------------------------------
  void serializeHeader(void);
};

typedef std::shared_ptr<SmpFile> SmpFilePtr;

}

#endif // GENIE_SMPFILE_H
