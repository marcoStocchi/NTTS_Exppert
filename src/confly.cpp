#include "confly.hpp"
#include "cluster.hpp"

#define confly_api 	confly:: 
#define service_api confly::service::

namespace confly
{
	namespace defaults 
	{// immutable strings
		const char* 
			CONFIG_FNAME(
			"config.json");

		const char* 
			TEST_INPUT(
			"data/hc_9_2_synth.csv.short");

		const char* 
			TEST_OUTPUT(
			"test.txt");

		const char*
			RSCRIPT_PTABLE(
			"crt_ptable.r");
	}

	namespace options
	{
		// configuration filename
		std::string 
			_ConfigFname(
				confly::defaults
				::CONFIG_FNAME);

		// if set, prevent 
		// traversing the cubes
		// in order to extract
		// the series of cells
		// at specific coordinates
		bool 
			_BNoTraverse(
				false);

		size_t
			_RandomTestDraws(1000);
	}
}

// -----------------------------------------
// allocate singletons

namespace confly
{
	// this ptable object lives
	// only in the address space
	// of the main binary.
	// rttg plugins have their
	// own ptable instance, and the 
	// content of this one must
	// be exported to them as
	// soon as the plugins are
	// loaded dynamically.
	// see plugin::import_ptable()
	ptable 
		_PTable;
}

// -----------------------------------------
// implement service class 

service_api service(
	const std::string& _cPathProject,
	const std::string& _cPathOutput,
	const std::string& _cTypes,
	const std::string& _cTypeCode,
	const size_t& _cMask)
	: _PathProject(_cPathProject)
	, _PathOutput(_cPathOutput)
	, _Types(_cTypes)
	, _TypeCode(_cTypeCode)
	, _Mask(_cMask)
	, _Plugin(
		_cPathProject,
		_cTypeCode,
		_cMask)
{
	init();
}

service_api ~ service()
{
	deinit();
}
		
void service_api init()
{// open/create-open plugin
	if (! _Plugin.good())
	{// plugin not found or
		// cannot get procedures
		fail(M_PLUGIN);

		flush_msg(std::cout,
				M_PLUGIN,
				M_NOT, M_FOUND,
				CHARCM,
				M_GENERATE,
				M_PLUGIN);

		// try to generate the
		// plugin at runtime
		// using GCC
		if (! plugin::create(
			_PathProject,
			_Types,
			_TypeCode))
		{
			fail(M_PLUGIN);

			flush_msg(std::cout,
				M_ERROR,
				M_PLUGIN,
				M_CREATE);
		}

		else
		{// plugin created
			_Plugin.reset();

			if (! _Plugin.open())
			{
				fail(M_PLUGIN);

				flush_msg(std::cout,
					M_ERROR,
					M_PLUGIN,
					_Plugin
					.last_error());
			}

			// got it, set 
			// service ok
			else reset();
		}
	}
}

void service_api deinit()
{	
}

auto service_api operator->() 
	->plugin*
{
	return &_Plugin;
}


// -----------------------------------------
// load config file

bool confly_api load_config(
	config& _Config,
	const std::string& _Fname)
{
	std::ifstream _Fin(
		_Fname);

	if (! _Fin.good() ||
		! _Config.load(_Fin))
	{
		flush_msg(std::cout,
			M_ERROR,
			M_READ,
			M_CONF);

		return false;
	}

	if (! _Config.parse())
	{
		flush_msg(std::cout,
			M_ERROR,
			M_PARSE,
			M_CONF);

		return false;
	}

	return true;
}

// -----------------------------------------
// read micro

bool confly_api read_micro(
	micro_base_ptr _Micro, 
	const std::string& _Fname)
{
	// protect against null ptr
	if (! _Micro)
		return false;

	// read the input file
	std::ifstream _Fin(_Fname);

	if (! _Fin.good())
	{// cannot open file
		flush_msg(std::cout,
			M_ERROR,
			M_OPEN,
			M_FILE,
			_Fname);

		return false;
	}

	if (! _Micro->read(_Fin, 
		CHARSC))
	{// error read operation
		flush_msg(std::cout,
			M_ERROR,
			M_READ,
			M_FILE,
			_Fname);

		return false;
	}

	return true;
}

// -----------------------------------------
// create cube filename

