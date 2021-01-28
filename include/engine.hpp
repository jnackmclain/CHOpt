/*
 * CHOpt - Star Power optimiser for Clone Hero
 * Copyright (C) 2021 Raymond Wright
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

#ifndef CHOPT_ENGINE_HPP
#define CHOPT_ENGINE_HPP

class Engine {
public:
    virtual int base_note_value() const = 0;
    virtual double burst_size() const = 0;
    virtual bool do_chords_multiply_sustains() const = 0;
    virtual bool has_bres() const = 0;
    virtual double sp_gain_rate() const = 0;
    virtual int sust_points_per_beat() const = 0;
    virtual double timing_window() const = 0;
    virtual ~Engine() = default;
};

class ChEngine final : public Engine {
public:
    int base_note_value() const override { return 50; }
    double burst_size() const override { return 0.25; }
    bool do_chords_multiply_sustains() const override { return false; }
    bool has_bres() const override { return false; }
    double sp_gain_rate() const override { return 1 / 30.0; }
    int sust_points_per_beat() const override { return 25; }
    double timing_window() const override { return 0.07; }
};

class RbEngine final : public Engine {
public:
    int base_note_value() const override { return 25; }
    double burst_size() const override { return 0.0; }
    bool do_chords_multiply_sustains() const override { return true; }
    bool has_bres() const override { return true; }
    double sp_gain_rate() const override { return 0.034; }
    int sust_points_per_beat() const override { return 12; }
    double timing_window() const override { return 0.1; }
};

#endif
