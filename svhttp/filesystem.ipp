#ifndef svhttp_filesystem_ipp__
#define svhttp_filesystem_ipp__

#include "filesystem.hpp"
#include "bencode.hpp"

#if defined(_WIN32)
#	include <windows.h>
#	include <ShlObj.h>
#endif

#if defined(_WIN32) 
#	include <direct.h>  
#	include <io.h>  
#else  
#	include <stdarg.h>  
#	include <sys/stat.h>  
#	include <stdlib.h>
#endif 

#if defined(_WIN32)
#	define ACCESS _access
#	define MKDIR(a) _mkdir((a))
#else
#	define ACCESS access
#	define MKDIR(a) mkdir((a),0755)
#endif

namespace svhttp
{
	namespace filesystem
	{
		std::string getcwd_t()
		{
			char fullpath[260];
#if defined(_WIN32)
			_getcwd(fullpath, sizeof(fullpath));
#else
			getcwd(fullpath, sizeof(fullpath));
#endif
			return std::string(fullpath);
		}
		
		bool read_file(const std::string& file_path, std::string& file_ctn)
		{
			std::ostringstream oss;
			std::ifstream ifile_stream(file_path, std::ios::binary);
			if (ifile_stream)
			{
				oss << ifile_stream.rdbuf();
				ifile_stream.close();
				file_ctn = oss.str();
				return true;
			}
			return false;
		}
		
		bool save_file(const std::string& file_path, const std::string& file_ctn)
		{
			std::ofstream ofile_stream(file_path, std::ios::binary);
			if (ofile_stream)
			{
				ofile_stream << file_ctn;
				ofile_stream.close();
				return true;
			}
			return false;
		}

		bool access_file(const std::string& file_path, int mode)
		{
			if (ACCESS(file_path.c_str(), mode) == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	
		// 多层目录结构的创建
		int create_direct(const std::string& direct_path)
		{
			std::string path_str = direct_path;
			path_str = svhttp::replace_all(path_str, "\\", "/");
			int nlen = direct_path.length();
			// 为兼容linux目录以'/'起，此处索引从1开始计算。
			for (int idx = 1; idx < nlen; ++idx)
			{
				if (path_str.at(idx) == '/')
				{
					std::string path_t = path_str.substr(0, idx);
					if (ACCESS(path_t.c_str(), 0) != 0)
					{
						if (MKDIR(path_t.c_str()) != 0)
						{
							return -1;
						}
					}
				}
			}
			if (ACCESS(path_str.c_str(), 0) != 0)
			{
				if (MKDIR(path_str.c_str()) != 0)
				{
					return -1;
				}
			}

			return 0;
		}
		//remove direct
		int remove_direct( const std::string& direct_path )
		{
#if defined(_WIN32)
			return _rmdir(direct_path.c_str());
#else
			return _rmdir(direct_path.c_str());
#endif
		}
		// remove file
		int remove_file( const std::string& file_path )
		{
			return remove(file_path.c_str());
		}

#ifdef _UNICODE
#define _tstring std::wstring
#else
#define _tstring std::string
#endif // end _UNICODE

		// 获取特殊目录路径
		std::string get_special_path(int csidl/* = CSIDL_APPDATA*/)
		{
			std::string special_path_str;
#if defined(_WIN32)
			//LPCTSTR sAppPath;
			_tstring sAppPath;
			LPITEMIDLIST pidl;
			LPMALLOC pShellMalloc;
			if (SUCCEEDED(SHGetMalloc(&pShellMalloc)))
			{
				TCHAR *path = new TCHAR[MAX_PATH + 1];
				if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, csidl, &pidl)))
				{
					if (SHGetPathFromIDList(pidl, path))
					{
						sAppPath = path;
					}
					pShellMalloc->Free(pidl);
				}
				delete[] path;
				pShellMalloc->Release();
			}

#if defined(_UNICODE)
			special_path_str = unicode_to_ansi(sAppPath);
#else
			special_path_str = sAppPath;
#endif //end _UNICODE

#else		// end _win32 -> linux
			special_path_str = std::string(getenv("HOME"));
#endif	//end _win32

			return special_path_str;
		}

	}// end filesystem
}// end svhttp

#endif // filesystem_ipp__