auto confly_api cube_fname(
	const std::string& _Path,
	const size_t _Idx)
	->std::string
{// e.g. path/to/cube_0.csv
	std::ostringstream 
		_OSS; _OSS 
		<< _Path << CHARSL
		<< OF_CUBE << CHARUS 
		<< _Idx << DOT_CSV;

	return _OSS.str();
}

// -----------------------------------------
// show version

auto confly_api show_version() 
	->int
{
	std::ostringstream _Oss; _Oss 
		<< VERMAJOR << CHARDT 
		<< VERMINOR;

	flush_msg(std::cout,
		M_PROD, 
		M_VERSION,
		_Oss.str());

	return 0;
}

// -----------------------------------------
// show usage

auto confly_api usage()
	->int
{
	::puts("");
	::puts("usage: confly [options]");
	::puts("");
	::puts("   -h    --help          show this message.");
	::puts("         --version       show version information.");
	::puts("   -c    --conf <file>   specify configuration file.");
	::puts("   -t    --test          test configuration.");
	::puts("         --code-list     create code lists files of");
	::puts("                         explanatory variables.");
    ::puts("         --ptable        create a ptable for cell-key");
    ::puts("                         noise");
	::puts("         --random        draw test random sequence");
	::puts("         --aggregate     sample and aggregate microdata");
	::puts("                         to create n hypercubes using");
	::puts("                         this machine. By default it");
	::puts("                         creates the traversing files;");
	::puts("                         use '--no-traverse' option to");
	::puts("                         create the hypercubes only.");
	::puts("         --distribute    distribute the aggregation to");
	::puts("                         multiple machines. On each");
	::puts("                         machine 'confly --aggregate");
	::puts("                         --no-traverse' is performed");
    ::puts("                         automatically.");
	::puts("         --traverse      read hypercubes and create");
	::puts("                         the traversing files.");
	::puts("");
	::puts("examples:");
	::puts("");
	::puts("./confly -c config.json --aggregate");
	::puts("./confly -c config.json --aggregate --no-traverse");
	::puts("./confly -c config.json --distribute");
	::puts("./confly -c config.json --traverse");
	::puts("");

	return 0;
}

// -----------------------------------------
// tests
#ifdef HAS_LIBSSH
auto confly_api test_ssh(
    const std::vector<std::string>& 
		_Hostnames)
	->int
{
	// test SSH to _Hostnames
	const bool _BVerbose(
		true);

	cluster _CL(_Hostnames);

	if (! _CL.create(_BVerbose))
	{
		flush_msg(std::cout,
			M_ERROR, 
			M_SSHSESSION,
			_CL.last_error());

		return 0;
	}

	if (! _CL.connect(_BVerbose))
	{
		flush_msg(std::cout,
			M_ERROR, 
			M_SSHSESSION,
			_CL.last_error());

		return 0;
	}

	std::ostringstream 
		_OSS;

	std::ostringstream 
		_CMD; _CMD 
		<< SH_HOSTNAME << CHARSC
		<< SH_HOSTNAME;

	if (! _CL.parallel_exec(
		_OSS,
		_CMD.str()))
	{
		flush_msg(std::cout,
			M_ERROR, 
			E_SSHEXEC,
			_CL.last_error());

		return 0;
	}

	std::string _Output(
		_OSS.str());
		
	for (size_t i=0; 
		i<_Hostnames.size(); ++i)
	{
		if (_Output.find(_Hostnames[i])
			!= std::string::npos)
		{// success
			color_flush_msg(
				GREEN,
				std::cout,
				M_HOST, 
				_Hostnames[i],
				M_VERIFIED);
		}

		else
		{// hostname mismatch
			color_flush_msg(
				RED,
				std::cout,
				M_ERROR, 
				M_HOST, M_NAME,
				_Hostnames[i],
				M_MISMATCH);
		}
	}

    return 0;
}
#else
auto confly_api test_ssh(
    const int& _Machines,
    const std::vector<std::string>& _Hostnames)
	->int
{
	flush_msg(std::cout,
        M_ERROR,
		M_MISSING, M_SSH,
        M_SUPPORT);

    return 0;
}
#endif

