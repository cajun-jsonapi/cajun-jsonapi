/******************************************************************************

Copyright (c) 2009-2010, Terry Caton
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the projecct nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

******************************************************************************/

#include <cassert>
#include <set>
#include <sstream>

/*

TODO:
* better documentation

*/

namespace json
{

    inline std::wistream& operator >> (std::wistream& istr, UnknownElement& elementRoot) {
        Reader::Read(elementRoot, istr);
        return istr;
    }

    inline Reader::Location::Location() :
        m_nLine(0),
        m_nLineOffset(0),
        m_nDocOffset(0)
    {}


    //////////////////////
    // Reader::InputStream

    class Reader::InputStream // would be cool if we could inherit from std::istream & override "get"
    {
    public:
        InputStream(std::wistream& iStr) :
            m_iStr(iStr) {}

        // protect access to the input stream, so we can keeep track of document/line offsets
        wchar_t Get(); // big, define outside
        wchar_t Peek() {
            assert(m_iStr.eof() == false); // enforce reading of only valid stream data 
            return m_iStr.peek();
        }

        bool EOS() {
            m_iStr.peek(); // apparently eof flag isn't set until a character read is attempted. whatever.
            return m_iStr.eof();
        }

        [[nodiscard]] const Location& GetLocation() const { return m_Location; }

    private:
        std::wistream& m_iStr;
        Location m_Location;
    };


