/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef GENIE_PALFILE_H
#define GENIE_PALFILE_H

#include <string>
#include <vector>
#include <stdint.h>
#include <boost/shared_ptr.hpp>

#include <SFML/Graphics/Color.hpp>

#include "genie/util/Logger.h"
#include "genie/file/IFile.h"

namespace genie
{

//------------------------------------------------------------------------------
/// Class for parsing aoe color palettes.
//
class PalFile : public IFile
{

public:
  //----------------------------------------------------------------------------
  /// Constructor
  //
  PalFile();
  
  //----------------------------------------------------------------------------
  /// Destructor
  //
  virtual ~PalFile();
   
  //----------------------------------------------------------------------------
  /// Returns a color at given index.
  ///
  /// @param index index of color in palette
  /// @return color object
  //
  sf::Color& operator[](uint16_t index);
  
  //----------------------------------------------------------------------------
  ///
  //
  size_t size(void) const;
  
  //----------------------------------------------------------------------------
  /// TODO: Somethings wrong...
  //
  virtual size_t objectSize(void); 
private:
  
  static Logger &log;
  
  std::vector<sf::Color> colors_;
  
  uint32_t num_colors_;
  
  std::string type_;
  std::string unknown_;
  
  std::string getHeader() const; 
  std::string getHeader2() const; //TODO Unknown
  
  virtual void serializeObject(void);
  
  // TODO: Not implemented yet
  
  //----------------------------------------------------------------------------
  /// Get number of characters of a number serialized as a string.
  //
  size_t numOfChars(uint8_t number);
};

typedef boost::shared_ptr<PalFile> PalFilePtr;

}

#endif // GENIE_PALFILE_H
