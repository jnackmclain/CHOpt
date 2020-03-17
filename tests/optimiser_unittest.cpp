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

#include <array>

#include "catch.hpp"

#include "chart.hpp"
#include "optimiser.hpp"

TEST_CASE("Beats to seconds conversion", "Beats<->S")
{
    const auto track = SyncTrack({{0, 4, 4}}, {{0, 150000}, {800, 200000}});
    const auto header = SongHeader(0.F, 200);
    const auto converter = TimeConverter(track, header);
    constexpr std::array beats {-1.0, 0.0, 3.0, 5.0};
    constexpr std::array seconds {-0.5, 0.0, 1.2, 1.9};

    for (auto i = 0U; i < beats.size(); ++i) {
        REQUIRE(converter.beats_to_seconds(beats.at(i))
                == Approx(seconds.at(i)));
    }

    for (auto i = 0U; i < beats.size(); ++i) {
        REQUIRE(converter.seconds_to_beats(seconds.at(i))
                == Approx(beats.at(i)));
    }
}

// Last checked: 24.0.1555-master
TEST_CASE("Non-hold notes", "Non hold")
{
    SECTION("Single notes give 50 points")
    {
        const auto track = NoteTrack({{768}, {1000}}, {}, {});
        const auto points = notes_to_points(track, SongHeader());
        const auto expected_points
            = std::vector<Point>({{768, 50}, {1000, 50}});

        REQUIRE(points == expected_points);
    }

    SECTION("Chords give multiples of 50 points")
    {
        const auto track = NoteTrack(
            {{768, 0, NoteColour::Green}, {768, 0, NoteColour::Red}}, {}, {});
        const auto points = notes_to_points(track, SongHeader());
        const auto expected_points = std::vector<Point>({{768, 100}});

        REQUIRE(points == expected_points);
    }
}

// Last checked: 24.0.1555-master
TEST_CASE("Hold notes", "Hold")
{
    SECTION("Hold note points depend on resolution")
    {
        const auto track = NoteTrack({{768, 15}}, {}, {});
        const auto first_points = notes_to_points(track, SongHeader());
        const auto first_expected_points
            = std::vector<Point>({{768, 50}, {775, 1}, {782, 1}, {789, 1}});
        const auto header = SongHeader(0.F, 200);
        const auto second_points = notes_to_points(track, header);
        const auto second_expected_points
            = std::vector<Point>({{768, 50}, {776, 1}, {784, 1}});

        REQUIRE(first_points == first_expected_points);
        REQUIRE(second_points == second_expected_points);
    }

    SECTION("Hold note points and chords")
    {
        const auto track = NoteTrack(
            {{768, 7, NoteColour::Green}, {768, 8, NoteColour::Red}}, {}, {});
        const auto points = notes_to_points(track, SongHeader());
        const auto expected_points
            = std::vector<Point>({{768, 100}, {775, 1}, {782, 1}});

        REQUIRE(points == expected_points);
    }

    SECTION("Resolutions below 25 do not enter an infinite loop")
    {
        const auto track = NoteTrack({{768, 2}}, {}, {});
        const auto header = SongHeader(0.F, 1);
        const auto points = notes_to_points(track, header);
        const auto expected_points
            = std::vector<Point>({{768, 50}, {769, 1}, {770, 1}});

        REQUIRE(points == expected_points);
    }
}

// Last checked: 24.0.1555-master
TEST_CASE("Points are sorted", "Sorted")
{
    const auto track = NoteTrack({{768, 15}, {770, 0}}, {}, {});
    const auto points = notes_to_points(track, SongHeader());
    const auto expected_points = std::vector<Point>(
        {{768, 50}, {770, 50}, {775, 1}, {782, 1}, {789, 1}});

    REQUIRE(points == expected_points);
}
