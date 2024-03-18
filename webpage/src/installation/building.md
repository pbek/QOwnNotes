## Building from GitHub

You can also build QOwnNotes yourself from the source code.

Checkout the code directly from the git repository:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

Then download* [Qt Creator](https://www.qt.io/download-open-source), you will also need the packages `qtwebsockets`;
With Qt6, you need also `qt5compat`.
Under Windows, stick to *MinGw 64-bit*.

* Note: the `offline` installer didn't work for me: QT Creator 12.x offline installer for Windows **does NOT** install websockets nor mingw64; 
I had to 
 1. register a QT account (mandatory for package selection?)
 2. during installation wizard, choose `Custom Installation`.   Can be done later via `Tools` -> `qt maintenantce` -> `start maintenance...`
 3. choose a QT release, and there select [X] MingW and [X] Websockets, [X] Qt5 Compatilibility module ... as shown in the screenshot:
![image](https://github.com/pbek/QOwnNotes/assets/30468664/f49e727c-9b7c-464b-bb99-ccc3433a3a4b)
`qt 5 compatibility module` is only required If you build with Qt6.

run QT Creator -> Edit menu -> Preferences -> Kits:
In the `Kits` tab:
 - Select the MingW kit (and click "Apply")
 - it might be useful to delete other kits that you don't need.
![image](https://github.com/pbek/QOwnNotes/assets/30468664/39f4b4ee-6de5-4dec-aa51-a4a4c9f2e83f)

in the `Qt Versions` tab:
 - select a Qt version, and then `apply`.
![image](https://github.com/pbek/QOwnNotes/assets/30468664/ecada213-aa3f-46be-9bcb-440c9a262071)

Afterward open the project file `src/QOwnNotes.pro`.

Select `debug` or `release` on the bottom left of QC Creator's screen (where it says: "releases"), here:
![image](https://github.com/pbek/QOwnNotes/assets/30468664/0fc21c34-ea9e-454b-bbab-2ec418caaa40)

Compile:
`Build` -> `Build Project QOwnNotes`

Or you can build it directly in your terminal:

```bash
cd src

# build binary translation files if you want another language than English
lrelease QOwnNotes.pro

# prepare build process and build the application
qmake
make
```

The executrable file will be found under a subfolder similar to "build-QOwnNotes-Desktop_Qt_6_6_2_MinGW_64_bit-Release",
Under Windows, the required DLLs can be found at `<qt-installation>\6.*.*\mingw_64\bin\*.dll`. Other variation will not work.

That's it (took me 3 days! to figure out)


Please feel free to contribute source code to this project, make suggestions or
report troubles on the [QOwnNotes issues page](https://github.com/pbek/QOwnNotes/issues).

Visit [QOwnNotes on GitHub](https://github.com/pbek/QOwnNotes).

::: tip
If you want to contribute source code to the project please make your pull requests
to the  `main` branch.
:::

## Source archive

You will find source archives of QOwnNotes on [QOwnNotes source archive](https://github.com/pbek/QOwnNotes/releases).

The source archives will be accessible like that:

`https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz`
