/*********************************
 * Reviewer:  
 * Author: Irit Ofir
 * File: functionTest.cpp
 * ******************************/

#include <stdio.h>
#include <iostream>
#include "function.hpp" 

using namespace ilrd;

class FOO
{
public:

	explicit FOO(int num=33): m_num(num) {}

	int foo(int a)
	{
		return a + m_num;
	}
private:
	int m_num;
};

float Bar(int a)
{
	return static_cast<float>(a) / 17;
}

int main()
{
	int num = 77;
	FOO foo1(num);

	int a = 3;
	Function<int(void)> f1(&FOO::foo, &foo1, a);

	if (f1() != (num + a))
	{
		std::cout << "MemberFunction failed on line " << __LINE__ << std::endl;
	}

	int b = 7;
	Function<float(void)> f2(Bar, b);

	if (f2() != (static_cast<float>(b) / 17))
	{
		std::cout << "FreeFunction failed on line " << __LINE__ << std::endl;
	}


	return 0;
}
