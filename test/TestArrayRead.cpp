#include "json/reader.h"
#include "ReadFixture.h"
#include "JsonOps.h"
#include <boost/test/unit_test.hpp>

class ArrayReadFixture : public ReadFixture
{
public:
   virtual void ArrParse() 
   { 
      RunParse();
      a = root;
   }
   // Putting a value in a const UnknownElement first causes an exception if there is a bad conversion
   const json::UnknownElement GetConstElement(size_t index)
   {
      const json::UnknownElement u = a[index];
      return u;
   }

   json::Array a;
};

BOOST_AUTO_TEST_SUITE(ArrayRead)

BOOST_FIXTURE_TEST_CASE(EmptyArray, ArrayReadFixture)
{
   input << "[]";

   ArrParse();

   BOOST_TEST(0 == a.Size());
   BOOST_TEST(true == a.Empty());
}

BOOST_FIXTURE_TEST_CASE(SingleItem, ArrayReadFixture)
{
   input << "[null]";
   ArrParse();

   BOOST_TEST(1 == a.Size());
   const json::Null n = GetConstElement(0);
}

BOOST_FIXTURE_TEST_CASE(MultipleItems, ArrayReadFixture)
{
   size_t numItems = 10;
   input << "[";
   bool firstDone = false;
   for (int i = 0; i < numItems; i++)
   {
      if (!firstDone)
      {
         firstDone = true;
      }
      else
      {
         input << ", ";
      }
      input << "null";
   }
   input << "]";
   ArrParse();

   BOOST_TEST(numItems == a.Size());
   for (int i = 0; i < numItems; i++)
   {
      const json::Null n = GetConstElement(i);
   }
}

BOOST_FIXTURE_TEST_CASE(MixedTypes, ArrayReadFixture)
{
   input << "[\"TEST_STRING\", 42, true, null, {}]";
   ArrParse();

   const json::String s = GetConstElement(0);
   const json::Number n = GetConstElement(1);
   const json::Boolean b = GetConstElement(2);
   const json::Null nu = GetConstElement(3);
}

BOOST_FIXTURE_TEST_CASE(NestedArrays, ArrayReadFixture)
{
   input << "[[null,null],[null],[]]";
   ArrParse();

   const json::Array a1 = GetConstElement(0);
   BOOST_TEST(2 == a1.Size());
   const json::Array a2 = GetConstElement(1);
   BOOST_TEST(1 == a2.Size());
   const json::Array a3 = GetConstElement(2);
   BOOST_TEST(true == a3.Empty());
}

BOOST_AUTO_TEST_SUITE_END()