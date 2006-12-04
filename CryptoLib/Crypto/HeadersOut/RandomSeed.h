// Copyright (C) 2004 nobody

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_RANDOMSEED_41B1D21B0048_INCLUDED
#define _INC_RANDOMSEED_41B1D21B0048_INCLUDED

#define		SEED_SIZE		(256*8)

//##ModelId=41B1D21B0048
class RandomSeed 
{
public:
	//##ModelId=41B1D2880384
	void Init(void * RandomData, int DataLength);

	//##ModelId=41B1D297010E
	__forceinline void Clear()
	{
		volatile char	*ptr=(volatile char*)&SeedData;
		int				i;

		i=SEED_SIZE;
		while(i)
		{
			*ptr++=0;
			i--;
		}
		Seq=0;
	}

	//##ModelId=41B1D29F007A
	void GetHash(void * HashData);

protected:
	//##ModelId=41B1D2600336
	unsigned char SeedData[SEED_SIZE];
	unsigned long Seq;
};

#endif /* _INC_RANDOMSEED_41B1D21B0048_INCLUDED */
