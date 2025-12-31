---
title: AI support was added to QOwnNotes
description: QOwnNotes now has AI support, currently for Groq and OpenAI!
image: /assets/robot-taking-notes.jpg
date: 2024-05-17
order: 20240517000000
---

# AI support was added to QOwnNotes

<BlogDate v-bind:fm="$frontmatter" />

QOwnNotes now has **"AI support"** as part of [#3026](https://github.com/pbek/QOwnNotes/issues/3026),
currently for **[Groq](https://groq.com/)** (because you can get free API keys) and **[OpenAI](https://openai.com/)**!

The **OpenAI completion API** was implemented to be used in **scripts**.
This way you can do whatever you want and experiment with it.

- There is a new **AI toolbar** and a section in the **Edit main menu**, where can turn
  the service on, and you can select the AI backend and model
- You can add an [OpenAI](https://openai.com/) and [Groq](https://groq.com/) API key in the new _AI settings_
- Use the new `script.aiComplete(prompt)` scripting command to your own imagination
  - For more information please take a look at the
    [Scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#use-a-completion-prompt-on-the-currently-selected-ai-model)
- In the **script repository**, as a first step, there is a new script
  [AI Autocompletion](https://github.com/qownnotes/scripts/tree/master/ai-autocompletion),
  that uses the selected AI model to **autocomplete the current selection** in the note text edit,
  and a new script [AI Text Tool](https://github.com/qownnotes/scripts/tree/master/ai-text-tool)
  that uses the selected AI model to **process the current selection** in the note text edit,
  for example to summarize it, to translate it or to fix typos and grammar errors

![robot-taking-notes](./media/robot-taking-notes.jpg)
