#!/bin/bash

# Define paths
WAL_PATH="/tmp/docs-db/.internal_storage/wal.log"
SEGMENT_BASE="/tmp/docs-db/.internal_storage/segments"

# Create base directory for WAL_PATH if it doesn't exist
WAL_DIR=$(dirname "$WAL_PATH")
if [ ! -d "$WAL_DIR" ]; then
  echo "Creating directory for WAL_PATH: $WAL_DIR"
  mkdir -p "$WAL_DIR"
fi

# Create WAL_PATH file if it doesn't exist
if [ ! -f "$WAL_PATH" ]; then
  echo "Creating WAL_PATH file: $WAL_PATH"
  touch "$WAL_PATH"
fi

# Create SEGMENT_BASE directory if it doesn't exist
if [ ! -d "$SEGMENT_BASE" ]; then
  echo "Creating SEGMENT_BASE directory: $SEGMENT_BASE"
  mkdir -p "$SEGMENT_BASE"
fi

echo "Setup complete."
