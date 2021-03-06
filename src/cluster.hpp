#pragma once
#include "def.hpp"
#include "shared.hpp"

// libSSH memo.
// the libSSH function ssh_channel_read()
// implemented in channels.c can return error
// SSH_AGAIN (typically associated to asynchronous
// socket calls), even if the channel used for
// the communication is synchronous. See also
// how ssh_poll_ctx_dopoll(), implemented in poll.c,
// handles the timeout while polling the remote 
// SSH server.
// See how it is handled below in the member
// function channel::request_exec.

// ----------------------------------------------
// compile this code only if the one
// of the two LIBSSHxxx are defined
#if ((defined LIBSSH481) || (defined LIBSSH423))
#define HAS_LIBSSH
#include <libssh/libssh.h>
#endif

#ifdef HAS_LIBSSH
namespace confly
{
	inline auto hash_type(
		const size_t& _Sz)
		->std::string
	{// return type of hash depending
		// on the size in bytes
		switch(_Sz)
		{
			case MD5_HASH_SIZE:
				return S_MD5_HASH_SIZE;

			case SHA1_HASH_SIZE:
				return S_SHA1_HASH_SIZE;

			case SHA224_HASH_SIZE:
				return S_SHA224_HASH_SIZE;

			case SHA256_HASH_SIZE:
				return S_SHA256_HASH_SIZE;

			case SHA384_HASH_SIZE:
				return S_SHA384_HASH_SIZE;

			case SHA512_HASH_SIZE:
				return S_SHA512_HASH_SIZE;

			default: break;
		}

		return "";
	}

	inline void print_hash(
		std::ostream& _S,
		unsigned char* _Hash,
		const size_t& _HashLen)
	{
		const std::string _HashTy(
			hash_type(_HashLen));

		if (_HashTy.empty())
		{
			flush_msg(_S,
				M_ERROR,
				M_HASH, M_TYPE);
		}

		else if (0==_Hash)
		{
			flush_msg(_S,
				M_ERROR,
				M_HASH);
		}

		else
		{
			_S 	
			<< _HashTy << CHARCC << CHARSP
			<< std::hex 
			<< std::setfill('0');

			for (size_t i=0; i<_HashLen;
				++i) _S << std::setw(2) 
					<< (int)(unsigned char)
						(_Hash[i]);
		}
	}

	#ifdef LIBSSH481
	/// @brief manage the public key
	/// of an SSH connection as implemented
	/// in LibSSH ver. 4.8.1
	struct public_key
		: enable_internal_error
	{
		/// @brief construct with session
		/// parameter
		public_key(
			ssh_session& _cSession)
			: _Pubkey(0)
			, _Hash(0)
			, _HashLen(0)
		{
			if(ssh_get_server_publickey(
				_cSession, &_Pubkey)<0)
				fail(M_SSHPUBKEY);
			
			else if(ssh_get_publickey_hash(
					_Pubkey,
					SSH_PUBLICKEY_HASH_SHA1,
					&_Hash,
					&_HashLen)<0)
				fail(M_SSHPUBKEY);
		}

		/// @brief destroy
		~ public_key()
		{
			if (_Hash)
			{
				::ssh_clean_pubkey_hash(
					&_Hash);

				_Hash=0; _HashLen=0;
			}

			if (_Pubkey)
			{
				::ssh_key_free(_Pubkey);

				_Pubkey=0;
			}
		}

		/// @brief write to stream
		void write(std::ostream& _S)
		{
			print_hash(_S, 
				_Hash, _HashLen);
		}

		private:

		ssh_key
			_Pubkey;
		unsigned char*
			_Hash;
		size_t
			_HashLen;
	};
	#elif (defined LIBSSH423)
	/// @brief manage the public key
	/// of an SSH connection as implemented
	/// in LibSSH ver. 4.2.3
	struct public_key
		: enable_internal_error
	{
		/// @brief construct with session
		/// parameter
		public_key(
			ssh_session& _cSession)
			: _Hash(0)
			, _HashLen(0)
		{
			_HashLen=
			::ssh_get_pubkey_hash(
				_cSession, &_Hash);

			if (_HashLen < 0)
				fail(M_SSHPUBKEY);
		}

		/// @brief destroy
		~ public_key()
		{
			if (_Hash)
			{
				::free(_Hash);

				_HashLen=0;
			}
		}

		/// @brief write to stream
		void write(std::ostream& _S)
		{
			print_hash(_S, 
				_Hash, _HashLen);
		}

		private:

		unsigned char*
			_Hash;
		size_t
			_HashLen;
	};
	#endif

