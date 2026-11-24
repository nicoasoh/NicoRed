 

#pragma once
#include <AMDGFX9DCNDisplay.hpp>

class AMDRadeonX5000_AMDGFX9DCN1Display : public AMDRadeonX5000_AMDGFX9DCNDisplay
{
    static VFT vft;

    static void Constructor(AMDRadeonX5000_AMDGFX9DCN1Display* self, const OSMetaClass* metaClass);

    static void initDCNRegOffs(AMDRadeonX5000_AMDGFX9DCN1Display* self);

public:
    PWDeclareRuntimeMC(AMDRadeonX5000_AMDGFX9DCN1Display, Constructor)

    static void resolve(const char* kext);
};
