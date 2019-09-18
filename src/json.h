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
 * { obj_name : [ false, true ], obj_name_2 : 1, obj_name_3 : 3}
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
    
/** seek significant character */
    static constexpr std::size_t invalid_position = std::numeric_limits<std::size_t>::max();
    std::size_t SeekSignificantCharacter(const char* const input, const std::size_t size)
    {
        char current_char{};
        for(std::size_t i = 0; i < size; i++)
        {
            current_char = input[i];
            if(current_char != token_tab &&
               current_char != token_lf &&
               current_char != token_cr)
            {
                return i;
            }
        }
        return invalid_position;
    }
    
    std::size_t SeekRightDelimiter(const char* const input, const std::size_t size)
    {
        char current_char{};
        for(std::size_t i = 0; i < size; i++)
        {
            current_char = input[i];
            if(current_char == token_tab ||
               current_char == token_lf ||
               current_char == token_cr ||
               current_char == token_rcb)
            {
                return i;
            }
        }
        return invalid_position;
    }

    template<typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, int>::type
    JsonGet(const char* const input, const std::size_t size)
    {
        const std::size_t begin = SeekSignificantCharacter(input, size);
        return fs::sun::string::ToNumber<T>(std::string(input + begin, input + SeekRightDelimiter(input,
                                                                                                  size - begin)));
    }

#define JSON_NAME(name, ...) __VA_ARGS__ name #name
#define FS_SUN_JSON_REGISTER_OBJECT_BEGIN(class_name)                 \
    template<>
    
    class_name JsonDecode(const char* input, std::size_t size)              \
    {                                                           \
    class_name obj;

#define FS_SUN_JSON_REGISTER_NAME(name, ...)        \
    obj.name = JsonDecode<__VA_ARGS__>(input, size);    \
    }
    struct Test
    {
        int a;
        bool b;
        void parse()
        {
            for(name in names)
            {
                a = parse_type(name)
            }
        }
    };
}
FS_SUN_NS_END
