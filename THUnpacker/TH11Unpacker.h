#pragma once
#include "THUnpackerBase.h"


class TH11Unpacker : public THUnpackerBase
{
public:
	TH11Unpacker(FILE* _f);

protected:
	void readHeader();
	void readIndex();
	void formatIndex(vector<Index>& index, const BYTE* indexBuffer, int fileCount, DWORD indexAddress);
	bool onUncompress(const Index& index, BYTE*& buffer, DWORD& size);
};
