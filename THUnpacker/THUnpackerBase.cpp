#include "stdafx.h"
#include "THUnpackerBase.h"
using namespace std;
#include <direct.h> // for _mkdir()

#include "TH06Unpacker.h"
#include "TH07Unpacker.h"
#include "TH0809Unpacker.h"
#include "TH10To16Unpacker.h"


// Create instance base on magic number
std::shared_ptr<THUnpackerBase> THUnpackerBase::Create(ifstream& _f)
{
	DWORD magicNumber = 0;
	_f.read((char*)&magicNumber, 4);

	switch (magicNumber)
	{
	case 0x33474250: // "PBG3" for TH06
		return make_shared<TH06Unpacker>(_f);
	case 0x34474250: // "PBG4" for TH07
		return make_shared<TH07Unpacker>(_f);
	case 0x5A474250: // "PBGZ" for TH08/09
		return make_shared<TH0809Unpacker>(_f);
	case 0xB0B35513: // Encrypted "THA1" for TH10
		return make_shared<TH10Unpacker>(_f);
	case 0xB2B35A13: // Encrypted "THA1" for TH11
		return make_shared<TH11Unpacker>(_f);
	case 0xB1B35A13: // Encrypted "THA1" for TH12
		return make_shared<TH12Unpacker>(_f);
	case 0xB3B35A13: // Encrypted "THA1" for TH13/15
	{
		cout << "Please input [3|5|6] if this file belongs to TH13/15/16" << endl;
		while (true)
		{
			switch (getchar())
			{
			case '3':
				return make_shared<TH13Unpacker>(_f);
			case '5':
			case '6':
				return make_shared<TH1516Unpacker>(_f);
			}
		}
	}
	case 0xB4B35A13: // Encrypted "THA1" for TH14
		return make_shared<TH14Unpacker>(_f);
	}
	return nullptr;
}


// Common static functions //////////////////////////////////////////////////////////

DWORD THUnpackerBase::GetFileSize(ifstream& f)
{
	auto offset = f.tellg();
	f.seekg(0, ios_base::end);
	DWORD size = (DWORD)f.tellg();
	f.seekg(offset);
	return size;
}

wstring THUnpackerBase::StringToWstring(const string& src)
{
	size_t size;
	mbstowcs_s(&size, NULL, 0, src.c_str(), 0);
	wstring dst(size, L'\0');
	mbstowcs_s(&size, &dst.front(), size, src.c_str(), size);
	return std::move(dst);
}


#pragma warning(push)
#pragma warning(disable:4018)
void THUnpackerBase::THDecrypt(BYTE* buffer, DWORD bufferSize, char a3, char a4, int a5, int a6)
{
	int v7; // edi@1
	int v8; // ecx@1
	size_t v9; // ecx@3
	void *v10; // ecx@5
	void *v11; // esi@5
	int v12; // eax@6
	char v13; // cl@7
	int v14; // edx@7
	int v15; // eax@11
	int v16; // edi@11
	int i; // eax@11
	char v18; // dl@12
	int v19; // eax@13
	int j; // edi@13
	char v21; // dl@14
	int v25; // [sp+14h] [bp-Ch]@6
	BYTE *v26; // [sp+18h] [bp-8h]@5
	int a5a; // [sp+30h] [bp+10h]@3
	int a5b; // [sp+30h] [bp+10h]@11

	v7 = a5;
	v8 = 0;
	if ((signed int)bufferSize % a5 < a5 / 4)
		v8 = (signed int)bufferSize % a5;
	a5a = v8;
	v9 = a6;
	if (a6 >(signed int)bufferSize)
		v9 = bufferSize;
	v26 = buffer;
	v10 = malloc(v9);
	v11 = v10;
	if (v10)
	{
		v25 = bufferSize - (a5a + (bufferSize & 1));
		memcpy(v10, buffer, v9);
		v12 = v25;
		if (v25 > 0)
		{
			v13 = a3;
			v14 = a6;
			do
			{
				if (v14 <= 0)
					break;
				if (v12 < v7)
					v7 = v12;
				v15 = (int)&v26[v7];
				v26 = (BYTE *)v15;
				a5b = v7;
				v16 = v15 - 1;
				for (i = (a5b + 1) / 2; i > 0; v16 -= 2)
				{
					v18 = v13 ^ *(BYTE *)v11;
					--i;
					v13 += a4;
					v11 = (char *)v11 + 1;
					*(BYTE *)v16 = v18;
				}
				v19 = a5b / 2;
				for (j = (int)(v26 - 2); v19 > 0; j -= 2)
				{
					v21 = v13 ^ *(BYTE *)v11;
					--v19;
					v13 += a4;
					v11 = (char *)v11 + 1;
					*(BYTE *)j = v21;
				}
				v7 = a5b;
				v12 = v25 - a5b;
				v14 = a6 - a5b;
				v25 = v12;
				a6 -= a5b;
			} while (v12 > 0);
		}
		free(v10);
	}
}

