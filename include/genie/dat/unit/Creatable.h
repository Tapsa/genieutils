/*
    geniedat - A library for reading and writing data files of genie
               engine games.
    Copyright (C) 2011 - 2013  Armin Preiml
    Copyright (C) 2011 - 2022  Mikko "Tapsa" P
    Copyright (C) 2024  Charles Harbord
    Copyright (C) 2024  Igor Djordjevic aka BugA_the_Great

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

#ifndef GENIE_CREATABLE_H
#define GENIE_CREATABLE_H

#include "genie/file/ISerializable.h"
#include "../ResourceUsage.h"

namespace genie
{

namespace unit
{

class Creatable : public ISerializable
{
public:
  Creatable();
  virtual ~Creatable();
  virtual void setGameVersion(GameVersion gv);

  static unsigned short getResourceCostsSize();

  typedef ResourceUsage<int16_t, int16_t, int16_t> ResourceCost;

  std::vector<ResourceCost> ResourceCosts;
  int16_t TrainTime = 0;
  int16_t TrainLocationID = -1;
  uint8_t ButtonID = 0;
  float RearAttackModifier = 0;
  float FlankAttackModifier = 0;
  uint8_t CreatableType = 0;
  uint8_t HeroMode = 0;
  int32_t GarrisonGraphic = -1;
  float TotalProjectiles = 0;
  uint8_t MaxTotalProjectiles = 0;

  /// The area from which projectiles spawn. Size and randomness.
  std::vector<float> ProjectileSpawningArea = {0, 0, 0};

  int32_t SecondaryProjectileUnit = -1;
  int32_t SpecialGraphic = -1;

  //TODO:
  /*
   * 1.Determines adjacent unit graphics
    2.Enter 0 or 1
    3.If “1”, the building will have the capability to change graphics depending
    on the buildings adjacent to it. For instance, a single wall will be a
    wall end piece, but if a wall is placed between two other walls, it will
    have a different graphic.
   */
  uint8_t SpecialAbility = 0;
  int16_t ButtonIconID = -1;
  int32_t ButtonShortTooltipID = -1;
  int32_t ButtonExtendedTooltipID = -1;
  int16_t ButtonHotkeyAction = -1;
  int16_t DisplayedPierceArmour = 0;
  int16_t SpawningGraphic = -1;
  int16_t UpgradeGraphic = -1;
  int16_t HeroGlowGraphic = -1;
  int16_t IdleAttackGraphic = -1;

  float MaxCharge = 0;
  float RechargeRate = 0;
  int16_t ChargeEvent = 0;
  int16_t ChargeType = 0;
  int16_t ChargeTarget = 0;
  int32_t ChargeProjectileUnit = -1;
  uint8_t AttackPriority = 0;
  float InvulnerabilityLevel = 0;

  float MinConversionTimeMod = 0;
  float MaxConversionTimeMod = 0;
  float ConversionChanceMod = 0;

protected:
  virtual void serializeObject(void);

};

}

}

#endif // GENIE_CREATABLE_H
