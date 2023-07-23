#!/usr/bin/env bash

find headers -type f -name '*.c' -o -name '*.cpp' -o -name '*.cc' -o -name '*.h' | xargs clang-format -i
