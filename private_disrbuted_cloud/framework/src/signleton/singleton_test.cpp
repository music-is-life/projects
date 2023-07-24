#include <iostream>
#include <cstdio>

#include "singleton.hpp"
#include "ThreadPool.hpp"

using namespace ilrd;

static void Print()
{
    Singleton<std::string> tp3;

    std::cout << "Function printing\n";
    std::cout << tp3->data() << std::endl;

    return;
}

int main(void)
{
    Singleton<std::string> tp1;
    Singleton<std::string> tp2;

    tp1->assign("Hello Darkness my old friend.");
    
    std::cout << tp2->data() << std::endl;

    Print();


    return 0;
}

