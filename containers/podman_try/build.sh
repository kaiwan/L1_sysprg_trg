#!/bin/bash

# Container 'Build' script wrapper

podman build -t my_alpine_pod1 . && echo "
Podman built successfully, run it" \
 || echo "
 *** Podman build failed (stat=$?)"

