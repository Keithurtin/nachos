// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#define MaxFileLength 32

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void AdvancePC()
{
    int pcAfter = machine->ReadRegister(NextPCReg) + 4;
	  machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
    machine->WriteRegister(NextPCReg, pcAfter);
}

char* User2System(int virtAddr, int limit)
{
    int i;
    int oneChar;
    char* kernelBuf = NULL;
    kernelBuf = new char[limit + 1];
    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);
    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

int System2User(int virtAddr, int len, char* buffer)
{
    if (len < 0) return -1;
    if (len == 0) return len;
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    }while(i < len && oneChar != 0);
    return i;
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which) 
    {
      	case NoException: 
      		  return; 
      	case PageFaultException:
        		printf("\nPageFaultException, No valid translation found.");
       			interrupt->Halt();
       			break;
     		case ReadOnlyException:
       			printf("\nReadOnlyException, Write attempted to page marked read-only.");
       			interrupt->Halt();
       			break;
     		case BusErrorException:
       			printf("BusErrorException, Translation resulted in an invalid physical address.\n");
       			interrupt->Halt();
       			break;
     		case AddressErrorException:
       			printf("\nAddressErrorException, Unaligned reference or one that was beyond the end of the address space.");
       			interrupt->Halt();
       			break;
     		case OverflowException:
       			printf("\nOverflowException, Integer overflow in add or sub.");
       			interrupt->Halt();
       			break;
     		case IllegalInstrException:
       			printf("\nIllegalInstrException, Unimplemented or reserved instr.");
       			interrupt->Halt();
       			break;
     		case NumExceptionTypes:
       			printf("\nNumExceptionTypes.");
       			interrupt->Halt();
       			break;
     		case SyscallException:
       			switch (type)
       			{
         			case SC_Halt:
                    {
           				printf("\nShutdown, initiated by user program.");
                        interrupt->Halt();
           				AdvancePC();
                        return;
                    }

                    case SC_ReadInt:
                    {
                        // -2^31 <= integer number <= 2^31 -1
                        // => max length of an int number is 10 + 1 (for "-")
                        const int maxLen = 11;
                        int num = 0;
                        int numOfDigit = 0;
                        int digit = 0;
                        int mark = 1;
                        int startPoint = 0;
                        char* buffer = new char[maxLen + 1];

                        // Read the string of number from console 
                        numOfDigit = gSynchConsole->Read(buffer, maxLen);
                        // Check if number is negative
                        if (buffer[0] == '-')
                        {
                            mark = -1;
                            startPoint++;
                        }

                        // Read each digit and combine into an integer
                        for (int i = startPoint; i < numOfDigit; i++)
                        {
                            if (buffer[i] >= '0' && buffer[i] <= '9')
                            {
                                digit = (int)(buffer[i] - '0');
                                num = num * 10 + digit;
                            }
                            // User did not input a valid number
                            else
                            {
                                machine->WriteRegister(2, 0);
                                AdvancePC();
                                delete buffer;
                                return;
                            }
                        }
                        num *= mark;
                        machine->WriteRegister(2, num);
                        AdvancePC();
                        delete buffer;
                        return;
                    }

                    case SC_PrintInt:
                    {
                        // -2^31 <= integer number <= 2^31 -1
                        // => max length of an int number is 10 + 1 (for "-")
                        const int maxLen = 11;
                        int num = machine->ReadRegister(4);
                        char* buffer = new char[maxLen + 1];
                        int startPoint = 0;
                        int digit;
                        char* rev = new char[maxLen + 1];// To read reversed number
                        int numOfDigit = 0;
                        // If the number is negative
                        // Change number into positive and set first character of the string into "-"
                        // increase starting point of of the string by 1 
                        if (num < 0)
                        {
                            num = -num;
                            buffer[startPoint++] = '-';
                        }
                        // check if number is 0
                        else if (num == 0)
                        {
                            gSynchConsole->Write((char*)"0", 1);
                            AdvancePC();
                            delete buffer;
                            delete rev;
                            return;
                        }
                        // Read reversed number
                        while(num > 0)
                        {
                            digit = num % 10;
                            rev[numOfDigit++] = (char)(digit + 48);
                            num /= 10;
                        }
                        // Reverse the string no get right number
                        while(numOfDigit >= 0)
                        {
                            
                            buffer[startPoint++] = rev[--numOfDigit];
                        }
                        gSynchConsole->Write(buffer, startPoint);
                        AdvancePC();
                        delete buffer;
                        delete rev;
                        return;
                    }

                    case SC_ReadFloat:
                    {
                        // Max Length of a float is 328 
                        const int maxLen = 328;
                        float num = 0;
                        float decimal = 0;
                        int numOfDigit = 0;
                        int digit = 0;
                        int mark = 1;
                        int startPoint = 0;
                        int startDecimal = 0;
                        float devisor = 0;
                        char* buffer = new char[maxLen + 1];

                        // Read the string of number from console 
                        numOfDigit = gSynchConsole->Read(buffer, maxLen);
                        // Check if number is negative
                        if (buffer[0] == '-')
                        {
                            mark = -1;
                            startPoint++;
                        }

                        // Read digits before "."
                        for (int i = startPoint; i < numOfDigit; i++)
                        {
                            if (buffer[i] >= '0' && buffer[i] <= '9')
                            {
                                digit = (int)(buffer[i] - '0');
                                num = num * 10 + digit;
                            }
                            else if (buffer[i] == '.')
                            {
                                startDecimal = i + 1;
                                break;
                            }
                            // User did not input a valid number
                            else
                            {
                                num = 0.0;
                                machine->WriteRegister(2, *(int*)&num);
                                AdvancePC();
                                delete buffer;
                                return;
                            }
                        }
                        // Read digits after "."
                        for (int i = startDecimal; i < numOfDigit; i++)
                        {
                            if (buffer[i] >= '0' && buffer[i] <= '9')
                            {
                                digit = (int)(buffer[i] - '0');
                                devisor *= 10;
                                num += (digit / devisor);
                            }
                            // User did not input a valid number
                            else
                            {
                                num = 0.0;
                                machine->WriteRegister(2, *(int*)&num);
                                AdvancePC();
                                delete buffer;
                                return;
                            }
                        }
                        num *= mark;
                        num += decimal;

                        // Write address of the float number into register
                        // beacause register only accepts int value
                        machine->WriteRegister(2, *(int*)&num);
                        AdvancePC();
                        delete buffer;
                        return;
                    }

                    case SC_PrintFloat:
                    {
                        // Max Length of a float is 328 
                        const int maxLen = 328;
                        int num_int = machine->ReadRegister(4);
                        float num = *(float*)&num_int;
                        long long natural;
                        char* buffer = new char[maxLen + 1];
                        int startPoint = 0;
                        float digit;
                        char* rev = new char[maxLen + 1];// To read reversed number
                        int numOfDigit = 0;

                        // Take the natural part of a float number
                        natural = (int)num;
                        if (natural > num && num > 0)
                            natural--;
                        else if (natural < num && num < 0)
                            natural++;

                        // Convert natural part into a string
                        // If the number is negative
                        // Change number into positive and set first character of the string into "-"
                        // increase starting point of of the string by 1 
                        if (natural < 0)
                        {
                            natural = -natural;
                            buffer[startPoint++] = '-';
                        }
                        // check if natural is 0
                        else if (natural == 0)
                        {
                            buffer[startPoint++] = '0';
                        }

                        if(natural > 0)
                        {
                            // Read reversed natural
                            while(natural > 0)
                            {
                                digit = natural  % 10;
                                rev[numOfDigit++] = (char)(digit + 48);
                                natural /= 10;
                            }
                            // Reverse the string no get right natural part
                            while(numOfDigit >= 0)
                            {                             
                                buffer[startPoint++] = rev[--numOfDigit];
                            }
                            delete rev;
                        }

                        // Start reading the decimal part
                        buffer[startPoint++] = '.';
                        natural = (int) num;
                        num = num - natural;    // Take decimal part
                        // Check if decimal part is 0
                        if (num == 0.0)
                        {
                            buffer[startPoint++] = '0';
                            gSynchConsole->Write(buffer, startPoint);
                            delete buffer;
                            AdvancePC();
                            return;
                        }

                        // Read each digit of the decimal part one by one
                        while(num != 0.0)
                        {                          
                            num *= 10;
                            natural = (int)num;
                            if (natural > num && num > 0)
                                natural--;
                            else if (natural < num && num < 0)
                                natural++;
                            buffer[startPoint++] = (char)(natural + 48);
                            num -= natural;
                        }

                        gSynchConsole->Write(buffer, startPoint);
                        AdvancePC();
                        delete buffer;
                        return;
                    }

                    case SC_ReadChar:
                    {
                        char* buffer;
                        buffer = new char[1];
                        char c;
                        gSynchConsole->Read(buffer, 1);
                        c = buffer[0];
                        machine->WriteRegister(2, (int)c);
                        AdvancePC();
                        delete buffer;
                        return;
                    }

                    case SC_PrintChar:
                    {
                        char c = (char)machine->ReadRegister(4);
                        gSynchConsole->Write(&c, 1);
                        AdvancePC();
                        return;
                    }

                    case SC_ReadString:
                    {
                        int virtAddr, maxLen;
                        char* buffer;
                        // Take virtual address of string
                        virtAddr = machine->ReadRegister(4);
                        // Read max length of string
                        maxLen = machine->ReadRegister(5);
                        // Use function User2System to get string in user space
                        // into buffer in system space to read
                        buffer = User2System(virtAddr, maxLen);
                        // Read string
                        gSynchConsole->Read(buffer, maxLen);
                        // Put string back to user space from system space
                        System2User(virtAddr, maxLen, buffer);
                        delete buffer;
                        AdvancePC();
                        return;
                    }

                    case SC_PrintString:
                    {
                        int virtAddr;
                        char* buffer;
                        // Take virtual address of string
                        virtAddr = machine->ReadRegister(4);
                        // Use function User2System to take string in user space
                        // into buffer in system space to print
                        buffer = User2System(virtAddr, 255);  // Let maxLen be 255
                        // Read length of string
                        int maxLen = strlen(buffer);
                        // Print string
                        gSynchConsole->Write(buffer, maxLen);
                        delete buffer;
                        AdvancePC();
                        return;
                    }

                    case SC_CreateFile:
                    {
                        int virtAddr;
                        char* filename;
                        // Take virtual address of filename
                        virtAddr = machine->ReadRegister(4);
                        // Use function User2System to get string in user space
                        // into buffer in system space to read
                        filename = User2System(virtAddr, MaxFileLength + 1);
                        if (filename == NULL)
                        {
                            printf("\nNot enough memory in system");
                            // Return as error
                            machine->WriteRegister(2, -1);
                            delete filename;
                            AdvancePC();
                            return;
                        }
                        // Create file with size = 0
                        // Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
                        // việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
                        // hành Linux, chúng ta không quản ly trực tiếp các block trên
                        // đĩa cứng cấp phát cho file, việc quản ly các block của file
                        // trên ổ đĩa là một đồ án khác
                        if (!fileSystem->Create(filename, 0))
                        {
                            printf("\nError create file %s", filename);
                            machine->WriteRegister(2, -1);
                            delete filename;
                            AdvancePC();
                            return;
                        }
                        // Return success
                        machine->WriteRegister(2, 0);
                        delete filename;
                        AdvancePC();
                        return;
                    }

                    case SC_Open:
                    {
                        int virtAddr;
                        int type;
                        char* filename;
                        // Take virtual address of filename
                        virtAddr = machine->ReadRegister(4);
                        // Take file type
                        type = machine->ReadRegister(5);
                        
                        // Use function User2System to get string in user space
                        // into buffer in system space to read
                        filename = User2System(virtAddr, MaxFileLength + 1);
                        int index = fileSystem->FindFreeSpace();
                        // No more space to open new file
                        if (index == -1)
                        {
                            machine->WriteRegister(2, -1);
                            printf("\nAlready opened 10 files. No more space to open new file.");
                            delete filename;
                            AdvancePC();
                            return;
                        }
                        // Open console input or output if type is 2 or 3
                        if (type == 2)
                        {
                            machine->WriteRegister(2, 0);
                            delete filename;
                            AdvancePC();
                            return;
                        }
                        else if (type == 3)
                        {
                            machine->WriteRegister(2, 1);
                            delete filename;
                            AdvancePC();
                            return;
                        }
                        // Return error ff file type is not 0 or 1
                        else if (type != 0 && type != 1)
                        {
                            printf("\nInvalid file type");
                            machine->WriteRegister(2, -1);
                            delete filename;
                            AdvancePC();
                            return;
                        }

                        if ((fileSystem->openfile[index] = fileSystem->Open(filename, type)) != NULL)
                        {
                            //printf("\nOpen file %s success", filename);
                            machine->WriteRegister(2, index);
                        }
                        else
                        {
                            //printf("\nCannot open file %s", filename);
                            machine->WriteRegister(2, -1);
                        }
                        delete filename;
                        AdvancePC();
                        return;
                    }

                    case SC_Close:
                    {
                        int id = machine->ReadRegister(4);
                        
                        // Return error if id is not in 0-9
                        if (id < 0 || id > 9)
                        {
                            printf("\nFile ID out of range");
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }
                        // File is alread closed or does not exist
                        if (fileSystem->openfile[id] == NULL)
                        {
                            printf("\nFile is alread closed or does not exist");
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }
                        else
                        {
                            delete fileSystem->openfile[id];
                            fileSystem->openfile[id] = NULL;
                            machine->WriteRegister(2, 0);
                            //printf("\nClose file success");
                            AdvancePC();
                            return;
                        }
                    }
                    case SC_Read:
                    {
                        int virtAddr = machine->ReadRegister(4);
                        int charcount = machine->ReadRegister(5);
                        int id = machine->ReadRegister(6);
                        char* buffer;
                        // Return error if id is not in 0-9
                        if (id < 0 || id > 9)
                        {
                            printf("\nFile ID out of range");
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }
                        // File is closed or does not exist
                        if (fileSystem->openfile[id] == NULL)
                        {
                            printf("\nFile is closed or does not exist");
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }
                        // Cannot read console output
                        if (fileSystem->openfile[id]->type == 3)
                        {
                            printf("\nCannot read console output");
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }
                        // Use function User2System to get content in user space
                        // into buffer in system space to read
                        buffer = User2System(virtAddr, charcount);
                        // In case of reading input console
                        if (fileSystem->openfile[id]->type == 2)
                        {
                            // Use Read function of SynchConsole to return
                            // the true amount of byte that has been read
                            int size = gSynchConsole->Read(buffer, charcount);
                            // Put content back to user space from system space with true size
                            System2User(virtAddr, size, buffer);
                            // Return true size
                            machine->WriteRegister(2, size);
                            delete buffer;
                            AdvancePC();
                            return;
                        }
                        // Get the true amount of read byte
                        int size = fileSystem->openfile[id]->Read(buffer, charcount);
                        // Reading normal file
                        if (size > 0)
                        {
                            // Put content back to user space from system space with true size
                            System2User(virtAddr, size, buffer);
                            // Return true size
                            machine->WriteRegister(2, size);
                        }
                        // return -2 if end of file
                        else
                        {
                            machine->WriteRegister(2, -2);
                        }
                        delete buffer;
                        AdvancePC();
                        return;
                    }

                    case SC_Write:
                    {
                        int virtAddr = machine->ReadRegister(4);
                        int charcount = machine->ReadRegister(5);
                        int id = machine->ReadRegister(6);
                        char* buffer;
                        // Return error if id is not in 0-9
                        if (id < 0 || id > 9)
                        {
                            printf("\nFile ID out of range");
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }
                        // File is closed or does not exist
                        if (fileSystem->openfile[id] == NULL)
                        {
                            printf("\nFile is closed or does not exist");
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }
                        // Cannot write on read-only file or console input
                        if (fileSystem->openfile[id]->type == 1 || fileSystem->openfile[id]->type == 2)
                        {
                            printf("\nCannot write on this file");
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }
                        // Use function User2System to get content user want to write in 
                        // user space into buffer in system space to write
                        buffer = User2System(virtAddr, charcount);
                        // Get the true amount of writen byte
                        // In case of writing output console
                        if (fileSystem->openfile[id]->type == 3)
                        {
                            int size = 0;
                            int len = fileSystem->openfile[id]->Length();
                            // Write until string end
                            while(buffer[size] != '\0')
                            {
                                gSynchConsole->Write(buffer + size, 1);
                                size++;
                            }
                            gSynchConsole->Write(buffer + size, 1);
                            // Return the true amount of byte written
                            machine->WriteRegister(2, size);
                            delete buffer;
                            AdvancePC();
                            return;
                        }
                        // Get the true amount of read byte
                        int size = fileSystem->openfile[id]->Write(buffer, charcount);
                        // Write on read-and-write file
                        if (size > 0)
                        {
                            
                            // Return true size
                            machine->WriteRegister(2, size);
                        }
                        else
                        {
                            machine->WriteRegister(2, -2);
                        }
                        delete buffer;
                        AdvancePC();
                        return;
                    }

                    case SC_Len:
                    {
                        int id = machine->ReadRegister(4);
                        // Return error if id is not in 0-9
                        if (id < 0 || id > 9)
                        {
                            printf("\nFile ID out of range");
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }
                        // File is closed or does not exist
                        if (fileSystem->openfile[id] == NULL)
                        {
                            printf("\nFile is closed or does not exist");
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }
                        int len = fileSystem->openfile[id]->Length();
                        machine->WriteRegister(2, len);
                        AdvancePC();
                        return;
                    }

                    case SC_ToString:
                    {
                        // -2^31 <= integer number <= 2^31 -1
                        // => max length of an int number is 10 + 1 (for "-")
                        const int maxLen = 11;
                        int virtAddr = machine->ReadRegister(4);
                        int num = machine->ReadRegister(5);
                        char* buffer = User2System(virtAddr, maxLen + 1);
                        int startPoint = 0;
                        int digit;
                        char* rev = new char[maxLen + 1];// To read reversed number
                        int numOfDigit = 0;
                        // If the number is negative
                        // Change number into positive and set first character of the string into "-"
                        // increase starting point of of the string by 1 
                        if (num < 0)
                        {
                            num = -num;
                            buffer[startPoint++] = '-';
                        }
                        // check if number is 0
                        else if (num == 0)
                        {
                            System2User(virtAddr, 1, "0");
                            machine->WriteRegister(2, 1);
                            AdvancePC();
                            delete buffer;
                            delete rev;
                            return;
                        }
                        // Read reversed number
                        while(num > 0)
                        {
                            digit = num % 10;
                            rev[numOfDigit++] = (char)(digit + 48);
                            num /= 10;
                        }
                        // Reverse the string no get right number
                        while(numOfDigit >= 0)
                        {   
                            buffer[startPoint++] = rev[--numOfDigit];
                        }
                        System2User(virtAddr, startPoint, buffer);
                        machine->WriteRegister(2, startPoint);
                        AdvancePC();
                        delete buffer;
                        delete rev;
                        return;
                    }

                    case SC_Exec:
                    {
                        int virtAddr;
                        virtAddr = machine->ReadRegister(4);
                        char* name;
                        name = User2System(virtAddr, MaxFileLength + 1);

                        if (name == NULL)
                        {
                            printf("Name cannot be empty\n");
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }
                        // Cannot call system call SC_Open in execption.cc
                        // so we need to open file this way
                        OpenFile *file = fileSystem->Open(name);
                        if (file == NULL)
                        {
                            printf("Cannot open this file\n");
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }

                        int id = pTab->ExecUpdate(name);
                        machine->WriteRegister(2, id);

                        delete[] name;
                        delete file;
                        AdvancePC();
                        return;
                    }

                    case SC_Join:
                    {
                        int id = machine->ReadRegister(4);
                        int ans = pTab->JoinUpdate(id);

                        machine->WriteRegister(2, ans);
                        AdvancePC();
                        return;                        
                    }

                    case SC_Exit:
                    {
                        int exitCode = machine->ReadRegister(4);

                        if (exitCode)
                        {
                            AdvancePC();
                            return;
                        }
                        pTab->ExitUpdate(exitCode);

                        AdvancePC();
                        return;
                    }

                    case SC_CreateSemaphore:
                    {
                        int virtAddr = machine->ReadRegister(4);
                        int semval = machine->ReadRegister(5);
                        char* name; 
                        name = User2System(virtAddr, MaxFileLength + 1);

                        if (name == NULL)
                        {
                            printf("Name cannot be empty\n");
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }

                        int ans = semTab->Create(name ,semval);

                        if (ans == -1)
                        {
                            printf("Cannot create semaphore\n");
                            machine->WriteRegister(2, -1);
                            delete[] name;
                            AdvancePC();
                            return;
                        }

                        delete[] name;
                        machine->WriteRegister(2, ans);
                        AdvancePC();
                        return;
                    }

                    case SC_Up:
                    {
                        int virtAddr = machine->ReadRegister(4);
                        char* name;
                        name = User2System(virtAddr, MaxFileLength + 1);

                        if (name == NULL)
                        {
                            printf("Name cannot be empty\n");
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }

                        int ans = semTab->Signal(name);

                        if (ans == -1)
                        {
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }

                        delete[] name;
                        machine->WriteRegister(2, ans);
                        AdvancePC();
                        return;
                    }

                    case SC_Down:
                    {
                        int virtAddr = machine->ReadRegister(4);
                        char* name;
                        name = User2System(virtAddr, MaxFileLength + 1);

                        if (name == NULL)
                        {
                            printf("Name cannot be empty\n");
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }

                        int ans = semTab->Wait(name);

                        if (ans == -1)
                        {
                            machine->WriteRegister(2, -1);
                            AdvancePC();
                            return;
                        }

                        delete[] name;
                        machine->WriteRegister(2, ans);
                        AdvancePC();
                        return;
                    }
                }
       		
    }
}
