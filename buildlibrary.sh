#! /bin/bash

g++ -c --std=c++11 filewriter.cpp
g++ -c --std=c++11  systemresourcehandler.cpp
g++ -c --std=c++11  stringfunctions.cpp

ar rvs support.a filewriter.o systemresourcehandler.o stringfunctions.o