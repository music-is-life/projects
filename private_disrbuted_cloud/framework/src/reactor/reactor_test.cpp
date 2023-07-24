#include "reactor.hpp"
#include "select.hpp"

class Hey
{
public:
    Hey();
}; 

void Print()
{
    std::cout << "Print!" << std::endl;
}

int foo(int num)
{
    char c;
    (void)num;
    c = getc(stdin);

    std::cout << c << std::endl;

    return (5);
}

using namespace ilrd;

int main(void)
{
    Reactor<Select> r;
    Function<int(void)> f1(&foo, 3);

    r.RegisterReadHandler(0, f1);

    r.Run();

    /* Reactor<Hey> r2; */
    return 0;
}