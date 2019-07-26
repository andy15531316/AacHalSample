/*
--------------------------------------------------------------------------------
*Module: WinRegistry

*Abstract: WinRegistry is a class that help user to access windows registry.

*Note:

   Beacuase RegValueIterator uses RegEnumValueEx, do not use other registry 
   function when iterator is used.

*Author: Mark Lai, Tel:3390

*Date:   10/12/2005

*Motivation and Work:

*Last modification date:

--------------------------------------------------------------------------------
*/


#ifndef   __GS_WIN_REGISTRY_H__
#define   __GS_WIN_REGISTRY_H__


#include <string>
#include <vector>
#include <iterator>
#include <assert.h>
#include <windows.h>




//-----------------------------------------------------------------------------

namespace dali { 


namespace system { 




//-----------------------------------------------------------------------------


// Registry
namespace Registry
{
   bool  CreateKey(HKEY& key, HKEY root, const char* keyName, DWORD right);
   bool  OpenKey(HKEY& key, HKEY root, const char* keyName, DWORD right);
   bool  DeleteKey(HKEY root, const char* keyName, bool bSubKey = false);
   bool  KeyExists(HKEY root, const char* keyName);
   void  CloseKey(HKEY key);
   bool  FlushKey(HKEY key);

   // Return the count and maximum name size of subkeys in a specified key.
   bool  GetSubKeyInfo(HKEY key, DWORD* subkeyCount, DWORD* maxSubkeySize);

   // Return the count, maximum name size and maximum value size of values 
   // associated with a key.
   bool  GetKeyValueInfo(HKEY key, DWORD* keyValueCount, DWORD* maxValueNameSize, DWORD* maxValueSize);

   // Get the type and data size of a specified value
   bool  GetValueInfo(HKEY key, const char* valueName, DWORD* type, DWORD* size);

   // key value operations 

   bool  DeleteValue(HKEY key, const char* valueName);
 
   bool ReadBinary(HKEY key, const char* valueName, BYTE* value, DWORD& size);
   bool WriteBinary(HKEY key, const char* valueName, const BYTE* value, DWORD size);

   bool WriteDword(HKEY key, const char* valueName, DWORD value);
   bool ReadDword(HKEY key, const char* valueName, DWORD& value);

   bool ReadString(HKEY key, const char* valueName,  std::basic_string<char>& str);
   bool WriteString(HKEY key, const char* valueName, const std::basic_string<char>& str);
   bool WriteExpandString(HKEY key, const char* valueName, const std::basic_string<char>& str);


   //bool WriteMultiStrings(HKEY key, const char* valueName, const std::basic_string<char>& str);
   bool ReadMultiStrings(HKEY key, const char* valueName,  std::vector< std::basic_string<char> >& strs);

} // namespace Registry



} // namespace system


} // namespace dali


//-----------------------------------------------------------------------------

namespace dali {



namespace detail {
  class RegValueBase;
}

class RegSubKeyBase;
class RegEnumValueBase;
class RegValueIterator;
class RegValueEnumerater;
class RegKeyIterator;
class RegKeyEnumerater;





//-----------------------------------------------------------------------------

// RegValueInfo
class RegValueInfo
{
public:
   

   const std::string& Name(void) const;
   DWORD Type(void) const;
   const std::vector<BYTE>& Value(void) const;

private:
   
   friend class detail::RegValueBase;

   std::string        name;
   DWORD              type;
   std::vector<BYTE>  value; 
};


inline const std::string& RegValueInfo::Name(void) const
{
   return name;
}


inline DWORD RegValueInfo::Type(void) const
{
   return type;
}


inline const std::vector<BYTE>& RegValueInfo::Value(void) const
{
   return value;
}




namespace detail {


//-----------------------------------------------------------------------------


// RegValueBase
//...A base class to help client to enumerate all value in specified key.
class RegValueBase
{
public:

   RegValueBase(HKEY key);
   ~RegValueBase();

   bool GetCount(size_t& count) const;
   bool Get(size_t index, RegValueInfo& info) const;
 
private:
  
