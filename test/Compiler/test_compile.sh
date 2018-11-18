#!/bin/bash
try() {
  expected="$1"
  input="$2"

  echo "$input" | ./fcc > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" != "$expected" ]; then
    echo "expected $expected, but got $actual"
    exit 1
  fi
}

try 0 0
try 123 123

echo "passed all tests"
