// patch GCC 4.4.7 error parameter pack expansion fixed-length argument list
// g++ -std=c++0x t3.cpp -o t3

#define NOBOOSTLIBS
#include "shared.hpp"

struct response
{};


template <class... _Other>
struct nested_map
{
    typedef nested_map <_Other...>
        type;
};

template <class _RespTy, class _Left, class _Right>
struct nested_map <_RespTy, _Left, _Right>
{
    typedef std::map<_Left, 
        std::map<_Right, _RespTy>>
        type;
};

template <class _RespTy, class _Left, class... _Other>
struct nested_map <_RespTy, _Left, _Other...> 
    : nested_map <_RespTy, _Other...>
{
    typedef nested_map<_RespTy, _Left, _Other...>
        upper;

    typedef std::map<_Left, 
        typename upper::type> 
        type;
};


int main()
{
    typedef nested_map <response, int,int,int>
        nested_map_type;

    flush_msg(std::cout,
        typeid(nested_map_type::type).name());

/*
St3mapIiS_IiS_IidSt4lessIiESaISt4pairIKidEEES1_SaIS2_IS3_S6_EEES1_SaIS2_IS3_S9_EEE
std::map<int, 
    std::map<int, 
        std::map<int, 
            double, 
                std::less<int>, std::allocator<std::pair<int const, double> > >, 
                std::less<int>, std::allocator<std::pair<int const, 
                    std::map<int, double, std::less<int>, std::allocator<std::pair<int const, double> > > > > >, 
                std::less<int>, std::allocator<std::pair<int const, 
                    std::map<int, std::map<int, double, std::less<int>, std::allocator<std::pair<int const, double> > >, 
                std::less<int>, std::allocator<std::pair<int const, 
                    std::map<int, double, std::less<int>, std::allocator<std::pair<int const, double> > > > > > 
        > 
    > 
>
St3mapIiS_IiS_Ii8responseSt4lessIiESaISt4pairIKiS0_EEES2_SaIS3_IS4_S7_EEES2_SaIS3_IS4_SA_EEE
std::map<int, 
    std::map<int, 
        std::map<int, 
            response, 
                std::less<int>, std::allocator<std::pair<int const, response> > >, 
                std::less<int>, std::allocator<std::pair<int const, 
                    std::map<int, response, std::less<int>, std::allocator<std::pair<int const, response> > > > > >, 
                std::less<int>, std::allocator<std::pair<int const, 
                    std::map<int, std::map<int, response, std::less<int>, std::allocator<std::pair<int const, response> > >, 
                std::less<int>, std::allocator<std::pair<int const, 
                    std::map<int, response, std::less<int>, std::allocator<std::pair<int const, response> > > > > > 
        > 
    > 
>

*/
    return 0;
}