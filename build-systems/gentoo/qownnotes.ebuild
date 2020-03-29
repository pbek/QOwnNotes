# Copyright 2014-2020 Patrizio Bekerle
# Distributed under the terms of the GNU General Public License v2
# $Id$

#
# QOwnNotes VERSION-STRING
#

EAPI=7

inherit qmake-utils eutils

DESCRIPTION="A plain-text file markdown note taking with Nextcloud/ownCloud integration"
HOMEPAGE="http://www.qownnotes.org/"
SRC_URI="https://download.tuxfamily.org/${PN}/src/${P}.tar.xz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86 ~amd64"
IUSE=""

DEPEND="
	dev-qt/qtwidgets:5
	dev-qt/qtgui:5
	dev-qt/qtcore:5
	dev-qt/qtsql:5
	dev-qt/qtsvg:5
	dev-qt/qtnetwork:5
	dev-qt/qtdeclarative:5
	dev-qt/qtxml:5
	dev-qt/qtxmlpatterns:5
	dev-qt/qtprintsupport:5
	dev-qt/qtwebsockets:5
	dev-qt/qtx11extras:5
"
RDEPEND="${DEPEND}"

src_prepare() {
	echo "#define RELEASE \"Gentoo\"" > release.h
	default
}

src_compile() {
	eqmake5 QOwnNotes.pro -r
}

src_install() {
	emake
	dobin QOwnNotes

	dodir /usr/share/qt5/translations
	insinto /usr/share/qt5/translations

	doins languages/*.qm

	insinto /usr/share/applications
	doicon -s 128 images/icons/128x128/apps/QOwnNotes.png
	doicon -s scalable images/icons/scalable/apps/QOwnNotes.svg
	doins PBE.QOwnNotes.desktop
}
