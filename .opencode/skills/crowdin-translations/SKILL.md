---
name: Crowdin Translations
description: Translate missing QOwnNotes application strings directly in Crowdin for German, Simplified Chinese, Russian, Italian, Brazilian Portuguese, Polish, French, and Spanish while preserving existing translations.
---

# Crowdin translations

Use this skill to fill missing application translations in the QOwnNotes Crowdin
project. Creating or updating this skill alone does not authorize submitting
translations. Respect Plan mode and obtain authorization before remote writes
when the user's request does not already authorize them.

## Default languages and scope

Unless the user selects a subset, use all eight languages:

| Requested locale | Crowdin language ID | Local application catalog          |
| ---------------- | ------------------- | ---------------------------------- |
| `de_DE`          | `de`                | `src/languages/QOwnNotes_de.ts`    |
| `zh_CN`          | `zh-CN`             | `src/languages/QOwnNotes_zh_CN.ts` |
| `ru_RU`          | `ru`                | `src/languages/QOwnNotes_ru.ts`    |
| `it_IT`          | `it`                | `src/languages/QOwnNotes_it.ts`    |
| `pt_BR`          | `pt-BR`             | `src/languages/QOwnNotes_pt_BR.ts` |
| `pl_PL`          | `pl`                | `src/languages/QOwnNotes_pl.ts`    |
| `fr_FR`          | `fr`                | `src/languages/QOwnNotes_fr.ts`    |
| `es_ES`          | `es-ES`             | `src/languages/QOwnNotes_es.ts`    |

Confirm language IDs against the live project. Read `crowdin.yaml` for the project
ID and file mapping rather than assuming they never change. The application
source is currently `src/languages/QOwnNotes_en.ts`, mapped to `/QOwnNotes_en.ts`
in Crowdin. Do not confuse it with website Markdown or browser-extension files.

Default to application strings only. Do not expand to the website, change source
strings, download translations into the repository, update release files, or
commit changes unless requested. Keep temporary scripts, snapshots, and reports
in a unique task directory under `/tmp/opencode`.

## Safety rules

- Fill missing translations and missing plural categories only. Preserve all
  existing suggestions, effective translations, and approvals, even if they have
  QA warnings. Fixing existing translations is a separate task.
- Submit new translations without approving them. Do not reset approvals, vote,
  or mark QA warnings resolved just to obtain a clean report.
- Do not invoke paid MT or AI engines without explicit authorization. Translate
  strings yourself; free reuse of existing project wording is preferable.
- Use the installed CLI's credential resolution. Credentials may be in
  `~/.crowdin.yaml` or the environment. Never print credentials, include tokens
  in command arguments, or write them into the repository, reports, or skill.
  When API calls are necessary, load credentials privately into process memory
  and use an Authorization header. Redact sensitive errors.
- Never use generic `crowdin upload` or `just translations-upload` for this task:
  they upload source files, not just the missing translations.

## Workflow

### 1. Discover live gaps

1. Read repository instructions and `crowdin.yaml`; check `git status --short`.
2. Check `crowdin --help` and relevant subcommand help for the installed version.
   Verify access with `crowdin status --no-progress --no-colors`.
3. Resolve the application file ID and requested languages using the live API.
   Fetch source strings, including their context, identifiers, and plural data.
4. Snapshot existing translations and approvals as needed for preservation
   checks. Local `.ts` files can provide context, but they may be stale and their
   unfinished counts are not authoritative Crowdin counts.
5. Paginate API collections completely. A typical page size is 500; honor the
   endpoint's actual limits. Keep a resumable submission log.

Useful API v2 paths, relative to `https://api.crowdin.com/api/v2`:

```text
GET /projects/{projectId}
GET /projects/{projectId}/files
GET /projects/{projectId}/strings?fileId={fileId}
GET /projects/{projectId}/languages/{languageId}/translations?fileId={fileId}
GET /projects/{projectId}/translations?stringId={stringId}&languageId={languageId}
GET /projects/{projectId}/files/{fileId}/languages/progress
```

The language-translations endpoint returns plain `text` or a `plurals` array.
It can omit incompletely translated plural strings entirely. An absent entry
therefore does **not** prove that a string has no existing suggestions. Query
the string-translations endpoint before writing; for plurals, check each
`pluralCategoryName` separately and preserve the categories already present.
Skip hidden source strings unless explicitly requested.