    inline wchar_t Reader::InputStream::Get()
    {
        assert(m_iStr.eof() == false); // enforce reading of only valid stream data 
        wchar_t c = m_iStr.get();

        ++m_Location.m_nDocOffset;
        if (c == '\n') {
            ++m_Location.m_nLine;
            m_Location.m_nLineOffset = 0;
        }
        else {
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

        [[nodiscard]] bool EOS() const;

    private:
        const Tokens& m_Tokens;
        Tokens::const_iterator m_itCurrent;
    };


    inline Reader::TokenStream::TokenStream(const Tokens& tokens) :
        m_Tokens(tokens),
        m_itCurrent(tokens.begin())
    {}

    inline const Reader::Token& Reader::TokenStream::Peek() {
        if (EOS())
        {
            const Token& lastToken = *m_Tokens.rbegin();
            std::wstring sMessage = L"Unexpected end of token stream";
            throw ParseException(sMessage, lastToken.locBegin, lastToken.locEnd); // nowhere to point to
        }
        return *(m_itCurrent);
    }

    inline const Reader::Token& Reader::TokenStream::Get() {
        const Token& token = Peek();
        ++m_itCurrent;
        return token;
    }

    inline bool Reader::TokenStream::EOS() const {
        return m_itCurrent == m_Tokens.end();
    }

    ///////////////////
    // Reader (finally)


    inline void Reader::Read(Object& object, std::wistream& istr) { Read_i(object, istr); }
    inline void Reader::Read(Array& array, std::wistream& istr) { Read_i(array, istr); }
    inline void Reader::Read(String& wstring, std::wistream& istr) { Read_i(wstring, istr); }
    inline void Reader::Read(Number& number, std::wistream& istr) { Read_i(number, istr); }
    inline void Reader::Read(Boolean& boolean, std::wistream& istr) { Read_i(boolean, istr); }
    inline void Reader::Read(Null& null, std::wistream& istr) { Read_i(null, istr); }
    inline void Reader::Read(UnknownElement& unknown, std::wistream& istr) { Read_i(unknown, istr); }


    template <typename ElementTypeT>
    void Reader::Read_i(ElementTypeT& element, std::wistream& istr)
    {
        Reader reader;

        Tokens tokens;
        InputStream inputStream(istr);
        reader.Scan(tokens, inputStream);

        TokenStream tokenStream(tokens);
        reader.Parse(element, tokenStream);

        if (tokenStream.EOS() == false)
        {
            const Token& token = tokenStream.Peek();
            std::wstring sMessage = std::wstring(L"Expected End of token stream; found ") + token.sValue;
            throw ParseException(sMessage, token.locBegin, token.locEnd);
        }
    }


    inline void Reader::Scan(Tokens& tokens, InputStream& inputStream)
    {
        while (EatWhiteSpace(inputStream),              // ignore any leading white space...
            inputStream.EOS() == false) // ...before checking for EOS
        {
            // if all goes well, we'll create a token each pass
            Token token;
            token.locBegin = inputStream.GetLocation();

            // gives us null-terminated wstring
            wchar_t sChar = inputStream.Peek();
            switch (sChar)
            {
            case L'{':
                token.sValue = MatchExpectedString(inputStream, L"{");
                token.nType = Token::TOKEN_OBJECT_BEGIN;
                break;

            case L'}':
                token.sValue = MatchExpectedString(inputStream, L"}");
                token.nType = Token::TOKEN_OBJECT_END;
                break;

            case '[':
                token.sValue = MatchExpectedString(inputStream, L"[");
                token.nType = Token::TOKEN_ARRAY_BEGIN;
                break;

            case L']':
                token.sValue = MatchExpectedString(inputStream, L"]");
                token.nType = Token::TOKEN_ARRAY_END;
                break;

            case L',':
                token.sValue = MatchExpectedString(inputStream, L",");
                token.nType = Token::TOKEN_NEXT_ELEMENT;
                break;

            case L':':
                token.sValue = MatchExpectedString(inputStream, L":");
                token.nType = Token::TOKEN_MEMBER_ASSIGN;
                break;

            case L'"':
                token.sValue = MatchString(inputStream);
                token.nType = Token::TOKEN_STRING;
                break;

            case L'-':
            case L'0':
            case L'1':
            case L'2':
            case L'3':
            case L'4':
            case L'5':
            case L'6':
            case L'7':
            case L'8':
            case L'9':
                token.sValue = MatchNumber(inputStream);
                token.nType = Token::TOKEN_NUMBER;
                break;

            case L't':
                token.sValue = MatchExpectedString(inputStream, L"true");
                token.nType = Token::TOKEN_BOOLEAN;
                break;

            case L'f':
                token.sValue = MatchExpectedString(inputStream, L"false");
                token.nType = Token::TOKEN_BOOLEAN;
                break;

            case L'n':
                token.sValue = MatchExpectedString(inputStream, L"null");
                token.nType = Token::TOKEN_NULL;
                break;

            default:
            {
                std::wstring sErrorMessage = std::wstring(L"Unexpected character in stream: ") + sChar;
                throw ScanException(sErrorMessage, inputStream.GetLocation());
            }
            }

            token.locEnd = inputStream.GetLocation();
            tokens.push_back(token);
        }
    }


    inline void Reader::EatWhiteSpace(InputStream& inputStream)
    {
        while (inputStream.EOS() == false &&
            ::isspace(inputStream.Peek()))
            inputStream.Get();
    }

    inline std::wstring Reader::MatchExpectedString(InputStream& inputStream, const std::wstring& sExpected)
    {
        std::wstring::const_iterator it(sExpected.begin()),
            itEnd(sExpected.end());
        for (; it != itEnd; ++it) {
            if (inputStream.EOS() ||      // did we reach the end before finding what we're looking for...
                inputStream.Get() != *it) // ...or did we find something different?
            {
                std::wstring sMessage = std::wstring(L"Expected wstring: ") + sExpected;
                throw ScanException(sMessage, inputStream.GetLocation());
            }
        }

        // all's well if we made it here
        return sExpected;
    }


    inline std::wstring Reader::MatchString(InputStream& inputStream)
    {
        MatchExpectedString(inputStream, L"\"");

        std::wstring wstring;
        while (inputStream.EOS() == false &&
            inputStream.Peek() != L'"')
        {
            wchar_t c = inputStream.Get();

            // escape?
            if (c == L'\\' &&
                inputStream.EOS() == false) // shouldn't have reached the end yet
            {
                c = inputStream.Get();
                switch (c) {
                case L'/':      wstring.push_back(L'/');     break;
                case L'"':      wstring.push_back(L'"');     break;
                case L'\\':     wstring.push_back(L'\\');    break;
                case L'b':      wstring.push_back(L'\b');    break;
                case L'f':      wstring.push_back(L'\f');    break;
                case L'n':      wstring.push_back(L'\n');    break;
                case L'r':      wstring.push_back(L'\r');    break;
                case L't':      wstring.push_back(L'\t');    break;
                case L'u': { // convert unicode to UTF-8
                    int x = 0, i;

                    // next four characters should be hex
                    for (i = 0; i < 4; ++i) {
                        c = inputStream.Get();
                        if (c >= L'0' && c <= L'9') {
                            x = (x << 4) | (c - L'0');
                        }
                        else if (c >= L'a' && c <= L'f') {
                            x = (x << 4) | (c - L'a' + 10);
                        }
                        else if (c >= L'A' && c <= L'F') {
                            x = (x << 4) | (c - L'A' + 10);
                        }
                        else {
                            std::wstring sMessage = std::wstring(L"Unrecognized hexadecimal character found in wstring: ") + c;
                            throw ScanException(sMessage, inputStream.GetLocation());
                        }
                    }

                    // encode as UTF-8
                    if (x < 0x80) {
                        wstring.push_back(x);
                    }
                    else if (x < 0x800) {
                        wstring.push_back(0xc0 | (x >> 6));
                        wstring.push_back(0x80 | (x & 0x3f));
                    }
                    else {
                        wstring.push_back(0xe0 | (x >> 12));
                        wstring.push_back(0x80 | ((x >> 6) & 0x3f));
                        wstring.push_back(0x80 | (x & 0x3f));
                    }
                    break;
                }
                default: {
                    std::wstring sMessage = std::wstring(L"Unrecognized escape sequence found in wstring: \\") + c;
                    throw ScanException(sMessage, inputStream.GetLocation());
                }
                }
            }
            else {
                wstring.push_back(c);
            }
        }

        // eat the last '"' that we just peeked
        MatchExpectedString(inputStream, L"\"");

        // all's well if we made it here
        return wstring;
    }


    inline std::wstring Reader::MatchNumber(InputStream& inputStream)
    {
        const wchar_t sNumericChars[] = L"0123456789.eE-+";
        std::set<wchar_t> numericChars;
        numericChars.insert(sNumericChars, sNumericChars + sizeof(sNumericChars));

        std::wstring sNumber;
        while (inputStream.EOS() == false &&
            numericChars.find(inputStream.Peek()) != numericChars.end())
        {
            sNumber.push_back(inputStream.Get());
        }

        return sNumber;
    }


    inline void Reader::Parse(UnknownElement& element, Reader::TokenStream& tokenStream)
    {
        const Token& token = tokenStream.Peek();
        switch (token.nType) {
        case Token::TOKEN_OBJECT_BEGIN:
        {
            // implicit non-const cast will perform conversion for us (if necessary)
            Object& object = element;
            Parse(object, tokenStream);
            break;
        }

        case Token::TOKEN_ARRAY_BEGIN:
        {
            Array& array = element;
            Parse(array, tokenStream);
            break;
        }

        case Token::TOKEN_STRING:
        {
            String& wstring = element;
            Parse(wstring, tokenStream);
            break;
        }

        case Token::TOKEN_NUMBER:
        {
            Number& number = element;
            Parse(number, tokenStream);
            break;
        }

        case Token::TOKEN_BOOLEAN:
        {
            Boolean& boolean = element;
            Parse(boolean, tokenStream);
            break;
        }

        case Token::TOKEN_NULL:
        {
            Null& null = element;
            Parse(null, tokenStream);
            break;
        }

        default:
        {
            std::wstring sMessage = std::wstring(L"Unexpected token: ") + token.sValue;
            throw ParseException(sMessage, token.locBegin, token.locEnd);
        }
        }
    }


    inline void Reader::Parse(Object& object, Reader::TokenStream& tokenStream)
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
                std::wstring sMessage = std::wstring(L"Duplicate object member token: ") + member.name;
                throw ParseException(sMessage, tokenName.locBegin, tokenName.locEnd);
            }

