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
#include <variant>

#include "deep_ptr.h"
#include "file.h"
#include "logger.h"
#include "ns.h"

FS_SUN_NS_BEGIN

/**
 * \brief conforms to Standard ECMA-404
 */
class FS_SUN_API Json {
 public:
  template <typename T>
  using TDictionary = std::unordered_map<std::string, T>;

  /**
   * \brief scalar type: number, string, boolen or object
   * \note Use DeepPtr due to ScalarValue needs to be copyable
   */
  template <typename TValue>
  using TScalarValue = std::variant<std::string, DeepPtr<TDictionary<TValue>>>;

  template <typename TValue>
  friend std::string to_string(const TScalarValue<TValue> &scalar_value) {
    return std::visit(
        Overloaded{[](const auto &sv) { return string::ToString(sv); }},
        scalar_value);
  }

  template <typename TValue>
  using TVectorValue = std::vector<TScalarValue<TValue>>;
  class Value : public std::variant<TScalarValue<Value>, TVectorValue<Value>> {
   public:
    using BaseType = std::variant<TScalarValue<Value>, TVectorValue<Value>>;
    using BaseType::variant;

   public:
    const std::string &GetString();
    const TVectorValue<Value> &GetArray();
    const TDictionary<Value> &GetDictionary();
  };
  using ScalarValue = TScalarValue<Value>;
  using VectorValue = TVectorValue<Value>;
  using Dictionary = TDictionary<Value>;

 private:
  /** 6 structural tokens */
  static constexpr char token_lsb = '[';
  static constexpr char token_lcb = '{';
  static constexpr char token_rsb = ']';
  static constexpr char token_rcb = '}';
  static constexpr char token_col = ':';
  static constexpr char token_com = ',';

  /** 3 literal name tokens */
  static constexpr char const *token_true = "true";
  static constexpr char token_t = 't';
  static constexpr char const *token_false = "false";
  static constexpr char token_f = 'f';
  static constexpr char const *token_null = "null";
  static constexpr char token_n = 'n';

  /** insignificant whitespace */
  static constexpr char token_space = ' ';
  static constexpr char token_tab = '\t';
  static constexpr char token_lf = '\n';
  static constexpr char token_cr = '\r';

  /** quote token */
  static constexpr char token_quote = '\"';
  static constexpr char token_backslash = '\\';

 private:
  class Deserializer {
   public:
    class Input {
     public:
      Input(const char *input);

     public:
      char operator*() const;

      Input &operator++();

      Input &operator+=(const std::size_t offset);

      const char *Get() const;

      Input operator-(const std::size_t offset);

      std::size_t GetLineNumber() const;
      std::size_t GetColumnNumber() const;
      std::string GetFormatedLineColumnNumberString() const;

     private:
      const char *input_;
      std::size_t line_number_{0u};
      std::size_t column_number_{0u};

     private:
      inline void AdvanceByOne();
    };

   public:
    Deserializer(Input &input, std::size_t &size);

   public:
    Dictionary Execute();
    bool IsGood() const;

   private:
    /** Seek token */
    template <char token>
    void SeekToken() {
      std::size_t i = 0u;
      for (; i < size_; i++) {
        if (*input_ == token) break;

        ++input_;
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

        ++input_;
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
    Input &input_;  // use reference because it needs to
                    // be called recursively
    std::size_t &size_;
    bool is_good_{true};
  };

 public:
  friend std::string to_string(const Json::Value &value) {
    return std::visit(Overloaded{[](const auto &v) -> std::string {
                        return string::ToString(v);
                      }},
                      static_cast<const Value::BaseType &>(value));
  }

  friend std::string to_string(const Json &value);

#define FS_SUN_JSON_GET_VALUE_(value_type)                      \
  bool has_succeeded{false};                                    \
  const ScalarValue &sv = GetScalarValue(value, has_succeeded); \
  if (!has_succeeded) return false;                             \
  auto ptr_value = std::get_if<value_type>(&sv);                \
  if (ptr_value == nullptr) return false;                       \
  const value_type &rv = *ptr_value;

 public:
  template <typename TValue,
            typename TRet>  // TValue should be template argument, since it
                            // could be Value or ScalarValue
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
    const VectorValue *ptr_v_value = std::get_if<VectorValue>(&value);
    if (ptr_v_value == nullptr) return false;
    ret.resize(ptr_v_value->size());
    return UpdateVectorValue(*ptr_v_value, ret.data(), ret.size());
  }