auto confly_api test_config()
	->int
{
	config _Config; if (! 
		load_config(_Config,
		options::_ConfigFname))
	{// config file not found
		// or config parsing error
		return 0;
	}

	const int _Machines(
		_Config.get_machines());

	std::vector<std::string>
		_Hostnames; _Config
		.get_machines(
			_Hostnames);

	// get the assigned index
	// to this machine
	const int _MID=_Config
		.get_local_mid();

	if (-1==_MID)
	{
		flush_msg(std::cout,
			M_LOCAL, M_HOST,
			M_MISSING,
			M_IN, M_CONF,
			M_FILE);
	}

	else
	{
		flush_msg(std::cout,
			M_LOCAL, M_HOST,
			M_CONFIGURED,
			M_AS, 
			M_ID, _MID, 
			M_OF, _Machines, M_MACHINES);
	}

	const int _LocalCubes(
		_Config.get_cubes_mid(
			_MID));

	if (-1!=_MID)
	{
		flush_msg(std::cout, 
			M_LOCAL, M_HOST, M_IS,
			M_TO, M_CREATE, 
			_LocalCubes,
			M_TOTAL, M_HYCUBES);
	}

    test_ssh(_Hostnames);

	return 0;
}

// -----------------------------------------
// parse cmd line options

auto confly_api cmdline_opts(
	int argc, 
	char** argv) 
	->int
{// parse cmd line arguments
	int _Cmd(C_HELP);

	if (argc==1)
		return C_HELP;

	for (int i=1; i<argc;
		++i)
	{
		if (! strcmp(
			argv[i], O_H) ||
			! strcmp(
			argv[i], O_HELP))
			// --help
			return C_HELP;

		else if (! strcmp(
			argv[i], O_VERSION))
			// --version
			return C_VERSION;

		if (! strcmp(
			argv[i], O_T) ||
			! strcmp(
			argv[i], O_TEST))
		{// --test
			_Cmd=C_TEST;
		}

		else if (! strcmp(
			argv[i], O_C) ||
			! strcmp(
			argv[i], O_CONF))
		{// --conf
			++i; if (i==argc)
			{
				flush_msg(
					std::cout,
					O_CONF,
					M_MISSING,
					M_CONF,
					M_FILENAME);
				return C_HELP;
			}

			options
			::_ConfigFname=
				argv[i];
		}

		else if (! strcmp(
			argv[i], O_NOTRAVERSE))
		{// --no-traverse
			options
			::_BNoTraverse=
				true;
		}

		else if (! strcmp(
			argv[i], O_CODELIST))
			// --code-list
			_Cmd=C_CODELIST;

		else if (! strcmp(
			argv[i], O_RAND))
			// --random
			_Cmd=C_RAND;

		else if (! strcmp(
			argv[i], O_PTABLE))
			// --ptable
			_Cmd=C_PTABLE;

		else if (! strcmp(
			argv[i], O_AGGR))
			// --aggregate
			_Cmd=C_AGGREGATE;

		else if (! strcmp(
			argv[i], O_DIST))
			// --distribute
			_Cmd=C_DISTRIBUTE;

		else if (! strcmp(
			argv[i], O_TRAV))
			// --traverse
			_Cmd=C_TRAVERSE;

		else if (! strcmp(
			argv[i], O_PLOT))
			// --plot
			_Cmd=C_PLOT;

		// cli opt not found
		else return C_HELP;
	}

	return _Cmd;
}

// -----------------------------------------
// code lists

auto confly_api code_lists()
	->int 
{// TBD
	return 0;
}

// -----------------------------------------
// create ptable

auto confly_api crt_ptable()
	->int 
{
	config _Config; if (! 
		load_config(_Config,
		options::_ConfigFname))
	{// config file not found
		// or config parsing error
		return 0;
	}

	std::ostringstream 
		_OSS; _OSS 
		<< _Config.get_path()
		<< CHARSL << DIR_SRC;

	change_dir _CD(_OSS.str()); 
		if (! _CD.good())
	{
		flush_msg(std::cout, 
			M_ERROR, 
			M_CWD,
			_OSS.str());
		return 0;
	}

	if(! create_ptable(
		R_CRTPTABLE DOT_R,
		_Config.get_ptable(), 
		_Config.get_CKD(),
		_Config.get_CKvar(),
		_Config.get_CKjs()))
	{
		flush_msg(std::cout, 
			M_ERROR, 
			M_PTABLE,
			M_CREATE);
		return 0;
	}

	return 0;
}

