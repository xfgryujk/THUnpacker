#include "stdafx.h"
#include "common.h"
#include <direct.h>


// common function /////////////////////////////////////////////////////////////////

DWORD getFileSize(FILE* f)
{
	long offset = ftell(f);
	fseek(f, 0, SEEK_END);
	DWORD size = ftell(f);
	fseek(f, offset, SEEK_SET);
	return size;
}


#pragma warning(push)
#pragma warning(disable:4018)
BYTE* decrypt(BYTE* buffer, DWORD bufferSize, char a3, char a4, int a5, int a6)
{
	int v7; // [sp+8h] [bp-28h]@2
	int lengtha; // [sp+Ch] [bp-24h]@6
	int lengthb; // [sp+Ch] [bp-24h]@18
	int v12; // [sp+18h] [bp-18h]@6
	BYTE* v13; // [sp+1Ch] [bp-14h]@4
	int v14; // [sp+1Ch] [bp-14h]@11
	int v15; // [sp+1Ch] [bp-14h]@14
	BYTE* resultBuffer; // [sp+20h] [bp-10h]@4
	int v17; // [sp+24h] [bp-Ch]@11
	int v18; // [sp+24h] [bp-Ch]@14
	BYTE* v19; // [sp+28h] [bp-8h]@11
	BYTE* v20; // [sp+2Ch] [bp-4h]@4

	if (bufferSize % a5 >= a5 / 4)
		v7 = 0;
	else
		v7 = bufferSize % a5;
	v20 = buffer;
	resultBuffer = new BYTE[bufferSize];
	v13 = resultBuffer;
	if (resultBuffer == NULL)
		return buffer;

	v12 = (bufferSize & 1) + v7;
	lengtha = bufferSize - v12;
	while (lengtha > 0 && a6 > 0)
	{
		if (lengtha < a5)
			a5 = lengtha;
		v19 = v13;
		v14 = (int)((BYTE *)v13 + a5 - 1);
		v17 = (a5 + 1) / 2;
		while (v17 > 0)
		{
			*(BYTE *)v14 = a3 ^ *(BYTE *)v20;
			v14 -= 2;
			a3 += a4;
			--v17;
			v20++;
		}
		v15 = (int)((BYTE *)v19 + a5 - 2);
		v18 = a5 / 2;
		while (v18 > 0)
		{
			*(BYTE *)v15 = a3 ^ *(BYTE *)v20;
			v15 -= 2;
			a3 += a4;
			--v18;
			v20++;
		}
		lengtha -= a5;
		v13 = v19 + a5;
		a6 -= a5;
	}
	lengthb = v12 + lengtha;
	if (lengthb > 0)
		memcpy(v13, v20, lengthb);
	return resultBuffer;
}

BYTE* uncompress(BYTE* buffer, DWORD bufferSize, BYTE* resultBuffer, DWORD originalSize)
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

int THUnpacker::unpack()
{
	int result;
	readHeader();
	if ((result = checkCountAndSize()) != 0)
		return result;
	readIndex();
	if (!exportFiles(f, index, dirName))
	{
		puts(E_EXPORT);
		return EN_EXPORT;
	}

	puts("done.");
	return 0;
}

int THUnpacker::checkCountAndSize()
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


void THUnpacker::formatIndex(vector<Index>& index, const BYTE* indexBuffer, int fileCount, DWORD indexAddress)
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

BOOL THUnpacker::exportFiles(FILE* f, const vector<Index>& index, string dirName)
{
	_mkdir(dirName.c_str());
	for (const Index& i : index)
	{
		// read file
		BYTE* fileBuffer = new BYTE[i.length];
		fseek(f, i.address, SEEK_SET);
		fread(fileBuffer, 1, i.length, f);
		// uncompress
		BYTE* result = uncompress(fileBuffer, i.length, NULL, i.originalLength);
		delete fileBuffer;
		fileBuffer = result;
		DWORD size = i.originalLength;
		onExport(fileBuffer, size);

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
