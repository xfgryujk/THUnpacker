#include "stdafx.h"
#include "common.h"


class TH06Unpacker : public THUnpacker
{
public:
	TH06Unpacker(FILE* _f) : THUnpacker(_f), f(_f) {}

protected:
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
	};

	PBG3File f;
	
protected:
	void readHeader()
	{
		count = f.readNumber();
		indexAddress = f.readNumber();
	}

	void readIndex()
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

	int exportFiles()
	{
		if (!::exportFiles(THUnpacker::f, index, "th06"))
		{
			puts(E_EXPORT);
			return EN_EXPORT;
		}
		return 0;
	}
};

class TH07Unpacker : public THUnpacker
{
public:
	TH07Unpacker(FILE* _f) : THUnpacker(_f) {}

protected:
	void readHeader()
	{
		DWORD header[3];
		fread(header, 1, 12, f);
		count = header[0];
		indexAddress = header[1];
		originalIndexSize = header[2];
	}

	void readIndex()
	{
		fseek(f, indexAddress, SEEK_SET);
		DWORD indexSize = fileSize - indexAddress;
		BYTE* indexBuffer = new BYTE[indexSize];
		fread(indexBuffer, 1, indexSize, f);
		// uncompress
		BYTE* result = uncompress(indexBuffer, indexSize, NULL, originalIndexSize);
		delete indexBuffer;
		indexBuffer = result;
		indexSize = originalIndexSize;

		// format index
		formatIndex(index, indexBuffer, count, indexAddress);
		delete indexBuffer;
	}

	int exportFiles()
	{
		if (!::exportFiles(f, index, "th07"))
		{
			puts(E_EXPORT);
			return EN_EXPORT;
		}
		return 0;
	}
};

class TH08Unpacker : public THUnpacker
{
public:
	TH08Unpacker(FILE* _f) : THUnpacker(_f) {}

protected:
	void readHeader()
	{
		DWORD header[3];
		fread(header, 1, 12, f);
		// decrypt
		DWORD* result = (DWORD*)decrypt((BYTE*)header, 12, 27, 55, 12, 1024);
		count = result[0] - 123456;
		indexAddress = result[1] - 345678;
		originalIndexSize = result[2] - 567891;
		delete result;
	}

	void readIndex()
	{
		fseek(f, indexAddress, SEEK_SET);
		DWORD indexSize = fileSize - indexAddress;
		BYTE* indexBuffer = new BYTE[indexSize];
		fread(indexBuffer, 1, indexSize, f);
		// decrypt
		BYTE* result = decrypt(indexBuffer, indexSize, 62, -101, 128, 1024);
		delete indexBuffer;
		indexBuffer = result;
		// uncompress
		result = uncompress(indexBuffer, indexSize, NULL, originalIndexSize);
		delete indexBuffer;
		indexBuffer = result;
		indexSize = originalIndexSize;

		// format index
		formatIndex(index, indexBuffer, count, indexAddress);
		delete indexBuffer;
	}

	int exportFiles()
	{
		if (!::exportFiles(f, index, "th08"))
		{
			puts(E_EXPORT);
			return EN_EXPORT;
		}
		return 0;
	}
};

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
	THUnpacker* unpacker = NULL;
	switch (magicNumber)
	{
	case 0x33474250: // "PBG3" for TH06
		unpacker = new TH06Unpacker(f);
		break;
	case 0x34474250: // "PBG4" for TH07
		unpacker = new TH07Unpacker(f);
		break;
	case 0x5A474250: // "PBGZ" for TH08
		unpacker = new TH08Unpacker(f);
		break;
	}

	// unpack
	int result;
	if (unpacker != NULL)
	{
		result = unpacker->unpack();
		delete unpacker;
	}
	else
	{
		puts(E_UNKNOWN_TYPE);
		result = EN_UNKNOWN_TYPE;
	}

	fclose(f);
	return result;
}
