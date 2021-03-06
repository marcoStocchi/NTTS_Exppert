#pragma once

#define FIELD_PATH_PROJ 	"path.dir_project"
#define FIELD_PATH_OUTP 	"path.dir_output"
#define FIELD_FILE_PTAB 	"path.file_ptable"
#define FIELD_FILE_INPU 	"path.file_input"

#define ARRAY_MACHINES  	"machines"

#define FIELD_RATE      	"rate"
#define FIELD_SIZE      	"size"
#define FIELD_BEGIN      	"begin"

#define FIELD_PRNG      	"prng.engine"
#define FIELD_SEED      	"prng.seed"
#define FIELD_TEST      	"prng.test"

#define FIELD_DPFMU     	"noise.DPF_mu"
#define FIELD_DPFB      	"noise.DPF_b"
#define FIELD_DPGEPS    	"noise.DPG_eps"

#define FIELD_CKD       	"noise.CK_D"
#define FIELD_CKVAR     	"noise.CK_var"
#define FIELD_CKjs      	"noise.CK_js"

#define FIELD_EXP       	"micro"
#define ELEM_TYPE       	"type"
#define ELEM_CUBE       	"cube"

namespace confly
{
	static const std::map<
		std::string, std::string> 
	_MetaTypes=
	{
		{"int", "long"},
		{"integer", "long"},
		{"string", "std::string"},
		{"decimal", "double"}
	};

	static const std::map<
		std::string, char> 
	_TypeCodes=
	{
		{"int", 'l'},
		{"integer", 'l'},
		{"string", 's'},
		{"decimal", 'd'}
	};

	/// @brief Read, parse and query the configuration file
	///
	/// The configuration file is a JSON structured file
	/// containing information about:
	/// - paths
	/// - machines used to run a distributed job
	/// - sampling rate of a microdata set
	/// - number of hypercubes to generate
	/// - starting ordinal of the first hypercube
	/// - pseudo random generator information
	/// - noise generation parameters for 
	///   a) bounded noise (Cell-Key)
	///   b) differential privacy (Vanilla Laplace)
	///   c) differential privacy (Geometric Distribution)
	/// - metadata of the microdata set (field names,
	///   types and flags indicating whether to aggregate
	///   only)
	/// @note Have a quick glance to the "config.json" file 
	/// published in the Git repository.  
	struct config
	{
		/// default constructor
		config()
			: _PT()
			, _PathProj()   // path to project
			, _PathOutput() // path to output
			, _FilePtable() // abs path to ptable.dat
			, _FileInput()  // abs path to input file

			, _HostNames()  // hostnames of the machines

			, _Rate(1.0)    // sampling rate
			, _Cubes(1)     // number of cubes
			, _Begin(0)     // 1st cube starting#

			, _PrngEng()    // name of rand engine
			, _PrngSeed(42) // seed
			, _PrngTest(0)  // test runs for --random

			, _DPFmu(0.0)   // noise params
			, _DPFb(2.0)
			, _DPGeps(0.5)

			, _CKD(3.0)
			, _CKvar(8.0)
			, _CKjs(1.0)

			, _Size(0)      // tuple size
			, _Types()      // cs tuple types
			, _TypeCode()   // type code 
			, _Mask(0)      // aggregate only mask
		{
			// mask: 
			// 1-bit for including the
			// field value in the cube,
			// 0-bit for aggregate it only.
		}

		/// destructor
		~ config()
		{}

		/// @brief load configuration from stream
		/// @return true if the configuration was
		/// correctly loaded; false if a read_json
		/// error occurred. 
		/// @note Uses boost::property_tree::json_parser
		/// for this operation.
		bool load(std::istream& _S)
		{// load json config
			try
			{
				boost::property_tree
				::read_json(_S, _PT);

				return true;
			}

			catch(...)
			{
				return false;
			}
		}

