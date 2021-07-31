#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright 2019 Volodymyr Kononenko
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


import argparse
import os,glob
import subprocess
import csv
from math import log10
import numpy as np

def psnr(A, ref):
    if A.shape != ref.shape:
        raise ValueError("Chromagrams sizes not equal")
    mse = np.mean((A - ref) ** 2)
    if(mse == 0):
        raise ValueError("Chord is too perfect")
    p = 10 * log10(1 / mse)
    s = 10 * log10(np.sum(A  ** 2) / A.size / mse)
    return (p, s)

def main(args):
    proc = subprocess.Popen([args.client_path, "--tplsdump"], stdout=subprocess.PIPE)
    output = proc.communicate()[0].decode('ascii')
    tpls_reader = csv.reader(output.splitlines(), delimiter=",")
    tpls = {}
    P_results = []
    S_results = []
    # drop all after '/': ignore bass info
    for row in tpls_reader:
        tpls[row[0].split('/')[0]] = np.array(row[13:], dtype=float)
    for file_paths in map(glob.glob, args.input_files):
        for file_path in file_paths:
            filename = os.path.basename(file_path)
            chord_name = filename[:-4]
            chord_name = chord_name.split('|')[0] # ignore bass info
            if not filename.endswith((".wav",".aif")) or chord_name not in tpls:
                continue
            #TODO: handle errors of client execution
            proc = subprocess.Popen([args.client_path, "--pcpcsv", file_path], stdout=subprocess.PIPE)
            output = proc.communicate()[0].decode('ascii')
            pcp_reader = csv.reader(output.splitlines(), delimiter=",")
            chromagram = np.array(list(pcp_reader), dtype=float, ndmin=2)
            chromagram = chromagram[:,12:] # use only treble part of PCP vector
            # scale template to [0,1], so it matches PCP range
            tpl = tpls[chord_name] / tpls[chord_name].max()
            tpl_ref = np.array([tpl] * len(chromagram), dtype=float, ndmin=2)
            (P, S) = psnr(chromagram, tpl_ref)
            print ("{0},{1:.2f},{2:.2f},{3}".format(chord_name, P, S, file_path))
            P_results.append(P)
            S_results.append(S)
    if not P_results:
        print ("no corresponding templates found")
        return
    for (nm,fn) in [("AVG",np.mean), ("MED",np.median), ("STD",np.std)]:
        print (nm+",{0:.2f},{1:.2f}".format(fn(P_results), fn(S_results)))

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("client_path", help="Path to lmclient")
    parser.add_argument("input_files", help="Input test files", nargs="+")
    args = parser.parse_args()

    main(args)
