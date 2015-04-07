/*
    genie/dat - A library for reading and writing data files of genie
               engine games.
    Copyright (C) 2015  Mikko T P

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

#include "genie/dat/TerrainBlock.h"

namespace genie
{

//------------------------------------------------------------------------------
TerrainBlock::TerrainBlock()
{
  TerrainsUsed2 = 0;
  RemovedBlocksUsed = 0;
  TerrainBordersUsed = 0;
}

//------------------------------------------------------------------------------
TerrainBlock::~TerrainBlock()
{
}

//------------------------------------------------------------------------------
void TerrainBlock::setGameVersion(GameVersion gv)
{
  ISerializable::setGameVersion(gv);

  updateGameVersion(Terrains);
  updateGameVersion(TerrainBorders);

  GraphicsRendering.resize(getTerrainHeaderSize());
  //AoEAlphaUnknown.resize(0);
  ZeroSpace.resize(getZeroSpaceSize());
  CivData.resize(getCivDataSize());
  SomeBytes.resize(getBytesSize());
  SomeInt32.resize(getSomethingSize());
}

//------------------------------------------------------------------------------
unsigned short TerrainBlock::getTerrainHeaderSize(void)
{
  if (getGameVersion() >= genie::GV_AoE)
    return 70;
  return 50;
}

//------------------------------------------------------------------------------
unsigned short TerrainBlock::getZeroSpaceSize(void)
{
  if (getGameVersion() >= genie::GV_AoK)
    return 14;
  if (getGameVersion() >= genie::GV_AoKA)
    return 10;
  return 2;
}

//------------------------------------------------------------------------------
unsigned short TerrainBlock::getCivDataSize(void)
{
  if (getGameVersion() >= genie::GV_AoKA)
    return 17;
  if (getGameVersion() >= genie::GV_AoEB)
    return 18; // 17.5
  return 12; // 12.5
}

//------------------------------------------------------------------------------
unsigned short TerrainBlock::getBytesSize(void)
{
  if (getGameVersion() >= genie::GV_SWGB)
    return 26;
  if (getGameVersion() >= genie::GV_AoKA)
    return 22;
  if (getGameVersion() >= genie::GV_AoEB)
    return 4;
  return 2;
}

//------------------------------------------------------------------------------
unsigned short TerrainBlock::getSomethingSize(void)
{
  if (getGameVersion() >= genie::GV_AoK)
    return 157;
  if (getGameVersion() >= genie::GV_AoKB)
    return 84;
  if (getGameVersion() >= genie::GV_AoKA)
    return 6;
  if (getGameVersion() >= genie::GV_AoEB)
    return 5;
  return 68;//return 2625; // Temporary skip for random maps
}

//------------------------------------------------------------------------------
/// Size in bytes
/// AoE Trial Beta	36264
/// AoE & RoR		36304
/// AoK A & B		36340
/// AoK				37208
/// TC				42208
/// SWGB & CC		49640
void TerrainBlock::serializeObject(void)
{
  serialize<int16_t>(GraphicsRendering, getTerrainHeaderSize());

  if (isOperation(OP_READ))
    serializeSub<Terrain>(Terrains, Terrain::getTerrainsSize(getGameVersion()));
  else
    serializeSub<Terrain>(Terrains, Terrains.size());

  if (getGameVersion() < genie::GV_AoEB)
    serialize<int16_t>(AoEAlphaUnknown, (16 * 1888) / 2);
  // TerrainBorders seem to be unused (are empty) in GV > AoK Alpha
  serializeSub<TerrainBorder>(TerrainBorders, 16); //TODO: fixed size?

  // Empty space.
  serialize<int16_t>(ZeroSpace, getZeroSpaceSize());

  serialize<uint16_t>(TerrainsUsed2);
  if (getGameVersion() < genie::GV_AoEB)
    serialize<uint16_t>(RemovedBlocksUsed);
  serialize<uint16_t>(TerrainBordersUsed);

  serialize<int16_t>(CivData, getCivDataSize());
  serialize<int8_t>(SomeBytes, getBytesSize());

  // Few pointers and small numbers.
  serialize<int32_t>(SomeInt32, getSomethingSize());
  float hack;
  if (getGameVersion() < genie::GV_AoEB)
  for (int i=0; i < 10228 / 4; i++)
  serialize<float>(hack);
}

}
