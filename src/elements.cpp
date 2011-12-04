/**********************************************

License: BSD
Project Webpage: http://cajun-jsonapi.sourceforge.net/
Author: Terry Caton, tcaton(a)hotmail.com

TODO: additional documentation. 

***********************************************/

#include "elements.h"
#include "visitor.h"
#include "exception.h"
#include <cassert>
#include <algorithm>

/*  

TODO:
* better documentation

*/

namespace json
{

//////////////////
// Element members

class ElementImp
{
public:
   virtual ~ElementImp() {}

   virtual ElementImp* Clone() const = 0;
   virtual ElementType Type() const = 0;

   virtual void Accept(const Element& element, ConstVisitor& visitor) const = 0;
   virtual void Accept(Element& element, Visitor& visitor) = 0;

   virtual bool operator == (const ElementImp& elementImp) const = 0;
};


Element::Element() :
   m_pElementImp(0) {
   *this = Null(); // every element is a Null by default
}

Element::Element(const Element& element) :
   m_pElementImp(element.m_pElementImp->Clone())
{}

Element::~Element() {
   delete m_pElementImp; 
}

Element& Element::operator= (const Element& element) {
   delete m_pElementImp;
   m_pElementImp = element.m_pElementImp->Clone();
   return *this; 
}

ElementType Element::Type() const {
   return m_pElementImp->Type(); 
}

Element::Element(ElementImp* elementImp) : 
   m_pElementImp(elementImp)
{}

void Element::Accept(ConstVisitor& visitor) const {
   m_pElementImp->Accept(*this, visitor);
}

void Element::Accept(Visitor& visitor) {
   m_pElementImp->Accept(*this, visitor);
}

bool Element::operator == (const Element& element) const {
   return *m_pElementImp == *element.m_pElementImp;
}



////////////////////
// Element_T members


template <typename ElementTypeT, typename ElementImpTypeT, ElementType TYPE>
class ElementImp_T : public ElementImp
{
public:
   typedef ElementTypeT RealTypeT;
   typedef ElementImpTypeT RealImpTypeT;

   static ElementType Type_i() {
      return TYPE;
   }

   virtual void Accept(const Element& element, ConstVisitor& visitor) const {
      const RealTypeT& realElement = static_cast<const RealTypeT&>(element);
      visitor.Visit(realElement);
   }

   virtual void Accept(Element& element, Visitor& visitor) {
      RealTypeT& realElement = static_cast<RealTypeT&>(element);
      visitor.Visit(realElement);
   }

   // covariant return type does nothing for us here, and we can't do
   //  a real one anyway due to ElementTypeT not being defined yet
   virtual ElementImp* Clone() const { 
      const RealImpTypeT* pRealImpType = static_cast<const RealImpTypeT*>(this);
      return new RealImpTypeT(*pRealImpType);
   }

   virtual ElementType Type() const { return TYPE; }

   virtual bool operator == (const ElementImp& elementImp) const {
      bool bEquals = false;
      if (Type() == elementImp.Type()) {
         const RealImpTypeT& impThis = static_cast<const RealImpTypeT&>(*this);
         const RealImpTypeT& impRhs = static_cast<const RealImpTypeT&>(elementImp);

         bEquals = (impThis == impRhs);
      }

      return bEquals;
   }
};


template <typename ElementImpTypeT>
Element_T<ElementImpTypeT>::Element_T() :
   Element(new ElementImpTypeT()) {
   Type_i();
}

template <typename ElementImpTypeT>
ElementType Element_T<ElementImpTypeT>::Type_i() {
   return ElementImpTypeT::Type_i();
}

template <typename ElementImpTypeT>
void Element_T<ElementImpTypeT>::SanityCheck() const {
  if (Type_i() != Type())
     throw Exception("Type/ImpType mismatch");
}

template <typename ElementImpTypeT>
ElementImpTypeT& Element_T<ElementImpTypeT>::Imp() {
   SanityCheck();
   return static_cast<ElementImpTypeT&>(*m_pElementImp); 
}

template <typename ElementImpTypeT>
const ElementImpTypeT& Element_T<ElementImpTypeT>::Imp() const {
   SanityCheck();
   return static_cast<const ElementImpTypeT&>(*m_pElementImp); 
}


/////////////////
// Array members

class ArrayImp : public ElementImp_T<Array, ArrayImp, ARRAY_ELEMENT>
{
public:
   Array::iterator Begin() { return m_Elements.begin(); }
   Array::iterator End() { return m_Elements.end(); }
   Array::const_iterator Begin() const { return m_Elements.begin(); }
   Array::const_iterator End() const { return m_Elements.end(); }
   
