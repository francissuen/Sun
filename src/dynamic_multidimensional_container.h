/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "variant.h"

FS_SUN_NS_BEGIN

template<typename T, template <typename ...> class TContainer>
class DynamicMultidimentionalContainer
{
public:
    using Element = Variant<T, DynamicMultidimentionalContainer*>;
    using Container = TContainer<Element>;

private:
    Element elements_;
};

template<typename T>
class DynamicMultidimentionalVector: public DynamicMultidimentionalContainer<T, std::vector>
{
public:
    using Element = typename DynamicMultidimentionalContainer<T, std::vector>::Element;
public:
    const Element & operator[](const std::size_t index)
    {
        this->elements_[0];
    }
};

class test
{
protected:
    using myType = int;  
};

class d: public test
{
    myType a;
};

FS_SUN_NS_END
