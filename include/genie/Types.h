/*
    genieutils - <description>
    Copyright (C) 2011 - 2013  Armin Preiml <email>
    Copyright (C) 2014  Mikko T P

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

namespace genie
{

enum GameVersion
{
  GV_None = 0, //Game version not set
  GV_TEST, // ?
  GV_AoEB, // 7.04
  GV_AoETB, // 7.11
  GV_AoE, // 7.2
  GV_RoR, // 7.24
  GV_AoKA, // 10.19
  GV_AoKB, // 11.05
  GV_AoK, // 11.5
  GV_TC, // 11.76
  GV_AoKHD, // 12.0
  GV_SWGB, // 1.0
  GV_CC // 1.1
};

}

#endif //GENIE_TYPES_H
