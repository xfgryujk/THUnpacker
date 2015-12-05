#pragma once
#include "THUnpackerBase.h"


class TH07Unpacker : public THUnpackerBase
{
public:
	TH07Unpacker(FILE* _f);

protected:
	void readHeader();
	void readIndex();
};
