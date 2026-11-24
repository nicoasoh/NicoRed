 

#pragma once
#include <Headers/kern_patcher.hpp>

class AGDP
{
public:
    static AGDP& singleton();

    void processKext(KernelPatcher& patcher, size_t id, mach_vm_address_t slide, size_t size);
};
