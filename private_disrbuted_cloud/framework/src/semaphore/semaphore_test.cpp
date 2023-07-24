/*********************************
 * Reviewer:  
 * Author: Roy Mantel
 * File Name: semaphoreTest.cpp
 * ******************************/

#include <unistd.h>
#include <iostream>
#include <thread>
#include "semaphore.hpp"

using namespace ilrd;

Semaphore g_sem(1);

void ThreadRoutine()
{
	while(1)
	{
		sleep(3);
		g_sem.Post();
	}
}

int main()
{
	std::thread t(&ThreadRoutine);

	while(1)
	{
		g_sem.Wait();
		std::cout << "Waiting" << std::endl;
	}
	
	return 0;
}
