

#g++-10 -std=c++20 -O2 -c nanobench.cpp -o nanobench.o -I../include -I../tests/pt
g++-10 -std=c++20 -O3 -c dsl-test.cpp -o dsl-test.o -I../include -I../tests/pt
g++-10 -std=c++20 -O3 dsl-test.o nanobench.o -L../cmake-build-release -ltrans-dsl
#g++ -std=c++17 -O2 -c dsl-test.cpp -o dsl-test.o -I../include -I../tests/pt
#g++ -std=c++17 -O2 dsl-test.o nanobench.o -L../cmake-build-release -ltrans-dsl
