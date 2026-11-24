 

#include <Headers/kern_api.hpp>
#include <Headers/kern_util.hpp>
#include <Headers/plugin_start.hpp>
#include <NicoRed.hpp>

static const char* bootargOff[]   = {"-NicoRedOff", "-NRedOff"};
static const char* bootargDebug[] = {"-NicoRedDebug", "-NRedDebug"};
static const char* bootargBeta[]  = {"-NicoRedBeta", "-NRedBeta"};

PluginConfiguration ADDPR(config){
    "NicoRed",
    parseModuleVersion(xStringify(MODULE_VERSION)),
    LiluAPI::AllowNormal | LiluAPI::AllowInstallerRecovery | LiluAPI::AllowSafeMode,
    bootargOff,
    2,
    bootargDebug,
    2,
    bootargBeta,
    2,
    KernelVersion::Catalina,
    KernelVersion::Tahoe,
    []() { NicoRed::singleton().init(); },
};
