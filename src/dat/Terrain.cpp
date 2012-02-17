/*
    genie/dat - A library for reading and writing data files of genie
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


#include "genie/dat/Terrain.h"

namespace genie
{

//------------------------------------------------------------------------------
Terrain::Terrain() : Unknown7(UNKNOWN7_LEN), Unknown8(UNKNOWN8_LEN), 
                     Unknown9(UNKNOWN9_LEN), 
                     Unknown11(UNKNOWN11_LEN),
                     SWGBUnknown1(SWGBUNKNOWN1_LEN)
{
  Unknown1 = 0;
  Unknown2 = 1;
  Name = "";
  Name2 = "";
  SLP = -1;
  Unknown3 = -1;
  SoundID = -1;
  BlendPriority = -1;
  BlendType = -1;
  Unknown5 = 0;
  Unknown6 = 0;
  FrameCount = 0;
  TerrainReplacementID = 0;
  NumberOfTerrainUnitsUsed = 0;
  TerrainDimensions.first = TerrainDimensions.second = -1;
}

//------------------------------------------------------------------------------
Terrain::~Terrain()
{
}

//------------------------------------------------------------------------------
short Terrain::getNameSize()
{
  if (getGameVersion() >= genie::GV_SWGB)
    return 17;
  else 
    return 13;
}

//------------------------------------------------------------------------------
short Terrain::getColorSize()
{
  return 3;
}

//------------------------------------------------------------------------------
short Terrain::getTerrainUnitSize()
{
  return 30;
}

//------------------------------------------------------------------------------
short Terrain::getUnknown10Size(void)
{
  if (getGameVersion() >= genie::GV_TC)
    return 84;
  else
    return 64;
}
  
//------------------------------------------------------------------------------
void Terrain::serializeObject(void )
{ 
  serialize<short>(Unknown1);
  serialize<short>(Unknown2);
  
  serialize<std::string>(Name, getNameSize());
  serialize<std::string>(Name2, getNameSize());
  
  serialize<long>(SLP);
  serialize<long>(Unknown3);
  serialize<long>(SoundID);
  
  if (getGameVersion() >= genie::GV_AoK)
  {
    serialize<long>(BlendPriority);
    serialize<long>(BlendType);
  }
  
  serialize<char>(Colours, getColorSize());
  serialize<short>(Unknown5);
  serialize<short>(Unknown6);
  serialize<char>(Unknown7, UNKNOWN7_LEN);
  serialize<short>(FrameCount);
  serialize<short>(Unknown8, UNKNOWN8_LEN);
  serialize<short>(Unknown9, UNKNOWN9_LEN);
  serialize<short>(TerrainReplacementID);
  serialize<short>(TerrainDimensions);
  
  serialize<char>(Unknown10, getUnknown10Size());
  
  serialize<short>(TerrainUnitID, getTerrainUnitSize());
  serialize<short>(TerrainUnitDensity, getTerrainUnitSize());
  serialize<char>(Unknown11, UNKNOWN11_LEN);
  serialize<short>(NumberOfTerrainUnitsUsed);
  
  if (getGameVersion() >= genie::GV_SWGB)
    serialize<char>(SWGBUnknown1, SWGBUNKNOWN1_LEN);
}

}
