

g++ -std=c++17 -O2 -c dsl-test.cpp -o dsl-test.o -I../include 
g++ -std=c++17 -O2 dsl-test.o nanobench.o -L../cmake-build-debug -ltrans-dsl
