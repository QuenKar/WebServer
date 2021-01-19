#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

using namespace std;

int main()
{
    struct timeval now = {0, 0};

    gettimeofday(&now, NULL);

    cout << now.tv_sec << endl;
    cout << now.tv_usec;

    return 0;
}