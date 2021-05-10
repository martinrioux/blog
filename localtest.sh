#!/bin/bash
# Local Jekyll test via docker
docker build . -t "martin.githubpage"
docker run -it --rm -v "$PWD":/usr/src/app -p "4000:4000" martin.githubpage
