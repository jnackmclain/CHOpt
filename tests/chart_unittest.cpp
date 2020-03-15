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

#include "catch.hpp"

#include "chart.hpp"

TEST_CASE("NoteTrack ctor maintains invariants", "NoteTrack")
{
    SECTION("Notes are sorted")
    {
        const auto notes = std::vector<Note>({{768}, {384}});
        const auto track = NoteTrack(notes, {}, {});
        const auto sorted_notes = std::vector<Note>({{384}, {768}});

        REQUIRE(track.notes() == sorted_notes);
    }
}

TEST_CASE("Chart reads resolution and offset", "Song")
{
    SECTION("Defaults are 192 Res and 0 Offset")
    {
        auto text = "[Song]\n{\n}\n[SyncTrack]\n{\n}\n[Events]\n{\n}\n";
        const auto header = Chart(text).header();
        constexpr auto DEFAULT_RESOLUTION = 192.F;

        REQUIRE(header.resolution() == Approx(DEFAULT_RESOLUTION));
        REQUIRE(header.offset() == 0.F);
    }

    SECTION("Defaults are overriden by specified values")
    {
        auto text = "[Song]\n{\nResolution = 200\nOffset = "
                    "100\n}\n[SyncTrack]\n{\n}\n[Events]\n{\n}\n";
        const auto header = Chart(text).header();
        constexpr auto RESOLUTION = 200.F;

        REQUIRE(header.resolution() == Approx(RESOLUTION));
        REQUIRE(header.offset() == 100.F);
    }
}

TEST_CASE("Chart reads sync track correctly", "SyncTrack")
{
    auto text = "[Song]\n{\n}\n[SyncTrack]\n{\n0 = B 200000\n0 = TS 4\n768 = "
                "TS 4 1\n}\n[Events]\n{\n}\n";
    const auto sync_track = Chart(text).sync_track();
    const auto time_sigs = std::vector<TimeSignature>({{0, 4, 4}, {768, 4, 2}});
    const auto bpms = std::vector<BPM>({{0, 200000}});

    REQUIRE(sync_track.time_sigs() == time_sigs);
    REQUIRE(sync_track.bpms() == bpms);
}

TEST_CASE("Chart reads events correctly", "Events")
{
    auto text = "[Song]\n{\n}\n[SyncTrack]\n{\n}\n[Events]\n{\n768 = E "
                "\"section intro\"\n}\n";
    const auto chart = Chart(text);
    const auto sections = std::vector<Section>({{768, "intro"}});

    REQUIRE(chart.sections() == sections);
}

TEST_CASE("Chart reads easy note track correctly", "Easy")
{
    auto text = "[Song]\n{\n}\n[SyncTrack]\n{\n}\n[Events]\n{\n}\n[EasySingle]"
                "\n{\n768 = N 0 0\n768 = S 2 100\n}\n";
    const auto chart = Chart(text);
    const auto note_track
        = NoteTrack {{{768, 0, NoteColour::Green}}, {{768, 100}}, {}};

    REQUIRE(chart.note_track(Difficulty::Easy) == note_track);
}

TEST_CASE("Chart skips UTF-8 BOM", "BOM")
{
    auto text = "\xEF\xBB\xBF[Song]\n{\nOffset = "
                "100\n}\n[SyncTrack]\n{\n}\n[Events]\n{\n}\n";
    const auto header = Chart(text).header();
    constexpr auto RESOLUTION = 192.F;

    REQUIRE(header.resolution() == Approx(RESOLUTION));
    REQUIRE(header.offset() == 100.F);
}

TEST_CASE("Chart can end without a newline", "End-NL")
{
    auto text = "\xEF\xBB\xBF[Song]\n{\nOffset = "
                "100\n}\n[SyncTrack]\n{\n}\n[Events]\n{\n}";
    const auto header = Chart(text).header();
    constexpr auto RESOLUTION = 192.F;

    REQUIRE(header.resolution() == Approx(RESOLUTION));
    REQUIRE(header.offset() == 100.F);
}

TEST_CASE("Chart does not need sections in usual order", "Section order")
{
    SECTION("Non note sections need not be present")
    {
        const auto chart = Chart("");
        constexpr auto RESOLUTION = 192.F;

        REQUIRE(chart.header().resolution() == Approx(RESOLUTION));
    }

    SECTION("Non note sections can be in any order")
    {
        auto text = "[SyncTrack]\n{\n0 = B 200000\n}\n[Events]\n{\n768 = E "
                    "\"section intro\"\n}\n[Song]\n{\nResolution = 200\n}";
        const auto chart = Chart(text);
        const auto sections = std::vector<Section>({{768, "intro"}});
        const auto bpms = std::vector<BPM>({{0, 200000}});
        constexpr auto RESOLUTION = 200.F;

        REQUIRE(chart.header().resolution() == Approx(RESOLUTION));
        REQUIRE(chart.sections() == sections);
        REQUIRE(chart.sync_track().bpms() == bpms);
    }
}

TEST_CASE("Note section can be split in multiple parts", "Note split")
{
    auto text = "[ExpertSingle]\n{\n768 = N 0 0\n}\n[ExpertSingle]\n{\n768 = N "
                "1 0\n}";
    const auto chart = Chart(text);
    const auto notes = std::vector<Note>(
        {{768, 0, NoteColour::Green}, {768, 0, NoteColour::Red}});

    REQUIRE(chart.note_track(Difficulty::Expert).notes() == notes);
}

TEST_CASE("Notes should be sorted", "NoteSort")
{
    auto text = "[ExpertSingle]\n{\n768 = N 0 0\n384 = N 0 0\n}";
    const auto chart = Chart(text);
    const auto notes = std::vector<Note>(
        {{384, 0, NoteColour::Green}, {768, 0, NoteColour::Green}});

    REQUIRE(chart.note_track(Difficulty::Expert).notes() == notes);
}

TEST_CASE("Tap flags do not always apply across tracks", "TapFlag")
{
    SECTION("Tap flags apply to earlier sections")
    {
        auto text = "[ExpertSingle]\n{\n768 = N 0 0\n}\n[ExpertSingle]\n{\n768 "
                    "= N 6 0\n}";
        const auto chart = Chart(text);

        REQUIRE(chart.note_track(Difficulty::Expert).notes()[0].is_tap);
    }

    SECTION("Tap flags do not apply to later sections")
    {
        auto text = "[ExpertSingle]\n{\n768 = N 6 0\n}\n[ExpertSingle]\n{\n768 "
                    "= N 0 0\n}";
        const auto chart = Chart(text);

        REQUIRE(!chart.note_track(Difficulty::Expert).notes()[0].is_tap);
    }
}

TEST_CASE("Notes with extra spaces are ignored", "NoteSpaceSkip")
{
    auto text = "[ExpertSingle]\n{\n768 = N  0 0\n768 = N 0  0\n}";
    const auto chart = Chart(text);

    REQUIRE(chart.note_track(Difficulty::Expert).notes().empty());
}
