/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include <cstdint>
#include <type_traits>
#include <limits>
#include "string.h"

FS_SUN_NS_BEGIN

/**
 * \brief conform to Standard ECMA-404
 */
namespace json
{
/** 6 structural tokens */
    static constexpr char token_lsb = u8"["[0];
    static constexpr char token_lcb = u8"{"[0];
    static constexpr char token_rsb = u8"]"[0];
    static constexpr char token_rcb = u8"}"[0];
    static constexpr char token_col = u8":"[0];
    static constexpr char token_com = u8","[0];
/** 3 literal name tokens */
    static constexpr char const * token_true = u8"true";
    static constexpr char const * token_false = u8"false";
    static constexpr char const * token_null =  u8"null";
/** insignificant whitespace */
    static constexpr char token_space = u8" "[0];
    static constexpr char token_tab =  u8"\t"[0];
    static constexpr char token_lf = u8"\n"[0];
    static constexpr char token_cr = u8"\r"[0];
    
    /** quote token */
    static constexpr char token_quote = u8"\""[0];

    struct SeekingResult
    {
        const char* input_{nullptr};
        const std::size_t size_{0u};
    };
    
/** seek significant character */
    /** static constexpr std::size_t invalid_position = std::numeric_limits<std::size_t>::max(); */
    SeekingResult SeekSignificantCharacter(const char* input, const std::size_t size)
    {
        char current_char{};
        for(std::size_t i = 0; i < size; i++)
        {
            current_char = *input;
            if(current_char != token_tab &&
               current_char != token_lf &&
               current_char != token_cr)
                return {input, size - i};
            
            input++;
        }
        return {};
    }
    
    SeekingResult SeekStringDelimiter(const char* input, const std::size_t size)
    {
        char current_char{};
        for(std::size_t i = 0; i < size; i++)
        {
            current_char = *input;
            if(current_char == token_tab ||
               current_char == token_lf ||
               current_char == token_cr ||
               current_char == token_col)
                return {input, size - i};
            
            input++;
        }
        return {};
    }
    
    SeekingResult SeekValueRightDelimiter(const char* const input, const std::size_t size)
    {
        char current_char{};
        for(std::size_t i = 0; i < size; i++)
        {
            current_char = input[i];
            if(current_char == token_tab ||
               current_char == token_lf ||
               current_char == token_cr ||
               current_char == token_rcb)
                return {input, size - i};
        }
        return {};
    }

    std::string GetName(const char* const input, const std::size_t size)
    {
        const SeekingResult ret = SeekStringDelimiter(input, size);
        return std::string(ret.input_, SeekStringDelimiter(ret.input_, ret.size_).size_);
    }

    template<typename T>
    typename std::enable_if<std::is_class<T>::value, T>::type
    GetValue(const char* const input, const std::size_t size)
    {
        return T::ParseFromJson(input, size);
    }

    template<typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, T>::type
    GetValue(const char* const input, const std::size_t size)
    {
        const std::size_t begin = SeekSignificantCharacter(input, size);
        return fs::sun::string::ToNumber<T>(
            std::string(input + begin, input + SeekValueRightDelimiter(input + begin,
                                                                       size - begin)));
    }

#define FS_SUN_JSON_REGISTER_OBJECT_BEGIN(class_name)                   \
    static class_name ParseFromJson(const char* const input, const std::size_t size) \
    {                                                                   \
    class_name obj;

#define FS_SUN_JSON_REGISTER_OBJECT_MEMBER(name, ...)           \
    obj.name = json::GetValue<__VA_ARGS__>(input, size);
    
#define FS_SUN_JSON_REGISTER_OBJECT_END()       \
    return obj; }

    struct Test
    {
        int a;
        FS_SUN_JSON_REGISTER_OBJECT_BEGIN(Test)
        FS_SUN_JSON_REGISTER_OBJECT_MEMBER(a, int)
        FS_SUN_JSON_REGISTER_OBJECT_END()
    };
    
    struct Test2
    {
        int a;
        Test b;
        FS_SUN_JSON_REGISTER_OBJECT_BEGIN(Test2)
        FS_SUN_JSON_REGISTER_OBJECT_MEMBER(a, int)
        FS_SUN_JSON_REGISTER_OBJECT_MEMBER(b, Test)
        FS_SUN_JSON_REGISTER_OBJECT_END()
    };

    void foo(int a)
    {
        Test t = Test::ParseFromJson(nullptr, 0);
    }

}
FS_SUN_NS_END
