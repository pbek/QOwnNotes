# Homepage custom integration (QOwnNotes suggestions)

This folder contains custom frontend assets for a Homepage instance.

- `custom.js`: Merges QOwnNotes suggestions into Homepage search suggestions and fixes opening behavior for mapped bookmark suggestions.
- `custom.css`: Adds visual styling for suggestions marked as coming from QOwnNotes.

## What `custom.js` does

1. Intercepts calls to Homepage's suggestion endpoint (`/api/search/searchSuggestion`).
2. Fetches QOwnNotes suggestions from:
   - `http://127.0.0.1:22224/suggest?q=<query>&limit=<QON_LIMIT>`
3. Merges Homepage and QOwnNotes suggestions into a single response.
4. Prefixes QOwnNotes suggestions with `[QON] ` so they are visible in the UI.
5. Builds a suggestion-to-URL map (for paired name/URL entries from QOwnNotes).
6. Intercepts `window.open(...)` to open mapped bookmark URLs directly in a new tab instead of searching them with the active search provider.
7. Prevents Homepage's client-side hard trim to 4 suggestions for merged responses.
8. Adds the CSS class `qon-suggestion` to matching suggestion rows.

## What `custom.css` does

- Styles `.qon-suggestion` rows with a left border and green text so QOwnNotes suggestions are visually distinct.

## Configuration knobs in `custom.js`

- `QON_URL`: QOwnNotes local suggestion API endpoint.
- `QON_LIMIT`: Number of QOwnNotes suggestions to fetch.
- `QON_LABEL`: Prefix shown for QOwnNotes suggestion text.
- `DEBUG`: Set to `true` to enable console logging.

## Usage

Place these files where your Homepage deployment loads custom assets as `custom.js` and `custom.css`.

After updating either file, refresh the Homepage UI (hard reload recommended) so the browser picks up the new versions.
