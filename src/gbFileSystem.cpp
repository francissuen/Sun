#include "gbFileSystem.h"

void gbFileSystem::GetFilesHere(const char* path, std::vector<std::string>& files, const std::vector<const char*>* suffix)
{
  #ifdef _WIN32
  HANDLE hFind  = INVALID_HANDLE_VALUE;
  WIN32_FIND_DATA ffd;
  
  hFind = FindFirstFile(path, &ffd);
  if(hFind == INVALID_HANDLE_VALUE)
    {
      gbLog::Instance.Log((gbString)"gbFileSystem::GetFileHere:nothing here" + path);
      return;
    }
  do
    {
      gbString file(ffd.cFileName);
      if(suffix != nullptr)
	{
	  gbString suf(file.SubStrAtLastOf_R('.'));
	  for(std::vector<const char*>::const_iterator i = suffix->begin(); i != suffix->end(); i++)
	    {
	      if(suf == (*i))
		{
		  gbString fileName(path);
		  fileName.Replace("*", file);
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

void gbFileSystem::GetWorkPath(gbString& buffer)
{
char path[gb_FILESYSTEM_MAX_PATH] = {'\0'};

#ifdef __GNUC__
::readlink("/proc/self/exe", path, gb_FILESYSTEM_MAX_PATH);
buffer = path;
buffer = buffer.SubStrAtLastOf_L('/', false);
#elif _MSC_VER
GetModuleFileName(NULL, path, gb_FILESYSTEM_MAX_PATH);	
buffer = path;
buffer = buffer.SubStrAtLastOf_L('\\', false);
#endif
}

// void gbFileSystem::gb_LC_Reg(lua_State* L)
// {
// 	std::vector<gb_LC_Func> funcs;
// 	funcs.push_back(gb_LC_Func("GetWorkPath", &gbFileSystem::gb_LC_EF_GetWorkPath));

// 	gbLuaCPP_PrvCns<gbFileSystem>::Register(L, "gbFileSystem", funcs);

// 	gb_LC_Singleton_Instance_Reg(gbFileSystem);
// }
