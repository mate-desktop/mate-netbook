#!/usr/bin/bash

# Use grouped output messages
infobegin() {
	echo "::group::${1}"
}
infoend() {
	echo "::endgroup::"
}

# Required packages on Ubuntu
requires=(
	ccache # Use ccache to speed up build
)

# https://git.launchpad.net/ubuntu/+source/mate-netbook/tree/debian/control
requires+=(
	autoconf-archive
	autopoint
	gcc
	git
	libdconf-dev
	libfakekey-dev
	libgtk-3-dev
	libmate-panel-applet-dev
	libwnck-3-dev
	make
	mate-common
	yelp-tools
)

infobegin "Update system"
apt-get update -y
infoend

infobegin "Install dependency packages"
env DEBIAN_FRONTEND=noninteractive \
	apt-get install --assume-yes \
	${requires[@]}
infoend
