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

#ifndef CHOPT_IMAGE_HPP
#define CHOPT_IMAGE_HPP

#include <cstdint>
#include <tuple>
#include <vector>

#include "cimg_wrapper.hpp"

#include "chart.hpp"

struct DrawnRow {
    double start;
    double end;
};

struct DrawnNote {
    double beat;
    double length;
    NoteColour colour;
    bool is_sp_note;
};

struct DrawingInstructions {
    std::vector<DrawnRow> rows;
    std::vector<double> half_beat_lines;
    std::vector<double> beat_lines;
    std::vector<double> measure_lines;
    std::vector<DrawnNote> notes;
    std::vector<std::tuple<double, double>> green_ranges;
};

DrawingInstructions create_instructions(const NoteTrack& track,
                                        std::int32_t resolution,
                                        const SyncTrack& sync_track);

using Image = cimg_library::CImg<unsigned char>;

Image create_path_image(const DrawingInstructions& instructions);

#endif
