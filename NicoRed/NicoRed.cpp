
#include <AGDP.hpp>
#include <AppleGFXHDA.hpp>
#include <Backlight.hpp>
#include <DebugEnabler.hpp>
#include <DriverInjector.hpp>
#include <GPUDriversAMD/ATOMBIOS.hpp>
#include <GPUDriversAMD/CAIL/Result.hpp>
#include <GPUDriversAMD/RavenIPOffset.hpp>
#include <GPUDriversAMD/SMU.hpp>
#include <GPUDriversAMD/TTL/SWIP/SMU.hpp>
#include <HWLibs.hpp>
#include <Headers/kern_api.hpp>
#include <Headers/kern_devinfo.hpp>
#include <Headers/kern_iokit.hpp>
#include <Headers/kern_patcher.hpp>
#include <Headers/kern_util.hpp>
#include <IOKit/IOLib.h>
#include <IOKit/IOTypes.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <Kexts.hpp>
#include <NicoRed.hpp>
#include <PenguinWizardry/RuntimeMC.hpp>
#include <Regs/GC.hpp>
#include <Regs/NBIO.hpp>
#include <Regs/SMU.hpp>
#include <X5000.hpp>
#include <X6000FB.hpp>
#include <kern/clock.h>
#include <libkern/OSTypes.h>
#include <libkern/c++/OSMetaClass.h>
#include <mach/i386/vm_types.h>

static NicoRed moduleInstance;

NicoRed& NicoRed::singleton() { return moduleInstance; }

void NicoRed::init()
{
    SYSLOG("NicoRed", "NicoRed: startup - registering AMD GPU hooks");
    SYSLOG("NicoRed", "|-----------------------------------------------------------------|")
    SYSLOG("NicoRed", "| Copyright 2022-2026 ChefKiss.                                   |");
    SYSLOG("NicoRed", "| If you've paid for this, you've been scammed. Ask for a refund! |");
    SYSLOG("NicoRed", "| Change the world for the better.                                |");
    SYSLOG("NicoRed", "|-----------------------------------------------------------------|");

    lilu.onKextLoadForce(&kextRadeonX6000Framebuffer);
    lilu.onKextLoadForce(&kextRadeonX5000HWLibs);
    lilu.onKextLoadForce(&kextRadeonX5000);
    lilu.onKextLoadForce(&kextAGDP);
    lilu.onKextLoadForce(&kextAppleGFXHDA);

    lilu.onPatcherLoadForce(
        [](void* const, KernelPatcher& patcher)
        {
            singleton().processPatcher();
            if (singleton().hardwareSupported) {
                Backlight::singleton().init();
                DriverInjector::singleton().processPatcher(patcher);
                PenguinWizardry::RuntimeMCManager::singleton().processPatcher(patcher);
            }
        },
        nullptr);

    lilu.onKextLoadForce(
        nullptr, 0,
        [](void* const, KernelPatcher& patcher, const size_t id, const mach_vm_address_t slide, const size_t size)
        {
            if (!singleton().hardwareSupported) { return; }
            AGDP::singleton().processKext(patcher, id, slide, size);
            Backlight::singleton().processKext(patcher, id, slide, size);
            DebugEnabler::singleton().processKext(patcher, id, slide, size);
            X6000FB::singleton().processKext(patcher, id, slide, size);
            AppleGFXHDA::singleton().processKext(patcher, id, slide, size);
            X5000HWLibs::singleton().processKext(patcher, id, slide, size);
            X5000::singleton().processKext(patcher, id, slide, size);
        },
        nullptr);
}

