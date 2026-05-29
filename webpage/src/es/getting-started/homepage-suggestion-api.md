# API de sugerencias para Homepage

QOwnNotes puede exponer un endpoint HTTP local para la compatibilidad con `suggestionUrl` de [Homepage](https://github.com/gethomepage/homepage), respaldado por el mismo análisis/indexación de marcadores que utiliza la fuente de datos de Web Companion.

## Habilitar en QOwnNotes

1. Abra `Configuración -> Extensión del navegador/fragmentos de comandos`.
2. Active `Habilitar servidor de sockets`.
3. En `API de sugerencias de marcadores`, habilite `Activar API de sugerencias de marcadores compatible con Homepage`.

El servicio solo se conecta a `127.0.0.1`.

## Endpoint

- `GET /suggest?q=home`
- Opcional: `limit` (valor predeterminado `10`, valor máximo `50`)
- Opcional: `token` (si configuró un token de seguridad en QOwnNotes)

Si utiliza los recursos personalizados de Homepage de `docs/homepage/custom.js`, establezca `QON_TOKEN` con el mismo token de seguridad que configuró en QOwnNotes. El script añadirá entonces `&token=...` al solicitar sugerencias.

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

Si protege el endpoint con un token de seguridad y utiliza `custom.js`, mantenga `suggestionUrl` sin cambios y configure `QON_TOKEN` dentro de `custom.js`.

## Qué archivo de Homepage editar

En una instalación estándar de Homepage, añada el bloque «search» en uno de estos archivos:

- `settings.yaml` (configuración global de Homepage)
- `settings.yml` (igual que lo anterior, dependiendo de su configuración)
- El archivo montado en su contenedor como `/app/config/settings.yaml`

Si la configuración de su Homepage se divide en varios archivos, edite el archivo que ya contiene la configuración de su proveedor de búsqueda.

## Recursos para Homepage personalizado

Este repositorio también contiene un ejemplo de personalización de Homepage listo para usarse en:

- [docs/homepage en GitHub](https://github.com/pbek/QOwnNotes/tree/main/docs/homepage)

Utilice estos archivos cuando la implementación de su Homepage admita la carga de `custom.js` y `custom.css`.

Si su Homepage se ejecuta en un servidor externo (máquina/contenedor diferente al de QOwnNotes),
necesita estos archivos en esa instancia de Homepage alojada para que pueda consultar la API de sugerencias de QOwnNotes local y combinar/mostrar esas sugerencias en la búsqueda de Homepage.

Cuando utilice esa configuración con un token de seguridad de QOwnNotes, asegúrese de que el mismo token esté configurado en
`QON_TOKEN` dentro de `custom.js`.

Si necesita acceso a la red más allá de localhost, coloque un proxy inverso delante del endpoint y restrinja el acceso (por ejemplo, mediante reglas de firewall o una lista de permitidos), ya que los datos de los marcadores pueden contener URL confidenciales.
