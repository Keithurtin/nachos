#define MAX_SEMAPHORE 10

#include "sem.h"
#include "bitmap.h"
#include "synch.h"
#include "thread.h"

class STable
{
private:
	BitMap *bm;
	Sem* semTab[MAX_SEMAPHORE];
public:
	STable();
	~STable();
	int Create(char* name, int init);
	int Wait(char* name);
	int Signal(char* name);
	int FindFreeSlot();
};