#include "stdafx.h"
#include "TH07Unpacker.h"


TH07Unpacker::TH07Unpacker(FILE* _f) 
: THUnpackerBase(_f)
{
	dirName = "th07";
}

void TH07Unpacker::readHeader()
{
	DWORD header[3];
	fread(header, 1, 12, f);
	count = header[0];
	indexAddress = header[1];
	originalIndexSize = header[2];
}

void TH07Unpacker::readIndex()
{
	fseek(f, indexAddress, SEEK_SET);
	DWORD indexSize = fileSize - indexAddress;
	BYTE* indexBuffer = new BYTE[indexSize];
	fread(indexBuffer, 1, indexSize, f);
	// uncompress
	BYTE* result = thUncompress(indexBuffer, indexSize, NULL, originalIndexSize);
	delete indexBuffer;
	indexBuffer = result;
	indexSize = originalIndexSize;

	// format index
	formatIndex(index, indexBuffer, count, indexAddress);
	delete indexBuffer;
}
