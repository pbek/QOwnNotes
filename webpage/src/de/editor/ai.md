# KI Unterstützung

Sie können ihr favorisiertes KI-Modell (LLM) aktivieren, das Sie bei ihren Notizen unterstützen soll.
There is built-in support for **[Groq](https://groq.com/)** (because you can get free API keys) and **[OpenAI](https://openai.com/)**,
but the internal API is designed to be easily extendable to other AI models with scripts.
There are several AI backend integrations scripts available in the script repository to get you started.

:::tip
The **OpenAI completion API** was implemented to be used in **scripts**.
This way you can do whatever you want and experiment with it.
:::

- There is an **AI toolbar** and a section in the **Edit main menu**, where can turn
  the service on, and you can select the AI backend and model
- You can add an [OpenAI](https://openai.com/) and [Groq](https://groq.com/) API key in the new **AI settings**
  - Look for more **AI backend integrations**, like _Ollama_ and _llama.cpp_ in the script repository
- Use the new `script.aiComplete(prompt)` scripting command to your own imagination
  - For more information please take a look at the
    [Scripting documentation](../scripting/methods-and-objects.md#use-a-completion-prompt-on-the-currently-selected-ai-model)
- In the **script repository**, as a first step, there is a script
  [AI Autocompletion](https://github.com/qownnotes/scripts/tree/master/ai-autocompletion),
  that uses the selected AI model to **autocomplete the current selection** in the note text edit,
  and a script [AI Text Tool](https://github.com/qownnotes/scripts/tree/master/ai-text-tool),
  that uses the selected AI model to **process the current selection** in the note text edit,
  for example to summarize it, to translate it or to fix typos and grammar errors

![ai-settings](/img/editor/ai-settings.webp)
