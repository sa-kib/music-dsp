/*
 * Copyright 2019 Volodymyr Kononenko
 *
 * This file is part of Music-DSP.
 *
 * Music-DSP is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Music-DSP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Music-DSP. If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file        lmhelpers.h
 * @brief       Header file for helpers
 * @addtogroup  libmusic
 * @{
 */

#pragma once

#include <stdint.h>
#include <vector>
#include <fstream>
#include <cassert>

#include "lmtypes.h"

#define UNUSED(expr) do { (void)(expr); } while (0)

// Introduced for cross-compilation for Android because log2 is
// not defined for all target platforms.
/** @TODO find a proper fix */
#define log2(x) (log(x) / log(2))

#ifndef HELPERS_TEST_FRIENDS
#define HELPERS_TEST_FRIENDS
#endif

/**
 * @ingroup libmusic
 */
class Helpers {

HELPERS_TEST_FRIENDS;

public:
    /**
     * Calculate first power of two number which is greater than n
     *
     * @param   n
     * @return  corresponding power of two value
     */
    static uint32_t nextPowerOf2(uint32_t n);

    /**
     * Check if number is a power of 2
     *
     * @param   n   number to check
     * @return true if power of 2, false otherwise
     */
    static bool isPowerOf2(uint32_t n);

    static std::vector<complex_t> timeDomain2ComplexVector(amplitude_t *, uint32_t, uint32_t);

    /**
     * Round number with requested precision
     *
     * @param   n       number to round up
     * @param   scale   numeric scale - number of digits after decimal point
     * @return  rounded number with scale
     */
    template<typename T> static T stdRound(T n, uint8_t scale)
    {
        if (scale == 0) {
            n = round(n);
        } else {
            n *= (pow(10, scale));
            n = round(n);
            n /= (pow(10, scale));
        }

        return n;
    }

    template<typename T1, typename T2>
    static bool cmpPairBySecond(const std::pair<T1, T2> &p1, const std::pair<T1, T2> &p2)
    {
       return p1.second < p2.second;
    }

    static bool almostEqual(double a, double b, double eps);

    template <typename T>
    static T sumProduct(const std::vector<T> &lv, const std::vector<T> &rv)
    {
        assert(lv.size() == rv.size());
        T sum = 0;
        auto li = lv.begin();
        auto ri = rv.begin();
        while (li != lv.end()) {
            sum += *li * *ri;
            li++; ri++;
        }
        return sum;
    }

    /**
     * Return median of the vector
     */
    template<typename T>
    static T median(std::vector<T> v)
    {
        if (v.empty())
            throw std::invalid_argument("median(): empty vector");
        if (v.size() == 1)
            return v[0];

        auto mid_idx = v.size() / 2;
        sort(v.begin(), v.end());

        if (v.size() % 2)
            return v[mid_idx];
        return (v[mid_idx] + v[mid_idx - 1]) / 2;
    }

    template<typename T>
    static void lm_helpers_peep(const char *name, const std::vector<std::vector<T>> & matrix)
    {
        const char *path = getenv(name);
        if (!path)
            return;
        std::ofstream csv(path, std::ios::out | std::ios::trunc);
        if (csv.is_open()) {
            for (auto &row : matrix) {
                for (auto &value: row)
                    csv << value << ",";
                csv.seekp(-1, std::ios_base::end);
                csv << std::endl;
            }
            csv.close();
        }
    }
    template<typename T>
    static void lm_helpers_peep(const char *name, const std::vector<T> & vect)
    {
        lm_helpers_peep(name, std::vector<std::vector<T>>(1, vect));
    }
};

#ifndef NDEBUG
    #define LM_PEEP(name, var) Helpers::lm_helpers_peep(#name, (var))
#else
    #define LM_PEEP(name, var) UNUSED(var) // make release build happpy
#endif

/** @} */
