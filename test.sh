#!/bin/bash

# just call ./test.sh [-O]
# will update the results file and print it

# exporting variable envirionment 
export PEEPDIR=~/2016-mcgill1/comp520/assignments/peephole/peephole/

if [[ $1 == "-O" ]]; then
	rm -f opt.size
else
	rm -f no-opt.size
fi

./joosc $1 PeepholeBenchmarks/bench01/*.java
./joosc $1 PeepholeBenchmarks/bench02/*java
./joosc $1 PeepholeBenchmarks/bench03/*java PeepholeBenchmarks/bench03/imports.joos
./joosc $1 PeepholeBenchmarks/bench04/*java
./joosc $1 PeepholeBenchmarks/bench05/*java
./joosc $1 PeepholeBenchmarks/bench06/*java
./joosc $1 PeepholeBenchmarks/bench07/*java

if [[ $1 == "-O" ]]; then
	awk '{sum += $3} END {print sum}' opt.size >> opt.results
	echo "opt results: "
	tail opt.results
else
	awk '{sum += $3} END {print sum}' no-opt.size >> no-opt.results
	echo "no-opt results: "
	tail no-opt.results
fi
