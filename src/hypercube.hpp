#pragma once
#include "nested_map.hpp"

namespace confly
{
	/// @brief read hypercube from file,
	/// update the traversing structure
	template <size_t I, size_t M>
	struct read_cube
    {
		/// @brief read key, descent to
		/// the next coordinate axis,
		/// both on the supplied hypercube
		/// and on the traversing one. 
		/// Then recurr.
		/// @param _CubeID ordinal identifying
		/// the hypercube
		/// @param _S the input stream
		/// @param _Cube nested map of the cube.
		/// As descent to the leaf progresses
		/// its type also changes.
		/// @param _Trv nested map of the
		/// traversing structure. 
		/// As descent to the leaf progresses
		/// its type also changes.
		/// @note a read error will cause the
		/// recursion to stop.
		/// @return false in case of read 
		/// failure, or transfer the control
		/// to the next level of recursion.
		template <
			class _MapTy,
			class _TrvTy>
        static bool read(
            const size_t& _CubeID,
            std::istream& _S,
			_MapTy& _Cube,
			_TrvTy& _Trv)
        {
            typedef typename _MapTy
                ::key_type
                key_type;

            key_type _Key; _S >> _Key;

            if (! _S.good())
                return false;

            auto& _Map(_Cube[_Key]);
            auto& _MapTrv(_Trv[_Key]);

            return read_cube<I+1, M>
                ::read(_CubeID, _S, 
                    _Map, _MapTrv);
        }
    };

	/// @brief read_cube stop
	template <size_t I>
	struct read_cube <I, I>
    {
		/// @brief read_cube stop.
		/// read the response at cube's
		/// coordinates, and insert
		/// the same response in the
		/// traversing structure at 
		/// _CubeID position. 
		/// @return false if read fails,
		/// true otherwise.
		template <
			class _MapTy,
			class _TrvTy>
        static bool read(
            const size_t& _CubeID,
            std::istream& _S,
			_MapTy& _Response,
			_TrvTy& _TrvResponse)
        {
            if (! _Response.read(_S))
                return false;

            _TrvResponse.insert(
                _CubeID, 
                _Response.get());

            return true;
        }        
    };

	// -------------------------------------

	/// @brief iterate metadata, write 
	/// traversed response at each 
	/// set of coordinates
	template <size_t I, size_t M>
	struct write_traversed_t
	{// start recursion
		/// @brief get metadata set
		/// at Ith position, start iterating
		/// it. In the same loop: store the
		/// coordinates in the _Carry tuple; 
		/// descend to the next level 
		/// of coordinates in the traversing 
		/// nested map, and recurr. This
		/// mechanism is contrived in order
		/// to iterate all the possible 
		/// combinations of the metadata, while
		/// at the same time creating empty
		/// buckets on the nested map, if they
		/// do not already exist. This is useful
		/// to fill the traversing nested map
		/// in one single take.
		template <
			class _MetaTy,
			class _TupleTy,
			class _MapTy>
		static void write(
			const std::string& _cPath, 
			char _Sep,
			const _MetaTy& _Meta,
			_TupleTy& _Carry,
			_MapTy& _Trv)
		{// iterating metadata
			const auto& _Set(
				std::get<I>(_Meta));

			for (auto It=_Set.cbegin(), 
				E= _Set.cend(); It!=E; 
					++It)
			{
				// store coordinate in
				// the carry 
				std::get<I>(_Carry)=*It;

				// climb the nested map
				auto& _Map(_Trv[*It]);

				// recurr...
				write_traversed_t<I+1, M>
					::write(_cPath, _Sep, 
						_Meta, _Carry,
							_Map);
			}
		}

		/// @brief similar to the write()
		/// with metadata, but this overload
		/// does not use metadata to iterate,
		/// rather it iterates directly the
		/// records of the traversing nested
		/// map. It presumes that the map
		/// has already allocated all the 
		/// buckets, and it is faster than
		/// its sibling overload.
		template <
			class _TupleTy,
			class _MapTy>
		static void write(
			const std::string& _cPath, 
			char _Sep,
			_TupleTy& _Carry,
			const _MapTy& _Trv)
		{// iterate the nested trav map
			for (auto It=_Trv.cbegin(), 
				E= _Trv.cend(); It!=E; 
					++It)
			{
				// store coordinate in
				// the carry 
				std::get<I>(_Carry)
                    =It->first;

				// climb the nested map
				const auto& _Map(
                    It->second);

				// recurr...
				write_traversed_t<I+1, M>
					::write(_cPath, _Sep, 
						_Carry, _Map);
			}
		}
	};