		/// @brief parse the configuration, extracting
		/// all the parameters.
		/// @return true if all parameters correctly 
		/// retrieved, false otherwise.
		/// @warning Missing parameters will cause the
		/// software to exit. The only exception is
		/// the group "machines", which can be an empty
		/// json array if the software is run on a 
		/// single machine (no distributed job).
		/// @note Check the sample configuration
		/// file named "config.json".
		bool parse()
		{// parse metadata types
			try
			{
				_PathProj=_PT.get<std::string>(
					FIELD_PATH_PROJ);
				_PathOutput=_PT.get<std::string>(
					FIELD_PATH_OUTP);
				_FilePtable=_PT.get<std::string>(
					FIELD_FILE_PTAB);
				_FileInput=_PT.get<std::string>(
					FIELD_FILE_INPU);

				if (! parse_machines())
				{
					return false;
				}

				_Rate=_PT.get<double>(
					FIELD_RATE);
				_Cubes=_PT.get<size_t>(
					FIELD_SIZE);
				_Begin=_PT.get<int>(
					FIELD_BEGIN);

				_PrngEng=_PT.get<std::string>(
					FIELD_PRNG);
				_PrngSeed=_PT.get<size_t>(
					FIELD_SEED);
				_PrngTest=_PT.get<size_t>(
					FIELD_TEST);
				
				_DPFmu=_PT.get<double>(
					FIELD_DPFMU);
				_DPFb=_PT.get<double>(
					FIELD_DPFB);
				_DPGeps=_PT.get<double>(
					FIELD_DPGEPS);

				_CKD=_PT.get<double>(
					FIELD_CKD);
				_CKvar=_PT.get<double>(
					FIELD_CKVAR);
				_CKjs=_PT.get<double>(
					FIELD_CKjs);

				if (! parse_exp())
				{
					return false;
				}

				return true;
			}

			catch(...)
			{
				flush_msg(std::cout,
				M_ERROR,
				M_PARSE,
				M_CONF);

				return false;
			}
		}

		/// @brief get the "path.dir_project"
		/// @return const reference to standard 
		/// string (absolute path of the Confly
		/// project as cloned by Git or copied
		/// manually)
		auto get_path() const
			->const std::string&
		{// path of the project's code
			return _PathProj;
		}

		/// @brief get the "path.dir_output"
		/// @return const reference to standard 
		/// string (absolute path to write the 
		/// results)
		auto get_output_path() const 
			->const std::string&
		{// output path
			return _PathOutput;
		}

		/// @brief get the "path.file_input"
		/// @return const reference to standard 
		/// string (absolute path to the microdata
		/// file)
		auto get_input() const 
			->const std::string&
		{// input filename
			return _FileInput;
		}

		/// @brief get the "path.file_ptable"
		/// @return const reference to standard 
		/// string (absolute path to the cell-key 
		/// ptable file)
		auto get_ptable() const 
			->const std::string&
		{// ptable filename
			return _FilePtable;
		}

		/// @brief get the number of machines
		/// listed in the "machines" json array
		/// @return unsigned long
		auto get_machines() const 
			->size_t
		{// return number of 
			// configured machines
			// for this job
			return _HostNames
				.size();
		}

		/// @brief get the "machines[_MID]"
		/// @return standard string, the hostname
		/// or IP address of the machine listed
		/// in the "machines" array and identified
		/// by the array ordinal _MID
		auto get_machine(
            const size_t& _MID) 
            const 
			->std::string
		{// return the hostnames
			return _HostNames[_MID];
		}

		/// @brief copy the array of strings
		/// of the machines
		void get_machines(
            std::vector<std::string>& _Out) 
            const 
		{// return the hostnames
			_Out=_HostNames;
		}

		/// @brief retrieve the MID of the 
		/// local machine, if listed in the
		/// "machines" json array.
		/// @return signed int, identifying
		/// the position of the machine in
		/// the vector of strings, or -1 if
		/// the local host name is not found
		/// among those listed in the array.
		auto get_local_mid() const 
			->int 
		{// find the localhost 
			// in the vector of 
			// configured machines
			const std::string 
				_LocalHost(
					get_host_name());

			for (size_t i=0; i<
				_HostNames.size();
					++i)
			{
				if (_LocalHost==
					_HostNames[i])
					// found match
					return (signed)i;
			}

			// hostname not found,
			// or empty list of 
			// machines
			return -1;
		}

