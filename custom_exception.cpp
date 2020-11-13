#include "custom_exception.h"

CustomException::CustomException(std::string message) {
    this->message = message;
}

std::string CustomException::getMessage() {
    return "[ERROR] " + message;
}