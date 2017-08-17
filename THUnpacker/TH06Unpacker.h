#pragma once
#include "THUnpackerBase.h"


class PBG3File final
{
protected:
	std::ifstream& f;
	BYTE buffer = 0;
	BYTE mask = 0;

public:
	PBG3File(std::ifstream& _f);

	void SetPointer(DWORD address);
	bool Read1Bit();
	DWORD ReadBits(DWORD bitsToRead);
	DWORD ReadNumber();
	void ReadString(char* strBuffer, DWORD size);
};

class TH06Unpacker final : public THUnpackerBase
{
public:
	TH06Unpacker(std::ifstream& _f);

protected:
	PBG3File f;

	virtual void ReadHeader() override;
	virtual void ReadIndex() override;
};
