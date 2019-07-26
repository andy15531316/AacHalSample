#include "GsXml.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>


namespace gs {

std::string Unicode_Utf8(const wchar_t* wszString)
{
	// UNICODE to UTF-8
	int u8Len = ::WideCharToMultiByte(CP_UTF8, 0, wszString, -1, NULL, 0, NULL, NULL);

	char* szU8 = new char[u8Len + 1];
	ZeroMemory(szU8, sizeof(char) * u8Len);

	::WideCharToMultiByte(CP_UTF8, NULL, wszString, wcslen(wszString), szU8, u8Len, NULL, NULL);

	// Result
	std::string utf8 = szU8;
	delete [] szU8;
	return utf8;
}

std::wstring Utf8_Unicode(const char* szU8)
{
	assert(szU8 != NULL);

	// UTF-8 to UNICODE
	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, -1, NULL, 0);

	wchar_t* wszString = new wchar_t[wcsLen + 1];
	ZeroMemory(wszString, sizeof(wchar_t) * wcsLen);

	::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);

	// Result
	std::wstring unicode = wszString;
	delete [] wszString;
	return unicode;
}

namespace {

inline char ToLower(char c)
{
	if ( c >= 'A' && c <= 'Z')
		return c - 'A' + 'a';
	else
		return c;
}

std::string ToLowerCaseStr(const char* type)
{
	std::string Str(type);
	transform(Str.begin(), Str.end(), Str.begin(), tolower);
	return Str;
}

bool CiStrCompare(const char* x, const char* y)
{
	if (x == y)
		return true;
	if (x == NULL || y == NULL)
		return false;

	while ( *x != '\0' && *y != '\0' )
	{
		if ( ToLower(*x) !=  ToLower(*y) )
			return false;
		++x;
		++y;
	}

	return *x == *y;
}

} // namespace


//------------------------------------------------------------------------------

GsElementNode::GsElementNode(void)
{
   m_proxy = Proxy();
}

//3-18 weili add
GsElementNode::GsElementNode(TiXmlElement* element)
{
	m_proxy = Proxy(element);
}

GsElementNode GsElementNode::Proxy::NextSibling()
{
	GsElementNode childElem;
	TiXmlNode* node = m_element->NextSibling();

	if (node)
	{
		childElem.m_proxy.m_element = node->ToElement();
		return childElem;
	}

	return childElem;
}

GsElementNode GsElementNode::Proxy::NextSibling(const char* type)
{
	std::string LoCaseType = ToLowerCaseStr(type);

	GsElementNode childElem;
	TiXmlNode* node = m_element->NextSibling(LoCaseType.c_str());

	if (node)
	{
		childElem.m_proxy.m_element = node->ToElement();
		return childElem;
	}

	return childElem;
}

GsElementNode GsElementNode::Proxy::NextSibling(const wchar_t* type)
{
	return NextSibling( Wcs2Str(type).c_str() );
}

GsElementNode GsElementNode::Proxy::SearchChild(const char* type, const char* key)
{
	if (key != NULL && key[0] == '\0') return SearchChild(type, NULL);

	std::string LoCaseType = ToLowerCaseStr(type);

	GsElementNode childElem;
	TiXmlNode* node = m_element->FirstChild(LoCaseType.c_str());

	if (node)
	{
		if(key == NULL)
		{
			childElem.m_proxy.m_element = node->ToElement();
			return childElem;
		}

		while (node)
		{
			const char* dstKey;
			dstKey = node->ToElement()->Attribute("key");

			if ( CiStrCompare(key, dstKey) )
			{
				childElem.m_proxy.m_element = node->ToElement();
				return childElem;
			}

			node = node->ToElement()->NextSibling(LoCaseType.c_str());
		}
	}

	return childElem;
}

//wayde add
GsElementNode GsElementNode::Proxy::SearchChild(const char* type, const char* key, const char *value)
{
	GsElementNode childElem;
	if (key == NULL || key[0] == '\0')
		return childElem;

	std::string LoCaseType = ToLowerCaseStr(type);

	TiXmlNode* node = m_element->FirstChild(LoCaseType.c_str());

	while (node)
	{
		const char* dstKey;
		dstKey = node->ToElement()->Attribute(key);

		if (CiStrCompare(value, dstKey))
		{
			childElem.m_proxy.m_element = node->ToElement();
			return childElem;
		}

		node = node->ToElement()->NextSibling(LoCaseType.c_str());
	}

	return childElem;
}

