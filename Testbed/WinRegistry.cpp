#include "WinRegistry.h"
#include <shlwapi.h>
#include <shellapi.h>
#include <windows.h>




//-----------------------------------------------------------------------------

namespace {


template < typename char_t >
void PaserStrings(const char_t* buf, std::vector< std::basic_string<char_t> >& strs)
{
   size_t i = 0;
   size_t strIndex = 0;
   char_t terminate = (char_t) '\0';

   strs.clear();
   while  ( buf[i] != terminate )
   {
      strs.resize( strIndex + 1 );
      strs[strIndex] = buf + i;
      i += strs[strIndex].length() + 1;
      ++strIndex;
   }
}


} // namespace



//------------------------------------------------------------------------------


namespace dali { 


namespace system { 



namespace Registry {



// CreateKey 
bool CreateKey(HKEY& key, HKEY root, const char* keyName, DWORD right)
{
   LONG result;
   DWORD disposition;

   result = RegCreateKeyExA( 
         root,  keyName, 0,    NULL, REG_OPTION_NON_VOLATILE, 
         right, NULL,    &key, &disposition
      );

   return result == ERROR_SUCCESS;
}


// OpenKey
bool OpenKey(HKEY& key, HKEY root, const char* keyName, DWORD right)
{
   LONG result;
 
   result = RegOpenKeyExA( root, keyName, 0, right, &key);

   return result == ERROR_SUCCESS;
}


// DeleteKey
bool DeleteKey(HKEY root, const char* keyName, bool bSubKey)
{
   LONG result;

   if ( !bSubKey )
      result = RegDeleteKeyA(root, keyName);
   /*else
      result = SHDeleteKey(root, keyName);*/

   return result == ERROR_SUCCESS;
}


// KeyExists
bool KeyExists(HKEY root, const char* keyName)
{
   HKEY key;

   if (!OpenKey(key, root, keyName, KEY_READ) )
     return false;

   CloseKey(key);
   return true;
}


// CloseKey
void CloseKey(HKEY key)
{
   ::RegCloseKey(key);
}


// FlushKey
bool FlushKey(HKEY key)
{
   LONG result;

   result = ::RegFlushKey(key);
   return result == ERROR_SUCCESS;
}


// GetSubKeyInfo
//...Return the count and maximum size of subkeys.
bool GetSubKeyInfo(HKEY key, DWORD* subkeyCount, DWORD* maxSubkeyLen)
{
   LONG  result;

   result = RegQueryInfoKeyA(
      key, NULL, NULL, NULL, subkeyCount, maxSubkeyLen, NULL, NULL, NULL, NULL, NULL, NULL);

   if ( maxSubkeyLen )
      ++(*maxSubkeyLen);
   return result == ERROR_SUCCESS;
}


// GetKeyValueInfo
bool GetKeyValueInfo(
   HKEY key, DWORD* keyValueCount, DWORD* maxValueNameSize, DWORD* maxValueSize)
{
   LONG  result;

   result = RegQueryInfoKeyA(
      key, NULL, NULL, NULL, NULL, NULL, NULL, 
      keyValueCount, maxValueNameSize, maxValueSize, NULL, NULL );

   if ( maxValueNameSize )
      ++(*maxValueNameSize);

   return result == ERROR_SUCCESS;
}


// GetValueInfo
bool GetValueInfo(HKEY key, const char* valueName, DWORD* type, DWORD* size)
{
   LONG  result;

   result = RegQueryValueExA(key, valueName, NULL, type, NULL, size);

   return result == ERROR_SUCCESS;
}



// DeleteValue
bool DeleteValue(HKEY key, const char* valueName)
{
   LONG result;

   result = RegDeleteValueA(key, valueName);

   return result == ERROR_SUCCESS;
}


// WriteBinary
bool WriteBinary(HKEY key, const char* valueName, const BYTE* value, DWORD size)
{
   LONG result;

   result = RegSetValueExA(key, valueName, NULL, REG_BINARY, value, size);

   return result == ERROR_SUCCESS;
}


// ReadBinary
bool ReadBinary(HKEY key, const char* valueName, BYTE* value, DWORD& size)
{
   LONG result;
   DWORD type;

   result = RegQueryValueExA(key, valueName, NULL, &type, value, &size);
   return result == ERROR_SUCCESS && type == REG_BINARY;
}


// WriteDword
bool WriteDword(HKEY key, const char* valueName, DWORD value)
{
   LONG result;

   result = RegSetValueExA( 
         key, valueName, NULL, REG_DWORD, (BYTE*)(&value), sizeof(DWORD)
      );

   return result == ERROR_SUCCESS;
}


// ReadDword
bool ReadDword(HKEY key, const char* valueName, DWORD& value)
{
   LONG result;
   DWORD size = sizeof(DWORD);
   DWORD type;

   result = RegQueryValueExA(key, valueName, NULL, &type, (BYTE *)(&value), &size);
   return result == ERROR_SUCCESS && type == REG_DWORD;
}


// ReadString
bool ReadString(HKEY key, const char* valueName,  std::basic_string<char>& str)
{
   bool  result;
   DWORD type, size;
   char* buf = NULL;

   result = GetValueInfo(key, valueName, &type, &size);

   if ( !result || (type != REG_SZ && type != REG_EXPAND_SZ && type != REG_MULTI_SZ) )
     return false;


   //// If type is valid, get the value from registry.

   buf = new char[ size + 1 ]; // add extra byte to ensure null terminated charater existent.
   if ( buf == NULL )
      return false;

   buf[size] = '\0'; 
   result = RegQueryValueExA( 
      key, valueName, NULL, &type, (LPBYTE)buf, &size ) == ERROR_SUCCESS;

   if ( result )
      str = buf;

   delete[] buf;

   return result;
}


// WriteString
bool WriteString(HKEY key, const char* valueName, const std::basic_string<char>& str)
{
   LONG result;

   result = RegSetValueExA( 
                  key, valueName, NULL, REG_SZ, (const BYTE*) str.c_str(), 
                  (DWORD) str.length() + 1
            );

  return result == ERROR_SUCCESS;
}


// WriteExpandString
bool WriteExpandString(HKEY key, const char* valueName, const std::basic_string<char>& str)
{
   LONG result;

   result = RegSetValueExA( 
                  key, valueName, NULL, REG_EXPAND_SZ, (const BYTE*) str.c_str(), 
                  (DWORD) str.length() + 1
            );

   return result == ERROR_SUCCESS;
}



// ReadMultiStrings
bool ReadMultiStrings(
      HKEY key, 
      const char* valueName,  
      std::vector< std::basic_string<char> >& strs
   )
{
   bool  result;
   DWORD type, size;
   char* buf = NULL;

   result = GetValueInfo(key, valueName, &type, &size);

   if ( !result || (type != REG_SZ && type != REG_EXPAND_SZ && type != REG_MULTI_SZ ) )
     return false;


   //// If type is valid, get the value from registry.  

   buf = new char[ size + 2 ]; // add extra bytes to ensure double null terminated charater existent.
   if ( buf == NULL )
      return false;

   buf[size] = '\0'; 
   buf[size + 1] = '\0'; 
   result = RegQueryValueExA( 
      key, valueName, NULL, &type, (LPBYTE)buf, &size ) == ERROR_SUCCESS;

   if ( result )
      PaserStrings(buf, strs);

   delete[] buf;

   return result;   
}


} // namespace Registry


} // namespace system


} // namespace dali



