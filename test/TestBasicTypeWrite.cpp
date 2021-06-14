#include "json/writer.h"
#include "JsonOps.h"
#include "WriteFixture.h"
#include <boost/test/unit_test.hpp>

struct BasicTypeWriteFixture : public WriteFixture
{
   void CheckString(std::string expected, std::string value)
   {
      output.clear();
      output.str("");
      root = json::String(value);
      RunPack();
      BOOST_TEST(expected == output.str());
   }
};

BOOST_AUTO_TEST_SUITE(BasicTypeWrite)

BOOST_FIXTURE_TEST_CASE(TypeString, BasicTypeWriteFixture)
{
   std::string test = "TEST_STRING";
   root = json::String(test);
   RunPack();
   BOOST_TEST(JsonOps::QuoteString(test) == output.str());
}
BOOST_FIXTURE_TEST_CASE(TypeBoolTrue, BasicTypeWriteFixture)
{
   bool test = true;
   root = json::Boolean(test);
   RunPack();
   BOOST_TEST(JsonOps::BoolToJson(test) == output.str());
}
BOOST_FIXTURE_TEST_CASE(TypeBoolFalse, BasicTypeWriteFixture)
{
   bool test = true;
   root = json::Boolean(test);
   RunPack();
   BOOST_TEST(JsonOps::BoolToJson(test) == output.str());
}
BOOST_FIXTURE_TEST_CASE(TypeDouble, BasicTypeWriteFixture)
{
   double test = 42.1;
   root = json::Number(test);
   RunPack();
   BOOST_TEST(std::to_string(test).substr(0,6) == output.str().substr(0, 6));
}
BOOST_FIXTURE_TEST_CASE(TypeDoubleNeg, BasicTypeWriteFixture)
{
   double test = -42.1;
   root = json::Number(test);
   RunPack();
   BOOST_TEST(std::to_string(test).substr(0, 6) == output.str().substr(0, 6));
}
// TODO: Doubles with a zero whole number, exponents
BOOST_FIXTURE_TEST_CASE(TypeInt, BasicTypeWriteFixture)
{
   int32_t test = 42;
   root = json::Number(test);
   RunPack();
   BOOST_TEST(std::to_string(test) == output.str());
}
BOOST_FIXTURE_TEST_CASE(TypeIntNeg, BasicTypeWriteFixture)
{
   int32_t test = -42;
   root = json::Number(test);
   RunPack();
   BOOST_TEST(std::to_string(test) == output.str());
}
BOOST_FIXTURE_TEST_CASE(TypeNull, BasicTypeWriteFixture)
{
   root = json::Null();
   RunPack();
   BOOST_TEST("null" == output.str());
}

BOOST_FIXTURE_TEST_CASE(StringEscapes, BasicTypeWriteFixture)
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
   CheckString("\"E_TEST_STRING\"", "\u0045_TEST_STRING");
   //CheckString("\"\xC8\xB7_TEST_STRING\"", "\xC8\xB7_TEST_STRING");
   CheckString("\"\\u0237_TEST_STRING\"", "\xC8\xB7_TEST_STRING");
}
BOOST_AUTO_TEST_SUITE_END()