  template <typename TValue, typename TRet, std::size_t N>
  static bool UpdateValue(const TValue &value, TRet (&ret)[N]) {
    const VectorValue *ptr_v_value = std::get_if<VectorValue>(&value);
    return ptr_v_value != nullptr ? UpdateVectorValue(*ptr_v_value, ret, N)
                                  : false;
  }

  template <typename TValue, typename TRet, std::size_t N>
  static bool UpdateValue(const TValue &value, std::array<TRet, N> &ret) {
    const VectorValue *ptr_v_value = std::get_if<VectorValue>(&value);
    return ptr_v_value != nullptr
               ? UpdateVectorValue(*ptr_v_value, ret.data(), N)
               : false;
  }

 private:
  static const ScalarValue &GetScalarValue(const Value &value,
                                           bool &has_succeeded) {
    auto ptr_scalar_value = std::get_if<ScalarValue>(&value);
    has_succeeded = ptr_scalar_value != nullptr ? true : false;
    return *ptr_scalar_value;
  }

  static const ScalarValue &GetScalarValue(const ScalarValue &s_value,
                                           bool &has_succeeded) {
    has_succeeded = true;
    return s_value;
  }

  template <typename TRet>
  static bool UpdateVectorValue(const VectorValue &v_value, TRet *ret,
                                const std::size_t ret_array_size) {
    const std::size_t value_size = v_value.size();
    const std::size_t min_size =
        ret_array_size >= value_size ? value_size : ret_array_size;
    for (std::size_t i = 0; i < min_size; i++) {
      if (!UpdateValue(v_value[i], ret[i])) {
        FS_SUN_ERROR("Failed to UpdateValue for array at index: " +
                     string::ToString(i));
        return false;
      }
    }
    return true;
  }

 public:
  Json() = default;
  Json(const char *json_string);
  Json(const char *json_string, std::size_t size);

  Json(Dictionary values);

 public:
  void Parse(const char *json_string, std::size_t size);
  bool IsGood() const;
  const Dictionary &GetValues() const;

 protected:
  Dictionary values_;
  bool is_good_{true};
};

// TODO, support multi files json
// { "my_obj" = {"member0":1}}
// or {"my_obj": "path_to_my_obj_json_file"}, in this case, the my_obj stored
// in a sperated json file
class FS_SUN_API JsonFile {
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
    } else {                                                    \
      FS_SUN_ERROR("Failed to Open JsonFile for file_path: " +  \
                   std::string(file_path));                     \
      return false;                                             \
    }                                                           \
  }                                                             \
  inline bool ParseFromJson(const char *json_string) {          \
    fs::sun::Json j(json_string);                               \
    if (j.IsGood())                                             \
      return ParseFromJsonDictionary(j.GetValues());            \
    else {                                                      \
      FS_SUN_ERROR("Failed to parse json string");              \
      return false;                                             \
    }                                                           \
  }                                                             \
  inline bool ParseFromJson(const char *json_string,            \
                            const std::size_t size) {           \
    fs::sun::Json j(json_string, size);                         \
    if (j.IsGood())                                             \
      return ParseFromJsonDictionary(j.GetValues());            \
    else {                                                      \
      FS_SUN_ERROR("Failed to parse json string");              \
      return false;                                             \
    }                                                           \
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
