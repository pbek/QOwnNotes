.PHONY: translations-build translations-download translations-upload translations-update-files nix-build nix-build-force
TRANSFER_DIR := $(shell if [ -d "$(HOME)/NextcloudPrivate/Transfer" ]; then echo "$(HOME)/NextcloudPrivate/Transfer"; else echo "$(HOME)/Nextcloud/Transfer"; fi)

translations-build:
	lrelease src/QOwnNotes.pro

translations-download:
	./scripts/download_translations.sh

translations-upload:
	crowdin upload

translations-update-files:
	./scripts/update-translations.sh

nix-build:
	nix-build -E '((import <nixpkgs> {}).qt6Packages.callPackage (import ./default.nix) { })'

nix-build-qt5:
	nix build '.?submodules=1#qownnotes-qt5'

nix-build-qt5153:
	nix build '.?submodules=1#qownnotes-qt5153'

nix-build-qt5-force:
	nix build '.?submodules=1#qownnotes-qt5' --rebuild

nix-build-qt5153-force:
	nix build '.?submodules=1#qownnotes-qt5153' --rebuild

nix-build-trace:
	nix-build -E '((import <nixpkgs> {}).qt6Packages.callPackage (import ./default.nix) { })' --show-trace

nix-build-force:
	nix-build -E '((import <nixpkgs> {}).qt6Packages.callPackage (import ./default.nix) { })' --check

nix-run:
	./result/bin/QOwnNotes --session test &

src-build:
	mkdir -p build-QOwnNotes; cd build-QOwnNotes && qmake "CONFIG+=debug USE_SYSTEM_BOTAN=1" ../src/QOwnNotes.pro && make

src-clean:
	rm -rf build-QOwnNotes

src-run:
	./build-QOwnNotes/QOwnNotes

src-build-run: src-build src-run

clang-format:
	./scripts/clang-format-project.sh

fix-linting:
	make clang-format

link-check:
	lychee './**/*.md'

git-apply-qownnotes-patch:
	git apply ~/Nextcloud/Transfer/qownnotes.patch

git-apply-qmarkdowntextedit-patch:
	cd ./src/libraries/qmarkdowntextedit && git apply ~/Nextcloud/Transfer/qmarkdowntextedit.patch

git-create-patch:
	@echo "TRANSFER_DIR: ${TRANSFER_DIR}"; \
	git diff --no-ext-diff --staged --binary > ${TRANSFER_DIR}/qownnotes.patch; \
	cd src/libraries/qmarkdowntextedit && git diff --no-ext-diff --staged --binary > ${TRANSFER_DIR}/qmarkdowntextedit.patch; \
	cd ../piwiktracker && git diff --no-ext-diff --staged  --binary > ${TRANSFER_DIR}/piwiktracker.patch; \
	cd ../qttoolbareditor && git diff --no-ext-diff --staged  --binary > ${TRANSFER_DIR}/qttoolbareditor.patch; \
	ls -l1t ${TRANSFER_DIR} | head -5

open-crowdin-webpage:
	xdg-open https://crowdin.com/project/qownnotes/activity-stream
