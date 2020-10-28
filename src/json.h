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
  /**
   * \brief scalar type value, number, string, boolen or object
   */
  using ScalarValue = Variant<std::string, DeepPtr<Json>>;

  /**
   * \brief for array or multidimensional array
   */
  using VectorValue = std::vector<ScalarValue>;

  using Value = Variant<ScalarValue, VectorValue>;

  template <typename T>
  using Dictionary = std::unordered_map<std::string, T>;

  /*
      enum struct Status : std::uint8_t
      {
          UNINITIALIZED = 0u,
          OK,
          BAD
      };
   */
 private:
  class Deserializer {
   public:
    Deserializer(const char *&input, std::size_t &size);

   public:
    Dictionary<Value> Execute();

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

    template <char token>
    void AdvanceCurrentToken() {
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
    const auto &j_ptr = GetScalarValue(value).template Get<DeepPtr<Json>>();
    if (j_ptr != nullptr) ret.ParseFromJson(*j_ptr);
  }

  template <typename TValue, typename TRet>
  static void UpdateValue(const TValue &value, TRet *&ret) {
    const auto &j_ptr = GetScalarValue(value).template Get<DeepPtr<Json>>();
    if (j_ptr != nullptr) {
      ret = new TRet;
      ret->ParseFromJson(*j_ptr);
    }
  }

  template <typename TValue, typename TRet>
  static void UpdateValue(const TValue &value, std::unique_ptr<TRet> &ret) {
    const auto &j_ptr = GetScalarValue(value).template Get<DeepPtr<Json>>();
    if (j_ptr != nullptr) {
      ret.reset(new TRet);
      ret->ParseFromJson(*j_ptr);
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
  static void UpdateValue(const TValue &value, Json &ret) {
    ret = *(GetScalarValue(value).template Get<DeepPtr<Json>>());
  }

  /* dictionary type */
  template <typename TValue, typename TRet>
  static void UpdateValue(const TValue &value, Dictionary<TRet> &ret) {
    const auto &j_ptr = GetScalarValue(value).template Get<DeepPtr<Json>>();
    if (j_ptr != nullptr) {
      const auto &values = j_ptr->GetValues();
      for (const auto &value : values) {
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
  Json(const char *buffer, std::size_t size);

  Json(Dictionary<Value> &&values);

 public:
  const Dictionary<Value> &GetValues() const;
  /* Status GetStatus() const; */

 private:
  Dictionary<Value> values_;
  /*     Status status_; */
};

template <>
const Json::ScalarValue &Json::GetScalarValue(const ScalarValue &value);

#define FS_SUN_JSON_REGISTER_OBJECT_BEGIN()                         \
  void ParseFromJson(const char *input, const std::size_t size) {   \
    fs::sun::Json j(input, size);                                   \
    ParseFromJson(j);                                               \
  }                                                                 \
                                                                    \
  void ParseFromJson(const fs::sun::Json &j) {                      \
    const fs::sun::Json::Dictionary<fs::sun::Json::Value> &values = \
        j.GetValues();

#define FS_SUN_JSON_REGISTER_OBJECT_MEMBER(member_variable)           \
  {                                                                   \
    const auto &itr = values.find(#member_variable);                  \
    if (itr != values.end()) {                                        \
      fs::sun::Json::UpdateValue(itr->second, this->member_variable); \
    }                                                                 \
  }

#define FS_SUN_JSON_REGISTER_OBJECT_END() }

FS_SUN_NS_END

#endif  // FS_SUN_JSON_H
