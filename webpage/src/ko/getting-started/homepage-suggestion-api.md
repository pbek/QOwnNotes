# 홈페이지 제안 API

QOwnNotes는 웹 컴패니언 데이터 소스에서 사용하는 동일한 북마크 구문 분석/인덱스를 지원하는 [Homepage](https://github.com/gethomepage/homepage) `suggestionUrl` 지원을 위한 로컬 HTTP 엔드포인트를 노출할 수 있습니다.

## QOwnNotes에서 활성화

1. '설정 -> 브라우저 확장/명령 스니펫'을 엽니다.
2. '소켓 서버 활성화'를 활성화합니다.
3. '북마크 제안 API'에서 '홈페이지 호환 북마크 제안 API 활성화'를 활성화하고 포트를 설정합니다.

이 서비스는 '127.0.0.1'에만 바인딩됩니다.

## 엔드포인트

- `GET /suggest?q=home`
- 선택 사항: '제한' (기본값 '10', 최대 '50')
- 선택 사항: '토큰' (QOwnNotes에서 보안 토큰을 구성한 경우)

'docs/homepage/custom.js'의 사용자 지정 홈페이지 자산을 사용하는 경우, QOwnNotes에서 설정한 것과 동일한 보안 토큰으로 'QON_TOKEN'을 설정합니다. 그런 다음 제안을 요청할 때 스크립트에 `&token=...`가 추가됩니다.

예시 응답:

```json
["home", ["Homepage", "https://example.com/home", "Some Home Link"]]
```

## 홈페이지 구성 예시

```yaml
search:
  provider: custom
  url: https://example.com/search?q=
  suggestionUrl: http://127.0.0.1:22224/suggest?q=
  showSearchSuggestions: true
```

보안 토큰으로 엔드포인트를 보호하고 'custom.js'를 사용하는 경우 '제안 Url'을 변경하지 않고 'custom.js' 내부에 'QON_TOKEN'을 설정합니다.

## 편집할 홈페이지 파일

표준 홈페이지 설치에서 다음 파일 중 하나에 '검색' 블록을 추가합니다:

- `settings.yaml` (전역 홈페이지 설정)
- `settings.yml` (위와 동일하지만, 설정에 따라 다름)
- 컨테이너에 '/app/config/settings.yaml'로 마운트된 파일

홈페이지 배포가 여러 파일에 걸쳐 구성을 분할하는 경우, 이미 '검색' 제공자 설정이 포함된 파일을 편집합니다.

## 사용자 지정 홈페이지 자산

이 저장소에는 즉시 사용할 수 있는 홈페이지 사용자 지정 예제도 포함되어 있습니다:

- [docs/homepage on GitHub](https://github.com/pbek/QOwnNotes/tree/main/docs/homepage)

이 파일들은 홈페이지 배포에서 'custom.js'와 'custom.css' 로딩을 지원할 때 사용하세요.

귀하의 홈페이지가 외부 호스트 (QOwnNotes와 다른 기계/컨테이너)에서 실행되는 경우,
호스팅된 홈페이지 인스턴스에 이 파일들이 있어야 로컬 QOwnNotes를 조회할 수 있습니다
제안 API를 사용하여 홈페이지 검색에서 해당 제안을 병합/표시합니다.

해당 설정을 QOwnNotes 보안 토큰과 함께 사용할 때는 'custom.js' 내부의 'QON_TOKEN'에 동일한 토큰이 구성되어 있는지 확인하세요.

localhost 외의 네트워크 액세스가 필요한 경우 엔드포인트 앞에 역방향 프록시를 설치하고 액세스를 제한합니다
(예: 방화벽 규칙 또는 허용 목록) 북마크 데이터에 민감한 URL이 포함될 수 있기 때문입니다.
