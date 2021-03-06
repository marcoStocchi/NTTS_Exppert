#pragma once
#include <vector>
#include <set>

#include "random_eng.hpp"
#include "random_dist.hpp"

namespace confly
{
    /// @brief utility class to draw
    /// single or multiple randoms 
    /// from arbitrary distributions
    /// featuring 2 parameters.
    template <class _DistrTy>
    struct draw_kit
    {
        /// @brief type of distribution.
        typedef _DistrTy
            distribution_type;

        /// @brief type of value.
        typedef typename 
            distribution_type
            ::value_type
            value_type;

        /// @brief type of parameters.
        typedef typename 
            distribution_type
            ::param_type
            param_type;

        /// @brief draw single value.
        /// @param _1 the first parameter
        /// of the distribution.
        /// @param _2 the second parameter
        /// of the distribution.
        auto draw(
            const param_type& _1,
            const param_type& _2) 
            const 
            ->value_type
        {// single draw
            distribution_type
                _D(_1, _2);

            return _D(_DRE);
        }

        /// @brief draw multiple values.
        /// @param _1 the first parameter
        /// of the distribution.
        /// @param _2 the second parameter
        /// of the distribution.
        /// @param _N how many.
        /// @param _Cont destination vector
        /// @note _Cont vector gets resized
        /// by this function in order to
        /// accomodate _N entries.
        void draw(
            const param_type& _1,
            const param_type& _2,
            const size_t& _N,
            std::vector<value_type>& _Cont)
        {// draw _N
            _Cont.resize(_N);

            distribution_type
                _D(_1, _2);

            for (size_t i=0; i<_N; 
                ++i) _Cont[i]= 
                    _D(_DRE);
        }

        /// @brief draw multiple values.
        /// @note overload for standard
        /// sets of value_type.
        void draw(
            const param_type& _1,
            const param_type& _2,
            const size_t& _N,
            std::set<value_type>& _Cont)
        {// draw _N unique
            // warn: this function will hang
            // if the distribution is int
            // and its domain is smaller 
            // than _N
            distribution_type
                _D(_1, _2);

            while(_Cont.size() <_N)
                _Cont.insert(_D(_DRE));
        }

        /// @brief draw multiple values.
        /// @note overload for arbitrary
        /// container destinations implementing
        /// the method 'insert(iterator
        /// _Where, value_type _What)'.
        template <class _ContTy>
        void draw(
            const param_type& _1,
            const param_type& _2,
            const size_t& _N, 
            _ContTy& _Cont)
        {// multiple draws
            distribution_type
                _D(_1, _2);

            for (size_t i=0; i<_N; 
                ++i) _Cont.insert(
                    _Cont.end(), 
                    _D(_DRE));
        }
    };

    // -------------------------------------
    // uniform, laplace, geometric draws
    
    /// @brief deriving the draw_kit for
    /// uniform distributions of arbitrary
    /// type.
    template <class T>
    struct unif 
        : draw_kit< 
            uniform_distribution<
                T>>
    {// paramenters: min, max
    };

    /// @brief deriving the draw_kit for
    /// Laplace distributions.
    /// @note paramenters _1,_2 are mu, b
    /// respectively.
    struct laplace
        : draw_kit<
            laplace_distribution>
    {// paramenters: mu, b
    };

    /// @brief deriving the draw_kit for
    /// Geometric distributions.
    /// @note parameter _1 is ignored.
    /// @note parameter,_2 is epsilon.
    struct geometric
        : draw_kit<
            geometric_distribution>
    {// paramenters: ignored, eps
    };
}