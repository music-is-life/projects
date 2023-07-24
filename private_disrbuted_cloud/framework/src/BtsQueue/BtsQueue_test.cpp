#include <iostream>
#include <unistd.h>
#include "BtsQueue.hpp"
#include "function.hpp"

void SimpleTest();
void *Producer(void *num);
void *Consumer(void *num);
void AdvanceTest();
void AnotherTest();

int ExampleFunc(int num);

using namespace ilrd;

class Compirason 
{
public:
    bool operator() (const Function<int(void)> &, const Function<int(void)> &)
    {
        return true;
    }

private:
    
};

int arr[1000] = {0};
BtsQueue<int> advance_q;

/* global variables (defines) */
const static int g_thread_amount = 1000;

int main(void)
{
/*     SimpleTest();
 */    /* AdvanceTest(); */
    AnotherTest();
    return 0;
}


void SimpleTest()
{
    int t1 = 1;
    int t3 = 5;
    int t2; 

    BtsQueue<int> queue;

    queue.Enqueue(t1);
    queue.Enqueue(t3);

    queue.Dequeue(t2);

    std::cout << t2 << std::endl;
}

void AdvanceTest()
{
    pthread_t thread_id[g_thread_amount];
    size_t i = 0, z = 0;

    for(i = 0; i < 1000; ++i)
    {
        arr[i] = i;
    }

    for(i = 0; i < 500; ++i)
    {
        pthread_create(&thread_id[i], NULL, Producer, static_cast<void*>(&(arr[i])));
    }

    for(z = 500; z < 1000; ++z)
    {
        pthread_create(&thread_id[z], NULL, Consumer, static_cast<void*>(&(z)));
    }

    for(i = 0; i < g_thread_amount; ++i)
    {
        pthread_join(thread_id[i], NULL);
    }

    
    return;

}

void AnotherTest()
{
    BtsQueue<Function<int(void)>, Compirason> func_q;
    Function<int(void)> f1(ExampleFunc, 5);
    Function<int(void)> f2;

    func_q.Enqueue(f1);
    func_q.Dequeue(f2);

    f1();
    f2();

}


void *Producer(void *num)
{
    sleep(1);
    advance_q.Enqueue(*static_cast<int*>(num));

    return NULL;
}

void *Consumer(void *num)
{
    int tmp = 0;
    (void)(num);
    advance_q.Dequeue(tmp);
    std::cout << tmp << std::endl;

    return NULL;
}

int ExampleFunc(int num)
{
    std::cout << "hey" << std::endl;
    return 1;
}