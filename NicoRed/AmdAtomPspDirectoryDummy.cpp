 

#include "AmdAtomPspDirectoryDummy.hpp"

AmdAtomPspDirectoryDummy::AmdAtomPspDirectoryDummy() { }

AmdAtomPspDirectoryDummy::~AmdAtomPspDirectoryDummy() { }

IOReturn AmdAtomPspDirectoryDummy::getFirmwareInfo(AtomPspFirmwareInfo& info) const
{
    memset(&info, 0, sizeof(info));
    return kIOReturnSuccess;
}

AmdAtomPspDirectoryDummy* AmdAtomPspDirectoryDummy::create() { return new AmdAtomPspDirectoryDummy; }
