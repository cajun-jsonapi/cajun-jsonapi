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
   class InputStream;

   enum TokenType;
   struct Token;
   typedef std::vector<Token> Tokens;
   class TokenStream;

   // scanning istream into token sequence
   void Scan(Tokens& tokens, InputStream& inputStream);

   void EatWhiteSpace(InputStream& inputStream);
   void MatchComment(std::string& sComment, InputStream& inputStream);
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

   const std::string& MatchExpectedToken(TokenType nExpected, TokenStream& tokenStream);
};


} // End namespace