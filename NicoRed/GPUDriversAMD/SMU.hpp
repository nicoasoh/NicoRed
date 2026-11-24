 

#pragma once
#include <IOKit/IOTypes.h>

enum AMDSMUFWResponse
{
    kSMUFWResponseNoResponse     = 0,
    kSMUFWResponseSuccess        = 1,
    kSMUFWResponseRejectedBusy   = 0xFC,
    kSMUFWResponseRejectedPrereq = 0xFD,
    kSMUFWResponseUnknownCommand = 0xFE,
    kSMUFWResponseFailed         = 0xFF,
};
