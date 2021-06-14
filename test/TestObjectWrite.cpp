#include "json/writer.h"
#include "WriteFixture.h"
#include "JsonOps.h"
#include <boost/test/unit_test.hpp>

class ObjectWriteFixture : public WriteFixture
{
public:
   virtual void ObjPack() 
   { 
      root = o;
      RunPack();
   }
   json::Object o;
};


BOOST_AUTO_TEST_SUITE(ObjectWrite)

BOOST_FIXTURE_TEST_CASE(EmptyObject, ObjectWriteFixture)
{
   ObjPack();

   BOOST_TEST("{}" == output.str());
}

BOOST_FIXTURE_TEST_CASE(SingleItem, ObjectWriteFixture)
{
   o["Null"] = json::Null();
   ObjPack();

   BOOST_TEST("{\n\t\"Null\" : null\n}" == output.str());
}

BOOST_FIXTURE_TEST_CASE(ObjectWithBasicTypes, ObjectWriteFixture)
{
   o["String"] = json::String("TEST_STRING");
   o["Number"] = json::Number(42);
   o["Boolean"] = json::Boolean(true);
   o["Null"] = json::Null();

   ObjPack();

   BOOST_TEST("{\n\t\"String\" : \"TEST_STRING\",\n\t\"Number\" : 42,\n\t\"Boolean\" : true,\n\t\"Null\" : null\n}"
      == output.str());
}

BOOST_FIXTURE_TEST_CASE(ObjectWithObject, ObjectWriteFixture)
{
   json::Object o2;
   o2["Null2"] = json::Null();
   o["NestedObject"] = o2;

   ObjPack();

   BOOST_TEST("{\n\t\"NestedObject\" : {\n\t\t\"Null2\" : null\n\t}\n}" == output.str());
}

BOOST_FIXTURE_TEST_CASE(ObjectWithArray, ObjectWriteFixture)
{
   json::Array a;
   a[0] = json::Null();
   a[1] = json::Null();
   o["NestedArray"] = a;

   ObjPack();

   BOOST_TEST("{\n\t\"NestedArray\" : [\n\t\tnull,\n\t\tnull\n\t]\n}" == output.str());
}

BOOST_AUTO_TEST_SUITE_END()