unique_ptr<BYTE[]> THUnpackerBase::THUncompress(const BYTE* buffer, DWORD bufferSize, DWORD originalSize)
{
	static BYTE tmp[8500];
	char v5; // ST18_1@53
	int v7; // [sp+8h] [bp-34h]@4
	DWORD v8; // [sp+Ch] [bp-30h]@13
	DWORD v9; // [sp+Ch] [bp-30h]@26
	DWORD v10; // [sp+Ch] [bp-30h]@39
	int v11; // [sp+14h] [bp-28h]@1
	int v13; // [sp+20h] [bp-1Ch]@10
	int v14; // [sp+20h] [bp-1Ch]@13
	int v15; // [sp+20h] [bp-1Ch]@26
	int v16; // [sp+20h] [bp-1Ch]@39
	const BYTE* pBuffer; // [sp+24h] [bp-18h]@4
	BYTE v18; // [sp+2Bh] [bp-11h]@1
	int i; // [sp+2Ch] [bp-10h]@51
	short v20; // [sp+30h] [bp-Ch]@38
	BYTE* pResultBuffer; // [sp+34h] [bp-8h]@4
	int v22; // [sp+38h] [bp-4h]@1
	int v23; // [sp+38h] [bp-4h]@57

	v18 = -128;
	v22 = 0;
	v11 = 0;
	auto resultBuffer = make_unique<BYTE[]>(originalSize);
	
	pBuffer = buffer;
	pResultBuffer = resultBuffer.get();
	v7 = 1;
	while (true)
	{
		while (true)
		{
			if (v18 == 128)
			{
				v22 = *(BYTE *)pBuffer;
				if (pBuffer - buffer < bufferSize)
					++pBuffer;
				else
					v22 = 0;
				v11 += v22;
			}
			v13 = (BYTE)(v18 & v22);
			v18 >>= 1;
			if (!v18)
				v18 = -128;
			if (!v13)
				break;
			v8 = 128;
			v14 = 0;
			while (v8)
			{
				if (v18 == 128)
				{
					v22 = *(BYTE *)pBuffer;
					if (pBuffer - buffer < bufferSize)
						++pBuffer;
					else
						v22 = 0;
					v11 += v22;
				}
				if (v18 & (BYTE)v22)
					v14 |= v8;
				v8 >>= 1;
				v18 >>= 1;
				if (!v18)
					v18 = -128;
			}
			*(BYTE *)pResultBuffer++ = v14;
			tmp[v7] = v14;
			v7 = ((short)v7 + 1) & 0x1FFF;
		}
		v9 = 4096;
		v15 = 0;
		while (v9)
		{
			if (v18 == 128)
			{
				v22 = *(BYTE *)pBuffer;
				if (pBuffer - buffer < bufferSize)
					++pBuffer;
				else
					v22 = 0;
				v11 += v22;
			}
			if (v18 & (BYTE)v22)
				v15 |= v9;
			v9 >>= 1;
			v18 >>= 1;
			if (!v18)
				v18 = -128;
		}
		v20 = v15;
		if (!v15)
			break;
		v10 = 8;
		v16 = 0;
		while (v10)
		{
			if (v18 == 128)
			{
				v22 = *(BYTE *)pBuffer;
				if (pBuffer - buffer < bufferSize)
					++pBuffer;
				else
					v22 = 0;
				v11 += v22;
			}
			if (v18 & (BYTE)v22)
				v16 |= v10;
			v10 >>= 1;
			v18 >>= 1;
			if (!v18)
				v18 = -128;
		}
		for (i = 0; i <= v16 + 2; ++i)
		{
			v5 = tmp[((short)i + v20) & 0x1FFF];
			*(BYTE *)pResultBuffer++ = v5;
			tmp[v7] = v5;
			v7 = ((short)v7 + 1) & 0x1FFF;
		}
	}
	while (v18 != 128)
	{
		if (v18 == 128)
		{
			v23 = *(BYTE *)pBuffer;
			if (pBuffer - buffer < bufferSize)
				++pBuffer;
			else
				v23 = 0;
			v11 += v23;
		}
		v18 >>= 1;
		if (!v18)
			v18 = -128;
	}
	return resultBuffer;
}
#pragma warning(pop)


