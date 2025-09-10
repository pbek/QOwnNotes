# Why metrics?

## Why are metrics useful?

QOwnNotes (as in the application itself) is **developed by mostly one person in his spare time**.

Releases are built for a lot of platforms and distributions (often very old versions of them).
Keeping them up is a lot of work.

The metrics are the only dependable source of information about which versions of platforms and
distributions QOwnNotes runs on. They also show which features are the most popular, so that decisions can be made
concerning which versions to retire, which features need more work, and which are hardly used.

The same applies to the user's selection of preferred languages, which promotes the improvement of the corresponding translations.

## Where are the metrics stored?

The metrics are transmitted to and stored on a self-hosted [Matomo](https://matomo.org/) server.
Only the two main developers have access to the data. **No data is shared with 3rd parties.**

## How are metrics collected?

Metrics are **anonymized** (e.g. ip address stripped out) on the Matomo server after they are received.

No personal information, like text you entered or note names are collected or stored on the server!

## What is collected?

Some basic information like screen resolution, operating system and version of QOwnNotes used is transmitted.

Furthermore, for example events are sent to the server when actions are used in the applications.

QOwnNotes is free open source software.
You can review the metrics all the time by searching for example for
[sendVisitIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendVisitIfEnabled)
and [sendEventIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendEventIfEnabled)
in the source code to see what exactly is sent.

**No personal information, like text you entered or note names are collected!**

## Can I turn metrics off?

You can turn them off upon first start of the application in the welcome dialog
or at a later time in the settings dialog.

## Why are metrics not off by default?

Defaults are king. Turning metrics off by default would result in no meaningful metrics at all.

Most of the people don't care about metrics. Those who care about metrics and
don't want to vote with their metrics about what is worked on or what is
supported can easily turn them off at the first start of the application.

You will even get a second dialog that will tell you about the metrics, if you
finished the welcome dialog too early.
