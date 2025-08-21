# Copyright 2014-2021 Patrizio Bekerle
# Copyright 2025 Patrick Nagel
# Distributed under the terms of the GNU General Public License v2
# $Id$

EAPI=8

inherit qmake-utils desktop xdg-utils

DESCRIPTION="A plain-text file markdown note taking with Nextcloud/ownCloud integration"
HOMEPAGE="https://www.qownnotes.org/"
SRC_URI="https://github.com/pbek/QOwnNotes/releases/download/v${PV}/${P}.tar.xz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86"

DEPEND="
>=dev-qt/qtbase-6.9:6[widgets,gui,concurrent,sql,network,xml]
>=dev-qt/qtwebsockets-6.9:6
>=dev-qt/qtdeclarative-6.9:6
>=dev-qt/qtsvg-6.9:6
"
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

	insinto /usr/share/qt6/translations
	doins languages/*.qm

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

	domenu PBE.QOwnNotes.desktop
}

pkg_postinst() {
	xdg_icon_cache_update
}

pkg_postrm() {
	xdg_icon_cache_update
}
