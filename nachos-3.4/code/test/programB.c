#include "syscall.h"

int main()
{
    int i;
    for(i = 0; i < 1000; i++)
    {
        // print B once
        Down("B");
        PrintChar('B');
        // Wake programA up
        Up("A");
    }

    return 0;
}