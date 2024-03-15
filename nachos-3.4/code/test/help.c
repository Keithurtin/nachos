#include "syscall.h"

int main()
{
	int id;
	int len;
	char* buffer;
	int i = 0;
	id = Open("mota.txt", 0);
	len = Len(id);
	Read(buffer, len, id);
	PrintString(buffer);
	Halt();
}