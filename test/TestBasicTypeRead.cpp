#include "json/reader.h"
#include "JsonOps.h"
#include "ReadFixture.h"
#include <boost/test/unit_test.hpp>

struct BasicTypeReadFixture : public ReadFixture
{
   void CheckString(std::string in, std::string expected)
   {
      input.clear();
      input.str("");
      input << in;
      RunParse();
      std::string output = json::String(root);
      BOOST_TEST(expected == output);
   }
};

BOOST_AUTO_TEST_SUITE(BasicTypeRead)

BOOST_FIXTURE_TEST_CASE(TypeString, BasicTypeReadFixture)
{
   std::string test = "TEST_STRING";
   input << JsonOps::QuoteString(test);
   RunParse();

   std::string output = json::String(root);
   BOOST_TEST(test == output);
}
BOOST_FIXTURE_TEST_CASE(TypeBoolTrue, BasicTypeReadFixture)
{
   bool test = true;
   input << JsonOps::BoolToJson(test);
   RunParse();

   bool output = json::Boolean(root);
   BOOST_TEST(test == output);
}
BOOST_FIXTURE_TEST_CASE(TypeBoolFalse, BasicTypeReadFixture)
{
   bool test = false;
   input << JsonOps::BoolToJson(test);
   RunParse();

   bool output = json::Boolean(root);
   BOOST_TEST(test == output);
}
BOOST_FIXTURE_TEST_CASE(TypeDouble, BasicTypeReadFixture)
{
   double test = 42.1;
   input << test;
   RunParse();

   double output = json::Number(root);
   BOOST_TEST(test == output);
}
BOOST_FIXTURE_TEST_CASE(TypeDoubleNeg, BasicTypeReadFixture)
{
   double test = -42.1;
   input << test;
   RunParse();

   double output = json::Number(root);
   BOOST_TEST(test == output);
}
// TODO: Doubles with a zero whole number, exponents
BOOST_FIXTURE_TEST_CASE(TypeIntCast, BasicTypeReadFixture)
{
   int32_t test = 42;
   input << test;
   RunParse();

   double output = json::Number(root);
   int32_t outputCast = static_cast<int32_t>(output);
   BOOST_TEST(test == outputCast);
}
BOOST_FIXTURE_TEST_CASE(TypeIntCastNeg, BasicTypeReadFixture)
{
   int32_t test = -42;
   input << test;
   RunParse();

   double output = json::Number(root);
   int32_t outputCast = static_cast<int32_t>(output);
   BOOST_TEST(test == outputCast);
}
BOOST_FIXTURE_TEST_CASE(TypeNull, BasicTypeReadFixture)
{
   input << "null";
   RunParse();

   auto output = json::Null(root);
   auto compare = json::Null();
   BOOST_TEST((compare == output));
}

BOOST_FIXTURE_TEST_CASE(StringEscapes, BasicTypeReadFixture)
{
   CheckString("\"TEST_STRING\"", "TEST_STRING");
   CheckString("\"\\\\TEST_STRING\"", "\\TEST_STRING");
   CheckString("\"TEST_S\\\\TRING\"", "TEST_S\\TRING");
   CheckString("\"\\\"TEST\\\"_STRING\"", "\"TEST\"_STRING");
   CheckString("\"/TEST_STRING\"", "/TEST_STRING");
   CheckString("\"TEST_/STRING\"", "TEST_/STRING");

   CheckString("\"\\tTEST_STRING\"", "\tTEST_STRING");
   CheckString("\"\\nTEST_STRING\"", "\nTEST_STRING");
   CheckString("\"\\rTEST_STRING\"", "\rTEST_STRING");
   CheckString("\"\\fTEST_STRING\"", "\fTEST_STRING");
   CheckString("\"\\bTEST_STRING\"", "\bTEST_STRING");
   // CheckString(std::string("\"\\") + "u0045_TEST_STRING\"", "E_TEST_STRING"); TODO: Fails, this is a bug in the lib
   CheckString("\"\xC8\xB7_TEST_STRING\"", "\xC8\xB7_TEST_STRING");

}

BOOST_AUTO_TEST_SUITE_END()
