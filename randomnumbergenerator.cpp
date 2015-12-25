#include <iostream>

#include "RandomNumberGenerator.h"

bool RandomNumberGenerator::s_initialized = false;
long RandomNumberGenerator::s_seed = 0;
std::mt19937 RandomNumberGenerator::s_eng;

void RandomNumberGenerator::initialize(long seed) {
    s_seed = seed;
    s_eng.seed(s_seed);
    srand(s_seed); // Just be sure...
    s_initialized = true;
}

