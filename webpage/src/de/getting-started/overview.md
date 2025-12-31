# Übersicht

<ScreenshotCarousel />

<v-divider />

**Q** für das verwendete [Qt-Framework](https://www.qt.io/)   
**Own**, weil Sie Ihre eigenen Notizen besitzen  
**Notes** für die Notizen, die Sie erstellen

<v-divider />

[QOwnNotes](https://www.qownnotes.org/) ist die **Open Source** (GPL) Klartextdatei-**Markdown-Notizanwendung** für **GNU/Linux**, **MacOS** und **Windows**, die (optional) mit der [**Notizenanwendung**](https://github.com/nextcloud/notes) von [Nextcloud](https://nextcloud.com/) und [ownCloud](https://owncloud.org/) zusammenarbeitet.

::: tip
Join our [Telegram Group](https://t.me/QOwnNotes) to discuss, learn, and connect with the QOwnNotes community.
:::

[Verstehen Sie das Konzept von QOwnNotes](concept.md)

## Features

- Sie können Ihre **bereits vorhandenen Text- oder Markdowndateien** verwenden; kein Import nötig
- QOwnNotes ist in C++ geschrieben und für **geringen Ressourcenverbrauch** optimiert (keine CPU- und speicherhungrige Electron-App)
- **Evernote- und Joplin-Import**
- **Kompatibel** mit der [Notes](https://apps.nextcloud.com/apps/notes)-App von **Nextcloud / ownCloud** und **Mobile** und Nextcloud Text
- Erstellen Sie **[Nextcloud Deck](https://apps.nextcloud.com/apps/deck) Karten** und verlinken Sie sie in Ihren Notizen
- Unterstützt das **Teilen von Notizen** auf Ihrem **ownCloud** / **Nextcloud** Server
- Verwalten Sie Ihre Nextcloud / ownCloud **To-do-Listen** (**Nextcloud tasks** oder **Tasks Plus** / **Calendar Plus**) oder verwenden Sie einen anderen **CalDAV**, um Ihre Aufgaben darauf zu synchronisieren
- Ältere **Versionen** Ihrer Notizen können von Ihrem **Nextcloud / ownCloud**-Server **wiederhergestellt** werden
- **Gelöschte Notizen** können von Ihrem **Nextcloud / ownCloud**-Server **wiederhergestellt** werden
- **Externe Änderungen** von Notizdateien werden **überwacht** (Notizen oder Notizliste werden neu geladen)
- Unterstützung für **hierarchische Notizschlagwörter** und **Notiz-Unterordner**
- Optionale **Verschlüsselung** von Notizen, AES-256 ist eingebaut, oder Sie können personalisierte Verschlüsselungsmethoden wie **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml)) oder **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml)) benutzen
- Der **Notizenordner** kann **frei gewählt** werden (mehrere Notizenordner können verwendet werden)
- **Das Suchen von Wortstücken** in Notizen ist möglich und Suchergebnisse werden in den Notizen hervorgehoben
- [**Browsererweiterung**](browser-extension.md) um **Notizen aus ausgewähltem Text hinzuzufügen**, **Screenshots zu erstellen** oder **um Ihre Lesezeichen zu verwalten**
- [**Rechtschreibprüfung**](../editor/spellchecking.md) wird unterstützt
- **Portabler Modus** um QOwnNotes auf USB-Sticks mitzunehmen
- **Skriptunterstützung** und ein online [**Skriptverzeichnis**](https://github.com/qownnotes/scripts), wo Sie Skripte in der App installieren können
- Implementation of the **[OpenAI completion API](https://www.qownnotes.org/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html)** to be used in scripts
- **Vim-Modus**
- **[Web-App](web-app.md)** um **Fotos von Ihrem mobilen Telefon** in Notizen auf QOwnNote auf Ihrem Desktop einzufügen

## Aussehen

- **Markdown-Hervorhebung** von Notizen und ein **Markdown-Vorschaumodus**
- **Unterstützung für Schemata im Dunkelmodus**
- **Schemata-Unterstützung** für die Hervorhebung von **Markdown-Syntax**
- Alle **Schaltflächen können platziert werden, wo immer Sie möchten**, sie können sogar **schweben** oder **gestapelt werden** (vollständig dockbar)
- Unterstützung für **Freedesktop-Schemasymbole**. Sie können QOwnNotes mit Ihren **originalen Desktop-Symbolen** und mit Ihrem bevorzugten **dunklen Desktop-Design** verwenden
- **Ablenkungsfreier Modus**, **Vollbildmodus** und **Schreibmaschinenmodus**
- **Zeilennummern**
- **Differences** between current note and externally changed note are **shown** in a dialog

## Sprachen

- **Verfügbar in mehr als 60 verschiedenen Sprachen** wie Englisch, Deutsch, Französisch, Polnisch, Chinesisch, Japanisch, Russisch, Portugiesisch, Ungarisch, Niederländisch und Spanisch
  - [Ihre Hilfe](../contributing/translation.md) wird sehr geschätzt, um diese Übersetzungen zu verbessern oder QOwnNotes in mehr Sprachen zu übersetzen

<style>
.v-divider {
  margin: 20px;
}

.sheet {
  position: absolute;
  bottom: 50px;
  background-color: rgba(0,0,0, 0.5);
  color: white;
  text-align: center;
  display: flex;
  align-items:center;
  justify-content:center;
  height: 50px;
  width: 100%;
}

.v-window__next {
  right: 0;
}

@media (max-width: 500px) {
  .v-carousel {
    height: 400px!important;
  }
}

@media (max-width: 350px) {
  .v-carousel {
    height: 250px!important;
  }
}

@media (max-width: 200px) {
  .v-carousel {
    height: 150px!important;
  }
}
</style>
