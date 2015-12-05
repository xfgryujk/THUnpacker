#pragma once
#include "THUnpackerBase.h"


class TH06Unpacker : public THUnpackerBase
{
protected:
	class PBG3File
	{
	protected:
		FILE* f;
		BYTE buffer;
		BYTE mask;

	public:
		PBG3File(FILE* _f);

		void setPointer(DWORD address);
		BOOL read1Bit();
		DWORD readBits(DWORD bitsToRead);
		DWORD readNumber();
		void readString(char* strBuffer, DWORD size);
	};

public:
	TH06Unpacker(FILE* _f);

protected:
	PBG3File f;

	void readHeader();
	void readIndex();
};
