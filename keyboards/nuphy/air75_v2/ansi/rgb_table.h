/*
Copyright 2023 @ Nuphy <https://nuphy.com/>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#define    BREATHE_TAB_LEN     128

#define    SIDE_BRIGHT_MAX     6
#define    SIDE_SPEED_MAX      4
#define    SIDE_COLOUR_MAX     10

#define    RF_LED_LINK_PERIOD  500
#define    RF_LED_PAIR_PERIOD  250

//----------------------------------------------------------------
//        color table
//----------------------------------------------------------------
const uint8_t colour_lib[SIDE_COLOUR_MAX][3] =
{
    {RGB_RED},
    {RGB_ORANGE},
    {RGB_YELLOW},
    {RGB_GREEN},
    {RGB_SPRINGGREEN},
    {RGB_CYAN},
    {RGB_BLUE},
    {RGB_PURPLE},
    {RGB_MAGENTA},
    {RGB_WHITE}
};