### 2. Reuse established translations

If authorized to write, first try perfect translation-memory matches for missing
application strings only. For the current CLI, the command is:

```sh
crowdin pre-translate --method=tm --file=/QOwnNotes_en.ts \
  -l de -l zh-CN -l ru -l it -l pt-BR -l pl -l fr -l es-ES \
  --translate-untranslated-only --translate-with-perfect-match-only \
  --no-duplicate-translations --no-progress --no-colors
```

Adjust the languages and file path to the requested scope. Leave auto-approval
unset. Refresh gaps afterward. Never assume that a successful TM job filled all
gaps or that fuzzy matches are acceptable without review.

An existing translation of exactly the same source text can be reused if the
contexts have the same meaning and there are no conflicting translations. Do
not propagate ambiguous short labels blindly. Check the glossary and surrounding
UI wording before writing new translations.

### 3. Translate and validate

Work in manageable batches and use source contexts and application code to
resolve ambiguities. Preserve product names, commands, protocol names, and
literal examples unless localization is appropriate. Numeric bookmark slots can
share a reviewed translation template when only the slot number changes.

Before submission, check:

- Qt placeholders, including `%1`, `%2`, `%L1`, `%n`, `%Ln`, `%v`, and `%m`.
  Preserve their identities and multiplicity; allow grammatical reordering.
- HTML tags, attributes, URLs, Markdown links, command-line flags, and constants
  such as `QOWNNOTESAPI_MIN_VERSION`.
- Meaningful line breaks, leading/trailing spaces used for concatenation, and
  Qt `&` keyboard accelerators. Accelerators can move to a suitable letter.
- Natural punctuation and ellipses for each locale. Chinese UI strings may need
  `。` and `……` rather than English punctuation.
- All plural categories required by the live language metadata, not merely the
  categories present in the English source. In particular, Russian and Polish
  also require `few` and `many`; preserve an existing `other` category as well.
  Read `pluralCategoryNames` instead of hardcoding a universal category list.
- Correct singular, plural, gender, and case. Do not copy English `(s)` notation
  into natural translated plural forms or remove `%n` from singular forms.

### 4. Submit missing entries only

Prefer the CLI where it supports the operation safely. For individual missing
strings, the API avoids uploading whole catalogs or unrelated files:

```text
POST /projects/{projectId}/translations
{
  "stringId": 123,
  "languageId": "fr",
  "text": "...",
  "pluralCategoryName": "one"
}
```

Omit `pluralCategoryName` for nonplural strings. Immediately before posting,
recheck existing suggestions for that string/language/category. Skip entries
someone else has translated in the meantime. Do not approve the result.

Use bounded concurrency, respect rate limits, and retry transient failures with
backoff. After an ambiguous POST failure, recheck whether the translation was
created before retrying. Log language, string ID, plural category, submitted text,
and returned translation ID. Resume from that log instead of duplicating work.
Do not run overlapping upload batches for the same string/language/category.

Crowdin's individual translation endpoint does not support editing text with a
JSON Patch request. If one of **this task's own new submissions** needs correction,
verify its ID and unchanged text, add the corrected suggestion, then remove only
that exact task-created suggestion. Log the replacement ID. Never delete a
pre-existing or externally edited translation.

### 5. Verify and report

1. Wait for all submitted batches to finish, then refetch effective translations
   and application-file language progress.
2. Verify that every active source string has all required translated categories,
   all submitted placeholders remain valid, and pre-existing translations and
   approvals are unchanged. Recheck partial plural strings explicitly.
3. Review Crowdin QA warnings for the new translation IDs/categories. Product
   names, German compounds, Chinese accelerator notation, and omitted English
   `(s)` notation can cause false positives. Glossary checks may also mistake
   parts of compounds or confuse a tab character with a UI tab. Fix real issues,
   but do not distort valid translations or dismiss existing warnings.
4. Confirm repository files remain unchanged unless changes were requested.
5. Report live application-file completion for each language, any remaining gaps
   or blockers, and that new entries await review. Do not equate "100% translated"
   with "100% approved" or imply the website was translated.
