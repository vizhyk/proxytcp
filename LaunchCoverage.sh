#!/bin/bash

PATH_TO_PROXYTCP=$1

sudo cmake .
sudo cmake --build .
sudo cmake -DCMAKE_BUILD_TYPE=Debug .

./bin/proxytcp_tests

cd Tests/RegressionTests/
./LaunchAllTests.sh
cd ../..

mkdir CodeCoverage
cd CodeCoverage
mkdir html

sudo lcov --rc lcov_branch_coverage=1 -d "../CMakeFiles/" --capture -o proxytcp.info -c
sudo lcov --rc lcov_branch_coverage=1 --remove proxytcp.info "$PATH_TO_PROXYTCP/proxytcp/Tests/UnitTests/*" "$PATH_TO_PROXYTCP/proxytcp/googletest/*" "/usr/include/*" -o filtered.info
sudo genhtml -o html filtered.info

cd ..

