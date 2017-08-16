#include "stdafx.h"
#include "TH07Unpacker.h"
using namespace std;


TH07Unpacker::TH07Unpacker(FILE* _f) :
	THUnpackerBase(_f)
{
	dirName = L"th07";
}

void TH07Unpacker::ReadHeader()
{
	DWORD header[3];
	fread(header, 1, 12, f);
	count = header[0];
	indexAddress = header[1];
	originalIndexSize = header[2];
}

void TH07Unpacker::ReadIndex()
{
	fseek(f, indexAddress, SEEK_SET);
	DWORD indexSize = fileSize - indexAddress;
	auto indexBuffer = make_unique<BYTE[]>(indexSize);
	fread(indexBuffer.get(), 1, indexSize, f);
	// Uncompress
	indexBuffer = THUncompress(indexBuffer.get(), indexSize, originalIndexSize);
	indexSize = originalIndexSize;

	// Format index
	FormatIndex(index, indexBuffer.get(), count, indexAddress);
}
