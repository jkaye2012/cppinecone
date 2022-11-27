#!/bin/bash

docker run --mount type=bind,source="$(pwd)",target=/app -p 8000:8000 -it cppinecone-build mkdocs serve -a 0.0.0.0:8000