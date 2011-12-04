/**********************************************

License: BSD
Project Webpage: http://cajun-jsonapi.sourceforge.net/
Author: Terry Caton, tcaton(a)hotmail.com

TODO: additional documentation. 

***********************************************/

#include "reader.h"
#include "exception.h"
#include "cast.h"
#include <cassert>
#include <set>
#include <sstream>

/*  

TODO:
* better documentation
* unicode character decoding

*/

namespace json
{

std::istream& operator >> (std::istream& istr, Element& elementRoot) {
   Reader::Read(elementRoot, istr);
   return istr;
}

Reader::Location::Location() :
   m_nLine(0),
   m_nLineOffset(0),
   m_nDocOffset(0)
{}


//////////////////////
// Reader::InputStream

class Reader::InputStream // would be cool if we could inherit from std::istream & override "get"
{
public:
   InputStream(std::istream& iStr) :
      m_iStr(iStr) {}

   // protect access to the input stream, so we can keeep track of document/line offsets
   char Get(); // big, define outside
   char Peek() {
      assert(m_iStr.eof() == false); // enforce reading of only valid stream data 
      return m_iStr.peek();
   }

   bool EOS() {
      m_iStr.peek(); // apparently eof flag isn't set until a character read is attempted. whatever.
      return m_iStr.eof();
   }

   const Location& GetLocation() const { return m_Location; }

private:
   std::istream& m_iStr;
   Location m_Location;
};


char Reader::InputStream::Get()
{
   assert(m_iStr.eof() == false); // enforce reading of only valid stream data 
   char c = m_iStr.get();
   
   ++m_Location.m_nDocOffset;
   if (c == '\n')
   {
      ++m_Location.m_nLine;
      m_Location.m_nLineOffset = 0;
   }
   else
   {
      ++m_Location.m_nLineOffset;
   }

   return c;
}



//////////////////////
// Reader::TokenStream

class Reader::TokenStream
{
public:
   TokenStream(const Tokens& tokens);

   const Token& Peek();
   const Token& Get();

