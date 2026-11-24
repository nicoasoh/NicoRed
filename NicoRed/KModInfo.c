#include <libkern/libkern.h>
#include <mach/mach_types.h>
#include <mach/kmod.h>

#define NICORED_STRINGIFY_VALUE(value) #value
#define NICORED_STRINGIFY(value) NICORED_STRINGIFY_VALUE(value)

__private_extern__ kern_return_t NicoRed_kern_start(kmod_info_t *ki, void *data);
__private_extern__ kern_return_t NicoRed_kern_stop(kmod_info_t *ki, void *data);
__private_extern__ kern_return_t _start(kmod_info_t *ki, void *data);
__private_extern__ kern_return_t _stop(kmod_info_t *ki, void *data);

__attribute__((visibility("default")))
KMOD_EXPLICIT_DECL(org.nicoasoh.NicoRed, NICORED_STRINGIFY(MODULE_VERSION), _start, _stop)

__private_extern__ kmod_start_func_t *_realmain = NicoRed_kern_start;
__private_extern__ kmod_stop_func_t *_antimain = NicoRed_kern_stop;
__private_extern__ int _kext_apple_cc = __APPLE_CC__;
