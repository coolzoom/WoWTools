#pragma once

#include "base.h"
#include <vector>
#include <set>
#include <string>

struct SShaderFile 
{
	std::vector<char>  Buffer;
};

//从文件中读取shader内容，包括include 注释, 宏处理等
class CShaderUtil
{
public:
	static bool loadFile_OpenGL(const char* absFileName, const std::set<std::string>& shaderMacro, SShaderFile& result);

	static bool writeFile(const char* buffer, const char* absFileName);

	static std::string getShaderMacroString(const std::set<std::string>& shaderMacro);
	static std::set<std::string> getShaderMacroSet(const char* macroString);

private:
	static bool processDirectiveInclude(const char* pAfterInclude, const char* szDirInclude, std::string& strInclude);
};
