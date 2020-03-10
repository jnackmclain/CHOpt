/*
 *  chopt - Star Power optimiser for Clone Hero
 *  Copyright (C) 2020  Raymond Wright
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "catch.hpp"

#include "chart.hpp"

TEST_CASE("Chart reads resolution and offset", "[Song]")
{
    SECTION("Defaults are 192 Res and 0 Offset")
    {
        auto text = "[Song]\n{\n}\n[SyncTrack]\n{\n}\n[Events]\n{\n}\n";
        const auto chart = Chart(text);
        const auto DEFAULT_RESOLUTION = 192.F;

        REQUIRE(chart.get_resolution() == DEFAULT_RESOLUTION);
        REQUIRE(chart.get_offset() == 0.F);
    }
    SECTION("Defaults are overriden by specified values")
    {
        auto text = "[Song]\n{\nResolution = 200\nOffset = "
                    "100\n}\n[SyncTrack]\n{\n}\n[Events]\n{\n}\n";
        const auto chart = Chart(text);
        const auto RESOLUTION = 200.F;

        REQUIRE(chart.get_resolution() == RESOLUTION);
        REQUIRE(chart.get_offset() == 100.F);
    }
}
