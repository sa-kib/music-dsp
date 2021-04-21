#!/usr/bin/env bash
# Reference:
#   https://scruss.com/blog/2017/12/19/synthesizing-simple-chords-with-sox/
#
# Read stdin line by line in form
# <chord> <octave> <note> ...
# Lines starting with # ignored
# Example:

#   Cm     2 C2  Eb2  G2
#   C/E    3 C3  E2   G2
#   # This is a comment
#   C7/Bb  3 C3  E3   G3  Bb2
#
# Generate files structure: oct<octave>/<chord>.wav

samplerate=44100
duration=2		# desired tracks duration in seconds
fade_out=.5		# duration * 0.25

# limits are slightly increased due to seq utility misbehave
delay=(0 0 0.051 0.11 0.151 0.21 0.251 0.31)

while read chord oct freq_list
do
  [[ -z $chord || $chord =~ ^# ]] && continue
  [ -d oct${oct} ] || mkdir oct${oct}
  [ -f "oct$oct/${chord}.wav" ] && echo "oct$oct/${chord}.wav already exist!" && exit 1
  freq_list=($(echo $freq_list))	# convert string to array
  d_off=${#freq_list[@]}
  sox -n \
    -r $samplerate -b 16 "oct$oct/${chord}.wav" \
    synth $(for f in ${freq_list[@]}; do echo -n "pl ${f} ";done) \
    delay $(seq -f "%.2f" 0 0.05 ${delay[$d_off]}) \
    remix - \
    fade h 0 $duration $fade_out \
    norm -1
done