//wayde add
GsElementNode GsElementNode::Proxy::SearchChild(const char* type, const char* key1, const char *value1, const char* key2, const char *value2)
{
	GsElementNode childElem;
	if (key1 == NULL || key1[0] == '\0')
		return childElem;

	if (key2 == NULL || key2[0] == '\0')
		return childElem;

	std::string LoCaseType = ToLowerCaseStr(type);
	std::string LoCasekey1 = ToLowerCaseStr(key1);
	std::string LoCasekey2 = ToLowerCaseStr(key2);
	TiXmlNode* node = m_element->FirstChild(LoCaseType.c_str());

	while (node)
	{
		const char* dstValue1;
		const char* dstValue2;
		dstValue1 = node->ToElement()->Attribute(LoCasekey1.c_str());
		dstValue2 = node->ToElement()->Attribute(LoCasekey2.c_str());

		if (CiStrCompare(value1, dstValue1) && CiStrCompare(value2, dstValue2))
		{
			childElem.m_proxy.m_element = node->ToElement();
			return childElem;
		}

		node = node->ToElement()->NextSibling(LoCaseType.c_str());
	}

	return childElem;
}

//wayde add
GsElementNode GsElementNode::Proxy::SearchChild(const wchar_t* type, const wchar_t* key1, const wchar_t *value1, const wchar_t* key2, const wchar_t *value2)
{
	return SearchChild( Wcs2Str(type).c_str(), Wcs2Str(key1).c_str(), Wcs2Str(value1).c_str(), Wcs2Str(key2).c_str(), Wcs2Str(value2).c_str() );
}

GsElementNode GsElementNode::Proxy::SearchChild(const wchar_t* type, const wchar_t* key)
{
	if (key != NULL)
		return SearchChild( Wcs2Str(type).c_str(), Wcs2Str(key).c_str() );
	else
		return SearchChild(Wcs2Str(type).c_str(), NULL);
}

GsElementNode GsElementNode::Proxy::FirstChild(void)
{
	GsElementNode childElem;
	TiXmlNode* node = m_element->FirstChild();

	if (node)
		childElem.m_proxy.m_element = node->ToElement();

	return childElem;
}

GsElementNode GsElementNode::Proxy::FirstChild(const char* type)
{
	return SearchChild(type, NULL);
}

GsElementNode  GsElementNode::Proxy::FirstChild(const wchar_t* type)
{
	return FirstChild( Wcs2Str(type).c_str() );
}

GsElementNode GsElementNode::Proxy::Parent(void)
{
	GsElementNode parentElem;

	if(!m_element)
		return parentElem;

	TiXmlNode* node = m_element->Parent();

	if ( node )
		parentElem.m_proxy.m_element = node->ToElement();

	return parentElem;
}



GsElementNode::Proxy* GsElementNode::operator ->(void)
{
	if (m_proxy.m_element == NULL)
		return NULL;
	else
		return &m_proxy;
}


const GsElementNode::Proxy* GsElementNode::operator ->(void) const
{
	if (m_proxy.m_element == NULL)
		return NULL;
	else
		return &m_proxy;
}

GsElementNode::Proxy& GsElementNode::operator *()
{
	return m_proxy;
}

bool GsElementNode::operator == (void* address)
{
	return (address == NULL && m_proxy.m_element == address);
}


bool GsElementNode::operator == (const GsElementNode& rhs)
{
	return	m_proxy.m_element == rhs.m_proxy.m_element;
}

bool GsElementNode::operator != (void* address)
{
	return  !(this->operator ==(address) );
}


bool GsElementNode::operator != (const GsElementNode& rhs)
{
	return	m_proxy.m_element != rhs.m_proxy.m_element;
}

GsElementNode::Proxy::Proxy(void)
	:m_element(NULL)
{

}

GsElementNode::Proxy::Proxy(TiXmlElement* element)
	:m_element(element)
{

}

GsElementNode GsElementNode::Proxy::AddElementWithoutKey(const char* type)
{
	std::string target = ToLowerCaseStr(type);
	TiXmlElement* element = NULL;
	element = new TiXmlElement(target.c_str());


	if (!m_element->LinkEndChild(element))
	{
		delete element;
		element = NULL;
	}

	return GsElementNode(element);

}


