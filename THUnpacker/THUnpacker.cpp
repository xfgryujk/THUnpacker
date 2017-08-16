#include "stdafx.h"
#include <locale.h>
#include <locale>
using namespace std;
#include "THUnpackerBase.h"


int wmain(int argc, WCHAR* argv[])
{
	setlocale(LC_ALL, "");
	locale::global(locale(""));

	if (argc != 2)
	{
		puts("Usage: THUnpacker filename");
		return 1;
	}

	// Open file
	FILE* f;
	_tfopen_s(&f, argv[1], _T("rb"));
	if (f == NULL)
	{
		puts("Failed to open the file!");
		return 1;
	}

	// Create unpacker
	auto unpacker = THUnpackerBase::Create(f);

	// Unpack
	int result;
	if (unpacker == nullptr)
	{
		puts("Unknown file type!");
		result = 1;
	}
	else
		result = unpacker->Unpack();

	// Close file
	fclose(f);
	return result;
}
