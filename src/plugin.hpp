#pragma once
#include <unistd.h>
#include <dlfcn.h>

#include "def.hpp"
#include "shared.hpp"
#include "micro.hpp"
#include "hypercube.hpp"

#define rttg_types		"rttg_types"
#define rttg_lib        "libconfly_rttg"
#define rttg_dothpp     ".hpp"
#define rttg_dotso      ".so"

namespace confly
{
    // -------------------------------------
    // plugin mangled symbols

    static const char*
        sym_import_ptable=
        "_ZN6confly13import_"
		"ptableERKNS_6ptable"
		"E";

    static const char*
        sym_import_noise_params=
        "_ZN6confly19import_"
        "noise_paramsERKdS1_"
        "S1_";

    static const char*
        sym_import_size=
        "_ZN6confly11import_"
        "sizeERKm";

    static const char*
        sym_seed_prng=
        "_ZN6confly9seed_prn"
        "gERKm";

	static const char*
		sym_make_micro=
		"_ZN6confly10make_mi"
        "croEv";

	static const char*
		sym_make_hypercube=
		"_ZN6confly14make_hy"
        "percubeEv";

	static const char*
		sym_sample_micro=
        "_ZN6confly12sample_"
        "microEPNS_10micro_b"
        "aseEPNS_14hypercube"
        "_baseERKdRKNSt7__cx"
        "x1112basic_stringIc"
        "St11char_traitsIcES"
        "aIcEEE";
	static const char*
		sym_sample_micro_old=
        "_ZN6confly12sample_"
		"microEPNS_10micro_b"
		"aseEPNS_14hypercube"
		"_baseERKdRKSs";

	static const char*
		sym_write_traversing=
        "_ZN6confly16write_t"
        "raversingEPNS_10mic"
        "ro_baseERKNSt7__cxx"
        "1112basic_stringIcS"
        "t11char_traitsIcESa"
        "IcEEE";
	static const char*
		sym_write_traversing_old=
		"_ZN6confly16write_t"
		"raversingEPNS_10mic"
		"ro_baseERKSs";

