#pragma once

#include <dlib/clustering.h>
#include <dlib/rand.h>

using namespace dlib;

using sample_type = matrix<double,7,1>;
using full_sample_type = matrix<double,8,1>;
// using kernel_type = radial_basis_kernel<sample_type>;

// const double norm = 1'000'000'000;

sample_type convert_to_sample (const full_sample_type& f)
{
    static const double norm = 1'000'000'000;
    sample_type m;
    for(std::size_t i = 0;i<6;++i)
    {
        if(i == 3) m(i) = f(i)/norm;
        else m(i) = f(i);
    }
    if(f(6) == f(7) || f(6) == 1) m(6) = 0;
    else m(6) = 1;
    return m;
}