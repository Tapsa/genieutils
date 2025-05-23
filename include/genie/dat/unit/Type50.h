/*
    geniedat - A library for reading and writing data files of genie
               engine games.
    Copyright (C) 2011 - 2013  Armin Preiml
    Copyright (C) 2011 - 2021  Mikko "Tapsa" P
    Copyright (C) 2023  Manuel Winocur

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

#ifndef GENIE_TYPE50_H
#define GENIE_TYPE50_H

#include "genie/file/ISerializable.h"
#include "AttackOrArmor.h"

namespace genie
{

namespace unit
{

class Type50 : public ISerializable
{
public:
  Type50();
  virtual ~Type50();
  virtual void setGameVersion(GameVersion gv);

  union
  {
     int16_t BaseArmor = 1000;
     uint8_t BaseArmorU8;
  };
  std::vector<unit::AttackOrArmor> Attacks;
  std::vector<unit::AttackOrArmor> Armours;
  int16_t DefenseTerrainBonus = -1;
  float BonusDamageResistance = 0;
  float MaxRange = 0;
  float BlastWidth = 0;
  float ReloadTime = 0;
  int16_t ProjectileUnitID = -1;

  /// Percentage value determining the probability of an attack hiting
  int16_t AccuracyPercent = 0;

  /// Used in AoE/RoR for towers.
  uint8_t BreakOffCombat = 0;
  int16_t FrameDelay = 0;

  std::vector<float> GraphicDisplacement = {0, 0, 0};

  uint8_t BlastAttackLevel = 0;
  float MinRange = 0;
  float AccuracyDispersion = 0;
  int16_t AttackGraphic = -1;
  int16_t AttackGraphic2 = -1;
  int16_t DisplayedMeleeArmour = 0;
  int16_t DisplayedAttack = 0;
  float DisplayedRange = 0;
  float DisplayedReloadTime = 0;
  float BlastDamage = 0;
  float FriendlyFireDamage = 1;
  float DamageReflection = 0;
  int16_t InterruptFrame = -1;
  float GarrisonFirepower = 0;

protected:
  virtual void serializeObject(void);
};

}

}

#endif // GENIE_TYPE50_H
