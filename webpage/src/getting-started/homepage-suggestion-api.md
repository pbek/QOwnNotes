# Homepage suggestion API

QOwnNotes can expose a local HTTP endpoint for [Homepage](https://github.com/gethomepage/homepage) `suggestionUrl` support,
backed by the same bookmark parsing/indexing used by the Web Companion data source.

## Enable in QOwnNotes

1. Open `Settings -> Browser extension / command snippets`.
2. Enable `Enable socket server`.
3. In `Bookmark suggestion API`, enable `Enable Homepage-compatible bookmark suggestions API` and set a port.

The service binds to `127.0.0.1` only.

## Endpoint

- `GET /suggest?q=home`
- Optional: `limit` (default `10`, max `50`)
- Optional: `token` (if you configured a security token in QOwnNotes)

If you use the custom Homepage assets from `docs/homepage/custom.js`, set `QON_TOKEN` to the same
security token you configured in QOwnNotes. The script will then append `&token=...` when requesting
suggestions.

Example response:

```json
["home", ["Homepage", "https://example.com/home", "Some Home Link"]]
```

## Homepage config example

```yaml
search:
  provider: custom
  url: https://example.com/search?q=
  suggestionUrl: http://127.0.0.1:22224/suggest?q=
  showSearchSuggestions: true
```

If you protect the endpoint with a security token and use `custom.js`, keep `suggestionUrl` unchanged
and set `QON_TOKEN` inside `custom.js` instead.

## Which Homepage file to edit

In a standard Homepage installation, add the `search` block in one of these files:

- `settings.yaml` (global Homepage settings)
- `settings.yml` (same as above, depending on your setup)
- the file mounted into your container as `/app/config/settings.yaml`

If your Homepage deployment splits configuration across multiple files, edit the file that already contains your `search` provider settings.

## Custom Homepage assets

This repository also contains a ready-to-use Homepage customization example in:

- [docs/homepage on GitHub](https://github.com/pbek/QOwnNotes/tree/main/docs/homepage)

Use these files when your Homepage deployment supports loading `custom.js` and `custom.css`.

If your Homepage runs on an external host (different machine / container than QOwnNotes),
you need these files on that hosted Homepage instance so it can query your local QOwnNotes
suggestion API and merge/display those suggestions in Homepage search.

When you use that setup with a QOwnNotes security token, make sure the same token is configured in
`QON_TOKEN` inside `custom.js`.

If you need network access beyond localhost, place a reverse proxy in front of the endpoint and restrict access
(for example by firewall rules or allowlist), since bookmark data may contain sensitive URLs.
