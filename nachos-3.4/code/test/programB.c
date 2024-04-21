#include "syscall.h"

int main()
{
    int i;
    for(i = 0; i < 1000; i++)
    {
        // print B once
        if(Down("B") == -1)
            return -1;

        PrintChar('B');

        // Wake programA up
        if(Up("A") == -1)
            return -1;
    }

    return 0;
}