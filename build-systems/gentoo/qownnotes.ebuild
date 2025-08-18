# Copyright 2014-2025 Patrizio Bekerle, Patrick Nagel
# Distributed under the terms of the GNU General Public License v2
# $Id$

#
# QOwnNotes VERSION-STRING
#

EAPI=8

inherit qmake-utils desktop xdg-utils

DESCRIPTION="A plain-text file markdown note taking with Nextcloud/ownCloud integration"
HOMEPAGE="https://www.qownnotes.org/"
SRC_URI="https://github.com/pbek/QOwnNotes/releases/download/v${PV}/${P}.tar.xz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86 ~amd64"
IUSE=""

DEPEND=">=dev-qt/qtbase-6.7.2:6[gui]"
RDEPEND="${DEPEND}"

src_prepare() {
	echo "#define RELEASE \"Gentoo\"" > release.h
	default
}

src_compile() {
	eqmake6 QOwnNotes.pro -r
}

src_install() {
	emake
	dobin QOwnNotes

	dodir /usr/share/qt6/translations
	insinto /usr/share/qt6/translations

	doins languages/*.qm

	insinto /usr/share/applications
	doicon -s 128 "images/icons/128x128/apps/QOwnNotes.png"
	doicon -s 16 "images/icons/16x16/apps/QOwnNotes.png"
	doicon -s 24 "images/icons/24x24/apps/QOwnNotes.png"
	doicon -s 256 "images/icons/256x256/apps/QOwnNotes.png"
	doicon -s 32 "images/icons/32x32/apps/QOwnNotes.png"
	doicon -s 48 "images/icons/48x48/apps/QOwnNotes.png"
	doicon -s 512 "images/icons/512x512/apps/QOwnNotes.png"
	doicon -s 64 "images/icons/64x64/apps/QOwnNotes.png"
	doicon -s 96 "images/icons/96x96/apps/QOwnNotes.png"
	doicon -s scalable "images/icons/scalable/apps/QOwnNotes.svg"
	doins PBE.QOwnNotes.desktop
}

pkg_postinst() {
	xdg_icon_cache_update
}

pkg_postrm() {
	xdg_icon_cache_update
}
