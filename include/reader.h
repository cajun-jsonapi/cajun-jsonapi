/**********************************************

License: BSD
Project Webpage: http://cajun-jsonapi.sourceforge.net/
Author: Terry Caton, tcaton(a)hotmail.com

TODO: additional documentation. 

***********************************************/

#pragma once

#include "elements.h"

namespace json
{

class Reader
{
public:
   struct Location
   {
      Location();

      unsigned int m_nLine;       // document line, zero-indexed
      unsigned int m_nLineOffset; // character offset from beginning of line, zero indexed
      unsigned int m_nDocOffset;  // character offset from entire document, zero indexed
   };

   static void Read(Element& elementRoot, std::istream& istr);

private:
   struct Token
   {
      enum Type
      {
         TOKEN_OBJECT_BEGIN,  //    {
         TOKEN_OBJECT_END,    //    }
         TOKEN_ARRAY_BEGIN,   //    [
         TOKEN_ARRAY_END,     //    ]
         TOKEN_NEXT_ELEMENT,  //    ,
         TOKEN_MEMBER_ASSIGN, //    :
         TOKEN_STRING,        //    "xxx"
         TOKEN_NUMBER,        //    [+/-]000.000[e[+/-]000]
         TOKEN_BOOLEAN,       //    true -or- false
         TOKEN_NULL           //    null
      };

      Type nType;
      std::string sValue;

      // for malformed file debugging
      Reader::Location locBegin;
      Reader::Location locEnd;
   };

   class InputStream;
   class TokenStream;
   typedef std::vector<Token> Tokens;

   // scanning istream into token sequence
   void Scan(Tokens& tokens, InputStream& inputStream);

   void EatWhiteSpace(InputStream& inputStream);
   void MatchString(std::string& sValue, InputStream& inputStream);
   void MatchNumber(std::string& sNumber, InputStream& inputStream);
   void MatchExpectedString(const std::string& sExpected, InputStream& inputStream);

   // parsing token sequence into element structure
   void Parse(Element& element, TokenStream& tokenStream);
   void Parse(Object& object, TokenStream& tokenStream);
   void Parse(Array& array, TokenStream& tokenStream);
   void Parse(String& string, TokenStream& tokenStream);
   void Parse(Number& number, TokenStream& tokenStream);
   void Parse(Boolean& boolean, TokenStream& tokenStream);
   void Parse(Null& null, TokenStream& tokenStream);

   const std::string& MatchExpectedToken(Token::Type nExpected, TokenStream& tokenStream);
};


} // End namespace
