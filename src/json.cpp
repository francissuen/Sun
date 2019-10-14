/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#include "json.h"

using namespace fs::sun;

std::string Json::RawValue::Refine() const
{
    /** extract meaningful string:
     * begin with a significant character
     * end with last insignificant character
     */
    std::size_t i = 0u;

    /** begin */
    for(; i < size_; i++)
    {
        const char cur_char = begin_[i];
        if(cur_char != token_space &&
           cur_char != token_tab &&
           cur_char != token_lf &&
           cur_char != token_cr)
            break;
    }
    const char* const begin = begin_ + i;

    /** end */
    for(; i < size_; i++)
    {
        const char cur_char = begin_[i];
        if(cur_char == token_space ||
           cur_char == token_tab ||
           cur_char == token_lf ||
           cur_char == token_cr)
            break;
    }

    return {begin, begin_ + i};
}

bool Json::StringToBoolean(const std::string & str)
{
    if(str.compare("true") == 0)
        return true;
    else if(str.compare("false") == 0)
        return false;
    else
    {
        FS_SUN_LOG("invalid string @str: " + str, Logger::S_WARNING);
        return false;        
    }

}

void Json::UpdateValue(const RawValue & raw_value, bool & ret)
{
    ret = StringToBoolean(raw_value.Refine());
}

void Json::UpdateValue(const RawValue & value, std::string & ret)
{
    ret = value.Refine();
}

Json::Json(const char* const  input, const std::size_t size):
    input_(input),
    size_(size)
{}

Json::Json(const char* const  sz_input):
    Json(sz_input, strlen(sz_input))
{}

bool Json::Initialize()
{
    /** seek lcb */
    SeekToken<token_lcb>();
    while(size_ != 0u)
    {
        /** seek left quote of name*/
        SeekToken<token_quote>();
        if(size_ != 0u)
        {
            std::string name = ReadString();
            SeekToken<token_col>();
            raw_variables_.insert(std::make_pair(std::move(name), ReadValue()));
            if(*input_ == token_rcb)
                return true;
        }
    }
    FS_SUN_LOG("", Logger::S_WARNING);
    return false;
}

const std::unordered_map<std::string, Json::RawValue> & Json::GetVariables() const
{
    return raw_variables_;
}

void Json::Advance(const std::size_t offset)
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
/** void Json::SeekSignificantCharacter() */
/** { */
/**     char current_char{}; */
/**     std::size_t i = 0u; */
/**     for(; i < size_; i++) */
/**     { */
/**         current_char = input_[i]; */
/**         if(current_char != token_space && */
/**            current_char != token_tab && */
/**            current_char != token_lf && */
/**            current_char != token_cr) */
/**             break; */
/**     } */

/**     size_ -= i; */
/**     input_ += i; */
/** } */

/** std::string Json::ReadNonStringTypeValue() */
/** { */
/**     if(size_ != 0u) */
/**     { */
/**         const char* const begin = input_; */
            
/**         char current_char{}; */
/**         std::size_t i = 0u; */
/**         for(; i < size_; i++) */
/**         { */
/**             current_char = input_[i]; */
/**             if(current_char == token_space || */
/**                current_char == token_tab || */
/**                current_char == token_lf || */
/**                current_char == token_cr || */
/**                current_char == token_com|| */
/**                current_char == token_rcb) */
/**                 break; */
/**         } */

/**         size_ -= i; */
/**         input_ += i; */

/**         if(size_ != 0u) */
/**         { */
/**             return {begin, input_}; */
/**         } */
/**         else */
/**             return {}; */
            
/**     } */
/**     else */
/**         return {}; */
/** } */


std::string Json::ReadString()
{
    FS_SUN_ASSERT(*input_ == token_quote); /** should be at quote now */
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
        else
            return {};
    }
    else
        return {};
}

/** std::string Json::ReadArrayFromLeftSquareBrace() */
/** { */
/**     FS_SUN_ASSERT(*input_ == token_lsb); */
/**     if(size_ != 0u) */
/**     { */
/**         Advance(1u); */
/**         const char* const begin = input_; */
/**         if(size_ != 0u) */
/**         { */
/**             /\** seek right quote *\/ */
/**             SeekToken<token_rsb>(); */
/**             if(size_ != 0u) */
/**             { */
/**                 return {begin, input_}; */
/**             } */
/**             else */
/**                 return {}; */
/**         } */
/**         else */
/**             return {}; */
/**     } */
/**     else */
/**         return {}; */
/** } */

Json::RawValue Json::ReadValue()
{
    /** should be at token_col now */
    FS_SUN_ASSERT(*input_ == token_col);
    RawValue ret;
    if(size_ != 0)
    {
        Advance(1u);
        
        std::size_t i = 0u;
        for(; i < size_; i++)
        {
            const char cur_char = input_[i];
            if(cur_char != token_space &&
               cur_char != token_tab &&
               cur_char != token_lf &&
               cur_char != token_cr)
                break;
        }
        
        size_ -= i;
        input_ += i;
        
        ret.begin_ = input_;
        char cur_char = *input_;
        if(cur_char == token_lsb) /** array */
        {
            std::size_t level = 1u;
            bool in_middle_of_string = false;
            for(i = 0u; i < size_; i++)
            {
                cur_char = input_[i];
                if(cur_char == token_quote && *(input_ - 1) != token_quote)
                    in_middle_of_string = !in_middle_of_string;
                if(!in_middle_of_string)
                {
                    if(cur_char == token_lsb)
                        level += 1u;
                    else if(cur_char == token_rsb)
                        level -= 1u;

                    if(level == 0u)
                        break;
                }
            }
        }
        else if(cur_char == token_lcb) /** object */
        size_ -= i;
        input_ += i;
        
        ret.size_ = i;
    }

    return ret;
}
