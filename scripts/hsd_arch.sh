#!/bin/sh

if [ "$HSD_ARCH" = "" ]; then
	HSD_ARCH=$(uname -m)

	# We need to convert from linux uname -m values to Onyx architectures
	case $HSD_ARCH in
		"aarch64")
			HSD_ARCH="arm64"
		;;
	esac
fi

echo $HSD_ARCH
