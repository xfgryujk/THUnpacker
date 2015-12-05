#include "stdafx.h"
#include "THUnpackerBase.h"


int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		puts("Usage: THUnpacker filename");
		return 1;
	}

	// open file
	FILE* f;
	fopen_s(&f, argv[1], "rb");
	if (f == NULL)
	{
		puts(E_OPEN_FILE);
		return EN_OPEN_FILE;
	}

	// create unpacker
	THUnpackerBase* unpacker = THUnpackerBase::create(f);

	// unpack
	int result;
	if (unpacker == NULL)
	{
		puts(E_UNKNOWN_TYPE);
		result = EN_UNKNOWN_TYPE;
	}
	else
	{
		result = unpacker->unpack();
		delete unpacker;
	}

	// close file
	fclose(f);
	return result;
}
