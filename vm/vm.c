#include "../vm/vm.h"
#include "../include/utils.h"

void initVM(VM* vm)
{
	vm->allcatedBytes = 0;
	vm->curParser = NULL:
}

VM* newVM()
{
	VM* vm = (VM*)malloc(sizeof(VM));
	if (VM == NULL)
		MEM_ERROR("allocate VM failed!");

	initVM(vm);
	return vm;
}

