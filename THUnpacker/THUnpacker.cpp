#include "stdafx.h"
#include "common.h"


int unpackTH06(FILE* _f)
{
	class PBG3File
	{
	protected:
		FILE* f;
		BYTE buffer;
		BYTE mask;

	public:
		PBG3File(FILE* _f)
		{
			f = _f;
			mask = 0;
		}

		void setPointer(DWORD address)
		{
			fseek(f, address, SEEK_SET);
			mask = 0;
		}

		BOOL read1Bit()
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

		DWORD readBits(DWORD bitsToRead)
		{
			DWORD result = 0;
			for (DWORD resultMask = 1 << (bitsToRead - 1); resultMask != 0; resultMask >>= 1)
				if (read1Bit())
					result |= resultMask;
			return result;
		}

		DWORD readNumber()
		{
			DWORD size = readBits(2) + 1;
			return readBits(size * 8);
		}

		void readString(char* strBuffer, DWORD size)
		{
			DWORD i;
			for (i = 0; i < size; i++)
				if ((strBuffer[i] = readBits(8)) == '\0')
					break;
			if (i >= size)
				strBuffer[size - 1] = '\0';
		}
	} f(_f);
	
	// read header
	DWORD count = f.readNumber();
	DWORD indexAddreass = f.readNumber();

	// check
	if (count <= 0)
	{
		puts(E_FILE_COUNT);
		return EN_FILE_COUNT;
	}

	DWORD fileSize = getFileSize(_f);
	if (fileSize <= indexAddreass)
	{
		puts(E_FILE_SIZE);
		return EN_FILE_SIZE;
	}

	// read index
	f.setPointer(indexAddreass);
	vector<Index> index(count);
	for (Index& i : index)
	{
		// we don't use them
		f.readNumber();
		f.readNumber();
		f.readNumber();

		i.address =	f.readNumber();
		i.originalLength = f.readNumber();
		i.name.resize(256);
		f.readString(const_cast<char*>(i.name.c_str()), 256);
		i.name.resize(strlen(i.name.c_str()));

		printf("%30s  %10d  %10d\n", i.name.c_str(), i.address, i.originalLength);
	}
	DWORD i;
	for (i = 0; i < count - 1; i++)
		index[i].length = index[i + 1].address - index[i].address;
	index[i].length = indexAddreass - index[i].address;

	// export files
	if (!exportFiles(_f, &index, "th06"))
	{
		puts(E_EXPORT);
		return EN_EXPORT;
	}

	puts("done.");
	return 0;
}

int unpackTH07(FILE* f)
{
	// read header
	DWORD header[3]; // file count, index address, original index size
	fread(header, 1, 12, f);

	// check
	if (header[0] <= 0)
	{
		puts(E_FILE_COUNT);
		return EN_FILE_COUNT;
	}

	DWORD fileSize = getFileSize(f);
	if (fileSize <= header[1])
	{
		puts(E_FILE_SIZE);
		return EN_FILE_SIZE;
	}

	// read index
	fseek(f, header[1], SEEK_SET);
	DWORD indexSize = fileSize - header[1];
	BYTE* indexBuffer = new BYTE[indexSize];
	fread(indexBuffer, 1, indexSize, f);
	// uncompress
	BYTE* result = uncompress(indexBuffer, indexSize, NULL, header[2]);
	delete indexBuffer;
	indexBuffer = result;
	indexSize = header[2];

	// format index
	vector<Index>* index = formatIndex(indexBuffer, header[0], header[1]);
	delete indexBuffer;

	// export files
	if (!exportFiles(f, index, "th07"))
	{
		delete index;
		puts(E_EXPORT);
		return EN_EXPORT;
	}
	delete index;

	puts("done.");
	return 0;
}

int unpackTH08(FILE* f)
{
	// read header
	DWORD header[3]; // file count, index address, original index size
	fread(header, 1, 12, f);
	// decrypt
	BYTE* result = decrypt((BYTE*)header, 12, 27, 55, 12, 1024);
	memcpy(header, result, 12);
	delete result;
	header[0] -= 123456;
	header[1] -= 345678;
	header[2] -= 567891;

	// check
	if (header[0] <= 0)
	{
		puts(E_FILE_COUNT);
		return EN_FILE_COUNT;
	}

	DWORD fileSize = getFileSize(f);
	if (fileSize <= header[1])
	{
		puts(E_FILE_SIZE);
		return EN_FILE_SIZE;
	}

	// read index
	fseek(f, header[1], SEEK_SET);
	DWORD indexSize = fileSize - header[1];
	BYTE* indexBuffer = new BYTE[indexSize];
	fread(indexBuffer, 1, indexSize, f);
	// decrypt
	result = decrypt(indexBuffer, indexSize, 62, -101, 128, 1024);
	delete indexBuffer;
	indexBuffer = result;
	// uncompress
	result = uncompress(indexBuffer, indexSize, NULL, header[2]);
	delete indexBuffer;
	indexBuffer = result;
	indexSize = header[2];

	// format index
	vector<Index>* index = formatIndex(indexBuffer, header[0], header[1]);
	delete indexBuffer;

	// export files
	if (!exportFiles(f, index, "th08"))
	{
		delete index;
		puts(E_EXPORT);
		return EN_EXPORT;
	}
	delete index;

	puts("done.");
	return 0;
}

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

	// check magic number
	DWORD magicNumber = 0;
	fread(&magicNumber, 4, 1, f);
	int result = 0;
	switch (magicNumber)
	{
	case 0x33474250: // "PBG3" for TH06
		result = unpackTH06(f);
		break;
	case 0x34474250: // "PBG4" for TH07
		result = unpackTH07(f);
		break;
	case 0x5A474250: // "PBGZ" for TH08
		result = unpackTH08(f);
		break;
	default:
		puts(E_UNKNOWN_TYPE);
		result = EN_UNKNOWN_TYPE;
		break;
	}

	fclose(f);
	return result;
}
