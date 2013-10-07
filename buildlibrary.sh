#! /bin/bash

g++ -c --std=c++11 filewriter.cpp
g++ -c --std=c++11  systemresourcehandler.cpp

ar rvs support.a filewriter.o systemresourcehandler.o