GsElementNode GsElementNode::Proxy::AddElementWithoutKey(const wchar_t* type)
{
	return AddElementWithoutKey(Wcs2Str(type).c_str());
}


GsElementNode GsElementNode::Proxy::AddElementValue(const char* type, const char* str)
{
	std::string target = ToLowerCaseStr(type);
	TiXmlElement* element = NULL;
	element = new TiXmlElement(target.c_str());


	if (!m_element->LinkEndChild(element))
	{
		delete element;
		element = NULL;
	}
	else
	{
		TiXmlText* text = new TiXmlText(str);
		if (!element->LinkEndChild(text))
		{
			delete text;
			delete element;
			element = NULL;
		}

	}

	return GsElementNode(element);
}


GsElementNode GsElementNode::Proxy::AddElementValue(const wchar_t* type, const wchar_t* str)
{
	return AddElementValue(Wcs2Str(type).c_str(), Wcs2Str(str).c_str());
}



GsElementNode GsElementNode::Proxy::AddElement(const char* type)
{
	return AddElement(type, NULL);
}

GsElementNode GsElementNode::Proxy::AddElement(const wchar_t* type)
{
	return AddElement(Wcs2Str(type).c_str(), NULL);
}

GsElementNode GsElementNode::Proxy::AddElement(const char* type, const char* key)
{
	if (key != NULL && key[0] == '\0' ) return AddElement(type);

	bool bCreatIfNotExit = true;

	TiXmlElement* leafElem = FindNoneLeafElement(m_element, type, key, bCreatIfNotExit);

	if ( !leafElem )
		return GsElementNode();

	return GsElementNode(leafElem);
}

GsElementNode GsElementNode::Proxy::AddElement(const wchar_t* type, const wchar_t* key)
{
	return AddElement( Wcs2Str(type).c_str(), Wcs2Str(key).c_str() );
}

bool GsElementNode::Proxy::AddChild(GsElementNode& node)
{

	std::string type;
	std::string key;
	node->GetType(type);
	node->GetType(key);
	TiXmlElement* element = FindElement(m_element, type.c_str(), key.c_str(), false);
	if (!element)
	{
		RemoveElement(type.c_str(), key.c_str());
	}

	TiXmlElement* copy = node->m_element->Clone()->ToElement();
	m_element->LinkEndChild(copy);
	return true;

}


bool GsElementNode::Proxy::AddChild(const std::wstring& xmlData)
{
	std::wstring data;

	//data  = L"<root>";
	data = xmlData;
	//data += L"</root>";

	TiXmlDocument doc;
	doc.Parse(Wcs2Str(data.c_str() ).c_str(), 0, TIXML_ENCODING_UTF8);


	TiXmlNode* node = doc.FirstChild();
	while (node != NULL)
	{
		TiXmlNode* clone = node->Clone();
		m_element->LinkEndChild(clone);
		node = node->NextSibling();
	}

	return true;
}

bool GsElementNode::Proxy::RemoveElement(const char* type, const char* key)
{
	bool bCreatIfNotExit = false;
	TiXmlElement* leafElem = FindNoneLeafElement(m_element, type, key, bCreatIfNotExit);

	if ( !leafElem )
		return false;

	return m_element->RemoveChild(leafElem);
}

void GsElementNode::Proxy::Clear(void)
{
	m_element->Clear();
}


/*bool GsElementNode::AddElement(const char* type, const char* key, GsElementNode& newElem)
{
	bool bCreatIfNotExit = true;

	TiXmlElement* leafElem = FindElement(m_element, type, key, bCreatIfNotExit);

	if ( !leafElem )
		return false;

	newElem.m_element = leafElem;
	return true;


}*/

bool GsElementNode::Proxy::GetInt(int& value)
{
	if (m_element == NULL)
		return false;

	TiXmlText* text = m_element->FirstChild()->ToText();
	if (text == NULL)
		return false;

	value = strtol(text->Value(), NULL, 0);

	return true;
}

bool GsElementNode::Proxy::GetInt(const char* type, int& value)
{
	return GetInt(type, (const char*)NULL, value);
}

bool GsElementNode::Proxy::GetInt(const wchar_t* type, int& value)
{
	return GetInt( Wcs2Str(type).c_str(), value);
}

bool GsElementNode::Proxy::GetInt(const char* type, int key, int& value)
{
	char buf[64];
	sprintf(buf, "%d", key);
	return GetInt(type, buf, value);
}

