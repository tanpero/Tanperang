#ifndef _INCLUDE_PARSER_PARSER_H_
#define _INCLUDE_PARSER_PARSER_H_

#include "common.h"
#include "..\vm\vm.h"

typedef enum
{
	// δ֪�� Token
	TOKEN_UNKNOWN,
	TOKEN_NUMBER,

	// ����
	TOKEN_STRING,
	TOKEN_ID,
	TOKEN_INTERPOLATION,

	// �ؼ���
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

	// �������ģ��Ĺؼ���
	TOKEN_CLASS,
	TOKEN_THIS,
	TOKEN_STATIC,
	TOKEN_IS,
	TOKEN_SUPER,
	TOKEN_IMPORT,

	// �ָ���
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

	// ˫Ŀ���������
	TOKEN_ADD,
	TOKEN_SUB,
	TOKEN_MUL,
	TOKEN_DIV,
	TOKEN_MOD,

	// ��ֵ�����
	TOKEN_ASSIGN,

	// λ�����
	TOKEN_BIT_AND,
	TOKEN_BIT_OR,
	TOKEN_BIT_NOT,
	TOKEN_BIT_SHIFT_RIGHT,
	TOKEN_BIT_SHIFT_KEFT,

	// �߼������
	TOKEN_LOGIC_AND,
	TOKEN_LOGIC_OR,
	TOKEN_LOGIC_NOT,

	// ��ϵ������
	TOKEN_EQUAL,
	TOKEN_NOT_EQUAL,
	TOKEN_GREATE,
	TOKEN_GREATE_EQUAL,
	TOKEN_LESS,
	TOKEN_LESS_EQUAL,

	TOKEN_QUESTION,

	// �ļ��������
	// ���ʷ�����ʱʹ��
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

	// �����������ʽ֮��ʱ������������������
	// ���ڸ���С���ŶԵ�Ƕ��
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

