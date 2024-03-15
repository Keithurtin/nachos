#include "syscall.h"

int main()
{
	PrintInt(CreateFile((char*)"text.txt"));
	Halt();
	return 0;
}