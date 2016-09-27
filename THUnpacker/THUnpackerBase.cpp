#include "stdafx.h"
#include "THUnpackerBase.h"
#include <direct.h> // for _mkdir()

#include "TH06Unpacker.h"
#include "TH07Unpacker.h"
#include "TH0809Unpacker.h"
#include "TH10Unpacker.h"
#include "TH14Unpacker.h"


// create instance base on magic number
THUnpackerBase* THUnpackerBase::create(FILE* _f)
{
	DWORD magicNumber = 0;
	fread(&magicNumber, 4, 1, _f);

	THUnpackerBase* instance = NULL;
	switch (magicNumber)
	{
	case 0x33474250: // "PBG3" for TH06
		instance = new TH06Unpacker(_f);
		break;
	case 0x34474250: // "PBG4" for TH07
		instance = new TH07Unpacker(_f);
		break;
	case 0x5A474250: // "PBGZ" for TH08/09
		instance = new TH0809Unpacker(_f);
		break;
	case 0xB0B35513: // encrypted "THA1" for TH10
		instance = new TH10Unpacker(_f);
		break;
	case 0xB4B35A13: // encrypted "THA1" for TH14
		instance = new TH14Unpacker(_f);
		break;
	}

	return instance;
}

// common static functions //////////////////////////////////////////////////////////

DWORD THUnpackerBase::getFileSize(FILE* f)
{
	long offset = ftell(f);
	fseek(f, 0, SEEK_END);
	DWORD size = ftell(f);
	fseek(f, offset, SEEK_SET);
	return size;
}

#pragma warning(push)
#pragma warning(disable:4018)
BYTE* THUnpackerBase::thDecrypt(BYTE* buffer, DWORD bufferSize, char a3, char a4, int a5, int a6)
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
	return buffer;
}

BYTE* THUnpackerBase::thUncompress(BYTE* buffer, DWORD bufferSize, BYTE* resultBuffer, DWORD originalSize)
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
	BYTE* pBuffer; // [sp+24h] [bp-18h]@4
	BYTE v18; // [sp+2Bh] [bp-11h]@1
	int i; // [sp+2Ch] [bp-10h]@51
	short v20; // [sp+30h] [bp-Ch]@38
	BYTE* pResultBuffer; // [sp+34h] [bp-8h]@4
	int v22; // [sp+38h] [bp-4h]@1
	int v23; // [sp+38h] [bp-4h]@57

	v18 = -128;
	v22 = 0;
	v11 = 0;
	if (resultBuffer == NULL && (resultBuffer = new BYTE[originalSize]) == NULL)
		return NULL;
	
	pBuffer = buffer;
	pResultBuffer = resultBuffer;
	v7 = 1;
	while (TRUE)
	{
		while (TRUE)
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

// unpack //////////////////////////////////////////////////////////////////////////

THUnpackerBase::THUnpackerBase(FILE* _f)
{
	f = _f;
	fileSize = getFileSize(_f);
	count = indexAddress = originalIndexSize = 0;
	dirName = "th";
}


int THUnpackerBase::unpack()
{
	int result;
	readHeader();
	if ((result = checkCountAndSize()) != 0)
		return result;
	readIndex();
	puts("Exporting...");
	if (!exportFiles(f, index, dirName))
	{
		puts(E_EXPORT);
		return EN_EXPORT;
	}

	puts("Done.");
	return 0;
}

int THUnpackerBase::checkCountAndSize()
{
	if (count <= 0)
	{
		puts(E_FILE_COUNT);
		return EN_FILE_COUNT;
	}
	if (fileSize <= indexAddress)
	{
		puts(E_FILE_SIZE);
		return EN_FILE_SIZE;
	}
	return 0;
}


void THUnpackerBase::formatIndex(vector<Index>& index, const BYTE* indexBuffer, int fileCount, DWORD indexAddress)
{
	index.resize(fileCount);
	int i;
	for (i = 0; i < fileCount; i++)
	{
		index[i].name = (char*)indexBuffer;
		indexBuffer += strlen((char*)indexBuffer) + 1;
		index[i].address = ((DWORD*)indexBuffer)[0];
		index[i].originalLength = ((DWORD*)indexBuffer)[1];
		indexBuffer += 12;

		printf("%30s  %10d  %10d\n", index[i].name.c_str(), index[i].address, index[i].originalLength);
	}
	for (i = 0; i < fileCount - 1; i++)
		index[i].length = index[i + 1].address - index[i].address;
	index[i].length = indexAddress - index[i].address;
}

BOOL THUnpackerBase::exportFiles(FILE* f, const vector<Index>& index, string dirName)
{
	_mkdir(dirName.c_str());
	for (const Index& i : index)
	{
		// read file
		DWORD size = i.length;
		BYTE* fileBuffer = new BYTE[size];
		fseek(f, i.address, SEEK_SET);
		fread(fileBuffer, 1, size, f);

		// decrypt 1
		bool uncomp = onUncompress(i, fileBuffer, size);

		// uncompress
		if (uncomp)
		{
			BYTE* result = thUncompress(fileBuffer, size, NULL, i.originalLength);
			delete fileBuffer;
			fileBuffer = result;
			size = i.originalLength;
		}

		// decrypt 2
		onExport(i, fileBuffer, size);

		// write file
		FILE* f2;
		fopen_s(&f2, (dirName + "\\" + i.name).c_str(), "wb");
		if (f2 == NULL)
		{
			delete fileBuffer;
			printf("Failed to open file: %s\n", (dirName + "\\" + i.name).c_str());
			return FALSE;
		}
		fwrite(fileBuffer, 1, size, f2);
		fclose(f2);
		delete fileBuffer;
	}
	return TRUE;
}
