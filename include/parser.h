#ifndef _INCLUDE_PARSER_PARSER_H_
#define _INCLUDE_PARSER_PARSER_H_

#include "common.h"
#include "..\vm\vm.h"

typedef enum
{
	// 未知的 Token
	TOKEN_UNKNOWN,
	TOKEN_NUMBER,

	// 数据
	TOKEN_STRING,
	TOKEN_ID,
	TOKEN_INTERPOLATION,

	// 关键字
	TOKEN_LET,
	TOEKN_CONST,
	TOKEN_FN,
	TOKEN_IF,
	TOKEN_ELSE,
	TOKEN_TRUE,
	TOKEN_FALSE,
	TOKEN_WHILE,
	TOKEN_FOR,
	TOKEN_IN,
	TOKEN_OF,
	TOKEN_BREAK,
	TOKEN_CONTINUE,
	TOKEN_RETURN,
	TOKEN_NULL,

	// 关于类和模块的关键字
	TOKEN_CLASS,
	TOKEN_THIS,
	TOKEN_STATIC,
	TOKEN_IS,
	TOKEN_SUPER,
	TOKEN_IMPORT,

	// 分隔符
	TOKEN_COMMAD,
	TOKEN_COLON,
	TOKEN_LEFT_PAREN,
	TOKEN_RIGHT_PAREN,
	TOKEN_LEFT_BRACKET,
	TOKEN_RIGHT_BRACKET,
	TOKEN_LEFT_BRACE,
	TOKEN_RIGHT_BRACE,
	TOKEN_DOT,
	TOKEN_DOT_DOT,

	// 双目算术运算符
	TOKEN_ADD,
	TOKEN_SUB,
	TOKEN_MUL,
	TOKEN_DIV,
	TOKEN_MOD,

	// 赋值运算符
	TOKEN_ASSIGN,

	// 位运算符
	TOKEN_BIT_AND,
	TOKEN_BIT_OR,
	TOKEN_BIT_NOT,
	TOKEN_BIT_SHIFT_RIGHT,
	TOKEN_BIT_SHIFT_KEFT,

	// 逻辑运算符
	TOKEN_LOGIC_AND,
	TOKEN_LOGIC_OR,
	TOKEN_LOGIC_NOT,

	// 关系操作符
	TOKEN_EQUAL,
	TOKEN_NOT_EQUAL,
	TOKEN_GREATE,
	TOKEN_GREATE_EQUAL,
	TOKEN_LESS,
	TOKEN_LESS_EQUAL,

	TOKEN_QUESTION,

	// 文件结束标记
	// 仅词法分析时使用
	TOKEN_EOF
} TokenType;

typedef struct
{
	TokenType type;
	const char* start;
	uint32_t length;
	uint32_t lineNo;
} Token;

struct parser
{
	const char* file;
	const char* sourceCode;
	const char* nextCharPtr;
	char curChar;
	Token curToken;
	Token preToken;

	// 处于内联表达式之中时，期望的右括号数量
	// 用于跟踪小括号对的嵌套
	int interpolationExpectRightParenNum;
	VM* vm;
};

#define PEEK_TOKEN(parserPtr) parserPtr->curToken.type

char lookAheadChar(Parser* parser);
void getNextToken(Parser* parser);
bool matchToken(Parser* parser, TokenType expected);
void consumeCurToken(Parser* parser, TokenType expected, const char* errorMessage);
void consumeNextToken(Parser* parser, TokenType expected, const char* errorMessage);
uint32_t getByteNumOfEncodingUtf8(uint8_t* buf, int value);
void initParser(VM* vm, Parser* parser, const char* file, const char* sourceCode);

#endif // !_INCLUDE_PARSER_PARSER_H_

