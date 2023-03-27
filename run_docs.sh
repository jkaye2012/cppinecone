#!/bin/bash
set -eo pipefail

SCRIPTPATH="$(dirname "$0")"
cd "$SCRIPTPATH" || exit 255

SHOULD_RETAG=0

while (("$#")); do
    case "$1" in
    -h | --help)
        echo "Usage: $0 [options]"
        echo "Options:"
        echo "  -h, --help      Show this help message and exit."
        echo "  -r, --retag     Rebuild and tag the Docker image locally."
        exit 0
        ;;
    -r | --retag)
        SHOULD_RETAG=1
        ;;
    *) break ;;
    esac
    shift
done

if [ $SHOULD_RETAG -eq 1 ]; then
    docker build -t cppinecone-build .
fi

# shellcheck disable=SC2068
docker run --mount type=bind,source="$(pwd)",target=/app -p 8000:8000 -it cppinecone-build /app/mkdocs.sh $@
