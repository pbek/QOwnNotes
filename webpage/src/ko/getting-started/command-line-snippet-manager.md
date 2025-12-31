# 명령줄 스니펫 관리자

[QOwnNotes 명령줄 스니펫 관리자](https://github.com/qownnotes/qc)를 사용하여 명령줄에서 QOwnNotes의 **노트에 저장된 명령 코드 조각을 실행**합니다.

![qc](/img/qc.png)

**특수 태그가 있는 노트**를 사용하여 **명령줄 스니펫 관리자에서 실행**할 수 있는 **명령 스니펫**을 저장할 수 있습니다.

![명령](/img/commands.png)

## 설치

[최신 릴리스 페이지](https://github.com/qownnotes/qc/releases/latest)을 방문하여 필요한 버전을 다운로드하십시오.

::: 팁 [jq](https://stedolan.github.io/jq)를 설치한 경우 이 스니펫을 사용하여 최신 Linux AMD64 AppImage를 `/usr/local/bin/qc`에 다운로드하여 설치할 수도 있습니다:

```bash
curl https://api.github.com/repos/qownnotes/qc/releases/latest | \
jq '.assets[] | select(.browser_download_url | endswith("_linux_amd64.tar.gz")) | .browser_download_url' | \
xargs curl -Lo /tmp/qc.tar.gz && \
tar xfz /tmp/qc.tar.gz -C /tmp && \
rm /tmp/qc.tar.gz && \
sudo mv /tmp/qc /usr/local/bin/qc && \
/usr/local/bin/qc --version
```

:::

## 의존성

명령줄에서 명령을 검색하려면 [fzf](https://github.com/junegunn/fzf) (이전 버전) 또는[peco](https://github.com/peco/peco) (기본적으로 설치되는 경우가 더 많음)를 설치해야 합니다.

::: 팁 기본적으로 `fzf`는 검색에 사용되지만 `qc configure`로 설정하여 `peco`를 사용할 수 있습니다. :::

## 설정

![socket-server-token](/img/socket-server-token.png)

스니펫 관리자를 사용하기 전에 QOwnNotes의 _브라우저 확장 / 명령 스니펫 _ (1) 설정에서 _웹 소켓 서버_ (2)를 활성화해야 합니다.

그런 다음 보안 토큰 (3)을 표시하고 복사해야 합니다 (4).

이제 다음을 사용하여 스니펫 관리자의 구성 파일을 엽니다:

```bash
# Configure the snippet manager
qc configure
```

그리고 보안 토큰을 `token` 속성에 넣습니다:

```toml
token = "yourtokenhere"
```

::: 팁 QOwnNotes 설정에서 노트에서 명령어를 검색하는 데 사용할 노트 태그를 설정할 수도 있습니다. 기본적으로 태그 `명령`이 사용됩니다. :::

## 명령 스니펫의 구문

**순서가 지정되지 않은 목록을 인라인 코드 블록 과 함께** 사용하여 명령 코드 조각을 저장할 수 있습니다. 태그 `명령`이 포함된 모든 노트에서 명령 스니펫이 검색됩니다.

인라인 코드 블록 앞에 `cmd:`를 추가하면 노트 태그에 관계없이 **현재 노트**에서도 명령어를 찾을 수 있습니다.

```markdown
- `echo I am a command` I am a description #tag1 #tag2 #tag3

* `echo I am also a command` I am a description #tag3 #tag4 #tag5

- cmd: `echo I will be found in the current note` 이 명령은 노트 태그에 관계없이 현재 노트에서 찾을 수 있습니다

<!-- Example for asking for user input -->

- `read -p "PR ID: " id && git fetch origin pull/$id/head:pull-$id && git checkout pull-$id` Ask for pull request ID and checkout pull request
```

**`bash` 또는 `shell` 코드 블록**은 설명으로 머리글 2 이상 앞에 있으며 명령 스니펫에도 사용할 수 있다. 태그가 제목과 코드 블록 사이에 있는 경우에도 지원됩니다.

    ## "bash" 코드 블록으로 이 작업 수행

    - 이 텍스트는 무시됩니다
    - 그러나 태그는 사용할 수 있습니다: #tag1 #tag2

    ```bash
    echo do this
    echo do that
    ```


    ## "sh" 코드 블록으로 다른 작업 수행

    ```sh
    echo do something else
    echo do something other
    ```

위의 예는 두 개의 명령 스니펫, 즉 두 개의 태그 `tag1` 및 `tag2`가 있는 첫 번째 명령 스니펫을 생성합니다.

## 사용법

```bash
# 명령 스니펫 검색 및 실행
qc exec
```

```bash
# 명령 스니펫 검색 및 인쇄
qc search
```

## 구성

`qc 구성`을 실행합니다.

```toml
  editor = "vim"            # 즐겨찾기 텍스트 편집기
  column = 40               # 목록 명령의 열 크기
  selectcmd = "fzf"         # 편집 명령에 대한 선택기 명령 (fzf 또는 peco)
  sortby = ""               # 스니펫 정렬 방법 지정 (최근 (기본값), -recency, description, -description, command, -command, output, -output)

[QOwnNotes]
  token = "MvTagHXF"        # QOwnNotes API 토큰
  websocket_port = 22222    # QOwnNotes의 웹소켓 포트
```

## 셸 완성

`qc completion <shell>`을 사용하여 셸의 셸 완료 코드를 생성할 수 있습니다.

예를 들어 Fish 셸의 경우 다음을 사용할 수 있습니다:

```bash
qc completion fish > ~/.config/fish/completions/qc.fish
```
