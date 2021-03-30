#!/bin/bash

PATH_TO_PROXYTCP=$1

sudo sh TLSTraffic/defaultMode.sh $PATH_TO_PROXYTCP
sudo sh TLSTraffic/captureMode.sh $PATH_TO_PROXYTCP
sudo sh TLSTraffic/failedBranches.sh