   // Inhibit the copy constructor and assign operator.
   RegValueBase(const RegValueBase& other);
   RegValueBase& operator = (const RegValueBase& other);

   void GetValueContent(std::vector<BYTE>& value) const;

   bool AllocRes(void); 
   void ReleaseRes(void);

   bool LoadInfo(DWORD& count, DWORD& maxNameSize) const;
   
private:

   HKEY  m_key;
   mutable char* m_nameBuf;
   mutable DWORD m_nameSize;
}; 





//-----------------------------------------------------------------------------

// RegEnumValueBase
//...A base class for RegValueIterator and RegValueEnumerator to enumerate 
//   key values.


class RegEnumValueBase
{
public:

   RegEnumValueBase(void);
   RegEnumValueBase(RegValueBase* m_base);

   bool  IsDone(void) const;
   void  Next(void);
   const RegValueInfo& Get(void) const;

protected:

   friend class ::dali::RegValueIterator;
   friend class ::dali::RegValueEnumerater;

   // Contructor for creating last iterator.
   RegEnumValueBase(RegValueBase* m_base, int);

   bool Equles(const RegEnumValueBase& other) const;

   RegValueBase* GetBase(void) const;

private:

   void GotoNext(void);

private:

   RegValueBase*  m_base;
   size_t         m_index;
   RegValueInfo   m_info;
};


// Get
inline const RegValueInfo& RegEnumValueBase::Get(void) const
{
   return m_info;
}


// GetBase
inline RegValueBase* RegEnumValueBase::GetBase(void) const
{
   return m_base;
}


// Equles
inline bool RegEnumValueBase::Equles(const RegEnumValueBase& other) const
{
   assert(m_base == other.m_base);

   return m_index == other.m_index;    
}



} // namespace detail





//-----------------------------------------------------------------------------

// RegValueIterator
//...The iterator used to travel the registry values.
class RegValueIterator: public std::iterator<std::input_iterator_tag, RegValueInfo> 
{
public:

   RegValueIterator(void);
  
   RegValueIterator& operator ++(void); 
   RegValueIterator  operator ++(int);

   bool operator ==(const RegValueIterator& other) const; 
   bool operator !=(const RegValueIterator& other) const; 

   const RegValueInfo&  operator *(void);
   const RegValueInfo* operator ->(void);

protected:

   friend  class RegValueSet;

   RegValueIterator(detail::RegValueBase* values);

   static RegValueIterator MakeLastIterator(detail::RegValueBase* values);

   void DoNext(void);

private:

   // Contructor for making last iterator.
   RegValueIterator(detail::RegValueBase* values, int);

private:

   detail::RegEnumValueBase m_base;
};


// operator == 
inline bool RegValueIterator::operator ==(const RegValueIterator& other) const
{
  return m_base.Equles(other.m_base); 
}


// operator !=
inline bool RegValueIterator::operator !=(const RegValueIterator& other) const
{
   return !m_base.Equles(other.m_base); 
}


// operator *
inline const RegValueInfo&  RegValueIterator::operator *(void)
{
   return m_base.Get();
}


// operator ->
inline const RegValueInfo* RegValueIterator::operator ->(void)
{
   return (&**this);
}






//-----------------------------------------------------------------------------

// RegValueSet
//...A class for accessing register value in a specified key.
//...NOTE:: The iterater is input iterator.
class RegValueSet
{
public:

   typedef RegValueInfo ValueType;
   typedef RegValueIterator Iterator; 

   RegValueSet(void);
   virtual ~RegValueSet(void) { }

   bool   IsEmpty(void) const;
   size_t Count(void) const;

   Iterator Begin(void);
   Iterator End(void);

   void Refresh(void);

protected:

   friend class RegAccessor;
   
   RegValueSet(detail::RegValueBase* base);

private:

   detail::RegValueBase* m_base;
};


// IsEmpty
inline bool RegValueSet::IsEmpty(void) const
{
   return Count() == 0;
}





//-----------------------------------------------------------------------------

// RegValueEnumerater
//...Enumerate all value in specified key.
class RegValueEnumerater
{
public:

