#ifndef WRITE_FIXTURE_H
#define WRITE_FIXTURE_H

#include "json/elements.h"
#include <sstream>

class WriteFixture
{
public:
   virtual void RunPack() { RunPack(root); }
   void RunPack(json::UnknownElement& input)
   {
      json::Writer::Write(input, output);
   }

   json::UnknownElement root;
   std::stringstream output;
};

#endif // WRITE_FIXTURE_H