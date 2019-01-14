#include "filesystem.h"

using fs::Sun::filesystem;

#define _FS_SUN_FILESYSTEM_MAX_PATH 128

filesystem::filesystem()
{
    char path[_FS_SUN_FILESYSTEM_MAX_PATH] = {'\0'};
#ifdef _MSC_VER
    GetModuleFileName(NULL, path, _FS_SUN_FILESYSTEM_MAX_PATH);
    _workingDir = path;
    _workingDir = _workingDir.substr_at_l_lastof('\\', false);
#elif __GNUC__
    ssize_t ret = ::readlink("/proc/self/exe", path, _FS_SUN_FILESYSTEM_MAX_PATH);
    if(ret == -1)
	throw string("readlink error");
    _workingDir = path;
    _workingDir = _workingDir.substr_at_l_lastof('/', false);
#endif
}
std::vector<fs::Sun::string> filesystem::get_files_here(const char* path, const std::vector<const char*>* suffix)const
{
    assert(path != nullptr && suffix != nullptr);
    std::vector<fs::Sun::string> files;
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

fs::Sun::string filesystem::get_absolute_path(const char* szPath)const
{
    assert(szPath != nullptr);
#ifdef _MSC_VER
    if(szPath[1] == ':')
#elif __GNUC__
    if(szPath[0] == '/')
#endif
	return fs::Sun::string(szPath);
    else//relative path
	return _workingDir + szPath;
}

// void filesystem::fs_LC_Reg(lua_State* L)
// {
// 	std::vector<fs_LC_Func> funcs;
// 	funcs.push_back(fs_LC_Func("GetWorkPath", &filesystem::fs_LC_EF_GetWorkPath));

// 	fsLuaCPP_PrvCns<filesystem>::Register(L, "filesystem", funcs);

// 	fs_LC_Singleton_Instance_Reg(filesystem);
// }
 
