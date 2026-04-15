#ifndef IO_ENUMS_HPP
#define IO_ENUMS_HPP

#include <iostream>

enum class IO_FileType{
  OBJ,
  OFF,
  NotFound
};

enum class IO_ResultType{
  OK,
  GenericError,
  FileNotFound,
  ExtensionNotFound,
  WrongStructure
};

inline void IO_printError(IO_ResultType result){
  if (result != IO_ResultType::OK){
    switch (result) {
      case IO_ResultType::FileNotFound: std::cout << "ERROR. File not found\n"; break;
      case IO_ResultType::WrongStructure: std::cout << "Structure of the file not readable\n"; break;
      case IO_ResultType::ExtensionNotFound: std::cout << "Cannot read the file format\n"; break;
      case IO_ResultType::GenericError: std::cout << "ERROR. Generic Error\n"; break;
      case IO_ResultType::OK: std::cout << "OK"; break;
    }
  }
}
#endif // !IO_ENUMS_HPP
