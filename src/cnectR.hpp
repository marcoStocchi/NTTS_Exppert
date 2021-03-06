// connector to R interpreter.
// nasty preprocessor macros ahead.
// recommend not to change unless
// absolutely necessary.

#pragma once

#define CSTACK_DEFNS
#define USE_RINTERNALS

// include needed C files
#include <Rinterface.h>
#include <Rembedded.h>  
#include <Rdefines.h>

#define E_CNECTOR_DLOPEN        "R connector open"
#define E_CNECTOR_PROCCALL      "R procedure call"

// replace R-internals macro 'sexp'
#define R_EXPRESSION            SEXP

namespace confly 
{
	/// @brief R connector
	struct cnectR
		: enable_internal_error
	{
		/// @brief type of string
		typedef std::string
			string_type;

		/// @brief construct with
		/// name of script to run
		cnectR(
			const string_type& _cRScript)
			: _RScript(_cRScript)
		{
			debug_msg<4>(
				M_CONNECTOR,
				M_CONSTRUCT);

			load();
		}

		/// @brief destroy
		~ cnectR()
		{
			debug_msg<4>(
				M_CONNECTOR,
				M_DESTROY);

			unload();
		}

		/// @brief call a R function taking
		/// three strings as parameters.
		/// @return true in case of success,
		/// false otherwise.
        bool call(
            const char* _Rfunc,
			const string_type& _In1, 
			const string_type& _In2, 
			const string_type& _In3)
        {
			int _Err(0); 
            
            R_EXPRESSION _Expr;

			PROTECT(_Expr = 
				lang4(install(_Rfunc), 
					mkString(_In1
						.c_str()),
					mkString(_In2
						.c_str()),
					mkString(_In3
						.c_str())));

			R_EXPRESSION _Res = 
				R_tryEval(_Expr, 
					R_GlobalEnv, 
					&_Err);

			if (_Err) 
			{// error 
				fail(
                    E_CNECTOR_PROCCALL);
                UNPROTECT(1);
				return false;
			}

            // resume garbage collector
			UNPROTECT(1);

			return true;
        }

		/// @brief call a R function taking
		/// a lot of parameters... method
		/// specifically written to be used
		/// in confly. 
        bool call(
            const char* _Rfunc,
			const string_type& _In1, 
			const string_type& _In2, 
			const string_type& _In3,
            const size_t& _I,
            const double& _D1,
            const double& _D2,
            const double& _D3)
        {
			int _Err(0); 
            
            R_EXPRESSION _Expr;

			PROTECT(_Expr =
				lang6(install(_Rfunc), 
					mkString(_In1
						.c_str()),
					mkString(_In2
						.c_str()),
					mkString(_In3
						.c_str()),
                    Rf_ScalarInteger(_I),
                    Rf_list3(
                        Rf_ScalarReal(_D1), 
                    Rf_ScalarReal(_D2),
                Rf_ScalarReal(_D3))
            ));

			R_EXPRESSION _Res = 
				R_tryEval(_Expr, 
					R_GlobalEnv, 
					    &_Err);

			if (_Err) 
			{// error 
				fail(E_CNECTOR_PROCCALL);
                UNPROTECT(1);
				return false;
			}

			UNPROTECT(1);

			return true;
        }

		/// @brief call a R function taking
		/// a lot of parameters... method
		/// specifically written to be used
		/// in confly. 
		bool call(
            const char* _Rfunc, 
			const string_type& _In, 
			const double& D_CK, 
			const double& V_CK, 
			const double& js_CK)
		{// call a R-consumer function
			int _Err(0);
            
            R_EXPRESSION _Expr;

			PROTECT(_Expr = 
				lang5(install(_Rfunc), 
					mkString(_In
						.c_str()),
					Rf_ScalarReal(D_CK),
                Rf_ScalarReal(V_CK),
            Rf_ScalarReal(js_CK)));

			R_EXPRESSION _Res = 
				R_tryEval(_Expr, 
					R_GlobalEnv, 
					&_Err);

			if (_Err) 
			{// error 
				fail(E_CNECTOR_PROCCALL);
                UNPROTECT(1);
				return false;
			}

			UNPROTECT(1);

			return true;
		}


	private:

		bool load()
		{// load library and symbols
			static const char* 
                argv[]= 
			{
				"Confly",
				"--silent"
			};

			// (1)
			Rf_initEmbeddedR(2, 
				const_cast<char**>(
                    argv));

			// (2)
			R_CStackLimit = (
                uintptr_t)-1;

			int _Err(0); 
            
            R_EXPRESSION _Expr;

			// (3)
			PROTECT(_Expr = lang2(
				install("source"), 
				    mkString(_RScript
				        .c_str())));

			// (4)
			R_tryEval(_Expr, R_GlobalEnv, 
				&_Err);
	
			if (_Err) 
			{// R error
				fail(E_CNECTOR_DLOPEN);
				UNPROTECT(1);
                return false;
			}

			else  
			{// ok
			}

			UNPROTECT(1);

			return true;
		}

		bool unload()
		{// close library
			// exit R session
			Rf_endEmbeddedR(0);

			return true;
		}

		string_type
			_RScript;
	};
}

// remove R macros interfering with 
// standard C++
#undef length
