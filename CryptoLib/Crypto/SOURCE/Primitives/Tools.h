
#pragma once

inline void XorData(size_t dataLength,
					const void* inputData1,
					const void* inputData2,
					void* outputData)
{
	const unsigned char *src1 = static_cast<const unsigned char*>(inputData1);
	const unsigned char *src2 = static_cast<const unsigned char*>(inputData2);
	unsigned char *dst = static_cast<unsigned char*>(outputData);

	while(dataLength--)
		*dst++ = *src1++ ^ *src2++;
}

inline void XorData(size_t dataLength,
					const void* inputData,
					void* outputData)
{
	const unsigned char *src = static_cast<const unsigned char*>(inputData);
	unsigned char *dst = static_cast<unsigned char*>(outputData);

	while(dataLength--)
		*dst++ ^= *src++;
}

inline void XorData(size_t dataLength,
					void* buffer,
					unsigned char val)
{
	unsigned char *dst = static_cast<unsigned char*>(buffer);

	while(dataLength--)
		*dst++ ^= val;
}

inline void XtsGfMult(size_t dataLength, void* buffer)
{
	unsigned char *dst = static_cast<unsigned char*>(buffer);
	unsigned char carryIn = 0;

	for (size_t j = 0; j < dataLength; j++)
	{
		unsigned char carryOut = (dst[j] >> 7) & 0x01;
		dst[j] = ((dst[j] << 1) + carryIn) & 0xFF;
		carryIn = carryOut;
	}
	if(carryIn)
	{
		dst[0] ^= 0x87;
	}
}
