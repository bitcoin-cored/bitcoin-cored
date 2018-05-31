
Debian
====================
This directory contains files used to package cored/core-qt
for Debian-based Linux systems. If you compile cored/core-qt yourself, there are some useful files here.

## bitcoincore: URI support ##


core-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install core-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your core-qt binary to `/usr/bin`
and the `../../share/pixmaps/bitcoin128.png` to `/usr/share/pixmaps`

core-qt.protocol (KDE)