// -----------------------------------------
// run to aggregate microdata 

auto confly_api aggregate()
	->int
{// running aggregation...
	const std::string _HName(
		get_host_name());

	// tell SSH client 
	// (distributing machine)
	// to start logging output
	flush_msg(std::cout,
			_HName);

	config _Config; if (! 
		load_config(_Config,
		options::_ConfigFname))
	{// config file not found
		// or config parsing error
		return 0;
	}

	if (! load_ptable(
		_PTable,
		_Config.get_ptable()))
	{// error
		return 0;
	}

	// tot number of commissioned
	// cubes among different machines
	const size_t&
		_CubeSz(_Config
			.get_size());

	// sampling rate
	const double& 
		_SampleRate(_Config
			.get_rate());

	// get the number of tasked
	// machines. 0 intepreted as
	// local host only.
	const int _Machines(
		_Config.get_machines());

	// get the assigned index
	// to this machine
	const int _MID=_Config
		.get_local_mid();

	if (-1==_MID)
	{// no change in _CubeBeg
		flush_msg(std::cout,
			_HName, 
			M_HOST, M_NAME, M_IS,
			M_MISSING,
			M_IN, M_THE, M_CONF,
			M_FILE);
		// job is intended to be
		// performed only on the
		// local host.
	}

	else
	{// start from specific cube#
		flush_msg(std::cout,
			_HName,
			M_CONFIGURED,
			M_AS, 
			M_ID, _MID, // _MID=starting cube#
			M_OF, _Machines, M_MACHINES);
	}

	// number of cubes to be created
	// by the local machine
	const int _LocalCubes(
		_Config.get_cubes_mid(
			_MID));

	// prepare filename id
	// for the 1st cube
	size_t _CubesBeg(
		_Config.get_cubes_begin(
			_MID));

	flush_msg(std::cout, 
		_HName, M_IS,
		M_TO, M_CREATE, 
		_LocalCubes, M_OF, 
		M_TOTAL, _CubeSz, M_HYCUBES, 
		M_START, M_FROM, M_ORDINAL, _CubesBeg);

	service _Srv(
		_Config.get_path(),
		_Config.get_output_path(),
		_Config.get_type(),
		_Config.get_typecode(),
		_Config.get_mask());
	
	if (! _Srv.good())
	{// plugin creation failed
		flush_msg(std::cout,
			M_ERROR,
			M_SERVICE);

		return 0;
	}

	// transfer ptable to the
	// plugin
	_Srv->import_ptable(
		_PTable);

	// transfer CK and DP
	// parameters to the
	// plugin
	_Srv->import_noise_params(
		_Config.get_DPFmu(),
		_Config.get_DPFb(),
		_Config.get_DPGeps());

	// allocate traversing vectors
	// only if traversing is requested.
	// this causes the response
	// static accumulator to allocate
	// a vector of _CubeSz tuples!
	//
	// when distributing the task of 
	// creating many cubes among
	// different machines, the option
	// --no-traverse must always be
	// specified, since there is no
	// point in traversing only a 
	// subset of cubes. 
	if (! options::_BNoTraverse)
		// = yes traverse
		_Srv->import_size(_CubeSz);

	// seed prng summing the provided
	// seed-value in the config file
	// and '_CubesBeg' (i.e. the number 
	// of the first cube worked out
	// on this machine).
	// if only the local machine is used,
	// _CubesBeg=0 thus the PRNG is seeded
	// with the value provided in the
	// config.
	_Srv->seed_prng(
		_Config.get_seed()+
		_CubesBeg);

	// create microDB
	micro_base_ptr _DB(
		_Srv->make_micro());

	// read the microdata
	// file, or fail and stop.
	if (! read_micro(_DB, 
		_Config.get_input()))
	{
		flush_msg(std::cout,
			M_ERROR,
			M_READ,
			M_MICRO,
			M_INPUT);
		return 0;
	}

	// start timing machine's
	// total processing time.
	stopwatch _SWTotal;

	flush_msg(std::cout,
		_HName, M_START, 
		M_AGGREGATE, 
		M_ELLIP);

	// preallocate cubes
	std::vector<hypercube_base_ptr>
		_Cubes; for (size_t i=0; 
			i<_LocalCubes; ++i)
	{
		_Cubes.push_back(
			hypercube_base_ptr(
				_Srv->make_hypercube()));
	}

	#pragma omp parallel for
	for (size_t i=0; i<_LocalCubes;
		++i)
	{// generate _CubeSz hypercubes
		//stopwatch _SW;

		hypercube_base_ptr _Cube(
			_Cubes[i]);

		// when traversing, each
		// cube updates the vector
		// of values of specific 
		// coordinates at its ID 
		// position.
		// if not traversing, the
		// cube id is not used.
		_Cube->set_id(i);

		const std::string _Fname(
			cube_fname(_Config
				.get_output_path(), 
				i+_CubesBeg));

		// service->plugin->sample_micro
		if (! _Srv->sample_micro(
			_DB.get(),
			_Cube.get(),
			_SampleRate,
			_Fname))
		{// internal error
			flush_msg(std::cout,
				M_INTERNAL,
				M_ERROR);
		}

		flush_msg(std::cout,
			_HName, CHARCC, 
			M_HYCUBE, i+_CubesBeg, 
			M_DONE);
			//_SW.stop(),
			//M_SECONDS);
	}
	#pragma omp barrier

	color_flush_msg(GREEN,
		std::cout,
		_HName, CHARCC,
		_LocalCubes, M_HYCUBES, 
		M_DONE, M_IN, 
		_SWTotal.stop(),
		M_SECONDS);

	if (options
		::_BNoTraverse)
	{// all done
		debug_msg<3>(
			M_HYCUBES, 
			M_NOT,
			M_TRAVERSED);

		return 0;
	}

	// traversing requested
	_Srv->write_traversing(
		_DB.get(),
		_Config.get_output_path());

	return 0;
}

