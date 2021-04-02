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

#ifndef GENIE_SPRITEFILE_H
#define GENIE_SPRITEFILE_H

namespace genie
{

class SpriteFile
{
public:
  //----------------------------------------------------------------------------
  /// Loads contents of a sprite file and then unlocks the file for others.
  //
  virtual void loadAndRelease(const char *fileName) = 0;

  //----------------------------------------------------------------------------
  /// Frees all content of a sprite file.
  //
  virtual void unload(void) = 0;

  //----------------------------------------------------------------------------
  /// Return number of frames stored in the file. Available after load.
  ///
  /// @return number of frames
  //
  virtual uint32_t getFrameCount(void) = 0;

  //----------------------------------------------------------------------------
  /// Check if the sprite is of SLP format.
  ///
  /// @return true if the sprite is of SLP format.
  //
  virtual bool isSLP(void) { return false; }

  //----------------------------------------------------------------------------
  /// Check if the sprite is of SLP format.
  ///
  /// @return true if the sprite is of SLP format.
  //
  virtual bool isSMP(void) { return false; }

  //----------------------------------------------------------------------------
  /// Check if the sprite is of SLP format.
  ///
  /// @return true if the sprite is of SLP format.
  //
  virtual bool isSMX(void) { return false; }
};

typedef std::shared_ptr<SpriteFile> SpriteFilePtr;

}

#endif // GENIE_SPRITEFILE_H
