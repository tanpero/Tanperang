#ifndef _INCLUDE_COMMON_H_
#define _INCLUDE_COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define UNUSED __attribute__ ((unused))

#ifdef _DEBUG
#	define ASSERT(condition, message)\
		do\
		{\
			if (!(condition))\
			{\
				fprintf(stderr, "ASSERT failed! %s:%d in function %s(): %s\n",\
						__FILE__, __LINE__, __func__, message);\
				abort();\
			}\
		}\
		while (0);
#else
#	define ASSERT(condition, message) ((void)0)
#endif

typedef struct vm VM;
typedef struct parser Parser;
typedef struct class Class;

#define VERSION_MAJOR 0
#define VERSION_MINOR 1

#define VERSION VERSION_MAJOR ##.## VERSION_MINOR 


#endif // !_INCLUDE_COMMON_H_

