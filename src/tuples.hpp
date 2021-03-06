#pragma once
#include <tuple>

namespace confly 
{
    /// @brief total code generator for 
    /// tuple elements.
    /// @note undefined struct to allow
    /// template specializations.
    template <class T>
    struct totcode; // undef

    /// @brief total code generator for 
    /// tuple elements.
    /// @note specialization for long.
    /// @note use only for integer fields 
    /// that are not supposed to contain 
    /// negative values (e.g. age of persons,
    /// country codes or encoded geographical 
    /// information, encoded sex or gender
    /// information, etc.)
    template <>
    struct totcode <long>
    {
        /// @brief get total code
        /// for type long.
        /// @return -1 (total code)
        static auto get() 
            ->long
        {
            return -1;
        }
    };

    /// @brief total code generator for 
    /// tuple elements.
    /// @note specialization for strings.
    /// @warning not tested.
    template <>
    struct totcode <std::string>
    {
        /// @brief get total code
        /// for type long.
        /// @return "T" (total code).
        static auto get() 
            ->std::string
        {
            return "T";
        }
    };

    /// @brief total code generator for 
    /// tuple elements.
    /// @note specialization for double.
    /// @warning not tested.
    template <>
    struct totcode <double>
    {
        /// @brief get total code
        /// for type double.
        /// @return total code, i.e. 
        /// numeric_limits<double>::min().
        static auto get() 
            ->double
        {
            return std::numeric_limits<
                double>::min();
        }
    };

    // -------------------------------------

    /// @brief given a unsigned long mask,
    /// conditionally set the values of a 
    /// tuple using an arbitrary generator
    template <
        template <class> 
            class _GenTy,
        size_t I, size_t M>
    struct conditional_set_tuple
    {
        /// @brief given a unsigned long mask,
        /// conditionally set the values of a 
        /// tuple using an arbitrary generator.
        /// @param _P mask to test for the Ith
        /// tuple position.
        /// @param _Tup reference to destination
        /// tuple
        /// @note test the Ith position using 
        /// the supplied mask and, in case of
        /// positive test, set the tuple
        /// element using the generator. Then
        /// recurr.
        /// @warning to be used only starting
        /// from tuple index 1 (not zero).
        template <
            class _TupleTy>
        static void set(
            const size_t& _P, 
            _TupleTy& _Tup)
        {
            // import element type
            typedef typename 
                std::tuple_element<
                    I, _TupleTy>::type
                    elem_type;

            // get I-position mask
            const size_t 
                _Bit(static_cast<size_t>(
                    std::pow(2, I-1)));

            if (_P&_Bit)
            {// condition met
                std::get<I>(_Tup)=_GenTy<
                    elem_type>::get();
            }

            // recurr
            conditional_set_tuple<
                _GenTy, I+1, M>
                ::set(_P, _Tup);
        }
    };

    /// @brief conditional_set_tuple
    /// stop recursion
    template <
        template <class> 
            class _GenTy,
        size_t I>
    struct conditional_set_tuple <
        _GenTy, I, I>
    {
        /// @brief conditional_set_tuple
        /// stop
        template <
            class _TupleTy>
        static void set(
            const size_t&, 
            _TupleTy&)
        {
        }
    };

    // -------------------------------------

    /// @brief set value in tuple using
    /// a generator function.
    /// @param _ValTy whatever template class
    /// used in order to generate values of
    /// arbitrary types.
    template <
        template <class> class _ValTy,
        size_t I, size_t M>
    struct set_tuple
    {
        /// @brief set value in tuple using
        /// a generator function
        /// @param _Tup destination tuple
        /// @note fill the Ith tuple position
        /// and recurr
        template <class _TupleTy>
        static void set(_TupleTy& _Tup)
        {
            std::get<I>(_Tup)=
                _ValTy<typename 
                    std::tuple_element<
                        I, _TupleTy>::type
                            >::get();

            set_tuple<_ValTy, I+1, M>
                ::set(_Tup);
        }
    };

    /// @brief set_tuple stop
    template <
        template <class> class _ValTy,
        size_t I>
    struct set_tuple <_ValTy, I, I>
    {
        /// @brief set_tuple stop
        /// @note do nothing
        template <class _TupleTy>
        static void set(_TupleTy& _Tup)
        {}
    };

    // -------------------------------------

