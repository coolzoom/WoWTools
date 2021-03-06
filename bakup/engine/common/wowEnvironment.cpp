#include "wowEnvironment.h"
#include "CFileSystem.h"
#include "CMemFile.h"
#include "CReadFile.h"
#include "stringext.h"
#include "function.h"
#include <regex>

#include "CascLib.h"
#include "CascCommon.h"

#define LISTFILE "listfile.csv"

//
wowEnvironment* g_WowEnvironment = nullptr;
bool createWowEnvironment(CFileSystem * fs, bool loadCascFile)
{
	g_WowEnvironment = new wowEnvironment(fs);
	if (!g_WowEnvironment->init())
	{
		delete g_WowEnvironment;
		g_WowEnvironment = nullptr;
		return false;
	}

	if (loadCascFile)
	{
		if (!g_WowEnvironment->loadCascListFiles())
			return false;
	}
	return true;
}

void destroyWowEnvironment()
{
	delete g_WowEnvironment;
	g_WowEnvironment = nullptr;
}

wowEnvironment::wowEnvironment(CFileSystem* fs)
	: FileSystem(fs), hStorage(nullptr), CascLocale(0)
{

}

wowEnvironment::~wowEnvironment()
{
	unloadRoot();
}

bool wowEnvironment::init()
{
	if (!initBuildInfo(Locale))
		return false;

	if (Locale == "enUS")
		CascLocale = CASC_LOCALE_ENUS;
	else if (Locale == "koKR")
		CascLocale = CASC_LOCALE_KOKR;
	else if (Locale == "frFR")
		CascLocale = CASC_LOCALE_FRFR;
	else if (Locale == "deDE")
		CascLocale = CASC_LOCALE_DEDE;
	else if (Locale == "zhCN")
		CascLocale = CASC_LOCALE_ZHCN;
	else if (Locale == "esES")
		CascLocale = CASC_LOCALE_ESES;
	else if (Locale == "ZhTW")
		CascLocale = CASC_LOCALE_ZHTW;
	else if (Locale == "enGB")
		CascLocale = CASC_LOCALE_ENGB;
	else if (Locale == "enCN")
		CascLocale = CASC_LOCALE_ENCN;
	else if (Locale == "enTW")
		CascLocale = CASC_LOCALE_ENTW;
	else if (Locale == "esMX")
		CascLocale = CASC_LOCALE_ESMX;
	else if (Locale == "ruRU")
		CascLocale = CASC_LOCALE_RURU;
	else if (Locale == "ptBR")
		CascLocale = CASC_LOCALE_PTBR;
	else if (Locale == "itIT")
		CascLocale = CASC_LOCALE_ITIT;
	else if (Locale == "ptPT")
		CascLocale = CASC_LOCALE_PTPT;
	else
		CascLocale = 0;

	loadRoot();

	return true;
}

bool wowEnvironment::loadCascListFiles()
{
	FileName2IdMap.clear();
	FileId2NameMap.clear();

	std::string dir = FileSystem->getDataDirectory();
	normalizeDirName(dir);
	dir += std_string_format("%d.%d", Version[0], Version[1]);
	normalizeDirName(dir);

	std::string listFile = dir + LISTFILE;

	CReadFile* file = FileSystem->createAndOpenFile(listFile.c_str(), false);
	if (!file)
		return false;

	char buffer[1024] = { 0 };
	std::vector<std::string> stringList;
	while (file->readLine(buffer, 1024))
	{
		std_string_split(buffer, ';', stringList);

		if (stringList.size() < 2)
			continue;

		std::string filename = stringList[1];
		normalizeFileName(filename);
		str_tolower(filename);

		int id = atoi(stringList[0].c_str());
		if (id >= 0)
		{
			FileName2IdMap[filename] = id;
			FileId2NameMap[id] = filename;
		}
	}

	delete file;

	DirIndexMap.clear();
	{
		for (const auto& itr : FileName2IdMap)
		{
			const char* szFile = itr.first.c_str();
			const char* p = strchr(szFile, '/');
			while (p)
			{
				std::string dir(szFile, (uint32_t)(p - szFile));
				if (DirIndexMap.find(dir) == DirIndexMap.end())
					DirIndexMap[dir] = szFile;														//add dir to index

				if (*(p + 1) == '0')
					break;
				p = strchr(p + 1, '/');
			}
		}
	}

	return true;
}

CMemFile * wowEnvironment::openFile(const char * filename) const
{
	HANDLE hFile;

	char realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (!CascOpenFile(hStorage, realfilename, CascLocale, 0, &hFile))
	{
		return nullptr;
	}

	DWORD dwHigh;
	uint32_t size = CascGetFileSize(hFile, &dwHigh);

	// HACK: in patch.mpq some files don't want to open and give 1 for filesize
	if (size <= 1 || size == 0xffffffff) {
		CascCloseFile(hFile);
		return nullptr;
	}

	uint8_t* buffer = new uint8_t[size];

	bool ret = CascReadFile(hFile, buffer, (DWORD)size, nullptr);
	if (!ret)
	{
		delete[] buffer;

		CascCloseFile(hFile);
		return nullptr;
	}

	CascCloseFile(hFile);

	return new CMemFile(buffer, size);
}

