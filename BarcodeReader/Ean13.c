/**
* EAN13 encoder/decoder long string <=> char array
**/

#include <string.h>

#include "Ean13.h"

#define L 0
#define G 1
#define R 2

static char leftPatterns[10][6] = {
	{L, L, L, L, L, L},
	{L, L, G, L, G, G},
	{L, L, G, G, L, G},
	{L, L, G, G, G, L},
	{L, G, L, L, G, G},
	{L, L, G, L, L, G},
	{L, G, G, G, L, L},
	{L, G, L, G, L, G},
	{L, G, L, G, G, L},
	{L, G, G, L, G, L},
};

static char digitEncoding[10][3] = {
	{0x0D, 0x27, 0x72}, 
	{0x19, 0x33, 0x66}, 
	{0x13, 0x1B, 0x6C}, 
	{0x3D, 0x21, 0x42}, 
	{0x23, 0x1D, 0x5C}, 
	{0x31, 0x39, 0x4E}, 
	{0x2F, 0x05, 0x50}, 
	{0x3B, 0x11, 0x44}, 
	{0x37, 0x09, 0x48}, 
	{0x0B, 0x17, 0x74}, 
};

#define PRINT_BYTE_BIN(b)	printf("%d%d%d%d%d%d%d ", 	(b >> 6) & 1, (b >> 5) & 1, \
													(b >> 4) & 1, (b >> 3) & 1, \
													(b >> 2) & 1, (b >> 1) & 1, \
													b & 1)

int Ean13_Encode(const char str[DECODED_BUFFER_SIZE], char encoded[ENCODED_BUFFER_SIZE])
{
	int firstDigit = str[0] - '0';
	char * pattern = leftPatterns[firstDigit];
	int i;
	for (i = 0; i < 6; ++i)
	{
		int current = str[i+1] - '0';
		encoded[i] = digitEncoding[current][(int)pattern[i]];
	}
	for (i = 6; i < 12; ++i)
	{
		int current = str[i+1] - '0';
		encoded[i] = digitEncoding[current][R];
	}
	return 0;
}

/*
b = (byte)((b & 0x55) + ((b >>  1) & 0x55)); 
		b = (byte)((b & 0x33) + ((b >>  2) & 0x33));
		b = (byte)((b & 0x0f) + ((b >>  4) & 0x0f));
*/
static char CountCharBits(char c)
{
	c = (c & 0x55) + ((c >> 1) & 0x55);
	c = (c & 0x33) + ((c >> 2) & 0x33);
	c = (c & 0x0f) + ((c >> 4) & 0x0f);
	return c;
}

static int DecodeLeftDigit(char digit, char * decoded, char * encodingType)
{
	int i;
	*encodingType = (CountCharBits(digit) % 2 == 0) ? G : L;
	for (i = 0; i < sizeof(digitEncoding) / sizeof(digitEncoding[0]); ++i)
	{
		if (digitEncoding[i][(int)*encodingType] == digit)
		{
			*decoded = (char)(i + '0');
			return 0;
		}
	}
	return 1;
}

static int DecodeRightDigit(char digit, char * decoded)
{
	int i;
	for (i = 0; i < sizeof(digitEncoding) / sizeof(digitEncoding[0]); ++i)
	{
		if (digitEncoding[i][R] == digit)
		{
			*decoded = (char)(i + '0');
			return 0;
		}
	}
	return 1;
}

int Ean13_Decode(const char barcode[ENCODED_BUFFER_SIZE], char decoded[DECODED_BUFFER_SIZE])
{
	char currentLeftPattern[6];
	int i;
	for (i = 0; i < 6; ++i)
	{
		if (DecodeLeftDigit(barcode[i], &decoded[i+1], &currentLeftPattern[i]))
		{
			return 1;
		}
	}
	for (i = 6; i < 12; ++i)
	{
		if (DecodeRightDigit(barcode[i], &decoded[i+1]))
		{
			return 1;
		}
	}
	for (i = 0; i < sizeof(leftPatterns) / sizeof(leftPatterns[0]); ++i)
	{
		if (memcmp(leftPatterns[i], currentLeftPattern, sizeof(currentLeftPattern)) == 0)
		{
			decoded[0] = (char)i + '0';
			decoded[13] = '\0';
			return 0;
		}
	}
	return 1;
}