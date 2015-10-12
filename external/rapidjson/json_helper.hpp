#ifndef JSON_HELPER_H__
#define JSON_HELPER_H__

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include <sstream>
#include <fstream>

namespace rapidjson
{
	/** rapidjson 使用范例
	 *  使用范例：
		std::string json_str = "{\"data\":{\"oms\":{\"mid\":\"001\",\"code\":\"600446\",\"volume\":100,\"price\":10.01}},\"tgw\":{\"appid\":20}}";
		std::cout << json_str << std::endl;
		std::cout << "-------------------------------------" << std::endl;
		rapidjson::Document doc;
		rapidjson::JsonParse(doc, json_str);

		if (rapidjson::CheckMember(doc, "data", rapidjson::MemberType::kObject) &&
		rapidjson::CheckMember(doc["data"], "oms", rapidjson::MemberType::kObject))
		{
			std::cout << "object data: " << rapidjson::GetMemberValueString(doc["data"], "oms") << std::endl;

			rapidjson::Value& val = doc["data"]["oms"];

			if (rapidjson::CheckMember(val, "code", rapidjson::MemberType::kString))
			{
				std::cout << "code: " << rapidjson::GetMemberValueString(val, "code") << std::endl;
			}
			std::cout << "mid: "<< rapidjson::GetMemberValueString(val, "mid") << std::endl;
			std::cout << "volume: " << rapidjson::GetMemberValueInt(val, "volume") << std::endl;
			std::cout << "price: " << rapidjson::GetMemberValueDouble(val, "price") << std::endl;

			std::cout << "object tgw: " << rapidjson::GetMemberValueString(doc, "tgw") << std::endl;
		}
	 */

	enum MemberType
	{
		kObject, kString, kInt, kDouble, kArray, kBool, kNumber
	};
	/**
	 *	成员类型检查
	 *  if(CheckMember(doc, member, MemberType::kObject))
	 */  
	template<class T>
	inline bool CheckMember(T& doc, const std::string& member, const MemberType type = kObject)
	{
		rapidjson::Value::ConstMemberIterator iter = doc.FindMember(member.c_str());
		if (iter != doc.MemberEnd())
		{
			switch (type)
			{
			case MemberType::kObject:
				if (iter->value.IsObject())
				{
					return true;
				}
				break;
			case MemberType::kString:
				if (iter->value.IsString())
				{
					return true;
				}
				break;
			case MemberType::kInt:
				if (iter->value.IsInt())
				{
					return true;
				}
				break;
			case MemberType::kDouble:
				if (iter->value.IsDouble())
				{
					return true;
				}
				break;
			case MemberType::kArray:
				if (iter->value.IsArray())
				{
					return true;
				}
				break;
			case MemberType::kBool:
				if (iter->value.IsBool())
				{
					return true;
				}
				break;
			case MemberType::kNumber:
				if (iter->value.IsNumber())
				{
					return true;
				}
			default:
				break;
			}
		}
		return false;
	}
	// 获取json成员的值。以std::base_string数据类型返回
	inline std::string GetMemberValueString(rapidjson::Value& doc, const std::string& member)
	{
		if (CheckMember(doc, member, MemberType::kString))
		{
			return doc[member.c_str()].GetString();
		}
		else if (CheckMember(doc, member, MemberType::kInt))
		{
			std::ostringstream oss;
			oss << doc[member.c_str()].GetInt();
			return oss.str();
		}
		else if (CheckMember(doc, member, MemberType::kDouble))
		{
			std::ostringstream oss;
			oss << doc[member.c_str()].GetDouble();
			return oss.str();
		}
		else if (CheckMember(doc, member, MemberType::kBool))
		{
			return (doc[member.c_str()].GetBool()) ? "true" : "false";
		}
		else if (CheckMember(doc, member, MemberType::kObject) || CheckMember(doc, member, MemberType::kArray))
		{
			rapidjson::Value& valx = doc[member.c_str()];
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			valx.Accept(writer);
			return buffer.GetString();
		}

		return "";
	}
	// 获取json(int类型)成员的值。以int数据类型返回
	inline int GetMemberValueInt(rapidjson::Value& doc, const std::string& member)
	{
		if (CheckMember(doc, member, MemberType::kInt))
		{
			return doc[member.c_str()].GetInt();
		}
		else if (CheckMember(doc, member, MemberType::kString))
		{
			std::string str = doc[member.c_str()].GetString();
			return atoi(str.c_str());
		}
		else if (CheckMember(doc, member, MemberType::kBool))
		{
			return (doc[member.c_str()].GetBool()) ? 1 : 0;
		}

		return 0;
	}
	// 获取json(double类型)成员的值。以double数据类型返回
	inline double GetMemberValueDouble(rapidjson::Value& doc, const std::string& member)
	{
		if (CheckMember(doc, member, kDouble))
		{
			return doc[member.c_str()].GetDouble();
		}
		else if (CheckMember(doc, member, kInt))
		{
			return doc[member.c_str()].GetDouble();
		}
		else if (CheckMember(doc, member, MemberType::kString))
		{
			std::string str = doc[member.c_str()].GetString();
			return atof(str.c_str());
		}
		else if (CheckMember(doc, member, MemberType::kBool))
		{
			return (doc[member.c_str()].GetBool()) ? 1 : 0;
		}

		return 0.0;
	}

	/**
	 *	读取文件
	 */
	inline std::string ReadFile(const std::string& file_path)
	{
		std::ostringstream oss;
		std::ifstream ifile_stream(file_path, std::ios::binary);
		if (ifile_stream)
		{
			oss << ifile_stream.rdbuf();
		}
		return (oss.str());
	}
	/**
	 *	写入文件
	 */
	bool WriteFile(const std::string& content, const std::string &file_path)
	{
		// 文件路径不允许为空
		if (file_path.empty())
		{
			return false;
		}
		std::fstream file_;
		file_.open(file_path.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
		file_.write(content.c_str(), content.size());

		return true;
	}
	/**
	 *	JsonParse / ParseFromString
	 *	将json字符串解析为json对象
	 */
	inline bool ParseFromString(rapidjson::Document& doc, const std::string& json_str)
	{
		doc.SetObject();
		doc.Parse<0>(json_str.c_str());
		if (doc.HasParseError())
		{
			return false;
		}
		return true;
	};
	/**
	 *	json文件对象解析
	 */
	inline int ParseFromFile(rapidjson::Document& doc, const std::string& file_path)
	{
		if (!rapidjson::ParseFromString(doc, ReadFile(file_path)))
		{
			return -1;
		}
		return 0;
	}


	/**
	 *	将rapidjson中的object对象解析为string字符串
	 */
	template<class T>
	inline std::string ParseToString(T& doc)
	{
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		doc.Accept(writer);
		return buffer.GetString();
	}
	/**
	 *	将rapidjson中的object对象解析写入文件
	 */
	template<class T>
	inline bool ParseToFile(T& doc, const std::string& file_path)
	{
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		doc.Accept(writer);
		
		return WriteFile(buffer.GetString(), file_path);
	}

}	// namespace rapidjson.


#endif // JSON_HELPER_H__
