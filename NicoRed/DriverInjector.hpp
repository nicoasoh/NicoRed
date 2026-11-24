

#pragma once
#include <Headers/kern_patcher.hpp>
#include <libkern/c++/OSUnserialize.h>

class DriverInjector
{
    struct Driver
    {
        const char* identifier{nullptr};
        OSArray*    personalities{nullptr};
        bool        valid{false};

        Driver(const char* identifier, const char* xml, const size_t xml_size) :
            identifier{identifier}
        {
            OSString*   errStr           = nullptr;
            auto* const dataUnserialized = OSUnserializeXML(xml, xml_size, &errStr);

            if (dataUnserialized == nullptr) {
                SYSLOG("DriverInjector", "Failed to unserialize XML for `%s`: `%s`",
                       identifier, errStr == nullptr ? "(null)" : errStr->getCStringNoCopy());
                return;
            }

            this->personalities = OSDynamicCast(OSArray, dataUnserialized);
            if (this->personalities == nullptr) {
                SYSLOG("DriverInjector", "XML for `%s` is not an array", identifier);
                dataUnserialized->release();
                return;
            }
            this->valid = true;
        }

        template<const size_t N>
        Driver(const char* identifier, const char (&xml)[N]) :
            Driver(identifier, xml, N)
        { }

        ~Driver()
        {
            if (this->personalities != nullptr) { this->personalities->release(); }
        }
    };

    mach_vm_address_t orgAddDrivers{0};
    UInt8             matchedDrivers{0};
    Driver            drivers[4];

public:
    DriverInjector();

    static DriverInjector& singleton();

    void processPatcher(KernelPatcher& patcher);

private:
    static bool wrapAddDrivers(void* self, OSArray* array, const bool doNubMatching);
};
