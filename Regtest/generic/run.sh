#!/bin/bash
set -e
set -o pipefail
../bin/proxytcp &
CLOCK_PID=$!
sleep 1
GOOGLE='google'
STACKOVERFLOW='stackoverflow'
curl  --socks5-hostname localhost:1080 https://google.com/ | grep $GOOGLE >> log_file.txt
curl  --socks5-hostname localhost:1080 https://stackoverflow.com/ | grep $STACKOVERFLOW >> log_file.txt
trap 'kill -9 $CLOCK_PID' EXIT