bool GsElementNode::Proxy::GetInt(const wchar_t* type, int key, int& value)
{
	wchar_t buf[64];
	swprintf(buf, L"%d", key);
	return GetInt(type, buf, value);
}

bool GsElementNode::Proxy::GetInt(const char* type, const char* key, int& value)
{
	if (key != NULL && key[0] == '\0' ) return GetInt(type, value);

	std::string ciType = ToLowerCaseStr(type);

	bool bCreatIfNotExist = false;
	TiXmlElement* element =  FindLeafElement(m_element, ciType.c_str(), key, bCreatIfNotExist);

	if (element == NULL)
		return false;

	TiXmlText* text = element->FirstChild()->ToText();
	if (text == NULL)
		return false;

	value = strtol( text->Value(), NULL, 0 );

	return true;
}


bool GsElementNode::Proxy::GetInt(const wchar_t* type, const wchar_t* key, int& value)
{
	return GetInt( Wcs2Str(type).c_str(), Wcs2Str(key).c_str(), value );
}


bool GsElementNode::Proxy::GetInt(int& value, int defaultValue)
{
	value = defaultValue;
	if (m_element == NULL)
		return false;

	TiXmlText* text = m_element->FirstChild()->ToText();
	if (text == NULL)
		return false;

	value = strtol(text->Value(), NULL, 0);

	return true;
}

bool GsElementNode::Proxy::GetInt(const char* type, int& value, int defaultValue)
{
	return GetInt(type, (const char*)NULL, value, defaultValue);
}

bool GsElementNode::Proxy::GetInt(const wchar_t* type, int& value, int defaultValue)
{
	if ( !GetInt( Wcs2Str(type).c_str(), value) )
	{
		value = defaultValue;
		return false;
	}
	else
		return true;
}

bool GsElementNode::Proxy::GetInt(const char* type, int key, int& value, int defaultValue)
{
	char buf[64];
	sprintf(buf, "%d", key);
	bool bSucceed =  GetInt(type, buf, value);

	if (!bSucceed)
		value = defaultValue;
	return bSucceed;
}


bool GsElementNode::Proxy::GetInt(const wchar_t* type, int key, int& value, int defaultValue)
{
	wchar_t buf[64];
	swprintf(buf, L"%d", key);
	bool bSucceed =  GetInt(type, buf, value);

	if (!bSucceed)
		value = defaultValue;
	return bSucceed;
}

bool GsElementNode::Proxy::GetInt(const char* type, const char* key, int& value, int defaultValue)
{
	value = defaultValue;
	if (key != NULL && key[0] == '\0' ) return GetInt(type, value);

	std::string ciType = ToLowerCaseStr(type);

	bool bCreatIfNotExist = false;
	TiXmlElement* element =  FindLeafElement(m_element, ciType.c_str(), key, bCreatIfNotExist);

	if (element == NULL)
		return false;

	TiXmlText* text = element->FirstChild()->ToText();
	if (text == NULL)
		return false;

	value = strtol(text->Value(), NULL, 0);

	return true;
}


bool GsElementNode::Proxy::GetInt(const wchar_t* type, const wchar_t* key, int& value, int defaultValue)
{
	return GetInt( Wcs2Str(type).c_str(), Wcs2Str(key).c_str(), value, defaultValue );
}

bool GsElementNode::Proxy::SetInt(int value)
{
	char buf[32];
#if defined( _MSC_VER )
	_itoa(value, buf, 10);
#else
	itoa(value, buf, 10);
#endif
	return SetStr(buf);
}

bool GsElementNode::Proxy::SetInt(const char* type, int value)
{
	return SetInt(type, (const char*)NULL, value);
}

bool GsElementNode::Proxy::SetInt(const wchar_t* type, int value)
{
	return SetInt( Wcs2Str(type).c_str(), value );
}

bool GsElementNode::Proxy::SetInt(const char* type, int key, int value)
{
	char buf[64];
	sprintf(buf, "%d", key);
	return SetInt(type, buf, value);
}
bool GsElementNode::Proxy::SetInt(const char* type, const char* key, int value)
{
	if (key != NULL && key[0] == '\0' ) return SetInt(type, value);

	TiXmlElement* leafElem = FindLeafElement(m_element, type, key, true);
	char buf[16];

	sprintf(buf, "%d", value);
	TiXmlText*    text = NULL;
	TiXmlNode* node = leafElem->FirstChild();
	if (node != NULL)
	{
		text = node->ToText();
		if ( text  == NULL )
			return false;
		text->SetValue(buf);
	}
	else
	{
		text = new TiXmlText(buf);
		if ( !leafElem->LinkEndChild( text ) )
		{
			delete text;
			return false;
		}

	}

	return true;
}

