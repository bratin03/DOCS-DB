#!/bin/bash

# Define paths
BASE_DIR="/tmp/docs-db"
WAL_PATH="$BASE_DIR/.internal_storage/wal.log"
SEGMENT_BASE="$BASE_DIR/.internal_storage/segments"

# Clean up: Remove the entire docs-db directory if it exists
if [ -d "$BASE_DIR" ]; then
  echo "Removing existing docs-db directory: $BASE_DIR"
  rm -rf "$BASE_DIR"
fi

# Create base directory for WAL_PATH if it doesn't exist
WAL_DIR=$(dirname "$WAL_PATH")
if [ ! -d "$WAL_DIR" ]; then
  echo "Creating directory for WAL_PATH: $WAL_DIR"
  mkdir -p "$WAL_DIR"
fi

# Create or empty WAL_PATH file
if [ ! -f "$WAL_PATH" ]; then
  echo "Creating WAL_PATH file: $WAL_PATH"
  touch "$WAL_PATH"
else
  echo "Emptying existing WAL_PATH file: $WAL_PATH"
  > "$WAL_PATH"
fi

# Create SEGMENT_BASE directory if it doesn't exist
if [ ! -d "$SEGMENT_BASE" ]; then
  echo "Creating SEGMENT_BASE directory: $SEGMENT_BASE"
  mkdir -p "$SEGMENT_BASE"
fi

echo "Setup complete."
