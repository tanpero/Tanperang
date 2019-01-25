#include <stdio.h>
#include <string.h>
#include "../include/parser.h"
#include "../vm/vm.h"
#include "../vm/core.h"

static void run(const char* path)
{
	const char* lastSlash = strrchr(path, '/');
	if (lastSlash != NULL)
	{
		char* root = (char*)malloc(lastSlash - path + 2);
		memcpy(root, path, lastSlash - path + 1);
		root[lastSlash - path + 1] = '\0';
		rootDir = root;
	}

	VM* vm = newVM();
	const char* sourceCode = readFile(path);

	struct Parser parser;
	initParser(vm, &parser, path, sourceCode);

#include "tokens.txt"
	while (parser.curToken.type != TOKEN_EOF)
	{
		getNextToken(&parser);
		printf("%dL: %s [", parser.curToken.lineNo, tokenArray[parser.curToken.type]);
		uint32_t idx = 0;
		while (idx < parser.curToken.lineNo)
		{
			printf("5c", *(parser.curToken.start + idx++));
		}
		puts(")");
	}
}

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		// TODO...
	}
	else
	{
		run(argv[1]);
	}
	return 0;
}
