#include "confly.hpp"


int main(int argc, char** argv)
{
	using namespace confly;

	switch(cmdline_opts(
		argc, argv))
	{
        case C_DISTRIBUTE:
            // distribute aggregate
            // among different machines
            return distribute();

		case C_AGGREGATE: 
            // make the cubes. The default
            // behaviour is to traverse
            // the cubes and write the
            // traversed response.
            // When distributing the job,
            // traversing on the fly
            // is not feasible, and it
            // must be suppressed using the
            // '--aggregate --no-traverse'
            // cmd line option.
			return aggregate();

        case C_TRAVERSE:
            // load pre-made cubes
            // and make the traversed
            // response
            return traverse();

        case C_PLOT:
            return plot();

		case C_VERSION:
			return show_version();
		
		case C_CODELIST:
			return code_lists();

		case C_PTABLE:
			return crt_ptable();

		case C_TEST:
			return test_config();

        case C_RAND:
            return random_draws();

		case C_HELP:
		default:
			return usage();
	}

	return 0;
}