bool GsElementNode::Proxy::SetStr(const char* value)
{
	if (m_element == NULL)
		return false;

	TiXmlText* text = NULL;
	TiXmlNode* node = m_element->FirstChild();
	if (node != NULL)
	{
		text = node->ToText();
		if ( text  == NULL )
			return false;
		text->SetValue(value);
	}
	else
	{
		text = new TiXmlText(value);
		if ( !m_element->LinkEndChild( text ) )
		{
			delete text;
			return false;
		}
	}
	return true;
}

bool GsElementNode::Proxy::SetStr(const wchar_t* str)
{
	return SetStr(Wcs2Str(str).c_str() );
}

bool GsElementNode::Proxy::SetStr(const wchar_t* type, const wchar_t* str)
{
	return SetStr( Wcs2Str(type).c_str(), Wcs2Str(str).c_str() );
}

bool GsElementNode::Proxy::GetType(std::string& type)
{
	std::string key;
	return GetType(type, key);
}

bool GsElementNode::Proxy::GetType(std::wstring& type)
{
	std::string str;
	if (!GetType(str))
		return false;
	type = Str2Wcs(str.c_str());
	return true;
}

bool GsElementNode::Proxy::GetType(std::string& type, std::string& key)
{
	if (m_element == NULL) return false;

	const char* pch;

	pch = m_element->Value();
	type = "";
	assert(pch);
	type = pch;
	type = ToLowerCaseStr(type.c_str() );

	pch = m_element->Attribute("key");
	key = "";
	if (pch)
	{
		key = pch;		
	}
	return true;
}

bool GsElementNode::Proxy::GetType(std::wstring& type, std::wstring& key)
{
	std::string t, k;
	if (!GetType(t, k))
		return false;
	type = Str2Wcs(t.c_str());
	key = Str2Wcs(k.c_str());
	return true;
}

bool GsElementNode::Proxy::GetKey(std::string& key)
{
	const char* pch;

	pch = m_element->Attribute("key");
	key = "";
	if (pch) key = pch;
	return true;
}


bool GsElementNode::Proxy::GetKey(std::wstring& key)
{
	std::string str;
	if (!GetKey(str))
		return false;
	key = Str2Wcs(str.c_str());
	return true;
}


//wayde add
bool GsElementNode::Proxy::GetValue(const char *subkey,std::string& value)
{
	const char* pch;
	std::string lowsubkey = ToLowerCaseStr(subkey);

	pch = m_element->Attribute(lowsubkey.c_str());
	value = "";
	if (pch) value = pch;
	return true;
}

//wayde add
bool GsElementNode::Proxy::GetValue(const wchar_t *subkey,std::wstring& value)
{
	std::string strvalue ;
	std::string strsubkey = Wcs2Str(subkey);
	if (!GetValue(strsubkey.c_str(),strvalue))
		return false;
	value = Str2Wcs(strvalue.c_str());
	return true;
}

bool GsElementNode::Proxy::SetKey(std::string key)
{
	if (m_element == NULL) return false;

	m_element->SetAttribute("key", key.c_str());
	return true;
}


// GetStr

bool GsElementNode::Proxy::GetStr(std::string& str)
{
	str = "";

	if (m_element == NULL)
		return false;

	const char* pch = m_element->GetText();
	if (pch == NULL)
		return false;

	str = pch;
	return true;
}

bool GsElementNode::Proxy::GetStr(std::wstring& wcs)
{
	std::string str;
	if (!GetStr(str))
		return false;
	wcs = Str2Wcs(str.c_str());
	return true;
}

bool GsElementNode::Proxy::GetStr(const char* type, std::string& str)
{
	return GetStr(type, (const char*)NULL, str);
}

bool GsElementNode::Proxy::GetStr(const wchar_t* type, std::wstring& str)
{
	std::string tmp;
	bool ret = GetStr( Wcs2Str(type).c_str(), tmp);
	if (!ret)
		return false;

	str = Str2Wcs(tmp.c_str());
	return true;
}

