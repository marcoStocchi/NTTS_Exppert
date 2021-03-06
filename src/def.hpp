#pragma once

// ISO streams and manipulators
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

// ISO containers
#include <set>
#include <map>
#include <vector>

// ISO maths
#include <cmath>
#include <random>

// ISO exceptions
#include <stdexcept>

// misc
#include <typeinfo>

// Boost libraries 
#ifndef NOBOOSTLIBS // for unit tests
// Boost libraries workarounds to
// retain C++0x backward compatibility
#define BOOST_THREAD_USES_MOVE 
#define BOOST_NO_CXX11_RVALUE_REFERENCES 

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#endif

#include "secure_allocator.hpp"

// confly version
#define VERMAJOR               1
#define VERMINOR               0

// debug levels
#define BDEBUGNONE             0
#define BDEBUGLEVEL1           1
#define BDEBUGLEVEL2           2
#define BDEBUGLEVEL3           3
#define BDEBUGLEVEL4           4

// define the debug level here
#define BDEBUG                 BDEBUGNONE
//#define BDEBUG                 BDEBUGLEVEL3
//#define BDEBUG                 BDEBUGLEVEL2

// default constants
#define DEFSSHPORT             22
#define DEFSSHBUFSZ            1024
#define MD5_HASH_SIZE          16
#define SHA1_HASH_SIZE         20
#define SHA224_HASH_SIZE       28
#define SHA256_HASH_SIZE       32
#define SHA384_HASH_SIZE       48
#define SHA512_HASH_SIZE       64

#define S_MD5_HASH_SIZE        "MD5"
#define S_SHA1_HASH_SIZE       "SHA1"
#define S_SHA224_HASH_SIZE     "SHA224"
#define S_SHA256_HASH_SIZE     "SHA256"
#define S_SHA384_HASH_SIZE     "SHA384"
#define S_SHA512_HASH_SIZE     "SHA512"

// term output colors
#define RESET   			   "\033[0m"
#define BLACK                  "\033[30m"
#define RED                    "\033[31m"
#define GREEN                  "\033[32m"
#define YELLOW                 "\033[33m"
#define BLUE                   "\033[34m"
#define MAGENTA                "\033[35m"
#define CYAN                   "\033[36m"
#define WHITE                  "\033[37m"

// common chars
#define CHARSP                 ' '
#define CHARNL                 '\n'
#define CHARSC                 ';'
#define CHARCC                 ':'
#define CHARCM                 ','
#define CHARUS                 '_'
#define CHARSL                 '/'
#define CHARDT                 '.'

// internal folders
#define DIR_SRC                "src"
#define DIR_BUILD              "build"
#define DIR_DATA               "data"

// output filename prefixes
#define OF_CUBE                "cube"
#define OF_TRAV                "trv"
#define OF_ECDF                "ecdf"
#define OF_RAND                "rand"

// file extensions
#define DOT_TXT                ".txt"
#define DOT_CSV                ".csv"
#define DOT_PNG                ".png"
#define DOT_SAS                ".sas"
#define DOT_R                  ".r"

// logical to string
#define M_NOT                  "not"
#define M_AND                  "and"
#define M_OR                   "or"

// objects
#define M_ID                   "id"
#define M_ORDINAL              "ordinal"
#define M_REMOTE               "remote"
#define M_SERVER               "server"
#define M_MACHINE              "machine"
#define M_MACHINES             "machines"
#define M_CONNECTION           "connection"
#define M_AUTH                 "authentication"
#define M_CMD                  "command"
#define M_LINE                 "line"
#define M_ARGS                 "arguments"
#define M_HOST                 "host"
#define M_NAME                 "name"
#define M_LOCAL                "local"
#define M_PROD                 "confly"
#define M_VERSION              "version"
#define M_NUMBER               "number"
#define M_INTERNAL             "internal"
#define M_ERROR                "error"
#define M_SERVICE              "service"
#define M_RTTG                 "rttg"
#define M_DIR                  "directory"
#define M_FILE                 "file"
#define M_FILENAME             "filename"
#define M_INPUT                "input"
#define M_MICRO                "microdata"
#define M_TYPE                 "type"
#define M_TYPES                "types"
#define M_RECORD               "record"
#define M_RECORDS              "records"
#define M_TREE                 "tree"
#define M_CELLKEY              "cellkey"
#define M_HYCUBE               "hypercube"
#define M_HYCUBES              "hypercubes"
#define M_PTABLE               "ptable"
#define M_TOTAL                "total"
#define M_SECONDS              "seconds"
#define M_CONF                 "configuration"
#define M_METADATA             "metadata"
#define M_EXPLANATORY          "explanatory"
#define M_VARIABLES            "variables"
#define M_SAMPLE               "sample"
#define M_RATE                 "rate"
#define M_CONNECTOR            "connector"
#define M_SUPPORT              "support"
#define M_PASSWORD             "password"
#define M_BYTES                "bytes"
#define M_SSH                  "ssh"
#define M_SSHSESSION           "ssh session"
#define M_SSHCONNECT           "ssh connection"
#define M_SSHDISCONNECT        "ssh disconnected"
#define M_SSHVERIFY            "ssh verify"
#define M_SSHCHANNEL           "ssh channel"
#define M_SSHPUBKEY            "ssh server public key"
#define M_HASH                 "hash"
#define M_PUBKEY               "public key"
#define E_SSH_KNOWN_HOSTS_CHANGED     "host key for server changed"
#define E_SSH_KNOWN_HOSTS_OTHER       "The host key for this server was not found but another " \
                					  "type of key exists"
