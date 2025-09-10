# Instalar como AppImage

Puede descargar la última AppImage de la [página de lanzamientos de QOwnNotes](https://github.com/pbek/QOwnNotes/releases). Debería llamarse algo parecido a `QOwnNotes-x86_64.AppImage` en esa página.

::: tip
Si tiene [jq](https://stedolan.github.io/jq/) instalado, también puede descarga la última AppImage directamente:

```bash
# consulte el último lanzamiento de Linux desde la API de QOwnNotes, analice el JSON para la URL y descárguelo
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```

:::

Luego puede cambiar los permisos de ejecución en el archivo:

```bash
chmod a+x QOwnNotes-*.AppImage
```

::: tip
¡AppImages necesita `libfuse2` para ejecutarse! Por ejemplo, en Ubuntu puede instalar la biblioteca con `sudo apt install libfuse2`.
:::

Posteriormente, debería ser capaz de ejecutar la AppImage para usar QOwnNotes.

::: warning
Si desea utilizar el **actualizador automático**, asegúrese de colocar su AppImage en un lugar donde su cuenta de usuario tenga acceso de escritura, como en algún lugar de su directorio de inicio.
:::

::: tip
Si tiene problemas para ejecutar AppImage, porque su versión de glibc es demasiado antigua, puede probar la [AppImage construida en OBS](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage), que debería estar construida con glibc 2.16.
:::
