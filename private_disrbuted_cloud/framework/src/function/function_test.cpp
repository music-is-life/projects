#include <iostream>
#include "function.hpp"

using namespace ilrd;

char foo(int num)
{
    std::cout << num << std::endl;

    return ('a');
}

class Shifon
{
public:
    explicit Shifon(int bread_num): m_bread_num(bread_num) {}
    int Print(char a)
    {
        std::cout << a << std::endl << "printing Shifon" << std::endl;
        return 1;
    }

    void PrintB(char a)
    {
        std::cout << a << std::endl << "printing Shifon" << std::endl;
    }

private:
    int m_bread_num;
};


int main(void)
{
    Function<char(void)> f1(&foo, 5);
    f1();

    Shifon a(3);
    Function<int(void)> f2(&Shifon::Print, &a, 'G');
    f2();

    Function<char(int)> f3(&foo);

    std::cout << "\n\n--------------Checking second specialization-------------\n\n";

    f3(5);
    f3(8);

    Function<void(char)> f4(&Shifon::PrintB, &a);

    f4('B');
    f4('Y');
    f4('E');


    

}
