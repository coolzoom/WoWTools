#include "CWriteFile.h"
#include "stringext.h"
#include "function.h"

CWriteFile::CWriteFile(const char * filename, bool binary, bool append)
	: IsBinary(binary)
{
	FileName = filename;
	normalizeFileName(FileName);
	openFile(IsBinary, append);
}

CWriteFile::~CWriteFile()
{
	if (File)
		fclose(File);
}

uint32_t CWriteFile::writeBuffer(const void* buffer, uint32_t sizeToWrite)
{
	if (!isOpen() || buffer == nullptr)
		return 0;

	return (uint32_t)fwrite(buffer, 1, sizeToWrite, File);
}

uint32_t CWriteFile::writeText(const char* buffer)
{
	if (!isOpen() || buffer == nullptr)
		return 0;

	ASSERT(!IsBinary);

	int32_t w1 = fputs(buffer, File);
	if (w1 == EOF)
		return 0;

	return (uint32_t)w1;
}

uint32_t CWriteFile::write(const char* format, ...)
{
	if (!isOpen() || format == nullptr)
		return 0;

	ASSERT(!IsBinary);

	va_list va;
	va_start(va, format);
	int32_t w = vfprintf(File, format, va);
	va_end(va);

	if (w == EOF)
		return 0;

	return (uint32_t)w;
}

uint32_t CWriteFile::writeLine(const char* format, ...)
{
	if (!isOpen() || format == nullptr)
		return 0;

	ASSERT(!IsBinary);

	va_list va;
	va_start(va, format);
	int32_t w = vfprintf(File, format, va);
	va_end(va);

	if (w == EOF)
		return 0;

	fprintf(File, "\n");

	return (uint32_t)w;
}

bool CWriteFile::flush()
{
	if (isOpen())
	{
		return 0 == fflush(File);
	}
	return false;
}

bool CWriteFile::seek(int32_t finalPos, bool relativeMovement /*= false*/)
{
	if (!isOpen())
		return false;

	return fseek(File, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
}

uint32_t CWriteFile::getPos() const
{
	return (uint32_t)ftell(File);
}

void CWriteFile::openFile(bool binary, bool append)
{
	const char* mode;
	if (append)
	{
		mode = binary ? "ab" : "at";
	}
	else
	{
		mode = binary ? "wb" : "wt";
	}

	File = Q_fopen(FileName.c_str(), mode);

	if (File)
	{
		fseek(File, 0, SEEK_END);
		long size = ftell(File);
		FileSize = size > 0 ? (uint32_t)size : 0;
		fseek(File, 0, SEEK_SET);
	}
	else
	{
		FileSize = 0;
	}
}