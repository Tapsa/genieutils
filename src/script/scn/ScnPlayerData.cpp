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

ScnPlayerData1::ScnPlayerData1()
{
}

ScnPlayerData1::~ScnPlayerData1()
{
}

void ScnPlayerData1::serializeObject(void)
{
  if (isOperation(OP_READ))
  {
    playerNames.resize(16);
    for (uint8_t i=0; i<16; i++)
      playerNames[i] = readString(256);
  }
  else if (isOperation(OP_WRITE))
    for (uint8_t i=0; i<16; i++)
      writeString(playerNames[i], 256);
  if (getGameVersion() >= genie::GV_AoK) // 1.16
    serialize<uint32_t>(playerNamesStringTable, 16);
  serializeSub<ScnPlayerInfo>(playerInfo, 16);
  serialize<uint8_t>(conquestVictory);
  serialize<ISerializable>(unknownData);
  serializeSizedString<uint16_t>(originalFileName);

  // Messages and cinematics
  serialize<ISerializable>(messagesCinematics);

  serializeSizedStrings<uint16_t>(aiNames, 16);
  serializeSizedStrings<uint16_t>(cityNames, 16);
  if (getGameVersion() >= genie::GV_AoE) // 1.08
    serializeSizedStrings<uint16_t>(personalityNames, 16);

  serializeSub(aiFiles, 16);

  serialize<uint8_t>(aiTypes, 16);
}

ScnPlayerInfo::ScnPlayerInfo()
{
}

ScnPlayerInfo::~ScnPlayerInfo()
{
}

void ScnPlayerInfo::serializeObject(void)
{
  serialize<uint32_t>(active);
  serialize<uint32_t>(human);
  serialize<uint32_t>(civilizationID);
  serialize<uint32_t>(unknown1);
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

  serialize<std::string>(aiFilename, aiFilenameSize);
  serialize<std::string>(cityFilename, cityFileSize);
  if (getGameVersion() >= genie::GV_AoE) // 1.08
    serialize<std::string>(perFilename, perFileSize);
}

PlayerResources::PlayerResources()
{
}

PlayerResources::~PlayerResources()
{
}

void PlayerResources::serializeObject(void)
{
  serializeSub(resources, 16);
}

Resources::Resources()
{
}

Resources::~Resources()
{
}

void Resources::serializeObject(void)
{
  serialize<uint32_t>(gold);
  serialize<uint32_t>(wood);
  serialize<uint32_t>(food);
  serialize<uint32_t>(stone);
  serialize<uint32_t>(oreX);
  serialize<uint32_t>(padding);
}

ScnVictory::ScnVictory()
{
}

ScnVictory::~ScnVictory()
{
}

void ScnVictory::serializeObject(void)
{
  serialize<uint32_t>(conquestRequired);
  serialize<uint32_t>(unused1);
  serialize<uint32_t>(numRelicsRequired);
  serialize<uint32_t>(unused2);
  serialize<uint32_t>(exploredPerCentRequired);
  serialize<uint32_t>(unused3);
  serialize<uint32_t>(allCustomConditionsRequired);
  serialize<uint32_t>(victoryMode);
  serialize<uint32_t>(scoreRequired);
  serialize<uint32_t>(timeForTimedGame);
}

ScnDiplomacy::ScnDiplomacy()
{
}

ScnDiplomacy::~ScnDiplomacy()
{
}

void ScnDiplomacy::serializeObject(void) // 12612 bytes
{
  serialize<uint32_t>(stances, 16, 16); // 1024 bytes
  serialize<uint32_t>(unused, 2880); // 11520 bytes
}

ScnDisables::ScnDisables()
{
}

ScnDisables::~ScnDisables()
{
}

void ScnDisables::serializeObject(void) // 5388 : 9868 bytes
{
  serialize<uint32_t>(alliedVictory, 16); // 64 bytes
  serialize<uint32_t>(numDisabledTechs, 16); // 64 bytes
  serialize<uint32_t>(disabledTechs, 16, getGameVersion() < genie::GV_SWGB ? 30 : 60); // 1920 : 3840 bytes
  serialize<uint32_t>(numDisabledUnits, 16); // 64 bytes
  serialize<uint32_t>(disabledUnits, 16, getGameVersion() < genie::GV_SWGB ? 30 : 60); // 1920 : 3840 bytes
  serialize<uint32_t>(numDisabledBuildings, 16); // 64 bytes
  serialize<uint32_t>(disabledBuildings, 16, getGameVersion() < genie::GV_SWGB ? 20 : 60); // 1280 : 3840 bytes
  serialize<uint32_t>(unused1); // 4 bytes
  serialize<uint32_t>(unused2); // 4 bytes
  serialize<uint32_t>(allTechs); // 4 bytes
  serialize<uint32_t>(startingAge, 16); // 64 bytes
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
  serialize<double>(unknown);
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
