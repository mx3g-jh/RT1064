// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_SCALARTYPE_EXECUTORCH_FLATBUFFER_H_
#define FLATBUFFERS_GENERATED_SCALARTYPE_EXECUTORCH_FLATBUFFER_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 &&
              FLATBUFFERS_VERSION_MINOR == 3 &&
              FLATBUFFERS_VERSION_REVISION == 25,
             "Non-compatible flatbuffers version included");

namespace executorch_flatbuffer {

enum class ScalarType : int8_t {
  BYTE = 0,
  CHAR = 1,
  SHORT = 2,
  INT = 3,
  LONG = 4,
  HALF = 5,
  FLOAT = 6,
  DOUBLE = 7,
  BOOL = 11,
  QINT8 = 12,
  QUINT8 = 13,
  QINT32 = 14,
  QUINT4X2 = 16,
  QUINT2X4 = 17,
  BITS16 = 22,
  FLOAT8E5M2 = 23,
  FLOAT8E4M3FN = 24,
  FLOAT8E5M2FNUZ = 25,
  FLOAT8E4M3FNUZ = 26,
  UINT16 = 27,
  UINT32 = 28,
  UINT64 = 29,
  MIN = BYTE,
  MAX = UINT64
};

inline const ScalarType (&EnumValuesScalarType())[22] {
  static const ScalarType values[] = {
    ScalarType::BYTE,
    ScalarType::CHAR,
    ScalarType::SHORT,
    ScalarType::INT,
    ScalarType::LONG,
    ScalarType::HALF,
    ScalarType::FLOAT,
    ScalarType::DOUBLE,
    ScalarType::BOOL,
    ScalarType::QINT8,
    ScalarType::QUINT8,
    ScalarType::QINT32,
    ScalarType::QUINT4X2,
    ScalarType::QUINT2X4,
    ScalarType::BITS16,
    ScalarType::FLOAT8E5M2,
    ScalarType::FLOAT8E4M3FN,
    ScalarType::FLOAT8E5M2FNUZ,
    ScalarType::FLOAT8E4M3FNUZ,
    ScalarType::UINT16,
    ScalarType::UINT32,
    ScalarType::UINT64
  };
  return values;
}

inline const char * const *EnumNamesScalarType() {
  static const char * const names[31] = {
    "BYTE",
    "CHAR",
    "SHORT",
    "INT",
    "LONG",
    "HALF",
    "FLOAT",
    "DOUBLE",
    "",
    "",
    "",
    "BOOL",
    "QINT8",
    "QUINT8",
    "QINT32",
    "",
    "QUINT4X2",
    "QUINT2X4",
    "",
    "",
    "",
    "",
    "BITS16",
    "FLOAT8E5M2",
    "FLOAT8E4M3FN",
    "FLOAT8E5M2FNUZ",
    "FLOAT8E4M3FNUZ",
    "UINT16",
    "UINT32",
    "UINT64",
    nullptr
  };
  return names;
}

inline const char *EnumNameScalarType(ScalarType e) {
  if (::flatbuffers::IsOutRange(e, ScalarType::BYTE, ScalarType::UINT64)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesScalarType()[index];
}

}  // namespace executorch_flatbuffer

#endif  // FLATBUFFERS_GENERATED_SCALARTYPE_EXECUTORCH_FLATBUFFER_H_