#define E_SSH_KNOWN_HOSTS_NOT_FOUND   "could not find known host file"
#define E_SSH_KNOWN_HOSTS_UNKNOWN     "the server is unknown"
#define E_SSH_READATTEMPT      "SSH read attempt"
#define E_SSHEXEC             "ssh exec"

// messages for running
#define M_RUNNING              "running"
#define M_ABORTING             "aborting"
#define M_ENTER                "enter"
#define M_EXIT                 "exit"
#define M_START                "starting"
#define M_STOP                 "stop"
#define M_CREATE               "create"
#define M_DESTROY              "destroy"
#define M_CONSTRUCT            "construct"
#define M_DESTRUCT             "destruct"
#define M_GENERATE             "generating"
#define M_OPEN                 "open"
#define M_CLOSE                "close"
#define M_CLOSING              "closing"
#define M_CONNECTING           "connecting"
#define M_DISCONNECTING        "disconnecting"
#define M_MAKE                 "making"
#define M_INIT                 "initializing"
#define M_DEINIT               "deinitializing"
#define M_READ                 "reading"
#define M_WRITE                "writing"
#define M_PARSE                "parsing"
#define M_LOOKUP               "lookup"
#define M_INSERT               "insertion"
#define M_REMOVE               "removing"
#define M_AGGREGATE            "aggregating"
#define M_FINALIZE             "finalizing"
#define M_PROCESS              "processing"
#define M_COMPILE              "compile"
#define M_CWD                  "change working directory"
#define M_GWD                  "get working directory"
#define M_SYSTEM               "system"
#define M_SYMBOL               "symbol"
#define M_MISSING              "missing"
#define M_ATTEMPTING           "attempting"
#define M_EXIST                "existing"
#define M_CHECK                "check"

// messages for completion
#define M_FOUND                "found"
#define M_LOADED               "loaded"
#define M_CONFIGURED           "configured"
#define M_UNINITIALIZED        "uninitialized"
#define M_FAILED               "failed"
#define M_ITERATED             "iterated"
#define M_SAMPLED              "sampled"
#define M_DONE                 "done"
#define M_UNIMPL               "unimplemented"
#define M_TRAVERSED            "traversed"
#define M_VERIFIED             "verified"
#define M_MISMATCH             "mismatch"
#define M_CONNECTED            "connected"
#define M_ALLOCATED            "allocated"
#define M_OCCURRED             "occurred"

// plugins
#define M_PLUGIN               "plugin"
#define M_RTTG_TYPES           "rttg_types"

// misc
#define M_ELLIP                "..."
#define M_THE                  "the"
#define M_IN                   "in"
#define M_OUT                  "out"
#define M_AS                   "as"
#define M_TO                   "to"
#define M_IS                   "is"
#define M_OF                   "of"
#define M_FROM                 "from"
#define M_OVER                 "over"
#define M_FOR                  "for"
#define M_CUMBER               "cumbersome"

// command line options
#define O_H                    "-h"
#define O_HELP                 "--help"
#define O_VERSION              "--version"
#define O_T                    "-t"
#define O_TEST                 "--test"
#define O_C                    "-c"
#define O_CONF                 "--conf"
#define O_CODELIST             "--code-list"
#define O_PTABLE               "--ptable"
#define O_AGGR                 "--aggregate"
#define O_NOTRAVERSE           "--no-traverse"
#define O_DIST                 "--distribute"
#define O_TRAV                 "--traverse"
#define O_PLOT                 "--plot"
#define O_RAND                 "--random"

// commands
#define C_HELP                 0
#define C_VERSION              1
#define C_TEST                 2
#define C_CODELIST             3
#define C_PTABLE               4
#define C_AGGREGATE            5
#define C_DISTRIBUTE           6
#define C_TRAVERSE             7
#define C_PLOT                 8
#define C_RAND                 9

// shell commands
#define SH_HOSTNAME            "hostname"
#define SH_PWD                 "pwd"
#define SH_CD                  "cd"

// R script and functions names
#define R_CRTPTABLE            "confly_crt_ptable"
#define R_PLOT_ECDF            "confly_plot_ecdf"
#define R_PLOT_ECDF_RAND       "confly_plot_ecdf_rand"

// import known types
#ifndef NOBOOSTLIBS
namespace confly
{
	typedef boost
		::property_tree::ptree
		tree_type;

	typedef boost::mutex
        mutex_type;

	typedef boost::lock_guard<
        mutex_type>
        lock_guard;
}
#endif
namespace confly
{
	typedef std::basic_string<char, 
		std::char_traits<char>,
		secure_allocator<char>>
		secure_string;
}