	/// @brief write_traversed_t stop
	/// recursion
	template <size_t I>
	struct write_traversed_t <I, I>
	{
		/// @brief stop recursion.
		/// Open write the traversing
		/// file and write output.
		template <
			class _MetaTy,
			class _TupleTy,
			class _MapTy>
		static void write(
			const std::string& _cPath, 
			char _Sep,
			const _MetaTy& _Meta,
			_TupleTy& _Carry,
			_MapTy& _TrvResponse)
		{
			std::ofstream _Fout(
				trv_filename(_cPath, 
                    _Carry));

			// flush the traversed
			// response to file
			_TrvResponse.write(
				_Fout, _Sep);
		}

		/// @brief stop recursion.
		/// Open write the traversing
		/// file and write output.
		template <
			class _TupleTy,
			class _MapTy>
		static void write(
			const std::string& _cPath, 
			char _Sep,
			_TupleTy& _Carry,
			const _MapTy& _TrvResponse)
		{
			std::ofstream _Fout(
				trv_filename(_cPath, 
                    _Carry));

			// flush the traversed
			// response to file
			_TrvResponse.write(
				_Fout, _Sep);
		}

        private:

        template <
			class _TupleTy>
        static auto trv_filename(
            const std::string& _cPath,
            _TupleTy& _Carry)
            ->std::string
        {
			// create a filename to
			// write the cell at
			// '_Carry' coordinates
			// e.g. trv_39_2_-1_-1.csv
			std::ostringstream 
				_OSS; _OSS 
				<< _cPath << CHARSL 
                << OF_TRAV << CHARUS;
				write_tuple<_TupleTy,1,I>(
					_OSS, CHARUS, _Carry);
				_OSS << DOT_CSV;
            return _OSS.str();
        }
	};

	// -------------------------------------

	/// @brief similar to write_traversed_t,
	/// except that the purpose is to
	/// write the hypercubes to stream, not
	/// the traversing structure; the latter
	/// is only updated in the first overload,
	/// while in the second is not considered.
	/// The second case is used when running
	/// confly ---aggregate --no-traverse
	/// as it is the case for jobs distributed
	/// among several different machines. 
	template <size_t I, size_t M>
	struct write_cube
	{
		/// @brief Iterate metadata, descend 
		/// both on cube and traversing maps 
		/// coordinates, update carry and 
		/// recurr.
		template <
			class _MetaTy,
			class _TupleTy,
			class _MapTy,
			class _TrvTy>
		static void write(
			const size_t& _CubeID,
			std::ostream& _S, 
			const char& _Sep,
			const _MetaTy& _Meta,
			_TupleTy& _Carry,
			_MapTy& _Cube,
			_TrvTy& _Trv)
		{
			const auto& _Set(
				std::get<I>(_Meta));

			for (auto It=_Set.cbegin(), 
				E= _Set.cend(); It!=E; 
					++It)
			{
				std::get<I>(_Carry)=*It;

				auto& _Map(_Cube[*It]);
				auto& _MapTrv(_Trv[*It]);

				write_cube<I+1, M>
					::write(_CubeID, _S,
						_Sep, _Meta, 
							_Carry, _Map, 
								_MapTrv);
			}
		}

		/// @brief Iterate metadata, descend
		/// only on cube maps coordinates,
		/// update carry and recurr.
		template <
			class _MetaTy,
			class _TupleTy,
			class _MapTy>
		static void write(
			std::ostream& _S, 
			const char& _Sep,
			const _MetaTy& _Meta,
			_TupleTy& _Carry,
			_MapTy& _Cube)
		{// climb coords only on cube
			const auto& _Set(
				std::get<I>(_Meta));

			for (auto It=_Set.cbegin(), 
				E= _Set.cend(); It!=E; 
					++It)
			{
				std::get<I>(_Carry)=*It;

				auto& _Map(_Cube[*It]);

				write_cube<I+1, M>
					::write(_S,
						_Sep, _Meta, 
							_Carry, _Map);
			}
		}
	};