bool GsElementNode::Proxy::GetStr(const char* type, int key, std::string& str)
{
	char buf[64];
	sprintf(buf, "%d", key);
	return GetStr(type, buf, str);
}
bool GsElementNode::Proxy::GetStr(const char* type, const char* key, std::string& str)
{
	if (key != NULL && key[0] == '\0' ) return GetStr(type, str);

	std::string LoCaseType = ToLowerCaseStr(type);

	TiXmlElement* element =  FindLeafElement(m_element, LoCaseType.c_str(), key);

	if (element == NULL)
		return false;

	TiXmlText* text = element->FirstChild()->ToText();
	if (text == NULL)
		return false;

	str = text->Value();

	return true;
}



bool GsElementNode::Proxy::GetStr(std::string& str, const char* defaultValue)
{
	str = "";

	if (m_element == NULL)
		return false;

	const char* pch = m_element->GetText();
	if (pch == NULL)
		return false;

	str = pch;
	return true;
}


bool GsElementNode::Proxy::GetStr(std::wstring& str, const wchar_t* default_value)
{
	if (!GetStr(str))
	{
		str = default_value;
		return false;
	}
	return true;
}


bool GsElementNode::Proxy::GetStr(const char* type, std::string& str, const char* defaultValue)
{
	return GetStr(type, (const char*)NULL, str, defaultValue);
}

bool GsElementNode::Proxy::GetStr(const wchar_t* type, std::wstring& str, const wchar_t* defaultValue)
{
	std::string tmp;
	bool ret = GetStr( Wcs2Str(type).c_str(), tmp);
	if (!ret)
	{
		str = defaultValue;
		return false;
	}

	str = Str2Wcs(tmp.c_str());
	return true;
}

bool GsElementNode::Proxy::GetStr(const char* type, int key, std::string& str, const char* defaultValue)
{
	char buf[64];
	sprintf(buf, "%d", key);
	return GetStr(type, buf, str, defaultValue);
}
bool GsElementNode::Proxy::GetStr(const char* type, const char* key, std::string& str, const char* defaultValue)
{
	if (key != NULL && key[0] == '\0' )
		return GetStr(type, str, defaultValue);

	std::string LoCaseType = ToLowerCaseStr(type);

	TiXmlElement* element =  FindLeafElement(m_element, LoCaseType.c_str(), key);

	str = defaultValue;
	if (element == NULL)
		return false;

	TiXmlText* text = element->FirstChild()->ToText();
	if (text == NULL)
		return false;

	str = text->Value();

	return true;
}

bool GsElementNode::Proxy::SetStr(const char* type, const char* str)
{
	return SetStr(type, (const char*)NULL, str);
}

bool GsElementNode::Proxy::SetStr(const char* type, int key, const char* str)
{
	char buf[64];
	sprintf(buf, "%d", key);
	return SetStr(type, buf, str);
}

bool GsElementNode::Proxy::SetStr(const char* type, const char* key, const char* str)
{
	if (key != NULL && key[0] == '\0' ) return SetStr(type, str);

	bool bNewElem = false;

	TiXmlElement* leafElem = FindLeafElement(m_element, type, key, true);

	TiXmlText*    text = NULL;
	TiXmlNode* node = leafElem->FirstChild();
	if (node != NULL)
	{
		text = node->ToText();
		if ( text  == NULL )
			return false;
		text->SetValue(str);
	}
	else
	{
		text = new TiXmlText(str);
		if ( !leafElem->LinkEndChild( text ) )
		{
			delete text;
			return false;
		}

	}
	return true;
}

bool GsElementNode::Proxy::SetStr(const wchar_t* type, const wchar_t* key, const wchar_t* str)
{
	std::string t = Wcs2Str(type);
	std::string k = Wcs2Str(key);
	std::string s = Wcs2Str(str);
	return SetStr(t.c_str(), k.c_str(), s.c_str());
}



TiXmlElement* GsElementNode::Proxy::
FindElement(TiXmlElement* parent, const char* type, const char* key, bool bCreateIfNotExist)
{
	std::string target = ToLowerCaseStr(type);

	TiXmlNode* node = parent->FirstChild( target.c_str() );
	TiXmlElement* element = NULL;

	if ( node )
		element = node->ToElement();

	if (element == NULL)
		goto Final;

	while (element)
	{
		const char* dstKey;
		dstKey = element->Attribute("key");
		if ( CiStrCompare(key, dstKey) )
			return element;

		node = element->NextSibling(target.c_str() );
		if (node)
			element = node->ToElement();
		else
			element = NULL;
	}

Final:

	if ( bCreateIfNotExist )
	{
		// No element found, create new
		element = new TiXmlElement( target.c_str() );
		if (key != NULL)
			element->SetAttribute("key", key);
		if ( ! parent->LinkEndChild( element ) )
		{
			delete element;
			element = NULL;
		}
	}

	return element;
}


