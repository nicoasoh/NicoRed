includes(path.join(os.getenv("AppleForgePath") or "../AppleForge", "xmake/rules/AppleForge.lua"))

set_project("NicoRed")
set_version("0.9.0")

AppleForge_ApplyDefaults()
set_allowedarchs("x86_64")
set_defaultarchs("x86_64")

target("NicoRed")
	add_rules("LiluPlugIn")
	set_kind("binary")
	set_languages("cxx23")
	add_cxflags("-Wno-c23-extensions")

	add_files("NicoRed/PenguinWizardry/**.cpp")
	add_files("NicoRed/**.cpp")
	add_includedirs("NicoRed")
	set_values("BundleIdentifier", "org.nicoasoh.NicoRed")
target_end()
