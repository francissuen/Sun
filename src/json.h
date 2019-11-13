/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>
#include <unordered_map>

#include "debug.h"
#include "dynamic_multidimensional_container.h"
#include "ns.h"

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
  using VectorValue = DynamicMultidimensionalVector<ScalarValue>;

  using Value = Variant<ScalarValue, VectorValue>;
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
    std::unordered_map<std::string, Value> Execute();

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
      FS_SUN_ASSERT(token == *input_);
      input_ += 1u;
      size_ -= 1u;
    }

    void AdvanceUntilSignificant();

    ScalarValue ReadString();
    ScalarValue ReadObject();
    ScalarValue ReadOthers();

    /** return DeepPtr<VectorValue> if TRet == VectorValue::Element */
    template <typename TRet>
    static TRet VectorAdaptor(VectorValue &&value) {
      return std::move(value);
    }

    template <typename TRet>
    TRet ReadValue() {
      AdvanceUntilSignificant();

      const char cur_char = *input_;
      TRet ret;
      if (cur_char == token_quote) /** string */
      {
        ret = ReadString();
      } else if (cur_char == token_lcb) /** object */
      {
        ret = ReadObject();
      } else if (cur_char == token_lsb) /** array */
      {
        /* if it's read from ReadArray, then a multidimensional array is met */
        ret = VectorAdaptor<TRet>(ReadArray());
      } else /** number, ture, false, null */
      {
        ret = ReadOthers();
      }

      return ret;
    }

    VectorValue ReadArray();

   private:
    const char *&input_;
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
        value.template Get<ScalarValue>().template Get<DeepPtr<Json>>();
    if (j_ptr != nullptr) ret.ParseFromJson(*j_ptr);
  }

  template <typename TValue, typename TRet>
  static void UpdateValue(const TValue &value, TRet *&ret) {
    const auto &j_ptr =
        value.template Get<ScalarValue>().template Get<DeepPtr<Json>>();
    if (j_ptr != nullptr) {
      ret = new TRet;
      ret->ParseFromJson(*j_ptr);
    }
  }

  template <typename TValue, typename TRet>
  static void UpdateValue(const TValue &value, std::unique_ptr<TRet> &ret) {
    const auto &j_ptr =
        value.template Get<ScalarValue>().template Get<DeepPtr<Json>>();
    if (j_ptr != nullptr) {
      ret.reset(new TRet);
      ret->ParseFromJson(*j_ptr);
    }
  }

  template <typename TValue, typename TRet>
  static typename std::enable_if<std::is_arithmetic<TRet>::value>::type
  UpdateValue(const TValue &value, TRet &ret) {
    ret = string::ToNumber<TRet>(
        value.template Get<ScalarValue>().template Get<std::string>());
  }

  template <typename TValue, typename TRet, std::size_t N>
  static void UpdateValue(const TValue &value, TRet (&ret)[N]) {
    const auto &vector_value = value.template Get<VectorValue>();
    const std::size_t value_size = vector_value.Size();
    const std::size_t min_size = N >= value_size ? value_size : N;

    for (std::size_t i = 0; i < min_size; i++) {
      UpdateValue(vector_value[i], ret[i]);
    }
  }

  static bool StringToBoolean(const std::string &str);

  template <typename TValue>
  static void UpdateValue(const TValue &value, bool &ret) {
    ret = StringToBoolean(
        value.template Get<ScalarValue>().template Get<std::string>());
  }

  template <typename TValue>
  static void UpdateValue(const TValue &value, std::string &ret) {
    ret = value.template Get<ScalarValue>().template Get<std::string>();
  }

  template <typename TValue>
  static void UpdateValue(const TValue &value, Json &ret) {
    ret = *(value.template Get<ScalarValue>().template Get<DeepPtr<Json>>());
  }

 private:
  template <typename TRet, std::size_t N>
  static void UpdateValue(const VectorValue::Element &value, TRet (&ret)[N]) {
    const auto &vector_value = value.Get<DeepPtr<VectorValue>>();
    if (vector_value != nullptr) {
      const std::size_t value_size = vector_value->Size();
      const std::size_t min_size = N >= value_size ? value_size : N;

      for (std::size_t i = 0; i < min_size; i++) {
        UpdateValue(vector_value->operator[](i), ret[i]);
      }
    }
  }

 public:
  Json();
  Json(const char *input, std::size_t size);

  Json(const char *sz_input);

  Json(std::unordered_map<std::string, Value> &&values);

 public:
  const std::unordered_map<std::string, Value> &GetValues() const;
  /* Status GetStatus() const; */

 private:
 private:
  std::unordered_map<std::string, Value> values_;
  /*     Status status_; */
};

#define FS_SUN_JSON_REGISTER_OBJECT_BEGIN(class_name)                     \
  void ParseFromJson(const char *input, const std::size_t size) {         \
    fs::sun::Json j(input, size);                                         \
    ParseFromJson(j);                                                     \
  }                                                                       \
                                                                          \
  void ParseFromJson(const fs::sun::Json &j) {                            \
    const std::unordered_map<std::string, fs::sun::Json::Value> &values = \
        j.GetValues();

#define FS_SUN_JSON_REGISTER_OBJECT_MEMBER(member_variable)     \
  {                                                             \
    const auto &itr = values.find(#member_variable);            \
    if (itr != values.end()) {                                  \
      fs::sun::Json::UpdateValue(itr->second, member_variable); \
    }                                                           \
  }

#define FS_SUN_JSON_REGISTER_OBJECT_END() }

FS_SUN_NS_END