	/// @brief channel wraps the ssh_channel
	/// using RAII paradigm to acquire and release
	/// resources. On construction, it creates and 
	/// opens a channel. On destruction, is closes
	/// and destroys the channel.
	/// See confly::session on how to create
	/// a managed ssh_session. 
	struct channel
		: enable_internal_error
	{
		/// @brief construct with session reference,
		/// creating and opening the SSH channel
		/// automatically. 
		/// @param _cSession reference to ssh_session. 
		/// See confly::session on how to create
		/// a managed ssh_session. 
		channel(
			ssh_session& _cSession)
			: _Session(_cSession)
			, _Channel(0)
			, _StartLogging(false)
		{
			create();
		}

		/// @brief close and destroy the SSH channel,
		/// destroy the object.
		~ channel() 
		{
			destroy();
		}

		/// @brief send the request for remote command,
		/// start polling synchronously the server 
		/// for a reply and, upon receiving bytes, flush 
		/// them to the provided output stream.
		/// @param _Host the remote host to which the
		/// command is sent. Confly clients, started e.g.
		/// using the '--aggregate' command, will flush 
		/// the machine's hostname to the standard output.
		/// This way, the hostname can be doublechecked with
		/// the one provided here.
		/// @param _S the output stream on which to flush
		/// the output of the execution of the remote command.
		/// The first bytes received, before the hostname
		/// is received, are typically the same output shown
		/// when connecting to a server using PuTTy. Thus
		/// they are not useful to Confly and they will not
		/// be written.
		/// @param _Cmd the command to be executed remotely.
		bool request_exec(
			const std::string& _Host,
			std::ostream& _S,
			const std::string& _Cmd)
		{
			if (! good())
				return false;

			if (SSH_OK!=
			::ssh_channel_request_exec(
				_Channel, _Cmd.c_str()))
			{
				fail(M_SSHCHANNEL);

				return false;
			}

			int _X(0); while (true)
			{
				debug_msg<3>(
					E_SSH_READATTEMPT, 
					_X++);

				switch (read(_Host, _S))
				{
					case 0: 
						fail(M_SSHCHANNEL);
						return false;

					case -1: // SSH_AGAIN
						break;

					default:
					case 1: 
						return true;
				}
			}

			return true;
		}

	private:

		void create() 
		{
			_Channel=
				::ssh_channel_new(
				_Session);

			if (! _Channel)
			{
				fail(M_SSHCHANNEL);

				return;
			}

			if (SSH_OK!=
			::ssh_channel_open_session(
				_Channel))
			{
				destroy();
			}
		}

		void destroy()
		{
			if (_Channel)
			{
				fail(M_SSHCHANNEL);

				if (! ::ssh_channel_is_closed(
					_Channel)) 
					::ssh_channel_close(
					_Channel);

				::ssh_channel_free(
					_Channel);

				_Channel=0;
			}
		}

		int read(
			const std::string& _Host,
			std::ostream& _S)
		{
			// returns: 
			//  1: read ok
			//  0: read failed
			// -1: poll failed,read again

			std::string _Buf(
				DEFSSHBUFSZ, '\0');

			int _RxBytes=
				::ssh_channel_read(
					_Channel, &_Buf[0],
					DEFSSHBUFSZ,
					0 // is_stderr?
					);

			debug_msg<3>(
				M_BYTES, _RxBytes);

			while (_RxBytes > 0)
			{
				size_t _Start(0); if (! 
					_StartLogging)
				{// wait for hostname to
					// be received from
					// the host
					const size_t _F(
						_Buf.find(_Host));

					if (_F!=std::string::npos)
					{
						_StartLogging=true;

						_Start=_F+
							_Host.length()+
								1; // '\n'
					}
				}

				if (_StartLogging)
				{
					exclusive_flush_msg(
						_S, _Buf.substr(
							_Start));
				}

                _Buf.assign(DEFSSHBUFSZ,
					'\0');
                
				_RxBytes=
					::ssh_channel_read(
						_Channel, &_Buf[0], 
						DEFSSHBUFSZ, 0);
			}
 
			if (_RxBytes < 0)
			{
				debug_msg<3>(
					M_BYTES, _RxBytes);

				if (_RxBytes==-2)
					// SSH_AGAIN.
					// must repeat the
					// read call
					return -1;

				// else definitively
				// a read error; 
				// fail here.
				return 0;
			}
			
			return 1;
		}

		ssh_session&
			_Session; 
		ssh_channel
			_Channel;
		bool 
			_StartLogging;
	};
}

