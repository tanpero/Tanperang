#ifndef _INCLUDE_VM_VM_H_
#define _INCLUDE_VM_VM_H_

#include "../include/common.h"

struct vm
{
	uint32_t allocatedBytes;
	Parser* curParser;
};

void initVM(VM* vm);
VM* newVM(void);

#endif _INCLUDE_VM_VM_H_