   bool EOS() const;

private:
   const Tokens& m_Tokens;
   Tokens::const_iterator m_itCurrent;
};


Reader::TokenStream::TokenStream(const Tokens& tokens) :
   m_Tokens(tokens),
   m_itCurrent(tokens.begin())
{}

const Reader::Token& Reader::TokenStream::Peek() {
   assert(m_itCurrent != m_Tokens.end());
   return *(m_itCurrent); 
}

const Reader::Token& Reader::TokenStream::Get() {
   assert(m_itCurrent != m_Tokens.end());
   return *(m_itCurrent++); 
}

bool Reader::TokenStream::EOS() const {
   return m_itCurrent == m_Tokens.end(); 
}

///////////////////
// Reader (finally)

void Reader::Read(Element& elementRoot, std::istream& istr)
{
   Reader reader;

   Tokens tokens;
   InputStream inputStream(istr);
   reader.Scan(tokens, inputStream);

   TokenStream tokenStream(tokens);
   reader.Parse(elementRoot, tokenStream);

   if (tokenStream.EOS() == false)
   {
      const Token& token = tokenStream.Peek();
      std::string sMessage = "Expected End of token stream; found " + token.sValue;
      throw ParseException(sMessage, token.locBegin, token.locEnd);
   }
}


void Reader::Scan(Tokens& tokens, InputStream& inputStream)
{
   while (EatWhiteSpace(inputStream),              // ignore any leading white space...
          inputStream.EOS() == false) // ...before checking for EOS
   {
      // if all goes well, we'll create a token each pass
      Token token;
      token.locBegin = inputStream.GetLocation();

      // gives us null-terminated string
      std::string sChar;
      sChar.push_back(inputStream.Peek());

      switch (sChar[0])
      {
         case '{':
            MatchExpectedString(sChar, inputStream);
            token.nType = Token::TOKEN_OBJECT_BEGIN;
            break;

         case '}':
            MatchExpectedString(sChar, inputStream);
            token.nType = Token::TOKEN_OBJECT_END;
            break;

         case '[':
            MatchExpectedString(sChar, inputStream);
            token.nType = Token::TOKEN_ARRAY_BEGIN;
            break;

         case ']':
            MatchExpectedString(sChar, inputStream);
            token.nType = Token::TOKEN_ARRAY_END;
            break;

         case ',':
            MatchExpectedString(sChar, inputStream);
            token.nType = Token::TOKEN_NEXT_ELEMENT;
            break;

         case ':':
            MatchExpectedString(sChar, inputStream);
            token.nType = Token::TOKEN_MEMBER_ASSIGN;
            break;

         case '"':
            MatchString(token.sValue, inputStream);
            token.nType = Token::TOKEN_STRING;
            break;

         case '-':
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            MatchNumber(token.sValue,inputStream);
            token.nType = Token::TOKEN_NUMBER;
            break;

         case 't':
            token.sValue = "true";
            MatchExpectedString(token.sValue, inputStream);
            token.nType = Token::TOKEN_BOOLEAN;
            break;

         case 'f':
            token.sValue = "false";
            MatchExpectedString(token.sValue, inputStream);
            token.nType = Token::TOKEN_BOOLEAN;
            break;

         case 'n':
            token.sValue = "null";
            MatchExpectedString(token.sValue, inputStream);
            token.nType = Token::TOKEN_NULL;
            break;

         default:
         {
            std::string sErrorMessage = "Unexpected character in stream: " + sChar;
            throw ScanException(sErrorMessage, inputStream.GetLocation());
         }
      }

      token.locEnd = inputStream.GetLocation();
      tokens.push_back(token);
   }
}


void Reader::EatWhiteSpace(InputStream& inputStream)
{
   while (inputStream.EOS() == false && 
          ::isspace(inputStream.Peek()))
      inputStream.Get();
}

void Reader::MatchExpectedString(const std::string& sExpected, InputStream& inputStream)
{
   std::string::const_iterator it(sExpected.begin()),
                               itEnd(sExpected.end());
   for ( ; it != itEnd; ++it)
   {
      if (inputStream.EOS() ||      // did we reach the end before finding what we're looking for...
          inputStream.Get() != *it) // ...or did we find something different?
      {
         std::string sMessage = "Expected string: " + sExpected;
         throw ScanException(sMessage, inputStream.GetLocation());
      }
   }

   // all's well if we made it here, return quietly
}


void Reader::MatchString(std::string& string, InputStream& inputStream)
{
   MatchExpectedString("\"", inputStream);
   
   while (inputStream.EOS() == false &&
          inputStream.Peek() != '"')
   {
      char c = inputStream.Get();

      // escape?
      if (c == '\\' &&
          inputStream.EOS() == false) // shouldn't have reached the end yet
      {
         c = inputStream.Get();
         switch (c)
         {
            case '/':      string.push_back('/');     break;
            case '"':      string.push_back('"');     break;
            case '\\':     string.push_back('\\');    break;
            case 'b':      string.push_back('\b');    break;
            case 'f':      string.push_back('\f');    break;
            case 'n':      string.push_back('\n');    break;
            case 'r':      string.push_back('\r');    break;
            case 't':      string.push_back('\t');    break;
            case 'u':      // TODO: what do we do with this?
            default:
            {
               std::string sMessage = "Unrecognized escape sequence found in string: \\" + c;
               throw ScanException(sMessage, inputStream.GetLocation());
            }
         }
      }
      else
      {
         string.push_back(c);
      }
   }

   // eat the last '"' that we just peeked
   MatchExpectedString("\"", inputStream);
}


void Reader::MatchNumber(std::string& sNumber, InputStream& inputStream)
{
   const char sNumericChars[] = "0123456789.eE-+";
   std::set<char> numericChars;
   numericChars.insert(sNumericChars, sNumericChars + sizeof(sNumericChars));

   while (inputStream.EOS() == false &&
          numericChars.find(inputStream.Peek()) != numericChars.end())
   {
      sNumber.push_back(inputStream.Get());   
   }
}


void Reader::Parse(Element& element, Reader::TokenStream& tokenStream)
{
   if (tokenStream.EOS())
   {
      std::string sMessage = "Unexpected end of token stream";
      throw ParseException(sMessage, Location(), Location()); // nowhere to point to
   }

   const Token& token = tokenStream.Peek();
   switch (token.nType)
   {
      case Token::TOKEN_OBJECT_BEGIN:
         element = Object();
         Parse(json_cast<Object&>(element), tokenStream);
         break;

      case Token::TOKEN_ARRAY_BEGIN:
         element = Array();
         Parse(json_cast<Array&>(element), tokenStream);
         break;

      case Token::TOKEN_STRING:
         element = String();
         Parse(json_cast<String&>(element), tokenStream);
         break;

      case Token::TOKEN_NUMBER:
         element = Number();
         Parse(json_cast<Number&>(element), tokenStream);
         break;

      case Token::TOKEN_BOOLEAN:
         element = Boolean();
         Parse(json_cast<Boolean&>(element), tokenStream);
         break;

      case Token::TOKEN_NULL:
         element = Null();
         Parse(json_cast<Null&>(element), tokenStream);
         break;

      default:
      {
         std::string sMessage = "Unexpected token: " + token.sValue;
         throw ParseException(sMessage, token.locBegin, token.locEnd);
      }
   }
}


void Reader::Parse(Object& object, Reader::TokenStream& tokenStream)
{
   MatchExpectedToken(Token::TOKEN_OBJECT_BEGIN, tokenStream);

   bool bContinue = (tokenStream.EOS() == false &&
                     tokenStream.Peek().nType != Token::TOKEN_OBJECT_END);
   while (bContinue)
   {
      Object::Member member;

      // first the member name. save the token in case we have to throw an exception
      const Token& tokenName = tokenStream.Peek();
      member.name = MatchExpectedToken(Token::TOKEN_STRING, tokenStream);

      // ...then the key/value separator...
      MatchExpectedToken(Token::TOKEN_MEMBER_ASSIGN, tokenStream);

      // ...then the value itself (can be anything).
      Parse(member.element, tokenStream);

      // try adding it to the object (this could throw)
      try
      {
         object.Insert(member);
      }
      catch (Exception&)
      {
         // must be a duplicate name
         std::string sMessage = "Duplicate object member token: " + member.name; 
         throw ParseException(sMessage, tokenName.locBegin, tokenName.locEnd);
      }

      bContinue = (tokenStream.EOS() == false &&
                   tokenStream.Peek().nType == Token::TOKEN_NEXT_ELEMENT);
      if (bContinue)
         MatchExpectedToken(Token::TOKEN_NEXT_ELEMENT, tokenStream);
   }

   MatchExpectedToken(Token::TOKEN_OBJECT_END, tokenStream);
}


void Reader::Parse(Array& array, Reader::TokenStream& tokenStream)
{
   MatchExpectedToken(Token::TOKEN_ARRAY_BEGIN, tokenStream);

   bool bContinue = (tokenStream.EOS() == false &&
                     tokenStream.Peek().nType != Token::TOKEN_ARRAY_END);
   while (bContinue)
   {
      // ...what's next? could be anything
      Array::iterator itElement = array.Insert(Element());
      Parse(*itElement, tokenStream);

      bContinue = (tokenStream.EOS() == false &&
                   tokenStream.Peek().nType == Token::TOKEN_NEXT_ELEMENT);
      if (bContinue)
         MatchExpectedToken(Token::TOKEN_NEXT_ELEMENT, tokenStream);
   }

   MatchExpectedToken(Token::TOKEN_ARRAY_END, tokenStream);
}


void Reader::Parse(String& string, Reader::TokenStream& tokenStream)
{
   string = MatchExpectedToken(Token::TOKEN_STRING, tokenStream);
}


void Reader::Parse(Number& number, Reader::TokenStream& tokenStream)
{
   const Token& currentToken = tokenStream.Peek(); // might need this later for throwing exception
   const std::string& sValue = MatchExpectedToken(Token::TOKEN_NUMBER, tokenStream);

   std::istringstream iStr(sValue);
   double dValue;
   iStr >> dValue;

   // did we consume all characters in the token?
   if (iStr.eof() == false)
   {
      std::string sMessage = "Unexpected character in NUMBER token: " + iStr.peek();
      throw ParseException(sMessage, currentToken.locBegin, currentToken.locEnd);
   }

   number = dValue;
}


void Reader::Parse(Boolean& boolean, Reader::TokenStream& tokenStream)
{
   const std::string& sValue = MatchExpectedToken(Token::TOKEN_BOOLEAN, tokenStream);
   boolean = (sValue == "true" ? true : false);
}


void Reader::Parse(Null&, Reader::TokenStream& tokenStream)
{
   MatchExpectedToken(Token::TOKEN_NULL, tokenStream);
}


const std::string& Reader::MatchExpectedToken(Token::Type nExpected, Reader::TokenStream& tokenStream)
{
   if (tokenStream.EOS())
   {
      std::string sMessage = "Unexpected End of token stream";
      throw ParseException(sMessage, Location(), Location()); // nowhere to point to
   }

   const Token& token = tokenStream.Get();
   if (token.nType != nExpected)
   {
      std::string sMessage = "Unexpected token: " + token.sValue;
      throw ParseException(sMessage, token.locBegin, token.locEnd);
   }

   return token.sValue;
}

} // End namespace
