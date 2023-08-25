#include <iostream>

class Base
{
public:
    virtual void method() {std::cout << "Base" << std::endl;}
    virtual int ReturnFourOrSeven(int num) = 0;
    virtual ~Base() {method();}
};

class Derived : public Base
{
private:
    void method() override {std::cout << "Derived!!" << std::endl;}
    ~Derived() {method();}
    int ReturnFourOrSeven(int num) override {return (num ^ 4) ^ 7;}
};

int main()
{
    Base *object = new Derived;
    object->method();

    std::cout << object->ReturnFourOrSeven(7) << std::endl;

    delete object;

    return 0;
}

