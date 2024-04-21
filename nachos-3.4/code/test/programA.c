#include "syscall.h"

int main()
{
    int i;
    for(i = 0; i < 1000; i++)
    {
        // print A once
        if(Down("A") == -1)
            return -1;

        PrintChar('A');

        // Wake programB up
        if( Up("B") == -1 )
            return -1;
    }

    return 0;
}