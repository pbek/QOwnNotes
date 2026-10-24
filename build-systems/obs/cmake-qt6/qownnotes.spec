#
# Spec file for the Qt6 CMake build of QOwnNotes
#

Name:           qownnotes
Version:        VERSION-STRING
Release:        1%{?dist}
Summary:        Note-taking app and todo list manager with Nextcloud integration
License:        GPL-2.0-only
URL:            https://www.qownnotes.org/
Source0:        %{name}-%{version}.tar.xz

BuildRequires:  cmake >= 3.16
BuildRequires:  gcc-c++
BuildRequires:  pkgconfig(libgit2)
BuildRequires:  pkgconfig(libsecret-1)

%if 0%{?fedora} || 0%{?rhel}
BuildRequires:  desktop-file-utils
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtdeclarative-devel
BuildRequires:  qt6-qtsvg-devel
BuildRequires:  qt6-qttools-devel
BuildRequires:  qt6-qtwebsockets-devel
%else
BuildRequires:  fdupes
BuildRequires:  update-desktop-files
BuildRequires:  cmake(Qt6Concurrent)
BuildRequires:  cmake(Qt6Core)
BuildRequires:  cmake(Qt6DBus)
BuildRequires:  cmake(Qt6Gui)
BuildRequires:  cmake(Qt6LinguistTools)
BuildRequires:  cmake(Qt6Network)
BuildRequires:  cmake(Qt6PrintSupport)
BuildRequires:  cmake(Qt6Qml)
BuildRequires:  cmake(Qt6Quick)
BuildRequires:  cmake(Qt6Sql)
BuildRequires:  cmake(Qt6Svg)
BuildRequires:  cmake(Qt6WebSockets)
BuildRequires:  cmake(Qt6Widgets)
BuildRequires:  cmake(Qt6Xml)
%endif

%description
QOwnNotes is a plain-text Markdown note-taking application with
Nextcloud Notes integration. Notes remain ordinary files and can be
synchronized with Nextcloud, ownCloud, or other synchronization tools.

%prep
%autosetup

%build
%cmake \
  -DQON_QT6_BUILD=ON \
  -DBUILD_WITH_LIBGIT2=ON \
  -DCMAKE_BUILD_TYPE=Release
%cmake_build

%install
%cmake_install

%if 0%{?suse_version}
%suse_update_desktop_file -c PBE.QOwnNotes QOwnNotes QOwnNotes QOwnNotes QOwnNotes "Utility;SyncUtility;"
%fdupes %{buildroot}%{_prefix}
%else
desktop-file-validate %{buildroot}%{_datadir}/applications/PBE.QOwnNotes.desktop
%endif

%files
%license LICENSE
%doc README.md CHANGELOG.md shortcuts.md
%{_bindir}/QOwnNotes
%{_datadir}/QOwnNotes/
%{_datadir}/applications/PBE.QOwnNotes.desktop
%{_datadir}/icons/hicolor/

%changelog
