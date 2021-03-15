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

#include "genie/dat/Graphic.h"
#include <string.h>

#include <iostream>

namespace genie
{

Graphic::Graphic()
{
}

Graphic::~Graphic()
{
}

//------------------------------------------------------------------------------
void Graphic::setGameVersion(GameVersion gv)
{
  ISerializable::setGameVersion(gv);

  updateGameVersion(Deltas);
  updateGameVersion(AngleSounds);
}

unsigned short Graphic::getNameSize()
{
  if (getGameVersion() < GV_SWGB)
    return 21;
  else
    return 25;
}

unsigned short Graphic::getName2Size()
{
  if (getGameVersion() < GV_SWGB)
    return 13;
  else
    return 25;
}

void Graphic::serializeObject(void)
{
  GameVersion gv = getGameVersion();

  if (gv > GV_LatestTap && gv < GV_C2 || gv < GV_Tapsa || gv > GV_LatestDE2)
  {
    serialize(Name, getNameSize());
    serialize(FileName, getName2Size());
  }
  else
  {
    serializeDebugString(Name);
    serializeDebugString(FileName);
    if (gv >= GV_C5 && gv <= GV_LatestDE2)
    {
      serializeDebugString(ParticleEffectName);
    }
    if (gv >= GV_T3 && gv < GV_C2)
    {
      serialize<int16_t>(FirstFrame);
    }
  }

  if (gv < GV_TEST)
  {
    serialize<int16_t>(ResourceIdS16);
    SLP = ResourceIdS16;
  }
  else
  {
    serialize<int32_t>(SLP);
  }
  serialize<uint8_t>(IsLoaded); // Unused
  serialize<uint8_t>(OldColorFlag); // Unused
  serialize<uint8_t>(Layer);
  serialize<int16_t>(PlayerColor);
  serialize<uint8_t>(TransparentSelection);

  serialize<int16_t>(Coordinates, 4);

  int16_t delta_count;
  serializeSize<int16_t>(delta_count, Deltas.size());
  serialize<int16_t>(SoundID);
  if (gv >= GV_C4 && gv <= GV_LatestDE2)
  {
    serialize<uint32_t>(WwiseSoundID);
  }
  serialize<uint8_t>(AngleSoundsUsed);
  serialize<int16_t>(FrameCount);
  serialize<int16_t>(AngleCount);
  serialize<float>(SpeedMultiplier);
  FrameDuration = FrameCount ? AnimationDuration / FrameCount : 0;
  serialize<float>(FrameDuration);
  AnimationDuration = FrameDuration * FrameCount;
  serialize<float>(ReplayDelay);
  serialize<uint8_t>(SequenceType);
  serialize<int16_t>(ID);
  serialize<uint8_t>(MirroringMode);

  if (gv >= GV_AoKB) // 10.72
    serialize<uint8_t>(EditorFlag); // A sprite editor thing

  serializeSub<GraphicDelta>(Deltas, delta_count);

  if (AngleSoundsUsed != 0)
  {
    if (isOperation(OP_WRITE) && AngleSounds.size() != AngleCount)
    {
      std::cerr << "Warning: Size mismatch between angle sounds and angles!"
                << std::endl;
      //Be naughty and force the size to be correct.
      AngleSounds.resize(AngleCount, AngleSounds.front());
    }

    serializeSub<GraphicAngleSound>(AngleSounds, AngleCount);
  }

}

}
