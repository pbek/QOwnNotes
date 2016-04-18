SingleApplication
=================

This is a replacement of the QSingleApplication for `Qt5`.

Keeps the Primary Instance of your Application and kills each subsequent instances.

Usage
-----
The `SingleApplication` class inherits from whatever `Q[Core|Gui]Application` class you specify in the `*.pro` file. Further usage is similar to use one of the `Q[Core|Gui]Application` classes.

The library uses your `Organization Name` and `Application Name` to set up a `QLocalServer` and a `QSharedMemory` block. The first instance of your Application would check if the shared memory block exists and if not it will start a `QLocalServer` and then listen for connections on it. Each subsequent instance of your application would check if the shared memory block exists and if it does, it will connect to the QLocalServer to notify it that a new instance had been started, after which would terminate the new instance with status code `0`. The Primary Instance, `SingleApplication` would emmit the `showUp()` signal upon detecting that a new instance had been started.

The library uses `stdlib` to terminate the program with the `exit()` function.

Here is an example usage of the library:
```cpp
// project.pro
DEFINES += QAPPLICATION_CLASS=QApplication # or whatever app class you want

// main.cpp
#include "singleapplication.h"

int main(int argc, char *argv[])
{
    QApplication::setApplicationName("{Your App Name}");
    QApplication::setOrganizationName("{Your Organization Name}");

    SingleApplication app(argc, argv);

    return app.exec();
}
```

The `Show Up` signal
------------------------
The SingleApplication class implements a `showUp()` signal. You can bind to that signal to raise your application's window when a new instance had been started.

Note that since `SingleApplication` extends the `QApplication` class  you can do the following:
```cpp
// Truly raise your window.
QObject::connect(&app, &SingleApplication::showUp, [&]
{
    window.show();
    window.raise();
    window.activateWindow();
}); // 'window' is your QWindow instance
```

Using `QApplication::instance()` is a neat way to get the `SingleApplication` instance at any place in your program.


Implementation
--------------
The library is implemented with a QSharedMemory block which is thread safe and guarantees a race condition will not occur. It also uses a QLocalSocket to notify the main process that a new instance had been spawned and thus invoke the `showUp()` signal.

To handle an issue on `*nix` systems, where the operating system owns the shared memory block and if the program crashes the memory remains untouched, the library binds to the following signals and closes the program with error code = `128 + signum` where signum is the number representation of the signal listed below. Handling the signal is required in order to safely delete the `QSharedMemory` block.

* `SIGINT ` - `2`
* `SIGILL ` - `4`
* `SIGABRT` - `6`
* `SIGFPE ` - `8`
* `SIGSEGV` - `11`
* `SIGTERM` - `15`


License
-------
This library and it's supporting documentation are released under `The MIT License (MIT)`.
