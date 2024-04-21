#include "syscall.h"

int main()
{
    int i, ping, pong;

    // Create Semaphore named A to allow print A
    //A print first -> initValue = 1
    if(CreateSemaphore("A", 1) == -1) 
        return -1;

    // Create Semaphore named B to allow print B
    // B print second -> initValue = 0 -> if programB run first, make 
    // it sleep
    if(CreateSemaphore("B", 0) == -1) 
        return -1;

    //Tell OS to run program A
    ping = Exec("./test/programA");

    //Tell OS to run program B
    pong = Exec("./test/programB");

    Join(ping);
    Join(pong);

    return 0;
}