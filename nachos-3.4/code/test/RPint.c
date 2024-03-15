#include "syscall.h"

int main(int argc, char* argv[])
{
	int num = ReadInt();
	PrintInt(num);
	Halt();
	return 0;
}