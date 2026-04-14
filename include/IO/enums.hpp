#ifndef IO_ENUMS_HPP
#define IO_ENUMS_HPP

enum class FileType{
  OBJ,
  OFF,
  NotFound
};

enum class ResultType{
  OK,
  GenericError,
  FileNotFound,
  ExtensionNotFound,
  WrongStructure
};
#endif // !IO_ENUMS_HPP
