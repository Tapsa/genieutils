/*
    genie/dat - A library for reading and writing data files of genie
               engine games.
    Copyright (C) 2011 - 2013  Armin Preiml
    Copyright (C) 2011 - 2021  Mikko "Tapsa" P

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

#include "genie/dat/PlayerColour.h"
#include <string.h>

namespace genie
{

PlayerColour::PlayerColour()
{
}

PlayerColour::~PlayerColour()
{
}

//------------------------------------------------------------------------------
void PlayerColour::setGameVersion(GameVersion gv)
{
  ISerializable::setGameVersion(gv);
}

void PlayerColour::serializeObject(void)
{
  if (getGameVersion() < GV_AoKE3)
  {
    serialize(Name, NAME_SIZE);
    serialize<int16_t>(IdS16);
    serialize<int16_t>(ResourceID);
    serialize<uint8_t>(MinimapColorU8);
    serialize<uint8_t>(Type);

    ID = IdS16;
    MinimapColour = MinimapColorU8;
  }
  else
  {
    serialize<int32_t>(ID);
    serialize<int32_t>(PlayerColorBase);
    serialize<int32_t>(UnitOutlineColor);
    serialize<int32_t>(UnitSelectionColor1);
    serialize<int32_t>(UnitSelectionColor2);
    serialize<int32_t>(MinimapColour);
    serialize<int32_t>(MinimapColor2);
    serialize<int32_t>(MinimapColor3);
    serialize<int32_t>(StatisticsText);
  }
}

}
