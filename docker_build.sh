#!/bin/bash

SCRIPTPATH="$(dirname "$0")"
cd "$SCRIPTPATH" || exit 255

if [ "$1" == "--retag" ]; then
    docker build -t cppinecone-build .
fi

docker run --mount type=bind,source="$(pwd)",target=/app -p 8000:8000 -it cppinecone-build /app/mkdocs.sh