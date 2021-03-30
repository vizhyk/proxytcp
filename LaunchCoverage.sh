#!/bin/bash

PATH_TO_PROXYTCP=$1

sudo cmake -DCMAKE_BUILD_TYPE=Debug .
sudo cmake --build .

./bin/proxytcp_tests

cd Tests/RegressionTests/
./LaunchAllTests.sh $PATH_TO_PROXYTCP
cd ../..

mkdir CodeCoverage
cd CodeCoverage
mkdir html

sudo lcov -d "../CMakeFiles/" --capture -o proxytcp.info -c
sudo lcov --remove proxytcp.info "$PATH_TO_PROXYTCP/proxytcp/Tests/UnitTests/*" "$PATH_TO_PROXYTCP/proxytcp/googletest/*" "/usr/include/*" -o filtered.info
sudo genhtml -o html filtered.info

cd ..