   Array::iterator Insert(const Element& element, Array::iterator itWhere) { 
      return m_Elements.insert(itWhere, element);
   }

   Array::iterator Erase(Array::iterator itWhere) { 
      return m_Elements.erase(itWhere);
   }

   void Resize(size_t newSize) {
      m_Elements.resize(newSize);
   }

   size_t Size() const { return m_Elements.size(); }
   bool Empty() const { return m_Elements.empty(); }

   Element& operator[] (size_t index) {
      if (index >= m_Elements.size())
         throw Exception("Array out of bounds");
      return m_Elements[index]; 
   }

   const Element& operator[] (size_t index) const {
      if (index >= m_Elements.size())
         throw Exception("Array out of bounds");
      return m_Elements[index]; 
   }

   void Clear() {
      m_Elements.clear();
   }

   bool operator == (const ArrayImp& arrayImp) const {
      return m_Elements == arrayImp.m_Elements;
   }

private:
   Array::Elements m_Elements;
};


Array::Array() {}

Array::iterator Array::Begin() {
   return Imp().Begin(); 
}

Array::iterator Array::End() {
   return Imp().End(); 
}

Array::const_iterator Array::Begin() const {
   return Imp().Begin(); 
}

Array::const_iterator Array::End() const {
   return Imp().End(); 
}

Array::iterator Array::Insert(const Element& element) {
   return Imp().Insert(element, End()); 
}

Array::iterator Array::Insert(const Element& element, Array::iterator itWhere) {
   return Imp().Insert(element, itWhere); 
}

Array::iterator Array::Erase(Array::iterator itWhere) {
   return Imp().Erase(itWhere); 
}

void Array::Resize(size_t newSize) {
   Imp().Resize(newSize); 
}

size_t Array::Size() const {
   return Imp().Size(); 
}

bool Array::Empty() const {
   return Imp().Empty();
}

Element& Array::operator [] (size_t index) {
   return Imp()[index]; 
}

const Element& Array::operator [] (size_t index) const {
   return Imp()[index]; 
}

void Array::Clear() {
   Imp().Clear();
}


//////////////////
// Object members

class ObjectImp : public ElementImp_T<Object, ObjectImp, OBJECT_ELEMENT>
{
public:
   Object::iterator Begin() { return m_Members.begin(); }
   Object::iterator End() { return m_Members.end(); }
   Object::const_iterator Begin() const { return m_Members.begin(); }
   Object::const_iterator End() const { return m_Members.end(); }

   size_t Size() const { return m_Members.size(); }
   bool Empty() const { return m_Members.empty(); }

   Object::iterator Find(const std::string& name) {
      return std::find_if(m_Members.begin(), m_Members.end(), Finder(name));
   }

   Object::const_iterator Find(const std::string& name) const {
      return std::find_if(m_Members.begin(), m_Members.end(), Finder(name));
   }

   Object::iterator Insert(const Object::Member& member, Object::iterator itWhere) {
      Object::Members::iterator it = std::find_if(m_Members.begin(), m_Members.end(), Finder(member.name));
      if (it != m_Members.end())
         throw Exception("Object member already exists: " + member.name);

      return m_Members.insert(itWhere, member);
   }

   Object::iterator Erase(Object::iterator itWhere) {
      return m_Members.erase(itWhere);
   }