   typedef RegValueInfo   DataType;

   RegValueEnumerater(void);
   RegValueEnumerater(HKEY key);
   RegValueEnumerater(const char* keyName);
   RegValueEnumerater(HKEY root, const char* keyName);
   virtual ~RegValueEnumerater();

   bool Open(HKEY key);
   bool Open(const char* keyName);
   bool Open(HKEY root, const char* keyName);
   bool IsOpen(void) const;
   void Close(void);
 
   bool  IsDone(void) const;         // Check wether all value have been visted
   void  Next(void);                 // Find next value. 
   const RegValueInfo& Get(void) const; 

protected:

   virtual void DoNext(void);

private:
  
   // Inhibit the copy constructor and assign operator.
   RegValueEnumerater(const RegValueEnumerater& other);
   RegValueEnumerater& operator = (const RegValueEnumerater& other);

private:

   detail::RegEnumValueBase*  m_base;
   HKEY               m_key;
   bool               m_releasingKeyNeeded;

}; // RegValueEnumerater


// IsOpen
inline bool RegValueEnumerater::IsOpen(void) const
{ 
   return m_base != NULL; 
}


// IsDone
inline bool RegValueEnumerater::IsDone(void) const
{ 
   assert( IsOpen() );

   return m_base->IsDone(); 
}
 

// Get
inline const RegValueInfo& RegValueEnumerater::Get(void) const
{
   assert( !IsDone() );

   return m_base->Get();
}




//-----------------------------------------------------------------------------


// RegEnumKeyBase
//...A base class for RegKeyIterator and RegKeyEnumerator to enumerate 
//   subkeys.


namespace detail {


class RegEnumKeyBase
{
public:

   RegEnumKeyBase(void);
   RegEnumKeyBase(RegSubKeyBase* m_base);

   bool  IsDone(void) const;
   void  Next(void);
   const std::string& Get(void) const;

protected:

   friend class ::dali::RegKeyIterator;
   friend class ::dali::RegKeyEnumerater;

   // Contructor for creating last iterator.
   RegEnumKeyBase(RegSubKeyBase* m_base, int);

   bool Equles(const RegEnumKeyBase& other) const;

   RegSubKeyBase* GetBase(void) const;

private:

   void GotoNext(void);

private:

   RegSubKeyBase*  m_base;
   size_t          m_index;
   std::string     m_keyName;
};


// Get
inline const std::string & RegEnumKeyBase::Get(void) const
{
   return m_keyName;
}


// GetBase
inline RegSubKeyBase* RegEnumKeyBase::GetBase(void) const
{
   return m_base;
}


// Equles
inline bool RegEnumKeyBase::Equles(const RegEnumKeyBase& other) const
{
   assert(m_base == other.m_base);

   return m_index == other.m_index;    
}



} // namespace detail





//-----------------------------------------------------------------------------

// RegKeyIterator
//...The iterator used to travel the registry keys.
class RegKeyIterator: public std::iterator<std::input_iterator_tag, std::string> 
{
public:

   RegKeyIterator(void);

   RegKeyIterator& operator ++(void); 
   RegKeyIterator  operator ++(int);

   bool operator ==(const RegKeyIterator& other) const; 
   bool operator !=(const RegKeyIterator& other) const; 

   const std::string&  operator *(void);
   const std::string* operator ->(void);

protected:

   friend  class RegKeySet;

   RegKeyIterator(RegSubKeyBase* keys);

   static RegKeyIterator MakeLastIterator(RegSubKeyBase* keys);

   void DoNext(void);

private:

   // Contructor for making last iterator.
   RegKeyIterator(RegSubKeyBase* values, int);

private:

   detail::RegEnumKeyBase m_base;
};


// operator == 
inline bool RegKeyIterator::operator ==(const RegKeyIterator& other) const
{
  return m_base.Equles(other.m_base); 
}


// operator !=
inline bool RegKeyIterator::operator !=(const RegKeyIterator& other) const
{
   return !m_base.Equles(other.m_base); 
}


// operator *
inline const std::string&  RegKeyIterator::operator *(void)
{
   return m_base.Get();
}


// operator ->
inline const std::string* RegKeyIterator::operator ->(void)
{
   return (&**this);
}






//-----------------------------------------------------------------------------

// RegKeySet
//...A class for accessing register subkeys in a specified key.
//...NOTE:: The iterater is input iterator.
class RegKeySet
{
public:

   typedef std::string ValueType;
   typedef RegKeyIterator Iterator; 

   RegKeySet(void);
   virtual ~RegKeySet(void) { }

   bool   IsEmpty(void) const;
   size_t Count(void) const;

   Iterator Begin(void);
   Iterator End(void);

   void Refresh(void);

protected:

   friend class RegAccessor;

   RegKeySet(RegSubKeyBase* base);

   RegSubKeyBase* Base(void) const;

private:

   RegSubKeyBase* m_base;
};


// IsEmpty
inline bool RegKeySet::IsEmpty(void) const
{
   return Count() == 0;
}


// Base
inline RegSubKeyBase* RegKeySet::Base(void) const
{
   return m_base;
}



//-----------------------------------------------------------------------------

// RegKeyEnumerater
//...Enumerate all subkeys in a specified key.
class RegKeyEnumerater
{
public:

   typedef std::string  DataType;

   RegKeyEnumerater(void);
   RegKeyEnumerater(HKEY key);
   RegKeyEnumerater(const char* keyName);
   RegKeyEnumerater(HKEY root, const char* keyName);
   ~RegKeyEnumerater();

   bool Open(HKEY key);
   bool Open(const char* keyName);
   bool Open(HKEY root, const char* keyName);
   bool IsOpen(void) const;
   void Close(void);
 
   bool  IsDone(void) const;         // Check wether all value have been visted
   void  Next(void);                 // Find next value. 
   const DataType& Get(void) const; 

protected:

   virtual void DoNext(void);

private:
  
   // Inhibit the copy constructor and assign operator.
   RegKeyEnumerater(const RegKeyEnumerater& other);
   RegKeyEnumerater& operator = (const RegKeyEnumerater& other);

private:

   detail::RegEnumKeyBase*  m_base;
   HKEY   m_key;
   bool   m_releasingKeyNeeded;
};


// IsOpen
inline bool RegKeyEnumerater::IsOpen(void) const
{ 
   return m_base != NULL; 
}


// IsDone
inline bool RegKeyEnumerater::IsDone(void) const
{ 
   assert( IsOpen() );

   return m_base->IsDone(); 
}
 

// Get
inline const std::string& RegKeyEnumerater::Get(void) const
{
   assert( !IsDone() );

   return m_base->Get();
}



//-----------------------------------------------------------------------------

// RegReader
//...Read a specified key.

class RegReader
{
public:

   RegReader(void);
   RegReader(HKEY key);
   RegReader(const char* keyName);
   RegReader(HKEY root, const char* keyName);
   
   ~RegReader();

   bool Open(HKEY key);
   bool Open(const char* keyName);
   bool Open(HKEY root, const char* keyName);
   bool IsOpen(void) const;
   void Close(void);


   // subkey operations
 
   bool  SubKeyExists(const char* keyName) const;
 

   // key value operations 

   bool ReadBinary(const char* valueName, BYTE* value, DWORD& size);
   bool ReadDword(const char* valueName, DWORD& value);
   bool ReadString(const char* valueName,  std::string& str);

   /*// enumerate keys and values
 
   RegKeySet    SubKeys(void) const;
   RegValueSet  Values(void) const;*/

protected:

   virtual REGSAM AccessRights(void) const;

private:

   // not assignable.
   RegReader(const RegReader& other);
   RegReader& operator =(const RegReader& other);

private:

   HKEY  m_key;
   bool  m_releasingKeyNeeded;

   RegSubKeyBase* m_keys;
   detail::RegValueBase*  m_values;
};





//-----------------------------------------------------------------------------

// RegAccessor
//...Access a specified key. If the key does not exist, create it automatically.

class RegAccessor
{
public:

   RegAccessor(void);
   RegAccessor(HKEY key);
   RegAccessor(const char* keyName);
   RegAccessor(HKEY root, const char* keyName);
   ~RegAccessor();

   bool Open(HKEY key);
   bool Open(const char* keyName, bool bCreate = false);
   bool Open(HKEY root, const char* keyName, bool bCreate = false);
   bool IsOpen(void) const;
   void Close(void);

   void Flush(void);


   // subkey operations
 
   bool  DeleteSubKey(const char* keyName, bool bSubKey = false);
   bool  SubKeyExists(const char* keyName) const;
 
   bool CreateSubKey(RegAccessor& key, const char* keyName);
   bool CreateSubKey(HKEY& key, const char* keyName, DWORD right);
   bool OpenSubKey(RegAccessor& key, const char* keyName);
   bool OpenSubKey(HKEY& key, const char* keyName, DWORD right);
   static void CloseKey(HKEY subkey);


   // key value operations 

   bool  DeleteValue(const char* valueName);
 
   bool ReadBinary(const char* valueName, BYTE* value, DWORD& size);
   bool WriteBinary(const char* valueName, const BYTE* value, DWORD size);

   bool WriteDword(const char* valueName, DWORD value);
   bool ReadDword(const char* valueName, DWORD& value);

   bool WriteString(const char* valueName, const std::string& str);
   bool ReadString(const char* valueName,  std::string& str);


   // enumerate keys and values
 
   RegKeySet    SubKeys(void) const;
   RegValueSet  Values(void) const;

private:

   // not assignable.
   RegAccessor(const RegAccessor& other);
   RegAccessor& operator =(const RegAccessor& other);

private:

   HKEY  m_key;
   bool  m_releasingKeyNeeded;

   RegSubKeyBase* m_keys;
   detail::RegValueBase*  m_values;
};






//-----------------------------------------------------------------------------


// WinRegistry
//...Access windows registry

namespace WinRegistry {


bool  CreateKey(HKEY& key, HKEY root, const char* keyName, DWORD right);
bool  OpenKey(HKEY& key, HKEY root, const char* keyName, DWORD right);
void  CloseKey(HKEY key);
bool  DeleteKey(HKEY root, const char* keyName, bool bSubKey = false);
bool  KeyExists(HKEY root, const char* keyName);
bool  FlushKey(HKEY key);


// Return the count and maximum name size of subkeys in a specified key.
bool  GetSubKeyInfo(HKEY key, DWORD& count, DWORD& size);
DWORD GetSubkeyCount(HKEY key);
DWORD GetSubkeySize(HKEY key);



bool  GetValueInfo(HKEY key, const char* valueName, DWORD& type, DWORD& size);
bool  GetValueType(HKEY key, const char* valueName, DWORD& type);
bool  GetValueSize(HKEY key, const char* valueName, DWORD& size);


// Return the count, maximum name size and maximum value size of values 
// associated with a key.
bool  GetKeyValueInfo(HKEY key, DWORD& count, DWORD& nameSize, DWORD& valueSize);
bool  GetKeyValueInfo(HKEY key, DWORD& count, DWORD& nameSize);

DWORD GetValueCount(HKEY key);
DWORD GetMaxValueNameSize(HKEY key);
DWORD GetMaxValueSize(HKEY key);

bool ReadBinary(HKEY key, const char* valueName, BYTE* value, DWORD& size);
bool WriteBinary(HKEY key, const char* valueName, const BYTE* value, DWORD size);

bool WriteDword(HKEY key, const char* valueName, DWORD value);
bool ReadDword(HKEY key, const char* valueName, DWORD& value);

bool WriteString(HKEY key, const char* valueName, const std::string& str);
bool ReadString(HKEY key, const char* valueName,  std::string& str);

bool DeleteValue(HKEY key, const char* valueName);

bool SaveKey(HKEY key, const char* file);

bool LoadRegFile(const char* file);


}  // namespace WinRegistry


} // namespace dali


#endif  //__GS_WIN_REGISTRY_H__