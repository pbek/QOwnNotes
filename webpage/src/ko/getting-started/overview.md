# 개요

<template>
<v-carousel cycle show-arrows-on-hover>
  <v-carousel-item>
    <img src="/screenshots/screenshot.png" alt="QOwnNotes 스크린샷" />
    <div class="sheet">
      마크다운 강조 표시, 색상 태그 및 하위 폴더를 사용하여 노트 편집
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-minimal.png" alt="최소 보기" />
    <div class="sheet">
      더 많이 제거할 수 있는 최소한의 기본 사용자 인터페이스
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-vertical.png" alt="수직 보기" />
    <div class="sheet">
      패널을 이동하여 수직 마크다운 보기에서 노트 보기
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-note-relations.png" alt="노트 관계" />
    <div class="sheet">
      노트 관계 패널
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-portable-mode.png" alt="휴대용 모드" />
    <div class="sheet">
      USB 스틱용 휴대용 모드
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-1col.png" alt="한 열" />
    <div class="sheet">
      모든 패널을 원하는 위치에 배치할 수 있습니다
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-darkmode.png" alt="스크린샷 어두운 모드" />
    <div class="sheet">
      어두운 모드
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-distraction-free-mode.png" alt="스크린샷 방해 금지 모드" />
    <div class="sheet">
      방해 금지 모드
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note-decrypted.png" alt="노트 암호화" />
    <div class="sheet">
      AES 노트 암호화 옵션 (스크립트 작성도 가능)
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note.png" alt="암호화된 노트" />
    <div class="sheet">
      암호화된 노트는 여전히 텍스트입니다
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-diff.png" alt="스크린샷 차이점" />
    <div class="sheet">
      외부에서 변경된 노트의 차이점 표시
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-export-print.png" alt="스크린샷-내보내기-인쇄" />
    <div class="sheet">
      참고 PDF 내보내기 및 인쇄
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-freedesktop-theme.png" alt="스크린샷-프리 데스크톱 테마" />
    <div class="sheet">
      프리데스크톱 테마를 통한 아이콘
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-other-workspace.png" alt="스크린샷-다른 작업 공간" />
    <div class="sheet">
      다양한 작업 공간을 가질 수 있습니다.
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-qml.png" alt="스크린샷-qml" />
    <div class="sheet">
      스크립트 가능
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-russian.png" alt="스크린샷-러시아어" />
    <div class="sheet">
      다양한 언어로 번역
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-all-notes.png" alt="모든 노트에서 스크린샷 검색" />
    <div class="sheet">
      모든 노트 검색
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-current-note.png" alt="현재 노트에서 스크린샷 검색" />
    <div class="sheet">
      현재 노트에서 검색
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-settings-note-folders.png" alt="스크린샷 설정-노트 폴더" />
    <div class="sheet">
      여러 개의 노트 폴더 사용 가능
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-todo.png" alt="스크린샷 - 할 일" />
    <div class="sheet">
      CalDAV를 통해 할 일 목록 관리
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-trash.png" alt="스크린샷-휴지통" />
    <div class="sheet">
      Nextcloud 서버에서 휴지통 메모 관리
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-versioning.png" alt="스크린샷-버전 관리" />
    <div class="sheet">
      Nextcloud 서버에서 노트 버전 관리
    </div>
  </v-carousel-item>
</v-carousel>
</template>

<v-divider />

