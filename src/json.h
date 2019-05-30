/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include <cstdint>
FS_SUN_NS_BEGIN

/**
 * \brief conform to Standard ECMA-404
 * { obj_name : [ false, true ], obj_name_2 : 1, obj_name_3 : 3}
 */
namespace json
{
/** here are six structural tokens */
    constexpr char STR_TOKEN_LSB = u8"["[0];
    constexpr char STR_TOKEN_LCB = u8"{"[0];
    constexpr char STR_TOKEN_RSB = u8"]"[0];
    constexpr char STR_TOKEN_RCB = u8"}"[0];
    constexpr char STR_TOKEN_COL = u8":"[0];
    constexpr char STR_TOKEN_COM = u8","[0];
/** three literal name tokens */
    constexpr char* LIT_NM_TOKEN_TRUE = u8"true";
#define FS_SUN_JSON_LIT_NM_TOKEN_F u8"false"
#define FS_SUN_JSON_LIT_NM_TOKEN_N u8"null"
/** insignificant whitespace */
#define FS_SUN_JSON_INS_WHI_TAB u8"\t"[0]
#define FS_SUN_JSON_INS_WHI_LF u8"\n"[0]
#define FS_SUN_JSON_INS_WHI_CR u8"\r"[0]

/** seek significant character */
    char* seek_for_significant_character(char* input, const std::size_t size)
    {
    for(std::size_t i = 0; i < size; i++)
    {
        if(input[i] != FS_SUN_JSON_INS_WHI_TAB ||
           input[i] != FS_SUN_JSON_INS_WHI_LF ||
           input[i] != FS_SUN_JSON_INS_WHI_CR)
            return input + i;
    }
    return nullptr;
    }

           void parse(char* input)
            {
            }
}
FS_SUN_NS_END
