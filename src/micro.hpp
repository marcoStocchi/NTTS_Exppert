#pragma once
#include <fstream>
#include <sstream>
#include <vector>

#include "def.hpp"
#include "shared.hpp"
#include "tuples.hpp"
#include "random_dist.hpp"

namespace confly
{
    /// @brief pure virtual class
    /// to handle typed microdata
    /// objects. See template struct
    /// micro. 
    struct micro_base
	{
        /// @brief virtual destructor
		virtual ~ micro_base() {}

        /// @brief read from stream
		virtual bool read(
			std::istream& _ISS,
			char _Sep) =0;

        /// @brief write to stream
		virtual void write(
			std::ostream& _S,
			char _Sep) =0;

        /// @brief set mask 
        virtual void set_mask(
            const size_t& _cMask) =0;
	};

    /// @brief arbitrary types
    /// microdata set
	template <class... _Types>
	struct micro
		: micro_base
	{
        /// @brief the type of tuple
		typedef std::tuple<
			_Types...>
			tuple_type;

        /// @brief the type of metadata
        typedef std::tuple<
            std::set<_Types>...>
            metadata;

        /// @brief the type of the
        /// tuple_size
		typedef std::tuple_size<
			tuple_type>
			tuple_size;

        /// @brief the type of the
        /// metadata_size
		typedef std::tuple_size<
			metadata>
			metadata_size;

        /// @brief the type of the
        /// container holding tuples.
        typedef std::vector<
            tuple_type>
            container;

        /// @brief type of the uniform
        /// integer distribution.
        typedef uniform_distribution<
            long>
            unif_type;

        /// @brief construct empty.
        /// @note set the minimum and
        /// maximum parameters of the unif.
		micro()
            : _DB()
            , _Meta()
            , _Mask(0)
            , _Unif(0, _DRE.max())
        {
            debug_msg<BDEBUGLEVEL3>
                (__func__, M_CONSTRUCT);
        }

        /// @brief destroy object.
		~ micro() 
        {
            debug_msg<BDEBUGLEVEL3>
                (__func__, M_DESTROY);
        }


        /// @brief read from stream.
		virtual bool read(
			std::istream& _S,
			char _Sep)
		{// read file, insert records
			while (_S.good())
			{// read
				std::string _Ln;

				std::getline(_S, _Ln);

				std::istringstream _ISS(_Ln);

				insert(_ISS, _Sep);
			}

            // insert total codes
            // into the metadata...

            tuple_type _Totals;

            set_tuple<totcode, 
                1, tuple_size::value>
                ::set(_Totals);

            copy_insert_tuple<
                1, // skip ids
                tuple_size::value>
                ::insert(_Totals, _Meta);

			return true;
		}

        /// @brief write to stream.
        /// @note test only.
		virtual void write(
			std::ostream& _S,
			char _Sep)
		{
			for (size_t i=0; 
				i<_DB.size(); ++i)
				write_tuple<tuple_type,
				0, tuple_size::value>(
					_S, _Sep, _DB[i]);
		}

        /// @brief set the mask.
        virtual void set_mask(
            const size_t& _cMask)
        {
            _Mask=_cMask;
        }

        /// @brief get const reference
        /// to the container holding
        /// microdata records.
        auto get() const 
            ->const container&
        {
            return _DB;
        }

        /// @brief get const reference
        /// to the container holding
        /// metadata
        auto get_meta() const 
            ->const metadata&
        {
            return _Meta;
        }

        /// @brief get mask
        auto get_mask() const 
            ->const size_t&
        {
            return _Mask;
        }

	private:

		void insert(
			std::istream& _S,
			const char& _Sep)
		{// convert line to tuple
			// and insert record
			tuple_type _Tup;

			read_tuple<tuple_type,
				0, tuple_size::value>(
					_S, _Sep, _Tup);

			if (! empty_tuple<
				tuple_type, 0, 
				    tuple_size::value>
				        ::test(_Tup))
            {
                //unif<long> _Unif;

                // set record key
                std::get<0>(_Tup)=
                    _Unif(_DRE);

                // install record
				_DB.push_back(_Tup);

                // replace masked tuple values
                // with total-codes in order to
                // update the metadata
                conditional_set_tuple<
                    totcode, 1, 
                    tuple_size::value>
                        ::set(_Mask, _Tup);

                // update metadata
                copy_insert_tuple<1, // skip id
                    tuple_size::value>
                        ::insert(_Tup, _Meta);
            }
		}

		container
			_DB;
        metadata
            _Meta;
        size_t 
            _Mask;
        unif_type
            _Unif;
	};
}