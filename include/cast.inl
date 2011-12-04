/**********************************************

License: BSD
Project Webpage: http://cajun-jsonapi.sourceforge.net/
Author: Terry Caton, tcaton(a)hotmail.com

TODO: additional documentation. 

***********************************************/

#include "exception.h"

namespace json
{


// just a declaration, specializations below
template <typename ElementT>
class ElementCastImp;

// reference to non-const
template <typename ElementT>
class ElementCastImp<ElementT&>
{
public:
   static ElementT& cast_i(Element& element) {
      if (element.Type() != ElementT::Type_i())
         throw Exception("Bad json_cast");
      return static_cast<ElementT&>(element);
   }
};

template <typename ElementRefT>
ElementRefT json_cast(Element& element) {
   return ElementCastImp<ElementRefT>::cast_i(element);   
}


// reference to const
template <typename ElementT>
class ElementCastImp<const ElementT&>
{
public:
   static const ElementT& cast_i(const Element& element) {
      if (element.Type() != ElementT::Type_i())
         throw Exception("Bad json_cast");
      return static_cast<const ElementT&>(element);
   }
};

template <typename ConstElementRefT>
ConstElementRefT json_cast(const Element& element) {
   return ElementCastImp<ConstElementRefT>::cast_i(element);   
}


// pointer to non-const
template <typename ElementT>
class ElementCastImp<ElementT*>
{
public:
   static ElementT* cast_i(Element* pElement) {
      return (pElement->Type() == ElementT::Type_i() ? static_cast<ElementT*>(pElement) :
                                                       0);
   }
};

template <typename ElementPtrT>
ElementPtrT json_cast(Element* pElement) {
   return ElementCastImp<ElementPtrT>::cast_i(pElement);   
}


// pointer to const
template <typename ElementT>
class ElementCastImp<const ElementT*>
{
public:
   static ElementT* cast_i(const Element* pElement) {
      return (pElement->Type() == ElementT::Type_i() ? static_cast<const ElementT*>(pElement) :
                                                       0);
   }
};

template <typename ConstElementTypePtrT>
ConstElementTypePtrT json_cast(const Element* pElement) {
   return ElementCastImp<ConstElementTypePtrT>::cast_i(pElement);   
}


} // End namespace
