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
		cout << "Usage: THUnpacker filename" << endl;
		return 1;
	}

	// Open file
	ifstream f(argv[1], ios_base::binary);
	if (!f.is_open())
	{
		cout << "Failed to open the file!" << endl;
		return 1;
	}

	// Create unpacker
	auto unpacker = THUnpackerBase::Create(f);

	// Unpack
	if (unpacker == nullptr)
	{
		cout << "Unknown file type!" << endl;
		return 1;
	}
	
	return unpacker->Unpack() ? 0 : 1;
}
