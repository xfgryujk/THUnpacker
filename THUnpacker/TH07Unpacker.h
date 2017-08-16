#pragma once
#include "THUnpackerBase.h"


class TH07Unpacker final : public THUnpackerBase
{
public:
	TH07Unpacker(FILE* _f);

protected:
	virtual void ReadHeader() override;
	virtual void ReadIndex() override;
};
