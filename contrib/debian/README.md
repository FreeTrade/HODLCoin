
Debian
====================
This directory contains files used to package hodlcoind/hodlcoin-qt
for Debian-based Linux systems. If you compile hodlcoind/hodlcoin-qt yourself, there are some useful files here.

## hodlcoin: URI support ##


hodlcoin-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install hodlcoin-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your hodlcoin-qt binary to `/usr/bin`
and the `../../share/pixmaps/hodlcoin128.png` to `/usr/share/pixmaps`

hodlcoin-qt.protocol (KDE)

