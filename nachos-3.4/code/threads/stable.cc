#include "stable.h"

STable::STable(){
	this->bm = new BitMap(MAX_SEMAPHORE);
	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		semTab[i] = NULL;
	}
}

STable::~STable()
{
	if (this->bm)
		delete bm;
	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		if (semTab[i])
			delete semTab[i];
	}
}

int STable::Create(char* name, int init)
{
	// Check if this semaphore is already exist
	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		// Check if bit i is used
		if (this->bm->Test(i))
		{
			if (strcmp(name, semTab[i]->GetName()) == 0)
			{
				printf("This semaphore is already exist\n");
				return -1;
			}
		}
	}

	int id = this->FindFreeSlot();
	if (id < 0)
		return -1;
	this->semTab[id] = new Sem(name, init);
	return 0;
}

int STable::Wait(char* name)
{
	// Check if this semaphore is already exist
	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		// Check if bit i is used
		if (this->bm->Test(i))
		{
			if (strcmp(name, semTab[i]->GetName()) == 0)
			{
				semTab[i]->wait();
				return 0;
			}
		}
	}
	printf("This semaphore is not exist\n");
	return -1;
}

int STable::Signal(char* name)
{
	// Check if this semaphore is already exist
	for (int i = 0; i < MAX_SEMAPHORE; i++)
	{
		// Check if bit i is used
		if (this->bm->Test(i))
		{
			if (strcmp(name, semTab[i]->GetName()) == 0)
			{
				semTab[i]->signal();
				return 0;
			}
		}
	}
	printf("This semaphore is not exist\n");
	return -1;
}

int STable::FindFreeSlot()
{
	return this->bm->Find();
}
