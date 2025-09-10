DEBTRANSFORM-RELEASE: 1
Format: 3.0 (quilt)
Source: qownnotes
Binary: qownnotes, qownnotes-l10n
Architecture: any all
Version: VERSION-STRING-1
Maintainer: Patrizio Bekerle <patrizio@bekerle.com>
Homepage: https://www.qownnotes.org
Standards-Version: 4.6.2
Package-List:
 qownnotes deb editors optional arch=any
 qownnotes-l10n deb localization optional arch=all
Build-Depends:
 cmake (>= 3.16),
 debhelper-compat (= 13),
 libxkbcommon-dev,
 pkg-config,
 qt6-base-dev,
 qt6-declarative-dev,
 qt6-l10n-tools,
 qt6-svg-dev,
 qt6-tools-dev,
 qt6-tools-dev-tools,
 qt6-websockets-dev
