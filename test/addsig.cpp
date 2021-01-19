#include <iostream>

using namespace std;

void func(int a)
{
    cout << a;
}

void addsig(void (*handler)(int))
{
    handler(2);
    cout << '\n';
}

void addsig2(void handler(int))
{
    handler(3);
    cout << '\n';
}

int main()
{
    addsig(func);
    cout << "---------\n";
    addsig2(func);

    return 0;
}