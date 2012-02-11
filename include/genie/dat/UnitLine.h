/*
    geniedat - A library for reading and writing data files of genie
               engine games.
    Copyright (C) 2011  Armin Preiml <email>

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


#ifndef GENIE_UNITLINE_H
#define GENIE_UNITLINE_H
#include "genie/file/ISerializable.h"

#include <vector>

namespace genie
{

class UnitLine : public ISerializable
{

public:
  UnitLine();
  virtual ~UnitLine();
  
  short ID;
  
  std::string Name;
  
  std::vector<short> UnitIDs;
  
private:
  
  unsigned short name_len_;
  
  unsigned short unit_count_;
  
  virtual void serializeObject(void);
  
};

}

#endif // GENIE_UNITLINE_H
