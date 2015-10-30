# add the keys for OSX code signing
security create-keychain -p travis osx-build.keychain
security import ../travis/osx/apple.cer -k ~/Library/Keychains/osx-build.keychain -T /usr/bin/codesign
security import ../travis/osx/dist.cer -k ~/Library/Keychains/osx-build.keychain -T /usr/bin/codesign
security import ../travis/osx/dist.p12 -k ~/Library/Keychains/osx-build.keychain -P $KEY_PASSWORD -T /usr/bin/codesign
security default-keychain -s osx-build.keychain
security unlock-keychain -p travis osx-build.keychain
