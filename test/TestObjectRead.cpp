#include "json/reader.h"
#include "ReadFixture.h"
#include "JsonOps.h"
#include <boost/test/unit_test.hpp>

class ObjectReadFixture : public ReadFixture
{
public:
   virtual void ObjParse() 
   { 
      RunParse();
      o = root;
   }
   // Putting a value in a const UnknownElement first causes an exception if there is a bad conversion
   const json::UnknownElement GetConstElement(std::string key)
   {
      const json::UnknownElement u = o[key];
      return u;
   }
   void KeyInObject(std::string key)
   {
      BOOST_TEST((o.Find(key) != o.End()), "Could not find key: " + key);
   }
   json::Object o;
};


BOOST_AUTO_TEST_SUITE(ObjectRead)

BOOST_FIXTURE_TEST_CASE(EmptyObject, ObjectReadFixture)
{
   input << "{}";

   ObjParse();

   BOOST_TEST((o.Begin() == o.End()));
}

BOOST_FIXTURE_TEST_CASE(SingleItem, ObjectReadFixture)
{
   input << "{\n\t\"Null\" : null\n}";
   ObjParse();

   BOOST_TEST(o.Begin()->name == "Null");
   const json::Null n = GetConstElement("Null");
}

BOOST_FIXTURE_TEST_CASE(ObjectWithBasicTypes, ObjectReadFixture)
{
   input << "{\n\t\"String\" : \"TEST_STRING\",\n\t\"Number\" : 42,\n\t\"Boolean\" : true,\n\t\"Null\" : null\n}";
   ObjParse();

   KeyInObject("String");
   const json::String s = GetConstElement("String");
   BOOST_TEST("TEST_STRING" == s.Value());

   KeyInObject("Number");
   const json::Number n = GetConstElement("Number");
   BOOST_TEST(42 == n.Value());

   KeyInObject("Boolean");
   const json::Boolean b = GetConstElement("Boolean");
   BOOST_TEST(true == b.Value());

   KeyInObject("Null");
   const json::Null nu = GetConstElement("Null");
}

BOOST_FIXTURE_TEST_CASE(ObjectWithObject, ObjectReadFixture)
{
   input << "{\n\t\"NestedObject\" : {\n\t\t\"Null2\" : null\n\t}\n}";
   ObjParse();

   KeyInObject("NestedObject");
   json::Object o2 = GetConstElement("NestedObject");
   BOOST_TEST((o2.Find("Null2") != o2.End()), "No sub object");
}

BOOST_FIXTURE_TEST_CASE(ObjectWithArray, ObjectReadFixture)
{
   input << "{\n\t\"NestedArray\" : [\n\t\tnull,\n\t\tnull\n\t]\n}";
   ObjParse();

   KeyInObject("NestedArray");
   json::Array a = GetConstElement("NestedArray");
   BOOST_TEST(2 == a.Size());
}

BOOST_AUTO_TEST_SUITE_END()