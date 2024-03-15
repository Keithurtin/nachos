#include "syscall.h"

int main()
{
	int id = 0;
	int i = 0;
	int anotherid = 0;
	id = Open("text.txt", 0);
	PrintInt(id);
	PrintChar('\n');
	anotherid = Open("Makefile.common", 0);
	PrintInt(anotherid);
	PrintChar('\n');
	CloseFile(id);
	id = Open("Makefile.dep", 0);
	PrintInt(id);
	PrintChar('\n');
	
	Halt();
}