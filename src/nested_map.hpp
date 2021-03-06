#pragma once
#include <map>
#include "response.hpp"

namespace confly
{
	namespace
	{
		/// @brief (1) compile-time accumulate 
		/// nested map types of arbitrary 
		/// key-types and arbitrary leaf
		/// type.
		template <class... _Other>
		struct nested_map 
		{
			/// @brief offer arbitrary 
			/// nested map type to the user.
			typedef nested_map <_Other...>
				type;
		};

		/// @brief (2) specialization to finalize
		/// the nested map type.
		template <class _RespTy, 
			class _Left, class _Right>
		struct nested_map <_RespTy, _Left, _Right>
		{	
			/// @brief stop compiler recursion
			/// started by (3) using:
			/// _Left as the leftmost mapping type, 
			/// _Right as arbitrary nested map,
			/// _RespTy as the leaf response type.
			typedef std::map<_Left, 
				std::map<_Right, _RespTy>>
				type;
		};

		/// @brief (3) specialization to build 
		/// the derivation of the structure
		/// nested_map at compile time.
		template <class _RespTy, 
			class _Left, class... _Other>
		struct nested_map <_RespTy, _Left, _Other...> 
			: nested_map <_RespTy, _Other...>
		{
			/// @brief specialize (1) in order
			/// to extract the response type, 
			/// and parametricly recurr upward 
			/// in order to build the fully 
			/// derived class.
			typedef nested_map<_RespTy, 
				_Left, _Other...>
				upper;

			/// @brief the type of the upper
			/// struct of the derivation.
			typedef std::map<_Left, 
				typename upper::type> 
				type;
		};
	}
}
