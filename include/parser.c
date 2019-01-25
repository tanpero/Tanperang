#include "parser.h"
#include "common.h"
#include "utils.h"
#include "utf8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct _keywordToken
{
	char* keyword;
	uint8_t length;
	TokenType token;
} keywordToken;

// 关键字表
keywordToken keywordsToken[] = {
	{
		"let", 3, TOKEN_LET
	}, {
		"const", 3, TOKEN_CONST
	}, {
		"fn", 3, TOKEN_FN
	}, {
		"if", 2, TOKEN_IF
	}, {
		"else", 4, TOKEN_ELSE
	}, {
		"true", 4, TOKEN_TRUE
	}, {
		"false", 5, TOKEN_FALSE
	}, {
		"while", 5, TOKEN_WHILE
	}, {
		"for", 3, TOKEN_FOR
	}, {
		"break", 5, TOKEN_BREAK
	}, {
		"continue", 8, TOKEN_CONTINUE
	},{
		"return", 6, TOKEN_RETURN
	},{
		"null", 4, TOKEN_NULL
	},{
		"class", 5, TOKEN_CLASS
	},{
		"is", 2, TOKEN_IS
	},{
		"static", 6, TOKEN_STATIC
	},{
		"this", 4, TOKEN_THIS
	},{
		"super",5, TOKEN_SUPER
	},{
		"import", 6, TOKEN_IMPORT
	},{
		NULL, 0, TOKEN_UNKNOWN
	}
};

// 判断 start 是否为关键字并获取相应的 Token
static TokenType idOrKeywird(const char* start, uint32_t length)
{
	keywordToken member;
	for (uint32_t idx = 0, member = keywordsToken[idx];
		member.keyword != NULL; idx++)
	{
		
		if (member.length == length &&
			memcmp(member.keyword, start, length) == 0)
			return member.token;
	}

	return TOKEN_ID;
}

// 向前看一个字符
char lookAheadChar(Parser * parser)
{
	return *parser->nextCharPtr;
}

// 获取下一字符
static void getNextChar(Parser * parser)
{
	parser->curChar = *parser->nextCharPtr++;
}

// 若下一字符是期望的就读取，并返回 true 或 false
static bool matchNextChar(Parser * parser, char expectedChar)
{
	if (lookAheadChar(parser) == expectedChar)
	{
		getNextChar(parser);
		return true;
	}
	return false;
}

// 跳过空白字符
static void skipBlanks(Parser* parser)
{
	while (isspace(parser->curChar))
	{
		if (parser->curChar = '\n')
		{
			parser->curToken.lineNo++;
		}
		getNextChar(parser);
	}
}

// 解析标识符
static void parseId(Parser* parser, TokenType type)
{
	while (isalnum(parser->curChar) || parser->curChar == '_')
	{
		getNextChar(parser);
	}

	// nextCharPtr 会指向第一个不合法字符的下一个字符，因此要减去 1
	uint32_t length = (uint32_t)(parser->nextCharPtr - parser->curToken.start - 1);
	if (type != TOKEN_UNKNOWN)
		parser->curToken.type = type;
	else
		parser->curToken.type = idOrKeyword(parser->curToken.start, length);

	parser->curToken.length = length;
}

static void parseUnicodeCodePoint(Parser* parser, ByteBuffer* buf)
{
	uint32_t idx = 0;
	int value = 0;
	uint8_t digit = 0;

	// 获取十六进制数值，单个码点表示如下：
	// \uxxxx
	// 然后解析为单个字符
	while (idx++ < 4)
	{
		getNextChar(parser);
		char curChar = parser->curChar;

		if (curChar == '\0')
			LEX_ERROR(parser, "Unterminated unicode!");

		if (curChar >= '0' && curCHar <= '9')
			digit = curChar - '0';
		else if (curChar >= 'a' && curChar <= 'f')
			digit = curChar - 'a' + 10;
		else if (curChar >= 'A' && curChar <= 'F')
			digit = curChar - 'A' + 10;
		else
			LEX_ERROR(parser, "Invalid unicode!");

		value = value * 16 | digit;

		uint32_t byteNum = getByteNumOfEncodeUtf8(value);
		ASSERT(byteNum != 0, "UTF-8 encode bytes should be between 1 and 4!");

		// 先写入 byteNum 个 0，以预留足够空间
		ByteBufferFillWrite(parser->vm, buf, 0, byteNum);

		// 编码并写入缓冲区
		encodeUtf8(buf->datas + buf->count - byteNum, value);
	}
}