#ifdef LIBSSH481
/// @brief read and check the public key of 
/// the remote server against the list of 
/// known hosts.
/// @param _Session the raw ssh_session, as
/// created by confly::session.
/// @note this function depends on LibSSH v.4.8.1,
/// which in turn depends on OpenSSL >=1.1.0.
/// @note this function is for internal use only.
/// @note firefighter approach: either the
/// remote host is among the list of known servers
/// or the function will fail. 
/// @warning the user must have performed SSH
/// access to the remote host at least once
/// before attempting to distribute jobs 
/// using Confly.
inline int verify_knownhost(
	ssh_session _Session)
{
	confly::public_key 
		_PKey(_Session);

	if (! _PKey.good())
	{
        flush_msg(std::cout,
    	    M_ERROR,
	        _PKey.last_error());
        return -1;
	}

    std::ostringstream _OSS;

	switch (ssh_known_hosts_e 
		_State=
		::ssh_session_is_known_server(
			_Session))
	{
		case SSH_KNOWN_HOSTS_OK:
            _PKey.write(_OSS);            
			debug_msg<3>(
		        M_SSHSESSION, M_SERVER,
				M_VERIFIED);
			debug_msg<3>(
		        M_SERVER, M_HASH,
				M_PUBKEY, _OSS.str());
			break;

		case SSH_KNOWN_HOSTS_CHANGED:
			flush_msg(std::cout,
		        M_ERROR, 
				E_SSH_KNOWN_HOSTS_CHANGED);
			_PKey.write(std::cout);
			flush_msg(std::cout, 
				M_CLOSING,
				M_CONNECTION);
			return -1;

		case SSH_KNOWN_HOSTS_OTHER:
			flush_msg(std::cout,
		        M_ERROR, 
				E_SSH_KNOWN_HOSTS_OTHER); 
			return -1;

		case SSH_KNOWN_HOSTS_NOT_FOUND:
			flush_msg(std::cout,
		        M_ERROR, 
				E_SSH_KNOWN_HOSTS_NOT_FOUND); 
			return -1;
 
		case SSH_KNOWN_HOSTS_UNKNOWN:
			flush_msg(std::cout,
		        M_ERROR, 
				E_SSH_KNOWN_HOSTS_UNKNOWN); 
			return -1;

		case SSH_KNOWN_HOSTS_ERROR:
			flush_msg(std::cout,
		        M_ERROR, M_HOST,
				ssh_get_error(_Session));
			return -1;
	}
 
	return 0;
}
#endif

#ifdef LIBSSH423
/// @brief read and check the public key of 
/// the remote server against the list of 
/// known hosts.
/// @param _Session the raw ssh_session, as
/// created by confly::session.
/// @note this function depends on LibSSH v.4.2.3,
/// which in turn depends on OpenSSL >=1.0.1.
/// @note this function is for internal use only.
/// @note firefighter approach: either the
/// remote host is among the list of known servers
/// or the function will fail. 
/// @warning the user must have performed SSH
/// access to the remote host at least once
/// before attempting to distribute jobs 
/// using Confly.
inline int verify_knownhost(
	ssh_session _Session)
{
	confly::public_key 
		_PKey(_Session);

	if (! _PKey.good())
	{
        flush_msg(std::cout,
    	    M_ERROR,
	        _PKey.last_error());
        return -1;
	}

	std::ostringstream _OSS;

	switch (int state=
  		::ssh_is_server_known(
			_Session))
	{
		case SSH_SERVER_KNOWN_OK:
            _PKey.write(_OSS);            
			debug_msg<3>(
		        M_SSHSESSION, M_SERVER,
				M_VERIFIED);
			debug_msg<3>(
		        M_SERVER, M_HASH,
				M_PUBKEY, _OSS.str());
			break;

    	case SSH_SERVER_KNOWN_CHANGED:
			flush_msg(std::cout,
		        M_ERROR, 
				E_SSH_KNOWN_HOSTS_CHANGED);
			_PKey.write(std::cout);
			flush_msg(std::cout, 
				M_CLOSING,
				M_CONNECTION);
			return -1;

    	case SSH_SERVER_FOUND_OTHER:
			flush_msg(std::cout,
		        M_ERROR, 
				E_SSH_KNOWN_HOSTS_OTHER); 
			return -1;

    	case SSH_SERVER_FILE_NOT_FOUND:
			flush_msg(std::cout,
		        M_ERROR, 
				E_SSH_KNOWN_HOSTS_NOT_FOUND); 
			return -1;

    	case SSH_SERVER_NOT_KNOWN:
			flush_msg(std::cout,
		        M_ERROR, 
				E_SSH_KNOWN_HOSTS_UNKNOWN); 
			return -1;

    	case SSH_SERVER_ERROR:
			flush_msg(std::cout,
		        M_ERROR, M_HOST,
				ssh_get_error(_Session));
			return -1;
	}

	return 0;
}
#endif 

