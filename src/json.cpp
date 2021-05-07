/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#include "json.h"

#include "file.h"

using namespace fs::sun;

template <>
const Json::ScalarValue &Json::GetScalarValue(const ScalarValue &value) {
  return value;
}

#warning TODO Get*
// const std::string &Json::Value::GetString() {
//   if (Is<std::string>())
//     return Get<std::string>();
//   else
//     return Get<ScalarValue>().Get<std::string>();
// }
const Json::TVectorValue<Json::Value> &GetArray();
const Json::TDictionary<Json::Value> &GetDictionary();
Json::Deserializer::Deserializer(const char *&input, std::size_t &size)
    : input_{input}, size_(size) {}

Json::Dictionary Json::Deserializer::Execute() {
  Dictionary ret;
  /** seek lcb */
  SeekToken<token_lcb>();
  PassCurrentToken<token_lcb>();

  while (size_ != 0u) {
    /** seek left quote of name */
    if (SeekToken<token_quote, token_rcb>()) {
      if (size_ != 0u) {
        std::string name = ReadString().Get<std::string>();
        SeekToken<token_col>();
        PassCurrentToken<token_col>();
        if (size_ != 0u) {
          ret.insert(std::make_pair(std::move(name), ReadValue()));

          AdvanceUntilSignificant();

          if (*input_ == token_rcb) {
            PassCurrentToken<token_rcb>();
            break;
          }

          if (*input_ != token_com) {
            FS_SUN_WARN(std::string("Expect a comma while encounter token: ") +
                        *input_)
            break;
          }
        }
      }
    } else {
      // found an empty elemen
      if (*input_ == token_rcb) {
        PassCurrentToken<token_rcb>();
      } else {
        FS_SUN_WARN(std::string("Expect a rcb while encounter token: ") +
                    *input_);
      }
      break;
    }
  }

  return ret;
}

void Json::Deserializer::AdvanceUntilSignificant() {
  while (size_ != 0u) {
    const char cur_char = *input_;
    if (cur_char != token_space && cur_char != token_tab &&
        cur_char != token_lf && cur_char != token_cr)
      break;

    input_ += 1u;
    size_ -= 1u;
  }
}

const Json::Dictionary &Json::GetValues() const & { return values_; }
Json::Dictionary Json::GetValues() && { return std::move(values_); }

Json::ScalarValue Json::Deserializer::ReadString() {
  /** should be at quote now */
  PassCurrentToken<token_quote>();

  std::string ret;

  if (size_ != 0u) {
    const char *const begin = input_;
    if (size_ != 0u) {
      do {
        /** seek right quote */
        SeekToken<token_quote>();
        /** check the prior character if it's \ */
        if (*(input_ - 1u) != token_backslash) break;
      } while (size_ != 0u);

      if (size_ != 0u) {
        ret = {begin, input_};
        PassCurrentToken<token_quote>();
      }
    }
  }

  return ret;
}

Json::ScalarValue Json::Deserializer::ReadObject() {
  Deserializer d{input_, size_};
  auto values = d.Execute();
  input_ = d.input_;
  size_ = d.size_;

  return DeepPtr<Dictionary>(std::move(values));
}

Json::ScalarValue Json::Deserializer::ReadOthers() {
  const char *begin = input_;
  do {
    const char cur_char = *input_;
    if (cur_char == token_com || cur_char == token_rsb ||
        cur_char == token_rcb || cur_char == token_space ||
        cur_char == token_tab || cur_char == token_lf || cur_char == token_cr)
      break;

    input_ += 1u;
    size_ -= 1u;
  } while (size_ != 0);

  return std::string{begin, input_};
}

Json::ScalarValue Json::Deserializer::ReadScalar() {
  ScalarValue ret;
  AdvanceUntilSignificant();
  const char cur_char = *input_;
  if (cur_char == token_quote) {  // string
    ret = ReadString();
  } else if (cur_char == token_lcb) {  // object
    ret = ReadObject();
  } else {  // number, ture, false, null
    ret = ReadOthers();
  }
  return ret;
}

Json::VectorValue Json::Deserializer::ReadArray() {
  VectorValue ret;
  PassCurrentToken<token_lsb>();

  if (size_ != 0u) {
    do {
      AdvanceUntilSignificant();
      if (*input_ != token_rsb) {
        ret.push_back(ReadScalar());

        if (SeekToken<token_com, token_rsb>()) PassCurrentToken<token_com>();
      } else {
        PassCurrentToken<token_rsb>();
        break;
      }
    } while (size_ != 0u);
  }

  return ret;
}

Json::Value Json::Deserializer::ReadValue() {
  AdvanceUntilSignificant();

  const char cur_char = *input_;
  Value ret;
  if (cur_char == token_lsb)
    ret = ReadArray();
  else
    ret = ReadScalar();

  return ret;
}

bool Json::StringToBoolean(const std::string &str) {
  if (str.compare("true") == 0)
    return true;
  else if (str.compare("false") == 0)
    return false;
  else {
    FS_SUN_ERROR("invalid string @str: " + str);
    return false;
  }
}

Json::Json() {}

Json::Json(const char *json_string)
    : Json{json_string, std::strlen(json_string)} {}

Json::Json(const char *json_string, std::size_t size) {
  Parse(json_string, size);
}

Json::Json(Dictionary values) : values_{std::move(values)} {}

void Json::Parse(const char *json_string, std::size_t size) {
  Deserializer d{json_string, size};
  values_ = d.Execute();
}

JsonFile::JsonFile(const char *file_path) : file_{file_path} {}

bool JsonFile::Open() {
  if (file_.Open()) {
    std::vector<char> json_string = file_.Read();
    json_.Parse(json_string.data(), json_string.size());
    return true;
  } else
    return false;
}

const Json &JsonFile::GetJson() const { return json_; }
