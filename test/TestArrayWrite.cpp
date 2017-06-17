#include "json/writer.h"
#include "WriteFixture.h"
#include "JsonOps.h"
#include <boost/test/unit_test.hpp>

class ArrayWriteFixture : public WriteFixture
{
public:
   virtual void ArrPack() 
   { 
      root = a;
      RunPack();
   }
   json::Array a;
};


BOOST_AUTO_TEST_SUITE(ArrayWrite)

BOOST_FIXTURE_TEST_CASE(EmptyArray, ArrayWriteFixture)
{
   ArrPack();

   BOOST_TEST("[]" == output.str());
}

BOOST_FIXTURE_TEST_CASE(SingleItem, ArrayWriteFixture)
{
   a.Insert(json::Null());
   ArrPack();

   BOOST_TEST("[\n\tnull\n]" == output.str());
}

BOOST_FIXTURE_TEST_CASE(MultipleItems, ArrayWriteFixture)
{
   a.Insert(json::Null());
   a.Insert(json::Null());
   a.Insert(json::Null());
   ArrPack();

   BOOST_TEST("[\n\tnull,\n\tnull,\n\tnull\n]" == output.str());
}

BOOST_FIXTURE_TEST_CASE(MixedTypes, ArrayWriteFixture)
{
   a.Insert(json::String("TEST_STRING"));
   a.Insert(json::Number(42));
   a.Insert(json::Boolean(true));
   a.Insert(json::Null());
   a.Insert(json::Object());
   ArrPack();

   BOOST_TEST("[\n\t\"TEST_STRING\",\n\t42,\n\ttrue,\n\tnull,\n\t{}\n]" == output.str());
}

BOOST_FIXTURE_TEST_CASE(NestedArrays, ArrayWriteFixture)
{
   json::Array a1;
   a1.Insert(json::Null());
   a1.Insert(json::Null());
   json::Array a2;
   a2.Insert(json::Null());
   json::Array a3;

   a.Insert(a1);
   a.Insert(a2);
   a.Insert(a3);
   ArrPack();

   BOOST_TEST("[\n\t[\n\t\tnull,\n\t\tnull\n\t],\n\t[\n\t\tnull\n\t],\n\t[]\n]" == output.str());
}

BOOST_AUTO_TEST_SUITE_END()