	/// @brief write_cube stop recursion
	template <size_t I>
	struct write_cube <I, I>
	{
		/// @brief finalize cube's response,
		/// write cube record to stream,
		/// update the traversing's structure
		/// nested map.
		template <
			class _MetaTy,
			class _TupleTy,
			class _MapTy,
			class _TrvTy>
		static void write(
			const size_t& _CubeID,
			std::ostream& _S, 
			const char& _Sep,
			const _MetaTy& _Meta,
			_TupleTy& _Carry,
			_MapTy& _Response,
			_TrvTy& _Traversed)
		{
			// write branch (skip 1st)
			write_tuple<_TupleTy,1,I>(
				_S, _Sep, _Carry);

			_S.put(_Sep);

			// finalize response
			_Response.finalize();

			// update traversed
			_Traversed.insert(
				_CubeID,
				_Response.get());

			// write response
			_Response.write(_S, _Sep);

			_S.put('\n');
		}

		/// @brief finalize cube's response,
		/// write cube record to stream.
		template <
			class _MetaTy,
			class _TupleTy,
			class _MapTy>
		static void write(
			std::ostream& _S, 
			const char&  _Sep,
			const _MetaTy& _Meta,
			_TupleTy& _Carry,
			_MapTy& _Response)
		{
			// 1. finalize response
			// 2. write cube record 
            //    to stream

			// write branch (skip 1st)
			write_tuple<_TupleTy,1,I>(
				_S, _Sep, _Carry);

			_S.put(_Sep);

			// finalize response
			_Response.finalize();

			// write response
			_Response.write(_S, _Sep);

			_S.put('\n');
		}
	};

	/// @brief updates the response
	/// found as leaf of a nested map
	/// at specific coordinates.
	template <size_t I, size_t M>
	struct update_t
	{
		/// @brief recurr to extract
		/// the cube's nested map at
		/// specific coordinates
		template <class _SrcTy, 
			class _DstTy>
		static void update(
			const _SrcTy& _Src,
			_DstTy& _Dst)
		{
			update_t<I+1, M>
			::update(_Src, 
				_Dst[std::get<I>(
					_Src)]);
		}
	};

	/// @brief update_t stop recursion
	template <size_t I>
	struct update_t <I, I>
	{
		/// @brief draw record-key
		/// for cell-key method, 
		/// call update() on the
		/// leaf.
		template <class _SrcTy, 
			class _DstTy>
		static void update(
			const _SrcTy& _Src,
			_DstTy& _Dst)
		{                
			const double
				_RKey(rkey(_Src));

			// pass rkey to the 
			// response of the cell
			_Dst.update(_RKey);
		}

	private:

		template <class _SrcTy>
		static double rkey(
			const _SrcTy& _Src)
		{// convert rkey to real
			static const double
				_DREMAX(_DRE.max());

			return (double)
				std::get<0>(_Src)/
					_DREMAX;
		}
	};

	// ------------------------------------------
	/// @brief update cube cell
	template <class _SrcTy, 
		class _DstTy>
	inline void update_totals(
		const _SrcTy& _Src,
		const size_t& _Mask,
		_DstTy& _Dst)
	{
		// call stack
		// confly::sample_micro() (rttg.cpp)
		// ->hypercube->update()
		typedef std::tuple_size<_SrcTy>
			_TupSzTy;

		// static const size_t 
		// 	_TupleSz(std::tuple_size<
		// 		_SrcTy>::value);

		// exclude 'id' from the 
		// possible combinations
		static const size_t 
			_Cases(std::pow(2, 
				_TupSzTy::value-1));

		for (size_t i=0; i<_Cases;
			++i)
		{
			// skip updates to masked cells
			// as defined with "cube=false"
			// in the configuration file
			if (i != (i|_Mask))
				continue;

			_SrcTy _Tup(_Src);

			conditional_set_tuple<
				totcode,
				1, _TupSzTy::value>
				::set(i, _Tup);

			update_t<1, _TupSzTy::value>
				:: update(_Tup, _Dst);
		}
	}

	// ------------------------------------------
	// hypercube 

	/// @brief hypercube pure virtual
	/// base class 
	struct hypercube_base
	{
		/// @brief virtual destructor
		virtual ~ hypercube_base() {}

		/// @brief set cube id
		virtual void set_id(
			const size_t&) =0;

		/// @brief read from stream
        virtual bool read(
            std::istream&) =0;

		/// @brief write cubes and
		/// traverse in one go
		virtual void write_traversing(
			const std::string&) =0;
	};

	/// @brief second base class
	/// for hypercube 
	template <class... _Other>
	struct hypercube_base_II
	{// legacy compilers
        // partially ISO 2011
        // compliant 
	};

