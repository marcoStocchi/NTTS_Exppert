#pragma once
#include <random>
#include "random_distp.hpp"

namespace confly
{
	/// @brief template uniform distribution 
	/// class for integer and real value types
	template <class T>
	struct uniform_distribution
	{
		/// @brief value type
        typedef T 
            value_type;

		/// @brief result type
		typedef value_type
			result_type;

		/// @brief parameter type
		typedef value_type
			param_type;

		/// @brief conditionally import
		/// uniform real or integer
		/// distribution based on the 
		/// supplied value type parameter
		typedef typename 
			std::conditional<
				std::is_floating_point<
					value_type>::value,
				uniform_real_dist<
					value_type>,
				uniform_int_dist<
					value_type>
			>::type 
			type;

		/// @brief construct with minimum
		/// and maximum parameters
		/// @param _cMin minimum
		/// @param _cMax maximum
		uniform_distribution(
			const param_type& _cMin, 
			const param_type& _cMax)
			: _Rep(_cMin, _cMax)
		{}

		/// @brief draw value.
		/// @param _RE random generator.
		/// of type _EngTy.
		/// @return extracted value.
		/// @note relies on _EngTy::operator()
		/// to perform the draw.
		template <class _EngTy>
		auto operator() ( 
			_EngTy& _RE)
			->value_type
		{
			return _Rep(_RE);
		}

	private:
	
		type
			_Rep;
	};

	/// @brief Laplace distribution class
	struct laplace_distribution
	{
		/// @brief value type
		typedef double
			value_type;

		/// @brief result type
		typedef value_type
			result_type;

		/// @brief parameter type
		typedef value_type
			param_type;

		/// @brief real uniform 
		/// distribution
		typedef 
			uniform_real_dist<
				value_type> 
			real_distribution_type;

		/// @brief construct with Laplace 
		/// distribution parameters
		/// @param _cMu location
		/// @param _cB scaling
		laplace_distribution(
			const param_type& _cMu, 
			const param_type& _cB)
			: _Mu(_cMu)
			, _B(_cB)
			, _RealUnif(0.0, 1.0)
		{}

		/// @brief set Laplace 
		/// distribution parameters
		/// @param _cMu location
		/// @param _cB scaling
        void set(
            const param_type& _cMu, 
			const param_type& _cB)
        {
            _Mu=_cMu;
            _B=_cB;
        }

		/// @brief draw value
		/// @return extracted value
		/// @note relies on _EngTy::operator()
		/// to perform the draw.
		template <class _EngTy>
		auto operator() ( 
			_EngTy& _RE)
			->value_type
		{
			const value_type _Q(
				_RealUnif(_RE));

			if (_Q < 0.5)
				return _Mu + 
					_B*std::log(
						2*_Q);

			if (_Q > 0.5)
				return _Mu - 
					_B*std::log(
						2.0-2*_Q);

			else
				return _Mu;
		}

	private:

		value_type 
			_Mu; // location
		value_type
			_B; // scale
		real_distribution_type
			_RealUnif;
	};

	/// @brief Geometric distribution class
	struct geometric_distribution
	{
		/// @brief value type
		typedef int
			value_type;

		/// @brief real type
		typedef double
			real_type;

		/// @brief result type
		typedef value_type
			result_type;

		/// @brief parameter type
		typedef real_type
			param_type;

		/// @brief real uniform 
		/// distribution
		typedef 
			uniform_real_dist<
				real_type> 
			real_distribution_type;


		/// @brief construct with Geometric 
		/// distribution parameter
		/// @param _cEps epsilon
		geometric_distribution(
			const param_type&,
    		const param_type& _cEps)
			: _Eps(_cEps)
			, _RealUnif(0.0, 1.0)
		{
		}

		/// @brief set Geometric 
		/// distribution parameter
		/// @param _cEps epsilon
        void set(
            const param_type&, 
			const param_type& _cEps)
        {
            _Eps=_cEps;
        }

		/// @brief draw value
		/// @return extracted value
		/// @note relies on _EngTy::operator()
		/// to perform the draw.
		template <class _EngTy>
		auto operator() ( 
			_EngTy& _RE)
			->value_type
		{
			const real_type _P(
				_RealUnif(_RE));

			int _Z(0);

			if (_P > 0.5) 
			{
				while (_P>cgeom(_Z)) 
				{
					++_Z;
				}
			} 

			else 
			{
				while (_P<cgeom(_Z-1))
				{
					--_Z;
				}
			}

			return _Z;
		}

	private:

		auto cgeom(
			const value_type& _Z)
			const
			->real_type 
		{
			const real_type 
				_A(std::exp(-_Eps));

			real_type
				_G(0.0);

			if (_Z<0) 
			{
				_G=
				std::pow(_A,-_Z)/(1+_A);
			}

			else 
			{
				_G=(1+_A - 
				std::pow(_A,_Z+1))/(1+_A);
			}

			return _G;
		}

		real_type
			_Eps;
		real_distribution_type
			_RealUnif;
	};
}