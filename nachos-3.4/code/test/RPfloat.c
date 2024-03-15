#include "syscall.h"

int main()
{
	float number = ReadFloat();
	PrintFloat(number);
	Halt();
}