void NicoRed::hwLateInit()
{
    if (!this->hardwareSupported) { return; }
    if (this->rmmioAvailable) { return; }
    if (this->iGPU == nullptr) {
        SYSLOG("NicoRed", "Cannot initialize RMMIO without an iGPU");
        return;
    }

    SYSLOG("NicoRed", "Hardware init: enabling PCI memory and bus mastering");
    this->iGPU->setMemoryEnable(true);
    this->iGPU->setBusMasterEnable(true);

    this->rmmio =
        this->iGPU->mapDeviceMemoryWithRegister(kIOPCIConfigBaseAddress5, kIOMapInhibitCache | kIOMapAnywhere);
    if (this->rmmio == nullptr || this->rmmio->getLength() == 0
        || this->rmmio->getVirtualAddress() == 0)
    {
        SYSLOG("NicoRed", "Failed to map RMMIO BAR5");
        OSSafeReleaseNULL(this->rmmio);
        this->rmmioPtr = nullptr;
        this->hardwareSupported = false;
        return;
    }
    this->rmmioPtr = reinterpret_cast<volatile UInt32*>(this->rmmio->getVirtualAddress());
    if (this->rmmio->getLength() < sizeof(UInt32)) {
        SYSLOG("NicoRed", "RMMIO BAR5 is too small");
        OSSafeReleaseNULL(this->rmmio);
        this->rmmioPtr = nullptr;
        this->hardwareSupported = false;
        return;
    }
    this->rmmioAvailable = true;
    SYSLOG("NicoRed", "Hardware init: RMMIO BAR5 mapped (length=0x%llX)", this->rmmio->getLength());

    this->fbOffset    = static_cast<UInt64>(this->readReg32(GC_BASE_0 + MC_VM_FB_OFFSET) & 0xFFFFFF) << 24;
    this->devRevision = (this->readReg32(NBIO_BASE_2 + RCC_DEV0_EPF0_STRAP0) & RCC_DEV0_EPF0_STRAP0_ATI_REV_ID_MASK)
                        >> RCC_DEV0_EPF0_STRAP0_ATI_REV_ID_SHIFT;

    if (this->attributes.isRenoir()) {
        if (!this->attributes.isGreenSardine() && this->devRevision == 0 && this->pciRevision >= 0x80
            && this->pciRevision <= 0x84)
        {
            this->attributes.setRenoirE();
        }
    }
    else {
        if (this->devRevision >= 0x8) {
            this->attributes.setRaven2();
            this->enumRevision = 0x79;
        }
        else if (this->attributes.isPicasso()) {
            this->enumRevision = 0x41;
        }
        else if (this->devRevision == 1) {
            this->enumRevision = 0x20;
        }
        else {
            this->enumRevision = 0x1;
        }
    }

    SYSLOG("NicoRed", "Init complete: deviceID=0x%X pciRevision=0x%X devRevision=0x%X enumRevision=0x%X fbOffset=0x%llX Picasso=%s",
           this->deviceID, this->pciRevision, this->devRevision, this->enumRevision, this->fbOffset,
           this->attributes.isPicasso() ? "yes" : "no");
    DBGLOG("NicoRed", "deviceID = 0x%X", this->deviceID);
    DBGLOG("NicoRed", "pciRevision = 0x%X", this->pciRevision);
    DBGLOG("NicoRed", "fbOffset = 0x%llX", this->fbOffset);
    DBGLOG("NicoRed", "devRevision = 0x%X", this->devRevision);
    DBGLOG("NicoRed", "isPicasso = %s", this->attributes.isPicasso() ? "true" : "false");
    DBGLOG("NicoRed", "isRaven2 = %s", this->attributes.isRaven2() ? "true" : "false");
    DBGLOG("NicoRed", "isRenoir = %s", this->attributes.isRenoir() ? "true" : "false");
    DBGLOG("NicoRed", "isGreenSardine = %s", this->attributes.isGreenSardine() ? "true" : "false");
    DBGLOG("NicoRed", "enumRevision = 0x%X", this->enumRevision);
}

