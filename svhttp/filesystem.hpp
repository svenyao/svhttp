#ifndef svhttp_filesystem_hpp__
#define svhttp_filesystem_hpp__

#ifndef SVHTTP_DISABLE_HEADER_ONLY
#	define SVHTTP_DECL inline
# else
#	define SVHTTP_DECL
#endif // defined(SVHTTP_DISABLE_HEADER_ONLY)

#include <string>

namespace svhttp
{
	namespace filesystem
	{
		/**
		 *	获取当前工作目录(绝对)路径,返回完整路径字符串
		 */
		SVHTTP_DECL std::string getcwd_t();
		/**
		 *	读文件
		 */
		SVHTTP_DECL bool read_file(const std::string& file_path, std::string& file_ctn);
		/**
		 *	写文件
		 */
		SVHTTP_DECL bool save_file(const std::string& file_path, const std::string& file_ctn);
		/**	 确定文件或文件夹的访问权限
		 * 	mode属性值：
		 *	06     检查读写权限
		 *	04     检查读权限
		 *	02     检查写权限
		 *	01     检查执行权限
		 *	00     检查文件的存在性
		 */
		SVHTTP_DECL bool access_file(const std::string& file_path, int mode = 0);
		/**
		 *	多层目录结构的创建
		 */
		SVHTTP_DECL int create_direct(const std::string& direct_path);
		/**
		 *	删除文件/文件夹
		 */
		SVHTTP_DECL int remove_direct(const std::string& direct_path);
		SVHTTP_DECL int remove_file(const std::string& file_path);
		
		/**
		 *	获取特殊目录路径
		 */
		SVHTTP_DECL std::string get_special_path(int csidl = 0x001a);

	}// end namespace filesystem
} //@ end namespace svhttp.

#ifndef SVHTTP_DISABLE_HEADER_ONLY
#include "filesystem.ipp"
#endif

#endif