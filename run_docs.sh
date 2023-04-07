#!/bin/bash

SCRIPTPATH="$(dirname "$0")"
cd "$SCRIPTPATH" || exit 255

SHOULD_DOXYGEN=1
SHOULD_BUILD=0

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
        echo "  -b, --build             Build artifacts rather than serving."
        ;;
    -sd | --skip-doxygen)
        SHOULD_DOXYGEN=0
        ;;
    -b | --build)
        SHOULD_BUILD=1
        ;;
    *) break ;;
    esac
    shift
done

if [ $SHOULD_BUILD -eq 1 ]; then
    doxygen Doxyfile
elif [ $SHOULD_DOXYGEN -eq 1 ]; then
    doxygen Doxyfile
    watch_doxygen &
fi

if [ $SHOULD_BUILD -eq 1 ]; then
    cp -R doxygen docs/
    mkdocs build
else
    mkdocs serve -a 0.0.0.0:8000
fi
