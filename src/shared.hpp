#pragma once
#define MAX_PATH_LEN    0x0100

#include <iostream>
#include <time.h>
#include <unistd.h>
#include <dirent.h>

#include "def.hpp"

// ----------------------------------------------
// utils for console output


namespace details
{
	template <class T, class... _Types>
	inline void _printmsg(std::ostream& _S, 
		const T& _Val)
	{// stop recursion
		_S << _Val << "\n";
	}

	template <class T, class... _Types>
	inline void _flushmsg(std::ostream& _S, 
		const T& _Val)
	{// stop recursion
		_printmsg(_S, _Val); _S.flush();
	}

	template <class T, class... _Types>
	inline void _flushmsg(std::ostream& _S, 
		const T& _Val, _Types... _Args)
	{// recurr
		_S << _Val << " "; 
			_flushmsg(_S, _Args...);
	}

	template <class T, class... _Types>
	inline void _printmsg(std::ostream& _S, 
		const T& _Val, _Types... _Args)
	{// recurr
		_S << _Val << " "; 
			_printmsg(_S, _Args...);
	}
}

// ----------------------------------------------
// console out flush, print and debug msg

template <class... _Types>
inline void flush_msg(std::ostream& _S, 
	_Types... _Args)
{// start recursion
	details::_flushmsg(_S, _Args...);
}

inline void exclusive_flush_msg(
	std::ostream& _S, 
	const std::string& _Val)
{// mutual exclusive writes
	static confly::mutex_type 
		_MStream;

	confly::lock_guard 
		_Lockit(_MStream);

	_S << _Val;
}

template <class... _Types>
inline void print_msg(std::ostream& _S, 
	_Types... _Args)
{// start recursion
	details::_printmsg(_S, _Args...);
}

template <size_t L, class... _Types>
inline void debug_msg(
	_Types... _Args)
{// start recursion
    if (BDEBUG>=L)
        flush_msg(std::cout,
        _Args...);
}

template <class... _Types>
inline void color_flush_msg(
	const char* _Clcode,
	std::ostream& _S, 
	_Types... _Args)
{// write colored, reset and flush
	_S << _Clcode;

	details::_printmsg(_S, _Args...);

	_S << RESET;

	_S.flush();
}

// ----------------------------------------------
// class utils

/// @brief base class for classes
/// that keep an internal error state
class enable_internal_error
{
protected:

	/// @brief construct setting
	/// the state as good.
	enable_internal_error()
		: _Good(true)
		, _StrErr()
	{}

public:

	/// @brief get the state
	/// @return true for good,
	/// false otherwise.
	bool good() const 
	{
		return _Good;
	}

	/// @brief set the state to
	/// false, set the supplied
	/// string as reason for fail.
	void fail(
		const std::string& _Str)
	{
		_StrErr=_Str;

		_Good=false;
	}

	/// @brief reset the state
	/// to good, clear the reason
	/// for fail.
    void reset()
    {
        _StrErr.clear();

        _Good=true;
    }

	/// @brief get the reason for
	/// fail.
	auto last_error() const
		->std::string
	{
		return _StrErr;
	}

private:

	bool
		_Good;
	std::string
		_StrErr;
};

// ----------------------------------------------
// fsutils

inline auto get_cwd() 
	->std::string
{// get current working dir.
	// construct/size/assign
	std::string _Res(
		MAX_PATH_LEN+1, 0);

	// try to get cdw
	const char* _Ptr( 
		::getcwd(&_Res[0], 
			_Res.size()-1));

	// on failure 
	// getcwd returns 0
	if (! _Ptr) return "";

	// extra paranoia
	_Res[MAX_PATH_LEN]=0;

	return _Res.c_str();
}

/// @brief utility class to change
/// working directory and restore
/// the previous one on destruction.
struct change_dir
    : enable_internal_error
{
	/// @brief construct with the
	/// new path as parameter, 
	/// change working directory
	/// automatically.
	change_dir(
		const std::string& _Path)
		: _New(_Path)
		, _Old(get_cwd())
	{
		if (_Old.empty())
			fail(M_GWD);

		else change();
	}

	/// @brief resume the previous
	/// working directory and destroy
	~ change_dir()
	{
		if (good()) 
			resume();
	}

private:

	void change() 
	{
		if (-1==::chdir(
			_New.c_str()))
			fail(M_CWD);
	}

	void resume()
	{
		// reset working dir
		:: chdir(_Old.c_str());
	}

	std::string 
		_New;
	std::string 
		_Old;
};

template <class _Cont>
inline bool dir_lookup(
    const std::string& _Path, 
    const std::string& _MatchStr,
    const size_t& _MatchPos,
    _Cont& _Out)
{// use containers such as
    // std::set<_StrTy> or 
    // std::vector<_StrTy>
    DIR* _DirPtr(0); 
    
    struct dirent *_DPtr(0);

    _Out.clear(); if ((_DirPtr=
        ::opendir(_Path.c_str()))
            == NULL) return false;

    do 
    {
        if ((_DPtr=::readdir(
            _DirPtr)) != NULL)
        {
            if (! strcmp(
                _DPtr->d_name, ".") ||
                ! strcmp(
                _DPtr->d_name, "..")) 
                continue;

            std::string _Name(
                _DPtr->d_name);

            if (_MatchPos==
                _Name.find(_MatchStr))
                    _Out.insert(
                        _Out.end(),
                        _Name);
        }

    } while (_DPtr != NULL);

    ::closedir(_DirPtr);

    return true;
}

inline auto path_ext(
    const std::string& _Path)
    -> std::string
{// .xxx
    return _Path.substr(_Path
        .rfind('.'));
}

// ----------------------------------------------
// mem utils

template <class T>
void safe_delete(T*& _Ptr)
{
	if (_Ptr)
	{
		delete _Ptr;

		_Ptr=0;
	}
}

// ----------------------------------------------
// time utils

/// @brief utility class to measure
/// elapsed time while executin routines.
struct stopwatch
{
	/// @brief construct and log
	/// the start time internally.
    stopwatch()
    {
        get(_Start);
    }

	/// @brief log the stop time
	/// internally and return the
	/// elapsed time.
    auto stop()
        ->double
    {
        get(_Stop);

        return (double)elapsed(
            _Start, _Stop)/
            1000000000.0;
    }

private:

    void get(timespec& _TS)
    {
        ::clock_gettime(
            CLOCK_MONOTONIC,
            &_TS);
    }

    auto elapsed(
        timespec& _TS1, // before
        timespec& _TS2) // after
        ->int64_t
    {
        return 
        (int64_t)(_TS2.tv_sec
            -_TS1.tv_sec)
            *(int64_t)1000000000UL
            +(int64_t)(_TS2.tv_nsec
            -_TS1.tv_nsec);
    }

    void clear(timespec& _TS)
    {
        _TS.tv_sec=0;
        _TS.tv_nsec=0;
    }

    timespec
        _Start;
    timespec
        _Stop;
};

// ----------------------------------------------
// network utils

inline auto get_host_name()
    ->std::string
{
	static const size_t 
        _SZ(256);

	char _Buf[_SZ]={0};

	::gethostname(_Buf, _SZ);

	if (_Buf[_SZ-1] != 0) 
	{// name too long, 
        // return it truncated
		_Buf[_SZ-1]=0;
	}

	return _Buf;
}
