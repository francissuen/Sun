/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#include "json.h"

#include "file.h"

FS_SUN_NS_BEGIN

class Json::Meta {
  friend class Json;
  friend std::string to_string(const Json &value);
  Meta() = default;
  Meta(Json::Dictionary new_values) : values_{std::move(new_values)} {}
  Json::Dictionary values_;
};

template <>
const Json::ScalarValue &Json::GetScalarValue(const ScalarValue &value,
                                              bool &has_succeeded) {
  has_succeeded = true;
  return value;
}

Json::~Json() { FS_SUN_DEL_PTR(meta_) }

// TODO Get*
// const std::string &Json::Value::GetString() {
//   if (Is<std::string>())
//     return Get<std::string>();
//   else
//     return Get<ScalarValue>().Get<std::string>();
// }
const Json::TVectorValue<Json::Value> &GetArray();
const Json::TDictionary<Json::Value> &GetDictionary();
Json::Deserializer::Deserializer(Input &input, std::size_t &size)
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
        std::string name = std::get<std::string>(ReadString());
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
            FS_SUN_ERROR(
                std::string("Expect a comma while encountered token: ") +
                *input_ + ", " + input_.GetFormatedLineColumnNumberString())
            is_good_ = false;
            break;
          }
        }
      }
    } else {
      // found an empty elemen
      if (*input_ == token_rcb) {
        PassCurrentToken<token_rcb>();
      } else {
        FS_SUN_ERROR(std::string("Expect a rcb while encountered token: ") +
                     *input_ + ", " +
                     input_.GetFormatedLineColumnNumberString());
        is_good_ = false;
      }
      break;
    }
  }

  return ret;
}

bool Json::Deserializer::IsGood() const { return is_good_; }

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

const Json::Dictionary &Json::GetValues() const { return meta_->values_; }

Json::Deserializer::Input::Input(const char *input) : input_{input} {}

char Json::Deserializer::Input::operator*() const { return *input_; }

Json::Deserializer::Input &Json::Deserializer::Input::operator++() {
  AdvanceByOne();
  return *this;
}

const char *Json::Deserializer::Input::Get() const { return input_; }

Json::Deserializer::Input Json::Deserializer::Input::operator-(
    const std::size_t offset) {
  return {input_ - offset};
}

std::size_t Json::Deserializer::Input::GetLineNumber() const {
  return line_number_;
}

std::size_t Json::Deserializer::Input::GetColumnNumber() const {
  return column_number_;
}

std::string Json::Deserializer::Input::GetFormatedLineColumnNumberString()
    const {
  return sun::string::ToString(line_number_) + ":" +
         sun::string::ToString(column_number_);
}

Json::Deserializer::Input &Json::Deserializer::Input::operator+=(
    const std::size_t offset) {
  for (std::size_t i = 0u; i < offset; i++) {
    AdvanceByOne();
  }
  return *this;
}

void Json::Deserializer::Input::AdvanceByOne() {
  if (*input_ == token_lf) {
    ++line_number_;
    column_number_ = 0u;
  } else
    ++column_number_;
  ++input_;
}

Json::ScalarValue Json::Deserializer::ReadString() {
  /** should be at quote now */
  PassCurrentToken<token_quote>();

  std::string ret;

  if (size_ != 0u) {
    const char *const begin = input_.Get();
    if (size_ != 0u) {
      do {
        /** seek right quote */
        SeekToken<token_quote>();
        /** check the prior character if it's \ */
        if (*(input_ - 1u) != token_backslash) break;
      } while (size_ != 0u);

      if (size_ != 0u) {
        ret = {begin, input_.Get()};
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
  const char *begin = input_.Get();
  do {
    const char cur_char = *input_;
    if (cur_char == token_com || cur_char == token_rsb ||
        cur_char == token_rcb || cur_char == token_space ||
        cur_char == token_tab || cur_char == token_lf || cur_char == token_cr)
      break;

    input_ += 1u;
    size_ -= 1u;
  } while (size_ != 0);

  return std::string{begin, input_.Get()};
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
    ret = Value(ReadArray());
  else
    ret = ReadScalar();

  return ret;
}

std::string to_string(const Json &value) {
  return string::ToString(value.meta_->values_);
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

Json::Json() { meta_ = new Meta{}; }

Json::Json(const char *json_string)
    : Json{json_string, std::strlen(json_string)} {}

Json::Json(const char *json_string, std::size_t size) : Json{} {
  Parse(json_string, size);
}

Json::Json(Dictionary values) : meta_{new Meta{std::move(values)}} {}

void Json::Parse(const char *json_string, std::size_t size) {
  Deserializer::Input input{json_string};
  Deserializer d{input, size};
  meta_->values_ = d.Execute();
  is_good_ = d.IsGood();
}

bool Json::IsGood() const { return is_good_; }

JsonFile::JsonFile(const char *file_path) : file_{file_path} {}

bool JsonFile::Open() {
  if (file_.Open()) {
    std::vector<unsigned char> json_string = file_.Read();
    json_.Parse(((char *)json_string.data()), json_string.size());
    return json_.IsGood();
  } else
    return false;
}

const Json &JsonFile::GetJson() const { return json_; }

FS_SUN_NS_END