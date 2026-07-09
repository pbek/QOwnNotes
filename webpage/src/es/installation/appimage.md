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

## Modo portátil

Puede ejecutar la AppImage en modo **portátil** pasando el argumento `--portable`:

```bash
./QOwnNotes-x86_64.AppImage --portable
```

En el modo portátil, todos los datos de la aplicación se almacenan en una carpeta `Data` junto al archivo AppImage, en lugar de en su directorio personal. Esto incluye:

- Configuración (`Data/QOwnNotes.ini`)
- La base de datos de notas (`Data/QOwnNotes.sqlite`)
- Sus notas (`Data/Notes/`)
- Scripts, diccionarios de corrección ortográfica y otros datos de la aplicación

Esto facilita llevar QOwnNotes junto con todas sus notas en una unidad USB o cualquier otro dispositivo de almacenamiento portátil.

::: Consejo Asegúrese de que el directorio que contiene el archivo AppImage tenga permisos de escritura, para que la carpeta `Data` pueda crearse junto a él.
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on your Linux distribution and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
