#include <iostream>
#include <ctime>

#ifndef RAND_ENGINE_H
#define RAND_ENGINE_H
#include <random>


class RandEngine {
    private:
        RandEngine() {}

    public:
        static void initialize(long seed);
        inline static std::mt19937& engine() {
            if(!s_initialized) {
                //std::cerr << "Warning: Random engine was not initialized. Seeding random generator with 0." << std::endl;
                initialize(1);
            }
            return s_eng;
        }

    private:
        static long s_seed;
        static bool s_initialized;
        static std::mt19937 s_eng;
};


#endif