//-----------------------------------------------------------------------------

namespace dali {



namespace detail {


RegValueBase::RegValueBase(HKEY key)
   :m_nameBuf(NULL),
    m_nameSize(0)
{
   m_key = key; 
   AllocRes();
}


RegValueBase::~RegValueBase()
{
   ReleaseRes();
}


// GetCount
bool RegValueBase::GetCount(size_t& count) const
{
   DWORD c, size;
   bool result;

   result = LoadInfo(c, size);

   if (result)
      count = c;

   return result;
}


// Get
//...get the ith value.
bool RegValueBase::Get(size_t index, RegValueInfo& info) const
{
   char* tmpName = NULL;
   DWORD nameSize = m_nameSize;
   LONG result;
   bool retValue;
   DWORD type;

   while (1)
   {
      result = RegEnumValueA( 
         m_key, (DWORD) index, m_nameBuf, &nameSize, NULL, &type, NULL, NULL);

      if (result != ERROR_SUCCESS && result != ERROR_MORE_DATA )
      {
         retValue = false;
         break;
      }

      if (nameSize <  m_nameSize - 1)
      {
         retValue = true;
         break;
      }
        
      // the size of name buffer is too small.

      nameSize = m_nameSize * 2;
      tmpName = new char[nameSize];

      delete[] m_nameBuf;
      m_nameBuf = tmpName;
      m_nameSize = nameSize;
      tmpName = NULL;
   }

   if (retValue)
   {
      info.name = m_nameBuf;
      info.type = type;
      GetValueContent(info.value);
   }

   return retValue;
}


// GetValueContent
void RegValueBase::GetValueContent(std::vector<BYTE>& value) const
{
  LONG  bSuccess;
  DWORD size;
  BYTE* valueBuf;

  // Measure value size
  bSuccess = RegQueryValueExA(m_key, m_nameBuf, NULL, NULL, NULL, &size);
  if (bSuccess == ERROR_SUCCESS)
  {
     valueBuf = new BYTE[size];
     bSuccess = RegQueryValueExA(m_key, m_nameBuf, NULL, NULL, valueBuf, &size);
     
     value = std::vector<BYTE>(valueBuf, valueBuf + size);
     delete valueBuf;
  }
  else
     value.clear(); 
}


// AllocRes
bool RegValueBase::AllocRes(void)
{
   DWORD count;

   if ( !LoadInfo(count, m_nameSize) )
   {
      ReleaseRes();
      return false;
   }

   m_nameBuf = new char[m_nameSize];
   return true;
}


// ReleaseRes
void RegValueBase::ReleaseRes(void)
{
   m_nameSize = 0;
   delete[] m_nameBuf;
   m_nameBuf = NULL;
}


// LoadInfo
//...Load value count and maximum name size from the resgistry.
inline bool RegValueBase::LoadInfo(DWORD& count, DWORD& size) const
{
   return WinRegistry::GetKeyValueInfo(m_key, count, size);
}




//-----------------------------------------------------------------------------


// RegEnumValueBase

RegEnumValueBase::RegEnumValueBase(void)
   : m_base(NULL),
     m_index(-1)
{

}


RegEnumValueBase::RegEnumValueBase(RegValueBase* base)
   : m_base(base),
     m_index(-1)
{
   GotoNext();
}


RegEnumValueBase::RegEnumValueBase(RegValueBase* base, int)
   :  m_base(base),
      m_index(-1)
{
   
}


// IsDone
bool RegEnumValueBase::IsDone(void) const
{
   assert(m_base != NULL);

   return m_index == -1;
}


// Next
void RegEnumValueBase::Next(void)
{
   assert( !IsDone() );

   GotoNext();
}


// GotoNext
void RegEnumValueBase::GotoNext(void)
{
   assert(m_base != NULL);

   while (1)
   {
      ++m_index;
      if ( !m_base->Get(m_index, m_info) )
      {
         m_index = -1;
         return;
      }
   }
}


} // namespace detail




//-----------------------------------------------------------------------------

// RegValueIterator

RegValueIterator::RegValueIterator(void)
{
   
}


RegValueIterator::RegValueIterator(detail::RegValueBase* values)
   : m_base(values)
{
   
}


RegValueIterator::RegValueIterator(detail::RegValueBase* values, int)
   : m_base(values, -1)
{

}


// operator ++
RegValueIterator& RegValueIterator::operator ++(void)
{
   DoNext();
   return (*this);
}


RegValueIterator  RegValueIterator::operator ++(int)
{
   RegValueIterator temp = *this; 

   DoNext();
   return temp;
}


// MakeLastIterator
RegValueIterator RegValueIterator::MakeLastIterator(detail::RegValueBase* values)
{
   return RegValueIterator(values, -1);
}


// DoNext
void RegValueIterator::DoNext(void)
{
   m_base.Next();
}




//-----------------------------------------------------------------------------

// RegValueSet

RegValueSet::RegValueSet(void)
   :m_base(NULL)
{
   
}


RegValueSet::RegValueSet(detail::RegValueBase* base)
   :m_base(base)
{
   
}


// Begin
RegValueSet::Iterator RegValueSet::Begin(void)
{
   return RegValueIterator(m_base);
}


// End
RegValueSet::Iterator RegValueSet::End(void)
{
   return RegValueIterator::MakeLastIterator(m_base) ;
}


// Count
size_t RegValueSet::Count(void) const
{
   size_t count;

   if (m_base == NULL || !m_base->GetCount(count) )
      return 0;
   else
      return count;
}



//-----------------------------------------------------------------------------


// RegValueEnumerater

RegValueEnumerater::RegValueEnumerater(void)
  : m_base(NULL),
    m_releasingKeyNeeded(false)
{

}


RegValueEnumerater::RegValueEnumerater(HKEY key)
  : m_base(NULL), 
    m_releasingKeyNeeded(false)
{
   Open(key); 
}


RegValueEnumerater::RegValueEnumerater(const char* keyName)
   :  m_base(NULL),
      m_releasingKeyNeeded(false)
{
   Open(keyName);
}


RegValueEnumerater::RegValueEnumerater(HKEY root, const char* keyName)
   :  m_base(NULL),
      m_releasingKeyNeeded(false)
{
   Open(root, keyName);
}


RegValueEnumerater::~RegValueEnumerater()
{
   Close();
}


// Open
//...Start to enumerate all value in specified key.
bool RegValueEnumerater::Open(HKEY key)
{
   detail::RegValueBase* values = NULL;

   try
   {
      values = new detail::RegValueBase(key);
      m_base = new detail::RegEnumValueBase(values);
   }
   catch (...)
   { 
      delete values;
      delete m_base;
      m_base = NULL;

      return false;
   }

   return true;
}


bool RegValueEnumerater::Open(const char* keyName)
{
   return Open(HKEY_CURRENT_USER, keyName);
}


bool RegValueEnumerater::Open(HKEY root, const char* keyName)
{
   bool bSuccess;

   bSuccess = WinRegistry::OpenKey(m_key, root, keyName, KEY_READ);
   if (bSuccess)
   {
      bSuccess = Open(m_key);
      if (!bSuccess)
         WinRegistry::CloseKey(m_key);
      else
         m_releasingKeyNeeded = true;
   }
   return bSuccess;
}


// Close
//...end the enumeration
void RegValueEnumerater::Close(void)
{
   if (m_base != NULL)
   {
      delete m_base->GetBase();
      delete m_base;
      m_base = NULL;
   }

   if (m_releasingKeyNeeded)
   {
      WinRegistry::CloseKey(m_key); 
      m_releasingKeyNeeded = false;
   }
}
  

// Next
//...Enumerate the next value.
void RegValueEnumerater::Next(void)
{
   assert( IsOpen() );

   DoNext();
}


// DoNext
void RegValueEnumerater::DoNext(void)
{
    m_base->Next(); 
}




//-----------------------------------------------------------------------------


// RegSubKeyBase
//...A base class for RegKeyEnumerater and RegKeyIterator.
class RegSubKeyBase
{
public:

   RegSubKeyBase(HKEY key);
   ~RegSubKeyBase();

   bool GetCount(size_t& count) const;
   bool Get(size_t index, std::string& keyName) const;
 
private:
  
   // Inhibit the copy constructor and assign operator.
   RegSubKeyBase(const RegSubKeyBase& other);
   RegSubKeyBase& operator = (const RegSubKeyBase& other);

   bool AllocRes(void); 
   void ReleaseRes(void);

   bool LoadInfo(DWORD& count, DWORD& maxNameSize) const;
   
private:

   HKEY    m_key;
   mutable char* m_nameBuf;
   mutable DWORD m_nameSize;
}; 


RegSubKeyBase::RegSubKeyBase(HKEY key)
   :m_nameBuf(NULL),
    m_nameSize(0)
{
   m_key = key; 
   AllocRes();
}


RegSubKeyBase::~RegSubKeyBase()
{
   ReleaseRes();
}


// GetCount
bool RegSubKeyBase::GetCount(size_t& count) const
{
   DWORD c, size;
   bool result;

   result = LoadInfo(c, size);

   if (result)
      count = c;

   return result;
}


// Get
//...get the ith key.
bool RegSubKeyBase::Get(size_t index, std::string& name) const
{
   LONG result;
   DWORD nameSize= m_nameSize;

   result = RegEnumKeyExA( 
      m_key, (DWORD) index, m_nameBuf, &nameSize, NULL, NULL, NULL, NULL );

   // if the buffer size of key-name is too small.
   if (result == ERROR_SUCCESS && nameSize >= m_nameSize)
   {
      delete[] m_nameBuf;
      m_nameSize = nameSize + 1;
      m_nameBuf = new char[m_nameSize];
      nameSize = m_nameSize;
      result = RegEnumKeyExA( 
         m_key, (DWORD) index, m_nameBuf, &nameSize, NULL, NULL, NULL, NULL );
   }
   
   if (result != ERROR_SUCCESS)
      name.clear();
   else
      name = m_nameBuf;

   return result == ERROR_SUCCESS;
}


// AllocRes
bool RegSubKeyBase::AllocRes(void)
{
   DWORD count;

   if ( !LoadInfo(count, m_nameSize) )
   {
      ReleaseRes();
      return false;
   }

   m_nameBuf = new char[m_nameSize];
   return true;
}


// ReleaseRes
void RegSubKeyBase::ReleaseRes(void)
{
   m_nameSize = 0;
   delete[] m_nameBuf;
   m_nameBuf = NULL;
}


// LoadInfo
//...Load key count and maximum name size from the resgistry.
inline bool RegSubKeyBase::LoadInfo(DWORD& count, DWORD& size) const
{
   return WinRegistry:: GetSubKeyInfo(m_key, count, size);
}




//-----------------------------------------------------------------------------


namespace detail {


// RegEnumKeyBase

RegEnumKeyBase::RegEnumKeyBase(void)
   : m_base(NULL),
     m_index(-1)
{

}


RegEnumKeyBase::RegEnumKeyBase(RegSubKeyBase* base)
   : m_base(base),
     m_index(-1)
{
   GotoNext();
}


RegEnumKeyBase::RegEnumKeyBase(RegSubKeyBase* base, int)
   :  m_base(base),
      m_index(-1)
{
   
}


// IsDone
bool RegEnumKeyBase::IsDone(void) const
{
   assert(m_base != NULL);

   return m_index == -1;
}


// Next
void RegEnumKeyBase::Next(void)
{
   assert( !IsDone() );

   GotoNext();
}


// GotoNext
void RegEnumKeyBase::GotoNext(void)
{
   assert(m_base != NULL);

   ++m_index;
   if ( !m_base->Get(m_index, m_keyName) )
   {
      m_index = -1;
      return;
   }
}


} // namespace detail







//-----------------------------------------------------------------------------

// RegKeyIterator

RegKeyIterator::RegKeyIterator(void)
{
   
}


RegKeyIterator::RegKeyIterator(RegSubKeyBase* keys)
   : m_base(keys)
{
   
}


RegKeyIterator::RegKeyIterator(RegSubKeyBase* keys, int)
   : m_base(keys, -1)
{

}


// operator ++
RegKeyIterator& RegKeyIterator::operator ++(void)
{
   DoNext();
   return (*this);
}


RegKeyIterator  RegKeyIterator::operator ++(int)
{
   RegKeyIterator temp = *this; 

   DoNext();
   return temp;
}



// MakeLastIterator
RegKeyIterator RegKeyIterator::MakeLastIterator(RegSubKeyBase* keys)
{
   return RegKeyIterator(keys, -1);
}


// DoNext
void RegKeyIterator::DoNext(void)
{
   m_base.Next(); 
}




//-----------------------------------------------------------------------------

// RegKeySet

RegKeySet::RegKeySet(void)
   :m_base(NULL)
{
   
}


RegKeySet::RegKeySet(RegSubKeyBase* base)
   :m_base(base)
{
   
}


// Begin
RegKeySet::Iterator RegKeySet::Begin(void)
{
   return RegKeyIterator(m_base);
}


// End
RegKeySet::Iterator RegKeySet::End(void)
{
   return RegKeyIterator::MakeLastIterator(m_base) ;
}


// Count
size_t RegKeySet::Count(void) const
{
   size_t count;

   if (m_base == NULL || !m_base->GetCount(count) )
      return 0;
   else
      return count;
}








//-----------------------------------------------------------------------------


// RegKeyEnumerater

RegKeyEnumerater::RegKeyEnumerater(void)
  : m_base(NULL),
    m_releasingKeyNeeded(false)
{

}


RegKeyEnumerater::RegKeyEnumerater(HKEY key)
  : m_base(NULL), 
    m_releasingKeyNeeded(false)
{
   Open(key); 
}


RegKeyEnumerater::RegKeyEnumerater(const char* keyName)
   :  m_base(NULL),
      m_releasingKeyNeeded(false)
{
   Open(keyName);
}


RegKeyEnumerater::RegKeyEnumerater(HKEY root, const char* keyName)
   :  m_base(NULL),
      m_releasingKeyNeeded(false)
{
   Open(root, keyName);
}


RegKeyEnumerater::~RegKeyEnumerater()
{
   Close();
}


// Open
//...Start to enumerate all subkeys in specified key.
bool RegKeyEnumerater::Open(HKEY key)
{
   RegSubKeyBase* subkeys = NULL;

   try
   {
      subkeys = new RegSubKeyBase(key);
      m_base = new detail::RegEnumKeyBase(subkeys);
   }
   catch (...)
   { 
      delete subkeys;
      delete m_base;
      m_base = NULL;

      return false;
   }

   return true;
}


bool RegKeyEnumerater::Open(const char* keyName)
{
   return Open(HKEY_CURRENT_USER, keyName);
}


bool RegKeyEnumerater::Open(HKEY root, const char* keyName)
{
   bool bSuccess;

   bSuccess = WinRegistry::OpenKey(m_key, root, keyName, KEY_READ);
   if (bSuccess)
   {
      bSuccess = Open(m_key);
      if (!bSuccess)
         WinRegistry::CloseKey(m_key);
      else
         m_releasingKeyNeeded = true;
   }
   return bSuccess;
}


// Close
//...end the enumeration
void RegKeyEnumerater::Close(void)
{
   if (m_base != NULL)
   {
      delete m_base->GetBase();
      delete m_base;
      m_base = NULL;
   }

   if (m_releasingKeyNeeded)
   {
      WinRegistry::CloseKey(m_key); 
      m_releasingKeyNeeded = false;
   }
}
  

// Next
//...Enumerate the next value.
void RegKeyEnumerater::Next(void)
{
   assert( IsOpen() );

   DoNext();
}


// DoNext
void RegKeyEnumerater::DoNext(void)
{
    m_base->Next(); 
}




//----------------------------------------------------------------------------

// RegReader


RegReader::RegReader(void)
   : m_values(NULL),
     m_keys(NULL),
     m_releasingKeyNeeded(false)
{
   
}


RegReader::RegReader(HKEY key)
   : m_values(NULL),
     m_keys(NULL),
     m_releasingKeyNeeded(false)
{
   Open(key);
}


RegReader::RegReader(const char* keyName)
   : m_values(NULL),
     m_keys(NULL),
     m_releasingKeyNeeded(false)
{
   Open(keyName);
}


RegReader::RegReader(HKEY root, const char* keyName)
   : m_values(NULL),
     m_keys(NULL),
     m_releasingKeyNeeded(false)
{
   Open(root, keyName);
}


// ~RegReader
RegReader::~RegReader()
{
   Close();
}


// Open
bool RegReader::Open(HKEY key)
{
   assert(!IsOpen() );

   m_key = key;
   
   m_values = new detail::RegValueBase(key);
   m_keys   = new RegSubKeyBase(key);

   return true;
}


bool RegReader::Open(const char* keyName)
{
   return Open(HKEY_CURRENT_USER, keyName);
}


bool RegReader::Open(HKEY root, const char* keyName)
{
   bool bSuccess;

   assert(!IsOpen() );

   bSuccess = WinRegistry::OpenKey(m_key, root, keyName, AccessRights() );
   if (bSuccess)
   {
      bSuccess = Open(m_key);
      m_releasingKeyNeeded = true;
   }
   
   return bSuccess;
}


// IsOpen
bool RegReader::IsOpen(void) const
{
   return m_keys != NULL;
}


// Close
void RegReader::Close(void)
{
   if (m_releasingKeyNeeded)
   {
      WinRegistry::CloseKey(m_key);
      m_releasingKeyNeeded = false;
   }

   delete m_values;
   delete m_keys;

   m_values = NULL;
   m_keys   = NULL;
}


// SubKeyExists
bool RegReader::SubKeyExists(const char* keyName) const
{
   return WinRegistry::KeyExists(m_key, keyName);
}


// ReadBinary
bool RegReader::ReadBinary(const char* valueName, BYTE* value, DWORD& size)
{
   return WinRegistry::ReadBinary(m_key, valueName, value, size);
}


// ReadDword
bool RegReader::ReadDword(const char* valueName, DWORD& value)
{
   return WinRegistry::ReadDword(m_key, valueName, value);
}


// ReadString
bool RegReader::ReadString(const char* valueName,  std::string& str)
{
   return system::Registry::ReadString(m_key, valueName, str);
}


// AccessRights
REGSAM RegReader::AccessRights(void) const
{
   return KEY_READ;
}



//----------------------------------------------------------------------------

// RegAccessor


RegAccessor::RegAccessor(void)
   : m_values(NULL),
     m_keys(NULL),
     m_releasingKeyNeeded(false)
{
   
}


RegAccessor::RegAccessor(HKEY key)
   : m_values(NULL),
     m_keys(NULL),
     m_releasingKeyNeeded(false)
{
   Open(key);
}


RegAccessor::RegAccessor(const char* keyName)
   : m_values(NULL),
     m_keys(NULL),
     m_releasingKeyNeeded(false)
{
   Open(keyName);
}


RegAccessor::RegAccessor(HKEY root, const char* keyName)
   : m_values(NULL),
     m_keys(NULL),
     m_releasingKeyNeeded(false)
{
   Open(root, keyName);
}


// ~RegAccessor
RegAccessor::~RegAccessor()
{
   Close();
}


// Open
bool RegAccessor::Open(HKEY key)
{
   assert(!IsOpen() );

   m_key = key;
   
   m_values = new detail::RegValueBase(key);
   m_keys   = new RegSubKeyBase(key);

   return true;
}


bool RegAccessor::Open(const char* keyName, bool bCreate)
{
   return Open(HKEY_CURRENT_USER, keyName, bCreate);
}


bool RegAccessor::Open(HKEY root, const char* keyName, bool bCreate)
{
   bool bSuccess;

   assert(!IsOpen() );

   if (bCreate)
      bSuccess = WinRegistry::CreateKey(m_key, root, keyName, KEY_ALL_ACCESS);
   else
      bSuccess = WinRegistry::OpenKey(m_key, root, keyName, KEY_ALL_ACCESS);

   if (bSuccess)
   {
      bSuccess = Open(m_key);
      m_releasingKeyNeeded = true;
   }
   
   return bSuccess;
}


// IsOpen
bool RegAccessor::IsOpen(void) const
{
   return m_keys != NULL;
}


// Close
void RegAccessor::Close(void)
{
   if (m_releasingKeyNeeded)
   {
      WinRegistry::CloseKey(m_key);
      m_releasingKeyNeeded = false;
   }

   delete m_values;
   delete m_keys;

   m_values = NULL;
   m_keys   = NULL;
}


// Flush
void RegAccessor::Flush(void)
{
   if (m_key != NULL)
      RegFlushKey(m_key);
}


// DeleteSubKey
bool  RegAccessor::DeleteSubKey(const char* keyName, bool bSubKey)
{
   return WinRegistry::DeleteKey(m_key, keyName, bSubKey);
}


// SubKeyExists
bool  RegAccessor::SubKeyExists(const char* keyName) const
{
   return WinRegistry::KeyExists(m_key, keyName);
}


// CreateSubKey
bool RegAccessor::
CreateSubKey(RegAccessor& key, const char* keyName)
{
   bool  result;
   HKEY  tmp;

   result = WinRegistry::CreateKey(tmp, m_key, keyName, KEY_ALL_ACCESS);

   if (result)
   {
      key.Close();
      key.Open(tmp); 
   }

   return result;
}


bool RegAccessor::CreateSubKey(HKEY& key, const char* keyName, DWORD right)
{
   return WinRegistry::CreateKey(key, m_key, keyName, right);
}


// OpenSubKey
bool RegAccessor::OpenSubKey(RegAccessor& key, const char* keyName)
{
   bool  result;
   HKEY  tmp;

   result = WinRegistry::OpenKey(tmp, m_key, keyName, KEY_ALL_ACCESS);
   if (result)
   {
      key.Close();
      key.Open(tmp); 
   }

   return result;
}


bool RegAccessor::OpenSubKey(HKEY& key, const char* keyName, DWORD right)
{
   return WinRegistry::OpenKey(key, m_key, keyName, right);
}


// CloseKey
void RegAccessor::CloseKey(HKEY subkey)
{
   WinRegistry::CloseKey(subkey);
}


// DeleteValue
bool  RegAccessor::DeleteValue(const char* valueName)
{
   return WinRegistry::DeleteValue(m_key, valueName);
}



// ReadBinary
bool RegAccessor::ReadBinary(const char* valueName, BYTE* value, DWORD& size)
{
   return WinRegistry::ReadBinary(m_key, valueName, value, size);
}


// WriteBinary
bool RegAccessor::WriteBinary(const char* valueName, const BYTE* value, DWORD size)
{
   return WinRegistry::WriteBinary(m_key, valueName, value, size);
}


// WriteDword
bool RegAccessor::WriteDword(const char* valueName, DWORD value)
{
   return WinRegistry::WriteDword(m_key, valueName, value);
}


// ReadDword
bool RegAccessor::ReadDword(const char* valueName, DWORD& value)
{
   return WinRegistry::ReadDword(m_key, valueName, value);
}


// WriteString
bool RegAccessor::WriteString(const char* valueName, const std::string& str)
{
   return system::Registry::WriteString(m_key, valueName, str);
}


// ReadString
bool RegAccessor::ReadString(const char* valueName,  std::string& str)
{
   return system::Registry::ReadString(m_key, valueName, str);
}


// SubKeys
RegKeySet RegAccessor::SubKeys(void) const
{
   return RegKeySet(m_keys);
}



// Values
RegValueSet  RegAccessor::Values(void) const
{
   return RegValueSet(m_values);
}






//-----------------------------------------------------------------------------

// WinRegistry
//...Access windows registry

namespace WinRegistry {



// CreateKey
bool CreateKey(HKEY& key, HKEY root, const char* keyName, DWORD right)
{
   LONG result;
   DWORD disposition;

   result = RegCreateKeyExA( 
         root,  keyName, 0,    NULL, REG_OPTION_NON_VOLATILE, 
         right, NULL,    &key, &disposition
      );

   return result == ERROR_SUCCESS;
}


// OpenKey
bool OpenKey(HKEY& key, HKEY root, const char* keyName, DWORD right)
{
   LONG result;
 
   result = RegOpenKeyExA( root, keyName, 0, right, &key);

   return result == ERROR_SUCCESS;
}


// CloseKey
void CloseKey(HKEY key)
{
   RegCloseKey(key);
}


// DeleteKey
bool DeleteKey(HKEY root, const char* keyName, bool bSubKey)
{
   LONG result;

   if ( !bSubKey )
      result = RegDeleteKeyA(root, keyName);
   /*else
      result = SHDeleteKey(root, keyName);*/

   return result == ERROR_SUCCESS;
}


// KeyExists
bool KeyExists(HKEY root, const char* keyName)
{
   HKEY key;

   if (!OpenKey(key, root, keyName, KEY_READ) )
     return false;

   CloseKey(key);
   return true;
}



// GetSubKeyInfo
//...Return the count and maximum size of subkeys.
bool GetSubKeyInfo(HKEY key, DWORD& count, DWORD& size)
{
   LONG  result;

   result = RegQueryInfoKey(
      key, NULL, NULL, NULL, &count, &size, NULL, NULL, NULL, NULL, NULL, NULL);

   ++size;
   return result == ERROR_SUCCESS;
}


// GetSubkeyCount
DWORD GetSubkeyCount(HKEY key)
{
   DWORD count;
   LONG  result;

   result = RegQueryInfoKey(
      key, NULL, NULL, NULL, &count, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

   if (result != ERROR_SUCCESS)
      count = 0;
   return count;
}


// GetSubkeySize
DWORD GetSubkeySize(HKEY key)
{
   DWORD size;
   LONG  result;

   result = RegQueryInfoKey(
      key, NULL, NULL, NULL, NULL, &size, NULL, NULL, NULL, NULL, NULL, NULL);

   if (result != ERROR_SUCCESS)
      size = 0;
   return ++size;
}


// FlushKey
bool FlushKey(HKEY key)
{
   LONG result;

   result = ::RegFlushKey(key);
   return result == ERROR_SUCCESS;
}


// GetValueInfo
bool GetValueInfo(HKEY key, const char* valueName, DWORD& type, DWORD& size)
{
   LONG  result;

   result = RegQueryValueExA(key, valueName, NULL, &type, NULL, &size);
   return result == ERROR_SUCCESS;
}


// GetValueType
bool GetValueType(HKEY key, const char* valueName, DWORD& type)
{
   LONG  result;

   result = RegQueryValueExA(key, valueName, NULL, &type, NULL, NULL);
   return result == ERROR_SUCCESS;
}


// GetValueSize
bool GetValueSize(HKEY key, const char* valueName, DWORD& size)
{
   LONG  result;

   result = RegQueryValueExA(key, valueName, NULL, NULL, NULL, &size);
   return result == ERROR_SUCCESS;
}


// GetKeyValueInfo
//...Return the count, maximum name size and maximum value size of values 
//   associated with a key.
bool GetKeyValueInfo(HKEY key, DWORD& count, DWORD& nameSize, DWORD& valueSize)
{
   LONG  result;

   result = RegQueryInfoKeyA(
      key, NULL, NULL, NULL, NULL, NULL, NULL, &count, &nameSize, &valueSize, NULL, NULL);

   ++nameSize;
   return result == ERROR_SUCCESS;
}


// GetKeyValueInfo
bool GetKeyValueInfo(HKEY key, DWORD& count, DWORD& nameSize)
{
   LONG  result;

   result = RegQueryInfoKeyA(
      key, NULL, NULL, NULL, NULL, NULL, NULL, &count, &nameSize, NULL, NULL, NULL);

   ++nameSize;
   return result == ERROR_SUCCESS;
}


// GetValueCount
DWORD GetValueCount(HKEY key)
{
   DWORD count;
   LONG  result;

   result = RegQueryInfoKeyA(
      key, NULL, NULL, NULL, NULL, NULL, NULL, &count, NULL, NULL, NULL, NULL);

   if (result != ERROR_SUCCESS)
      count = 0;
   return count;
}


// GetMaxValueNameSize
DWORD GetMaxValueNameSize(HKEY key)
{
   DWORD size;
   LONG  result;

   result = RegQueryInfoKey(
      key, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &size, NULL, NULL, NULL);

   if (result != ERROR_SUCCESS)
      size = 0;
   return ++size;
}


// GetMaxValueSize
DWORD GetMaxValueSize(HKEY key)
{
   DWORD size;
   LONG  result;

   result = RegQueryInfoKey(
      key, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &size, NULL, NULL);

   if (result != ERROR_SUCCESS)
      size = 0;
   return size;
}


// WriteBinary
bool WriteBinary(HKEY key, const char* valueName, const BYTE* value, DWORD size)
{
   LONG result;

   result = RegSetValueExA(key, valueName, NULL, REG_BINARY, value, size);

   return result == ERROR_SUCCESS;
}


// ReadBinary
bool ReadBinary(HKEY key, const char* valueName, BYTE* value, DWORD& size)
{
   LONG result;
   DWORD type;

   result = RegQueryValueExA(key, valueName, NULL, &type, value, &size);
   return result == ERROR_SUCCESS && type == REG_BINARY;
}


// WriteDword
bool WriteDword(HKEY key, const char* valueName, DWORD value)
{
   LONG result;

   result = RegSetValueExA( 
         key, valueName, NULL, REG_DWORD, (BYTE*)(&value), sizeof(DWORD)
      );

   return result == ERROR_SUCCESS;
}


// ReadDword
bool ReadDword(HKEY key, const char* valueName, DWORD& value)
{
   LONG result;
   DWORD size = sizeof(DWORD);
   DWORD type;

   result = RegQueryValueExA(key, valueName, NULL, &type, (BYTE *)(&value), &size);
   return result == ERROR_SUCCESS && type == REG_DWORD;
}


// WriteString
bool WriteString(HKEY key, const char* valueName, const std::string& str)
{
   LONG result;

   result = RegSetValueExA( 
                  key, valueName, NULL, REG_SZ, (const BYTE*) str.c_str(), 
                  (DWORD) str.length() + 1
            );

  return result == ERROR_SUCCESS;
}


// ReadString
bool ReadString(HKEY key, const char* valueName,  std::string& str)
{
   bool  result;
   DWORD type, size;
   char* buf = NULL;


   result = GetValueInfo(key, valueName, type, size);

   if ( !result || (type != REG_SZ && type != REG_EXPAND_SZ) )
     return false;

   // If type is valid, get the value from registry.
   buf = new char[size];
   buf[0] = '\0';
   result = RegQueryValueExA( 
      key, valueName, NULL, &type, (LPBYTE)buf, &size ) == ERROR_SUCCESS;

   str = buf;
   delete[] buf;

   return result;
}



// DeleteValue
bool DeleteValue(HKEY key, const char* valueName)
{
   LONG result;

   result = RegDeleteValueA(key, valueName);

   return result == ERROR_SUCCESS;
}


// SaveKey
bool SaveKey(HKEY key, const char* file)
{
   LONG result;

   result = RegSaveKeyA(key, file, NULL);

   return result == ERROR_SUCCESS;
}


// LoadRegFile
bool LoadRegFile(const char* filename)
{
   char shortName[256];
   HINSTANCE hinst;
   std::string param = " /s " ;

   if (GetShortPathNameA(filename, shortName, 256) == 0)
      return false;

   param += shortName;

   hinst = ::ShellExecuteA(NULL, "open", "regedit.exe", param.c_str(), NULL,SW_SHOWNORMAL);


   #if defined(__BORLANDC__) && __BORLANDC__ <= 0x0600
      return (long) hinst > 32;
   #else
      return (long long) hinst > 32;
   #endif
}


} //namespace WinRegistry


} // namespace dali