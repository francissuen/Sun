/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_JSON_H
#define FS_SUN_JSON_H

#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>
#include <unordered_map>

#include "deep_ptr.h"
#include "file.h"
#include "logger.h"
#include "ns.h"
#include "variant.h"

FS_SUN_NS_BEGIN

/**
 * \brief conforms to Standard ECMA-404
 */
class Json {
 public:
  template <typename T>
  using TDictionary = std::unordered_map<std::string, T>;

  /**
   * \brief scalar type: number, string, boolen or object
   * \note Use DeepPtr due to ScalarValue needs to be copyable
   */
  template <typename TValue>
  using TScalarValue = Variant<std::string, DeepPtr<TDictionary<TValue>>>;
  template <typename TValue>
  using TVectorValue = std::vector<TScalarValue<TValue>>;
  class Value : public Variant<TScalarValue<Value>, TVectorValue<Value>> {
   public:
    using Variant<TScalarValue<Value>, TVectorValue<Value>>::Variant;

   public:
    const std::string &GetString();
    const TVectorValue<Value> &GetArray();
    const TDictionary<Value> &GetDictionary();
  };
  using ScalarValue = TScalarValue<Value>;
  using VectorValue = TVectorValue<Value>;
  using Dictionary = TDictionary<Value>;
  using Array = VectorValue;

 private:
  class Deserializer {
   public:
    Deserializer(const char *&input, std::size_t &size);

   public:
    Dictionary Execute();

   private:
    /** Seek token */
    template <char token>
    void SeekToken() {
      std::size_t i = 0u;
      for (; i < size_; i++) {
        if (*input_ == token) break;

        input_++;
      }

      size_ -= i;
    }
    /** Seek token except found unexpected token */
    template <char token, char unexpected_token>
    bool SeekToken() {
      static_assert(token != unexpected_token,
                    "token should not equal unexpected_token");
      std::size_t i = 0u;
      for (; i < size_; i++) {
        if (*input_ == unexpected_token)
          return false;
        else if (*input_ == token)
          break;

        input_++;
      }

      size_ -= i;
      return true;
    }

    template <char token>
    void PassCurrentToken() {
      assert(token == *input_);
      input_ += 1u;
      size_ -= 1u;
    }

    void AdvanceUntilSignificant();

    ScalarValue ReadString();
    ScalarValue ReadObject();
    ScalarValue ReadOthers();

    ScalarValue ReadScalar();

    Value ReadValue();

    VectorValue ReadArray();

   private:
    const char *&input_;  // use reference because it needs to
                          // be called recursively
    std::size_t &size_;
  };

 public:
  friend std::string to_string(const Json &value) {
    return string::ToString(value.values_);
  }

 private:
  /** 6 structural tokens */
  static constexpr char token_lsb = u8"["[0];
  static constexpr char token_lcb = u8"{"[0];
  static constexpr char token_rsb = u8"]"[0];
  static constexpr char token_rcb = u8"}"[0];
  static constexpr char token_col = u8":"[0];
  static constexpr char token_com = u8","[0];

  /** 3 literal name tokens */
  static constexpr char const *token_true = u8"true";
  static constexpr char token_t = u8"t"[0];
  static constexpr char const *token_false = u8"false";
  static constexpr char token_f = u8"f"[0];
  static constexpr char const *token_null = u8"null";
  static constexpr char token_n = u8"n"[0];

  /** insignificant whitespace */
  static constexpr char token_space = u8" "[0];
  static constexpr char token_tab = u8"\t"[0];
  static constexpr char token_lf = u8"\n"[0];
  static constexpr char token_cr = u8"\r"[0];

  /** quote token */
  static constexpr char token_quote = u8"\""[0];
  static constexpr char token_backslash = u8"\\"[0];

#define FS_SUN_JSON_GET_VALUE_(value_type)                      \
  bool has_succeeded{false};                                    \
  const ScalarValue &sv = GetScalarValue(value, has_succeeded); \
  if (!has_succeeded) return false;                             \
  if (!(sv.Is<value_type>())) return false;                     \
  const value_type &rv = sv.RawGet<value_type>();

