 

#pragma once
#include <AMDGFX9DCNDisplay.hpp>

class AMDRadeonX5000_AMDGFX9DCN2Display : public AMDRadeonX5000_AMDGFX9DCNDisplay
{
    static VFT vft;

    static void Constructor(AMDRadeonX5000_AMDGFX9DCN2Display* self, const OSMetaClass* metaClass);

    static void initDCNRegOffs(AMDRadeonX5000_AMDGFX9DCN2Display* self);

public:
    PWDeclareRuntimeMC(AMDRadeonX5000_AMDGFX9DCN2Display, Constructor)

    static void resolve(const char* kext);
};