		/// @brief get the number of
		/// hypercubes to be created by the
		/// machine identified by MID
		/// @return signed int, corresponding
		/// to the "size" field if only one
		/// machine is scheduled for the task.
		/// Otherwise, split the load equally
		/// among all the listed machines and
		/// return how many hypercubes the
		/// local machine is tasked to compute.
		/// @note If MID identifies the last of the
		/// listed machines, it returns the sum
		/// of the local cubes plus the residual 
		/// cubes of the integer division (cubes
		/// \% machines).
		auto get_cubes_mid(
			const int& _MID)
			->int 
		{
			const size_t 
				_TotCubes(
					get_size());

			if (-1==_MID)
				// missing local 
				// host from the 
				// list
				return _TotCubes;

			// divide the load
			// equally (integer div)
			const size_t _LocalCubes(
				_TotCubes/_HostNames
					.size());

			// 0 to size-1
			if (_MID<_HostNames
				.size()-1)
				return _LocalCubes;

			// last machine takes
			// care of the remainder
			return _LocalCubes+
				_TotCubes%_HostNames
					.size();
		}

		/// @brief get the ordinal of the
		/// first hypercube to be generated
		/// by the local machine. Sum up the
		/// "begin" value specified in the 
		/// config file.
		/// @return unsigned long (ordinal
		/// of the cube).
		auto get_cubes_begin(
			const int& _MID) 
			const
			->size_t
		{
			// divide the load
			// equally (integer div),
			// sum up to the number
			// assigned to the first cube.
			if (-1==_MID)
				return _Begin;

			const size_t 
				_TotCubes(
					get_size());

			const size_t _LocalCubes(
				_TotCubes/_HostNames
					.size());

			return _Begin
				+_MID*_LocalCubes;
		}

		/// @brief get the total number
		/// of commissioned hypercubes.
		auto get_size() const
			->const size_t&
		{// get number of tot cubes
			return _Cubes;
		}

		/// @brief get the sampling rate
		/// of the microdata set ("rate"
		/// field of the configuration).
		/// @return const reference to double 
		auto get_rate() const
			->const double&
		{// get sampling rate
			return _Rate;
		}

		/// @brief get the initial seed
		/// provided in the "prng.seed"
		/// field of the config. 
		/// @return const reference to
		/// unsigned long
		/// @note this is not the seed
		/// used to seed the pseudo random
		/// generator, rather a hint. The
		/// actual seed will take the machine
		/// ID into account, in order to
		/// avoid trivial seed collisions
		/// among different machines.
		auto get_seed() const 
			->const size_t&
		{// get prng seed
			return _PrngSeed;
		}

		/// @brief get the size of the
		/// test for differential privacy
		/// distributions, as provided 
		/// in the "prng.test"
		/// field of the config. 
		/// @return const reference to
		/// unsigned long
		auto get_test() const 
			->const size_t&
		{// get prng size of test
			return _PrngTest;
		}

		/// @brief get the differential
		/// privacy (Laplace) location  
		/// @return const reference to double 
		auto get_DPFmu() const
			->const double&
		{// get Laplace location
			return _DPFmu;
		}

		/// @brief get the differential
		/// privacy (Laplace) scaling  
		/// @return const reference to double 
		auto get_DPFb() const
			->const double&
		{// get Laplace scaling
			return _DPFb;
		}

		/// @brief get the differential
		/// privacy (Geometric) privacy 
		/// budget (epsilon)  
		/// @return const reference to double 
		auto get_DPGeps() const
			->const double&
		{// get Geometric epsilon
			return _DPGeps;
		}

		/// @brief get the Cell-Key
		/// bound parameter  
		/// @return const reference to double 
		auto get_CKD() const
			->const double&
		{// get cellkey D
			return _CKD;
		}

		/// @brief get the Cell-Key
		/// variance parameter  
		/// @return const reference to double 
		auto get_CKvar() const
			->const double&
		{// get cellkey variance
			return _CKvar;
		}

		/// @brief get the Cell-Key
		/// small-count threshold parameter  
		/// @return const reference to double 
		auto get_CKjs() const
			->const double&
		{// get cellkey js
			return _CKjs;
		}

