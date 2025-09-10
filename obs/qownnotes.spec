#
# Spec file for package qownnotes for openSUSE Linux, Fedora Linux and CentOS 7
#
# Check for Linux distribution version numbers here:
# https://en.opensuse.org/openSUSE:Build_Service_cross_distribution_howto
#

%global lrelease_qt5 /usr/bin/lrelease-qt5
Name:           qownnotes
BuildRequires:  gcc-c++ fdupes
BuildRequires:  pkgconfig(Qt5Svg)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Concurrent)
BuildRequires:  pkgconfig(Qt5WebSockets)
BuildRequires:  pkgconfig(Qt5X11Extras)

# This is for all Fedora CentOS 7
%if 0%{?fedora} || 0%{?rhel} >= 7 || 0%{?centos} >= 7

BuildRequires:  desktop-file-utils
BuildRequires:  aspell-devel
BuildRequires:  qt5-linguist
Requires:       qt5-qtsvg qt5-qtwebsockets qt5-qtx11extras

%else
# This is for all SUSE

BuildRequires:  libqt5-qtbase-devel libqt5-qtdeclarative-devel libQt5Svg-devel libQt5X11Extras-devel libqt5-qtwebsockets-devel libqt5-linguist

# no aspell for SLE
%if 0%{?sle_version} == 150000 || 0%{?sle_version} == 150100 && !0%{?is_backports}
#BuildRequires:  aspell-devel
%else
BuildRequires:  libaspell15
%endif

BuildRequires:  update-desktop-files
Requires:       libQt5Svg5 libQt5Sql5 libQt5Sql5-sqlite libQt5Gui5 libQt5Network5 libQt5Widgets5 libQt5Xml5 libQt5PrintSupport5 libQt5WebSockets5 libQt5X11Extras5

# try to detect openSUSE Tumbleweed
# the way described in https://en.opensuse.org/Archive:How_to_detect_Tumbleweed didn't seem to work
#%if 0%{suse_version} > 1320
Requires:   libQtQuick5
#%else
#Requires:   libQt5Declarative5
#%endif

%endif
%if 0%{?suse_version}
Recommends:     %{name}-lang
%endif

License:        GPL-2.0
Group:          System/GUI/Productivity
Summary:        Note-taking app and todo list manager with ownCloud/Nextcloud integration
Url:            https://www.qownnotes.org/
Version:        VERSION-STRING
Release:        1%{?dist}
Source0:        %{name}-%{version}.tar.xz


%description
Plain-text file markdown note taking with Nextcloud/ownCloud integration.

You are able to write down your thoughts with QOwnNotes and edit or search for them later from your mobile device (like with CloudNotes or the ownCloud/Nextcloud web-service.

The notes are stored as plain text files and are synced with ownCloud's/Nextcloud's file sync functionality. Of course other software, like Dropbox can be used too.

I like the concept of having notes accessible in plain text files, like it is done in the Nextcloud notes app, to gain a maximum of freedom, but I was not able to find a decent desktop note taking tool or a text editor, that handles them well. Out of this need QOwnNotes was born.

www.qownnotes.org

Author
======
Patrizio Bekerle <patrizio@bekerle.com>

%if 0%{?suse_version}
%lang_package
%endif

# Fedora 27 and above wasn't able to create debug packages
%if 0%{?fedora} >= 27
%global debug_package %{nil}

# prevent RPM build error: Installed (but unpackaged) file(s) found
%define _unpackaged_files_terminate_build 0
%endif

# Fixes: /usr/bin/ld: cannot open linker script file /home/abuild/rpmbuild/BUILD/.package_note-qownnotes-22.7.1-1.1.x86_64.ld: No such file or directory
%if 0%{?fedora} >= 36
%global _package_note_file  %{_builddir}/%{name}-%{version}/.package_note-%{name}-%{version}-%{release}.%{_arch}.ld
%endif

%prep
%setup -q

mkdir build
pushd build
%if 0%{?fedora} || 0%{?rhel} >= 7 || 0%{?centos} >= 7
%qmake_qt5 ..
%else
%qmake5 ..
%endif
popd

%build
echo centos_version 0%{?centos_version}
echo rhel_version   0%{?rhel_version}
echo fedora 0%{?fedora}
echo suse_version   0%{?suse_version}

%{lrelease_qt5} QOwnNotes.pro
pushd build
CFLAGS=$RPM_OPT_FLAGS CCFLAGS=$CFLAGS

%if 0%{?fedora} || 0%{?rhel} >= 7 || 0%{?centos} >= 7

make

%else

%make_jobs

%endif

popd

%install
pushd build
export INSTALL_ROOT=%{buildroot}
%make_install
popd

%if 0%{?suse_version}
# see: https://de.opensuse.org/openSUSE:Paketbauvereinbarungen_zu_RPM-Makros
%suse_update_desktop_file -c  PBE.QOwnNotes QOwnNotes QOwnNotes QOwnNotes QOwnNotes "Utility;SyncUtility;"
%endif
%find_lang QOwnNotes --with-qt

%fdupes $RPM_BUILD_ROOT/%{_prefix}

%clean  
rm -rf $RPM_BUILD_ROOT  

%files
%defattr(-,root,root)
%doc LICENSE README.md CHANGELOG.md shortcuts.md
%{_bindir}/QOwnNotes
%{_datadir}/icons/hicolor/
%{_datadir}/icons/hicolor/scalable/apps/QOwnNotes.svg
%{_datadir}/icons/hicolor/*/apps/*.png
%{_datadir}/applications/PBE.QOwnNotes.desktop
%if 0%{?fedora} || 0%{?rhel} >= 7 || 0%{?centos} >= 7
%{_datadir}/qt5/
%else
%files lang -f QOwnNotes.lang
%{_datadir}/qt5/
%endif

%changelog
