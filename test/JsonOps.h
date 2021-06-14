#ifndef BASIC_OPERATIONS_H
#define BASIC_OPERATINOS_H

#include <string>

class JsonOps
{
public:
   static std::string QuoteString(std::string input) { return "\"" + input + "\""; }
   static std::string BoolToJson(bool input) { if (input) { return "true"; } return "false"; }
};

#endif // !BASIC_OPERATIONS_H