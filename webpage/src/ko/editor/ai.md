# AI 지원

좋아하는 AI 모델 (LLM)을 사용하여 노트 작성을 도와드릴 수 있습니다.
**[Groq](https://groq.com/)** (무료 API 키를 받을 수 있기 때문에) 및 **[OpenAI](https://openai.com/)** 에 대한 기본 지원이 있습니다
하지만 내부 API는 스크립트를 사용하는 다른 AI 모델로 쉽게 확장할 수 있도록 설계되었습니다.
스크립트 저장소에는 시작할 수 있도록 여러 AI 백엔드 통합 스크립트가 있습니다.

::: tip
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

## MCP 서버

QOwnNotes에는 서버 전송 이벤트 (SSE) 전송을 통해 HTTP를 통해 외부 AI 에이전트가 사용자의 노트에 액세스할 수 있는 **MCP (모델 컨텍스트 프로토콜) 서버**가 내장되어 있습니다.

**AI / MCP 서버** 설정 페이지에서 MCP 서버를 활성화하고 구성할 수 있습니다.

- **MCP 서버 활성화/비활성화**
- **포트** 구성 (기본값: 22226)
- **베어러 토큰 인증** — 보안 토큰은 자동으로 생성되며 모든 요청의 '권한 부여' 헤더에 '베어러' 토큰으로 포함되어야 합니다

### 사용 가능한 MCP 도구

MCP 서버는 AI 에이전트에게 다음 도구를 노출합니다:

- 82
  **`search_notes`** — 현재 노트 폴더에 있는 모든 노트에서 전체 텍스트를 검색합니다.
  'query' 문자열 매개변수와 선택적 'limit' (기본값 20)을 허용합니다.
  일치하는 노트 이름과 각각의 짧은 미리보기를 반환합니다.
- **'fetch_note'** - '이름' 또는 'id'로 단일 노트의 전체 내용을 가져옵니다.
  노트 텍스트, 파일 이름, 노트 ID를 반환합니다.

### AI 에이전트 연결

AI 에이전트는 HTTP+SSE 전송을 사용하여 MCP 서버에 연결합니다:

1. **SSE 엔드포인트**: 'http://localhost:22226/sse 가져오기'  
   서버에서 보낸 이벤트 스트림을 엽니다. 서버는 JSON-RPC 메시지를 보내기 위한 URL을 포함하는 '엔드포인트' 이벤트를 전송합니다.
2. **메시지 엔드포인트**: 'POST http://localhost:22226/message?sessionId=<id>'
   여기로 MCP JSON-RPC 요청을 보냅니다. 응답은 SSE 스트림을 통해 전달됩니다.

모든 요청에는 헤더가 포함되어야 합니다:

```
권한 부여: Bearer <your-security-token>
```

### `curl` 요청 예시

토큰과 포트를 먼저 설정합니다:

```bash
export TOKEN="your-mcp-token"
export PORT=22226
```

한 터미널에서 SSE 스트림을 엽니다:

```bash
curl -N \
  -H "Accept: text/event-stream" \
  -H "Authorization: Bearer $TOKEN" \
  "http://localhost:$PORT/sse"
```

서버는 다음과 같은 URL을 포함하는 '엔드포인트' 이벤트를 전송합니다:

```text
event: endpoint
data: http://localhost:22226/message?sessionId=3d8c7b0e-...
```

다음 요청에서 해당 이벤트의 'sessionId'를 사용합니다. 'POST' 요청 자체가 '202 수락됨'을 반환하고, JSON-RPC 응답은 SSE 스트림을 통해 전달됩니다.

MCP 세션을 초기화합니다:

```bash
curl \
  -X POST \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  --data '{
    "jsonrpc": "2.0",
    "id": 1,
    "method": "initialize",
    "params": {}
  }' \
  "http://localhost:$PORT/message?sessionId=3d8c7b0e-..."
```

사용 가능한 MCP 도구 목록을 작성합니다:

```bash
curl \
  -X POST \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  --data '{
    "jsonrpc": "2.0",
    "id": 2,
    "method": "tools/list",
    "params": {}
  }' \
  "http://localhost:$PORT/message?sessionId=3d8c7b0e-..."
```

노트 검색:

```bash
curl \
  -X POST \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  --data '{
    "jsonrpc": "2.0",
    "id": 3,
    "method": "tools/call",
    "params": {
      "name": "search_notes",
      "arguments": {
        "query": "meeting notes",
        "limit": 5
      }
    }
  }' \
  "http://localhost:$PORT/message?sessionId=3d8c7b0e-..."
```

이름으로 노트 가져오기:

```bash
curl \
  -X POST \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  --data '{
    "jsonrpc": "2.0",
    "id": 4,
    "method": "tools/call",
    "params": {
      "name": "fetch_note",
      "arguments": {
        "name": "Daily Journal"
      }
    }
  }' \
  "http://localhost:$PORT/message?sessionId=3d8c7b0e-..."
```

ID로 노트 가져오기:

```bash
curl \
  -X POST \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  --data '{
    "jsonrpc": "2.0",
    "id": 5,
    "method": "tools/call",
    "params": {
      "name": "fetch_note",
      "arguments": {
        "id": 123
      }
    }
  }' \
  "http://localhost:$PORT/message?sessionId=3d8c7b0e-..."
```
