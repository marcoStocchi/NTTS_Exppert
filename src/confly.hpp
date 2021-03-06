#include "def.hpp"
#include "shared.hpp"
#include "config.hpp"
#include "cluster.hpp"
#include "plugin.hpp"
#include "ptable.hpp"

namespace confly
{
	namespace defaults 
	{// immutable strings
		extern const char* 
			CONFIG_FNAME;

		extern const char* 
			TEST_INPUT;

		extern const char* 
			TEST_OUTPUT;
	}

    namespace options
    {
        extern std::string 
            _ConfigFname;

        extern bool 
            _BNoTraverse;
    }

    /// @brief utility class to hold
    /// a plugin and its configuration
    /// parameters.
	class service
		: public enable_internal_error
	{// wrap a plugin and provide basic
        // plugin init-deinit
	
	public:
	   
        /// @brief construct with parameters.
        /// Init automatically.
		service(
			const std::string& _cPathProject,
			const std::string& _cPathOutput,
			const std::string& _cTypes,
			const std::string& _cTypeCode,
            const size_t& _cMask);

        /// @brief Deinit and destroy.
		~ service();

        /// @brief Try open the plugin
        /// identified by the supplied 
        /// typecode (see constructor).
        /// if not found, create the
        /// plugin from scratch.
        /// fail internally if the 
        /// plugin is not successfully
        /// loaded at the end.
		void init();

        /// @brief deinit plugin.
		void deinit();

        /// @brief direct access to
        /// the plugin oject. 
		auto operator->() 
			->plugin*;
		
        
	private:

		std::string 
			_PathProject;
        std::string
            _PathOutput;
		std::string 
			_Types;
		std::string 
			_TypeCode;
        size_t
            _Mask;
		plugin
			_Plugin;
	};

    // ------------------------------------------
    // smart pointer types to plugin objects

    typedef boost::shared_ptr<
        micro_base>
        micro_base_ptr;
    typedef boost::shared_ptr<
        hypercube_base>
        hypercube_base_ptr;

    // ------------------------------------------
    // helper functions for main routine

    bool load_config(
        config& _Config,
        const std::string& _Fname);

    bool read_micro(
        micro_base_ptr _Micro, 
        const std::string& _Fname);

    auto cube_fname(
        const std::string& _Path,
        const size_t _Idx)
        ->std::string;

    // parse cmd line arguments
    auto cmdline_opts(
        int argc, 
        char** argv) 
        ->int;

    // show usage instructions
    auto usage()
        ->int;

    // show version
    auto show_version() 
        ->int;

    // test ssh connections
    auto test_ssh(
        const std::vector<std::string>&)
	->int;

    // test configuration
    auto test_config()
        ->int;

    // random draws
    auto random_draws()
        ->int;

    // TODO
    // generate code list files
    auto code_lists()
        ->int;

    // create ptable
    auto crt_ptable()
        ->int;

    // run aggregation on a 
    // single machine
    auto aggregate()
        ->int;

    // distribute a job in order
    // to aggregate a large number
    // of cubes.
    auto distribute()
        ->int;

    // reduce a large number of
    // cubes into traversing files.
    auto traverse()
        ->int;

    // plot the ecdfs from all
    // the traversing files found
    // in the output path
    auto plot()
        ->int;
}