TiXmlElement* GsElementNode::Proxy::
FindLeafElement(TiXmlElement* parent, const char* type, const char* key, bool bCreateIfNotExist)
{
	TiXmlElement* element = FindElement(parent, type, key, bCreateIfNotExist);
	if (!element) return element;

	TiXmlNode* node = element->FirstChild();

	if (node == NULL)
	{
		TiXmlText*  text = new TiXmlText("");
		if ( !element->LinkEndChild( text ) )
			delete text;
	}


	assert( CheckNoElementNode(element) );  // error if not leaf node

	return element;
}


TiXmlElement* GsElementNode::Proxy::
FindNoneLeafElement(TiXmlElement* parent, const char* type, const char* key, bool bCreateIfNotExist)
{
	TiXmlElement* element = FindElement(parent, type, key, bCreateIfNotExist);

	//assert( CheckNoValueNode(element) );  // error if it is a leaf node

	return element;
}


bool GsElementNode::Proxy::CheckNoElementNode(TiXmlElement* element)
{
	// if the element has text, it must not be a leaf node.
	if (element == NULL)
		return true;

	TiXmlNode* node = element->FirstChild();
	TiXmlText*    text = NULL;
	if (node != NULL)
	{
		text = node->ToText();
		return text != NULL;
	}
	return false;

}

bool GsElementNode::Proxy::CheckNoValueNode(TiXmlElement* element)
{
	// if the element has text, it must not be a leaf node.
	if (element == NULL)
		return true;

	TiXmlNode* node = element->FirstChild();
	TiXmlText*    text = NULL;
	if (node != NULL)
	{
		text = node->ToText();
		return text == NULL;
	}
	return true;

}


std::wstring GsElementNode::Proxy::ToWstring(void)
{
	TiXmlPrinter printer;
	printer.SetIndent("    ");
	m_element->Accept(&printer);
	return Str2Wcs( printer.CStr() );
}


//------------------------------------------------------------------------------
std::wstring GsElementNode::Str2Wcs(const char* str)
{
	int len =  strlen(str) ;
	int wlen = MultiByteToWideChar(CP_ACP, 0, str, len, NULL, 0);
	wchar_t* wcs = new wchar_t [wlen + 1];
	wcs[wlen] = 0;
	MultiByteToWideChar(CP_ACP, 0, str, len, wcs, wlen + 1);
	std::wstring ret = wcs;
	delete [] wcs;
	return ret;
}

std::string GsElementNode::Wcs2Str(const wchar_t* wcs)
{
	int wlen =  wcslen(wcs) ;
	int len = WideCharToMultiByte(CP_ACP, 0, wcs, wlen, NULL, 0, NULL, NULL);
	char* str = new char [len + 1];
	str[len] = 0;
	WideCharToMultiByte(CP_ACP, 0, wcs, wlen, str, len + 1, NULL, NULL);
	std::string ret = str;
	delete [] str;
	return ret;
}

GsConfigFile::GsConfigFile(void)
	: m_xml(NULL)
{

}

GsConfigFile::~GsConfigFile()
{
	Close();
}

bool GsConfigFile::Open(const char* filename, bool bCreateIfNotExist)
{
	assert( !IsOpen() );

	m_xml = new TiXmlDocument;
	//bool bExist = m_xml->LoadFile( filename, TIXML_ENCODING_UTF8 );
	bool bExist = m_xml->LoadFile( filename);

	if ( !bExist && bCreateIfNotExist)
	{
		// Add <?xml version="1.0"?>
		//TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
		//TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "ISO-8859-1", "" );
		TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "no" );
		m_xml->LinkEndChild( decl );
		TiXmlElement* root = new TiXmlElement("root");
		m_xml->LinkEndChild( root );

		bExist = m_xml->SaveFile(filename);
	}

	TiXmlDeclaration* decl;
	TiXmlElement* root;
	if ( !bExist )
		goto Error;

	decl = m_xml->FirstChild()->ToDeclaration();
	if (decl == NULL)
		goto Error;

	root = decl->NextSibling()->ToElement();
	if (root == NULL)
		goto Error;

	m_root = GsElementNode(root);

	return true;
