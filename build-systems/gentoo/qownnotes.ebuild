# Copyright 2014-2018 Patrizio Bekerle
# Distributed under the terms of the GNU General Public License v2
# $Id$

#
# QOwnNotes VERSION-STRING
#

EAPI=5

inherit qmake-utils eutils

DESCRIPTION="A plain-text file notepad with markdown support and ownCloud/NC integration"
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
"
RDEPEND="${DEPEND}"

src_prepare() {
	echo "#define RELEASE \"Gentoo\"" > release.h
}

src_compile() {
	eqmake5 QOwnNotes.pro -r
}

src_install() {
	emake
	dobin QOwnNotes

	dodir /usr/share/QOwnNotes/languages
	insinto /usr/share/QOwnNotes/languages

	doins languages/*.qm

	insinto /usr/share/applications
	doicon -s 128 images/icons/128x128/apps/QOwnNotes.png
	doicon -s scalable images/icons/scalable/apps/QOwnNotes.svg
	doins PBE.QOwnNotes.desktop
}
