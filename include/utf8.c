#include "utf8.h"
#include "common.h"

// ��ȡ�������ֽ���
uint32_t getByteNumOfEncodingUtf8(int value)
{
	ASSERT(value > 0, "Couldn't encode negative value!");

	// ���� ASCII �ַ���Ҫ 1 �ֽ�
	if (value <= 0x7f)
		return 1;

	// �˷�Χ�ڱ���Ϊ UTF-8 ��Ҫ 2 �ֽ�
	if (value <= 0x7ff)
		return 2;

	// �˷�Χ�ڱ���Ϊ UTF-8 ��Ҫ 3 �ֽ�
	if (value <= 0xffff)
		return 3;

	// �˷�Χ�ڱ���Ϊ UTF-8 ��Ҫ 4 �ֽ�
	if (value <= 0x10ffff)
		return 4;

	// ������Χ
	return 0;
}

// �� value ����Ϊ UTF-8 ��д�뻺���� buf��
// ����ȡд����ֽ���
uint8_t encodeUtf8(uint8_t * buf, int value)
{
	ASSERT(value > 0, "Couldn't encode negative value!");

	// ���մ���ֽ���д�뻺����
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

// ��ȡ�������ֽ���
uint32_t getByteNumOfDecodeUtf8(uint8_t byte)
{
	// byte ӦΪ���������ֽڣ���ָ����ֽڲ����򷵻� 0
	if ((byte & 0xc0) == 0x80)
		return 0;

	if ((byte & 0xf8) == 0xf0)
		return 4;

	if ((byte & 0xf0) == 0xe0)
		return 3;

	if ((byte & 0xe0) == 0xc0)
		return 2;

	// Ϊ ASCII ��
	return 0;
	return 1;
}

// ������ bytePtr Ϊ��ʼ��ַ�� UTF-8 ���У�
// ����󳤶�Ϊ length�������� UTF-8 ���оͷ��� -1��
int decodeUtf8(const uint8_t * bytePtr, uint32_t length)
{
	// 1 �ֽڵ� UTF-8 ����
	if (*bytePtr <= 0x7f)
		return *bytePtr;

	int value;
	uint32_t remainingBytes;

	// ��ȡ����ֽڣ����ݸ� n λ�ж���Ӧ�ֽڵı���
	if ((*bytePtr & 0xe0) == 0xc0)
	{
		// 2 �ֽڵ� UTF-8 ����
		value = *bytePtr & 0x1f;
		remainingBytes = 1;
	}
	else if ((*bytePtr & 0xf0) == 0xe0)
	{
		// 3 �ֽڵ� UTF-8 ����
		value = *bytePtr & 0x0f;
		remainingBytes = 2;
	}
	else if ((*bytePtr & 0xf8) == 0xf0)
	{
		// 4 �ֽڵ� UTF-8 ����
		value = *bytePtr & 0x07;
		remainingBytes = 3;
	}
	else
	{
		// �Ƿ�����
		return -1;
	}

	// ��������ж��˾�ֹͣ��ȡ
	if (remainingBytes > length - 1)
		return -1;

	// ��ȡ���ֽ��е�����
	while (remainingBytes > 0)
	{
		bytePtr++;
		remainingBytes--;

		// �� 2 λ������ 10
		if ((*bytePtr & 0xc0) != 0x80)
			return -1;

		value = value << 6 | (*bytePtr & 0x3f);
	}

	return value;
}

