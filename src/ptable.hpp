#pragma once
#include "def.hpp"
#include "shared.hpp"
#include "cnectR.hpp"


namespace confly
{
	/// @brief ptable record 
	/// structure
	struct ptable_record
	{// hold ptable record
		/// @brief integer type
        typedef long
            integer_type;

		/// @brief real type
        typedef double  
            real_type;

		/// @brief construct empty
		/// record
		ptable_record();

		/// @brief construct record 
		/// with parameters
		ptable_record(
			const integer_type& _cI,
			const integer_type& _cJ,
			const real_type& _cP,
			const integer_type& _cV,
			const real_type& _cPLb,
			const real_type& _cPUb);

		/// @brief destroy record
        ~ ptable_record(); 
        
		/// @brief comparison operator		
		bool operator < (
			const ptable_record& _Right) 
			const;
		
		/// @brief read record from 
		/// input stream
		bool read(
			std::istream& _S);

		/// @brief write record to
		/// stream
		void write(
			std::ostream& _S, 
			char _NewLine)
			const;
		
		/// @brief get value
		auto value() const
		    ->integer_type;
		
		/// @brief get lower bound
		auto lower() const
		    ->real_type;
		
		/// @brief get upper bound
		auto upper() const
		    ->real_type;

    private:

        friend struct ptable;

		integer_type
			_I;
		integer_type
			_J;
		real_type
			_P;
		integer_type
			_V;
		real_type
			_PLb;
		real_type
			_PUb;
	};

	inline std::ostream& operator << (
		std::ostream& _S, 
			const ptable_record& _Rec)
	{
		_Rec.write(_S, ' ');

		return _S;
	}

	// -------------------------------------

	/// @brief ptable I/O class
	struct ptable
	{// hold a ptable

		/// @brief integer type
        typedef ptable_record
            ::integer_type
            integer_type;

		/// @brief real type
        typedef ptable_record
            ::real_type
            real_type;

		/// @brief container type
		typedef std::multimap<
            integer_type, 
			ptable_record>
			container;

		/// @brief construct empty
		ptable();
		
		/// @brief destroy object
		~ ptable();

		
		/// @brief insert record
		void insert(
			const ptable_record& _Rec);
		
		/// @brief write record to
		/// stream
		void write(
			std::ostream& _S)
			const;

		/// @brief read record from
		/// stream
		bool read(
			std::istream& _S);
		
		/// @brief lookup ptable
		auto lookup(
            const integer_type& _I, 
			const real_type& _Ckey)
			const ->integer_type;

		/// @brief get number of 
		/// categories
        auto get_ncat() const 
            ->integer_type;

		/// @brief set number of
		/// categories
        void set_ncat(
            const integer_type&);

		/// @brief get size of
		/// container
		auto size() const
			->size_t;

    private:			
			
		integer_type 
			_NCat;
		container
			_PtableRecords;
	};

	// --------------------------------------------
    // load preexisting ptable or create new one

    typedef ptable::integer_type
        integer_type;

    typedef ptable::real_type
        real_type;

	bool load_ptable(
		ptable& _PTable,
		const std::string& _Fname);

	bool create_ptable(
        const std::string& _ScriptFname,
		const std::string& _Fname,
		const real_type& D_CK, 
		const real_type& V_CK, 
		const real_type& js_CK);
}



