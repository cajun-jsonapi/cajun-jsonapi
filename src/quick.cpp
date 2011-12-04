/**********************************************

License: BSD
Project Webpage: http://cajun-jsonapi.sourceforge.net/
Author: Terry Caton, tcaton(a)hotmail.com

TODO: additional documentation. 

***********************************************/

#include "quick.h"
#include "cast.h"
#include "exception.h"


namespace json
{

///////////////////////////
// QuickInterpreter members

QuickInterpreter::QuickInterpreter(const Element& element) :
  m_Element(element) {}

QuickInterpreter QuickInterpreter::operator[] (const std::string& key) const {
  const Object& obj = *this;
  return obj[key];
}

size_t QuickInterpreter::Size() const { 
  const Array& array = *this;
  return array.Size();
}

QuickInterpreter QuickInterpreter::operator[] (size_t index) const {
  const Array& array = *this;
  return array[index];
}



////////////////////////
// QuickBuilder members

QuickBuilder::QuickBuilder(Element& element) :
  m_Element(element) {}

QuickBuilder QuickBuilder::operator[] (const std::string& key) {
  Object& obj = Convert<Object>();
  return obj[key];
}

size_t QuickBuilder::Size() { 
  Array& array = *this;
  return array.Size();
}

QuickBuilder QuickBuilder::operator[] (size_t index) {
   // how should we handle out of bounds? error? operator[string] implicitly 
   //  creates a new key/value. we'll implicitly resize
  Array& array = Convert<Array>();
  size_t nMinSize = index + 1; // zero indexed
  if (array.Size() < nMinSize)
     array.Resize(nMinSize);
  return array[index];
}

QuickBuilder& QuickBuilder::operator = (const Element& element) {
  m_Element = element;
  return *this;
}


} // End namespace
