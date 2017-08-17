#include "stdafx.h"
#include "TH07Unpacker.h"
using namespace std;


TH07Unpacker::TH07Unpacker(ifstream& _f) :
	THUnpackerBase(_f)
{
	dirName = L"th07";
}

void TH07Unpacker::ReadHeader()
{
	DWORD header[3];
	f.read((char*)header, 12);
	count = header[0];
	indexAddress = header[1];
	originalIndexSize = header[2];
}

void TH07Unpacker::ReadIndex()
{
	f.seekg(indexAddress);
	DWORD indexSize = fileSize - indexAddress;
	auto indexBuffer = make_unique<BYTE[]>(indexSize);
	f.read((char*)indexBuffer.get(), indexSize);
	// Uncompress
	indexBuffer = THUncompress(indexBuffer.get(), indexSize, originalIndexSize);
	indexSize = originalIndexSize;

	// Format index
	FormatIndex(index, indexBuffer.get(), count, indexAddress);
}
