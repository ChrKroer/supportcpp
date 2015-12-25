#! /bin/bash

g++ -c --std=c++11 filewriter.cpp
g++ -c --std=c++11  systemresourcehandler.cpp
g++ -c --std=c++11  stringfunctions.cpp
g++ -c --std=c++11  -I ~/Applications/IBM/ILOG/CPLEX_Studio124/cplex/include -I ~/Applications/IBM/ILOG/CPLEX_Studio124/concert/include/ -I ~/install/ -L ~/Applications/IBM/ILOG/CPLEX_Studio124/cplex/lib/x86-64_darwin9_gcc4.0/static_pic -L ~/Applications/IBM/ILOG/CPLEX_Studio124/concert/lib/x86-64_darwin9_gcc4.0/static_pic -L /opt/local/lib -DIL_STD -m64 -c -fPIC -fexceptions  extensiveformsolver.cpp
g++ -c --std=c++11  game.cpp

ar rvs support.a filewriter.o systemresourcehandler.o stringfunctions.o extensiveformsolver.o game.o