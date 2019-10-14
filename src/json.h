/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include <cstdint>
#include <type_traits>
#include <limits>
#include "string.h"
#include <unordered_map>
#include "debug.h"
#include <cstring>

FS_SUN_NS_BEGIN

/**
 * \brief conform to Standard ECMA-404
 */
class Json
{
public:
    struct RawValue
    {
        const char* begin_;
        std::size_t size_;
        std::string Refine() const;
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

public:
    template<typename T>
    static typename std::enable_if<std::is_class<T>::value>::type
    UpdateValue(const RawValue & raw_value, T & ret)
    {
        ret.ParseFromJson(raw_value.begin_, raw_value.size_);
    }

    template<typename T>
    static typename std::enable_if<std::is_arithmetic<T>::value>::type
    UpdateValue(const RawValue & raw_value, T & ret)
    {
        ret = string::ToNumber<T>(raw_value.Refine());
    }

    template<typename T>
    static typename std::enable_if<std::is_array<T>::value>::type
    UpdateValue(const RawValue & raw_value, T & ret)
    {
        /** remove [], find comma */
        static constexpr std::size_t array_size = fs::sun::CountOfArray(ret);
        const char* raw_begin = raw_value.begin_;
        std::size_t raw_size = raw_value.size_;

        /** advance pass left square brace */
        std::size_t i = 0u;
        for(; i < raw_size; i++)
        {
            if(raw_begin[i] == token_lsb)
            {
                i++;
                break;
            }
        }
        raw_size -= i;
        raw_begin += i;

        for(std::size_t array_index = 0u; array_index < array_size; array_index++)
        {
            const char* element_begin = raw_begin;
            for(i = 0u; i < raw_size; i++)
            {
                if(raw_begin[i] == token_com)
                    break;
            }
            
            if(i == 0u)
                break;

            UpdateValue(RawValue{element_begin, i}, ret[array_index]);
            raw_size -= i;
            raw_begin += i;
        }
    }

    static bool StringToBoolean(const std::string & str);

    static void UpdateValue(const RawValue & raw_value, bool & ret);

    static void UpdateValue(const RawValue & raw_value, std::string & ret);

public:
    Json(const char* const  input, const std::size_t size);

    Json(const char* const  sz_input);

public:
    bool Initialize();

    const std::unordered_map<std::string, RawValue> & GetVariables() const;

private:
    const char* input_;
    std::size_t size_;
    std::unordered_map<std::string, RawValue> raw_variables_;
    
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

    void Advance(const std::size_t offset);
    
    /** seek significant character */
    /** void SeekSignificantCharacter(); */
    
    /** std::string ReadNonStringTypeValue(); */

    std::string ReadString();
    /** std::string ReadArray(); */
    RawValue ReadValue();
};


#define FS_SUN_JSON_REGISTER_OBJECT_BEGIN(class_name)                   \
    void ParseFromJson(const char* input, const std::size_t size)       \
    {                                                                   \
    fs::sun::Json j(input, size);                                       \
    if(j.Initialize())                                                  \
    {                                                                   \
    const std::unordered_map<std::string, fs::sun::Json::RawValue> & variables = j.GetVariables();


#define FS_SUN_JSON_REGISTER_OBJECT_MEMBER(member_variable)             \
    {                                                                   \
        const auto & itr = variables.find(#member_variable);            \
        if(itr != variables.end())                                      \
        {                                                               \
            fs::sun::Json::UpdateValue(itr->second, member_variable);   \
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
