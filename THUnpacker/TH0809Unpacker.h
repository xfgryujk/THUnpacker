#pragma once
#include "THUnpackerBase.h"


class TH0809Unpacker : public THUnpackerBase
{
public:
	TH0809Unpacker(FILE* _f);

protected:
	virtual void ReadHeader() override;
	virtual void ReadIndex() override;
	virtual void OnExport(const Index& index, std::unique_ptr<BYTE[]>& buffer, DWORD& size) override;
};
