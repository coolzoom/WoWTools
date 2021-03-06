#include "wowDatabase.h"
#include "wowEnvironment.h"
#include "CFileSystem.h"
#include "stringext.h"
#include "function.h"
#include "CMemFile.h"

#include "wowDbFile.h"

#include "pugixml.hpp"

#define DATABASEFILE "database.xml"
#define DBFILESDIR "DBFilesClient/"

const char* g_szDB_Ext[] = {".db2", ".dbc"};

//
wowDatabase* g_WowDatabase = nullptr;
bool createWowDatabase(const wowEnvironment * wowEnv)
{
	g_WowDatabase = new wowDatabase(wowEnv);
	if (!g_WowDatabase->init())
		return false;

	return true;
}

void destroyWowDatabase()
{
	delete g_WowDatabase;
	g_WowDatabase = nullptr;
}

wowDatabase::wowDatabase(const wowEnvironment* env)
	: Environment(env)
{
}


bool wowDatabase::init()
{
	if (!initFromXml())
		return false;

	if (!loadAllTables())
		return false;

	return true;
}

CMemFile* wowDatabase::loadDBMemFile(const char* name) const
{
	std::string baseName = DBFILESDIR;
	normalizeDirName(baseName);
	baseName += name;

	CMemFile* file = nullptr;
	for (const auto& ext : g_szDB_Ext)
	{
		std::string filename = baseName + ext;
		file = Environment->openFile(filename.c_str());
		if (file)
			break;
	}
	
	return file;
}

bool wowDatabase::loadAllTables()
{
	if (!m_CharBaseSectionTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_CharComponentTextureLayoutsTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_CharComponentTextureSectionsTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_CharHairGeoSetsTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_CharSectionsTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_CharacterFacialHairStylesTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ChrClassesTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ChrCustomizationTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ChrRacesTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ComponentModelFileDataTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ComponentTextureFileDataTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_CreatureDisplayInfoTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_CreatureDisplayInfoExtraTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_CreatureModelDataTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_CreatureTypeTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_HelmetGeosetDataTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ItemTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ItemAppearanceTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ItemClassTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ItemDisplayInfoTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ItemDisplayInfoMaterialResTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ItemModifiedAppearanceTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ItemSetTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ItemSparseTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ItemSubClassTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ModelFileDataTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_MountTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_MountXDisplayTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_NpcModelItemSlotDisplayInfoTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_ParticleColorTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	if (!m_TextureFileDataTable.loadData(this))
	{
		ASSERT(false);
		return false;
	}

	return true;
}

const CTableStruct* wowDatabase::getDBStruct(const char* name) const
{
	auto itr = DbStructureMap.find(name);
	if (itr != DbStructureMap.end())
		return &itr->second;
	return nullptr;
}

const DBFile* wowDatabase::loadDBFile(const char* name) const
{
	CMemFile* memFile = loadDBMemFile(name);
	if (!memFile)
		return nullptr;

	const DBFile* file = DBFile::readDBFile(memFile);
	return file;
}

bool wowDatabase::initFromXml()
{
	const CFileSystem* fs = Environment->getFileSystem();
	const auto& version = Environment->getVersion();

	std::string dir = fs->getDataDirectory();
	normalizeDirName(dir);
	dir += std_string_format("%d.%d", version[0], version[1]);
	normalizeDirName(dir);

	std::string xmlFile = dir + DATABASEFILE;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(xmlFile.c_str());
	if (result.status != pugi::status_ok)
		return false;

	auto root = doc.first_child();
	for (auto dbtable : root.children("table"))
	{
		CTableStruct tblStruct;

		tblStruct.name = dbtable.attribute("name").as_string();

		int fieldId = 0;
		for (auto dbfield : dbtable.children("field"))
		{
			CFieldStruct fieldStruct;
			fieldStruct.id = fieldId;

			auto attr_name = dbfield.attribute("name");
			auto attr_type = dbfield.attribute("type");
			auto attr_key = dbfield.attribute("primary");
			auto attr_arraySize = dbfield.attribute("arraySize");
			auto attr_index = dbfield.attribute("createIndex");
			auto attr_pos = dbfield.attribute("pos");
			auto attr_commonData = dbfield.attribute("commonData");
			auto attr_relationshipData = dbfield.attribute("relationshipData");

			if (!attr_name.empty() && !attr_type.empty())
			{
				fieldStruct.name = attr_name.as_string();
				fieldStruct.type = attr_type.as_string();

				fieldStruct.isKey = !attr_key.empty();
				fieldStruct.needIndex = !attr_index.empty();
				fieldStruct.arraySize = attr_arraySize.empty() ? 1 : attr_arraySize.as_uint();

				fieldStruct.pos = attr_pos.as_int();
				fieldStruct.isCommonData = attr_commonData.as_bool();
				fieldStruct.isRelationshipData = attr_relationshipData.as_bool();

				//add
				tblStruct.fields.emplace_back(fieldStruct);
			}

			++fieldId;
		}

		DbStructureMap[tblStruct.name] = tblStruct;
	}
	return true;
}

