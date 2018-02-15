#! /bin/sh

if [ "$#" -lt 3 ]; then
	echo "usage: scripts.sh <bc> <test_output1> <test_output2>"
	exit 1
fi

set -e

bc="$1"
shift

out1="$1"
shift

out2="$1"
shift

script="$0"

testdir=$(dirname "${script}")

for s in $testdir/scripts/*.bc; do

	rm -rf "$out1"
	rm -rf "$out2"

	echo "halt" | bc -lq "$s" > "$out1"
	echo "halt" | "$bc" -lq "$s" > "$out2"

	diff "$out1" "$out2"

done

set +e

# TODO: Read tests
# TODO: Lex errors
# TODO: Parse errors
# TODO: VM errors
# TODO: Math errors
# TODO: POSIX warnings
# TODO: POSIX errors