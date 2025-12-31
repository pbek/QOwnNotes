# AI 지원

좋아하는 AI 모델 (LLM)을 사용하여 노트 작성을 도와드릴 수 있습니다.
**[Groq](https://groq.com/)** (무료 API 키를 받을 수 있기 때문에) 및 **[OpenAI](https://openai.com/)** 에 대한 기본 지원이 있습니다
하지만 내부 API는 스크립트를 사용하는 다른 AI 모델로 쉽게 확장할 수 있도록 설계되었습니다.
스크립트 저장소에는 시작할 수 있도록 여러 AI 백엔드 통합 스크립트가 있습니다.

:::tip
\*OpenAI completion API\*\*는 **scripts**에서 사용하도록 구현되었습니다.
이렇게 하면 원하는 것을 무엇이든 할 수 있고 실험할 수 있습니다.
:::

- **AI 도구 모음**과 **편집 기본 메뉴**에 서비스를 켤 수 있는 섹션이 있으며, AI 백엔드와 모델을 선택할 수 있습니다
- 새로운 **AI 설정**에서 [OpenAI](https://openai.com/) 및 [Groq](https://groq.com/) API 키를 추가할 수 있습니다
  - 스크립트 저장소에서 _Ollama_ 및 _llama.cpp_와 같은 **AI 백엔드 통합**을 더 찾아보세요
- 새로운 `script.aiComplete(prompt)` 스크립팅 명령을 자신의 상상력에 맞게 사용하세요
  - 자세한 내용은 다음을 참조하시기 바랍니다
    [Scripting documentation](../scripting/methods-and-objects.md#use-a-completion-prompt-on-the-currently-selected-ai-model)
- **스크립트 저장소**에서는 첫 번째 단계로 스크립트가 있습니다
  [AI 자동 완성](https://github.com/qownnotes/scripts/tree/master/ai-autocompletion),
  선택한 AI 모델을 사용하여 노트 텍스트 편집에서 **현재 선택을 자동으로 완료**합니다,
  스크립트 [AI 텍스트 도구](https://github.com/qownnotes/scripts/tree/master/ai-text-tool),
  선택된 AI 모델을 사용하여 노트 텍스트 편집에서 **현재 선택**을 처리합니다,
  예를 들어 요약하거나 번역하거나 오타와 문법 오류를 수정하는 것입니다

![ai-settings](/img/editor/ai-settings.webp)
