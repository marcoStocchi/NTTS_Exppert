#pragma once
#include <random>

namespace confly
{
	typedef std::mt19937 
		mersenne_twister;

	typedef std::minstd_rand0
		linear_congruential;

	extern mersenne_twister
		_DRE;


    inline void engine_seed(
        const size_t& _Seed)
    {
        _DRE.seed(_Seed);
    }

    inline auto engine_min()  
        ->size_t
    {// random engine min
        return _DRE.min();
    }

    inline auto engine_max()  
        ->size_t
    {
        return _DRE.max();
    }
}