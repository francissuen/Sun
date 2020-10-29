/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#include "json.h"
#include "file.h"
#include "logger.h"

using namespace fs::sun;

template <>
const Json::ScalarValue &Json::GetScalarValue(const ScalarValue &value) {
  return value;
}

Json::Deserializer::Deserializer(const char *&input, std::size_t &size)
    : input_{input}, size_(size) {}

Json::Dictionary Json::Deserializer::Execute() {
  Dictionary ret;
  /** seek lcb */
  SeekToken<token_lcb>();
  AdvanceCurrentToken<token_lcb>();

  while (size_ != 0u) {
    /** seek left quote of name */
    SeekToken<token_quote>();
    if (size_ != 0u) {
      std::string name = ReadString().Get<std::string>();
      SeekToken<token_col>();
      AdvanceCurrentToken<token_col>();
      if (size_ != 0u) {
        ret.insert(std::make_pair(std::move(name), ReadValue()));

        AdvanceUntilSignificant();

        if (*input_ == token_rcb) {
          AdvanceCurrentToken<token_rcb>();
          break;
        }

        if (*input_ != token_com) {
          FS_SUN_LOG(std::string("Expect a comma while encounter a @token: ") +
                         *input_,
                     Logger::S_ERROR);
          break;
        }
      }
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

const Json::Dictionary &Json::GetValues() const { return values_; }

/* Json::Status Json::GetStatus() const
{
    return status_;
}
 */
Json::ScalarValue Json::Deserializer::ReadString() {
  /** should be at quote now */
  AdvanceCurrentToken<token_quote>();

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
        AdvanceCurrentToken<token_quote>();
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

  return DeepPtr<Json>(std::move(values));
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
  AdvanceCurrentToken<token_lsb>();

  if (size_ != 0u) {
    do {
      ret.push_back(ReadScalar());

      AdvanceUntilSignificant();
      if (*input_ == token_rsb) {
        AdvanceCurrentToken<token_rsb>();
        break;
      } else
        AdvanceCurrentToken<token_com>();
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
    FS_SUN_LOG("invalid string @str: " + str, Logger::S_WARNING);
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

Json::operator const Dictionary &() const { return GetValues(); }

void Json::Parse(const char *json_string, std::size_t size) {
  Deserializer d{json_string, size};
  values_ = d.Execute();
}

JsonFile::JsonFile(const char *file_path) {
  File f{file_path};
  if (f.IsGood()) {
    std::vector<char> json_string = f.Read();
    Parse(json_string.data(), json_string.size());
  }
}
