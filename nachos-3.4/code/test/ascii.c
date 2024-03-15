#include "syscall.h"

int main()
{
	char ascii[95];
	int size = 0;
	int i = 32;
	int id;
	CreateFile("ascii.txt");
	id = Open("ascii.txt", 0);
	for (i; i < 127; i++) 
	{
		ascii[size] = (char)i;
		PrintChar(ascii[size]);
		PrintChar('\n');
		Write(ascii + size, 1, id);
		Write("\r\n", 2, id);
		size++;
	}
	CloseFile(id);
	Halt();
	return 0;
}