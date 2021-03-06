// unit test SSH
// g++ -std=c++0x -I/home/eshome/stocmar/libssh-0.5.3/include -L/home/eshome/stocmar/lib t2.cpp -lssl -lcrypto -lssh -o t2
#define NOBOOSTLIBS
#define LIBSSH423
#include "cluster.hpp"

int main()
{
    using namespace confly;

    std::ostringstream _OSS;

    const std::string 
        _Passw=::getpass("Password: ");

    std::vector<std::string>
        _Commands(
            {"pwd",
            "ls -l"}
        );

    if (! ssh(
        "stocmar",
        _Passw,
        "s-estat-bgdnode-01.eurostat.cec",
        _Commands,
        _OSS))
    {
        flush_msg(std::cout,
            M_ERROR, 
            M_SSHSESSION
            );

        return 0;
    }

    std::cout << _OSS.str() << "\n";

    return 0;
}

// {
//     using namespace confly;

//     session _Session(
//         "stocmar",
//         "localhost");

//     if (! _Session.good())
//     {
//         flush_msg(std::cout,
//         M_ERROR, M_SSHSESSION,
//         _Session.last_error());
//         return 0;
//     }

//     const std::string 
//         _Passw=::getpass("Password: ");

//     connection _Cnet(
//         _Session, _Passw);

//     if (! _Cnet.good())
//     {
//         flush_msg(std::cout,
//         M_ERROR, M_SSHSESSION,
//         _Cnet.last_error());
//         return 0;
//     }

//     std::ostringstream _OSS;

//     _Cnet->ssh_exec("pwd", _OSS);
//     _Cnet->ssh_exec("ls -l", _OSS);

//     flush_msg(std::cout,
//         _OSS.str());
// }