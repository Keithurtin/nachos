#include "syscall.h"

int main()
{
	char* str;
	ReadString(str, 20);
	PrintString(str);
	Halt();
}