		/// @brief get a train of comma separated
		/// types needed in order to generate
		/// software plugins
		/// @return cref to std string
		auto get_type() const
			->const std::string&
		{// get comma separated
			// train of types
			return _Types;
		}

		/// @brief get a code identifying
		/// the types needed in order to generate
		/// software plugins. Useful to create
		/// plugin filenames and distiguish among
		/// each other.
		/// @return cref to std string
		auto get_typecode() const
			->const std::string&
		{// get typecode 
			return _TypeCode;
		}

		/// @brief get a bit mask in order
		/// to distinguish which of the 
		/// fields have to be aggregated only
		/// or not (ref. "micro.cube" field).
		auto get_mask() const
			->const size_t&
		{// get mask 
			return _Mask;
		}

	private:

		bool parse_machines()
		{
			const tree_type& _Grp(
				_PT.get_child(
					ARRAY_MACHINES));

			_Size=_Grp.size();

			if (0==_Size)
			{// no machines specified
				// keep on with localhost
				return true;
			}

			for (auto I=_Grp.begin(),
				E=_Grp.end(); I!=E;
					++I)
			{
				const tree_type& _Elem(
					I->second);

				// read "type"
				const std::string& 
					_Hostname(_Elem
						.get_value<
						std::string>());

				_HostNames.push_back(
					_Hostname);
			}

			return true;
		}

		bool parse_exp()
		{
			const tree_type& _Grp(
				_PT.get_child(
					FIELD_EXP));

			_Size=_Grp.size();

			if (0==_Size)
			{
				flush_msg(std::cout,
				M_ERROR, M_PARSE, M_CONF,
				M_METADATA,
				_Size,
				FIELD_EXP,
				M_VARIABLES);

				return false;
			}

			for (auto I=_Grp.begin(),
				E=_Grp.end(); I!=E;
					++I)
			{
				const tree_type& _Elem(
					I->second);

				// read "type"
				const std::string& _Ty( 
					_Elem.get<std::string>(
						ELEM_TYPE));

				auto T(I); ++T;
				if (! append_type(
					_Ty, T!=E))
				{// checks for synthax
					flush_msg(std::cout,
						M_ERROR, 
						M_PARSE, 
						M_CONF,
						M_METADATA,
						FIELD_EXP,
						_Ty);

					return false;
				}
			}

			auto E=_Grp.rend();--E;
			for (auto I=_Grp.rbegin(); 
				I!=E; ++I)
			{
				const tree_type& 
					_Elem(I->second);

				// read "cube"
				const bool& _BCube(
					_Elem.get<bool>(
						ELEM_CUBE));

				auto T(I); ++T;

				append_mask(
					_BCube, T!=E);
			}

			return true;
		}

		bool append_type(
			const std::string& _Ty,
			bool _BSep)
		{// helper for parsing the 
			// types specified in the
			// config 

			auto F=_MetaTypes
				.find(_Ty);
			auto FC=_TypeCodes
				.find(_Ty);

			if (F==_MetaTypes.cend() ||
				FC==_TypeCodes.cend())
				return false;

			_Types.append(F->second);

			if (_BSep) _Types
				.append(",");

			_TypeCode.append(1, 
				FC->second);

			return true;
		}

		void append_mask(
			bool _Bcube,
			bool _BSep)
		{
			if (! _Bcube)
				_Mask |= 1;

			if (_BSep)
				_Mask <<=1;
		}

		tree_type
			_PT;
		std::string
			_PathProj;
		std::string
			_PathOutput;
		std::string
			_FilePtable;
		std::string
			_FileInput;
		std::vector<std::string>
			_HostNames;
		double
			_Rate;
		size_t
			_Cubes;
		int
			_Begin;
		std::string 
			_PrngEng;
		size_t 
			_PrngSeed;
		size_t
			_PrngTest;
		double 
			_DPFmu;
		double
			_DPFb;
		double
			_DPGeps;
		double
			_CKD;
		double
			_CKvar;
		double
			_CKjs;
		size_t 
			_Size;
		std::string
			_Types;
		std::string
			_TypeCode;
		size_t
			_Mask;
	};
}