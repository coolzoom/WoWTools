#pragma once

#include "base.h"
#include <vector>
#include <set>
#include <string>

struct SShaderFile 
{
	std::vector<char>  Buffer;
};

//���ļ��ж�ȡshader���ݣ�����include ע��, �괦���
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
