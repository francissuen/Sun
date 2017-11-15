#include "filesystem.h"

using gb::utils::filesystem;
using gb::utils::logger;
using gb::utils::string;

void filesystem::get_files_here(const char* path, std::vector<std::string>& files, const std::vector<const char*>* suffix)
{
  #ifdef _WIN32
  HANDLE hFind  = INVALID_HANDLE_VALUE;
  WIN32_FIND_DATA ffd;
  
  hFind = FindFirstFile(path, &ffd);
  if(hFind == INVALID_HANDLE_VALUE)
    {
      logger::Instance.log((string)"filesystem::GetFileHere:nothing here" + path);
      return;
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
}

void filesystem::get_work_path(string& buffer)
{
char path[GB_FILESYSTEM_MAX_PATH] = {'\0'};

#ifdef __GNUC__
::readlink("/proc/self/exe", path, GB_FILESYSTEM_MAX_PATH);
buffer = path;
buffer = buffer.substr_at_l_lastof('/', false);
#elif _MSC_VER
GetModuleFileName(NULL, path, GB_FILESYSTEM_MAX_PATH);	
buffer = path;
buffer = buffer.substr_at_l_lastof('\\', false);
#endif
}

// void filesystem::gb_LC_Reg(lua_State* L)
// {
// 	std::vector<gb_LC_Func> funcs;
// 	funcs.push_back(gb_LC_Func("GetWorkPath", &filesystem::gb_LC_EF_GetWorkPath));

// 	gbLuaCPP_PrvCns<filesystem>::Register(L, "filesystem", funcs);

// 	gb_LC_Singleton_Instance_Reg(filesystem);
// }
