// unit test MT for GCC 4.4.7 and GCC 9.8.1
// g++ -std=c++0x t1.cpp -o t1


#include <algorithm>
#include <limits>
#include <iostream>
#include <random>
#include <iomanip>
#include <set>
#include <vector>
#include <type_traits>
#include <typeinfo>


#include "random_draw.hpp"

namespace confly
{

	mersenne_twister
		_DRE; // 32bit

}

using namespace confly;

template <class T>
auto test_unif(T m, T M)
	->T
{
	unif<T> _Unif;

	return _Unif.draw(m,M);

	// std::cout 
	// << std::setprecision(32) << std::fixed
	// << _Unif.draw(
	//     m,M) << "\n";
}

template <class T>
void test_unif(size_t N, T m, T M)
{
	std::cout << "UNIF " 
	<< typeid(T).name() << " range "
	<< m << " : " << M << "\n";
	unif<T> _Unif;

	for (int i=0; i<N; ++i)
	std::cout 
	<< std::setprecision(32) << std::fixed
	<< _Unif.draw(
		m,M) << "\n";

	std::cout << "----------------------\n\n";
}

void test_laplace(size_t N, double mu, double b)
{
	std::cout << "LAPLACE\n";
	laplace _Lap;

	for (int i=0; i<N; ++i)
	std::cout 
	<< std::setprecision(32) << std::fixed
	<< _Lap.draw(
		mu,b) << "\n";

	std::cout << "----------------------\n\n";
}

void test_geometric(size_t N, double _Eps)
{
	std::cout << "GEOMETRIC\n";
	geometric _Geo;

	for (int i=0; i<N; ++i)
	std::cout 
	<< std::fixed
	<< _Geo.draw(0,
		_Eps) << "\n";

	std::cout << "----------------------\n\n";
}

int main()
{
	// for (size_t i=0; i<30; ++i)
	// {
	// 	long x = test_unif<long>(
	// 		0, _DRE.max()); 

	// 	std::cout 
	// 	<< x << "\t"
	// 	<< (double)x/(double)_DRE.max()
	// 	<< "\n";
	// }

	test_unif<int>(50, -100, +100);    
	test_unif<double>(30, -1.0, 1.0);
	test_unif<double>(30, 0.0, 1.0);
   
	test_laplace(30, 0, 2);
	test_geometric(30, 0.5);

	return 0;
}

