#include "syscall.h"

int main()
{
    int i;
    char* name;

    //Create Semaphore named A to allow print A
    name = "A";
    if(CreateSemaphore(name, 1) == -1) //A print first -> initValue = 1
        return -1;

    //Create Semaphore named B to allow print B
    name = "B";
    if(CreateSemaphore(name, 0) == -1) //B print second -> initValue = 0 -> if programB run first, make it sleep
        return -1;

    //Tell OS to run program A
    name = "programA.c";
    Exec(name);

    //Tell OS to run program B
    name = "programB.c";
    Exec(name);

    return 0;
}