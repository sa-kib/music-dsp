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

/*
 * pitch_cls_profile.cpp
 *
 * Pitch Class Profile implementation
 */

#include <algorithm>
#include <iomanip>
#include <math.h>
#include <sstream>

#include "lmhelpers.h"
#include "pitch_calculator.h"
#include "pitch_cls_profile.h"
#include "window_functions.h"
#include "nnls.h"

using namespace std;

namespace anatomist {

log_spectrogram_t PitchClsProfile::complex_dict_ = log_spectrogram_t(0);

PitchClsProfile::PitchClsProfile()
{
    __mPCP.resize(notes_Total * 2);
    __mPCP.assign(__mPCP.size(), 0);
}

PitchClsProfile::PitchClsProfile(FFT *fft)
{
    PitchCalculator& pc = PitchCalculator::getInstance();

    __mPCP.resize(notes_Total, 0);

    for (int n = note_Min; n <= note_Max; n++) {
        note_t note = (note_t)n;

        freq_hz_t pitch_freq;
        uint32_t bin_idx;
        amplitude_t pitch_cls = 0;
        amplitude_t mag = 0;
        for (int o = OCTAVE_MIN; o <= OCTAVE_MAX; o++) {
            octave_t oct = (octave_t)o;

            pitch_freq = pc.noteToPitch(note, oct);
            bin_idx = fft->FreqToIdx(pitch_freq, round);

            if (bin_idx >= fft->GetFreqDomainLen()) {
                throw runtime_error("fftIdx >= pointsCnt");
            }

            mag = fft->GetFreqDomain().p[bin_idx];
            pitch_cls += mag * mag;
            __mPitchClsMax = max(pitch_cls, __mPitchClsMax);
        }

        __mPCP[note - note_Min] = pitch_cls;
    }
    __normalize();
}

PitchClsProfile::PitchClsProfile(const fd_t &fd_mags, tft_t *tft)
{
    PitchCalculator& pc = PitchCalculator::getInstance();
    uint8_t bps = tft->BinsPerSemitone();
    int32_t offset = tft->FreqToBin(pc.noteToPitch(note_E, OCTAVE_MIN));
    uint8_t semitones_cnt = fd_mags.size() / bps;
    freq_hz_t f3 = pc.noteToPitch(note_F, OCTAVE_3);
    uint32_t f3_idx = tft->FreqToBin(f3);
    uint32_t win_offset = 0; // TODO: calculate
    vector<amplitude_t> bass_win = WindowFunctions::getHamming(f3_idx / bps, win_offset);
    vector<amplitude_t> treble_win = WindowFunctions::getHamming(semitones_cnt, win_offset);

    __mPCP.resize(notes_Total * 2, 0);

    for (uint32_t bin = offset; bin < fd_mags.size() - (bps/2+1); bin += bps) {
        note_t note;
        amplitude_t tmp = 0;
        for (int32_t i = -bps/2; i < bps/2+1; i++) {
            tmp += fd_mags[bin + i] * (1 - abs(i * 1.0 / (bps/2 + 1)));
        }

        if (0 == tmp)
            continue;

        note = pc.pitchToNote(pc.getPitch(tft->BinToFreq(bin)));

        if (bin / bps < bass_win.size()) {
            __mPCP[note - note_Min] += tmp * bass_win[bin / bps];
        }
        __mPCP[note - note_Min + notes_Total] += tmp * treble_win[bin / bps];
    }

    __mPitchClsMax = *max_element(__mPCP.begin(), __mPCP.end());

    __normalize();
}

void PitchClsProfile::__normalize()
{
    if (__mPitchClsMax == 0) {
        return;
    }

    for (uint8_t i = 0; i < __mPCP.size(); i++) {
        __mPCP[i] = __mPCP[i] / __mPitchClsMax;
    }
}

amplitude_t PitchClsProfile::getPitchCls(note_t note, bool is_treble) const
{
    if ((note < note_Min) || (note > note_Max)) {
        throw std::invalid_argument("Invalid note");
    }
    size_t idx = (note - note_Min + notes_Total * is_treble) % __mPCP.size();
    return __mPCP[idx];
}

size_t PitchClsProfile::size()
{
    return __mPCP.size();
}

amplitude_t PitchClsProfile::euclideanDistance(PitchClsProfile &pcp)
{
    amplitude_t d = euclideanDistance<typeof(__mPCP[0])>(pcp.__mPCP);
    return d;
}

amplitude_t PitchClsProfile::divergenceKullbackLeibler(vector<amplitude_t> &v)
{
    if (v.size() != __mPCP.size()) {
        throw invalid_argument("divergenceKullbackLeibler(): wrong vector size");
    }

    amplitude_t d = 0;

    for (uint8_t i = 0; i < __mPCP.size(); i++) {
        d += __mPCP[i] * log10(__mPCP[i] / v[i]);
    }

    return d;
}

PitchClsProfile & PitchClsProfile::operator+=(const PitchClsProfile& pcp)
{
    for (uint8_t i = 0; i < __mPCP.size(); i++) {
        __mPCP[i] += pcp.__mPCP[i];
    }

    return *this;
}

PitchClsProfile & PitchClsProfile::operator/=(float denominator)
{
    for (uint8_t i = 0; i < __mPCP.size(); i++) {
        __mPCP[i] /= denominator;
    }

    return *this;
}

#define PCP_ROWS            10
#define PCP_SYM_PER_COL     3

ostream& operator<<(ostream& os, const PitchClsProfile& pcp)
{
    int percentPerRow = 100 / PCP_ROWS;

    for (int row = 0; row < PCP_ROWS; row++) {
        for (auto note_amp : pcp.__mPCP) {
            uint8_t progress = note_amp * 100;
            os << std::setw(PCP_SYM_PER_COL)
               << ((progress >= round(100 - percentPerRow * row)) ? '|' : ' ');
        }
        os << endl;
    }

    for (uint8_t i = 0; i < pcp.__mPCP.size() / notes_Total; i++) {
        for (int n = note_Min; n <= note_Max; n++) {
            os << std::setw(PCP_SYM_PER_COL) << static_cast<note_t>(n);
        }
    }

    return os;
}

string PitchClsProfile::toCSV()
{
    std::stringstream csv;

    for (uint8_t i = 0; i < __mPCP.size(); i++) {
        csv << setprecision(8) << __mPCP[i] << ((i < __mPCP.size() - 1) ? "," : "");
    }

    return csv.str();
}

fd_t PitchClsProfile::__makeComplexTone(freq_hz_t fq, tft_t &tft)
{
    td_t tone(tft.SpectrogramInterval(), 0);
    freq_hz_t harmonic_fq = fq;
    for (auto i = 1u; harmonic_fq < tft.GetMaxFreq(); harmonic_fq = fq * ++i) {
        for (auto j = 0u; j < tone.size(); j++) {
            tone[j] += pow(0.6, i - 1) * sin(2 * M_PI * j * harmonic_fq / tft.GetSampleRate());
        }
    }
    tft.Process(tone, 0);
    return tft.GetSpectrogram()[0];
}

void PitchClsProfile::__initComplexDictionary(tft_t &tft)
{
    log_spectrogram_t &cd = PitchClsProfile::complex_dict_;
    if (!cd.empty())
        return;
    PitchCalculator& pc = PitchCalculator::getInstance();
    freq_hz_t fq = pc.getPitch(tft.GetMinFreq());
    while (fq < tft.GetMaxFreq()) {
        cd.push_back(__makeComplexTone(fq, tft));
        fq = pc.getPitchByInterval(fq, 1);
    }
    LM_PEEP(PCP_complex_dict, cd);
}

fd_t PitchClsProfile::__nnlsWrapper(fd_t spectra)
{
    log_spectrogram_t &cd = PitchClsProfile::complex_dict_;
    if (cd.empty())
        throw runtime_error("complex dictionary not initialized");
    if (cd[0].size() != spectra.size())
        throw invalid_argument("incompatible spectra size");
    int mode, M = cd[0].size(), N = cd.size();
    float A[M * N], B[M], X[N], W[N], ZZ[M];
    int I[N];
    float rnorm;

    copy(spectra.begin(), spectra.end(), B); // amplitude_t to float
    for (int row = 0;
         row < N;
         copy(cd[row].begin(), cd[row].end(), A + row * M), row++);

    nnls(A, M, M, N, B, X, &rnorm, W, ZZ, I, &mode);
    if (mode == 3)
        throw invalid_argument("nnls unexpected mode");
    spectra.clear();
    if (mode == 1)
        spectra.insert(spectra.begin(), X, X + N);
    return spectra;
}

}
