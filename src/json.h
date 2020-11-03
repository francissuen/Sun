/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_JSON_H
#define FS_SUN_JSON_H

#include <cassert>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>
#include <unordered_map>

#include "deep_ptr.h"
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
  };
  using ScalarValue = TScalarValue<Value>;
  using VectorValue = TVectorValue<Value>;
  using Dictionary = TDictionary<Value>;

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

 public:
  template <typename TValue, typename TRet>
  static typename std::enable_if<std::is_class<TRet>::value>::type UpdateValue(
      const TValue &value, TRet &ret) {
    const auto &j_ptr =
        GetScalarValue(value).template Get<DeepPtr<Dictionary>>();
    if (j_ptr != nullptr) ret.ParseFromDictionary(*j_ptr);
  }

  template <typename TValue, typename TRet>
  static void UpdateValue(const TValue &value, TRet *&ret) {
    const auto &j_ptr =
        GetScalarValue(value).template Get<DeepPtr<Dictionary>>();
    if (j_ptr != nullptr) {
      ret = new TRet;
      ret->ParseFromDictionary(*j_ptr);
    }
  }

  template <typename TValue, typename TRet>
  static void UpdateValue(const TValue &value, std::unique_ptr<TRet> &ret) {
    const auto &j_ptr =
        GetScalarValue(value).template Get<DeepPtr<Dictionary>>();
    if (j_ptr != nullptr) {
      ret.reset(new TRet);
      ret->ParseFromDictionary(*j_ptr);
    }
  }

  template <typename TValue, typename TRet>
  static typename std::enable_if<std::is_arithmetic<TRet>::value>::type
  UpdateValue(const TValue &value, TRet &ret) {
    ret = string::ToNumber<TRet>(
        GetScalarValue(value).template Get<std::string>());
  }

  template <typename TValue, typename TRet, std::size_t N>
  static void UpdateValue(const TValue &value, TRet (&ret)[N]) {
    const auto &vector_value = value.template Get<VectorValue>();
    const std::size_t value_size = vector_value.size();
    const std::size_t min_size = N >= value_size ? value_size : N;

    string::ToString(vector_value);
    for (std::size_t i = 0; i < min_size; i++) {
      UpdateValue(vector_value[i], ret[i]);
    }
  }

  static bool StringToBoolean(const std::string &str);

  template <typename TValue>
  static void UpdateValue(const TValue &value, bool &ret) {
    ret = StringToBoolean(GetScalarValue(value).template Get<std::string>());
  }

  template <typename TValue>
  static void UpdateValue(const TValue &value, std::string &ret) {
    ret = GetScalarValue(value).template Get<std::string>();
  }

  template <typename TValue>
  static void UpdateValue(const TValue &value, Dictionary &ret) {
    ret = *(GetScalarValue(value).template Get<DeepPtr<Dictionary>>());
  }

  /* dictionary type */
  template <typename TValue, typename TRet>
  static void UpdateValue(const TValue &value, TDictionary<TRet> &ret) {
    const auto &dict_ptr =
        GetScalarValue(value).template Get<DeepPtr<Dictionary>>();
    if (dict_ptr != nullptr) {
      const auto &dict = *dict_ptr;
      for (const auto &value : dict) {
        TRet tmp_ret{};
        UpdateValue(value.second, tmp_ret);
        ret.insert(std::make_pair(value.first, std::move(tmp_ret)));
      }
    }
  }

  /* std::vector */
  template <typename TValue, typename TRet>
  static void UpdateValue(const TValue &value, std::vector<TRet> &ret) {
    const auto &vector_value = value.template Get<VectorValue>();
    const std::size_t value_size = vector_value.size();

    for (std::size_t i = 0; i < value_size; i++) {
      TRet tmp_ret{};
      UpdateValue(vector_value[i], tmp_ret);
      ret.push_back(std::move(tmp_ret));
    }
  }

 private:
  template <typename TValue>
  static const ScalarValue &GetScalarValue(const TValue &value) {
    return value.template Get<ScalarValue>();
  }

 public:
  Json();
  Json(const char *json_string);
  Json(const char *json_string, std::size_t size);

  Json(Dictionary values);

 public:
  operator const Dictionary &() const;

 public:
  const Dictionary &GetValues() const;

 protected:
  Dictionary values_;

 protected:
  void Parse(const char *json_string, std::size_t size);
};

template <>
const Json::ScalarValue &Json::GetScalarValue(const ScalarValue &value);

class JsonFile : public Json {
 public:
  JsonFile(const char *file_path);
};

#define FS_SUN_JSON_REGISTER_OBJECT_BEGIN()              \
  inline void ParseFromJsonFile(const char *file_path) { \
    fs::sun::JsonFile jf(file_path);                     \
    ParseFromDictionary(jf);                             \
  }                                                      \
  inline void ParseFromJson(const char *json_string) {   \
    fs::sun::Json j(json_string);                        \
    ParseFromDictionary(j);                              \
  }                                                      \
  inline void ParseFromJson(const char *json_string,     \
                            const std::size_t size) {    \
    fs::sun::Json j(json_string, size);                  \
    ParseFromDictionary(j);                              \
  }                                                      \
                                                         \
  inline void ParseFromDictionary(const fs::sun::Json::Dictionary &dict) {
#define FS_SUN_JSON_REGISTER_OBJECT_MEMBER(member_variable)           \
  {                                                                   \
    const auto &itr = dict.find(#member_variable);                    \
    if (itr != dict.end()) {                                          \
      fs::sun::Json::UpdateValue(itr->second, this->member_variable); \
    }                                                                 \
  }

#define FS_SUN_JSON_REGISTER_OBJECT_END() }

FS_SUN_NS_END

#endif  // FS_SUN_JSON_H
