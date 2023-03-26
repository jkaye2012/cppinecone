#!/bin/bash

# TODO: set -eo pipefail

SCRIPTPATH="$(dirname "$0")"
cd "$SCRIPTPATH" || exit 255

SHOULD_DOXYGEN=1

function watch_doxygen() {
    while inotifywait -r -e modify include; do
        doxygen Doxyfile
    done
}

while (("$#")); do
    case "$1" in
    -h | --help)
        echo "Usage: $0 [options]"
        echo "Options:"
        echo "  -h, --help              Show this help message and exit."
        echo "  -sd, --skip-doxygen     Do not generate Doxygen documentation."
        ;;
    -sd | --skip-doxygen)
        SHOULD_DOXYGEN=0
        ;;
    *) break ;;
    esac
    shift
done

if [ $SHOULD_DOXYGEN -eq 1 ]; then
    doxygen Doxyfile
    watch_doxygen &
fi
mkdocs serve -a 0.0.0.0:8000
