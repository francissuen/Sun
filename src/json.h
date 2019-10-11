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

FS_SUN_NS_BEGIN

/**
 * \brief conform to Standard ECMA-404
 */
class Json
{
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
    Json(const char* const  input, const std::size_t size):
        input_(input),
        size_(size)
    {}

public:
    bool Initialize()
    {
        /** seek lcb */
        SeekToken<token_lcb>();
        if(size_ != 0u)
        {
            /** seek left quote of name*/
            SeekToken<token_quote>();
            if(size_ != 0u)
            {
                std::string name = ReadStringAtLeftQuote();
                SeekToken<token_col>();
                if(size_ != 0)
                {
                    /** seek value begin */
                    SeekSignificantCharacter();
                    if(size_ != 0u)
                    {
                        const char cur_char = *input_;
                        if(cur_char == token_quote) /** a string value */
                        {
                            variables_.insert(std::make_pair(std::move(name),
                                                             ReadStringAtLeftQuote()));
                        }
                        else
                        {
                            variables_.insert(std::make_pair(std::move(name),
                                                             ReadNonStringTypeValue()));
                        }
                    }
                }
                else
                    return false;

                
            }
            else
                return false;
        }
    }

private:
    const char* input_;
    std::size_t size_;
    std::unordered_map<std::string, std::string> variables_;
    
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

    void Advance(const std::size_t offset)
    {
        if(offset < size_)
        {
            input_ += offset;
            size_ -= offset;
        }
        else
            FS_SUN_ASSERT(false);
    }
    
    /** seek significant character */
    void SeekSignificantCharacter()
    {
        char current_char{};
        std::size_t i = 0u;
        for(; i < size_; i++)
        {
            current_char = *input_;
            if(current_char != token_space &&
               current_char != token_tab &&
               current_char != token_lf &&
               current_char != token_cr)
                break;
            
            input_++;
        }

        size_ -= i;
    }

    std::string ReadNonStringTypeValue()
    {
        if(size_ != 0u)
        {
            const char* const begin = input_;
            
            char current_char{};
            std::size_t i = 0u;
            for(; i < size_; i++)
            {
                current_char = *input_;
                if(current_char == token_space ||
                   current_char == token_tab ||
                   current_char == token_lf ||
                   current_char == token_cr ||
                   current_char == token_com||
                   current_char == token_rcb)
                    break;

                input_++;
            }

            size_ -= i;

            if(size_ != 0u)
            {
                return {begin, input_ + 1}; /**TODO */
            }
            else
                return {};
            
        }
        else
            return {};
    }

    std::string ReadStringAtLeftQuote()
    {
        FS_SUN_ASSERT(*input_ == token_quote);
        if(size_ != 0u)
        {
            Advance(1u);
            const char* const begin = input_;
            if(size_ != 0u)
            {
                /** seek right quote */
                SeekToken<token_quote>();
                if(size_ != 0u)
                {
                    return {begin, input_};
                }
                else
                    return {};
            }
        }
        else
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