namespace confly
{
	/// @brief session class is a wrapper to ssh_session
	/// that allows basic options configuration such as
	/// user, host and port. It provides RAII-paradigm
	/// acquisition and release of resources, connection
	/// and disconnection, and remote execution.
	struct session 
		: enable_internal_error
	{
		/// @brief construct with no parameters.
		/// The SSH session is not created automatically
		/// until init() is called, providing the needed
		/// parameters (user, host, port).
		session()
			: _Session(0)
			, _User()
			, _Host()
			, _Port()
		{}

		/// @brief construct with user, host and 
		/// port parameters and create an SSH session
		/// automatically. 
		/// @param _cUser the user name used to SSH.
		/// @param _cHost the target host to SSH.
		/// @param _cPort the host port to be used. Defaults
		/// to port 22.
		session(
			const std::string& _cUser,
			const std::string& _cHost,
			const int& _cPort=DEFSSHPORT)
			: _Session(0)
			, _User(_cUser)
			, _Host(_cHost)
			, _Port(_cPort)
		{
			if (! create())
			{
				fail(M_SSHSESSION);
			}

			set_user();
			set_host();
			set_port();
		}

		/// @brief destroy SSH session first, disconnecting
		/// as required. Then destroy the session object.
		~ session()
		{
			destroy();
		}

		/// @brief lazy initialization of SSH session 
		/// parameters user, host and port and create 
		/// an SSH session. After creation, it will
		/// not be possible to change the user before
		/// the authentication.
		/// @param _cUser the user name used to SSH.
		/// @param _cHost the target host to SSH.
		/// @param _cPort the host port to be used. Defaults
		/// to port 22.
		void init(
			const std::string& _cUser,
			const std::string& _cHost,
			const int& _cPort=DEFSSHPORT)
		{
			_User=_cUser;
			_Host=_cHost;
			_Port=_cPort;

			if (! create())
			{
				fail(M_SSHSESSION);
			}

			set_user();
			set_host();
			set_port();
		}

		/// @brief test if the SSH session is
		/// blocking.
        bool blocking() const
        {
            return 1==::ssh_is_blocking(
                _Session);
        }

		/// @brief connect the SSH session, verify
		/// that the remote host is known, and 
		/// authenticate the user with password.
		/// @param _Passw the user's password.
		/// @return true on success, false otherwise.
		/// The last error is stored internally and
		/// can be queried by the caller.
		bool connect(
			const secure_string& _Passw)
		{
			if (SSH_OK!=::ssh_connect(
				_Session))
			{
				fail(::ssh_get_error(
					_Session));

				return false;
			}

			if (verify_knownhost(
				_Session) <0)
			{
				disconnect();

				fail(M_SSHVERIFY);

				return false;
			}

			// authenticate ourselves
			// without changing username
			if (SSH_AUTH_SUCCESS!= 
				::ssh_userauth_password(
					_Session, NULL, 
						_Passw.c_str()))
			{
				disconnect();

				fail(::ssh_get_error(
					_Session));

				return false;
			}

			return true;		
		}

		/// @brief disconnect a session. Checks if
		/// the session is connected first.
		void disconnect()
		{
			debug_msg<3>(
				M_SSHDISCONNECT);

			if (! _Session)
				return;

			if (::ssh_is_connected(
					_Session))
				::ssh_disconnect(
					_Session);
		}

		/// @brief creates a SSH channel and request
		/// the execution of a command.
		/// @param _S output stream.
		/// @param _Cmd the command to be executed.
		/// @return true if the command was successfully
		/// run, false otherwise. 
		bool ssh_exec(
			std::ostream& _S,
			const std::string& _Cmd)
		{// run cmd on a fresh new 
			// channel
			channel _Channel(
				_Session); if (! 
					_Channel.good())
			{
				fail(M_SSHSESSION);

				flush_msg(std::cout,
					M_ERROR, _Channel
						.last_error(),
					::ssh_get_error(
					_Session));

				return false;
			}

			// next blocks until SSH cmd 
			// returns, flushing the stream
			// on time to time with partial
			// output produced remotely.
			if (! _Channel
				.request_exec(
					_Host,
					_S, _Cmd))
			{
				fail(M_SSHSESSION);

				flush_msg(std::cout,
					M_ERROR, _Channel
						.last_error(),
					::ssh_get_error(
					_Session));

				return false;
			}

			return true;
		}

	private:

		bool create()
		{// create session
			_Session=::ssh_new();

			return (_Session
				!=NULL);
		}

		void destroy()
		{// disconnect and free session
			if (_Session)
			{
				disconnect();

				::ssh_free(_Session);

				_Session=0;
			}
		}

		void set_host()
		{
			::ssh_options_set(
				_Session, 
				SSH_OPTIONS_HOST, 
				_Host.c_str());
		}

		void set_port()
		{
			::ssh_options_set(
				_Session, 
				SSH_OPTIONS_PORT, 
				&_Port);
		}

		void set_user()
		{
			if (! _User.empty())
			::ssh_options_set(
				_Session, 
				SSH_OPTIONS_USER, 
				_User.c_str());
		}

		ssh_session
			_Session;
		std::string
			_User;
		std::string
			_Host;
		int 
			_Port;
	};

