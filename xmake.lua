set_project("NicoRed")
local version = os.getenv("NICORED_VERSION")
if not version or version == "" then
	local output = os.iorunv("cat", {"VERSION"})
	version = output:gsub("%s+", "")
end
if not version:match("^%d+%.%d+%.%d+[%w%.%-]*$") then
	os.raise("NICORED_VERSION must be a semantic version")
end
set_version(version)

set_allowedarchs("x86_64")
set_defaultarchs("x86_64")

local root = os.projectdir()
local lilu = path.join(root, ".ci-deps", "Lilu")
local kernel_sdk = path.join(root, ".ci-deps", "MacKernelSDK")
if not os.isdir(path.join(lilu, "Lilu", "Headers")) then
	os.raise("Missing Lilu headers at .ci-deps/Lilu")
end
if not os.isfile(path.join(kernel_sdk, "Headers", "mach", "mach_types.h")) then
	os.raise("Missing MacKernelSDK at .ci-deps/MacKernelSDK")
end

local sdkroot = os.getenv("SDKROOT")
if not sdkroot or sdkroot == "" then
	local output, _, code = os.iorunv("xcrun", {"--sdk", "macosx", "--show-sdk-path"})
	if code ~= 0 then
		os.raise("Unable to locate the macOS SDK with xcrun")
	end
	sdkroot = output:gsub("%s+$", "")
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
		"-Wno-stdlibcxx-not-found")
	add_cxflags("-fapple-kext", "-mkernel", "-fno-builtin", "-fno-exceptions", "-fno-rtti",
		"-mmacosx-version-min=10.15", "-nostdlib", "-Wno-stdlibcxx-not-found", "-Wno-c23-extensions")

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
