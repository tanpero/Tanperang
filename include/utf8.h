#ifndef _UTF8_H_
#define _UTF8_H_

#include <stdint.h>

uint32_t getByteNumOfEncodingUtf8(int value);
uint32_t getByteNumOfDecodingUtf8(uint8_t byte);
uint8_t encodeUtf8(uint8_t* buf, int value);
int decodeUtf8(const uint8_t* bytePtr, uint32_t length);

#endif // !_UTF8_H_

