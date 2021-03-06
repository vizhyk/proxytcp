#!/bin/bash
set -e
# set -o pipefail
./../../bin/proxytcp --mode capture --output "$1/proxytcp/bin/meme.pcap" --port 1080 &
CLOCK_PID=$!
sleep 1

echo "" > log_file.txt
curl  --socks5-hostname localhost:1080 https://www.google.com/ | grep "google.com" >> log_file.txt
curl  --socks5-hostname localhost:1080 https://www.stackoverflow.com/ | grep "stackoverflow.com" >> log_file.txt
curl  --socks5-hostname localhost:1080 https://en.lyrsense.com/ | grep "en.lyrsense.com" >> log_file.txt
curl  --socks5-hostname localhost:1080 https://football.ua/ | grep "football.ua" >> log_file.txt
trap 'kill -2 $CLOCK_PID' EXIT
ls