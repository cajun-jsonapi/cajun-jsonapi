/**********************************************

License: BSD
Project Webpage: http://cajun-jsonapi.sourceforge.net/
Author: Terry Caton, tcaton(a)hotmail.com

TODO: additional documentation. See class headers for additional
info.

***********************************************/

#pragma once

#include <vector>
#include <list>
#include <string>


/*  

TODO:
* better documentation (doxygen?)
* Unicode support
* parent element accessors

*/

namespace json
{

class Visitor;
class ConstVisitor;


enum ElementType
{
   OBJECT_ELEMENT,
   ARRAY_ELEMENT,
   NUMBER_ELEMENT,
   STRING_ELEMENT,
   BOOLEAN_ELEMENT,
   NULL_ELEMENT
};


/////////////////////////////////////////////////////////////////////////
// base class - provides little useful in client code except constructor, 
//  visitor-related functions & equality operator

class ElementImp;
class Element
{
public:
   // note: nothing virtual in here! and only one (1) pointer member
   Element();
   Element(const Element& element);
   ~Element();

   Element& operator= (const Element& element);

   ElementType Type() const;

   // const & non-const visitor interfaces
   void Accept(Visitor& visitor);
   void Accept(ConstVisitor& visitor) const;

   bool operator == (const Element& element) const;

protected:
   Element(ElementImp* elementImp);

   ElementImp* m_pElementImp;
};


///////////////////////////////////////////////////////////////////////////////
// template base class for all "concrete" element types. employs "curiously 
//  recursive template pattern". also provides little interface used in client 
//  code 

template <typename ElementImpTypeT>
class Element_T : public Element
{
public:
   static ElementType Type_i();

protected:
   Element_T();

   void SanityCheck() const;

   ElementImpTypeT& Imp();
   const ElementImpTypeT& Imp() const;
};


/////////////////////////////////////////////////////////////////////////////////
// Array - mimics std::vector, except the array contents are effectively 
//  heterogeneous thanks to the Element base class. push_back has been replaced
//  by more generic insert functions

class ArrayImp;
class Array : public Element_T<ArrayImp>
{
public:
   typedef std::vector<Element> Elements;
   typedef Elements::iterator iterator;
   typedef Elements::const_iterator const_iterator;

   Array(); // necessary, or we get linker errors... ?

   iterator Begin();
   iterator End();
   const_iterator Begin() const;
   const_iterator End() const;

   size_t Size() const;
   bool Empty() const;

   void Resize(size_t newSize);

   iterator Insert(const Element& element);
   iterator Insert(const Element& element, iterator itWhere);
   iterator Erase(iterator itWhere);

   Element& operator [] (size_t index);
   const Element& operator [] (size_t index) const;

   void Clear();
};


/////////////////////////////////////////////////////////////////////////////////
// Object - mimics std::map<std::string, Element>, except the member value 
//  contents are effectively heterogeneous thanks to the Element base class

class ObjectImp;
class Object : public Element_T<ObjectImp>
{
public:
   struct Member {
      Member(const std::string& nameIn = std::string(), const Element& elementIn = Element());

      bool operator == (const Member& member) const;

      std::string name;
      Element element;
   };

   typedef std::list<Member> Members; // map faster, but does not preserve order
   typedef Members::iterator iterator;
   typedef Members::const_iterator const_iterator;

   Object(); // necessary, or we get linker errors... ?

   iterator Begin();
   iterator End();
   const_iterator Begin() const;
   const_iterator End() const;

   size_t Size() const;
   bool Empty() const;

   iterator Find(const std::string& name);
   const_iterator Find(const std::string& name) const;

   iterator Insert(const Member& member);
   iterator Insert(const Member& member, iterator itWhere);
   iterator Erase(iterator itWhere);

   Element& operator [] (const std::string& name);
   const Element& operator [] (const std::string& name) const;

   void Clear();
};



template <typename DataTypeT, ElementType TYPE>
class TrivialImpType_T;

template <typename DataTypeT, ElementType TYPE>
class TrivialType_T : public Element_T<TrivialImpType_T<DataTypeT, TYPE> >
{
public:
   typedef DataTypeT DataType;
   typedef TrivialType_T<DataTypeT, TYPE> ThisType;

   TrivialType_T(const DataTypeT& t = DataTypeT());

   operator const DataTypeT&() const;
   operator DataTypeT&();
};

typedef TrivialType_T<double, NUMBER_ELEMENT> Number;
typedef TrivialType_T<bool, BOOLEAN_ELEMENT> Boolean;
typedef TrivialType_T<std::string, STRING_ELEMENT> String;



class NullImp;
class Null : public Element_T<NullImp>
{};




} // End namespace