 public:
  template <typename TValue, typename TRet>
  static typename std::enable_if<std::is_class<TRet>::value, bool>::type
  UpdateValue(const TValue &value, TRet &ret) {
    FS_SUN_JSON_GET_VALUE_(DeepPtr<Dictionary>);
    if (rv != nullptr) {
      ret.ParseFromJsonDictionary(*rv);
      return true;
    } else
      return false;
  }

  template <typename TValue, typename TRet>
  static bool UpdateValue(const TValue &value, TRet *&ret) {
    FS_SUN_JSON_GET_VALUE_(DeepPtr<Dictionary>);
    if (rv != nullptr) {
      ret = new TRet;
      ret->ParseFromJsonDictionary(*rv);
      return true;
    } else
      return false;
  }

  template <typename TValue, typename TRet>
  static bool UpdateValue(const TValue &value, std::unique_ptr<TRet> &ret) {
    FS_SUN_JSON_GET_VALUE_(DeepPtr<Dictionary>);
    if (rv != nullptr) {
      ret.reset(new TRet);
      ret->ParseFromJsonDictionary(*rv);
      return true;
    } else
      return false;
  }

  template <typename TValue, typename TRet>
  static typename std::enable_if<std::is_arithmetic<TRet>::value, bool>::type
  UpdateValue(const TValue &value, TRet &ret) {
    FS_SUN_JSON_GET_VALUE_(std::string);
    ret = string::ToNumber<TRet>(rv);
    return true;
  }

  template <typename TValue, typename TRet>
  static typename std::enable_if<std::is_enum<TRet>::value, bool>::type
  UpdateValue(const TValue &value, TRet &ret) {
    if (!UpdateValue<TValue, int>(value, ret)) return false;
    ret = static_cast<TRet>(ret);
    return true;
  }

  static bool StringToBoolean(const std::string &str);

  template <typename TValue>
  static bool UpdateValue(const TValue &value, bool &ret) {
    FS_SUN_JSON_GET_VALUE_(std::string);
    ret = StringToBoolean(rv);
    return true;
  }

  template <typename TValue>
  static bool UpdateValue(const TValue &value, std::string &ret) {
    FS_SUN_JSON_GET_VALUE_(std::string);
    ret = rv;
    return true;
  }

  template <typename TValue>
  static bool UpdateValue(const TValue &value, Dictionary &ret) {
    FS_SUN_JSON_GET_VALUE_(DeepPtr<Dictionary>);
    ret = *(rv);
    return true;
  }

  /* dictionary type */
  template <typename TValue, typename TRet>
  static bool UpdateValue(const TValue &value, TDictionary<TRet> &ret) {
    FS_SUN_JSON_GET_VALUE_(DeepPtr<Dictionary>);
    if (rv != nullptr) {
      const auto &dict = *rv;
      for (const auto &value : dict) {
        TRet tmp_ret{};
        if (!UpdateValue(value.second, tmp_ret)) {
          FS_SUN_ERROR("Failed to UpdateValue for TDictionary at key: " +
                       value.first);
          return false;
        }
        ret.insert(std::make_pair(value.first, std::move(tmp_ret)));
      }
      return true;
    } else
      return false;
  }

  /* array type */
  template <typename TValue, typename TRet>
  static bool UpdateValue(const TValue &value, std::vector<TRet> &ret) {
    if (!value.template Is<VectorValue>()) return false;
    const auto &vector_value = value.template RawGet<VectorValue>();
    const std::size_t value_size = vector_value.size();

    for (std::size_t i = 0; i < value_size; i++) {
      TRet tmp_ret{};
      if (!UpdateValue(vector_value[i], tmp_ret)) {
        FS_SUN_ERROR("Failed to UpdateValue for std::vector at index: " +
                     string::ToString(i));
        return false;
      }
      ret.push_back(std::move(tmp_ret));
    }
    return true;
  }