Error:

	delete m_xml;
	m_xml = NULL;

	return false;
}

bool GsConfigFile::Open(const wchar_t* filename, bool bCreateIfNotExist)
{
	return Open( Wcs2Str(filename).c_str(), bCreateIfNotExist );
}

//3-18 weili add.
bool GsConfigFile::SetProfileFromString(const wchar_t* xmlData)
{
	assert(!IsOpen());

	m_xml = new TiXmlDocument;
	std::wstring data;
	data = xmlData;
	m_xml->Parse(Wcs2Str(data.c_str()).c_str(), 0, TIXML_ENCODING_UTF8);

	TiXmlElement* root = m_xml->RootElement();

	if (m_root == NULL)
		m_root = GsElementNode(root);
	else
		return false;
	



	//const char* strversion;
	//const char*  strhead;
	//const char*  strfuncId;
	//const char*  strdevice;
	//const char*  strIsEnabled;

	//TiXmlElement* version = root->FirstChildElement("version");
	//strversion = version->GetText();
	//TiXmlElement* header = root->FirstChildElement("header");
	//strhead = header->GetText();
	//TiXmlElement* funcId = root->FirstChildElement("funcid");
	//strfuncId = funcId->GetText();
	//TiXmlElement* device = root->FirstChildElement("device");
	//strdevice = device->Attribute("key");
	////TiXmlElement* isEnabled = device->FirstChildElement("isenabled");
	////strIsEnabled = isEnabled->GetText();
	//

	return true;
}


bool GsConfigFile::OpenFromString(const wchar_t* xmlData)
{
	assert(!IsOpen());

	m_xml = new TiXmlDocument;

	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "UTF-8", "no");
	decl->Parse(Wcs2Str(xmlData).c_str(), 0, TIXML_ENCODING_UTF8);
	TiXmlNode* node = decl->NextSibling();

	TiXmlElement* root;
	if (node == NULL)
		return false;
	root = node->ToElement();
	m_xml->LinkEndChild(root);

	return true;
}

bool GsConfigFile::Create(void)
{
	assert(!IsOpen());

	m_xml = new TiXmlDocument();

	// Add <?xml version="1.0"?>
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "UTF-8", "no");
	m_xml->LinkEndChild(decl);
	TiXmlElement* root = new TiXmlElement("root");
	m_xml->LinkEndChild(root);

	m_root = GsElementNode(root);
	return true;
Error:

	delete m_xml;
	m_xml = NULL;

	return false;
}

bool GsConfigFile::Create(const char* filename)
{
	assert( !IsOpen() );

	m_xml = new TiXmlDocument(filename);

	// Add <?xml version="1.0"?>
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "no" );
	m_xml->LinkEndChild( decl );
	TiXmlElement* root = new TiXmlElement("root");
	m_xml->LinkEndChild( root );

	bool result = m_xml->SaveFile(filename);
	
	if ( !result )	
		goto Error;
	m_root = GsElementNode(root);
	return true;
Error:

	delete m_xml;
	m_xml = NULL;

	return false;
}

bool GsConfigFile::Create(const wchar_t* filename)
{
	return Create( Wcs2Str(filename).c_str() );
}

bool GsConfigFile::IsOpen(void) const
{
	return m_xml != NULL;
}

bool GsConfigFile::Save(void)
{
	assert( IsOpen() );
	return m_xml->SaveFile();
}

bool GsConfigFile::SaveAs(const wchar_t* filename)
{
	assert(IsOpen());

	return m_xml->SaveFile(Wcs2Str(filename).c_str());
}


void GsConfigFile::Close(void)
{
	if (m_xml)
	{
		delete m_xml;
		m_xml = NULL;
	}
}


GsElementNode& GsConfigFile::Root(void)
{
	return m_root;
}

std::string GsConfigFile::Wcs2Str(const wchar_t* wcs)
{
	return GsElementNode::Wcs2Str(wcs);
}


void GsConfigFile::ToString(std::wstring& msg)
{
	TiXmlPrinter printer;
	printer.SetIndent("    ");
	m_xml->Accept(&printer);
	msg = GsElementNode::Str2Wcs(printer.CStr());
	
}


}  // namespace gs



