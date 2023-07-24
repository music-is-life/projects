#include "observer_test.hpp"

void Client::OnUpdate(std::string weather)
{
    std::cout << weather << "\n";
}

Client::Client(): m_callback(Function<void(std::string)>(&OnUpdate)) {}


void Shop::Subscribe(const Callback<std::string> &cb)
{
    m_dispatcher.Add(cb);
}

void Shop::Unsubscribe(const Callback<std::string> &cb)
{
    m_dispatcher.Remove(cb);
}

void Shop::NotifyAll(std::string event)
{
    m_dispatcher.Notify(event);
}

int main(void)
{
    Shop shop;
    Client client_one;
    Client client_two;
    Client client_three;

    shop.Subscribe(client_one.m_callback);
    shop.Subscribe(client_two.m_callback);
    shop.Subscribe(client_three.m_callback);

    shop.NotifyAll(std::string("Sunny"));
    shop.NotifyAll(std::string("Windy, massive clouds"));

    shop.Unsubscribe(client_one.m_callback);

    shop.NotifyAll(std::string("Inferno, we'll die today."));


    return 0;
}