// -----------------------------------------
// distribute aggregate() jobs among
// several different machines

#ifdef HAS_LIBSSH
auto confly_api distribute()
	->int
{
	const bool _BVerbose(
		false);

	config _Config; if (! 
		load_config(_Config,
		options::_ConfigFname))
	{// config file not found
		// or config parsing error
		return 0;
	}

	service _Srv(
		_Config.get_path(),
		_Config.get_output_path(),
		_Config.get_type(),
		_Config.get_typecode(),
		_Config.get_mask());
	
	if (! _Srv.good())
	{// plugin creation failed
		flush_msg(std::cout,
			M_ERROR,
			M_SERVICE);

		return 0;
	}

	// get hostnames of machines
	std::vector<std::string>
		_Hostnames; _Config
		.get_machines(
			_Hostnames);

	const int 
		_Machines(
		_Hostnames.size());

	const size_t&
		_CubeSz(_Config
			.get_size());

	if (_Machines > _CubeSz)
	{// more machines than cubes
		flush_msg(std::cout,
			_Machines, M_MACHINES,
			M_ALLOCATED, M_FOR,
			_CubeSz, M_HYCUBES,
			M_IS, M_CUMBER, CHARDT);

		flush_msg(std::cout,
			M_CHECK, M_CONF, M_FILE);

		return 0;
	}

	cluster _CL(_Hostnames);

	if (! _CL.create(_BVerbose))
	{
		flush_msg(std::cout,
			M_ERROR, 
			M_SSHSESSION,
			_CL.last_error());

		return 0;
	}

	if (! _CL.connect(_BVerbose))
	{
		flush_msg(std::cout,
			M_ERROR, 
			M_SSHSESSION,
			_CL.last_error());

		return 0;
	}

	flush_msg(std::cout,
		M_MACHINES,
		M_CONNECTED);

	// prepare command
	std::ostringstream 
		_CMD; _CMD 
	<< SH_CD << CHARSP
	<< _Config.get_path() << CHARSC << CHARSP
	<< M_PROD << CHARSP
	<< O_C << CHARSP 
	<< options::_ConfigFname << CHARSP
	<< O_AGGR << CHARSP 
	<< O_NOTRAVERSE;

	color_flush_msg(
		YELLOW, std::cout, 
		M_RUNNING, M_REMOTE,
		M_CMD, _CMD.str(),
		M_ELLIP, CHARNL);

	stopwatch _SWTotal;

	if (! _CL.parallel_exec(
		std::cout,
		_CMD.str()))
	{
		flush_msg(std::cout,
			M_ERROR, 
			E_SSHEXEC,
			_CL.last_error());

		return 0;
	}

	if (_CL.good())
	{// alright
		color_flush_msg(
			YELLOW, std::cout,
			_CubeSz, M_HYCUBES, 
			M_DONE, M_IN, 
			_SWTotal.stop(),
			M_SECONDS);
	}

	else
	{// at least 1 error
		// and test failed. 
		color_flush_msg(
			RED, std::cout,
			M_ERROR, M_OCCURRED,
			M_IN, M_MACHINE,
			_CL.last_error());
	}

	return 0;
}
#else 
auto confly_api distribute()
	->int
{
	flush_msg(std::cout,
        M_ERROR,
		M_MISSING, M_SSH,
        M_SUPPORT);

    return 0;
}
#endif