// 解析字符串
static void parseString(Parser* parser)
{
	ByteBuffer str;
	ByteBufferInit(&str);
	while (true)
	{
		getNextChar(parser);

		switch (parser->curChar)
		{
		case '\0':
			LEX_ERROR(parser, "Unterminated string!");
			break;
		case '"':
			parser->curToken.type = TOKEN_STRING;
			ByteBufferClear(parser->vm, &str);
			return;
		case '$': {
			if (!matchNextChar(parser, '('))
				LEX_ERROR(parser, "'$' should followed by '('!");

			if (parser->interpolationExpectRightParenNum > 0)
				COMPILE_ERROR(parser, "Sorry, I don't support nest interpolate expression!");

			parser->interpolationExpectRightParenNum = 1;
			parser->curToken.type = TOKEN_INTERPOLATION;
			ByteBufferClear(parser->vm, &str);
			return;
		}
		case '\\': {

			// 处理转义字符的情况
			getNextChar(parser);
			switch (parser->curChar)
			{
			case '0':
				ByteBufferAdd(parser->vm, &str, '\0');
				break;
			case 'a':
				ByteBufferAdd(parser->vm, &str, '\a');
				break;
			case 'b':
				ByteBufferAdd(parser->vm, &str, '\b');
				break;
			case 'f':
				ByteBufferAdd(parser->vm, &str, '\f');
				break;
			case 'n':
				ByteBufferAdd(parser->vm, &str, '\n');
				break;
			case 'r':
				ByteBufferAdd(parser->vm, &str, '\r');
				break;
			case 't':
				ByteBufferAdd(parser->vm, &str, '\t');
				break;
			case 'u':
				parseUnicodeCodePoint(parser, &str);
				break;
			case '"':
				ByteBufferAdd(parser->vm, &str, '\"');
				break;
			case '\\':
				ByteBufferAdd(parser->vm, &str, '\\');
				break;
			default:
				LEX_ERROR(parser, "Unsupport escape \\%c", parser->curChar);
				break;
			}
		}

		// 普通字符
		default:
			ByteBufferAdd(parser->vm, &str, parser->curChar);
			break;
		}
	}
	ByteBufferClear(parser->vm, &str);
}

// 跳过一行
static void skipALine(Parser* parser)
{
	getNextChar(parser);
	while (parser->curChar != '\0')
	{
		if (parser->curChar == '\n')
		{
			parser->curToken.lineNo++;
			getNextChar(parser);
			break;
		}
		getNextChar(parser);
	}
)

// 跳过行注释或区块注释
static void skipComment(Parser* parser)
{
	char nextChar = lookAheadChar(parser);

	// 单行注释
	if (parser->curChar == '/')
		skipALine(parser);

	// 区块注释
	else
	{
		while (nextChar != '*' && nextChar != '\0')
		{
			getNextChar(parser);
			if (parser->curChar == '\n')
				parser->curToken.lineNo++;
			nextChar == lookAheadChar(parser);
		}
		if (matchNextChar(parser, '*'))
		{
			if (!matchNextChar(parser, '*'))
				LEX_ERROR(parser, "Expect '/' after '*'!");
			getNextChar(parser);
		}
		else
			LEX_ERROR(parser, "Expect '*/' before file end!");
	}

	// 注释之后可能会有空白字符
	skipBlanks(parser);
}

