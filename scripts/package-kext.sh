#!/bin/bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
VERSION="${1:?usage: package-kext.sh VERSION}"
KEXT="$ROOT/build/NicoRed.kext"
PLIST="$KEXT/Contents/Info.plist"
ZIP="$ROOT/build/artifacts/NicoRed-$VERSION.zip"

[[ "$VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+([.-][A-Za-z0-9.]+)?$ ]] || {
	echo "Invalid NicoRed version: $VERSION" >&2
	exit 1
}
[[ -d "$KEXT" ]] || { echo "Kext bundle not found: $KEXT" >&2; exit 1; }
[[ -f "$PLIST" ]] || { echo "Kext Info.plist not found: $PLIST" >&2; exit 1; }
plutil -lint "$PLIST"
EXECUTABLE_NAME="$(plutil -extract CFBundleExecutable raw -o - "$PLIST")"
BUNDLE_ID="$(plutil -extract CFBundleIdentifier raw -o - "$PLIST")"
PLIST_VERSION="$(plutil -extract CFBundleShortVersionString raw -o - "$PLIST")"
BUNDLE_VERSION="$(plutil -extract CFBundleVersion raw -o - "$PLIST")"
EXECUTABLE="$KEXT/Contents/MacOS/$EXECUTABLE_NAME"
[[ "$BUNDLE_ID" == "org.nicoasoh.NicoRed" ]] || {
	echo "Unexpected or missing Kext bundle identifier: $BUNDLE_ID" >&2
	exit 1
}
[[ -n "$PLIST_VERSION" ]] || { echo "Kext version is missing" >&2; exit 1; }
[[ "$PLIST_VERSION" == "$VERSION" ]] || {
	echo "Kext version $PLIST_VERSION does not match package version $VERSION" >&2
	exit 1
}
[[ "$BUNDLE_VERSION" == "$VERSION" ]] || {
	echo "Kext bundle version $BUNDLE_VERSION does not match package version $VERSION" >&2
	exit 1
}
[[ -s "$EXECUTABLE" && -x "$EXECUTABLE" ]] || {
	echo "Kext executable is missing, empty, or not executable: $EXECUTABLE" >&2
	exit 1
}

mkdir -p "$(dirname "$ZIP")"
rm -f "$ZIP"
ditto -c -k --keepParent "$KEXT" "$ZIP"
unzip -t "$ZIP"
python3 - "$ZIP" "$EXECUTABLE_NAME" <<'PY'
import sys
import zipfile

archive_path, executable_name = sys.argv[1:]
expected_files = {
    "NicoRed.kext/Contents/Info.plist",
    f"NicoRed.kext/Contents/MacOS/{executable_name}",
}
with zipfile.ZipFile(archive_path) as archive:
    names = set(archive.namelist())
    if not expected_files.issubset(names):
        raise SystemExit(f"Archive is missing required Kext files: {expected_files - names}")
    if any(not name.startswith("NicoRed.kext/") for name in names):
        raise SystemExit("Archive contains files outside NicoRed.kext")
PY

echo "Created $ZIP"
