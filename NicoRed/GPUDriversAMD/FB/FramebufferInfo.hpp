

#pragma once
#include <IOKit/IOTypes.h>

struct AMDFBRect
{
    UInt32 width;
    UInt32 height;
};

struct FramebufferInfo
{
    UInt64    crtOffset;
    UInt32    size;
    UInt32    crtSize;
    UInt32    pitch;
    AMDFBRect rect;
    bool      isMirrored;
    bool      isEnabled;
    bool      isMapped;
    bool      isOnline;
    UInt64    savedSize;
    UInt64    pageCount;
};
