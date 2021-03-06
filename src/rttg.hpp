#pragma once
#include "random_draw.hpp"
#include "ptable.hpp"
#include "micro.hpp"
#include "hypercube.hpp"


namespace confly
{
    void import_ptable(
        const ptable&);

    void import_noise_params(
        const double& _cDPFmu,
        const double& _cDPFb,
        const double& _cDPGeps);

    void import_size(
        const size_t& _Sz);

    void seed_prng(
        const size_t& _Seed);

    auto make_micro()
        ->micro_base*;
    
    auto make_hypercube()
        ->hypercube_base*;

    bool sample_micro(
        micro_base* _Micro, 
        hypercube_base* _Cube,
        const double& _Rate,
        const std::string& _Fname);
    
    bool write_traversing(
        micro_base* _Micro, 
        const std::string& _cPath);
}