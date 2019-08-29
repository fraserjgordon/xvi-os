#!/bin/bash

rsync -avz --progress -e "ssh" */ --exclude=_build fraser@fractal:~/Projects/XVI/Sprockets

