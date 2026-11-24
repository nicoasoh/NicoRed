set_project("NicoRed")
local root = os.projectdir()
local version = os.getenv("NICORED_VERSION")
if not version or version == "" then
	version = "0.9.0"
end
if not version:match("^%d+%.%d+%.%d+[%w%.%-]*$") then
	version = "0.9.0"
end
set_version(version)

set_allowedarchs("x86_64")
set_defaultarchs("x86_64")

local lilu = path.join(root, ".ci-deps", "Lilu")
local kernel_sdk = path.join(root, ".ci-deps", "MacKernelSDK")
if not os.isdir(path.join(lilu, "Lilu", "Headers")) then
end
if not os.isfile(path.join(kernel_sdk, "Headers", "mach", "mach_types.h")) then
end

local sdkroot = os.getenv("SDKROOT")
if not sdkroot or sdkroot == "" then
	local detected_sdk = "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk"
	if not os.isdir(detected_sdk) then
		detected_sdk = sdkroot
	end
	sdkroot = detected_sdk
end
if not os.isdir(sdkroot) then
	os.raise("SDKROOT does not point to an installed macOS SDK")
end

target("NicoRed")
	set_kind("binary")
	set_targetdir(path.join(root, "build", "bin"))
	set_languages("cxx23")
	add_defines("KERNEL", "PRODUCT_NAME=NicoRedPlugin", "MODULE_VERSION=" .. version)
	add_cflags("-fapple-kext", "-mkernel", "-mmacosx-version-min=10.15", "-fno-builtin", "-nostdlib",
		"-Wno-stdlibcxx-not-found", "-Wno-c2x-extensions")
	add_cxflags("-fapple-kext", "-mkernel", "-fno-builtin", "-fno-exceptions", "-fno-rtti",
		"-mmacosx-version-min=10.15", "-nostdlib", "-Wno-stdlibcxx-not-found")

	add_files("NicoRed/PenguinWizardry/**.cpp")
	add_files("NicoRed/**.cpp")
	add_files("NicoRed/KModInfo.c")
	add_files(path.join(lilu, "Lilu", "Library", "plugin_start.cpp"))
	add_files(path.join(kernel_sdk, "Library", "kmod", "c_start.c"))
	add_files(path.join(kernel_sdk, "Library", "kmod", "c_stop.c"))
	add_includedirs("NicoRed", path.join(lilu, "Lilu"), path.join(lilu, "Lilu", "PrivateHeaders"),
		path.join(kernel_sdk, "Headers"))
	add_linkdirs(path.join(kernel_sdk, "Library", "x86_64"),
		path.join(sdkroot, "System", "Library", "Frameworks", "Kernel.framework", "Versions", "A", "Libraries"))
	add_links("kmod", "kmodc++", "cc_kext")
	add_ldflags("-Wl,-kext", {force = true})
	add_ldflags("-Wl,-undefined,dynamic_lookup", "-mmacosx-version-min=10.15", "-static", "-nostdlib")

	before_build(function (build_target)
		os.execv("python3", {
			path.join(root, "scripts", "generate-personality-includes.py"),
			root,
			path.join(root, "build", "generated-personality-inc")
		})
	end)
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
