set_project("NicoRed")
local version = os.getenv("NICORED_VERSION")
if not version or version == "" then
	version = io.readfile("VERSION"):gsub("%s+", "")
end
assert(version:match("^%d+%.%d+%.%d+[%w%.%-]*$"), "NICORED_VERSION must be a semantic version")
set_version(version)

set_allowedarchs("x86_64")
set_defaultarchs("x86_64")

local root = os.projectdir()
local lilu = path.join(root, ".ci-deps", "Lilu")
local kernel_sdk = path.join(root, ".ci-deps", "MacKernelSDK")
assert(os.isdir(path.join(lilu, "Lilu", "Headers")), "Missing Lilu headers at .ci-deps/Lilu")
assert(os.isfile(path.join(kernel_sdk, "Headers", "mach", "mach_types.h")), "Missing MacKernelSDK at .ci-deps/MacKernelSDK")

local sdkroot = os.getenv("SDKROOT")
if not sdkroot or sdkroot == "" then
	local output, _, code = os.iorunv("xcrun", {"--sdk", "macosx", "--show-sdk-path"})
	assert(code == 0, "Unable to locate the macOS SDK with xcrun")
	sdkroot = output:gsub("%s+$", "")
end
assert(os.isdir(sdkroot), "SDKROOT does not point to an installed macOS SDK")

local kmodinfo = path.join(root, "build", "KModInfo.c")
os.mkdir(path.directory(kmodinfo))
io.writefile(kmodinfo, string.format([[
#include <libkern/libkern.h>
#include <mach/mach_types.h>
#include <mach/kmod.h>

__private_extern__ kern_return_t NicoRed_kern_start(kmod_info_t *ki, void *data);
__private_extern__ kern_return_t NicoRed_kern_stop(kmod_info_t *ki, void *data);
__private_extern__ kern_return_t _start(kmod_info_t *ki, void *data);
__private_extern__ kern_return_t _stop(kmod_info_t *ki, void *data);

__attribute__((visibility("default")))
KMOD_EXPLICIT_DECL(org.nicoasoh.NicoRed, "%s", _start, _stop)

__private_extern__ kmod_start_func_t *_realmain = NicoRed_kern_start;
__private_extern__ kmod_stop_func_t *_antimain = NicoRed_kern_stop;
__private_extern__ int _kext_apple_cc = __APPLE_CC__;
]], version))

target("NicoRed")
	set_kind("binary")
	set_targetdir(path.join(root, "build", "bin"))
	set_languages("cxx23")
	add_defines("PRODUCT_NAME=NicoRed", "MODULE_VERSION=" .. version)
	add_cflags("-fapple-kext", "-mkernel", "-mmacosx-version-min=10.15", "-fno-builtin", "-nostdlib",
		"-Wno-stdlibcxx-not-found")
	add_cxflags("-fapple-kext", "-mkernel", "-fno-builtin", "-fno-exceptions", "-fno-rtti",
		"-mmacosx-version-min=10.15", "-nostdlib", "-Wno-stdlibcxx-not-found", "-Wno-c23-extensions")

	add_files("NicoRed/PenguinWizardry/**.cpp")
	add_files("NicoRed/**.cpp")
	add_files(path.join(lilu, "Lilu", "Library", "plugin_start.cpp"))
	add_files(path.join(kernel_sdk, "Library", "kmod", "c_start.c"))
	add_files(path.join(kernel_sdk, "Library", "kmod", "c_stop.c"))
	add_files(kmodinfo)
	add_includedirs("NicoRed", path.join(lilu, "Lilu"), path.join(lilu, "Lilu", "PrivateHeaders"),
		path.join(kernel_sdk, "Headers"))
	add_linkdirs(path.join(kernel_sdk, "Library", "x86_64"),
		path.join(sdkroot, "System", "Library", "Frameworks", "Kernel.framework", "Versions", "A", "Libraries"))
	add_links("kmod", "kmodc++", "cc_kext")
	add_ldflags("-Wl,-kext", "-Wl,-undefined,dynamic_lookup", "-mmacosx-version-min=10.15", "-static", "-nostdlib")

	after_build(function (build_target)
		os.execv("python3", {
			path.join(root, "scripts", "assemble-kext.py"),
			build_target:targetfile(),
			path.join(root, "NicoRed", "Info.plist"),
			path.join(root, "build", "NicoRed.kext"),
			version
		})
	end)
target_end()
