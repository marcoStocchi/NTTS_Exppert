#include "rttg.hpp"
#include "rttg_types.hpp"

namespace confly
{
	typedef micro<PARAMS>
		micro_type;

	typedef hypercube<PARAMS>
		hypercube_type;

    // -----------------------------------------
    // allocate/hold singletons

    // prng instance
    mersenne_twister
        _DRE;

    // differential privacy
    double 
        _DPFmu(0.0),
        _DPFb(0.0),
        _DPGeps(0.0);

    // static response members
    laplace_distribution
        response::_Laplace(0.0, 0.0);
    geometric_distribution
        response::_Geometric(0.0, 0.0);
    ptable 
        response::_PTable;

    // static traversed_response members
    size_t 
        traversed_response::_MaxSz=0;

    // static hypercube's traversing 
    // nested map
    template <class... _Types>
        typename hypercube<_Types...>
            ::traversing_nested_map_type
        hypercube<_Types...>::_Trv;

    // -----------------------------------------
    // implement plugin functions

    void import_ptable(
        const ptable& _Ptab)
    {
        response::set_ptable(
            _Ptab);
    }

    void import_noise_params(
        const double& _cDPFmu,
        const double& _cDPFb,
        const double& _cDPGeps)
    {
        _DPFmu=_cDPFmu;
        _DPFb=_cDPFb;
        _DPGeps=_cDPGeps;

        response::set_laplace_parm(
            _DPFmu, _DPFb);

        response::set_geometr_parm(
            _DPGeps);
    }

    void import_size(
        const size_t& _Sz)
    {// import number of cubes
        traversed_response
            ::_MaxSz=_Sz;
    }

    void seed_prng(
        const size_t& _Seed)
    {
        _DRE.seed(_Seed);
    }


	auto make_micro()
		->micro_base*
	{
		return new 
            micro_type();
	}
	
	auto make_hypercube()
		->hypercube_base*
	{
		return new 
            hypercube_type();
	}

    bool sample_micro(
        micro_base* _Micro, 
        hypercube_base* _Cube,
        const double& _Rate,
        const std::string& _Fname)
    {
        // call stack:
        // main->service
        // ->plugin->sample_micro

        micro_type* _CastMicro=
            dynamic_cast<micro_type*>
                (_Micro);
        hypercube_type* _CastCube=
            dynamic_cast<hypercube_type*>
                (_Cube);
                
        if (! _CastMicro ||
            ! _CastCube)
        {
            flush_msg(std::cout,
                M_ERROR,
                M_PLUGIN,
                __func__);

            return false;
        }

        // protect against wrong
        // sampling rate parameter
        if (_Rate >1.0 || 
            _Rate <0.0)
        {
            flush_msg(std::cout,
                M_ERROR,
                M_SAMPLE,
                M_RATE,
                __func__);

            return false;
        }

        debug_msg<3>(_CastMicro);
        debug_msg<3>(_CastCube);

        // get ref to vector tuples
        const auto& _Vec(
            _CastMicro->get());

        // get ref to metadata
        const auto& _Meta(
            _CastMicro->get_meta());

        // get mask
        const auto& _Mask(
            _CastMicro->get_mask());

        // get sample size
        const size_t 
            _RecordsSz(_Vec.size()),
            _SampleSz(std::round(
                static_cast<double>(
                _RecordsSz)*_Rate));

        // hold indeces here
        std::set<size_t> 
            _Indeces;

        // draw _SampleSz vector indeces
        // between 0 and vector.size-1
        unif<size_t> _Unif; _Unif
            .draw(0, _RecordsSz-1, 
                _SampleSz, _Indeces); 

        debug_msg<3>(_Indeces.size());

        // aggregate hypercube
        for (auto I=_Indeces.cbegin(),
            E=_Indeces.cend(); I!=E; 
                ++I)
        {
            _CastCube->update(
                _Mask, _Vec[*I]);
        }

        // finalize/write cube
        std::ofstream 
            _Fout(_Fname);

        // interpret traversing size
        // equal to zero as "--no-traverse"
        const bool 
            _BNoTrav(
            traversed_response
            ::_MaxSz==0);

        _CastCube->write(
            _Meta, _Fout,
            _BNoTrav);

        return true;
    }

    bool write_traversing(
        micro_base* _Micro, 
        const std::string& _cPath)
    {
        // call stack:
        // main->service
        // ->plugin->sample_micro

        micro_type* _CastMicro=
            dynamic_cast<micro_type*>
                (_Micro);
                
        if (! _CastMicro)
        {
            flush_msg(std::cout,
                M_ERROR,
                M_PLUGIN,
                __func__);

            return false;
        }

        // get ref to metadata
        const auto& _Meta(
            _CastMicro->get_meta());

        hypercube_type
            ::write_traversing(
            _Meta, _cPath);

        return true;
    }
}