	/// @brief manage SSH connection and
	/// disconnection of a SSH session 
	/// as RAII.
	struct connection
	{// RAII connect-disconnect

		/// @brief construct with session
		/// and password parameters, connect
		/// automatically.
		connection(
			session& _cSess,
			const secure_string& _Passw)
			: _Session(_cSess)
		{
			_Session
				.connect(_Passw);
		}

		/// @brief disconnect and destroy.
		~ connection()
		{
			_Session
				.disconnect();
		}

		/// @brief access the session 
		/// object
		auto operator ->()
			->session*
		{// forward session 
			// interface
			return &_Session;
		}

		/// @brief relay the session
		/// internal error status 
		bool good() const 
		{
			return _Session
				.good();
		}

		/// @brief relay the session
		/// internal error string
		auto last_error() const
			->std::string
		{
			return _Session
				.last_error();
		}

	private:

		session&
			_Session;
	}; 

	inline bool ssh(
		const std::string& 
			_User,
		const secure_string& 
			_Passw,
		const std::string& 
			_Host,
		const std::vector<std::string>&
			_Commands,
		std::ostream& 
			_S)
	{// single ssh session
		// running _Commands sequentially

		session _Session(
			_User, _Host);

		if (! _Session.good())
		{
			flush_msg(std::cout,
				M_ERROR, M_SSHSESSION,
				_Session.last_error());

			return false;
		}

		connection _Cnet(
			_Session, _Passw);

		if (! _Cnet.good())
		{
			flush_msg(std::cout,
				M_ERROR, M_SSHSESSION,
				_Cnet.last_error());

			return false;
		}

		for (size_t i=0; i<_Commands
			.size(); ++i)
		{
			_Cnet->ssh_exec(
				_S,
				_Commands[i]);
		}

		return true;
	}

	// ------------------------------------------

