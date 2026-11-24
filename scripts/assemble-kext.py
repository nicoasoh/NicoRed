#!/usr/bin/env python3

import plistlib
import re
import shutil
import sys
from pathlib import Path


def main() -> None:
    if len(sys.argv) != 5:
        raise SystemExit("usage: assemble-kext.py EXECUTABLE SOURCE_PLIST OUTPUT_KEXT VERSION")

    executable = Path(sys.argv[1])
    source_plist = Path(sys.argv[2])
    output_kext = Path(sys.argv[3])
    version = sys.argv[4]
    if not re.fullmatch(r"\d+\.\d+\.\d+(?:[.-][A-Za-z0-9.]+)?", version):
        raise SystemExit(f"invalid NicoRed version: {version}")
    if not executable.is_file() or executable.stat().st_size == 0:
        raise SystemExit(f"built executable is missing or empty: {executable}")

    substitutions = {
        "EXECUTABLE_NAME": "NicoRed",
        "MODULE_VERSION": version,
        "PRODUCT_BUNDLE_IDENTIFIER": "org.nicoasoh.NicoRed",
        "PRODUCT_NAME": "NicoRed",
        "PRODUCT_NAME:rfc1034identifier": "NicoRed",
    }

    def replace(value: object) -> object:
        if isinstance(value, dict):
            return {key: replace(item) for key, item in value.items()}
        if isinstance(value, list):
            return [replace(item) for item in value]
        if isinstance(value, str):
            def substitute(match: re.Match[str]) -> str:
                key = match.group(1)
                if key not in substitutions:
                    raise ValueError(f"unsupported Info.plist build setting: {key}")
                return substitutions[key]

            return re.sub(r"\$\(([^)]+)\)", substitute, value)
        return value

    with source_plist.open("rb") as file:
        info = replace(plistlib.load(file))

    contents = output_kext / "Contents"
    macos = contents / "MacOS"
    if output_kext.exists():
        shutil.rmtree(output_kext)
    macos.mkdir(parents=True, exist_ok=True)
    shutil.copy2(executable, macos / "NicoRed")
    with (contents / "Info.plist").open("wb") as file:
        plistlib.dump(info, file, fmt=plistlib.FMT_XML, sort_keys=False)


if __name__ == "__main__":
    main()
