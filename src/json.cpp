/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#include "json.h"

using namespace fs::sun;

template<>
Json::VectorValue::Element Json::VectorAdaptor<Json::VectorValue::Element>(Json::VectorValue && value)
{
    return std::unique_ptr<VectorValue>(new VectorValue(std::move(value)));
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
                std::string name = ReadString().Get<std::string>();
                SeekToken<token_col>();
                AdvanceCurrentToken<token_col>();
                if(size_ != 0u)
                {
                    values_.insert(std::make_pair(std::move(name), ReadValue<Value>()));

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

Json::Status Json::GetStatus() const
{
    return status_;
}

Json::ScalarValue Json::ReadString()
{
    /** should be at quote now */
    AdvanceCurrentToken<token_quote>();

    std::string ret;
    
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
                ret = {begin, input_};
                AdvanceCurrentToken<token_quote>();
            }
        }
    }

    return ret;
}

Json::ScalarValue Json::ReadObject()
{
    Json* ret = new Json(input_, size_);
    if(ret->Initialize())
    {
        input_ = ret->input_;
        size_ = ret->size_;

        return std::unique_ptr<Json>(ret);
    }
    else
        return std::unique_ptr<Json>(nullptr);
}

Json::ScalarValue Json::ReadOthers()
{
    const char * begin = input_;
    do
    {
        const char cur_char = *input_;
        if(cur_char == token_com ||
           cur_char == token_rsb ||
           cur_char == token_rcb ||
           cur_char == token_space ||
           cur_char == token_tab ||
           cur_char == token_lf ||
           cur_char == token_cr)
            break;

        input_ += 1u;
        size_ -= 1u;
    }
    while(size_ != 0);
    
    return std::string{begin, input_};
}

Json::VectorValue Json::ReadArray()
{
    VectorValue ret;
    AdvanceCurrentToken<token_lsb>();

    if(size_ != 0u)
    {
        do
        {
            ret.PushBack(ReadValue<VectorValue::Element>());

            AdvanceUntilSignificant();
            if(size_ != 0u)
            {
                if(*input_ == token_rsb)
                {
                    AdvanceCurrentToken<token_rsb>();
                    break;                    
                }

                else
                    AdvanceCurrentToken<token_com>();
            }
        }
        while(size_ != 0u);
    }

    return ret;
}