	/// @brief manage multiple SSH connections to
	/// a cluster of servers, as specified by the
	/// list of hostnames in the configuration file.
	/// Offers methods for creating and connecting,
	/// and to run parallel and sequential SSH commands.
	/// Connections are then closed and destroyed  
	/// automatically at cluster object destruction. 
	struct cluster
		: enable_internal_error
	{
		/// @brief type of smart pointer
		/// to session object
		typedef boost
			::shared_ptr<session>
			session_ptr;

		/// @brief construct with the list of hostnames
		/// to which the connection has to be made.
		/// @param _cHostnames vector of strings 
		/// containing hostnames or IP addresses 
		/// of the cluster machines.
		cluster(
			const std::vector<std::string>&
				_cHostnames)
			: _Hostnames(_cHostnames)
		{}
		
		/// @brief destrory sessions and destroy
		~ cluster()
		{}

		/// @brief create SSH sessions, one for each
		/// machine.
		/// @param _BVerb flag to set the print of
		/// diagnostic output in case of failure
		bool create(bool _BVerb)
		{
			// number of machines
			const size_t _Sz(
				_Hostnames.size());

			// get current user
			std::string _User(
				::cuserid(0));

			// allocate sessions
			_Sessions.resize(_Sz);

			for (size_t i=0; i<_Sz; 
					++i)
			{
				_Sessions[i]=session_ptr(
					new session( 
					_User,
					_Hostnames[i]));

				if (! _Sessions[i]->good())
				{
					if (_BVerb)
						flush_msg(std::cout,
							M_ERROR,
							_Hostnames[i]);

					fail(_Sessions[i]
						->last_error());
					
					return false;
				}

				debug_msg<3>(
					_Sessions[i]
						->blocking());
			}

			return true;
		}

		/// @brief prompt the user for password,
		/// connect to remote hosts.
		/// @param _BVerb flag to set the print of
		/// diagnostic output in case of failure, 
		/// and additional verbosity.
		bool connect(bool _BVerb)
		{
			if (_BVerb)
			{
				// get current user
				std::string _User
					=::cuserid(0);

				flush_msg(std::cout,
					M_ATTEMPTING,
					M_SSH, M_AS,
					_User);
			}

			// prompt user for SSH
			// password
			secure_string _Passw
				=::getpass(
					this->prompt()
						.c_str());

			// connect to hosts...
			for (size_t i=0; i<
				_Sessions.size(); ++i)
			{
				if (! _Sessions[i]
					->connect(_Passw))
				{// ith connection failed
					if (_BVerb)
						flush_msg(std::cout,
							M_ERROR,
							M_SSHCONNECT,
							_Hostnames[i]);

					fail(_Hostnames[i]);
					// suspend here
					return false;
				}

				else
				{
					if (_BVerb)
						flush_msg(std::cout,
							_Hostnames[i],
							M_CONNECTED);
				}
			}

			return true;
		}

		/// @brief run a remote command on each 
		/// connected machine. Parallelism is 
		/// achieved using OpenMP. Check the results
		/// at the end.
		/// @param _S the output stream to write the
		/// output of each command.
		/// @param _Cmd the command to run remotely 
		bool parallel_exec(
			std::ostream& _S,
			const std::string& _Cmd)
		{
			#pragma omp parallel for
			for (size_t i=0; i<
				_Sessions.size(); ++i)
			{        
				_Sessions[i]->ssh_exec(
					_S,_Cmd);
			}
			#pragma omp barrier

			this->check();

			return this->good();
		}

		/// @brief similar to cluster::parallel_exec
		/// except that jobs are run sequentially (no
		/// parallelism here).
		/// @param _S the output stream to write the
		/// output of each command.
		/// @param _Cmd the command to run remotely 
		bool sequential_exec(
			std::ostream& _S,
			const std::string& _Cmd)
		{
			for (size_t i=0; i<
				_Sessions.size(); ++i)
			{        
				_Sessions[i]->ssh_exec(
					_S,_Cmd);
			}

			this->check();

			return this->good();
		}

	private:

		auto prompt()
			const 
			->std::string
		{
			// string used to prompt
			// the user for a password...
			std::ostringstream 
				_PSW; _PSW 
				<< M_ENTER << CHARSP
				<< M_SSH << CHARSP 
				<< M_PASSWORD << CHARCC;

			return _PSW.str();
		}

		void check()
		{
			// check status of the sessions			
			for (size_t i=0; i<
				_Sessions.size(); ++i)
			{
				if (! _Sessions[i]->good())
				{
					fail(_Hostnames[i]); 
					
					break;
				}
			}
		}

		std::vector<std::string>
			_Hostnames;
		std::vector<session_ptr>
			_Sessions;
	};

} // ns confly


#endif // HAS_LIBSSH

