#include "observer.hpp"

using namespace ilrd;

class Shop
{
public:
    void Subscribe(const Callback<std::string> &cb);
    void Unsubscribe(const Callback<std::string> &cb);
    void NotifyAll(std::string event);
private:
    Dispatcher<std::string> m_dispatcher;
};

class Client
{
public:
    Client();
    static void OnUpdate(std::string weather);

    Callback<std::string> m_callback;
private:
};
