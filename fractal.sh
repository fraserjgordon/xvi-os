#!/bin/bash

set -e

ssh fraser@fractal -- "cd Projects/XVI/Toolchains" "&&" "$@"
