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
 * chord_tpl.cpp
 *
 * Implementation of the ChordTpl class
 */

#include "chord_tpl.h"
#include "config.h"
#include "lmhelpers.h"
#include "lmtypes.h"
#include "music_scale.h"


using namespace std;

namespace anatomist {

std::map<chord_quality_t, std::vector<std::vector<note_presense_state_t>>> ChordTpl::chord_qlty_tpls_ = {
    {cq_maj,                {
                                    {nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_P,  nps_NP, nps_P,  nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP},
                                }},
    {cq_min,                {
                                    {nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_PF, nps_NP, nps_P,  nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP},
                                }},
    {cq_5,                  {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_NP, nps_NP, nps_P,  nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    {cq_7,                  {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_P,  nps_NP, nps_P,  nps_NP, nps_PF,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP},
                                }},
    {cq_maj7,               {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_P,  nps_NP, nps_P,  nps_NP, nps_P,   nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    {cq_min7,               {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_PF, nps_NP, nps_P,  nps_NP, nps_PF,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    {cq_sus2,               {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_P,  nps_NP, nps_NP, nps_P,  nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    {cq_sus4,               {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_NP, nps_P,  nps_P,  nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    {cq_hdim7,              {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_PF, nps_NP, nps_PF, nps_NP, nps_PF,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    {cq_aug,                {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_P,  nps_NP, nps_PS, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    {cq_dim,                {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_PF, nps_NP, nps_PF, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    {cq_dim7,               {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_PF, nps_NP, nps_PF, nps_NP, nps_PFF, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    /* END OF MINIMIM SUPPORTED SET */
    {cq_dim7,               {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_PF, nps_NP, nps_PF, nps_NP, nps_PFF, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    {cq_maj_add9,           {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_P,  nps_NP, nps_P,  nps_NP, nps_NP,  nps_NP, nps_P,  nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    {cq_min_add9,           {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_PF, nps_NP, nps_P,  nps_NP, nps_NP,  nps_NP, nps_P,  nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    {cq_maj6,               {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_P,  nps_NP, nps_P,  nps_P,  nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    {cq_min6,               {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_PF, nps_NP, nps_P,  nps_P,  nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    {cq_maj9,               {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_P,  nps_NP, nps_P,  nps_NP, nps_P,   nps_NP, nps_P,  nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    {cq_min9,               {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_PF, nps_NP, nps_P,  nps_NP, nps_PF,  nps_NP, nps_P,  nps_NP, nps_NP, nps_NP, nps_NP}
                                }},
    {cq_maj_add11,          {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_P,  nps_NP, nps_P,  nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_P,  nps_NP, nps_NP}
                                }},
    {cq_7_add9sharp,        {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_P,  nps_NP, nps_P,  nps_NP, nps_PF,  nps_NP, nps_PS, nps_NP, nps_NP,  nps_NP, nps_NP}
                                }},
    {cq_9,                  {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_P,  nps_NP, nps_P,  nps_NP, nps_PF,  nps_NP, nps_P,  nps_NP, nps_NP, nps_NP, nps_NP},
                                }},
    {cq_aug7,               {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_P,  nps_NP, nps_PS, nps_NP, nps_PF,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP},
                                }},
    {cq_maj11,              {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_P,  nps_NP, nps_P,  nps_NP, nps_P,   nps_NP, nps_P,  nps_NP, nps_P,  nps_NP, nps_NP}
                                }},
    {cq_min11,              {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_PF, nps_NP, nps_P,  nps_NP, nps_PF,  nps_NP, nps_P,  nps_NP, nps_P,  nps_NP, nps_NP}
                                }},
    {cq_maj13,              {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_P,  nps_NP, nps_P,  nps_NP, nps_P,   nps_NP, nps_P,  nps_NP, nps_P,  nps_NP, nps_P}
                                }},
    {cq_min13,              {
                                    {nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,  nps_NP, nps_NP, nps_NP, nps_NP, nps_NP, nps_NP,
                                     nps_P,  nps_NP, nps_PF, nps_NP, nps_P,  nps_NP, nps_PF,  nps_NP, nps_P,  nps_NP, nps_P,  nps_NP, nps_P}
                                }},
};


ChordTpl::ChordTpl(note_t root_note, chord_quality_t cq, uint8_t slash_subtype) :
        root_note_(root_note), chord_quality_(cq)
{
    if ((root_note < note_Min) || (root_note > note_Max)) {
        if ((root_note == note_Unknown) && (cq == cq_unknown)) {
            InitN_();
            return;
        } else {
            throw std::invalid_argument("ChordTpl(): Invalid note");
        }
    }
    if ((cq < cq_Min) || (cq > cq_Max) ||
        (chord_qlty_tpls_.find(cq) == chord_qlty_tpls_.end()))
    {
        throw std::invalid_argument("ChordTpl(): Invalid chord quality");
    }
    if (slash_subtype >= chord_qlty_tpls_[cq].size()) {
        throw std::invalid_argument("ChordTpl(): Invalid slash subtype");
    }

    InitTpl_(root_note, cq, slash_subtype);
}

ChordTpl::ChordTpl(note_t root_note, chord_quality_t cq, std::vector<prob_t> &tpl) :
        root_note_(root_note), chord_quality_(cq), tpl_(tpl)
{
    if ((root_note < note_Min) || (root_note > note_Max)) {
        throw std::invalid_argument("ChordTpl(): Invalid note");
    }

    if ((cq < cq_Min) || (cq > cq_Max)) {
        throw std::invalid_argument("ChordTpl(): Invalid chord quality");
    }
}

void ChordTpl::InitTpl_(note_t root_note, chord_quality_t cq, uint8_t ss)
{
    vector<note_t> scale = MusicScale::getMajorScale(root_note);
    vector<note_presense_state_t> *qt = &chord_qlty_tpls_[cq][ss];

    tpl_.resize(notes_Total * 2, 0);

    for (uint8_t i = 0; i < qt->size(); i++) {
        note_t note = note_Unknown;
        note_presense_state_t presenseState = (*qt)[i];
        uint8_t treble_offset = notes_Total;
        uint8_t scale_idx = i % (qt->size() / 2);

        switch(presenseState) {
            case nps_present:
                note = scale[scale_idx];
                break;
            case nps_present_flat:
            case nps_present_flat_flat:
            case nps_present_sharp:
            {
                int8_t interval;

                if (presenseState == nps_present_flat) {
                    interval = -1;
                } else if (presenseState == nps_present_flat_flat) {
                    interval = -2;
                } else {
                    interval = 1;
                }

                note = scale[scale_idx] + interval;

                break;
            }
            default:
                note = note_Unknown;
                break;
        }

        if (note != note_Unknown) {
            // the first half of a template is bass and the second is treble
            if (i < qt->size() / 2) {
                bass_note_ = note;
                bass_interval_ = scale_idx + 1;
                treble_offset = 0;
            }
            tpl_[note - note_Min + treble_offset] = 1;
        }
    }
}

void ChordTpl::InitN_()
{
    typeof(tpl_) treble(notes_Total, 1);
    tpl_.resize(notes_Total, 0);
    tpl_.insert(tpl_.end(), treble.begin(), treble.end());
}

tpl_score_t ChordTpl::GetScore(pcp_t *pcp)
{
    if (pcp->size() == tpl_.size()) {
        // 𝜎² = cfgScoringVariance
        // 𝛴 = 𝜎²𝑰₂₄, 𝛴⁻¹ = 𝜎⁻²𝑰₂₄
        // |𝛴| = det 𝛴 = 𝜎²⁴
        vector<amplitude_t> diff = *pcp - tpl_;
        diff.erase(diff.begin(), diff.begin() + notes_Total); // do not consider bass
        const float D = pow(2 * M_PI * cfgScoringVariance, 12);
        //TODO: optimize by pre-calculating denominator
        return exp(-0.5  * Helpers::sumProduct(diff, diff) / cfgScoringVariance) / sqrt(D);
    } else if (pcp->size() == tpl_.size() / 2) { /* no separation between bass and treble */
        vector<typeof(tpl_[0])> treble(tpl_.begin() + notes_Total, tpl_.end());
        return pcp->euclideanDistance<typeof(tpl_[0])>(treble);
    } else {
        throw invalid_argument("Incompatible PCP size");
    }
}

note_t ChordTpl::RootNote()
{
    return root_note_;
}

note_t ChordTpl::BassNote()
{
    return bass_note_;
}

chord_quality_t ChordTpl::Quality()
{
    return chord_quality_;
}

size_t ChordTpl::SlashSubtypesCnt(chord_quality_t q)
{
    if ((q < cq_Min) || (q > cq_Max) ||
        (chord_qlty_tpls_.find(q) == chord_qlty_tpls_.end()))
    {
        throw std::invalid_argument("ChordTpl::SlashSubtypesCnt(): Invalid chord quality");
    }

    return chord_qlty_tpls_[q].size();
}

ostream& operator<<(std::ostream& os, const ChordTpl& tpl)
{
    os << Chord(tpl.root_note_, tpl.chord_quality_, tpl.bass_note_, tpl.bass_interval_);
    if (tpl.bass_interval_ == 1) {
#if defined(CFG_HARTE_SYNTAX) && CFG_HARTE_SYNTAX == 1
        os << "/" << 1;
#else
        os << "/" << tpl.bass_note_;
#endif /* CFG_HARTE_SYNTAX */
    }
    for (auto &v : tpl.tpl_) {
        os << "," << v;
    }

    return os;
}

}
