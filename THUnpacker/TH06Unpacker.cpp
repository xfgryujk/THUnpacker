#include "stdafx.h"
#include "TH06Unpacker.h"
using namespace std;


// PBG3File ///////////////////////////////////////////////////////////////////////////////

PBG3File::PBG3File(ifstream& _f) :
	f(_f)
{
}

void PBG3File::SetPointer(DWORD address)
{
	f.seekg(address);
	mask = 0;
}

bool PBG3File::Read1Bit()
{
	if (mask == 0)
	{
		f.read((char*)&buffer, 1);
		mask = 1 << 7;
	}
	bool result = (buffer & mask) != 0;
	mask >>= 1;
	return result;
}

DWORD PBG3File::ReadBits(DWORD bitsToRead)
{
	DWORD result = 0;
	for (DWORD resultMask = 1 << (bitsToRead - 1); resultMask != 0; resultMask >>= 1)
	{
		if (Read1Bit())
			result |= resultMask;
	}
	return result;
}

DWORD PBG3File::ReadNumber()
{
	DWORD size = ReadBits(2) + 1;
	return ReadBits(size * 8);
}

void PBG3File::ReadString(char* strBuffer, DWORD size)
{
	DWORD i;
	for (i = 0; i < size; i++)
	{
		if ((strBuffer[i] = (char)ReadBits(8)) == '\0')
			break;
	}
	if (i >= size)
		strBuffer[size - 1] = '\0';
}


// TH06Unpacker ///////////////////////////////////////////////////////////////////////////

TH06Unpacker::TH06Unpacker(ifstream& _f) : 
	THUnpackerBase(_f),
	f(_f)
{
	dirName = L"th06";
}

void TH06Unpacker::ReadHeader()
{
	count = f.ReadNumber();
	indexAddress = f.ReadNumber();
}

void TH06Unpacker::ReadIndex()
{
	f.SetPointer(indexAddress);
	index.resize(count);
	for (auto& i : index)
	{
		// Useless
		f.ReadNumber();
		f.ReadNumber();
		f.ReadNumber();

		i.address = f.ReadNumber();
		i.originalLength = f.ReadNumber();
		i.name.resize(256);
		f.ReadString(&i.name.front(), 256);
		i.name.resize(strlen(i.name.c_str()));

		printf("%30s  %10d  %10d\n", i.name.c_str(), i.address, i.originalLength);
	}
	int i;
	for (i = 0; i < count - 1; i++)
		index[i].length = index[i + 1].address - index[i].address;
	index[i].length = indexAddress - index[i].address;
}