            bContinue = (tokenStream.EOS() == false &&
                tokenStream.Peek().nType == Token::TOKEN_NEXT_ELEMENT);
            if (bContinue)
                MatchExpectedToken(Token::TOKEN_NEXT_ELEMENT, tokenStream);
        }

        MatchExpectedToken(Token::TOKEN_OBJECT_END, tokenStream);
    }


    inline void Reader::Parse(Array& array, Reader::TokenStream& tokenStream)
    {
        MatchExpectedToken(Token::TOKEN_ARRAY_BEGIN, tokenStream);

        bool bContinue = (tokenStream.EOS() == false &&
            tokenStream.Peek().nType != Token::TOKEN_ARRAY_END);
        while (bContinue)
        {
            // ...what's next? could be anything
            Array::iterator itElement = array.Insert(UnknownElement());
            UnknownElement& element = *itElement;
            Parse(element, tokenStream);

            bContinue = (tokenStream.EOS() == false &&
                tokenStream.Peek().nType == Token::TOKEN_NEXT_ELEMENT);
            if (bContinue)
                MatchExpectedToken(Token::TOKEN_NEXT_ELEMENT, tokenStream);
        }

        MatchExpectedToken(Token::TOKEN_ARRAY_END, tokenStream);
    }


    inline void Reader::Parse(String& wstring, Reader::TokenStream& tokenStream)
    {
        wstring = MatchExpectedToken(Token::TOKEN_STRING, tokenStream);
    }


    inline void Reader::Parse(Number& number, Reader::TokenStream& tokenStream)
    {
        const Token& currentToken = tokenStream.Peek(); // might need this later for throwing exception
        const std::wstring& sValue = MatchExpectedToken(Token::TOKEN_NUMBER, tokenStream);

        std::wistringstream iStr(sValue);
        double dValue;
        iStr >> dValue;

        // did we consume all characters in the token?
        if (iStr.eof() == false)
        {
            wchar_t c = iStr.peek();
            std::wstring sMessage = std::wstring(L"Unexpected character in NUMBER token: ") + c;
            throw ParseException(sMessage, currentToken.locBegin, currentToken.locEnd);
        }

        number = dValue;
    }


    inline void Reader::Parse(Boolean& boolean, Reader::TokenStream& tokenStream)
    {
        const std::wstring& sValue = MatchExpectedToken(Token::TOKEN_BOOLEAN, tokenStream);
        boolean = (sValue == L"true" ? true : false);
    }


    inline void Reader::Parse(Null&, Reader::TokenStream& tokenStream)
    {
        MatchExpectedToken(Token::TOKEN_NULL, tokenStream);
    }


    inline const std::wstring& Reader::MatchExpectedToken(Token::Type nExpected, Reader::TokenStream& tokenStream)
    {
        const Token& token = tokenStream.Get();
        if (token.nType != nExpected)
        {
            std::wstring sMessage = std::wstring(L"Unexpected token: ") + token.sValue;
            throw ParseException(sMessage, token.locBegin, token.locEnd);
        }

        return token.sValue;
    }

} // End namespace
