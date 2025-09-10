# Soporte de IA

Puede optar por utilizar su modelo de IA favorito (LLM) para ayudarle con sus notas.
Hay soporte integrado para **[Groq](https://groq.com/)** (porque puede obtener claves gratuitas de la API) y **[OpenAI](https://openai.com/)**,
pero la API interna está diseñada para poder extenderse fácilmente a otros modelos de IA con scripts.
Hay varios scripts de integración de backend de IA disponibles en el repositorio de scripts para que pueda comenzar.

:::tip
La **API de finalización de OpenAI** se implementó para utilizarse en **scripts**.
De esta manera puede hacer lo que quiera y experimentar con ella.
:::

- Hay una **barra de herramientas de IA** y una sección en el **menú principal Editar**, en donde puede activar
  el servicio activado y seleccionar el backend y el modelo de IA
- Puede agregar una clave de la API de [OpenAI](https://openai.com/) y [Groq](https://groq.com/) en la nueva **configuración de IA**
  - Busque más **integraciones de backend de IA**, como _Ollama_ y _llama.cpp_ en el repositorio de scripts
- Utilice la nueva orden de secuencias de órdenes "script.aiComplete(prompt)" de acuerdo con su propia imaginación
  - Para más información, por favor revise la
    [documentación de secuencias de órdenes](../scripting/methods-and-objects.md#use-a-completion-prompt-on-the-currently-selected-ai-model)
- En el **repositorio de scripts**, como primer paso, hay un script de
  [finalización automática de IA](https://github.com/qownnotes/scripts/tree/master/ai-autocompletion),
  que utiliza el modelo de IA seleccionado para **completar automáticamente la selección actual** en la edición del texto de la nota,
  y un script de [herramienta de texto de IA](https://github.com/qownnotes/scripts/tree/master/ai-text-tool),
  que utiliza el modelo de IA seleccionado para **procesar la selección actual** en la edición del texto de la nota,
  por ejemplo para resumirlo, traducirlo o corregir errores tipográficos y gramaticales

![ai-settings](/img/editor/ai-settings.webp)
