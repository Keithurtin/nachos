#include "syscall.h"

int main()
{
	int id;
	int size;
	int sizeW;
	char* buffer;
	id = Open("text.txt", 0);
	//size = Read(buffer, 25, 0);
	sizeW = Write("stupid nachos", 13, id);
	PrintInt(sizeW);
	CloseFile(id);
	//rintInt(size);
	//PrintChar('\n');
	//PrintString(buffer);
	Halt();
}