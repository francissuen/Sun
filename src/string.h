/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_STRING_H
#define FS_SUN_STRING_H

#include <array>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "config.h"
#include "ns.h"

FS_SUN_NS_BEGIN

namespace string {
/**
 * \brief Extract blocks which are bounded by \b bounding_delimiters.
 *
 * \return Returned value is in the same order as \b bounding_delimiters.
 */
std::vector<std::string> ExtractBlocks(
    const std::string& src,
    const std::vector<std::pair<std::string, std::string>>&
        bounding_delimiters);

std::vector<std::string> Split(const std::string& src, const char* delimiter);

std::string DirName(const char* path);
std::string FileName(const char* path);
std::string JoinPath(const char* dir, const char* path);
std::string FileExtension(const char* path);
std::array<std::string, 2> SplitIntoDirNameAndFileName(const char* path);

template <typename string_0, typename string_1>
std::string Concat(string_0&& str0, string_1&& str1) {
  return str0 + str1;
}
template <typename string_0, typename string_1, typename string_2,
          typename... string_n>
std::string Concat(string_0&& str0, string_1&& str1, string_2&& str2,
                   string_n... strn) {
  return Concat(
      concat(std::forward<string_0>(str0), std::forward<string_1>(str1)), str2,
      strn...);
}

template <typename string_0, typename string_1>
std::string ConcatWithDelimiter(const char* delimiter, string_0&& str0,
                                string_1&& str1) {
  return str0 + delimiter + str1;
}

template <typename string_0, typename string_1, typename string_2,
          typename... string_n>
std::string ConcatWithDelimiter(const char* delimiter, string_0&& str0,
                                string_1&& str1, string_2&& str2,
                                string_n... strn) {
  return Concat(delimiter,
                Concat(delimiter, std::forward<string_0>(str0),
                       std::forward<string_1>(str1)),
                str2, strn...);
}

bool StartsWith(const char* string_0, const char* string_1);

template <typename T>
T ToNumber(const std::string& str) {
  /** fallback to int */
  return (T)std::stoi(str);
}

#define FS_SUN_STRING_DEFINE_TO_NUMBER(ret_type, suffix_of_stox) \
  template <>                                                    \
  inline ret_type ToNumber(const std::string& str) {             \
    return std::sto##suffix_of_stox(str);                        \
  }

FS_SUN_STRING_DEFINE_TO_NUMBER(int, i)
FS_SUN_STRING_DEFINE_TO_NUMBER(long, l)
FS_SUN_STRING_DEFINE_TO_NUMBER(long long, ll)
FS_SUN_STRING_DEFINE_TO_NUMBER(unsigned long, ul)
FS_SUN_STRING_DEFINE_TO_NUMBER(unsigned long long, ull)
FS_SUN_STRING_DEFINE_TO_NUMBER(float, f)
FS_SUN_STRING_DEFINE_TO_NUMBER(double, d)
FS_SUN_STRING_DEFINE_TO_NUMBER(long double, ld)

#undef FS_SUN_STRING_DEFINE_TO_NUMBER

template <typename T>
std::string ToString(const T& value) {
  using std::to_string;

  return to_string(value);
}

inline std::string ToString(const char* value) {
  return value != nullptr ? value : "";
}

inline std::string ToString(const std::string& value) { return value; }

inline std::string ToString(const bool& value) {
  return value ? "true" : "false";
}

template <typename T, typename TDeleter>
std::string ToString(const std::unique_ptr<T, TDeleter>& value) {
  if (value != nullptr) {
    return ToString(*value);
  } else
    return "nullptr";
}

// map
template <typename TOrderedOrUnorderedMap>
std::string MapToString(const TOrderedOrUnorderedMap& map) {
  std::string ret;
  ret += "{";
  for (const auto& pair : map) {
    ret += (ToString(pair.first) + " : " + ToString(pair.second) + ", ");
  }
  if (ret.back() == ' ') ret.erase(ret.end() - 2, ret.end());
  ret += "}";
  return ret;
}

template <typename TKey, typename TValue>
std::string ToString(const std::unordered_map<TKey, TValue>& value) {
  return MapToString(value);
}

template <typename TKey, typename TValue>
std::string ToString(const std::map<TKey, TValue>& value) {
  return MapToString(value);
}

// array
template <typename TElement>
std::string ArrayToString(const TElement* value, const std::size_t size) {
  std::string ret;
  ret += "[";
  for (std::size_t i = 0; i < size; i++) {
    ret += (ToString(value[i]) + ", ");
  }
  if (ret.back() == ' ') ret.erase(ret.end() - 2, ret.end());
  ret += "]";

  return ret;
}

template <typename TElement>
std::string ToString(const std::vector<TElement>& value) {
  return ArrayToString(value.data(), value.size());
}

template <typename TElement, std::size_t N>
std::string ToString(const TElement (&value)[N]) {
  return ArrayToString(&value[0], N);
}

template <typename TElement, std::size_t N>
std::string ToString(const std::array<TElement, N>& value) {
  return ArrayToString(value.data(), N);
}

}  // namespace string

FS_SUN_NS_END

#endif  // FS_SUN_STRING_H
