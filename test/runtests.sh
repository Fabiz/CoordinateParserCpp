echo build...
g++ -std=c++11 -o parsertests tests.cpp

echo run tests...
./parsertests

echo cleanup
rm parsertests
