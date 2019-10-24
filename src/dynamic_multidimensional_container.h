/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "variant.h"

FS_SUN_NS_BEGIN

/**
 * \brief Dynamic multidimensional and has flexibility for any T in Ts.
 */
template<template <typename ...> class TContainer, typename T, typename TDerived>
class DynamicMultidimensionalContainer
{
    
protected:
    
    using Element = Variant<T, std::unique_ptr<TDerived>>;
    using Container = TContainer<Element>;

public:

    friend std::string to_string(const DynamicMultidimensionalContainer & value)
    {
        return string::ToString(value.container_);
    }

protected:
    
    Container container_;
};

template<typename T>
class DynamicMultidimensionalVector: public DynamicMultidimensionalContainer<std::vector, T,
                                                                             DynamicMultidimensionalVector<T>>
{
public:
    using Element = typename DynamicMultidimensionalContainer<std::vector, T, DynamicMultidimensionalVector<T>>::
    Element;
    using Container = typename DynamicMultidimensionalContainer<std::vector, T, DynamicMultidimensionalVector<T>>::
    Container;
    
public:
    const Element & operator[](const std::size_t index) const
    {
        return this->container_[index];
    }

    std::size_t Size() const
    {
        return this->container_.size();
    }

    template <typename TElement>
    void PushBack(TElement && element)
    {
        this->container_.push_back(std::forward<TElement>(element));
    }
};

FS_SUN_NS_END
