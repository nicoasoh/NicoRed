 

#pragma once
#include <GPUDriversAMD/FB/BiosParser.hpp>

class AmdAtomPspDirectoryDummy : public AmdAtomPspDirectory
{
    AmdAtomPspDirectoryDummy();

public:
    virtual ~AmdAtomPspDirectoryDummy();
    virtual IOReturn getFirmwareInfo(AtomPspFirmwareInfo& info) const override;

    static AmdAtomPspDirectoryDummy* create();
};
