#!/bin/bash

set -e

dir=$(dirname "${BASH_SOURCE[0]}")
cd "${dir}"

rsync -avz --progress -e "ssh" */ --exclude=.vscode/ipch --exclude=_build/ --exclude=build/ --exclude=.xvi.build/ fraser.gordon@10.30.1.253:~/Projects.rsync/XVI/Sprockets

