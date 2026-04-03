#!/bin/sh
# Run a single assembler test.
# Usage: run_one.sh <exe> positive|negative <input_file>
exe="$1"
type="$2"
in_file="$3"

result="$("$exe" "$in_file" 2>&1)"

case "$type" in
    positive)
        expected="$(cat "${in_file%.in}.out")"
        ;;
    negative)
        expected="${in_file}$(cat "${in_file%.in}.err")"
        ;;
    *)
        printf "%s: unknown test type: %s\n" "$0" "$type" >&2
        exit 1
        ;;
esac

if [ "$result" = "$expected" ]; then
    exit 0
else
    printf "expected: %s\ngot:      %s\n" "$expected" "$result"
    exit 1
fi
