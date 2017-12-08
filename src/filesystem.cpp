#include "filesystem.h"

using gb::utils::filesystem;

#define _GB_FILESYSTEM_MAX_PATH 128

filesystem::filesystem()
{
    char path[_GB_FILESYSTEM_MAX_PATH] = {'\0'};
#ifdef _MSC_VER
    GetModuleFileName(NULL, path, _GB_FILESYSTEM_MAX_PATH);
    _workingDir = path;
    _workingDir = _workingDir.substr_at_l_lastof('\\', false);
#elif __GNUC__
    ::readlink("/proc/self/exe", path, _GB_FILESYSTEM_MAX_PATH);
    _workingDir = path;
    _workingDir = _workingDir.substr_at_l_lastof('/', false);
#endif
}
std::vector<gb::utils::string> filesystem::get_files_here(const char* path, const std::vector<const char*>* suffix)const
{
    assert(path != nullptr && suffix != nullptr);
    std::vector<gb::utils::string> files;
#ifdef _WIN32
    HANDLE hFind  = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;
  
    hFind = FindFirstFile(path, &ffd);
    if(hFind == INVALID_HANDLE_VALUE)
    {
	logger::Instance().warning((string)"filesystem::get_files_here:nothing here @" + path);
	return files;
    }
    do
    {
	string file(ffd.cFileName);
	if(suffix != nullptr)
	{
	    string suf(file.substr_at_r_lastof('.'));
	    for(std::vector<const char*>::const_iterator i = suffix->begin(); i != suffix->end(); i++)
	    {
		if(suf == (*i))
		{
		    string fileName(path);
		    fileName.replace("*", file);
		    files.push_back((const char*)fileName);
		    continue;
		}
	    }
	}
	else
	    files.push_back((const char*)file);
    }
    while(FindNextFile(hFind, &ffd) != 0);
#endif

    return files;
}

gb::utils::string filesystem::get_absolute_path(const char* szPath)const
{
    assert(szPath != nullptr);
#ifdef _MSC_VER
    if(szPath[1] == ':')
#elif __GNUC__
    if(szPath[0] == '/')
#endif
	return gb::utils::string(szPath);
    else//relative path
	return _workingDir + szPath;
}

// void filesystem::gb_LC_Reg(lua_State* L)
// {
// 	std::vector<gb_LC_Func> funcs;
// 	funcs.push_back(gb_LC_Func("GetWorkPath", &filesystem::gb_LC_EF_GetWorkPath));

// 	gbLuaCPP_PrvCns<filesystem>::Register(L, "filesystem", funcs);

// 	gb_LC_Singleton_Instance_Reg(filesystem);
// }
 
