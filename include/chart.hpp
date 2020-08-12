/*
 * chopt - Star Power optimiser for Clone Hero
 * Copyright (C) 2020 Raymond Wright
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CHOPT_CHART_HPP
#define CHOPT_CHART_HPP

#include <map>
#include <string>
#include <string_view>
#include <vector>

struct ChartEvent {
    int position;
    std::string event_type;
    std::string data;
};

struct ChartSection {
    std::string name;
    std::map<std::string, std::string> key_value_pairs;
    std::vector<ChartEvent> events;
};

struct Chart {
    std::vector<ChartSection> sections;
};

Chart parse_chart(std::string_view data);

#endif
