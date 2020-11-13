#ifndef CUSTOM_EXCEPTION_H
#define CUSTOM_EXCEPTION_H

#include <iostream>
#include <exception>
#include <string>

class CustomException : public std::exception {

public:
    CustomException(std::string message);
    std::string getMessage();

private:
    std::string message;
};

#endif //CUSTOM_EXCEPTION_H