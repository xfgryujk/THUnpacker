#include "stdafx.h"
#include "TH06Unpacker.h"


// PBG3File ///////////////////////////////////////////////////////////////////////////////

TH06Unpacker::PBG3File::PBG3File(FILE* _f)
{
	f = _f;
	mask = 0;
}

void TH06Unpacker::PBG3File::setPointer(DWORD address)
{
	fseek(f, address, SEEK_SET);
	mask = 0;
}

BOOL TH06Unpacker::PBG3File::read1Bit()
{
	if (mask == 0)
	{
		fread(&buffer, 1, 1, f);
		mask = 1 << 7;
	}
	BOOL result = (buffer & mask) != 0;
	mask >>= 1;
	return result;
}

DWORD TH06Unpacker::PBG3File::readBits(DWORD bitsToRead)
{
	DWORD result = 0;
	for (DWORD resultMask = 1 << (bitsToRead - 1); resultMask != 0; resultMask >>= 1)
	if (read1Bit())
		result |= resultMask;
	return result;
}

DWORD TH06Unpacker::PBG3File::readNumber()
{
	DWORD size = readBits(2) + 1;
	return readBits(size * 8);
}

void TH06Unpacker::PBG3File::readString(char* strBuffer, DWORD size)
{
	DWORD i;
	for (i = 0; i < size; i++)
	if ((strBuffer[i] = readBits(8)) == '\0')
		break;
	if (i >= size)
		strBuffer[size - 1] = '\0';
}

// TH06Unpacker ///////////////////////////////////////////////////////////////////////////

TH06Unpacker::TH06Unpacker(FILE* _f) 
: THUnpackerBase(_f), f(_f)
{
	dirName = "th06";
}

void TH06Unpacker::readHeader()
{
	count = f.readNumber();
	indexAddress = f.readNumber();
}

void TH06Unpacker::readIndex()
{
	f.setPointer(indexAddress);
	index.resize(count);
	for (Index& i : index)
	{
		// we don't use them
		f.readNumber();
		f.readNumber();
		f.readNumber();

		i.address = f.readNumber();
		i.originalLength = f.readNumber();
		i.name.resize(256);
		f.readString(const_cast<char*>(i.name.c_str()), 256);
		i.name.resize(strlen(i.name.c_str()));

		printf("%30s  %10d  %10d\n", i.name.c_str(), i.address, i.originalLength);
	}
	int i;
	for (i = 0; i < count - 1; i++)
		index[i].length = index[i + 1].address - index[i].address;
	index[i].length = indexAddress - index[i].address;
}
