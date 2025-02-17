#!/bin/bash

parabuild \
    . \
    main \
    -t gemm.cpp.template \
    -w workspaces \
    --data-file parabuild_example/data.json \
    --init-bash-script="" \
    --compile-bash-script="make main -B -j" \
    --run-bash-script="python test/gemm_test.py --ignore_submit" \
    -j 1 \
    --run-in-place \
    --seperate-template \
    --panic-on-compile-error \
    --format-output \
    --makefile
