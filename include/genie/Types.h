/*
    genieutils - A library for reading and writing data files of genie
               engine games.
    Copyright (C) 2011 - 2013  Armin Preiml
    Copyright (C) 2014 - 2022  Mikko "Tapsa" P
    Copyright (C) 2023  Manuel Winocur
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

#ifndef GENIE_TYPES_H
#define GENIE_TYPES_H

#include <map>

namespace genie
{

// Combine T1 to T8 and C2 to C14 into single versions.
enum GameVersion
{
  GV_None = 0, //Game version not set
  GV_TEST, // ?
  GV_MIK, // ?
  GV_DAVE, // ?
  GV_MATT, // ?
  GV_AoEB, // 7.04 - 7.11
  GV_AoE, // 7.2
  GV_RoR, // 7.24
  GV_Tapsa, GV_T2, GV_T3, GV_T4, GV_T5, GV_T6, GV_T7, GV_T8, // 10.1 - 10.8
  GV_AoKE3, // 9.36
  GV_AoKA, // 10.19
  GV_AoKB, // 11.05
  GV_AoK, // 11.5
  GV_TC, // 11.76
  GV_TCV, // Terrain patch
  GV_Cysion, // 12.0
  GV_C2, GV_C3, GV_C4, GV_CK, GV_C5, GV_C6, GV_C7, GV_C8, GV_C9, GV_C10, GV_C11, GV_C12, GV_C13, GV_C14, // 12.52 - 12.94
  GV_C15, // 13.11
  GV_C16, // 20.01
  GV_C17, // 20.14
  GV_C18, // 25.27
  GV_C19, GV_C20, // 26.23 - 40.3
  GV_C21, // 61.4
  GV_C22, GV_C23, GV_C24, GV_C25, GV_C26, GV_C27, // 61.6 - 63.4
  GV_C28, // 64.6
  GV_C29, // 64.8
  GV_C30, // 65.0
  GV_C31, // 65.5
  GV_C32, // 67.9
  GV_SWGB, // 1.0
  GV_CC, // 1.1
  GV_CCV, // Terrain patch
  GV_CCV2 // Terrain patch + tech tree patch
};

const std::map<std::string, GameVersion> SupportedDatVersionsToGameVersion =
{
    {"VER 7.1", GV_C14},
    {"VER 7.2", GV_C15},
    {"VER 7.3", GV_C16},
    {"VER 7.4", GV_C17},
    {"VER 7.5", GV_C18},
    {"VER 7.6", GV_C19},
    {"VER 7.7", GV_C20},
    {"VER 7.8", GV_C21},
    {"VER 7.9", GV_C22},
    {"VER 8.0", GV_C23},
    {"VER 8.1", GV_C24},
    {"VER 8.2", GV_C25},
    {"VER 8.3", GV_C26},
    {"VER 8.4", GV_C27},
    {"VER 8.5", GV_C28},
    {"VER 8.6", GV_C29},
    {"VER 8.7", GV_C30},
    {"VER 8.8", GV_C31},
    {"VER 8.9", GV_C32},
};

const GameVersion GV_LatestTap = GV_T8;
const GameVersion GV_LatestDE2 = GV_C32;

struct XYZF
{
  float x, y, z;
};

}

#endif //GENIE_TYPES_H