   Element& operator [](const std::string& name) { 
      Object::Members::iterator it = std::find_if(m_Members.begin(), m_Members.end(), Finder(name));
      if (it == m_Members.end())
      {
         it = m_Members.insert(m_Members.end(), Object::Member());
         it->name = name;
      }
      return it->element;      
   }

   const Element& operator [](const std::string& name) const {
      Object::Members::const_iterator it = std::find_if(m_Members.begin(), m_Members.end(), Finder(name));
      if (it == m_Members.end())
         throw Exception("Object member not found: " + name);
      return it->element;
   }

   void Clear() { m_Members.clear(); }

   bool operator == (const ObjectImp& objectImp) const {
      return m_Members == objectImp.m_Members;
   }

private:
   class Finder : public std::unary_function<Object::Member, bool>
   {
   public:
      Finder(const std::string& name) : m_name(name) {}
      bool operator () (const Object::Member& member) {
         return member.name == m_name;
      }

   private:
      std::string m_name;
   };

   Object::Members m_Members;
};


Object::Member::Member(const std::string& nameIn, const Element& elementIn) :
   name(nameIn), element(elementIn) {}

bool Object::Member::operator == (const Member& member) const {
   return name == member.name &&
          element == member.element;
}


Object::Object() {}

Object::iterator Object::Begin() {
   return Imp().Begin(); 
}

Object::iterator Object::End() {
   return Imp().End(); 
}

Object::const_iterator Object::Begin() const {
   return Imp().Begin(); 
}

Object::const_iterator Object::End() const {
   return Imp().End();
}

size_t Object::Size() const {
   return Imp().Size(); 
}

bool Object::Empty() const {
   return Imp().Empty(); 
}

Object::iterator Object::Find(const std::string& name) {
   return Imp().Find(name);
}

Object::const_iterator Object::Find(const std::string& name) const {
   return Imp().Find(name);
}

Object::iterator Object::Insert(const Member& member) {
   return Imp().Insert(member, End());
}

Object::iterator Object::Insert(const Member& member, Object::iterator itWhere) {
   return Imp().Insert(member, itWhere);
}

Element& Object::operator [] (const std::string& name) {
   return Imp()[name]; 
}

const Element& Object::operator [] (const std::string& name) const {
   return Imp()[name]; 
}

void Object::Clear() {
   Imp().Clear();
}

///////////////
// Null members


class NullImp : public ElementImp_T<Null, NullImp, NULL_ELEMENT>
{
public:
   bool operator == (const NullImp&) const {
      return true;
   }
};



////////////////////////
// TrivialType_T members

template <typename DataTypeT, ElementType TYPE>
class TrivialImpType_T : public ElementImp_T<TrivialType_T<DataTypeT, TYPE>, TrivialImpType_T<DataTypeT, TYPE>, TYPE>
{
public:
   operator DataTypeT&() { return m_tValue; }
   operator const DataTypeT&() const { return m_tValue; }

   bool operator == (const TrivialImpType_T& trivialImp) const {
      return m_tValue == trivialImp.m_tValue;
   }

private:
   DataTypeT m_tValue;
};


template <typename DataTypeT, ElementType TYPE>
TrivialType_T<DataTypeT, TYPE>::TrivialType_T(const DataTypeT& t)
{
   operator DataTypeT&() = t;
}


template <typename DataTypeT, ElementType TYPE>
TrivialType_T<DataTypeT, TYPE>::operator const DataTypeT&() const {
   return ThisType::Imp().operator const DataTypeT&();
}

template <typename DataTypeT, ElementType TYPE>
TrivialType_T<DataTypeT, TYPE>::operator DataTypeT&() {
   return ThisType::Imp().operator DataTypeT&();
}

// explicit template instantiations
template class TrivialType_T<double, NUMBER_ELEMENT>;
template class TrivialType_T<bool, BOOLEAN_ELEMENT>;
template class TrivialType_T<std::string, STRING_ELEMENT>;


} // End namespace
