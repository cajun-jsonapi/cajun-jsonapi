/******************************************************************************

License: BSD
Project Webpage: http://cajun-jsonapi.sourceforge.net/
Author: Terry Caton, tcaton(a)hotmail.com

classes: QuickInterpreter
         QuickBuilder

The classes in this file provide an alternative interface for very quickly 
extracting or  compiling data into a JSON document structure. They are useful 
when the structure of a document is rigid and well-known, which is often the 
case. 

QuickInterpreter allows quick, read-only access to an existing document 
structure. For examples, given the document...

{
   "XYZ" : {
         "ABC" : [ 1 ]
   }
}

QuickInterpreter interpreter(elemRoot); // elemRoot containing above structure
const Object& object = interpreter["XYZ"];
const Array& array = interpreter["XYZ"]["ABC"];
const Number& num = interpreter["XYZ"]["ABC"][0];

QuickBuilder allows building the above structure with one line of code:

QuickBuilder builder(elemRoot); // elemRoot being an empty Element
builder["XYZ"][ABC][0] = Number(1);

******************************************************************************/

#pragma once

#include "elements.h"

namespace json
{


class QuickInterpreter
{
public:
   QuickInterpreter(const Element& element);

      // object member access
   QuickInterpreter operator[] (const std::string& key) const;

   // array element access
   size_t Size() const;
   QuickInterpreter operator[] (size_t index) const;

   // implicit (safe) cast to any element
   template <typename ElementTypeT>
   operator const ElementTypeT& () const;

private:
   const Element& m_Element;
};


class QuickBuilder
{
public:
   QuickBuilder(Element& element);

   // object element access
   QuickBuilder operator[] (const std::string& key);

   // array element access
   size_t Size();
   QuickBuilder operator[] (size_t index);

   // implicit (safe) cast to any element
   QuickBuilder& operator = (const Element& element);

   template <typename ElementTypeT>
   operator ElementTypeT& ();

   template <typename ElementTypeT>
   ElementTypeT& Convert();

private:
   Element& m_Element;
};


} // End namespace


#include "quick.inl"

