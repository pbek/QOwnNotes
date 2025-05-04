# OBS Build for Debian 12+ with cmake and Qt6

[OBS package](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop-cmake3)

CDBS doesn't support qmake after Debian 12 anymore, see [#3267](https://github.com/pbek/QOwnNotes/issues/3267).

To build the QOwnNotes package with cmake under Debian, some changes need to be made,
see [001-translations-and-icons.patch](./001-translations-and-icons.patch).

The [OBS package](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop-cmake3)
applies the patch and builds the package with cmake and Qt6.
