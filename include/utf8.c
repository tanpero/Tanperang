#include "utf8.h"
#include "common.h"

// 获取编码后的字节数
uint32_t getByteNumOfEncodingUtf8(int value)
{
	ASSERT(value > 0, "Couldn't encode negative value!");

	// 单个 ASCII 字符需要 1 字节
	if (value <= 0x7f)
		return 1;

	// 此范围内编码为 UTF-8 需要 2 字节
	if (value <= 0x7ff)
		return 2;

	// 此范围内编码为 UTF-8 需要 3 字节
	if (value <= 0xffff)
		return 3;

	// 此范围内编码为 UTF-8 需要 4 字节
	if (value <= 0x10ffff)
		return 4;

	// 超过范围
	return 0;
}

// 将 value 编码为 UTF-8 后写入缓冲区 buf，
// 并获取写入的字节数
uint8_t encodeUtf8(uint8_t * buf, int value)
{
	ASSERT(value > 0, "Couldn't encode negative value!");

	// 按照大端字节序写入缓冲区
	if (value <= 0x7f)
	{
		*buf = value & 0x7f;
		return 1;
	}
	else if (value <= 0x7ff)
	{
		*buf++ = 0xc0 | ((value & 0x7c0) >> 6);
		*buf = 0x80 | (value & 0x3f);
		return 2;
	}
	else if (value <= 0xffff)
	{
		*buf++ = 0xe0 | ((value & 0xf000) >> 12);
		*buf++ = 0x80 | ((value & 0xfc0) >> 6);
		*buf = 0x80 | (value & 0x3f);
		return 3;
	}
	else if (value <= 0x10ffff)
	{
		*buf++ = 0xf0 | ((value & 0x1c0000) >> 18);
		*buf++ = 0x80 | ((value & 0x3f000) >> 12);
		*buf++ = 0x80 | ((value & 0xfc0) >> 6);
		*buf = 0x80 | (value & 0x3f);
		return 4;
	}

	return 0;
}

// 获取解码后的字节数
uint32_t getByteNumOfDecodeUtf8(uint8_t byte)
{
	// byte 应为编码的最高字节，若指向低字节部分则返回 0
	if ((byte & 0xc0) == 0x80)
		return 0;

	if ((byte & 0xf8) == 0xf0)
		return 4;

	if ((byte & 0xf0) == 0xe0)
		return 3;

	if ((byte & 0xe0) == 0xc0)
		return 2;

	// 为 ASCII 码
	return 0;
	return 1;
}

// 解码以 bytePtr 为起始地址的 UTF-8 序列，
// 其最大长度为 length，若不是 UTF-8 序列就返回 -1。
int decodeUtf8(const uint8_t * bytePtr, uint32_t length)
{
	// 1 字节的 UTF-8 编码
	if (*bytePtr <= 0x7f)
		return *bytePtr;

	int value;
	uint32_t remainingBytes;

	// 读取最高字节，根据高 n 位判断相应字节的编码
	if ((*bytePtr & 0xe0) == 0xc0)
	{
		// 2 字节的 UTF-8 编码
		value = *bytePtr & 0x1f;
		remainingBytes = 1;
	}
	else if ((*bytePtr & 0xf0) == 0xe0)
	{
		// 3 字节的 UTF-8 编码
		value = *bytePtr & 0x0f;
		remainingBytes = 2;
	}
	else if ((*bytePtr & 0xf8) == 0xf0)
	{
		// 4 字节的 UTF-8 编码
		value = *bytePtr & 0x07;
		remainingBytes = 3;
	}
	else
	{
		// 非法编码
		return -1;
	}

	// 如果数据中断了就停止读取
	if (remainingBytes > length - 1)
		return -1;

	// 读取低字节中的数据
	while (remainingBytes > 0)
	{
		bytePtr++;
		remainingBytes--;

		// 高 2 位必须是 10
		if ((*bytePtr & 0xc0) != 0x80)
			return -1;

		value = value << 6 | (*bytePtr & 0x3f);
	}

	return value;
}