void NicoRed::processPatcher()
{
    this->hardwareSupported = false;
    this->rmmioAvailable = false;

    const auto devInfo = DeviceInfo::create();
    if (devInfo == nullptr) {
        SYSLOG("NicoRed", "Failed to create Lilu DeviceInfo");
        return;
    }

    devInfo->processSwitchOff();

    if (devInfo->videoBuiltin == nullptr) {
        DBGLOG("NicoRed", "No iGPU detected by Lilu");
        DeviceInfo::deleter(devInfo);
        return;
    }
    this->iGPU = OSDynamicCast(IOPCIDevice, devInfo->videoBuiltin);
    if (this->iGPU == nullptr) {
        SYSLOG("NicoRed", "Lilu iGPU is not an IOPCIDevice");
        DeviceInfo::deleter(devInfo);
        return;
    }
    if (WIOKit::readPCIConfigValue(this->iGPU, WIOKit::kIOPCIConfigVendorID) != WIOKit::VendorID::ATIAMD) {
        DBGLOG("NicoRed", "iGPU is not AMD; NicoRed will stay inactive");
        DeviceInfo::deleter(devInfo);
        return;
    }

    WIOKit::renameDevice(this->iGPU, "IGPU");
    WIOKit::awaitPublishing(this->iGPU);
    UInt8 builtInBytes[] = {0x00};
    this->iGPU->setProperty("built-in", builtInBytes, sizeof(builtInBytes));
    char slotNameBytes[] = "built-in";
    this->iGPU->setProperty("AAPL,slot-name", slotNameBytes, sizeof(slotNameBytes));
    char hdaGfxBytes[] = "onboard-1";
    this->iGPU->setProperty("hda-gfx", hdaGfxBytes, sizeof(hdaGfxBytes));

    this->deviceID = static_cast<UInt16>(WIOKit::readPCIConfigValue(this->iGPU, WIOKit::kIOPCIConfigDeviceID));
    switch (this->deviceID) {
        case 0x15D8: {
            this->attributes.setPicasso();
        } break;
        case 0x15DD: {
            // Picasso/Raven Ridge mobile Vega device ID.
            this->attributes.setPicasso();
            this->enumRevision = 0x41;
            SYSLOG("NicoRed", "Detected Picasso/Raven Ridge iGPU (PCI ID 0x15DD); enumRevision=0x41");
        } break;
        case 0x164C:
        case 0x1636: {
            this->attributes.setRenoir();
            this->enumRevision = 0x91;
        } break;
        case 0x15E7:
        case 0x1638: {
            this->attributes.setRenoir();
            this->attributes.setGreenSardine();
            this->enumRevision = 0xA1;
        } break;
        default:
            DBGLOG("NicoRed", "Unsupported AMD iGPU device ID 0x%X; NicoRed will stay inactive", this->deviceID);
            DeviceInfo::deleter(devInfo);
            return;
    }
    this->pciRevision = static_cast<UInt8>(WIOKit::readPCIConfigValue(this->iGPU, WIOKit::kIOPCIConfigRevisionID));
    this->hardwareSupported = true;
    SYSLOG("NicoRed", "AMD iGPU accepted: deviceID=0x%X pciRevision=0x%X", this->deviceID, this->pciRevision);

    char name[128];
    for (size_t i = 0, ii = 0; i < devInfo->videoExternal.size(); i++) {
        auto device = OSDynamicCast(IOPCIDevice, devInfo->videoExternal[i].video);
        if (device == nullptr) { continue; }

        snprintf(name, arrsize(name), "GFX%zu", ii++);
        WIOKit::renameDevice(device, name);
        WIOKit::awaitPublishing(device);
    }

    DeviceInfo::deleter(devInfo);
}

void NicoRed::setProp32(const char* const key, const UInt32 value) const { this->iGPU->setProperty(key, value, 32); }

UInt32 NicoRed::readReg32(const UInt32 reg) const
{
    if (!this->rmmioAvailable || this->rmmio == nullptr || this->rmmioPtr == nullptr) {
        SYSLOG("NicoRed", "readReg32 called before RMMIO initialization (reg 0x%X)", reg);
        return 0;
    }

    const UInt64 byteOffset64 = static_cast<UInt64>(reg) * sizeof(UInt32);
    if (byteOffset64 <= UINT32_MAX && byteOffset64 + sizeof(UInt32) <= this->rmmio->getLength()) {
        return this->rmmioPtr[reg];
    }
    else {
        const UInt64 pcieWindowEnd =
            (static_cast<UInt64>(PCIE_DATA2) + 1) * sizeof(UInt32);
        if (this->rmmio->getLength() < pcieWindowEnd) {
            SYSLOG("NicoRed", "RMMIO too small for register extension (reg 0x%X)", reg);
            return 0;
        }
        this->rmmioPtr[PCIE_INDEX2] = reg;
        return this->rmmioPtr[PCIE_DATA2];
    }
}