    /// @brief read tuple from input stream
	template <class _TupleTy, 
		size_t I, size_t M>
	struct read_tuple
	{
        /// @brief read tuple
        /// @param _S the input stream
        /// @param _Sep separator character 
        /// to discard when reading tuple values
        /// @param _Tup reference to
        /// the tuple to fill.
        /// @note read value from stream into
        /// the Ith tuple position and recurr
		read_tuple(
			std::istream& _S,
			const char& _Sep,
			_TupleTy& _Tup)
		{
            std::string x; 
            
            std::getline(_S, x, _Sep);

            std::istringstream _ISS(x);

			_ISS >> std::get<I>(_Tup);

			read_tuple<_TupleTy, 
				I+1, M>(_S, _Sep, _Tup);
		}
	};

    /// @brief read tuple stop
	template <class _TupleTy, 
		size_t I>
	struct read_tuple <_TupleTy, I, I>
	{
        /// @brief read tuple stop
        /// @param _S ignored
        /// @param _Sep ignored  
        /// @param _Tup ignored
        /// @note do nothing
		read_tuple(
			std::istream& _S,
			const char& _Sep,
			_TupleTy& _Tup)
		{
		}
	};

    // -------------------------------------

    /// @brief write tuple to output stream
	template <class _TupleTy, 
		size_t I, size_t M>
	struct write_tuple
	{
        /// @brief write tuple
        /// @param _S the output stream
        /// @param _Sep separator character 
        /// to use when writing tuple values
        /// @param _Tup const reference to
        /// the tuple to write.
        /// @note write tuple element at Ith
        /// position and recurr
		write_tuple(
			std::ostream& _S,
			const char& _Sep,
			const _TupleTy& _Tup)
		{
			_S << std::get<I>(_Tup);
            
            if (I+1<M) _S.put(_Sep);

			write_tuple<_TupleTy, 
				I+1, M>(_S, _Sep, _Tup);
		}
	};

    /// @brief write tuple stop
	template <class _TupleTy, 
		size_t I>
	struct write_tuple <_TupleTy, I, I>
	{
        /// @brief write tuple stop
        /// @param _S ignored
        /// @param _Sep ignored  
        /// @param _Tup ignored
        /// @note do nothing (no newline
        /// character is added)
		write_tuple(
			std::ostream& _S,
			const char& _Sep,
			const _TupleTy& _Tup)
		{
            //_S.put('\n');
		}
	};

    // -------------------------------------

    /// @brief test empty tuple
	template <class _TupleTy, 
		size_t I, size_t M>
	struct empty_tuple
	{
        /// @brief tuple element type
        /// at the Ith tuple position
        typedef typename 
            std::tuple_element<I, 
                _TupleTy>::type 
            element_type;

        /// @brief test Ith tuple element
        /// @param _Tup const reference to 
        /// the tuple
        /// @return false if non-empty, 
        /// recurr otherwise.
		static bool test(
			const _TupleTy& _Tup)
		{
			if (std::get<I>(_Tup)
                !=element_type())
                    return false;

			return empty_tuple<_TupleTy, 
				I+1, M>::test(_Tup);
		}
	};

    /// @brief test empty tuple stop
	template <class _TupleTy, 
		size_t I>
	struct empty_tuple <_TupleTy, I, I>
	{
        /// @brief stop compile time recursion
        /// @param _Tup ignored
        /// @return true
		static bool test(
			const _TupleTy& _Tup)
		{
            return true;
		}
	};

    // -------------------------------------

    /// @brief copy insert tuple values into
    /// a tuple of containers, of the same 
    /// respective types, which implements
    /// the method "insert" (e.g. std::set)
    template <
		size_t I, size_t M>
    struct copy_insert_tuple
    {
        /// @brief insert the Ith value
        /// of a source tuple into a container
        /// hosted at the Ith position
        /// of a destination tuple.
        /// @param _Src source tuple
        /// @param _Dst destination tuple of 
        /// containers
        /// @note used to create metadata on
        /// the fly while reading microdata
        /// sources.
        template <
            class _TupleTy, 
            class _MetaTy>
        static void insert(
            const _TupleTy& _Src,
            _MetaTy& _Dst)
        {
            std::get<I>(_Dst)
                .insert(
            std::get<I>(_Src));

            // recurr
            copy_insert_tuple<I+1, M>
                ::insert(_Src, _Dst);
        }

    };

    /// @brief stop compile time recursion
    template <
		size_t I>
    struct copy_insert_tuple <I, I>
    {
        /// @brief insert the Ith value
        /// of a source tuple into a container
        /// hosted at the Ith position
        /// of a destination tuple.
        /// @param _Src ignored
        /// @param _Dst ignored
        /// @note empty member made in order
        /// to stop compiler recursion
        template <
            class _TupleTy, 
            class _MetaTy>
        static void insert(
            const _TupleTy& _Src,
            _MetaTy& _Dst)
        {
        }
    };
}