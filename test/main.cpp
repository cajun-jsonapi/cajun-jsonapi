// json.cpp : Defines the entry point for the console application.
//

#include "elements.h"
#include "quick.h"
#include "writer.h"
#include "reader.h"
#include "cast.h"
#include "exception.h"

#include <iostream>
#include <fstream>



int main()
{
   using namespace json;

   /* we'll generate: 
      {
         "Driver" : {
            "Name" : "Terry"
            "Age" : 28,
            "Cars" : [
               {
                  "MakeModel" : "Ford Tempo",
                  "Year" : 1990
               },
               {
                  "MakeModel" : "Volkswagen Passat",
                  "Year" : 2003
               }
            ]
         }
      }
   */

   // function scope, since we'll (re)use this repeatedly
   Object objRoot;

   //////////////////////////////////////////////
   // "verbose" construction, the traditional way
   {
      Object objCar1;
      objCar1["MakeModel"] = String("Ford Tempo");
      objCar1["Year"] = Number(1990);

      Object objCar2;
      objCar2["MakeModel"] = String("Volkswagen Passat");
      objCar2["Year"] = Number(2003);

      Array arrayCars;
      arrayCars.Insert(objCar1);
      arrayCars.Insert(objCar2);

      Object objDriver;
      objDriver["Name"] = String("Terry");
      objDriver["Age"] = Number(28);
      objDriver["Cars"] = arrayCars;

      objRoot["Driver"] = objDriver;

      // what's it look like?
      Writer::Write(objRoot, std::cout);
   }

   //////////////////////////////////////////////
   // "quick" construction, with the helper class
   {
      objRoot.Clear(); // reset it
      QuickBuilder builder(objRoot);

      builder["Driver"]["Name"] = String("Terry");
      builder["Driver"]["Age"] = Number(28);
      builder["Driver"]["Cars"][0]["MakeModel"] = String("Ford Tempo");
      builder["Driver"]["Cars"][0]["Year"] = Number(1990);
      builder["Driver"]["Cars"][1]["MakeModel"] = String("Volkswagen Passat");
      builder["Driver"]["Cars"][1]["Year"] = Number(2003);

      // we'll verify the contents in the intpretation test below
   }

   
   ///////////////////////////
   // "verbose" interpretation
   {
      // get the element we want, and cast it to the expected type
      const Element& elemDriver = objRoot["Driver"];
      const Object& objDriver = json_cast<const Object&>(elemDriver);

      // we can combine the two operations of course
      const Array& arrayCars = json_cast<const Array&>(objDriver["Cars"]);
      const Object& objCar0 = json_cast<const Object&>(arrayCars[0]);
      const Number& numCarYear0 = json_cast<const Number&>(objCar0["Year"]);
      std::cout << "Year of driver's Cars[0]: " << numCarYear0 << std::endl;

      const std::string& strCarMake0 = json_cast<const String&>(objCar0["MakeModel"]);
      std::cout << "MakeModel of driver's Cars[0]: " << strCarMake0 << std::endl;
      
      // everything's happy until we get here, since arrayCars[0] is an object not a string
      try
      {
         const String& str = json_cast<const String&>(arrayCars[0]);
      }
      catch (const Exception& e)
      {
         std::cout << "Caught expected json::Exception: " << e.what() << std::endl;
      }
   }


   /////////////////////////
   // "quick" interpretation
   {
      QuickInterpreter interpeter(objRoot);
      const std::string& sDriverName = interpeter["Driver"]["Name"].As<String>(); // implicit cast from String to std::string
      std::cout << "Driver's name: " << sDriverName << std::endl;

      const Number& numYear = interpeter["Driver"]["Cars"][0]["Year"].As<Number>();
      std::cout << "Year of driver's Cars[0]: " << int(numYear) << std::endl;

      try 
      {
         // exception thrown again when expected data not found, since there's no "Color" member of Cars[0]
         const String& strColor = interpeter["Driver"]["Cars"][0]["Color"].As<String>();
      }
      catch (const Exception& e)
      {
         std::cout << "Caught expected json::Exception: " << e.what() << std::endl;
      }
   }

   ///////////////////////
   // document deep copying
   {
      // we can make an exact duplicate too
      Element objRoot2 = objRoot; 
      QuickBuilder builder2(objRoot2);

      // prove objRoot2 is a deep copy of objRoot:
      //  change year of Cars[0]
      //  remove Cars[1]
      builder2["Driver"]["Cars"][0]["Year"] = Number(1991);
      Array& array = builder2["Driver"]["Cars"].As<Array>();
      array.Resize(1); // trim it down to one. this leaves objRoot the same

      try
      {
         // objRoot["Driver"]["Cars"][0] still exists...
         QuickInterpreter interpreter(objRoot);
         std::cout << interpreter["Driver"]["Cars"][0]["Year"].As<Number>() << std::endl;

         // but objRoot2["Driver"]["Cars"][1] no longer does...
         QuickInterpreter interpreter2(objRoot2);
         std::cout << interpreter2["Driver"]["Cars"][1]["Year"].As<Number>() << std::endl;
      }
      catch (const Exception& e)
      {
         std::cout << "Caught expected json::Exception: " << e.what() << std::endl;
      }
   }


   //////////////////////////
   // read/write sanity check
   {
      // write it out to a file....
      std::cout << "Writing file out...";
      Writer::Write(objRoot, std::ofstream("json.txt"));

      // ...then read it back in
      std::cout << "then reading it back in." << std::endl;
      Element elemRootFile = String();
      Reader::Read(elemRootFile, std::ifstream("json.txt"));

      // still look right?
      QuickInterpreter interpeter(elemRootFile);
      const std::string& sName = interpeter["Driver"]["Name"].As<String>();

      std::cout << "Driver's name is still: " << sName << std::endl;
   }

   return 0;
}