    // -------------------------------------
    /// @brief plugin class
	struct plugin
		: enable_internal_error
	{// manage shared library

		/// @brief type of handle
		typedef void* 
			handle_type;

		/// @brief type of function
		/// import_ptable
        typedef void (*
            func_import_ptable) (
                const ptable&);

		/// @brief type of function
		/// import_noise_params
        typedef void (*
            func_import_noise_params) (
                const double&,
                const double&,
                const double&);

		/// @brief type of function
		/// import_size
        typedef void (*
            func_import_size) (
                const size_t&);

		/// @brief type of function
		/// seed_prng
        typedef void (*
            func_seed_prng) (
                const size_t&);

		/// @brief type of function
		/// make_micro
		typedef micro_base* (*
            func_make_micro)();

		/// @brief type of function
		/// make_hypercube
		typedef hypercube_base* (*
            func_make_hypercube)();

		/// @brief type of function
		/// sample_micro
        typedef bool (*
            func_sample_micro) (
                micro_base*,
                hypercube_base*,
                const double&,
                const std::string&);

		/// @brief type of function
		/// write_traversing
        typedef bool (*
            func_write_traversing) (
                micro_base*,
                const std::string&);

		/// @brief construct with parameters
		/// @param _cPathProject path to the
		/// project's folder as specified in
		/// the configuration.
		/// @param _cTypeCode identifying the
		/// types used to read the microdata 
		/// @param _cMask bit mask used to 
		/// flags fields that must be included
		/// as is in the hypercubes, or rather
		/// only their aggregation is needed.
		plugin(
			const std::string& 
                _cPathProject,
			const std::string& 
                _cTypeCode,
            const size_t& 
                _cMask)
			: _PathProject(
                _cPathProject)
			, _SOName(so_name(
				_cTypeCode))
            , _Mask(_cMask)
			, _Handle(0)
            , _FuncImportPtable(0)
            , _FuncImportNP(0)
            , _FuncImportSize(0)
            , _FuncSeedPrng(0)
			, _FuncMakeMicro(0)
			, _FuncMakeCube(0)
            , _FuncSampleMicro(0)
            , _FuncWriteTraversing(0)
		{
			open();
		}

		/// @brief close plugin
		~plugin()
		{
			close();
		}

		/// @brief open plugin
		bool open()
		{
			if (! open(_SOName))
			{// error set
				return false;
			}

			if(! get_procedure(
				_FuncImportPtable,
				sym_import_ptable))
			{
				return false;
			}

			else if(! get_procedure(
				_FuncImportNP,
				sym_import_noise_params))
			{
				return false;
			}

			else if(! get_procedure(
				_FuncImportSize,
				sym_import_size))
			{
				return false;
			}

			else if(! get_procedure(
				_FuncSeedPrng,
				sym_seed_prng))
			{
				return false;
			}

			else if(! get_procedure(
				_FuncMakeMicro,
				sym_make_micro))
			{
				return false;
			}

			else if(! get_procedure(
				_FuncMakeCube,
				sym_make_hypercube))
			{
				return false;
			}

			else if(! get_procedure(
				_FuncSampleMicro,
				sym_sample_micro) &&
				! get_procedure(
				_FuncSampleMicro,
				sym_sample_micro_old))
			{// both versions failed
				return false;
			}

			else if(! get_procedure(
				_FuncWriteTraversing,
				sym_write_traversing) &&
				! get_procedure(
				_FuncWriteTraversing,
				sym_write_traversing_old))
			{
				return false;
			}
			
			return good();
		}

		/// @brief cast pointer to
		/// plugin's import_ptable
		/// function.
        void import_ptable(
            const ptable& _Ptab)
        {
			if (! good())
			{
				fail(__func__);

				return;
			}

			func_import_ptable 
                _Cast=reinterpret_cast<
					func_import_ptable>(
                        _FuncImportPtable);

			_Cast(_Ptab);
        }

		/// @brief cast pointer to
		/// plugin's import_noise_params
		/// function.
        void import_noise_params(
            const double& _cDPFmu,
            const double& _cDPFb,
            const double& _cDPGeps)
        {
			if (! good())
			{
				fail(__func__);

				return;
			}

			func_import_noise_params
                _Cast=reinterpret_cast<
					func_import_noise_params>(
                        _FuncImportNP);

			_Cast(_cDPFmu, 
                _cDPFb, _cDPGeps);
        }

		/// @brief cast pointer to
		/// plugin's import_size
		/// function.
        void import_size(
            const size_t& _Sz)
        {
			if (! good())
			{
				fail(__func__);

				return;
			}

			func_import_size
                _Cast=reinterpret_cast<
					func_import_size>(
                        _FuncImportSize);

			_Cast(_Sz);
        }

		/// @brief cast pointer to
		/// plugin's seed_prng
		/// function.
        void seed_prng(
            const size_t& _Seed)
        {
			if (! good())
			{
				fail(__func__);

				return;
			}

			func_seed_prng
                _Cast=reinterpret_cast<
					func_seed_prng>(
                        _FuncSeedPrng);

			_Cast(_Seed);
        }

		/// @brief cast pointer to
		/// plugin's make_micro
		/// function.
		auto make_micro() 
			->micro_base*
		{
			micro_base* _Ptr=
                call_procedure<
				func_make_micro, 
				micro_base*>(
					_FuncMakeMicro);

            if (_Ptr)
            {
                _Ptr->set_mask(_Mask);
            }
            
            return _Ptr;
		}

		/// @brief cast pointer to
		/// plugin's make_hypercube
		/// function.
		auto make_hypercube() 
			->hypercube_base*
		{
			return call_procedure<
				func_make_hypercube, 
				hypercube_base*>(
					_FuncMakeCube);
		}

		/// @brief cast pointer to
		/// plugin's sample_micro
		/// function.
        bool sample_micro(
            micro_base* _Micro, 
            hypercube_base* _Cube,
            const double& _Rate,
            const std::string& _Fname)
        {
			if (! good())
			{
				fail(__func__);

				return 0;
			}

			func_sample_micro _Cast=
				reinterpret_cast<
					func_sample_micro>(
                        _FuncSampleMicro);

			return _Cast(
                _Micro, _Cube,
                _Rate, _Fname);
        }

		/// @brief cast pointer to
		/// plugin's write_traversing
		/// function.
        bool write_traversing(
            micro_base* _Micro, 
            const std::string& _Path)
        {
			if (! good())
			{
				fail(__func__);

				return 0;
			}

			func_write_traversing _Cast=
				reinterpret_cast<
					func_write_traversing>(
                        _FuncWriteTraversing);

			return _Cast(
                _Micro, _Path);
        }

		/// @brief create plugin.
		/// @param _PathProject path to the
		/// project's folder as specified in
		/// the configuration.
		/// @param _Types train of comma 
		/// separated types used to instantiate
		/// the template classes of the plugin.
		/// @param _TypeCode identifying the
		/// types used to read the microdata.
		/// @return true in case of success in
		/// creating the plugin, or false
		/// otherwise.
		static bool create(
			const std::string&
                _PathProject,
			const std::string&
                _Types,
			const std::string&
                _TypeCode)
		{
			if (! create_rttg_types(
				_PathProject,
				_Types))
			{
				return false;
			}

			if (! build(
				_TypeCode))
			{
				return false;
			}

			return true;
		}

	private:

		template <class _FuncTy, 
			class _ResTy> 
		auto call_procedure(
			handle_type& _Func)
			->_ResTy
		{
			if (! good())
			{
				fail(__func__);

				return 0;
			}

			_FuncTy _Cast=
				reinterpret_cast<
					_FuncTy>(_Func);

			return _Cast();
		}

		static auto name(
			const std::string& _Types)
			->std::string
		{// libconfly_rttg_iiiii
			std::ostringstream 
				_OSS; _OSS 
				<< rttg_lib << '_'
				<< _Types; 

			return _OSS.str();
		}

		static auto so_name(
			const std::string& _Types)
			->std::string
		{// libconfly_rttg_iiiii.so
			std::ostringstream 
				_OSS; _OSS 
				<< name(_Types)
				<< rttg_dotso;

			return _OSS.str();
		}

		static auto rttg_types_name()
			->std::string
		{// rttg_types.hpp
			std::ostringstream 
				_OSS; _OSS 
				<< rttg_types
				<< rttg_dothpp;

			return _OSS.str();
		}

		static bool create_rttg_types(
			const std::string& 
                _PathProject,
			const std::string& 
                _Types)
		{
			std::ostringstream 
				_OSS; _OSS 
				<< _PathProject 
				<< "/src/"
				<< rttg_types_name();

			std::ofstream _Fout(
				_OSS.str());

			_Fout 
			<< "#define PARAMS "
			<< _Types
			<< CHARNL;

			return _Fout.good();
		}

		static bool build(
			const std::string& 
                _TypeCode)
		{
			std::ostringstream 
				_OSS; _OSS
				<< "make plugin CFLAGS="
				<< _TypeCode;

			int _Res=::system(
				_OSS.str().c_str());

			return -1!=_Res;
		}

		bool open(
			const std::string& _Name)
		{// DL open
			std::ostringstream 
				_OSS; _OSS
			<< _PathProject << CHARSL
			<< DIR_BUILD << CHARSL
			<< _Name;

			_Handle = ::dlopen(
				_OSS.str().c_str(), 
				RTLD_NOW);

			if (! _Handle)
			{
				fail(__func__);

				return false;
			}

			char* _Err(::dlerror());

			if (_Err)
			{
				fail(_Err);

				return false;
			}

			return true;
		}

		bool get_procedure(
			handle_type& _Func,
			const char* _Name)
		{// resets the good flag first
			reset(); char* _Err(0);

			if (_Func)
			{
				std::ostringstream
					_OSS; _OSS
					<< M_IN << __func__
					<< _Name << M_EXIST;

				fail(_OSS.str());

				return false;
			}

			_Func=::dlsym(_Handle, 
				_Name);

			if (! _Func)
			{
				fail(::dlerror());

				return false;
			}

			// symbol found
			if ((_Err = ::dlerror()) 
				!= NULL)
			{// error nonetheless
				fail(_Err);

				return false;
			}

			return true;
		}

		void close()
		{
			if (_Handle)
			{
				::dlclose(_Handle);

				_Handle=0; 
				_FuncMakeMicro=0;
				_FuncMakeCube=0;
			}
		}

		std::string
			_PathProject;
		std::string
			_SOName;
        size_t
            _Mask;
		handle_type
			_Handle;
		handle_type
            _FuncImportPtable;
		handle_type
            _FuncImportNP;
		handle_type
            _FuncImportSize;
		handle_type
            _FuncSeedPrng;
		handle_type
			_FuncMakeMicro;
		handle_type
			_FuncMakeCube;
        handle_type
            _FuncSampleMicro;
        handle_type
            _FuncWriteTraversing;
	};
}
