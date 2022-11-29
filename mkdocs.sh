#!/bin/bash

SCRIPTPATH="$(dirname "$0")"
cd "$SCRIPTPATH" || exit 255

doxygen Doxyfile
mkdocs serve -a 0.0.0.0:8000