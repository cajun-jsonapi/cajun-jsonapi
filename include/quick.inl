/**********************************************

License: BSD
Project Webpage: http://cajun-jsonapi.sourceforge.net/
Author: Terry Caton, tcaton(a)hotmail.com

TODO: additional documentation. 

***********************************************/

#include "cast.h"
#include "exception.h"

namespace json
{


template <typename ElementTypeT>
QuickInterpreter::operator const ElementTypeT& () const {
   return json_cast<const ElementTypeT&>(m_Element); 
}

template <typename ElementTypeT>
QuickBuilder::operator ElementTypeT& () {
   return json_cast<ElementTypeT&>(m_Element); 
}

template <typename ElementTypeT>
ElementTypeT& QuickBuilder::Convert() {
  // we want an ElementTypeT. make it one if it's not.
  try {
     return *this;
  }
  catch (Exception&) {
     m_Element = ElementTypeT();
     return *this;
  }
}
 

} // End namespace
