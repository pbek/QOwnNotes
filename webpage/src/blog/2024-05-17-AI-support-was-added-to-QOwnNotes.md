---
title: AI support was added to QOwnNotes
description: QOwnNotes now has AI support, currently for Groq and OpenAI!
image: /assets/img/robot-taking-notes.jpg
date: 2024-05-17
order: 20240517000000
---

# AI support was added to QOwnNotes

<BlogDate v-bind:fm="$frontmatter" />

QOwnNotes now has **AI support**, currently for **[Groq](https://groq.com/)** (because you can get free API keys) and **[OpenAI](https://openai.com/)**!

- There is a new **AI toolbar** where can turn the service on, and you can select the AI backend and model
- As a first step, there is a new script `AI Autocompletion` in the script repository
  that uses the selected AI model to **autocomplete the current block** in the note text edit
- You can now add a [OpenAI](https://openai.com/) and [Groq](https://groq.com/) API key in the new *AI settings*
- Use the new `script.aiComplete(prompt)` scripting command to your own imagination
  - For more information please take a look at the
    [Scripting documentation](https://www.qownnotes.org/scripting/methods-and-objects.html#use-a-completion-prompt-on-the-currently-selected-ai-model)

![robot-taking-notes](./media/robot-taking-notes.jpg)