	/// @brief compile-time import 
	/// arbitrary tuple-typed nested
	/// map types with different
	/// response types.
	/// @note second base class
	/// for hypercube.
	/// @note specialization is needed
	/// when compiling using legacy GCC,
	/// in order to extract and disregard
	/// the microdata's record ID as first
	/// found among the fields.
	template <class _IdTy, 
		class... _Other>
	struct hypercube_base_II <
        _IdTy, _Other...>
	{
		/// @brief type of the nested map
		/// of the hypercube
		typedef typename 
			nested_map<response, 
				_Other...>::type
			nested_map_type;

		/// @brief type of the nested map
		/// used to traverse the hypercubes.
		typedef typename 
			nested_map<
                traversed_response,
				_Other...>::type
			traversing_nested_map_type;
	};

	/// @brief hypercube class
	template <class... _Types>
	struct hypercube
		: hypercube_base
		, hypercube_base_II<_Types...>
	{
		/// @brief type of second base
		/// class
		typedef hypercube_base_II<
   			_Types...>
			base_II;

		/// @brief type of nested map
		typedef typename base_II
			::nested_map_type
			nested_map_type;

		/// @brief type of traversing
		/// nested map
		typedef typename base_II
			::traversing_nested_map_type
			traversing_nested_map_type;

		/// @brief type of microdata
		/// structure
		typedef micro<_Types...>
			micro_type;

		/// @brief type of micro's
		/// record (here called branch)
		typedef typename
			micro_type::tuple_type
			branch_type;

		/// @brief type of the tuple_size
		/// of the micro's records.
		typedef typename
			micro_type::tuple_size
			branch_size;

		/// @brief type of the tuple
		/// hosting the metadata.
		typedef typename
			micro_type::metadata
			metadata;

		/// @brief type of the tuple_size
		/// of the metadata
		typedef typename
			micro_type::metadata_size
			metadata_size;

		/// @brief construct empty
		hypercube() : _ID(0) {}

		/// @brief destroy
		~ hypercube() {}


		/// @brief set ID
		virtual void set_id(
			const size_t& _cID)
		{// associate this object
			// to its number
			_ID=_cID;
		}

		/// @brief update cube with
		/// the supplied record, depending
		/// on the mask.
		void update(
			const size_t& _Mask,
			const branch_type& _Tup)
		{            
			update_totals(
				_Tup, _Mask, _Cube);
		}

		/// @brief read from stream
        virtual bool read(
            std::istream& _S)
        {
            while(_S.good())
            {
                std::string _L;

                std::getline(_S, _L);

                if (_L.size()<
                    sizeof...(_Types))
                    continue;

                size_t i=0; while((
                    i=_L.find(CHARSC, i))
                    !=std::string::npos)
                {
                    _L.replace(i, 
                        1, 1, CHARSP);
                }

                std::istringstream 
                    _ISS(_L);

                if (! read_cube<0, 
                    branch_size::value-1>
                    ::read(_ID, _ISS, 
                        _Cube, _Trv))
                            return false;
            }

            return true;
        }

		/// @brief write to stream.
		/// @param _Meta the metadata.
		/// @param _S the output stream.
		/// @param _BNoTrav if true, do
		/// not perform traversing in one go.
		void write(
			const metadata& _Meta,
			std::ostream& _S,
			bool _BNoTrav)  
		{// finalize cubes and traverse 
			// their cells at the same
			// time; unless _BNoTrav.
			branch_type _Carry;

			if (_BNoTrav)
			write_cube<
				1, metadata_size::value>
				::write(_S, CHARSC, 
					_Meta, _Carry, 
						_Cube);

			else // traverse as well
			write_cube<
				1, metadata_size::value>
				::write(_ID, _S, CHARSC, 
					_Meta, _Carry, 
						_Cube, _Trv);
		}

		/// @brief iterate the metadata
		/// and write the traversing files
		/// @param _Meta the metadata.
		/// @param _cPath the path to write the
		/// traversing files.
		static void write_traversing(
			const metadata& _Meta,
			const std::string& _cPath)
		{
			branch_type _Carry;

			write_traversed_t<
				1, metadata_size::value>
				::write(_cPath, CHARSC,
					_Meta, _Carry, _Trv);
		}

		/// @brief iterate the nested map
		/// and write the traversing files
		/// @param _cPath the path to write the
		/// traversing files.
		virtual void write_traversing(
			const std::string& _cPath)
		{// write the traversing files
            // iterating trv
			branch_type _Carry;

			write_traversed_t<
				1, metadata_size::value>
				::write(_cPath, CHARSC,
					_Carry, _Trv);
		}

	private:

		size_t 
			_ID;
		nested_map_type
			_Cube;
		static traversing_nested_map_type
			_Trv;
	};    
}

