# Control de versiones de Git

Puede configurar QOwnNotes para que realice cambios de los commits automáticamente en su carpeta de notas localmente a un repositorio de git en su carpeta de notas.

Esto puede realizarse en la configuración del _Control de versiones de Git_ en la _Configuración general_.

![git-note-folder-settings](/img/git/settings.png)

Necesita tener git instalado en su sistema, y el ejecutable de git necesita estar en su ruta PATH. También puede establecer la ruta del ejecutable de git en la configuración del _Control de versiones de Git_.

¡También necesita activar el soporte de git para cada carpeta de notas si desea usar el control de versiones de git en ellas!

![git-note-folder-settings](/img/git/note-folder-settings.png)

::: tip
Don't forget to set your git username and email address in the terminal:

```bash
git config --global user.email "tu-email@dominio.com"
git config --global user.name "Tu nombre"
```

:::
