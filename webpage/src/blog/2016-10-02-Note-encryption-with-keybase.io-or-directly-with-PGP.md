---
title: Note encryption with keybase.io or directly with PGP
image: /img/blog/keybase.io-logo_reference.png
date: 2016-10-02T10:39:15+00:00
order: 20161002103915
---

# Note encryption with keybase.io or directly with PGP

<v-subheader class="blog">Date: 2016-10-02</v-subheader>

You now can use your **custom method** to **encrypt and decrypt notes** via the scripting engine (for [Issue #334](https://github.com/pbek/QOwnNotes/issues/334)).

- you can use the new scripting hook encryptionHook() do implement your own encryption
- there are examples to encrypt notes with **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/doc/scripting/encryption-keybase.qml)), **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/doc/scripting/encryption-pgp.qml)) or do a trivial "encryption" with ROT13 ([encryption-rot13.qml](https://github.com/pbek/QOwnNotes/blob/main/doc/scripting/encryption-rot13.qml))
- you can disable the password dialog with script.encryptionDisablePassword()
- to start an external synchronous process you can usescript.startSynchronousProcess()
- to check on which platform the script is running you can use script.platformIsLinux(), script.platformIsOSX() and script.platformIsWindows()
- for more information on the new commands please take a look at the [scripting documentation](http://docs.qownnotes.org/en/develop/scripting/README.html)

![keybase.io logo](/img/blog/keybase.io-logo_reference.png "keybase.io logo")

(image by keybase.io)
