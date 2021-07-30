#!/usr/bin/env python3
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
# Usage:
#
#   slicer.py <track> <annotation>
#
# Where <annotation> is text file of format
#   <timestamp> <timestamp> <chord>
#
# Splits <track> into slices, each begin and end at timestamps from <annotation>,
# naming output slices by chord name from <annotation> and writing them to
# filesystem along with <track>
#
import sys, os
import soundfile as sf

if __name__ == "__main__":
    if len(sys.argv) != 3:
        exit(-1)
    track_name = sys.argv[1]
    info = sf.info(track_name)
    slices = []
    with open(sys.argv[2], "r") as slice_data:
        slices = [line.split() for line in slice_data]
    format_width = len(str(len(slices)))
    dir2format = "{0:0" + str(format_width) + "}"
    dirname = track_name + "_slices"
    os.mkdir(dirname)
    i = 0
    for slice in slices:
        startIdx = round(float(slice[0]) * info.samplerate)
        endIdx = round(float(slice[1]) * info.samplerate)
        name = slice[2].replace("/", "|", 1)
        data, dummy = sf.read(track_name, start=startIdx, stop=endIdx+1)
        dir2name = os.path.join(dirname, dir2format.format(i))
        os.mkdir(dir2name)
        filename = os.path.join(dir2name, name + '.' + info.format)
        sf.write(filename, data, samplerate=info.samplerate,format=info.format)
        i += 1
    print("processed {0} slices".format(i))
