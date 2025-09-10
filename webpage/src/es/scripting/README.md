# Scripts de QOwnNotes

Un script de QOwnNotes es principalmente **JavaScript** en [ archivos Qt QML](https://doc.qt.io/qt-5/qtqml-index.html).

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
     * Se ejecutará cuando se inicialice el motor de secuencias de órdenes.
     */
    function init() {
        script.log("¡Hola mundo!");
    }
}
```

Puede colocar esos archivos QML en cualquier lugar que desee y **agregarlos en QOwnNotes** agregándolos en la **Configuración de secuencias de comandos** (hay un botón `Agregar secuencia de comandos` / `Agregar secuencia de comandos local`).

::: tip
Eche un vistazo a los [scripts de ejemplo](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples) para iniciar rápidamente.
:::

En la **Configuración de scripts** también puede instalar scripts directamente desde el [**Repositorio de scripts**](https://github.com/qownnotes/scripts). Existe una aplicación web alojada en la comunidad para buscar scripts en [beurt.github.io/QOwnNoteScriptsList](https://beurt.github.io/QOwnNoteScriptsList/).

Para problemas, preguntas o solicitudes de funciones para scripts del **Repositorio de scripts**, abra un problema en la [página de problemas del repositorio de scripts de QOwnNotes](https://github.com/qownnotes/scripts/issues).

::: tip
Si desea proponer un script para el **Repositorio de scripts**, siga las instrucciones del [Repositorio de scripts de QOwnNotes](https://github.com/qownnotes/scripts).
:::

Si necesita acceder a una determinada funcionalidad en QOwnNotes o tiene preguntas o ideas, abra un problema en la [página de problemas de QOwnNotes](https://github.com/pbek/QOwnNotes/issues).

::: tip
Para el registro, puede utilizar la orden `script.log()` para acceder al _Panel de registros_, que puede encender en el menú _Ventanas/Paneles_.
:::