CMemFile* wowEnvironment::openFile(int fileid) const
{
	HANDLE hFile;

	if (!CascOpenFile(hStorage, CASC_FILE_DATA_ID(fileid), CascLocale, CASC_OPEN_BY_FILEID, &hFile))
	{
		return nullptr;
	}

	DWORD dwHigh;
	uint32_t size = CascGetFileSize(hFile, &dwHigh);

	// HACK: in patch.mpq some files don't want to open and give 1 for filesize
	if (size <= 1 || size == 0xffffffff) {
		CascCloseFile(hFile);
		return nullptr;
	}

	uint8_t* buffer = new uint8_t[size];

	bool ret = CascReadFile(hFile, buffer, (DWORD)size, nullptr);
	if (!ret)
	{
		delete[] buffer;

		CascCloseFile(hFile);
		return nullptr;
	}

	CascCloseFile(hFile);

	return new CMemFile(buffer, size);
}

bool wowEnvironment::exists(const char * filename) const
{
	if (strlen(filename) == 0)
		return false;

	HANDLE hFile;
	if (!CascOpenFile(hStorage, filename, CascLocale, 0, &hFile))
		return false;

	CascCloseFile(hFile);
	return true;
}

void wowEnvironment::iterateFiles(const char* ext, WOWFILECALLBACK callback) const
{
	for (const auto& itr : FileName2IdMap)
	{
		const char* filename = itr.first.c_str();
		if (hasFileExtensionA(filename, ext))
		{
			callback(filename);
		}
	}
}

void wowEnvironment::iterateFiles(const char* path, const char * ext, WOWFILECALLBACK callback) const
{
	std::string strBaseDir(path);
	str_tolower(strBaseDir);
	auto itr = DirIndexMap.find(strBaseDir);
	if (itr == DirIndexMap.end())
		return;

	//calc start
	std::string firstFile = itr->second;

	auto it = FileName2IdMap.find(firstFile);
	while (it != FileName2IdMap.end())
	{
		const char* filename = itr->first.c_str();
		if (hasFileExtensionA(filename, ext))
		{
			callback(filename);
		}

		if (strstr(filename, strBaseDir.c_str()) == nullptr)
			break;

		++itr;
	}
}

const char* wowEnvironment::getFileNameById(uint32_t id) const
{
	auto itr = FileId2NameMap.find(id);
	if (itr != FileId2NameMap.end())
		return itr->second.c_str();
	return nullptr;
}

void wowEnvironment::buildWmoFileList()
{
	WmoFileList.clear();
	iterateFiles("world", "wmo", [this](const char* filename)
	{
		if (strstr(filename, "internal"))
			return;

		uint32_t len = (uint32_t)strlen(filename);
		if (len > 8 &&
			filename[len - 8] == '_' &&
			isdigit((int)filename[len - 7]) &&
			isdigit((int)filename[len - 6]) &&
			isdigit((int)filename[len - 5]))		//_xxx.wmo
		{
			return;
		}

		WmoFileList.emplace_back(filename);
	});
}

bool wowEnvironment::initBuildInfo(std::string& activeLocale)
{
	std::string buildInfo = FileSystem->getWowBaseDirectory();
	normalizeDirName(buildInfo);
	buildInfo += ".build.info";

	CReadFile* file = FileSystem->createAndOpenFile(buildInfo.c_str(), false);
	if (!file)
		return false;

	activeLocale = "";
	char buffer[1024] = { 0 };

	//read header
	file->readLine(buffer, 1024);
	std::vector<std::string> headers;
	int activeIndex = 0;
	int versionIndex = 0;
	int tagIndex = 0;
	std_string_split(std::string(buffer), '|', headers);
	for (int i = 0; i < (int)headers.size(); ++i)
	{
		if (strstr(headers[i].c_str(), "Active"))
			activeIndex = i;
		else if (strstr(headers[i].c_str(), "Version"))
			versionIndex = i;
		else if (strstr(headers[i].c_str(), "Tags"))
			tagIndex = i;
	}

	//read values
	while (file->readLine(buffer, 1024))
	{
		std::vector<std::string> values;
		std_string_split(std::string(buffer), '|', values);

		ASSERT(values.size() == headers.size());

		//skip inactive
		if (values[activeIndex] == "0")
			continue;

		//version
		const std::regex pattern("^(\\d).(\\d).(\\d).(\\d+)");
		std::match_results<std::string::const_iterator> sm;
		std::regex_match(values[versionIndex], sm, pattern);
		if (sm.size() == 5)
		{
			Version[0] = atoi(sm[1].str().c_str());
			Version[1] = atoi(sm[2].str().c_str());
			Version[2] = atoi(sm[3].str().c_str());
			Version[3] = atoi(sm[4].str().c_str());
		}

		//locale
		std::string tag = values[tagIndex];
		std_string_split(tag, ':', values);
		for (const auto& str : values)
		{
			if (strstr(str.c_str(), "text?"))
			{
				std::vector<std::string> taglist;
				std_string_split(str, ' ', taglist);

				if (taglist.size() >= 2)
					activeLocale = taglist[taglist.size() - 2];
			}
		}
	}

	delete file;
	return true;
}

bool wowEnvironment::loadRoot()
{
	unloadRoot();
	const char* dataDir = FileSystem->getWowDataDirectory();
	if (!CascOpenStorage(dataDir, CascLocale, &hStorage))
	{
		hStorage = nullptr;
		return false;
	}
	return true;
}

void wowEnvironment::unloadRoot()
{
	if (hStorage != nullptr)
	{
		CascCloseStorage(hStorage);
		hStorage = nullptr;
	}
}
