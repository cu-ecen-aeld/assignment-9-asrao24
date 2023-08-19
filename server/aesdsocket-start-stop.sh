#!/bin/sh

# Function to start aesdsocket in daemon mode
start() {
  echo "Starting aesdsocket in daemon mode..."
  ./usr/bin/aesdsocket -d
}

# Function to stop aesdsocket
stop() {
  echo "Stopping aesdsocket..."
  pkill -TERM 'aesdsocket'
  # Add any additional cleanup steps here
}

# Main script logic
case "$1" in
  start)
    start
    ;;
  stop)
    stop
    ;;
  *)
    echo "Usage: $0 {start|stop}"
    exit 1
    ;;
esac

exit 0

