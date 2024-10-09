# Installa come AppImage

Puede descargar la última AppImage de la [página de versiones de QOwnNotes](https://github.com/pbek/QOwnNotes/releases). Debe llamarse como `QOwnNotes-x86_64.AppImage` en esa página.

::: tip
Si tiene [jq](https://stedolan.github.io/jq/) instalado, también puede descarga la última AppImage directamente:

```bash
# consulte la última versión de Linux desde la API QOwnNotes, analice el JSON para la URL y descárguelo
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```
:::

Luego puede cambiar los permisos de ejecución en el archivo:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Tras hacer esto, debería ser capaz de ejecutar el AppImage ejecutar QOwnNotes.

::: warning
Si desea utilizar el **actualizador automático**, asegúrese de colocar su AppImage en un lugar donde su cuenta de usuario tenga acceso de escritura, como en algún lugar de su directorio de inicio.
:::

::: tip
Si tiene problemas para ejecutar AppImage, porque su versión de glibc es demasiado antigua, puede probar la [AppImage construida en OBS](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage), debe estar construida con glibc 2.16.
:::
