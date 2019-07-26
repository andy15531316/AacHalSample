#ifndef  __GS_CONFIGURATION_FILE_H__
#define  __GS_CONFIGURATION_FILE_H__

#if defined( _MSC_VER )
	#if !defined( _CRT_SECURE_NO_WARNINGS )
		#define _CRT_SECURE_NO_WARNINGS
	#endif
	#if !defined( _CRT_NON_CONFORMING_SWPRINTFS )
		#define _CRT_NON_CONFORMING_SWPRINTFS
	#endif
#endif

#include "tinyxml.h"
#include <windows.h>
#include <string>

namespace gs{

std::string Unicode_Utf8(const wchar_t* wszString);
std::wstring Utf8_Unicode(const char* szU8);

class GsElementNode
{
public:

	class Proxy;
	friend class GsConfigFile;
	friend class GsElementNode::Proxy;

	GsElementNode(void);
	//GsElementNode(TiXmlElement* element);


	Proxy* operator ->(void);
	const Proxy* operator ->(void) const;
	Proxy& operator *();

	bool operator == (void* address);
	bool operator == (const GsElementNode& rhs);
	bool operator != (void* address);
	bool operator != (const GsElementNode& rhs);

	class Proxy
	{
		friend GsElementNode;
	public:

		Proxy(void);
		explicit Proxy(TiXmlElement* element);

		bool AddChild(GsElementNode& node);
		bool AddChild(const std::wstring& xmlData);

		GsElementNode AddElement(const char* type);
		GsElementNode AddElement(const wchar_t* type);
		GsElementNode AddElement(const char* type, const char* key);
		GsElementNode AddElement(const wchar_t* type, const wchar_t* key);
		bool RemoveElement(const char* type, const char* key);
		void Clear(void);

		bool GetInt(int& value);
		bool GetInt(const char* type, int& value);
		bool GetInt(const wchar_t* type, int& value);
		bool GetInt(const char* type, const char* key, int& value);
		bool GetInt(const wchar_t* type, const wchar_t* key, int& value);
		bool GetInt(const char* type, int key, int& value);
		bool GetInt(const wchar_t* type, int key, int& value);

		bool GetInt(int& value, int defaultValue);
		bool GetInt(const char* type, int& value, int defaultValue);
		bool GetInt(const wchar_t* type, int& value, int defaultValue);
		bool GetInt(const char* type, const char* key, int& value, int defaultValue);
		bool GetInt(const wchar_t* type, const wchar_t* key, int& value, int defaultValue);
		bool GetInt(const char* type, int key, int& value, int defaultValue);
		bool GetInt(const wchar_t* type, int key, int& value, int defaultValue);

		bool SetInt(int value);
		bool SetInt(const char* type, int value);
		bool SetInt(const wchar_t* type, int value);
		bool SetInt(const char* type, const char* key, int value);
		bool SetInt(const char* type, int key, int value);

		bool GetType(std::string& type);
		bool GetType(std::wstring& type);
		bool GetType(std::string& type, std::string& key);
		bool GetType(std::wstring& type, std::wstring& key);
		bool GetKey(std::string& key);
		bool GetKey(std::wstring& key);
		bool GetValue(const char *subkey,std::string& value);
		bool GetValue(const wchar_t *subkey,std::wstring& value);
		bool SetKey(std::string key);

		bool GetStr(std::string& str);
		bool GetStr(std::wstring& str);
		bool GetStr(const char* type, std::string& str);
		bool GetStr(const wchar_t* type, std::wstring& str);
		bool GetStr(const char* type, const char* key, std::string& str);
		bool GetStr(const char* type, int key, std::string& str);

		bool GetStr(std::string& str, const char* defaultValue);
		bool GetStr(std::wstring& str, const wchar_t* default_value);
		bool GetStr(const char* type, std::string& str, const char* defaultValue);
		bool GetStr(const wchar_t* type, std::wstring& str, const wchar_t* default_value);
		bool GetStr(const char* type, const char* key, std::string& str, const char* defaultValue);
		bool GetStr(const char* type, int key, std::string& str, const char* defaultValue);

		bool SetStr(const char* str);
		bool SetStr(const wchar_t* str);
		bool SetStr(const char* type, const char* str);
		bool SetStr(const wchar_t* type, const wchar_t* str);
		bool SetStr(const char* type, const char* key, const char* str);
		bool SetStr(const wchar_t* type, const wchar_t* key, const wchar_t* str);
		bool SetStr(const char* type, int key, const char* str);

		GsElementNode AddElementWithoutKey(const char* type);
		GsElementNode AddElementWithoutKey(const wchar_t* type);

		GsElementNode AddElementValue(const char* type, const char* str);
		GsElementNode AddElementValue(const wchar_t* type, const wchar_t* str);

		GsElementNode FirstChild(void);
		GsElementNode FirstChild(const char* type);
		GsElementNode FirstChild(const wchar_t* type);

		GsElementNode NextSibling(void);
		GsElementNode NextSibling(const char* type);
		GsElementNode NextSibling(const wchar_t* type);

		GsElementNode Parent(void);
		GsElementNode SearchChild(const char* type, const char* key);
		GsElementNode SearchChild(const wchar_t* type, const wchar_t* key);
		GsElementNode SearchChild(const char* type, const char* key, const char *value);
		GsElementNode SearchChild(const char* type, const char* key1, const char *value1, const char* key2, const char *value2);
		GsElementNode SearchChild(const wchar_t* type, const wchar_t* key1, const wchar_t *value1, const wchar_t* key2, const wchar_t *value2);

		std::wstring ToWstring(void);

	private:

		TiXmlElement* FindElement(TiXmlElement* parent, const char* type, const char* key, bool bCreateIfNotExist = false);
		TiXmlElement* FindLeafElement(TiXmlElement* parent, const char* type, const char* key, bool bCreateIfNotExist = false);
		TiXmlElement* FindNoneLeafElement(TiXmlElement* parent, const char* type, const char* key, bool bCreateIfNotExist = false);

		bool CheckNoValueNode(TiXmlElement* element);
		bool CheckNoElementNode(TiXmlElement* element);

	private:
		TiXmlElement* m_element;

	};

protected:

	explicit GsElementNode(TiXmlElement* element);

private:
	Proxy m_proxy;
public:
	static std::wstring Str2Wcs(const char* str);
	static std::string Wcs2Str(const wchar_t* wcs);
};


//------------------------------------------------------------------------------

class GsConfigFile
{
public:

	GsConfigFile(void);

	~GsConfigFile();

	bool OpenFromString(const wchar_t*);
	//weili 3-18 add
	bool SetProfileFromString(const wchar_t*);

	bool Open(const char* filename, bool bCreateIfNotExist);
	bool Open(const wchar_t* filename, bool bCreateIfNotExist);
	bool Create(void);
	bool Create(const char* filename);
	bool Create(const wchar_t* filename);
	bool IsOpen(void) const;

	bool Save(void);
	bool SaveAs(const wchar_t* filename);
	void Close(void);

	void ToString(std::wstring& msg);

	GsElementNode& Root(void);

private:
	static std::string GsConfigFile::Wcs2Str(const wchar_t* wcs);
	TiXmlDocument* m_xml;
	GsElementNode  m_root;

};


} // namespace gs

#endif //__GS_CONFIGURATION_FILE_H__
