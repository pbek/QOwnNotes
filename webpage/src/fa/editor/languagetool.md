# LanguageTool

QOwnNotes can use a local or remote [LanguageTool](https://languagetool.org/) service for
optional **grammar and style checking** in the note editor.

It complements the existing [spellchecking](spellchecking.md) support instead of replacing it.

## Features

- Colored underlines for detected grammar, style, punctuation, and typography issues
- Context-menu suggestions for replacements
- `More info...` links for rules that provide extra documentation
- `Ignore this rule` actions for rules you do not want to see again
- A quick toggle in the **Spelling** menu with `Check grammar with LanguageTool`

## Setup

Open `Settings -> Editor` and configure the **LanguageTool** section.

- Turn on `Enable grammar and style checking with LanguageTool`
- Set the `Server URL`
  - Local servers can use something like `http://localhost:8081`
  - QOwnNotes automatically uses the `/v2/check` endpoint on that server
- Choose the `Language`
  - `Auto-detect` is available
  - You can also enter a custom language code if needed
- Add an optional `API key` if your LanguageTool service requires one
- Adjust the `Check delay` to control how long QOwnNotes waits after typing before sending a request
- Select which categories should be checked
  - `Spelling`
  - `Grammar`
  - `Style`
  - `Punctuation`
  - `Typography`

Use `Test Connection` to verify that the configured service is reachable.

## How It Works

- QOwnNotes checks visible editor blocks instead of the whole document at once
- Empty lines, headings, and code blocks are skipped
- Requests are debounced so the service is not queried on every keystroke
- Results are shown inline in the Markdown editor

## Notes

- LanguageTool support is optional and depends on the feature being enabled in your build
- If the service cannot be reached, QOwnNotes shows a warning and stops checking until it is available again
- You can use it with a local self-hosted LanguageTool server or another compatible LanguageTool endpoint

## Related

- [Spellchecking](spellchecking.md)
- [Concept](../getting-started/concept.md)