auto confly_api traverse()
    ->int
{
	config _Config; if (! 
		load_config(_Config,
		options::_ConfigFname))
	{// config file not found
		// or config parsing error
		return 0;
	}

	if (options::_BNoTraverse)
    {// "--traverse --no-traverse"
		flush_msg(std::cout,
			M_ERROR,
			O_TRAV,
			O_NOTRAVERSE,
            M_IS, M_CUMBER, CHARDT,
            M_CHECK, M_CMD, M_LINE,
            M_ARGS);

        return 0;
    }

	service _Srv(
		_Config.get_path(),
		_Config.get_output_path(),
		_Config.get_type(),
		_Config.get_typecode(),
		_Config.get_mask());
	
	if (! _Srv.good())
	{// plugin creation failed
		flush_msg(std::cout,
			M_ERROR,
			M_SERVICE);

		return 0;
	}

	// tot number of commissioned
	// cubes
	const size_t&
		_CubeSz(_Config
			.get_size());

    _Srv->import_size(_CubeSz);

	// preallocate cubes
	std::vector<hypercube_base_ptr>
		_Cubes; for (size_t i=0; 
			i<_CubeSz; ++i)
	{
		_Cubes.push_back(
			hypercube_base_ptr(
				_Srv->make_hypercube()));
	}

    stopwatch _SW;

    // no parallel (read_cube::read
    // accesses/creates buckets on the
    // traversing map)
	for (size_t i=0; i<_CubeSz;
		++i)
	{// generate _CubeSz hypercubes
		hypercube_base_ptr _Cube(
			_Cubes[i]);

		// when traversing, each
		// cube updates the vector
		// of values of specific 
		// coordinates at its ID 
		// position.
		_Cube->set_id(i);

		const std::string _Fname(
			cube_fname(_Config
				.get_output_path(), 
				i));

        std::ifstream _Fin(
            _Fname); if(_Fin
                .good())
        {
            if (! _Cube
                ->read(_Fin))
            {// read error, stop
                flush_msg(std::cout,
                    M_ERROR, 
                    M_READ, 
                    M_HYCUBE,
                    i, 
                    M_ABORTING);

                return 0;
            }
        }
	}

    _Cubes[0]->write_traversing(
        _Config.get_output_path());

    flush_msg(std::cout,
        M_TRAVERSED, _CubeSz, 
        M_HYCUBES, M_DONE, M_IN,
        _SW.stop(), M_SECONDS);

	return 0;
}

