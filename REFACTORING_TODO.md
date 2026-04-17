# Settings Dialog Refactoring TODO

Issue: [#3570](https://github.com/pbek/QOwnNotes/issues/3570)

## Goal

Extract each page of `settingsStackedWidget` in `settingsdialog.ui` into separate
widget classes with their own `.h`, `.cpp`, and `.ui` files, following the
`ColorModeSettingsWidget` pattern. Files go in `src/widgets/settings/`.

## Pattern

Each widget has:

- `initialize()` — lazy init called from `initializePage()`
- `readSettings()` — load settings from QSettings
- `storeSettings()` — save settings to QSettings
- In `settingsdialog.ui`: page has zero-margin grid layout containing promoted widget
- `settingsdialog.cpp` only calls widget methods; all logic lives in the widget

## Completed Extractions

- [x] ColorModeSettingsWidget (original pattern)
- [x] GitSettingsWidget
- [x] LocalTrashSettingsWidget
- [x] DebugOptionSettingsWidget (`removeLogFile()` made static)
- [x] EditorFontColorSettingsWidget (`needRestart` signal, wiki link visibility)
- [x] WebApplicationSettingsWidget (`needRestart` signal, 7 slots moved)
- [x] PreviewFontSettingsWidget (font members moved)
- [x] DebugSettingsWidget (`aboutToOutputSettings()` + `issueAssistantRequested()` signals)
- [x] LanguageToolSettingsWidget (`#ifdef LANGUAGETOOL_ENABLED` handled internally)
- [x] NetworkSettingsWidget (proxy, SSL, app metrics all moved)
- [x] TodoSettingsWidget (cross-page deps handled via signals + forwarding methods)
- [x] AiSettingsWidget (`searchScriptRepositoryRequested` signal, `buildAiScriptingTreeWidget()` called from dialog on page switch)
- [x] WebCompanionSettingsWidget (`needRestart` signal, 8 slots moved, `enableSocketServer` + bookmarks/commands settings)
- [x] EditorSettingsWidget (`needRestart` signal, `wikiLinkSupportToggled` cross-page signal, 5 slots moved)
- [x] Moved all widgets to `src/widgets/settings/`
- [x] Updated CHANGELOG.md

## Remaining Pages to Extract (ordered by complexity)

### Higher Complexity

| Page            | UI Lines | Notes                                                                 |
| --------------- | -------- | --------------------------------------------------------------------- |
| panelsPage      | 495      |                                                                       |
| mcpServerPage   | ~467     |                                                                       |
| noteFoldersPage | 469      | Cross-page deps with cloud connections                                |
| scriptingPage   | 569      | Complex: script management                                            |
| ownCloudPage    | 647      | Cross-page deps: `initCloudConnectionComboBox`, `replaceOwnCloudText` |
| generalPage     | 663      |                                                                       |
| interfacePage   | 1170     | Largest page                                                          |

### Skipped (too small to extract)

- portableModePage (30 lines)
- experimentalPage (36)
- toolbarPage (42)
- layoutPage (65)
- shortcutsPage (69)

## Key Cross-Page Interactions

- `initCloudConnectionComboBox()` populates combo boxes on ownCloudPage, noteFoldersPage, and todoPage
- `replaceOwnCloudText()` modifies text on ownCloudPage, noteFoldersPage, and todoPage
- `on_searchLineEdit_textChanged` searches across ALL pages' child widgets (still works since widgets are children)
- `storeOwncloudDebugData` uses private members of SettingsDialog — stays in dialog, widget emits signal
- Some `_initial*` members track state for cancel/restore in `closeEvent()`

## Critical Lessons Learned

1. When removing functions from `settingsdialog.cpp`, verify exactly which functions
   exist before/after using `grep '^void SettingsDialog::' | sort` and diff.
2. Some functions appear with different comment blocks. Include enough unique context
   to avoid matching wrong blocks.
3. `automaticNoteFolderDatabaseClosingCheckBox->hide()` belongs to general page, not git.
4. Watch for duplicate declarations in `settingsdialog.h` when replacing `#ifdef` blocks.
5. Include paths from `widgets/settings/` need `dialogs/` prefix for dialog headers.
6. Custom widget headers in `.ui` files must use `location="global"` and paths like
   `widgets/settings/gitsettingswidget.h`.
7. The `/* */` nested comment issue: never leave duplicate `/**` when editing comments.