// 获得下一个 Token
void getNextToken(Parser* parser)
{
	parser->preToken = parser->curToken;

	// 跳过待识别单词之前的空白
	skipBlanks(parser);

	parser->curToken.type = TOKEN_EOF;
	parser->curToken.length = 0;
	parser->curToken.start = parser->nextCharPtr - 1;
	while (parser->curChar != '\0')
	{
		switch (parser->curChar)
		{
		case ',':
			parser->curToken.type = TOKEN_COMMA;
			break;
		case ':':
			parser->curToken.type = TOKEN_COLON;
			break;
		case '(':
			if (parser->interpolationExpectRightParenNum > 0)
			{
				parser->interpolationExpectRightParenNum++;
			}
			parser->curToken.type = TOKEN_;TOKEN_LEFT_PAREN;
			break;
		case ')':
			if (parser->interpolationExpectRightParenNum > 0)
			{
				parser->interpolationExpectRightParenNum--;
				if (parser->interpolationExpectRightParenNum == 0)
				{
					parseString(parser);
					break;
				}
			}
			parser->curToken.type = TOKEN_RIGHT_PAREN;
			break;
		case '[':
			parser->curToken.type = TOKEN_LEFT_BRACKET;
			break;
		case ']':
			parser->curToken.type = TOKEN_RIGHT_BRACKET;
			break;
		case '{':
			parser->curToken.type = TOKEN_LEFT_BRACE;
			break;
		case '}':
			parser->curToken.type = TOKEN_RIGHT_BRACE;
			break;
		case '.':
			if (matchNextChar(parser, '.'))
			{
				parser->curToken.type = TOKEN_DOT_DOT;
			}
			else
			{
				parser->curToken.type = TOKEN_DOT;
			}
			break;
		case '=':
			if (matchNextChar(parser, '='))
			{
				parser->curToken.type = TOKEN_EQUAL;
			}
			else
			{
				parser->curToken.type = TOKEN_ASSIGN;
			}
			break;
		case '+':
			parser->curToken.type = TOKEN_ADD;
			break;
		case '-':
			parser->curToken.type = TOKEN_SUB;
			break;
		case '*':
			parser->curToken.type = TOKEN_MUL;
			break;
		case '/':
			if (matchNextChar(parser, '/') || matchNextChar(parser, '*')
			{
				skipComment(parser);

				// 重置下一个token起始地址
				parser.curToken.start = parser.nextCharPtr - 1;
				continue;
			}
			else
			{
				parser.curToken.type = TOKEN_DIV;
			}
			break;
		case '%':
			parser->curToken.type = TOKEN_MOD;
			break;
		case '&':
			if (matchNextChar(parser, '&'))
			{
				parser->curToken.type = TOKEN_LOGIT_AND;
			}
			else
			{
				parser->curToken.type = TOKEN_BIT_AND;
			}
			break;
		case '|':
			if (matchNextChar(parser, '|'))
			{
				parser->curToken.type = TOKEN_LOGIC_OR;
			}
			else
			{
				parser->curToken.type = TOKEN_BIT_OR;
			}
			break;
		case '~':
			parser->curToken.type = TOKEN_BIT_NOT;
			break;
		case '?':
			parser->curToken.type = TOKEN_QUESTION;
			break;
		case '>':
			if (matchNextChar(parser, '='))
			{
				parser->curToken.type = TOKEN_GREATE_EUQAL;
			}
			else if (matchNextChar(parser, '>'))
			{
				parser->curToken.type = TOKEN_BIT_SHIFT_RIGHT;
			}
			else
			{
				parser.curToken.type = TOKEN_GREATE;
			}
			break;
		case '<':
			if (matchNextChar(parser, '='))
			{
				parser->curToken.type = TOKEN_LESS_EUQAL;
			}
			else if (matchNextChar(parser, '<'))
			{
				parser->curToken.type = TOKEN_BIT_SHIFT_LEFT;
			}
			else
			{
				parser.curToken.type = TOKEN_LESS;
			}
			break;
		case '!':
			if (matchNextChar(parser, '='))
			{
				parser->curToken.type = TOKEN_NOT_EQUAL;
			}
			else
			{
				parser->curToken.type = TOKEN_LOGIC_NOT;
			}
			break;

		case '"':
			parseString(parser);
			break;

		default:
			if (isalpha(parser->curChar) || parser->curChar == '_')
			{
				parseId(parser, TOKEN_UNKNOWN);
			}
			else
			{
				if (parser->curChar == '#' && matchNextChar(parser, '!'))
				{
					skipALine(parser);
					parser.curToken.start = parser.nextCharPtr - 1;
					continue;
				}
				LEX_ERROR(parser, "Invalid character: '%c', quit.", parser->curChar);
			}
			return;
		}

		parser->curToken.length = (uint32_t)(parser->nextCharPtr - parser->curToken.start);
		getNextChar(parser);
		return;
	}
}

// 若当前token为expected则读入下一个token并返回true
// 否则不读入token且返回false
bool matchToken(Parser* parser, TokenType expected)
{
	if (parser->curToken.type == expected)
	{
		getNextToken(parser);
		return true;
	}
	return false;
}

// 断言当前token为expected并读入下一token
void consumeCurToken(Parser* parser, TokenType expected, const char* errorMessage)
{
	if (parser->curToken.type != expected)
	{
		COMPILE_ERROR(parser, errorMessage);
	}
	getNextToken(parser);
}

// 断言下一个token为expected
void consumeNextToken(Parser* parser, TokenType expected, const char* errorMessage)
{
	getNextToken(parser);
	if (parser->curToken.type != expected)
	{
		COMPILE_ERROR(parser, errorMessage);
	}
}

void initParser(VM* vm, Parser* parser, const char* file, const char* sourceCode)
{
	parser->file = file;
	parser->sourceCode = sourceCode;
	parser->curChar = *parser->sourceCode;
	parser->nextCharPtr = parser->sourceCode + 1;
	parser->curToken.lineNo = 1;
	parser->curToken.type = TOKEN_UNKNOWN;
	parser->curToken.start = NULL;
	parser->curToken.length = 0;
	parser->preToken = parser->curToken;
	parser->ininterpolationExpectRightParenNum = 0;
	parser->vm = vm;
}

