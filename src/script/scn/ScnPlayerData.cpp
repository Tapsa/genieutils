/*
    genieutils - <description>
    Copyright (C) 2011 - 2013  Armin Preiml <email>
    Copyright (C) 2015  Mikko "Tapsa" P <email>

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

#include "genie/script/scn/ScnPlayerData.h"
#include "genie/script/ScnFile.h"

namespace genie
{

bool CombinedResources::playerInfo = false;

ScnPlayerData1::ScnPlayerData1()
{
  playerNames.resize(16);
}

ScnPlayerData1::~ScnPlayerData1()
{
}

void ScnPlayerData1::serializeObject(void)
{
  serializePlayerDataVersion();
  if (getGameVersion() >= genie::GV_AoK)
  {
    for (unsigned int i=0; i<16; ++i)
      serialize<std::string>(playerNames[i], 256); // 1.14 <-- this is read much later in AoE 1
    serialize<uint32_t>(playerNamesStringTable, 16); // 1.16
    CombinedResources::playerInfo = true;
    serializeSub<CombinedResources>(resourcesPlusPlayerInfo, 16); // 1.14
  }
  if (getGameVersion() >= genie::GV_AoE) // 1.07
    serialize<uint8_t>(conquestVictory);
  serialize<ISerializable>(unknownData);
  serializeSizedString<uint16_t>(originalFileName);

  // Messages and cinematics
  serialize<ISerializable>(messagesCinematics); // should be 100 % done for 1.13+

  serializeSizedStrings<uint16_t>(aiNames, 16);
  serializeSizedStrings<uint16_t>(cityNames, 16);
  if (getGameVersion() >= genie::GV_AoE) // 1.08
    serializeSizedStrings<uint16_t>(personalityNames, 16);
  serializeSub(aiFiles, 16);
  if (getGameVersion() >= genie::GV_TC) // 1.2
    serialize<uint8_t>(aiTypes, 16);
  serialize<uint32_t>(separator_); // > 1.02

  if (getGameVersion() < genie::GV_AoK) // 1.14
  {
    for (unsigned int i=0; i<16; ++i)
      serialize<std::string>(playerNames[i], 256);
    serializeSub<CombinedResources>(resourcesPlusPlayerInfo, 16);
  }
  else
  {
    CombinedResources::playerInfo = false;
    serializeSub<CombinedResources>(resourcesPlusPlayerInfo, 16);
  }
  serialize<uint32_t>(separator_); // > 1.02
  serialize<ISerializable>(victoryConditions);
  serialize<ISerializable>(diplomacy);
  serialize<uint32_t>(separator_); // > 1.02
  serialize<uint32_t>(alliedVictory, 16); // 16*16*4 in 1.01
  if (getGameVersion() >= genie::GV_AoE) // 1.04
  {
    serialize<ISerializable>(disables);
    if (getGameVersion() >= genie::GV_AoE) // 1.05
    {
      serialize<uint32_t>(unused1);
      if (getGameVersion() >= genie::GV_AoK) // 1.12
      {
        serialize<uint32_t>(unused2);
        serialize<uint32_t>(allTechs);
      }
      if (getGameVersion() >= genie::GV_AoE) // 1.06
        serialize<uint32_t>(startingAge, 16);
    }
  }
  serialize<uint32_t>(separator_); // > 1.02
}

CombinedResources::CombinedResources()
{
}

CombinedResources::~CombinedResources()
{
}

void CombinedResources::serializeObject(void)
{
  if (playerInfo || getGameVersion() < genie::GV_AoK) // 1.14
    serialize<uint32_t>(state);
  if (!playerInfo || getGameVersion() < genie::GV_AoK) // 1.14
  {
    serialize<uint32_t>(gold);
    serialize<uint32_t>(wood);
    serialize<uint32_t>(food);
    serialize<uint32_t>(stone);
  }
  if (playerInfo || getGameVersion() < genie::GV_AoK) // 1.14
  {
    serialize<uint32_t>(type);
    serialize<uint32_t>(civilizationID);
    serialize<uint32_t>(unknown1);
  }
  if (!playerInfo && getGameVersion() >= genie::GV_AoK) // 1.17
  {
    serialize<uint32_t>(ore);
    serialize<uint32_t>(goods);
  }
}

UnknownData1::UnknownData1()
{
}

UnknownData1::~UnknownData1()
{
}

void UnknownData1::serializeObject(void)
{
  serialize<uint16_t>(unknownCount);
  serialize<uint16_t>(unknown2);
  serialize<float>(unknown3);

  /*/ 48 bytes? Lots of data if count is over 0
  ReadData((HANDLE)_hScenFile, hUnknown, 4u);
  ReadData((HANDLE)_hScenFile, (char *)hUnknown + 4, 1u);
  ReadData((HANDLE)_hScenFile, (char *)hUnknown + 6, 2u);
  ReadData((HANDLE)_hScenFile, (char *)hUnknown + 8, 1u);
  ReadData((HANDLE)_hScenFile, (char *)hUnknown + 12, 4u);
  ReadData((HANDLE)_hScenFile, (char *)hUnknown + 16, 4u);
  ReadData((HANDLE)_hScenFile, (char *)hUnknown + 20, 4u);
  ReadData((HANDLE)_hScenFile, (char *)hUnknown + 24, 2u);
  ReadData((HANDLE)_hScenFile, (char *)hUnknown + 32, 2u);
  ReadData((HANDLE)_hScenFile, &hScenFile, 2u);
  ReadData((HANDLE)_hScenFile, (char *)hUnknown + 40, 2u);
  ReadData((HANDLE)_hScenFile, &v15, 2u);*/
}

