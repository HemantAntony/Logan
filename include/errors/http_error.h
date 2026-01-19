#include <exception>
#include <string>

class HttpError : public std::exception {
  int status_code;
  std::string message;

public:
  HttpError(int code, std::string msg)
    : status_code(code), message(std::move(msg)) {}
  
  int status() const noexcept {
      return status_code;
  }

  const char* what() const noexcept override {
      return message.c_str();
  }
};
