#ifndef READ_FIXTURE_H
#define READ_FIXTURE_H

#include "json/elements.h"
#include <sstream>

class ReadFixture
{
public:
   void RunParse() { RunParse(input); }
   void RunParse(std::istream& file)
   {
      json::Reader::Read(root, file);
   }

   json::UnknownElement root;
   std::stringstream input;
};
#endif //  READ_FIXTURE_H