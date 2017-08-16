#pragma once
#include "THUnpackerBase.h"


class PBG3File final
{
protected:
	FILE* f = NULL;
	BYTE buffer = 0;
	BYTE mask = 0;

public:
	PBG3File(FILE* _f);

	void SetPointer(DWORD address);
	BOOL Read1Bit();
	DWORD ReadBits(DWORD bitsToRead);
	DWORD ReadNumber();
	void ReadString(char* strBuffer, DWORD size);
};

class TH06Unpacker final : public THUnpackerBase
{
public:
	TH06Unpacker(FILE* _f);

protected:
	PBG3File f;

	virtual void ReadHeader() override;
	virtual void ReadIndex() override;
};
