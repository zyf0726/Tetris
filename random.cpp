#ifndef SINGLEFILE
#include "shared.h"
#endif

int l_rand(int a, int b)
{
    return (std::uniform_int_distribution<int>(a, b))(RAND);
}

int b_rand()
{

    return l_rand(0, 2);
}

float f_rand()
{
    return (std::uniform_real_distribution<float>()).operator()(RAND);
}

mt19937_64 RAND((random_device())());