auto confly_api plot()
    ->int
{
	config _Config; if (! 
		load_config(_Config,
		options::_ConfigFname))
	{// config file not found
		// or config parsing error
		return 0;
	}

    std::vector<std::string>
        _TrvFnames;

    if (! dir_lookup(
        _Config
            .get_output_path(),
            OF_TRAV, 0,
                _TrvFnames))
    {
        flush_msg(std::cout, 
            M_ERROR,
            M_DIR,
            M_LOOKUP);
        return 0;
    }

    // change working directory
    // to 'src'
	std::ostringstream 
		_OSS; _OSS 
		<< _Config.get_path()
		<< CHARSL << DIR_SRC;

	change_dir _CD(_OSS.str()); 
		if (! _CD.good())
	{
		flush_msg(std::cout, 
			M_ERROR, 
			M_CWD,
			_OSS.str());
		return 0;
	}

    // create R connector
    cnectR _Connector(
        R_PLOT_ECDF DOT_R); if (! 
            _Connector.good())
    {
        flush_msg(std::cout, 
            M_ERROR,
            M_CONNECTOR,
            M_OPEN);

        return 0;
    }

    // iterate-plot all the
    // traversing files
    for (size_t i=0; i<
        _TrvFnames.size(); 
            ++i)
    {
        const size_t _DOT
            =_TrvFnames[i]
            .rfind(DOT_CSV);

        if (std::string::npos
            ==_DOT)
        {// file ext not found
            // in what is supposed
            // to be a .csv file
            flush_msg(std::cout, 
                M_ERROR,
                M_FILENAME);

            return 0;
        }

        // compose filename for
        // ecdf .png image
        std::ostringstream 
            _OSS; _OSS 
            << OF_ECDF 
            << CHARUS
            << _TrvFnames[i]
                .substr(4, 
                    _DOT-4)
            << DOT_PNG;

        std::string _PngFname(
            _OSS.str());

        // call R script to plot
        // the ecdf and export 
        // its image
        if (! _Connector.call(
            R_PLOT_ECDF,
            _Config
                .get_output_path(),
            _TrvFnames[i],
            _PngFname))
        {
            flush_msg(std::cout, 
                M_ERROR,
                M_CONNECTOR);
        }

		flush_msg(std::cout,
			_PngFname, 
			M_DONE);
    }

    return 0;
}

auto confly_api random_draws()
    ->int
{
	config _Config; if (! 
		load_config(_Config,
		options::_ConfigFname))
	{// config file not found
		// or config parsing 
        // error
		return 0;
	}

    laplace_distribution
        _Laplace(
		_Config.get_DPFmu(),
		_Config.get_DPFb());

    geometric_distribution
        _Geometric(
        0, 
		_Config.get_DPGeps());

    mersenne_twister _MT;

    _MT.seed(
		_Config.get_seed());

    size_t _RSz(_Config
		.get_test());

	if (0==_RSz) 
		_RSz=options::
		_RandomTestDraws; // 1000


    std::string 
        _CsvFname,
        _PngFname;

    {
        std::ostringstream
            _OSS; _OSS 
        << OF_RAND 
        << CHARUS
        << _RSz;

        std::ostringstream
            _OSS1, _OSS2; 
            _OSS1
        << _OSS.str()
        << DOT_CSV;
            _OSS2
        << _OSS.str()
        << DOT_PNG;

        _CsvFname=_OSS1.str();
        _PngFname=_OSS2.str();
    }

    // create csv file with 
    // _RSz random pairs

    {
        std::ostringstream
            _OSS; _OSS 
        << _Config
            .get_output_path()
        << CHARSL << _CsvFname;

        std::ofstream _Fout(
            _OSS.str());

        for (size_t i=0; 
            i<_RSz; ++i)
        {
            _Fout 
            << _Laplace(_MT) 
            << CHARSC 
            << _Geometric(_MT) 
            << CHARNL;
        }
    }

    // change working directory
    // to 'src'
	std::ostringstream 
		_OSS; _OSS 
		<< _Config.get_path()
		<< CHARSL << DIR_SRC;

	change_dir _CD(_OSS.str()); 
		if (! _CD.good())
	{
		flush_msg(std::cout, 
			M_ERROR, 
			M_CWD,
			_OSS.str());
		return 0;
	}

    // create R connector
    cnectR _Connector(
        R_PLOT_ECDF_RAND DOT_R); 

    if (! _Connector.good())
    {
        flush_msg(std::cout, 
            M_ERROR,
            M_CONNECTOR,
            M_OPEN);

        return 0;
    }

    // call R script to plot
    // the ecdf and export 
    // its image
    if (! _Connector.call(
        // function sig
        R_PLOT_ECDF_RAND,
        // output path
        _Config
            .get_output_path(),
        // filenames
        _CsvFname, _PngFname,
        // size of draws
        _RSz, 
        // noise params pack
        _Config
            .get_DPFmu(),
		_Config
            .get_DPFb(),
		_Config
            .get_DPGeps()
        ))
    {
        flush_msg(std::cout, 
            M_ERROR,
            M_CONNECTOR);
    }

    flush_msg(std::cout,
        _PngFname, 
        M_DONE);

    return 0;
}