**Q**가 사용되는 [Qt framework](https://www.qt.io/)  
**Own** 자신의 노트를 소유하고 있으므로 소유   
**Notes** 노트를 위한 노트

<v-divider />

[QOwnNotes](https://www.qownnotes.org/)는 **GNU/리눅스**, **macOS** 및 **Windows**용 응용 프로그램을 사용하는 **오픈 소스** (GPL) **일반 텍스트 파일 마크다운 노트**로, (선택적으로) [Nextcloud](https://nextcloud.com/) 및 [ownCloud](https://owncloud.org/)의 [Notes 응용 프로그램](https://github.com/nextcloud/notes)과 함께 작동합니다.

::: tip Join our [Telegram Group](https://t.me/QOwnNotes) to discuss, learn, and connect with the QOwnNotes community. :::

[QOwnNotes의 개념 이해](concept.md)

## 기능

- 가져오기 없이 **기존 텍스트 또는 마크다운 파일**을 사용할 수 있습니다
- QOwnNotes는 C++로 작성되었으며 **낮은 리소스 소비**에 최적화되어 있습니다 (CPU 및 메모리 사용량이 많은 일렉트론 앱 없음)
- **Evernote 및 Joplin 가져오기**
- **Nextcloud / ownCloud** 및 **모바일**의 [노트](https://apps.nextcloud.com/apps/notes) 응용 프로그램 및 Nextcloud 텍스트와 호환 가능
- **[Nextcloud Deck ](https://apps.nextcloud.com/apps/deck)카드**를 생성하여 노트에 연결
- **ownCloud** / **Nextcloud** 서버에서 **노트 공유** 지원
- Nextcloud / ownCloud **할 일 목록** (**Nextcloud 작업** 또는 **Tasks Plus** / **Calendar Plus**)를 관리하거나 다른 **CalDAV 서버**를 사용하여 작업을 동기화할 수 있습니다
- 이전 **버전** 의 노트를 **Nextcloud / ownCloud**에서 **복원**할 수 있습니다
- **폐기된 노트**는 **Nextcloud / ownCloud** 서버에서 **복원**할 수 있습니다
- 노트 파일의 **외부 변경 사항**이 **감시**됩니다 (노트 또는 노트 목록이 다시 로드됨)
- **계층적 노트 태깅** 및 **노트 하위 폴더** 지원
- 노트의 옵션인 ** 암호화 **가 내장되어 있거나, **[Keybase.io ](https://keybase.io/)**([ encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml)) 또는 ** PGP**([ encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml))와 같은 사용자 지정 암호화 방법을 사용할 수 있습니다
- **노트 폴더**는 **자유롭게 선택**할 수 있습니다 (여러 개의 노트 폴더를 사용할 수 있음)
- 노트에서 **단어의 일부를 검색**할 수 있으며 검색 결과가 노트에서 강조 표시됩니다
- **선택한 텍스트에서 노트 추가**, **스크린샷 만들기** 또는 **북마크를 관리**하는 [**브라우저 확장**](browser-extension.md)
- [**맞춤법 검사**](../editor/spellchecking.md) 지원
- USB 스틱에 QOwnNotes를 휴대하기 위한 **휴대용 모드**
- **스크립트 지원** 및 응용 프로그램 내부에 스크립트를 설치할 수 있는 온라인 [**스크립트 저장소**](https://github.com/qownnotes/scripts)
- 스크립트에 사용할 **[OpenAI 완료 API 구현](https://www.qownnotes.org/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html)**
- **Vim 모드**
- 바탕 화면의 QOwnNotes에서 **휴대폰의 사진을 현재 노트에 삽입**하는 **[웹 응용 프로그램](web-app.md)**

## 외관

- 노트의 **마크다운 강조 표시** 및 **마크다운 미리 보기 모드**
- **어두운 모드 테마 지원**
- **마크다운 구문 강조**에 대한 **디밍 지원**
- 모든 **패널은 원하는 위치에 배치**할 수 있으며, **플로팅** 또는 **스택** (완전 도킹 가능) 도 가능합니다
- **무료 데스크톱 테마 아이콘 **을 지원하므로  **기본 바탕 화면 아이콘 ** 및 즐겨찾는 **어두운 바탕 화면 테마**와 함께 QOnNotes를 사용할 수 있습니다
- **무장애 모드**, **전체 화면 모드** 및 **타이프라이터 모드**
- **줄 번호**
- 현재 노트와 외부적으로 변경된 노트 간의 **차이**가 대화 상자에 **표시**됩니다

## 언어

- 영어, 한국어, 독일어, 프랑스어, 폴란드어, 중국어, 일본어, 러시아어, 포르투갈어, 헝가리어, 네덜란드어 및 스페인어와 같은 **60개 이상의 다양한 언어**로 제공됩니다
  - 이러한 번역을 개선하거나 QOnNotes를 더 많은 언어로 번역할 수 있도록 [도움](../contributing/translation.md)을 주시면 감사하겠습니다

<style>
.sheet {
  position: absolute;
  bottom: 50px;
  background-color: rgba(0,0,0, 0.5);
  color: white;
  text-align: center;
  display: flex;
  align-items:center;
  justify-content:center;
  height: 50px;
  width: 100%;
}

.v-window__next {
  right: 0;
}

@media (max-width: 500px) {
  .v-carousel {
    height: 400px!important;
  }
}

@media (max-width: 350px) {
  .v-carousel {
    height: 250px!important;
  }
}

@media (max-width: 200px) {
  .v-carousel {
    height: 150px!important;
  }
}
</style>