AiFile::AiFile()
{
}

AiFile::~AiFile()
{
}

void AiFile::serializeObject(void)
{
  serializeSize<uint32_t>(aiFilenameSize, aiFilename, true);
  serializeSize<uint32_t>(cityFileSize, cityFilename, true);
  if (getGameVersion() >= genie::GV_AoE) // 1.08
    serializeSize<uint32_t>(perFileSize, perFilename, true);

  // crap in exe, says these are >= 1.15
  serialize<std::string>(aiFilename, aiFilenameSize);
  serialize<std::string>(cityFilename, cityFileSize);
  if (getGameVersion() >= genie::GV_AoE) // 1.08
    serialize<std::string>(perFilename, perFileSize);
}

ScnVictory::ScnVictory()
{
}

ScnVictory::~ScnVictory()
{
}

void ScnVictory::serializeObject(void)
{
  {
    serialize<uint32_t>(conquestRequired);
    serialize<uint32_t>(unused1);
    serialize<uint32_t>(numRelicsRequired);
    serialize<uint32_t>(unused2);
    serialize<uint32_t>(exploredPerCentRequired);
    serialize<uint32_t>(unused3);
  }
  serialize<uint32_t>(allConditionsRequired);
  if (getGameVersion() >= genie::GV_AoK) // 1.14
  {
    serialize<uint32_t>(victoryMode);
    serialize<uint32_t>(scoreRequired);
    serialize<uint32_t>(timeForTimedGame);
  }
}

ScnDiplomacy::ScnDiplomacy()
{
}

ScnDiplomacy::~ScnDiplomacy()
{
}

void ScnDiplomacy::serializeObject(void)
{
  serialize<uint32_t>(stances, 16, 16); // Diplomacy (16*16*4)
  serialize<uint32_t>(individualVictory, 16, 180); // Individual Victory (12*60)
}

ScnDisables::ScnDisables()
{
}

ScnDisables::~ScnDisables()
{
}

void ScnDisables::serializeObject(void)
{
  if (getGameVersion() >= genie::GV_AoK) // 1.18
    serialize<uint32_t>(numDisabledTechs, 16);
  serialize<uint32_t>(disabledTechs, 16, getGameVersion() < genie::GV_AoK /*>1.03*/ ? 20 : getGameVersion() < genie::GV_SWGB ? 30 : 60);
  if (getGameVersion() >= genie::GV_AoK) // 1.18
  {
    serialize<uint32_t>(numDisabledUnits, 16);
    serialize<uint32_t>(disabledUnits, 16, getGameVersion() < genie::GV_SWGB ? 30 : 60);
    serialize<uint32_t>(numDisabledBuildings, 16);
    serialize<uint32_t>(disabledBuildings, 16, getGameVersion() < genie::GV_SWGB ? 20 : 60);
  }
}

ScnPlayerData3::ScnPlayerData3()
{
}

ScnPlayerData3::~ScnPlayerData3()
{
}

void ScnPlayerData3::serializeObject(void)
{
  serialize<uint32_t>(playerCount_);
  serializeSub<ScnPlayerData3Sub>(playerData, 8);
}

ScnPlayerData3Sub::ScnPlayerData3Sub()
{
}

ScnPlayerData3Sub::~ScnPlayerData3Sub()
{
}

void ScnPlayerData3Sub::serializeObject(void)
{
  serializeSizedString<uint16_t>(playerName);
  serialize<float>(initCameraX);
  serialize<float>(initCameraY);
  serialize<int16_t>(initCameraX2);
  serialize<int16_t>(initCameraY2);
  serialize<uint8_t>(alliedVictory);
  serializeSize<uint16_t>(playerCount_, diplomacy1.size());
  serialize<uint8_t>(diplomacy1, playerCount_);
  serialize<uint32_t>(diplomacy2, playerCount_);
  serialize<uint32_t>(playerColor);
  serialize<float>(unknown1);
  serializeSize<uint16_t>(unknownCount_, unknown3.size() / 44);
  serialize<uint8_t>(unknown2, 8);
  serialize<uint8_t>(unknown3, unknownCount_ * 44);
  serialize<uint8_t>(unknown4, 7);
  serialize<int32_t>(unknown5);
}

}
