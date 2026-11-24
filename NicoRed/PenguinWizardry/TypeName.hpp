 

#pragma once

template<typename T>
const char* type_name();

#define DEFINE_TYPE_NAME(_ty)    \
    template<>                   \
    const char* type_name<_ty>() \
    { return #_ty; }
