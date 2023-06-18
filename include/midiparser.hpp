/*
 * CHOpt - Star Power optimiser for Clone Hero
 * Copyright (C) 2023 Raymond Wright
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

#ifndef CHOPT_MIDIPARSER_HPP
#define CHOPT_MIDIPARSER_HPP

#include <cstdint>
#include <set>
#include <span>
#include <string>

#include "hopothreshold.hpp"
#include "ini.hpp"
#include "midi.hpp"
#include "song.hpp"
#include "songparts.hpp"

class MidiParser {
private:
    std::string m_song_name;
    std::string m_artist;
    std::string m_charter;
    HopoThreshold m_hopo_threshold;
    std::set<Instrument> m_permitted_instruments;
    bool m_permit_solos;

public:
    explicit MidiParser(const IniValues& ini);
    MidiParser& hopo_threshold(HopoThreshold hopo_threshold);
    MidiParser& permit_instruments(std::set<Instrument> permitted_instruments);
    MidiParser& parse_solos(bool permit_solos);
    Song from_midi(const Midi& midi) const;
    Song parse(std::span<const std::uint8_t> data) const;
};

#endif
