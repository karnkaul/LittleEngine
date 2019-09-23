#!/bin/bash

# This script packs each .app bundle in `Runtime` into a .tar.gz archive
# Requirements:
#  - tar
#  [symlinks] (*nix)

cd "$(dirname "${BASH_SOURCE[0]}")/.."
. ./Utils/os.sh

if [ "$IS_SYMLINKS" != "TRUE" ]; then
	echo "Error: OS does not support symlinks!"
	exit 1
fi

BUNDLES=$(ls | grep .app)
for BUNDLE in $BUNDLES; do
	if [[ ! "$BUNDLE" == *".tar.gz" ]]; then
		echo "== Archiving $BUNDLE > $BUNDLE.tar.gz"
		tar -czf $BUNDLE.tar.gz $BUNDLE
	fi
done

exit
