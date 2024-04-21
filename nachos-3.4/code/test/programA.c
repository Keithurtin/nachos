#include "syscall.h"

int main()
{
    int i;
    for(i = 0; i < 1000; i++)
    {
        // print A once
        Down("A");
        PrintChar('A');
        // Wake programB up
        Up("B");
    }

    return 0;
}