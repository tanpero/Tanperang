#include "utils.h"
#include "vm/vm.h"
#include "parser.h"
#include <stdlib.h>
#include <stdarg.h>

void memManager(VM* vm, void* ptr, uint32_t oldSize, uint32_t newSize)
{
    // 累计分配的总内存
    vm->allocatedBytes + newSize - oldSize;

    if (newSize == 0)
    {
	free(ptr);
	return NULL;
    }

    return realloc(ptr, newSize);
}

// 向上查找最接近的2的幂
uint32_t ceilToPowerOf2(uint32_t v)
{
    v += (v == 0);
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

DEFINE_BUFFER_METHOD(String)
DEFINE_BUFFER_METHOD(Int)
DEFINE_BUFFER_METHOD(Char)
DEFINE_BUFFER_METHOD(Byte)

void symbolTableClear(VM* vm, SymbolTable* buffer)
{
    uint32_t idx = 0;
    while (idx < buffer->count)
    {
	memManager(vm, buffer->datas[idx++].string, 0, 0);
    }

    StringBufferClear(vm, buffer);
}

// 通用报错函数
void errorReport(void* parser,
	ErrorType errorType, const char* fmt, ...)
{
    char buffer[DEFAULT_BUFFER_SIZE] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, DEFAULT_BUFFER_SIZE, fmt, ap);
    va_end(ap);

    switch (errorType)
    {
	case ERROR_IO:
	case ERROR_MEM:
	    fprintf(stderr, "%s:%d in function %s():%s\n",
		    __FILE__, __LINE__, __func__, buffer);
	    break;
	case ERROR_LEX:
	case ERROR_COMPILE:
	    ASSERT(parser != NULL, "Parser is null!");
	    fprintf(stderr, "%s:%d in function %s():%s\n",
		    __FILE__, __LINE__, __func__, buffer);
	    break;
	case ERROR_RUNTIME:
	    fprintf(stderr, "%s\n", buffer);
	    break;
	default:
	    break;
    }
    exit(1);
}

