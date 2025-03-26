# AI-ondersteuning

Je kunt ervoor kiezen om je favoriete AI-model (LLM) te gebruiken om je te helpen met je notities.
Er is ingebouwde ondersteuning voor **[Groq](https://groq.com/)** (omdat je gratis API-sleutels kunt krijgen) en **[OpenAI](https://openai.com/)**, maar de interne API is ontworpen om gemakkelijk uit te breiden naar andere AI-modellen met scripts.
Er zijn verschillende scripts voor AI-backend-integraties beschikbaar in de script-opslagplaats om je op weg te helpen.

:::tip
De **OpenAI-voltooiings-API** is geïmplementeerd om te worden gebruikt in **scripts**.
Zo kun je doen wat je wilt en ermee experimenteren.
:::

- Er is een **AI-werkbalk** en een sectie in het **Hoofdmenu bewerken**, waar je de service aan kunt zetten, en de AI-backend en het AI-model selecteren
- Je kunt een [OpenAI](https://openai.com/) en [Groq](https://groq.com/) API-sleutel toevoegen in de nieuwe **AI-instellingen**
  - Look for more **AI backend integrations**, like _Ollama_ and _llama.cpp_ in the script repository
- Gebruik het nieuwe scriptcommando 'script.aiComplete(prompt)' naar je eigen idee
  - Voor meer informatie kun je de
    [Documentatie voor scripts](.. /scripting/methods-and-objects.md#use-a-completion-prompt-on-the-currently-selected-ai-model) bestuderen
- In de **script-opslagplaats** is er om te beginnen een script
  [AI-automatische voltooiing](https://github.com/qownnotes/scripts/tree/master/ai-autocompletion),
  die het geselecteerde AI-model gebruikt om **de huidige selectie automatisch aan te vullen** in de tekstbewerking van de notitie,
  en een script [AI Text Tool](https://github.com/qownnotes/scripts/tree/master/ai-text-tool),
  die het geselecteerde AI-model gebruikt om **de huidige selectie te verwerken** in de tekstbewerking van de notitie,
  bijvoorbeeld om het samen te vatten, te vertalen of om typefouten en grammaticale fouten te herstellen

![ai-instellingen](/img/editor/ai-settings.webp)
