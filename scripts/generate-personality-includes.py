#!/usr/bin/env python3
import sys
import re
from pathlib import Path


def main() -> int:
    if len(sys.argv) != 3:
        return 1

    project_root = Path(sys.argv[1])
    output_dir = Path(sys.argv[2])
    output_dir.mkdir(parents=True, exist_ok=True)

    personalities = (
        ("com_apple_kext_AMDRadeonX5000", "Personalities/com.apple.kext.AMDRadeonX5000.xml"),
        ("com_apple_kext_AMDRadeonX5000HWServices", "Personalities/com.apple.kext.AMDRadeonX5000HWServices.xml"),
        ("com_apple_kext_AMDRadeonX6000Framebuffer", "Personalities/com.apple.kext.AMDRadeonX6000Framebuffer.xml"),
        ("com_apple_driver_AppleGFXHDA", "Personalities/com.apple.driver.AppleGFXHDA.xml"),
    )

    for name, relative_path in personalities:
        data = (project_root / "NicoRed" / relative_path).read_bytes()
        lines = [f"static const char {name}[] = {{"]
        for offset in range(0, len(data), 12):
            chunk = data[offset:offset + 12]
            lines.append("    " + ", ".join(f"char(0x{byte:02X})" for byte in chunk) + ",")
        lines.append("    0x00")
        lines.append("};")
        (output_dir / f"{name}.inc").write_text("\n".join(lines) + "\n", encoding="ascii")

    firmware_dir = output_dir.parent / "generated-firmware-inc"
    firmware_dir.mkdir(parents=True, exist_ok=True)

    hwlibs_path = project_root / "NicoRed" / "HWLibs.cpp"
    hwlibs = hwlibs_path.read_text(encoding="utf-8")
    pattern = re.compile(
        r"static const char ([A-Za-z0-9_]+)\[\] = \{\s*#embed \"Firmware/([^\";]+)\"\s*\};"
    )

    def replace_firmware(match: re.Match[str]) -> str:
        name = match.group(1)
        relative_path = match.group(2)
        data = (project_root / "NicoRed" / "Firmware" / relative_path).read_bytes()
        lines = [f"static const char {name}[] = {{"]
        for offset in range(0, len(data), 12):
            chunk = data[offset:offset + 12]
            lines.append("    " + ", ".join(f"char(0x{byte:02X})" for byte in chunk) + ",")
        lines.append("};")
        (firmware_dir / f"{name}.inc").write_text("\n".join(lines) + "\n", encoding="ascii")
        return f'#include "../build/generated-firmware-inc/{name}.inc"'

    hwlibs = pattern.sub(replace_firmware, hwlibs)
    hwlibs_path.write_text(hwlibs, encoding="utf-8")

    for include_match in re.finditer(
        r'#include "../build/generated-firmware-inc/([A-Za-z0-9_]+)\.inc"', hwlibs
    ):
        name = include_match.group(1)
        candidates = [
            name,
            name.lstrip("_"),
            name.removesuffix("_dat") + ".dat",
            name.removesuffix("_bin") + ".bin",
        ]
        firmware_path = None
        for candidate in candidates:
            candidate_path = project_root / "NicoRed" / "Firmware" / candidate
            if candidate_path.is_file():
                firmware_path = candidate_path
                break
        if firmware_path is None:
            stem = name.lstrip("_")
            for candidate_path in (project_root / "NicoRed" / "Firmware").glob("*"):
                if candidate_path.stem == stem:
                    firmware_path = candidate_path
                    break
        if firmware_path is None:
            continue
        data = firmware_path.read_bytes()
        lines = [f"static const char {name}[] = {{"]
        for offset in range(0, len(data), 12):
            chunk = data[offset:offset + 12]
            lines.append("    " + ", ".join(f"char(0x{byte:02X})" for byte in chunk) + ",")
        lines.append("};")
        (firmware_dir / f"{name}.inc").write_text("\n".join(lines) + "\n", encoding="ascii")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
