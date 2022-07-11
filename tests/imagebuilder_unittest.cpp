/*
 * CHOpt - Star Power optimiser for Clone Hero
 * Copyright (C) 2020, 2021, 2022 Raymond Wright
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

#include <cstdlib>

#include <boost/test/unit_test.hpp>

#include "imagebuilder.hpp"

template <typename T>
static bool operator!=(const DrawnNote<T>& lhs, const DrawnNote<T>& rhs)
{
    return std::abs(lhs.beat - rhs.beat) >= 0.000001
        || std::abs(lhs.length - rhs.length) >= 0.000001
        || std::tie(lhs.colour, lhs.is_sp_note)
        != std::tie(rhs.colour, rhs.is_sp_note);
}

template <typename T>
static std::ostream& operator<<(std::ostream& stream, const DrawnNote<T>& note)
{
    stream << '{' << note.beat << "b, Length " << note.length << ", Colour "
           << static_cast<int>(note.colour) << ", Is SP Note "
           << note.is_sp_note << '}';
    return stream;
}

static bool operator!=(const DrawnRow& lhs, const DrawnRow& rhs)
{
    return std::abs(lhs.start - rhs.start) >= 0.000001
        || std::abs(lhs.end - rhs.end) >= 0.000001;
}

static std::ostream& operator<<(std::ostream& stream, const DrawnRow& row)
{
    stream << '{' << row.start << ", " << row.end << '}';
    return stream;
}

static std::ostream& operator<<(std::ostream& stream, TrackType track_type)
{
    stream << static_cast<int>(track_type);
    return stream;
}

static std::ostream& operator<<(std::ostream& stream, DrumNoteColour colour)
{
    stream << static_cast<int>(colour);
    return stream;
}

namespace boost::test_tools::tt_detail {
template <> struct print_log_value<std::tuple<double, double>> {
    void operator()(std::ostream& stream,
                    const std::tuple<double, double>& tuple)
    {
        const auto& [first, second] = tuple;
        stream << '{' << first << ", " << second << '}';
    }
};

template <> struct print_log_value<std::tuple<double, int, int>> {
    void operator()(std::ostream& stream,
                    const std::tuple<double, int, int>& tuple)
    {
        const auto& [first, second, third] = tuple;
        stream << '{' << first << ", " << second << ", " << third << '}';
    }
};
}

BOOST_AUTO_TEST_SUITE(track_type_is_stored_correctly)

BOOST_AUTO_TEST_CASE(five_fret_gets_the_right_track_type)
{
    NoteTrack<NoteColour> track {{}, {}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, false};

    BOOST_CHECK_EQUAL(builder.track_type(), TrackType::FiveFret);
}

BOOST_AUTO_TEST_CASE(six_fret_gets_the_right_track_type)
{
    NoteTrack<GHLNoteColour> track {{}, {}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, false};

    BOOST_CHECK_EQUAL(builder.track_type(), TrackType::SixFret);
}

BOOST_AUTO_TEST_CASE(drums_gets_the_right_track_type)
{
    NoteTrack<DrumNoteColour> track {{}, {}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, DrumSettings::default_settings(), false};

    BOOST_CHECK_EQUAL(builder.track_type(), TrackType::Drums);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(notes_are_handled_correctly)

BOOST_AUTO_TEST_CASE(non_sp_non_sustains_are_handled_correctly)
{
    NoteTrack<NoteColour> track {
        {{0}, {768, 0, NoteColour::Red}}, {}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, false};
    std::vector<DrawnNote<NoteColour>> expected_notes {
        {0.0, 0.0, NoteColour::Green, false},
        {4.0, 0.0, NoteColour::Red, false}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.notes().cbegin(), builder.notes().cend(),
        expected_notes.cbegin(), expected_notes.cend());
}

BOOST_AUTO_TEST_CASE(sustains_are_handled_correctly)
{
    NoteTrack<NoteColour> track {{{0, 96}}, {}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, false};
    std::vector<DrawnNote<NoteColour>> expected_notes {
        {0.0, 0.5, NoteColour::Green, false}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.notes().cbegin(), builder.notes().cend(),
        expected_notes.cbegin(), expected_notes.cend());
}

BOOST_AUTO_TEST_CASE(sp_notes_are_recorded)
{
    NoteTrack<NoteColour> track {{{0}, {768}}, {{768, 100}}, {}, {},
                                 {},           {},           192};
    ImageBuilder builder {track, {}, false};
    std::vector<DrawnNote<NoteColour>> expected_notes {
        {0.0, 0.0, NoteColour::Green, false},
        {4.0, 0.0, NoteColour::Green, true}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.notes().cbegin(), builder.notes().cend(),
        expected_notes.cbegin(), expected_notes.cend());
}

BOOST_AUTO_TEST_CASE(six_fret_notes_are_handled_correctly)
{
    NoteTrack<GHLNoteColour> track {
        {{0}, {768, 0, GHLNoteColour::BlackHigh}}, {}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, false};
    std::vector<DrawnNote<GHLNoteColour>> expected_notes {
        {0.0, 0.0, GHLNoteColour::WhiteLow, false},
        {4.0, 0.0, GHLNoteColour::BlackHigh, false}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.ghl_notes().cbegin(), builder.ghl_notes().cend(),
        expected_notes.cbegin(), expected_notes.cend());
}

BOOST_AUTO_TEST_CASE(drum_notes_are_handled_correctly)
{
    NoteTrack<DrumNoteColour> track {
        {{0}, {768, 0, DrumNoteColour::YellowCymbal}}, {}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, DrumSettings::default_settings(), false};
    std::vector<DrawnNote<DrumNoteColour>> expected_notes {
        {0.0, 0.0, DrumNoteColour::Red, false},
        {4.0, 0.0, DrumNoteColour::YellowCymbal, false}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.drum_notes().cbegin(), builder.drum_notes().cend(),
        expected_notes.cbegin(), expected_notes.cend());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(drawn_rows_are_handled_correctly)

BOOST_AUTO_TEST_CASE(simple_four_four_is_handled_correctly)
{
    NoteTrack<NoteColour> track {{{2880}}, {}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, false};
    std::vector<DrawnRow> expected_rows {{0.0, 16.0}};

    BOOST_CHECK_EQUAL_COLLECTIONS(builder.rows().cbegin(),
                                  builder.rows().cend(), expected_rows.cbegin(),
                                  expected_rows.cend());
}

BOOST_AUTO_TEST_CASE(three_x_time_sigs_are_handled)
{
    NoteTrack<NoteColour> track {{{2450}}, {}, {}, {}, {}, {}, 192};
    SyncTrack sync_track {{{0, 4, 4}, {768, 3, 4}, {1344, 3, 8}, {1632, 4, 4}},
                          {}};
    ImageBuilder builder {track, sync_track, false};
    std::vector<DrawnRow> expected_rows {{0.0, 12.5}, {12.5, 16.5}};

    BOOST_CHECK_EQUAL_COLLECTIONS(builder.rows().cbegin(),
                                  builder.rows().cend(), expected_rows.cbegin(),
                                  expected_rows.cend());
}

BOOST_AUTO_TEST_CASE(time_signature_changes_off_measure_are_coped_with)
{
    NoteTrack<NoteColour> track {{{768}}, {}, {}, {}, {}, {}, 192};
    SyncTrack sync_track {{{0, 4, 4}, {767, 3, 4}, {1344, 3, 8}}, {}};
    ImageBuilder builder {track, sync_track, false};
    std::vector<DrawnRow> expected_rows {{0.0, 7.0}};

    BOOST_CHECK_EQUAL_COLLECTIONS(builder.rows().cbegin(),
                                  builder.rows().cend(), expected_rows.cbegin(),
                                  expected_rows.cend());
}

BOOST_AUTO_TEST_CASE(x_four_for_x_gt_16_is_handled)
{
    NoteTrack<NoteColour> track {{{0}}, {}, {}, {}, {}, {}, 192};
    SyncTrack sync_track {{{0, 17, 4}}, {}};
    ImageBuilder builder {track, sync_track, false};
    std::vector<DrawnRow> expected_rows {{0.0, 16.0}, {16.0, 17.0}};

    BOOST_CHECK_EQUAL_COLLECTIONS(builder.rows().cbegin(),
                                  builder.rows().cend(), expected_rows.cbegin(),
                                  expected_rows.cend());
}

BOOST_AUTO_TEST_CASE(enough_rows_are_drawn_for_end_of_song_sustains)
{
    NoteTrack<NoteColour> track {{{0, 3840}}, {}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, false};

    BOOST_CHECK_EQUAL(builder.rows().size(), 2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(beat_lines_are_correct)

BOOST_AUTO_TEST_CASE(four_four_works_fine)
{
    NoteTrack<NoteColour> track {{{767}}, {}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, false};
    std::vector<double> expected_half_beat_lines {0.5, 1.5, 2.5, 3.5};
    std::vector<double> expected_beat_lines {1.0, 2.0, 3.0};
    std::vector<double> expected_measure_lines {0.0, 4.0};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.half_beat_lines().cbegin(), builder.half_beat_lines().cend(),
        expected_half_beat_lines.cbegin(), expected_half_beat_lines.cend());
    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.beat_lines().cbegin(), builder.beat_lines().cend(),
        expected_beat_lines.cbegin(), expected_beat_lines.cend());
    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.measure_lines().cbegin(), builder.measure_lines().cend(),
        expected_measure_lines.cbegin(), expected_measure_lines.cend());
}

BOOST_AUTO_TEST_CASE(four_eight_works_fine)
{
    NoteTrack<NoteColour> track {{{767}}, {}, {}, {}, {}, {}, 192};
    SyncTrack sync_track {{{0, 4, 8}}, {}};
    ImageBuilder builder {track, sync_track, false};
    std::vector<double> expected_half_beat_lines {0.25, 0.75, 1.25, 1.75,
                                                  2.25, 2.75, 3.25, 3.75};
    std::vector<double> expected_beat_lines {0.5, 1.0, 1.5, 2.5, 3.0, 3.5};
    std::vector<double> expected_measure_lines {0.0, 2.0, 4.0};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.half_beat_lines().cbegin(), builder.half_beat_lines().cend(),
        expected_half_beat_lines.cbegin(), expected_half_beat_lines.cend());
    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.beat_lines().cbegin(), builder.beat_lines().cend(),
        expected_beat_lines.cbegin(), expected_beat_lines.cend());
    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.measure_lines().cbegin(), builder.measure_lines().cend(),
        expected_measure_lines.cbegin(), expected_measure_lines.cend());
}

BOOST_AUTO_TEST_CASE(combination_of_four_four_and_four_eight_works_fine)
{
    NoteTrack<NoteColour> track {{{1151}}, {}, {}, {}, {}, {}, 192};
    SyncTrack sync_track {{{0, 4, 4}, {768, 4, 8}}, {}};
    ImageBuilder builder {track, sync_track, false};
    std::vector<double> expected_half_beat_lines {0.5,  1.5,  2.5,  3.5,
                                                  4.25, 4.75, 5.25, 5.75};
    std::vector<double> expected_beat_lines {1.0, 2.0, 3.0, 4.5, 5.0, 5.5};
    std::vector<double> expected_measure_lines {0.0, 4.0, 6.0};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.half_beat_lines().cbegin(), builder.half_beat_lines().cend(),
        expected_half_beat_lines.cbegin(), expected_half_beat_lines.cend());
    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.beat_lines().cbegin(), builder.beat_lines().cend(),
        expected_beat_lines.cbegin(), expected_beat_lines.cend());
    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.measure_lines().cbegin(), builder.measure_lines().cend(),
        expected_measure_lines.cbegin(), expected_measure_lines.cend());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(time_signatures_are_handled_correctly)

BOOST_AUTO_TEST_CASE(normal_time_signatures_are_handled_correctly)
{
    NoteTrack<NoteColour> track {{{1920}}, {}, {}, {}, {}, {}, 192};
    SyncTrack sync_track {{{0, 4, 4}, {768, 4, 8}}, {}};
    ImageBuilder builder {track, sync_track, false};
    builder.add_time_sigs(sync_track, 192);
    std::vector<std::tuple<double, int, int>> expected_time_sigs {{0.0, 4, 4},
                                                                  {4.0, 4, 8}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.time_sigs().cbegin(), builder.time_sigs().cend(),
        expected_time_sigs.cbegin(), expected_time_sigs.cend());
}

BOOST_AUTO_TEST_CASE(time_sig_changes_past_the_end_of_the_song_are_removed)
{
    NoteTrack<NoteColour> track {{{768}}, {}, {}, {}, {}, {}, 192};
    SyncTrack sync_track {{{0, 4, 4}, {1920, 3, 4}}, {}};
    ImageBuilder builder {track, sync_track, false};
    builder.add_time_sigs(sync_track, 192);

    BOOST_CHECK_EQUAL(builder.time_sigs().size(), 1);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(tempos_are_handled_correctly)

BOOST_AUTO_TEST_CASE(normal_tempos_are_handled_correctly)
{
    NoteTrack<NoteColour> track {{{1920}}, {}, {}, {}, {}, {}, 192};
    SyncTrack sync_track {{}, {{0, 150000}, {384, 120000}, {768, 200000}}};
    ImageBuilder builder {track, sync_track, false};
    builder.add_bpms(sync_track, 192);
    std::vector<std::tuple<double, double>> expected_bpms {
        {0.0, 150.0}, {2.0, 120.0}, {4.0, 200.0}};

    BOOST_CHECK_EQUAL_COLLECTIONS(builder.bpms().cbegin(),
                                  builder.bpms().cend(), expected_bpms.cbegin(),
                                  expected_bpms.cend());
}

BOOST_AUTO_TEST_CASE(tempo_changes_past_the_end_of_the_song_are_removed)
{
    NoteTrack<NoteColour> track {{{768}}, {}, {}, {}, {}, {}, 192};
    SyncTrack sync_track {{}, {{0, 120000}, {1920, 200000}}};
    ImageBuilder builder {track, sync_track, false};
    builder.add_bpms(sync_track, 192);

    BOOST_CHECK_EQUAL(builder.bpms().size(), 1);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(song_header_information_is_added)

BOOST_AUTO_TEST_CASE(normal_speed)
{
    NoteTrack<NoteColour> track {{{0}}, {}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, false};
    builder.add_song_header("TestName", "GMS", "NotGMS", 100);

    BOOST_CHECK_EQUAL(builder.song_name(), "TestName");
    BOOST_CHECK_EQUAL(builder.artist(), "GMS");
    BOOST_CHECK_EQUAL(builder.charter(), "NotGMS");
}

BOOST_AUTO_TEST_CASE(double_speed)
{
    NoteTrack<NoteColour> track {{{0}}, {}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, false};
    builder.add_song_header("TestName", "GMS", "NotGMS", 200);

    BOOST_CHECK_EQUAL(builder.song_name(), "TestName (200%)");
    BOOST_CHECK_EQUAL(builder.artist(), "GMS");
    BOOST_CHECK_EQUAL(builder.charter(), "NotGMS");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(green_ranges_for_sp_phrases_are_added_correctly)
{
    NoteTrack<NoteColour> track {
        {{960}, {1344, 96}}, {{768, 384}, {1200, 150}}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, false};
    builder.add_sp_phrases(track, {});
    std::vector<std::tuple<double, double>> expected_green_ranges {{5.0, 5.1},
                                                                   {7.0, 7.5}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.green_ranges().cbegin(), builder.green_ranges().cend(),
        expected_green_ranges.cbegin(), expected_green_ranges.cend());
}

BOOST_AUTO_TEST_CASE(green_ranges_have_a_minimum_size)
{
    NoteTrack<NoteColour> track {{{768}}, {{768, 384}}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, false};
    builder.add_sp_phrases(track, {});

    std::vector<std::tuple<double, double>> expected_green_ranges {{4.0, 4.1}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.green_ranges().cbegin(), builder.green_ranges().cend(),
        expected_green_ranges.cbegin(), expected_green_ranges.cend());
}

BOOST_AUTO_TEST_CASE(green_ranges_for_six_fret_sp_phrases_are_added_correctly)
{
    NoteTrack<GHLNoteColour> track {
        {{960}, {1344, 96}}, {{768, 384}, {1200, 150}}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, false};
    builder.add_sp_phrases(track, {});
    std::vector<std::tuple<double, double>> expected_green_ranges {{5.0, 5.1},
                                                                   {7.0, 7.5}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.green_ranges().cbegin(), builder.green_ranges().cend(),
        expected_green_ranges.cbegin(), expected_green_ranges.cend());
}

BOOST_AUTO_TEST_CASE(green_ranges_for_drums_sp_phrases_are_added_correctly)
{
    NoteTrack<DrumNoteColour> track {
        {{960}, {1344}}, {{768, 384}, {1200, 150}}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, DrumSettings::default_settings(), false};
    builder.add_sp_phrases(track, {});
    std::vector<std::tuple<double, double>> expected_green_ranges {{5.0, 5.1},
                                                                   {7.0, 7.1}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.green_ranges().cbegin(), builder.green_ranges().cend(),
        expected_green_ranges.cbegin(), expected_green_ranges.cend());
}

BOOST_AUTO_TEST_CASE(drum_fills_are_drawn_with_add_drum_fills)
{
    NoteTrack<DrumNoteColour> track {
        {{288, 0, DrumNoteColour::Red}}, {}, {}, {{192, 96}}, {}, {}, 192};
    ImageBuilder builder {track, {}, DrumSettings::default_settings(), false};
    builder.add_drum_fills(track);

    std::vector<std::tuple<double, double>> expected_fill_ranges {{1.0, 1.5}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.fill_ranges().cbegin(), builder.fill_ranges().cend(),
        expected_fill_ranges.cbegin(), expected_fill_ranges.cend());
}

BOOST_AUTO_TEST_CASE(drum_fills_cancelled_by_a_kick_are_not_drawn)
{
    NoteTrack<DrumNoteColour> track {
        {{288, 0, DrumNoteColour::Kick}}, {}, {}, {{192, 96}}, {}, {}, 192};
    ImageBuilder builder {track, {}, DrumSettings::default_settings(), false};
    builder.add_drum_fills(track);

    BOOST_TEST(builder.fill_ranges().empty());
}

BOOST_AUTO_TEST_CASE(double_kicks_only_drawn_with_enable_double_kick)
{
    NoteTrack<DrumNoteColour> track {
        {{0, 0, DrumNoteColour::Kick}, {192, 0, DrumNoteColour::DoubleKick}},
        {},
        {},
        {},
        {},
        {},
        192};
    ImageBuilder no_double_builder {
        track, {}, {false, false, false, false}, false};
    ImageBuilder double_builder {track, {}, {true, false, false, false}, false};

    BOOST_CHECK_EQUAL(no_double_builder.drum_notes().size(), 1);
    BOOST_CHECK_EQUAL(double_builder.drum_notes().size(), 2);
}

BOOST_AUTO_TEST_CASE(single_kicks_disappear_with_disable_kick)
{
    NoteTrack<DrumNoteColour> track {
        {{0, 0, DrumNoteColour::Kick}, {192, 0, DrumNoteColour::DoubleKick}},
        {},
        {},
        {},
        {},
        {},
        192};
    ImageBuilder builder {track, {}, {true, true, false, false}, false};

    BOOST_CHECK_EQUAL(builder.drum_notes().size(), 1);
}

BOOST_AUTO_TEST_CASE(cymbals_become_toms_with_pro_drums_off)
{
    NoteTrack<DrumNoteColour> track {
        {{0, 0, DrumNoteColour::YellowCymbal}}, {}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, {true, false, false, false}, false};

    BOOST_CHECK_EQUAL(builder.drum_notes().size(), 1);
    BOOST_CHECK_EQUAL(builder.drum_notes()[0].colour, DrumNoteColour::Yellow);
}

BOOST_AUTO_TEST_CASE(disco_flip_matters_only_with_pro_drums_on)
{
    NoteTrack<DrumNoteColour> track {{{192, 0, DrumNoteColour::YellowCymbal},
                                      {288, 0, DrumNoteColour::Yellow}},
                                     {},
                                     {},
                                     {},
                                     {{192, 192}},
                                     {},
                                     192};
    ImageBuilder normal_builder {track, {}, {true, false, false, false}, false};
    ImageBuilder pro_builder {
        track, {}, DrumSettings::default_settings(), false};

    BOOST_CHECK_EQUAL(normal_builder.drum_notes().size(), 2);
    BOOST_CHECK_EQUAL(normal_builder.drum_notes()[0].colour,
                      DrumNoteColour::Yellow);
    BOOST_CHECK_EQUAL(pro_builder.drum_notes().size(), 2);
    BOOST_CHECK_EQUAL(pro_builder.drum_notes()[0].colour, DrumNoteColour::Red);
    BOOST_CHECK_EQUAL(pro_builder.drum_notes()[1].colour,
                      DrumNoteColour::Yellow);
}

BOOST_AUTO_TEST_CASE(unison_phrases_are_added_correctly)
{
    NoteTrack<NoteColour> track {
        {{960}, {1344, 96}}, {{768, 384}, {1200, 150}}, {}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, false};
    builder.add_sp_phrases(track, {{768, 384}});
    std::vector<std::tuple<double, double>> expected_unison_ranges {{5.0, 5.1}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.unison_ranges().cbegin(), builder.unison_ranges().cend(),
        expected_unison_ranges.cbegin(), expected_unison_ranges.cend());
}

BOOST_AUTO_TEST_SUITE(add_sp_acts_adds_correct_ranges)

BOOST_AUTO_TEST_CASE(normal_path_is_drawn_correctly)
{
    NoteTrack<NoteColour> track {
        {{0, 96}, {192}}, {{0, 50}}, {}, {}, {}, {}, 192};
    TimeConverter converter {{}, 192, ChGuitarEngine(), {}};
    PointSet points {track,
                     converter,
                     {},
                     SqueezeSettings::default_settings(),
                     DrumSettings::default_settings(),
                     ChGuitarEngine()};
    ImageBuilder builder {track, {}, false};
    Path path {{{points.cbegin(), points.cend() - 1, Beat {0.25}, Beat {0.1},
                 Beat {0.9}}},
               0};
    builder.add_sp_phrases(track, {});
    builder.add_sp_acts(points, converter, path);
    std::vector<std::tuple<double, double>> expected_blue_ranges {{0.1, 0.9}};
    std::vector<std::tuple<double, double>> expected_red_ranges {{0.0, 0.1},
                                                                 {0.9, 1.0}};
    std::vector<std::tuple<double, double>> expected_yellow_ranges {
        {0.25, 0.5}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.blue_ranges().cbegin(), builder.blue_ranges().cend(),
        expected_blue_ranges.cbegin(), expected_blue_ranges.cend());
    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.red_ranges().cbegin(), builder.red_ranges().cend(),
        expected_red_ranges.cbegin(), expected_red_ranges.cend());
    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.yellow_ranges().cbegin(), builder.yellow_ranges().cend(),
        expected_yellow_ranges.cbegin(), expected_yellow_ranges.cend());
}

BOOST_AUTO_TEST_CASE(squeezes_are_only_drawn_when_required)
{
    NoteTrack<NoteColour> track {
        {{0}, {192}, {384}, {576}}, {}, {}, {}, {}, {}, 192};
    TimeConverter converter {{}, 192, ChGuitarEngine(), {}};
    PointSet points {track,
                     converter,
                     {},
                     SqueezeSettings::default_settings(),
                     DrumSettings::default_settings(),
                     ChGuitarEngine()};
    ImageBuilder builder {track, {}, false};
    Path path {{{points.cbegin(), points.cbegin() + 1, Beat {0.25}, Beat {0.1},
                 Beat {1.1}},
                {points.cbegin() + 2, points.cbegin() + 3, Beat {0.25},
                 Beat {2.0}, Beat {2.9}}},
               0};
    builder.add_sp_acts(points, converter, path);
    std::vector<std::tuple<double, double>> expected_red_ranges {{0.0, 0.1},
                                                                 {2.9, 3.0}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.red_ranges().cbegin(), builder.red_ranges().cend(),
        expected_red_ranges.cbegin(), expected_red_ranges.cend());
}

BOOST_AUTO_TEST_CASE(blue_ranges_are_cropped_for_reverse_squeezes)
{
    NoteTrack<NoteColour> track {
        {{192}, {384}, {576}, {768}}, {}, {}, {}, {}, {}, 192};
    TimeConverter converter {{}, 192, ChGuitarEngine(), {}};
    PointSet points {track,
                     converter,
                     {},
                     SqueezeSettings::default_settings(),
                     DrumSettings::default_settings(),
                     ChGuitarEngine()};
    ImageBuilder builder {track, {}, false};
    Path path {{{points.cbegin() + 1, points.cbegin() + 2, Beat {5.0},
                 Beat {0.0}, Beat {5.0}}},
               0};
    builder.add_sp_acts(points, converter, path);
    std::vector<std::tuple<double, double>> expected_blue_ranges {{1.0, 4.0}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.blue_ranges().cbegin(), builder.blue_ranges().cend(),
        expected_blue_ranges.cbegin(), expected_blue_ranges.cend());
}

BOOST_AUTO_TEST_CASE(blue_ranges_are_cropped_by_the_end_of_the_song)
{
    NoteTrack<NoteColour> track {{{192}}, {}, {}, {}, {}, {}, 192};
    TimeConverter converter {{}, 192, ChGuitarEngine(), {}};
    PointSet points {track,
                     converter,
                     {},
                     SqueezeSettings::default_settings(),
                     DrumSettings::default_settings(),
                     ChGuitarEngine()};
    ImageBuilder builder {track, {}, false};
    Path path {{{points.cbegin(), points.cbegin(), Beat {0.0}, Beat {0.0},
                 Beat {16.0}}},
               0};
    builder.add_sp_acts(points, converter, path);
    std::vector<std::tuple<double, double>> expected_blue_ranges {{0.0, 4.0}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.blue_ranges().cbegin(), builder.blue_ranges().cend(),
        expected_blue_ranges.cbegin(), expected_blue_ranges.cend());
}

BOOST_AUTO_TEST_CASE(blue_and_red_ranges_are_shifted_by_video_lag)
{
    NoteTrack<NoteColour> track {
        {{0}, {192}, {384}, {576}, {768}, {1530}}, {}, {}, {}, {}, {}, 192};
    TimeConverter converter {{}, 192, ChGuitarEngine(), {}};
    PointSet points {track,
                     converter,
                     {},
                     {1.0, 1.0, Second(0.0), Second(0.05), Second(0.0)},
                     DrumSettings::default_settings(),
                     ChGuitarEngine()};
    ImageBuilder builder {track, {}, false};
    Path path {{{points.cbegin(), points.cbegin() + 1, Beat {0.25}, Beat {0.1},
                 Beat {1.1}},
                {points.cbegin() + 2, points.cbegin() + 3, Beat {0.25},
                 Beat {2.0}, Beat {2.9}},
                {points.cbegin() + 5, points.cbegin() + 5, Beat {0.25},
                 Beat {7.0}, Beat {23.0}}},
               0};
    std::vector<std::tuple<double, double>> expected_blue_ranges {
        {0.0, 1.0}, {1.9, 2.8}, {6.9, 8.0}};
    std::vector<std::tuple<double, double>> expected_red_ranges {{2.8, 3.0}};

    builder.add_sp_acts(points, converter, path);

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.blue_ranges().cbegin(), builder.blue_ranges().cend(),
        expected_blue_ranges.cbegin(), expected_blue_ranges.cend());
    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.red_ranges().cbegin(), builder.red_ranges().cend(),
        expected_red_ranges.cbegin(), expected_red_ranges.cend());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(add_solo_sections_add_correct_ranges)
{
    NoteTrack<NoteColour> track {{{0}}, {}, {{192, 384, 0}}, {}, {}, {}, 192};
    ImageBuilder builder {track, {}, false};
    builder.add_solo_sections(track.solos(DrumSettings::default_settings()),
                              192);
    std::vector<std::tuple<double, double>> expected_solo_ranges {{1.0, 2.0}};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.solo_ranges().cbegin(), builder.solo_ranges().cend(),
        expected_solo_ranges.cbegin(), expected_solo_ranges.cend());
}

BOOST_AUTO_TEST_SUITE(add_measure_values_gives_correct_values)

BOOST_AUTO_TEST_CASE(notes_with_no_activations_or_solos)
{
    NoteTrack<NoteColour> track {{{0}, {768}}, {}, {}, {}, {}, {}, 192};
    PointSet points {track,
                     {{}, 192, ChGuitarEngine(), {}},
                     {},
                     SqueezeSettings::default_settings(),
                     DrumSettings::default_settings(),
                     ChGuitarEngine()};
    Path path;
    ImageBuilder builder {track, {}, false};
    builder.add_measure_values(points, {{}, 192, ChGuitarEngine(), {}}, path);
    std::vector<int> expected_base_values {50, 50};
    std::vector<int> expected_score_values {50, 100};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.base_values().cbegin(), builder.base_values().cend(),
        expected_base_values.cbegin(), expected_base_values.cend());
    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.score_values().cbegin(), builder.score_values().cend(),
        expected_score_values.cbegin(), expected_score_values.cend());
}

BOOST_AUTO_TEST_CASE(solos_are_added)
{
    NoteTrack<NoteColour> track {
        {{768}}, {}, {{0, 100, 100}, {200, 800, 100}}, {}, {}, {}, 192};
    PointSet points {track,
                     {{}, 192, ChGuitarEngine(), {}},
                     {},
                     SqueezeSettings::default_settings(),
                     DrumSettings::default_settings(),
                     ChGuitarEngine()};
    Path path;
    ImageBuilder builder {track, {}, false};
    builder.add_measure_values(points, {{}, 192, ChGuitarEngine(), {}}, path);
    std::vector<int> expected_score_values {100, 250};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.score_values().cbegin(), builder.score_values().cend(),
        expected_score_values.cbegin(), expected_score_values.cend());
}

// This bug caused a crash in a few songs, for example Satch Boogie (Live) from
// Guitar Hero X.
BOOST_AUTO_TEST_CASE(solos_ending_past_last_note_are_handled_correctly)
{
    NoteTrack<NoteColour> track {{{0}}, {}, {{0, 1600, 50}}, {}, {}, {}, 192};
    PointSet points {track,
                     {{}, 192, ChGuitarEngine(), {}},
                     {},
                     SqueezeSettings::default_settings(),
                     DrumSettings::default_settings(),
                     ChGuitarEngine()};
    Path path;
    ImageBuilder builder {track, {}, false};
    builder.add_measure_values(points, {{}, 192, ChGuitarEngine(), {}}, path);
    std::vector<int> expected_score_values {100};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.score_values().cbegin(), builder.score_values().cend(),
        expected_score_values.cbegin(), expected_score_values.cend());
}

BOOST_AUTO_TEST_CASE(activations_are_added)
{
    NoteTrack<NoteColour> track {
        {{0}, {192}, {384}, {768}}, {}, {}, {}, {}, {}, 192};
    PointSet points {track,
                     {{}, 192, ChGuitarEngine(), {}},
                     {},
                     SqueezeSettings::default_settings(),
                     DrumSettings::default_settings(),
                     ChGuitarEngine()};
    Path path {
        {{points.cbegin() + 2, points.cbegin() + 3, Beat {0.0}, Beat {0.0}}},
        100};
    ImageBuilder builder {track, {}, false};
    builder.add_measure_values(points, {{}, 192, ChGuitarEngine(), {}}, path);
    std::vector<int> expected_score_values {200, 300};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.score_values().cbegin(), builder.score_values().cend(),
        expected_score_values.cbegin(), expected_score_values.cend());
}

BOOST_AUTO_TEST_CASE(video_lag_is_accounted_for)
{
    NoteTrack<NoteColour> track {{{0}, {768}}, {}, {}, {}, {}, {}, 192};
    PointSet points {track,
                     {{}, 192, ChGuitarEngine(), {}},
                     {},
                     {1.0, 1.0, Second {0.0}, Second {-0.1}, Second {0.0}},
                     DrumSettings::default_settings(),
                     ChGuitarEngine()};
    Path path {
        {{points.cbegin() + 1, points.cbegin() + 1, Beat {0.0}, Beat {0.0}}},
        50};
    ImageBuilder builder {track, {}, false};
    builder.add_measure_values(points, {{}, 192, ChGuitarEngine(), {}}, path);
    std::vector<int> expected_base_values {50, 50};
    std::vector<int> expected_score_values {50, 150};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.base_values().cbegin(), builder.base_values().cend(),
        expected_base_values.cbegin(), expected_base_values.cend());
    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.score_values().cbegin(), builder.score_values().cend(),
        expected_score_values.cbegin(), expected_score_values.cend());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(add_sp_values_gives_correct_values)
{
    NoteTrack<NoteColour> track {
        {{0}, {192, 768}}, {{192, 50}}, {}, {}, {}, {}, 192};
    SpData sp_data {
        track, {}, {}, SqueezeSettings::default_settings(), ChGuitarEngine()};
    ImageBuilder builder {track, {}, false};
    builder.add_sp_values(sp_data, ChGuitarEngine());
    std::vector<double> expected_sp_values {3.14, 1.0};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        builder.sp_values().cbegin(), builder.sp_values().cend(),
        expected_sp_values.cbegin(), expected_sp_values.cend());
}

BOOST_AUTO_TEST_CASE(set_total_score_sets_the_correct_value)
{
    NoteTrack<NoteColour> track {{{0}, {192}}, {{0, 50}}, {}, {}, {}, {}, 192};
    TimeConverter converter {{}, 192, ChGuitarEngine(), {}};
    PointSet points {track,
                     converter,
                     {},
                     SqueezeSettings::default_settings(),
                     DrumSettings::default_settings(),
                     ChGuitarEngine()};
    ImageBuilder builder {track, {}, false};
    Path path {{{points.cbegin(), points.cend() - 1, Beat {0.25}, Beat {0.1},
                 Beat {0.9}}},
               50};
    builder.set_total_score(points, {{0, 1, 100}}, path);

    BOOST_CHECK_EQUAL(builder.total_score(), 250);
}

BOOST_AUTO_TEST_CASE(lefty_flip_is_handled)
{
    NoteTrack<NoteColour> track {{{0}}, {}, {}, {}, {}, {}, 192};
    ImageBuilder lefty_builder {track, {}, true};
    ImageBuilder righty_builder {track, {}, false};

    BOOST_TEST(lefty_builder.is_lefty_flip());
    BOOST_TEST(!righty_builder.is_lefty_flip());
}