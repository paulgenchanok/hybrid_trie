#!/usr/bin/env bash

# A wonderful TWO liner to run the CMake commands to build

cmake -H. -Bbuild
cmake --build build
