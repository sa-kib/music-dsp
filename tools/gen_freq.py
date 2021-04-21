#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2021 Sergiy Kibrik
#
# This file is part of Music-DSP.
#
# Music-DSP is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Music-DSP is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with Music-DSP. If not, see <https://www.gnu.org/licenses/>.
#
# About:
#
# Generate music chords suitable as input for synth.sh script in format:
#    <chord> <octave> <note> ...
# All possible root notes and inversions combinations are considered,
# chords crossing octave boundaries also taken into account.
# See tpls dictionary for supported chord types.
#

maj_scale = {
    "C":["C", "D", "E", "F", "G", "A", "B", "C", "D", "E", "F", "G", "A", "B", "C"],
    "C#":["C#","D#","F", "F#","G#","A#","C", "C#","D#","F", "F#","G#","A#","C", "C#"],
    "D":["D", "E", "F#","G", "A", "B", "C#","D", "E", "F#","G", "A", "B", "C#","D" ],
    "D#":["D#","F", "G", "G#","A#","C", "D", "D#","F", "G", "G#","A#","C", "D", "D#"],
    "E":["E", "F#","G#","A", "B", "C#","D#","E", "F#","G#","A", "B", "C#","D#","E" ],
    "F":["F", "G", "A", "A#","C", "D", "E", "F", "G", "A", "A#","C", "D", "E", "F" ],
    "F#":["F#","G#","A#","B", "C#","D#","F", "F#","G#","A#","B", "C#","D#","F", "F#"],
    "G":["G", "A", "B", "C", "D", "E", "F#","G", "A", "B", "C", "D", "E", "F#","G" ],
    "G#":["G#","A#","C", "C#","D#","F", "G", "G#","A#","C", "C#","D#","F", "G", "G#"],
    "A":["A", "B", "C#","D", "E", "F#","G#","A", "B", "C#","D", "E", "F#","G#","A" ],
    "A#":["A#","C", "D", "D#","F", "G", "A", "A#","C", "D", "D#","F", "G", "A", "A#"],
    "B":["B", "C#","D#","E", "F#","G#","A#","B", "C#","D#","E", "F#","G#","A#","B" ],
}

to_flat  = {"C":"B","C#":"C","D":"C#","D#":"D","E":"D#","F":"E","F#":"F","G":"F#","G#":"G","A":"G#","A#":"A","B":"A#"}
to_sharp = {"C":"C#","C#":"D","D":"D#","D#":"E","E":"F","F":"F#","F#":"G","G":"G#","G#":"A","A":"A#","A#":"B","B":"C"}
oct_min=2
oct_max=7

# generate chords with given <root> starting with octave <oct>
def gen(root, tpl, oct):
    chord = [(root,oct)]
    for fn in tpl:
        note = fn(root)
        o = oct+1 if ICO(root,note) else oct
        if o > oct_max:
            return []
        chord.append((note, o))
    return [chord] + gen(root, tpl, oct + 1)

# generate <N>th inversion of <chord>
def inv(name, chord, N):
    ch = list(chord)
    for i in range(N):
        if ch[i][1] == oct_max:
            return []
        ch[i] = (ch[i][0], ch[i][1] + 1)
    ostr = ch[0][0]+str(name)
    if N != 0:
        ostr += "|" + ch[N][0]
    ostr += ' ' + str(ch[0][1]) + ' ' + ' '.join(map(lambda tup: "".join(map(str,tup)), ch))
    print(ostr)
    return ch

# generate all possible inversions of <chord>
def gen_inv(name, chord):
    return map(lambda i: inv(name, chord,i), range(len(chord)))

# is notes cross octave
def ICO(lnote,rnote):
    halftones = ["C","C#","D","D#","E","F","F#","G","G#","A","A#","B"]
    l_idx = halftones.index(lnote)
    r_idx = halftones.index(rnote)
    return l_idx > r_idx

if __name__ == "__main__":
    tpls = {
        "":     [lambda r: maj_scale[r][2], lambda r: maj_scale[r][4]],
        "m":    [lambda r: to_flat[maj_scale[r][2]], lambda r: maj_scale[r][4]],
        "7":    [lambda r: maj_scale[r][2], lambda r: maj_scale[r][4], lambda r: to_flat[maj_scale[r][6]]],
        "maj7": [lambda r: maj_scale[r][2], lambda r: maj_scale[r][4], lambda r: maj_scale[r][6]],
        "m7":   [lambda r: to_flat[maj_scale[r][2]], lambda r: maj_scale[r][4], lambda r: to_flat[maj_scale[r][6]]],
        "5":    [lambda r: maj_scale[r][4]],
        "sus2": [lambda r: maj_scale[r][1], lambda r: maj_scale[r][4]],
        "sus4": [lambda r: maj_scale[r][3], lambda r: maj_scale[r][4]],
        "m7b5": [lambda r: to_flat[maj_scale[r][2]], lambda r: to_flat[maj_scale[r][4]], lambda r: to_flat[maj_scale[r][6]]],
        "aug":  [lambda r: maj_scale[r][2], lambda r: to_sharp[maj_scale[r][4]]],
        "dim":  [lambda r: to_flat[maj_scale[r][2]], lambda r: to_flat[maj_scale[r][4]]],
        "dim7": [lambda r: to_flat[maj_scale[r][2]], lambda r: to_flat[maj_scale[r][4]], lambda r: to_flat[to_flat[maj_scale[r][6]]]],
    }
    for T in tpls.keys():
        for root in maj_scale.keys():
            map(lambda ch: gen_inv(T,ch), gen(root, tpls[T], oct_min))
