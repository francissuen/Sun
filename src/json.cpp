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
    AdvanceCurrentToken<token_lcb>();
    
    while(size_ != 0u)
    {
        /** seek left quote of name*/
        SeekToken<token_quote>();
        if(size_ != 0u)
        {
            std::string name = ReadString();
            SeekToken<token_col>();
            values_.insert(std::make_pair(std::move(name), ReadValue()));
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


std::string Json::ReadString()
{
    /** should be at quote now */
    AdvanceCurrentToken<token_quote>();
    
    if(size_ != 0u)
    {

        const char* const begin = input_;
        if(size_ != 0u)
        {
            do
            {
                /** seek right quote */
                SeekToken<token_quote>();
                /** check the prior character if it's \ */
                if(*(input_ - 1u) != token_backslash)
                    break;
            }
            while(size_ != 0u);
            
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

std::unique_ptr<Json> Json::ReadObject()
{
    Json* ret = new Json(input_, size_);
    if(ret->Initialize())
        return std::unique_ptr<Json>(ret);
    else
        return nullptr;
}

Json::Value Json::ReadValue()
{
    /** should be at token_col now */
    AdvanceCurrentToken<token_col>();
    if(size_ != 0u)
    {
        /** pass the insignificants */
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
        
        const char cur_char = *input_;
        Value ret;
        if(cur_char == token_quote) /** string */
        {
            ret.push_back(ReadString());
        }
        else if(cur_char == token_lcb) /** object */
        {
            ret.push_back(ReadObject());
        }
        else if(cur_char == token_lsb) /** array */
        {
            std::size_t level = 1u;
            bool is_among_string = false;
            for(i = 0u; i < size_; i++)
            {
                const char inner_char = input_[i];
                if(inner_char == token_quote && *(input_ - 1) != token_backslash)
                    is_among_string = !is_among_string;
                if(!is_among_string)
                {
                    if(inner_char == token_lsb)
                        level += 1u;
                    else if(inner_char == token_rsb)
                        level -= 1u;

                    if(level == 0u)
                        break;
                }
            }
        }
        size_ -= i;
        input_ += i;
        
        ret.size_ = i;
    }

    return ret;
}
