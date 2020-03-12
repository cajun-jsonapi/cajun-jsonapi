#pragma once
#include <stdexcept>

using namespace std;

class w_runtime_error : public runtime_error {
public:
    w_runtime_error(const wstring& msg) : runtime_error("Error!"), message(msg) {};
    ~w_runtime_error() throw() {};

    wstring get_message() { return message; }

private:
    wstring message;
};