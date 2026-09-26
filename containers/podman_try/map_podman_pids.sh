#!/bin/bash
# Podman demo

# Check if a container name or ID was provided
if [ -z "$1" ]; then
    echo "Usage: $0 <container_name_or_id>"
    echo "Example: $0 elegant_hawking"
    exit 1
fi

CONTAINER="$1"

# Verify that the container is actually running
if ! podman ps --format "{{.Names}} {{.ID}}" | grep -q "$CONTAINER"; then
    echo "Error: Container '$CONTAINER' is not running or does not exist."
    exit 1
fi

echo "Mapping PIDs for container: $CONTAINER"
echo "========================================="

# Fetch and format the internal PID, Host PID, and execution command
podman top "$CONTAINER" pid hpid args 2>/dev/null

if [ $? -ne 0 ]; then
    echo "Failed to retrieve PID data. Ensure you have the required permissions."
fi

echo "
FYI:
To see all the *namespaces* associated with the container process, do this on the host:
 sudo ls -l /proc/HPID/ns"
