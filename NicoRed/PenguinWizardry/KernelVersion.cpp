 

#include <PenguinWizardry/KernelVersion.hpp>
#include <PenguinWizardry/New.hpp>
#include <libkern/OSTypes.h>

alignas(PenguinWizardry::KernelVersion) static UInt8 _currentKernelVersion[sizeof(PenguinWizardry::KernelVersion)];

[[gnu::constructor(101)]]
static void _GLOBAL_initCurrentKernelVersion()
{
    new (_currentKernelVersion)
        PenguinWizardry::KernelVersion{static_cast<UInt32>(version_major), static_cast<UInt32>(version_minor)};
}

const PenguinWizardry::KernelVersion& currentKernelVersion()
{ return *launder(reinterpret_cast<PenguinWizardry::KernelVersion*>(_currentKernelVersion)); }
