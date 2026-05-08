# API de sugerencias para Homepage

QOwnNotes can expose a local HTTP endpoint for [Homepage](https://github.com/gethomepage/homepage) `suggestionUrl` support,
backed by the same bookmark parsing/indexing used by the Web Companion data source.

## Habilitar en QOwnNotes

1. Abra `Configuración -> Extensión del navegador/fragmentos de comandos`.
2. Enable `Enable socket server`.
3. In `Bookmark suggestion API`, enable `Enable Homepage-compatible bookmark suggestions API` and set a port.

El servicio solo se conecta a `127.0.0.1`.

## Endpoint

- `GET /suggest?q=home`
- Opcional: `limit` (valor predeterminado `10`, valor máximo `50`)
- Optional: `token` (if you configured a security token in QOwnNotes)

If you use the custom Homepage assets from `docs/homepage/custom.js`, set `QON_TOKEN` to the same
security token you configured in QOwnNotes. The script will then append `&token=...` when requesting
suggestions.

Respuesta de ejemplo:

```json
["home", ["Homepage", "https://ejemplo.com/home", "Un enlace de página de inicio"]]
```

## Ejemplo de configuración de Homepage

```yaml
search:
  provider: custom
  url: https://ejemplo.com/search?q=
  suggestionUrl: http://127.0.0.1:22224/suggest?q=
  showSearchSuggestions: true
```

If you protect the endpoint with a security token and use `custom.js`, keep `suggestionUrl` unchanged
and set `QON_TOKEN` inside `custom.js` instead.

## Qué archivo de Homepage editar

En una instalación estándar de Homepage, añada el bloque «search» en uno de estos archivos:

- `settings.yaml` (global Homepage settings)
- `settings.yml` (igual que lo anterior, dependiendo de su configuración)
- El archivo montado en su contenedor como `/app/config/settings.yaml`

Si la configuración de su Homepage se divide en varios archivos, edite el archivo que ya contiene la configuración de su proveedor de búsqueda.

## Recursos para Homepage personalizado

This repository also contains a ready-to-use Homepage customization example in:

- [docs/homepage on GitHub](https://github.com/pbek/QOwnNotes/tree/main/docs/homepage)

Utilice estos archivos cuando la implementación de su Homepage admita la carga de `custom.js` y `custom.css`.

Si su Homepage se ejecuta en un servidor externo (máquina/contenedor diferente al de QOwnNotes),
necesita estos archivos en esa instancia de Homepage alojada para que pueda consultar la API de sugerencias de QOwnNotes local y combinar/mostrar esas sugerencias en la búsqueda de Homepage.

When you use that setup with a QOwnNotes security token, make sure the same token is configured in
`QON_TOKEN` inside `custom.js`.

If you need network access beyond localhost, place a reverse proxy in front of the endpoint and restrict access
(for example by firewall rules or allowlist), since bookmark data may contain sensitive URLs.
