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

#ifndef GENIE_SMXFILE_H
#define GENIE_SMXFILE_H

#include <istream>
#include <vector>

#include "genie/file/IFile.h"
#include "genie/util/Logger.h"
#include "PalFile.h"
#include "SmxFrame.h"
#include "SpriteFile.h"

namespace genie
{

//------------------------------------------------------------------------------
/// A smx file stores one or several images encoded using simple commands.
/// The image is stored as 10 bits per pixel, that means only the index of a
/// color in a (1024 color) palette is saved.
//
class SmxFile : public SpriteFile, public IFile
{

public:
  //----------------------------------------------------------------------------
  /// Constructor
  //
  SmxFile();

  //----------------------------------------------------------------------------
  /// Destructor
  //
  virtual ~SmxFile();

  inline bool isSMX(void) const override { return true; }

  //----------------------------------------------------------------------------
  /// Loads contents of a sprite file and then unlocks the file for others.
  //
  void loadAndRelease(const char *fileName) override;

  //----------------------------------------------------------------------------
  /// Frees all content of a smx file.
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
  /// Returns the smx frame at given frame index.
  ///
  /// @param frame frame index
  /// @return SmxFrame
  //
  SmxFramePtr getFrame(uint16_t frame = 0);
  void setFrame(uint16_t, SmxFramePtr);

  std::string signature;
  uint16_t version;
  uint32_t size;
  uint32_t original_size;
  uint32_t checksum;

  union
  {
    struct
    {
      uint32_t odd_size;
      uint32_t odd_original_size;
      uint32_t odd_checksum;
    };
    struct
    {
      uint16_t num_unique_frames;
    };
  };

private:
  static Logger &log;

  bool loaded_ = false;

  uint16_t num_frames_ = 0;

  typedef std::vector<SmxFramePtr> FrameVector;
  FrameVector frames_;

  //----------------------------------------------------------------------------
  virtual void serializeObject(void);

  //----------------------------------------------------------------------------
  /// Loads the file and its frames.
  //
  void loadFile(void);
  void saveFile(void);

  //----------------------------------------------------------------------------
  void serializeHeader(void);
};

typedef std::shared_ptr<SmxFile> SmxFilePtr;

}

#endif // GENIE_SMXFILE_H
