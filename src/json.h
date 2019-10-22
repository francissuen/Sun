/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include <cstdint>
#include <type_traits>
#include <limits>
#include <unordered_map>
#include "debug.h"
#include <cstring>
#include "variant.h"

FS_SUN_NS_BEGIN

/**
 * \brief conforms to Standard ECMA-404
 */
class Json
{
    template<typename T, template <typename ...> class container>
    struct DynamicMultidimentionalContainer
    {
        Variant<T, container<DynamicMultidimentionalContainer*>> data;
    };
    
public:
    using ScalarValue = Variant<std::string, std::unique_ptr<Json>>;
    using Value = Variant<ScalarValue, std::vector<void*>>;
    
    enum struct Status : std::uint8_t
    {
        UNINITIALIZED = 0u,
        OK,
        BAD
        };    
    
private:
    
/** 6 structural tokens */
    static constexpr char token_lsb = u8"["[0];
    static constexpr char token_lcb = u8"{"[0];
    static constexpr char token_rsb = u8"]"[0];
    static constexpr char token_rcb = u8"}"[0];
    static constexpr char token_col = u8":"[0];
    static constexpr char token_com = u8","[0];
    
/** 3 literal name tokens */
    static constexpr char const * token_true = u8"true";
    static constexpr char token_t = u8"t"[0];
    static constexpr char const * token_false = u8"false";
    static constexpr char token_f = u8"f"[0];
    static constexpr char const * token_null =  u8"null";
    static constexpr char token_n = u8"n"[0];
    
/** insignificant whitespace */
    static constexpr char token_space = u8" "[0];
    static constexpr char token_tab =  u8"\t"[0];
    static constexpr char token_lf = u8"\n"[0];
    static constexpr char token_cr = u8"\r"[0];
    
    /** quote token */
    static constexpr char token_quote = u8"\""[0];
    static constexpr char token_backslash = u8"\\"[0];

public:
    
    template<typename T>
    static typename std::enable_if<std::is_class<T>::value>::type
    UpdateValue(const Value & value, T & ret, const std::size_t index)
    {
        const auto & j_ptr = value.Get<ScalarValue>().Get<std::unique_ptr<Json>>();
        if(j_ptr != nullptr)
            ret.ParseFromJson(*j_ptr);
    }

    template<typename T>
    static typename std::enable_if<std::is_arithmetic<T>::value>::type
    UpdateValue(const Value & value, T & ret, const std::size_t index)
    {
        ret = string::ToNumber<T>(value.Get<ScalarValue>().Get<std::string>());
    }

    template<typename T>
    static typename std::enable_if<std::is_array<T>::value>::type
    UpdateValue(const Value & value, T & ret)
    {
        static constexpr std::size_t array_size = fs::sun::CountOfArray(ret);
        const auto & values = value.Get<VectorValue>();
        const std::size_t value_size = values.size();
        const std::size_t min_size = array_size >= value_size? value_size : array_size;

        for(std::size_t i = 0; i < min_size; i++)
        {
            UpdateValue(values[i], ret[i], i);
        }
    }

    static bool StringToBoolean(const std::string & str);

    static void UpdateValue(const Value & value, bool & ret, const std::size_t index);

    static void UpdateValue(const Value & value, std::string & ret, const std::size_t index);

public:
    
    Json(const char* const  input, const std::size_t size);

    Json(const char* const  sz_input);

public:
    
    bool Initialize();

    const std::unordered_map<std::string, Value> & GetValues() const;

private:
    
    /** Seek token */
    template<char token>
    void SeekToken()
    {
        std::size_t i = 0u;
        for(; i < size_; i++)
        {
            if(*input_ == token)
                break;

            input_++;
        }
        
        size_ -= i;
    }

    template<char token>
    void AdvanceCurrentToken()
    {
        FS_SUN_ASSERT(token == *input_);
        input_ += 1u;
        size_ -= 1u;
    }

    void AdvanceUntilSignificant();

    /** void Advance(const std::size_t offset); */
    
    /** seek significant character */
    /** void SeekSignificantCharacter(); */
    
    /** std::string ReadNonStringTypeValue(); */

    std::string ReadString();
    std::unique_ptr<Json> ReadObject();
    Value ReadArray();
    std::string ReadOthers();
    Value ReadValue();
    
private:
    
    const char* input_;
    std::size_t size_;
    std::unordered_map<std::string, Value> values_;
    Status status_;
};


#define FS_SUN_JSON_REGISTER_OBJECT_BEGIN(class_name)                   \
    void ParseFromJson(const char* input, const std::size_t size)       \
    {                                                                   \
        fs::sun::Json j(input, size);                                   \
        if(j.Initialize())                                              \
            ParseFromJson(j);                                           \
    }                                                                   \
                                                                        \
    void ParseFromJson(const fs::sun::Json & j)                         \
    {                                                                   \
        if(j.GetStatus() == fs::sun::Json::Status::OK)                  \
        {                                                               \
            const std::unordered_map<std::string, fs::sun::Json::Value> & values = j.GetValues();


#define FS_SUN_JSON_REGISTER_OBJECT_MEMBER(member_variable)             \
    {                                                                   \
        const auto & itr = values.find(#member_variable);               \
        if(itr != values.end())                                         \
        {                                                               \
            fs::sun::Json::UpdateValue(itr->second, member_variable, 0u); \
        }                                                               \
    }
    
    
#define FS_SUN_JSON_REGISTER_OBJECT_END()                               \
    }                                                                   \
    else                                                                \
    {                                                                   \
        FS_SUN_LOG("Json::Initialize failed", fs::sun::Logger::S_ERROR); \
    }                                                                   \
    }

FS_SUN_NS_END