// Unpack //////////////////////////////////////////////////////////////////////////

THUnpackerBase::THUnpackerBase(ifstream& _f) :
	f(_f),
	fileSize(GetFileSize(_f))
{
}


bool THUnpackerBase::Unpack()
{
	ReadHeader();
	if (!CheckCountAndSize())
		return false;
	ReadIndex();
	cout << "Exporting..." << endl;
	if (!ExportFiles(f, index, dirName))
	{
		cout << "Failed to export files!" << endl;
		return false;
	}

	cout << "Done." << endl;
	return true;
}

bool THUnpackerBase::CheckCountAndSize()
{
	if (count <= 0)
	{
		cout << "Invalid file count!" << endl;
		return false;
	}
	if (fileSize <= indexAddress)
	{
		cout << "Invalid file size!" << endl;
		return false;
	}
	return true;
}

void THUnpackerBase::FormatIndex(vector<Index>& index, const BYTE* indexBuffer, int fileCount, DWORD indexAddress)
{
	index.resize(fileCount);
	for (int i = 0; i < fileCount; i++)
	{
		index[i].name = (char*)indexBuffer;
		indexBuffer += index[i].name.size() + 1;
		index[i].address = ((DWORD*)indexBuffer)[0];
		index[i].originalLength = ((DWORD*)indexBuffer)[1];
		indexBuffer += 12;

		printf("%30s  %10d  %10d\n", index[i].name.c_str(), index[i].address, index[i].originalLength);
	}
	int i;
	for (i = 0; i < fileCount - 1; i++)
		index[i].length = index[i + 1].address - index[i].address;
	index[i].length = indexAddress - index[i].address;
}

bool THUnpackerBase::ExportFiles(ifstream& f, const vector<Index>& index, wstring dirName)
{
	_wmkdir(dirName.c_str());
	for (const auto& i : index)
	{
		// Read file
		DWORD size = i.length;
		auto fileBuffer = make_unique<BYTE[]>(size);
		f.seekg(i.address);
		f.read((char*)fileBuffer.get(), size);

		// First decrypt
		bool uncompress = OnUncompress(i, fileBuffer, size);

		// Uncompress
		if (uncompress)
		{
			fileBuffer = THUncompress(fileBuffer.get(), size, i.originalLength);
			size = i.originalLength;
		}

		// Second decrypt
		OnExport(i, fileBuffer, size);

		// Write file
		wstring outputFilePath = dirName + L"\\" + StringToWstring(i.name);
		ofstream f2(outputFilePath, ios_base::binary);
		if (!f2.is_open())
		{
			wcout << L"Failed to open file: " << outputFilePath << endl;
			return false;
		}
		f2.write((char*)fileBuffer.get(), size);
	}
	return true;
}
