#ifndef SINGLEFILE

#include "shared.h"
#endif

int l_rand (int a, int b, struct options * ) {
    return (std::uniform_int_distribution<int>(a, b))(RAND);
}

int b_rand (struct options * opt) {

    return l_rand(0, 2, opt);
}

float f_rand (struct options * opt) {
    return (std::uniform_real_distribution<float>()).operator()(RAND);
}

mt19937_64 RAND((random_device())());
void initalize_rng (struct options* ) { }