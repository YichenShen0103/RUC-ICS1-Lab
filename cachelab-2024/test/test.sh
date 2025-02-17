#!/bin/bash

g++ -fsyntax-only -Wall -O0 -g -std=c++17 -fsanitize=address -fno-omit-frame-pointer -Itest/fake_include test/legality_test_example1.cpp > /dev/null 2>&1 &
pid1=$!

g++ -fsyntax-only -Wall -O0 -g -std=c++17 -fsanitize=address -fno-omit-frame-pointer -Itest/fake_include test/legality_test_example2.cpp > /dev/null 2>&1 &
pid2=$!

g++ -fsyntax-only -Wall -O0 -g -std=c++17 -fsanitize=address -fno-omit-frame-pointer -Itest/fake_include test/legality_test_example3.cpp > /dev/null 2>&1 &
pid3=$!

g++ -fsyntax-only -Wall -O0 -g -std=c++17 -fsanitize=address -fno-omit-frame-pointer -Itest/fake_include test/legality_test_example4.cpp > /dev/null 2>&1 &
pid4=$!

wait $pid1
status1=$?

wait $pid2
status2=$?

wait $pid3
status3=$?

wait $pid4
status4=$?

if [ $status1 -eq 0 ] && [ $status2 -eq 1 ] && [ $status3 -eq 1 ] && [ $status4 -eq 1 ]; then
    exit 0
else
    exit 1
fi