  template <typename TValue, typename TRet, std::size_t N>
  static bool UpdateValue(const TValue &value, TRet (&ret)[N]) {
    return UpdateValue(value, &ret[0], N);
  }

  template <typename TValue, typename TRet, std::size_t N>
  static bool UpdateValue(const TValue &value, std::array<TRet, N> &ret) {
    return UpdateValue(value, ret.data(), N);
  }

 private:
  template <typename TValue>
  static const ScalarValue &GetScalarValue(const TValue &value,
                                           bool &has_succeeded) {
    has_succeeded = value.template Is<ScalarValue>();
    return value.template Get<ScalarValue>();
  }

  template <typename TValue, typename TRet>
  static bool UpdateValue(const TValue &value, TRet *ret,
                          const std::size_t array_size) {
    if (!(value.template Is<VectorValue>())) return false;
    const auto &vector_value = value.template RawGet<VectorValue>();
    const std::size_t value_size = vector_value.size();
    const std::size_t min_size =
        array_size >= value_size ? value_size : array_size;
    for (std::size_t i = 0; i < min_size; i++) {
      if (!UpdateValue(vector_value[i], ret[i])) {
        FS_SUN_ERROR("Failed to UpdateValue for array at index: " +
                     string::ToString(i));
        return false;
      }
    }
    return true;
  }

 public:
  Json();
  Json(const char *json_string);
  Json(const char *json_string, std::size_t size);

  Json(Dictionary values);

 public:
  void Parse(const char *json_string, std::size_t size);
  const Dictionary &GetValues() const &;
  Dictionary GetValues() &&;

 protected:
  Dictionary values_;
};

template <>
const Json::ScalarValue &Json::GetScalarValue(const ScalarValue &value,
                                              bool &has_succeeded);

class JsonFile {
 public:
  JsonFile(const char *file_path);

 public:
  bool Open();
  const Json &GetJson() const;

 private:
  Json json_;
  File file_;
};

#define FS_SUN_JSON_REGISTER_OBJECT_BEGIN()                     \
  inline bool ParseFromJsonFile(const char *file_path) {        \
    fs::sun::JsonFile jf(file_path);                            \
    if (jf.Open()) {                                            \
      return ParseFromJsonDictionary(jf.GetJson().GetValues()); \
    } else                                                      \
      return false;                                             \
  }                                                             \
  inline bool ParseFromJson(const char *json_string) {          \
    fs::sun::Json j(json_string);                               \
    return ParseFromJsonDictionary(j.GetValues());              \
  }                                                             \
  inline bool ParseFromJson(const char *json_string,            \
                            const std::size_t size) {           \
    fs::sun::Json j(json_string, size);                         \
    return ParseFromJsonDictionary(j.GetValues());              \
  }                                                             \
  inline bool ParseFromJson(const fs::sun::Json &j) {           \
    return ParseFromJsonDictionary(j.GetValues());              \
  }                                                             \
                                                                \
  inline bool ParseFromJsonDictionary(const fs::sun::Json::Dictionary &dict) {
#define FS_SUN_JSON_REGISTER_OBJECT_MEMBER(member_variable)                  \
  {                                                                          \
    const auto &itr = dict.find(#member_variable);                           \
    if (itr != dict.end()) {                                                 \
      if (!fs::sun::Json::UpdateValue(itr->second, this->member_variable)) { \
        FS_SUN_ERROR("Failed to UpdateValue for " #member_variable);         \
        return false;                                                        \
      }                                                                      \
    }                                                                        \
  }

#define FS_SUN_JSON_REGISTER_OBJECT_END() \
  return true;                            \
  }

// only works when the number of args is less or equal than 9
#define FS_SUN_JSON_REGISTER(...)                                       \
  FS_SUN_JSON_REGISTER_OBJECT_BEGIN()                                   \
  FS_SUN_CALL_FOR_EACH(FS_SUN_JSON_REGISTER_OBJECT_MEMBER, __VA_ARGS__) \
  FS_SUN_JSON_REGISTER_OBJECT_END()

FS_SUN_NS_END

#endif  // FS_SUN_JSON_H
