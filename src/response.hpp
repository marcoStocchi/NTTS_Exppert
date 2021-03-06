#pragma once
#include "ptable.hpp"
#include "random_dist.hpp"

namespace confly
{
    extern double 
        _DPFmu,
        _DPFb,
        _DPGeps;

    typedef size_t
        count_type;

    typedef int
        integral_noise_type;

    typedef double
        real_noise_type;

    typedef double
        cellkey_key_type;

    typedef integral_noise_type
        cellkey_value_type;

    typedef double 
        laplace_param_type;

    typedef real_noise_type
        laplace_value_type;

    typedef double 
        geometric_param_type;

    typedef integral_noise_type
        geometric_value_type;

    typedef std::tuple<
        count_type,     // 0 Count
        integral_noise_type, // 1 CK
        real_noise_type, // 2 DPF
        integral_noise_type // 3 DPG
        > 
        response_record_type;


    /// @brief hypercube leaf at 
    /// arbitrary coordinates.
    /// @note implements:
    /// - counts, 
    /// - cell-key's record-key, 
    /// - cell-key's bounded noise, 
    /// - differential privacy as vanilla Laplace,
    /// - differential privacy as geometric.
    /// @note access to static objects
    /// ptable, Laplace distribution and
    /// geometric distribution
    struct response
    {
        /// @brief record type
        typedef response_record_type
            record_type;

        /// @brief construct empty
        response()
            : _Rkey(0.0)
            , _Rec()
        {
        }

        /// @brief increment the count,
        /// sum up the supplied cell-key
        /// @param _Key cell key
        void update(
            const cellkey_key_type& _Key)
        {// update count and record key
            ++std::get<0>(_Rec);

            _Rkey+=_Key;
        }

        /// @brief finalize cell-key (ptable
        /// lookup) and differential privacy
        /// (draw noise from Laplace and 
        /// Geometric).
        void finalize()
        {// finalize CK and DP
            // called by
            // iterate_write_cube

            noise_ck_finalize();

            noise_dp_finalize();
        }

        /// @brief write count, CK noise,
        /// DP Laplace and DP Geometric.
        /// @param _S the output stream.
        /// @param _Sep the character used
        /// to separate the values.
        void write(
            std::ostream& _S,
            char _Sep) 
            const 
        {// write data
            // called by
            // iterate_write_cube
            _S 
            << std::get<0>(_Rec) << _Sep
            << std::get<1>(_Rec) << _Sep
            << std::get<2>(_Rec) << _Sep
            << std::get<3>(_Rec);
        }

        /// @brief read count, CK noise, 
        /// DP Laplace and DP Geometric 
        /// from input stream.
        /// @param _S the input stream.
        bool read(
            std::istream& _S)
        {
            _S
            >> std::get<0>(_Rec)
            >> std::get<1>(_Rec)
            >> std::get<2>(_Rec)
            >> std::get<3>(_Rec);

            return true;
        }

        /// @brief get record
        /// @return const reference 
        /// to record
        auto get() const 
            ->const record_type&
        {// get record
            return _Rec;
        }

        /// @brief import Laplace 
        /// distribution parameters.
        /// @param _cDPFmu location.
        /// @param _cDPFb scaling.
        static void set_laplace_parm(
            const laplace_param_type& _cDPFmu,
            const laplace_param_type& _cDPFb)
        {
            _Laplace.set(
                _DPFmu, _DPFb);
        }

        /// @brief import Geometric 
        /// distribution parameters.
        /// @param _cDPGeps epsilon (noise budget).
        static void set_geometr_parm(
            const geometric_param_type& _cDPGeps)
        {
            _Geometric.set(
                0, _DPGeps);
        }

        /// @brief import ptable. 
        /// @param _Ptab precomputed ptable.
        static void set_ptable(
            const ptable& _Ptab)
        {
            _PTable=_Ptab;
        }

    private:

		void noise_ck_finalize()
		{
			// find cell key
			cellkey_key_type
                 _CellKey=
                ::fmod(_Rkey, 1.0);;

			// lookup ptable
			// CK noise
            std::get<1>(_Rec)=
                _PTable.lookup(
                    std::get<0>(_Rec), // count
                        _CellKey);

            #if (BDEBUG)
			debug_msg<4>(
                M_LOOKUP, 
                std::get<0>(_Rec), // count
                _CellKey, 
                std::get<1>(_Rec)); // CK noise
            #endif
		}

		void noise_dp_finalize()
		{
            // DPF
            std::get<2>(_Rec)
                =_Laplace(_DRE);

            // DPG
            std::get<3>(_Rec)
                =_Geometric(_DRE);
		}

        cellkey_key_type
            _Rkey;
        record_type
            _Rec;
        static laplace_distribution
            _Laplace;
        static geometric_distribution
            _Geometric;
        static ptable
            _PTable;
    };

    /// @brief structure to hold
    /// a vector of response
    /// records (1 for each hypercube's
    /// coordinate)
    struct traversed_response
    {
        /// @brief record type
        typedef 
            response_record_type
            record_type;

        /// @brief type of vector of 
        /// records
        typedef std::vector<
            record_type>
            container;

        /// @brief construct empty response,
        /// @note allocate the container
        /// to accomodate _MaxSz records.
        traversed_response()
            : _Cont()
        {
            _Cont.resize(_MaxSz);
        }

        /// @brief insert record at vector
        /// position _ID.
        /// @param _ID index of the record in
        /// the vector.
        /// @param _Rec const reference to
        /// the record to insert.
        void insert(
            const size_t& _ID,
            const record_type& _Rec)
        {
            _Cont[_ID]=_Rec;
        }

        /// @brief write the container to the
        /// supplied output stream, using the 
        /// supplied separator character.
        /// @param _S the output stream.
        /// @param _Sep the separator char.
        /// @note writes records composed of
        /// - count
        /// - pure generated noise cellkey
        /// - pure generated noise differential 
        ///   privacy - vanilla laplace, 
        /// - pure generated noise differential 
        ///   privacy - geometric distribution
        /// - perturbed count using noise cellkey
        /// - perturbed count using noise DP Laplace
        /// - perturbed count using noise DP Geometric
        void write(
            std::ostream& _S,
            char _Sep)
            const 
        {
            for (size_t i=0; i<_Cont.size();
                ++i)
            {
                const record_type&
                    _R(_Cont[i]);

                _S 
                << std::get<0>(_R) << _Sep // count
                << std::get<1>(_R) << _Sep // naked CK
                << std::get<2>(_R) << _Sep // naked DPF
                << std::get<3>(_R) << _Sep // naked DPG
                << std::get<0>(_R)
                    + std::get<1>(_R) << _Sep // CK
                << std::get<0>(_R)
                    + std::get<2>(_R) << _Sep // DPF
                << std::get<0>(_R)
                    + std::get<3>(_R) // DPG
                << CHARNL;
            }
        }

        /// @brief static member used to allocate
        /// vector of records (basically 1 for each
        /// hypercube)
        static size_t
            _MaxSz;
        /// @brief vector of records 
        container
            _Cont;
    };
}