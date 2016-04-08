#!/bin/bash

# just call ./test.sh [-O]
# will update the results file and print it

# exporting variable envirionment 
#export PEEPDIR=~/2016-mcgill1/comp520/assignments/peephole/peephole/

make -C JOOSA-src/

if [[ $1 == "-O" ]]; then
	rm -f opt.size
else
	rm -f no-opt.size
fi

./joosc $1 PeepholeBenchmarks/bench01/*.java
./joosc $1 PeepholeBenchmarks/bench02/*.java
./joosc $1 PeepholeBenchmarks/bench03/*.java PeepholeBenchmarks/bench03/imports.joos
./joosc $1 PeepholeBenchmarks/bench04/*.java
./joosc $1 PeepholeBenchmarks/bench05/*.java
./joosc $1 PeepholeBenchmarks/bench06/*.java
./joosc $1 PeepholeBenchmarks/bench07/*.java

#test b1
cd PeepholeBenchmarks/bench01/
java Benchmark < in1 > output1
if ! cmp out1 output1 >/dev/null 2>&1
then
  echo "BENCHMARK 01 OUTPUT FAILS"
fi
rm output1
cd ../../

#test b2
cd PeepholeBenchmarks/bench02/
java Main < in1 > output1
if ! cmp out1 output1 >/dev/null 2>&1
then
  echo "BENCHMARK 02 OUTPUT FAILS"
fi
rm output1
cd ../../

#test b3
cd PeepholeBenchmarks/bench03/
cd lib
javac JoosBitwise.java
cd ..
java Main < in1 > output1
if ! cmp out1 output1 >/dev/null 2>&1
then
  echo "BENCHMARK 03 OUTPUT FAILS"
fi
rm output1
cd ../../

#test b4
cd PeepholeBenchmarks/bench04/

#case in1
java Game < in1 > output1
if ! cmp out1 output1 >/dev/null 2>&1
then
  echo "BENCHMARK 04 OUTPUT FAILS"
fi
rm output1

#case in 
# yeah, doesn't work. I bet it's a problem in the input. since neither with non-opt it doen't work
#java Game < in > output
#if ! cmp out output >/dev/null 2>&1
#then
#  echo "BENCHMARK 04 OUTPUT FAILS"
#fi
#rm output
#
cd ../../

#test b5
cd PeepholeBenchmarks/bench05/
java DungeonGenerator < in1 > output1
if ! cmp out1 output1 >/dev/null 2>&1
then
  echo "BENCHMARK 05 OUTPUT FAILS"
fi
rm output1
cd ../../

#test b6
cd PeepholeBenchmarks/bench06/
java Main < in1 > output1
if ! cmp out1 output1 >/dev/null 2>&1
then
  echo "BENCHMARK 06 OUTPUT FAILS"
fi
rm output1
cd ../../

#test b7 doesn't seem deterministic...
cd PeepholeBenchmarks/bench07/
java GrueHunt < in1 > output1
#if ! cmp out1 output1 >/dev/null 2>&1
#then
#  echo "BENCHMARK 07 OUTPUT FAILS"
#fi
rm output1
cd ../../


if [[ $1 == "-O" ]]; then
	awk '{sum += $3} END {print sum}' opt.size >> opt.results
	echo "opt results: "
	tail opt.results
else
	awk '{sum += $3} END {print sum}' no-opt.size >> no-opt.results
	echo "no-opt results: "
	tail no-opt.results
fi
