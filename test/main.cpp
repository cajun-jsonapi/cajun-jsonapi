// json.cpp : Defines the entry point for the console application.
//

#include "elements.h"
#include "quick.h"
#include "writer.h"
#include "reader.h"
#include "cast.h"
#include "exception.h"

#include <iostream>
#include <sstream>



int main()
{
   using namespace json;

   /* we'll generate: 
      {
         "Delicious Beers" : [
            {
               "Name" : "Schlafly American Pale Ale",
               "Origin" : "St. Louis, MO, USA",
               "ABV" : 5.9,
               "BottleConditioned" : true
            },
            {
               "Name" : "John Smith's Extra Smooth",
               "Origin" : "Tadcaster, Yorkshire, UK",
               "ABV" : 3.8,
               "Bottle Conditioned" : false
            }
         ]
      }
   */

   //////////////////////////////////////////////
   // "verbose" construction, the traditional way
   {
      Object objAPA;
      objAPA["Name"] = String("Schlafly American Pale Ale");
      objAPA["Origin"] = String("St. Louis, MO, USA");
      objAPA["ABV"] = Number(5.9);
      objAPA["BottleConditioned"] = Boolean(true);

      Object objJohnSmiths;
      objJohnSmiths["Name"] = String("John Smith's Extra Smooth");
      objJohnSmiths["Origin"] = String("Tadcaster, Yorkshire, England");
      objJohnSmiths["ABV"] = Number(5.9);
      objJohnSmiths["BottleConditioned"] = Boolean(false);

      Array arrayBeers;
      arrayBeers.Insert(objAPA);
      arrayBeers.Insert(objJohnSmiths);

      Object objRoot;
      objRoot["Delicious Beers"] = arrayBeers;

      // what's it look like?
      Writer::Write(objRoot, std::cout);
      std::cout << std::endl << std::endl;
   }


   //////////////////////////////////////////////
   // "quick" construction, with the helper class
   // we'll verify the contents in the intpretation test below

   Element elemRoot;
   QuickBuilder builder(elemRoot);

   builder["Delicious Beers"][0]["Name"] = String("Schlafly American Pale Ale");
   builder["Delicious Beers"][0]["Origin"] = String("St. Louis, MO");
   builder["Delicious Beers"][0]["ABV"] = Number(5.9);
   builder["Delicious Beers"][0]["BottleConditioned"] = Boolean(true);

   builder["Delicious Beers"][1]["Name"] = String("John Smith's Extra Smooth");
   builder["Delicious Beers"][1]["Origin"] = String("Tadcaster, England");
   builder["Delicious Beers"][1]["ABV"] = Number(5.9);
   builder["Delicious Beers"][1]["BottleConditioned"] = Boolean(false);

   
   ///////////////////////////
   // "verbose" interpretation
   {
      // get the element we want, and cast it to the expected type
      const Object& objRoot = json_cast<const Object&>(elemRoot);
      
      // accessing an array or object member always results in an Element
      const Element& elemBeers = objRoot["Delicious Beers"];
      const Array& arrayBeers = json_cast<const Array&>(elemBeers);

      // we can combine the two operations of course
      const Object& objBeer0 = json_cast<const Object&>(arrayBeers[0]);

      // and we can take advantage of TrivialType's implicit casts to their respective value types
      const std::string& sName0 = json_cast<const String&>(objBeer0["Name"]);
      double dAbv0 = json_cast<const Number&>(objBeer0["ABV"]);
      std::cout << "First beer name: " << sName0 << std::endl;
      std::cout << "First beer ABV: " << dAbv0 << std::endl;
   
      // everything's cool until we try to access a non-existent array element
      try
      {
         std::cout << "Expecting exception: Array out of bounds" << std::endl;
         const Element& elemName2 = arrayBeers[2];
      }
      catch (const Exception& e)
      {
         std::cout << "Caught json::Exception: " << e.what() << std::endl << std::endl;
      }
   }


   /////////////////////////
   // "quick" interpretation
   {
      QuickInterpreter interpeter(elemRoot);
      const String& sBeerName1 = interpeter["Delicious Beers"][1]["Name"]; // implicit cast from QuickInterpretter to String
      std::cout << "Second beer name: " << std::string(sBeerName1) << std::endl;

      const Boolean& boolConditioned = interpeter["Delicious Beers"][1]["BottleConditioned"];
      std::cout << "Second beer, bottle conditioned? : "
                << (boolConditioned ? "true" : "false") << std::endl;

      // an exception will be thrown when expected data not found, since "Rice" is never a member of good beer
      try 
      {
         std::cout << "Expecting exception: Object member not found" << std::endl;
         const Boolean& boolRice = interpeter["Delicious Beers"][1]["Rice"];
      }
      catch (const Exception& e)
      {
         std::cout << "Caught json::Exception: " << e.what() << std::endl << std::endl;
      }
   }


   ///////////////////////
   // document deep copying
   {
      // we can make an exact duplicate too
      Element elemRoot2 = elemRoot; 

      // the two documents should start out equal
      bool bEqualInitially = (elemRoot == elemRoot2);
      std::cout << "Document copies should start out equivalent. operator == returned: "
                << (bEqualInitially ? "true" : "false") << std::endl;

      // prove objRoot2 is a deep copy of objRoot:
      //  remove Beers[1]
      QuickBuilder builder2(elemRoot2);
      Array& array = builder2["Delicious Beers"];
      array.Erase(array.Begin()); // trim it down to one. this leaves elemRoot the same

      // the two documents should start out equal
      bool bEqualNow = (elemRoot == elemRoot2);
      std::cout << "Document copies should now be different. operator == returned: "
                << (bEqualNow ? "true" : "false") << std::endl << std::endl;
   }


   //////////////////////////
   // read/write sanity check
   {
      // write it out to a string stream (file stream would work the same)....
      std::cout << "Writing file out...";

      std::stringstream stream;
      Writer::Write(elemRoot, stream);

      // ...then read it back in
      std::cout << "then reading it back in." << std::endl;
      Element elemRootFile;
      Reader::Read(elemRootFile, stream);

      // still look right?
      bool bEquals = (elemRoot == elemRootFile);
      std::cout << "Original document and streamed document should be equivalent. operator == returned: "
                << (bEquals ? "true" : "false") << std::endl;
   }

   return 0;
}

