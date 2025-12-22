#!/bin/bash
# Install AU plugins on macOS
# Removes quarantine, ad-hoc signs, copies to system folder

cd "$(dirname "$0")"
DEST="$HOME/Library/Audio/Plug-Ins/Components"

prep() {
  echo "Preparing: $1"
  sudo xattr -cr "$1"
  sudo xattr -r -d com.apple.quarantine "$1"
  sudo codesign --force --sign - "$1"
  cp -R "$1" "$DEST/"
  echo "  -> Installed to $DEST/"
}

mkdir -p "$DEST"

for plugin in *.component; do
  [ -d "$plugin" ] && prep "$plugin"
done

echo "Done. Restart your DAW to load plugins."
