#pragma once
#include "THUnpackerBase.h"


class TH0809Unpacker : public THUnpackerBase
{
public:
	TH0809Unpacker(FILE* _f);

protected:
	void readHeader();
	void readIndex();
	void onExport(BYTE*& buffer, DWORD& size);
};
