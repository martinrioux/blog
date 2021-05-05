#!/bin/bash
# Local Jekyll test via docker
docker run -it --rm -v "$PWD":/usr/src/app -p "4000:4000" starefossen/github-pages

