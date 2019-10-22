/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#include "json.h"

using namespace fs::sun;

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

void Json::UpdateValue(const Value & value, bool & ret, const std::size_t index)
{
    FS_SUN_ASSERT(value.size() > index);
    ret = StringToBoolean(value[index].Get<std::string>());
}

void Json::UpdateValue(const Value & value, std::string & ret, const std::size_t index)
{
    FS_SUN_ASSERT(value.size() > index);
    ret = value[index].Get<std::string>();
}

Json::Json(const char* const  input, const std::size_t size):
    input_(input),
    size_(size),
    status_(Status::UNINITIALIZED)
{}

Json::Json(const char* const  sz_input):
    Json(sz_input, strlen(sz_input))
{}

bool Json::Initialize()
{
    if(status_ == Status::UNINITIALIZED)
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
                AdvanceCurrentToken<token_col>();
                if(size_ != 0u)
                {
                    values_.insert(std::make_pair(std::move(name), ReadValue()));

                    AdvanceUntilSignificant();
                
                    if(*input_ == token_rcb)
                    {
                        AdvanceCurrentToken<token_rcb>();
                        status_ = Status::OK;
                        break;
                    }
                }
            }
        }

        status_ = Status::BAD;
    }
    
    return status_ == Status::OK;
}

void Json::AdvanceUntilSignificant()
{
    while(size_ != 0u)
    {
        const char cur_char = *input_;
        if(cur_char != token_space &&
           cur_char != token_tab &&
           cur_char != token_lf &&
           cur_char != token_cr)
            break;

        input_ += 1u;
        size_ -= 1u;
    }
}

const std::unordered_map<std::string, Json::Value> & Json::GetValues() const
{
    return values_;
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
                AdvanceCurrentToken<token_quote>();
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
    {
        input_ = ret->input_;
        size_ = ret->size_;

        return std::unique_ptr<Json>(ret);
    }
    else
        return nullptr;
}

Json::Value Json::ReadArray()
{
    Value ret;
    AdvanceCurrentToken<token_lsb>();

    if(size_ != 0u)
    {
        do
        {
            ret.push_back(ReadValue());

            AdvanceUntilSignificant();
            if(size_ != 0u)
            {
                if(*input_ == token_rsb)
                    break;
                else
                    AdvanceCurrentToken<token_com>();
            }
        }
        while(size_ != 0u);
    }

    return ret;
}

std::string Json::ReadOthers()
{
    const char * begin = input_;
    do
    {
        const char cur_char = *input_;
        if(cur_char == token_com ||
           cur_char == token_space ||
           cur_char == token_tab ||
           cur_char == token_lf ||
           cur_char == token_cr)
            break;

        input_ += 1u;
        size_ -= 1u;
    }
    while(size_ != 0);
    
    return {begin, input_};
}

Json::Value Json::ReadValue()
{
    AdvanceUntilSignificant();
    
    const char cur_char = *input_;
    Value ret;
    if(cur_char == token_quote) /** string */
    {
        ret = ReadString();
    }
    else if(cur_char == token_lcb) /** object */
    {
        ret = ReadObject();
    }
    else if(cur_char == token_lsb) /** array */
    {
        ret = ReadArray();
    }
    else                        /** number, ture, false, null */
    {
        ret = ReadOthers();
    }

    return ret;
}
