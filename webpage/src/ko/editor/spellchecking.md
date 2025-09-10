# 맞춤법 검사

맞춤법 오류를 수정하세요!

::: 팁 정보 QOwnNotes에는 사전을 다운로드하거나 삭제하는 데 사용할 수 있는 **사전 관리자** (_편집_ 메뉴)가 내장되어 있습니다( 언어가 누락된 경우 사전 추가를 요청하는 방법은 아래를 참조하세요). 사전을 수동으로 다운로드하려면 아래 지침을 따르십시오. :::

- **Linux**에서 distro의 pkg 관리자를 사용하여 해당 언어의 Hunspell 사전을 설치할 수 있습니다
  - 언어 파일을 추가해야 하는 경우 `~/.local/share/hunspell` 또는 `/usr/share/hunspell`에서 추가할 수 있습니다.
  - QOwnNotes의 Snap 패키지를 사용하는 경우, Snap이 `~/.local`에 액세스할 수 없으므로 언어 파일에 대해서도 `~/hunspell`을 사용할 수 있습니다

- **Windows**에서는 Qownnotes 응용 프로그램 폴더 또는 `C:\Users\[username]\dicts`에서 언어에 대한 Hunspell 사전을 폴더 `dicts`에 복사해야합니다.
  - 사전은 [LibreOffice 사전](https://github.com/LibreOffice/dictionaries)에서 다운로드할 수 있습니다
  - **macOS** 에서 해당 언어의 Hunspell 사전을 `/Library/Spelling` 또는 `~/Library/Spelling`로 복사해야 합니다. 디렉터리가 존재하지 않으면 디렉터리를 만들어야 합니다.

사전은 [QOwnNotes 사잔](https://github.com/qownnotes/dictionaries)에서 다운로드할 수 있습니다. 이 문서의 끝에 있는 표를 사용하여 해당 언어와 해당 언어 코드를 찾으십시오.

해당 언어의 사전이 ** 사전 관리자** 또는 위에 언급된 링크에 없는 경우 인터넷에서 찾거나 문제를 만드십시오. 사전이 발견되면 QOwnNotes의 다음 릴리스에 반드시 포함시킬 것입니다.

::: 팁 정보 이 표에서 해당 언어를 찾을 수 없으면 유감스럽게도 LibreOffice 파일에 해당 언어의 사전이 없습니다. 사전을 만들려면 [Documentfoundation.org Dictionaries Development](https://wiki.documentfoundation.org/Development/Dictionaries)으로 이동하십시오

| Sr | 언어 코드    | 언어                 |
| -- | -------- | ------------------ |
| 1  | af_ZA    | 아프리칸스어 (남아프리카 공화국) |
| 2  | an_ES    | 아라곤어 - 스페인 (ES)    |
| 3  | ar       | 아랍어                |
| 4  | be_BY    | 벨라루스어              |
| 5  | bg_BG    | 불가리아어              |
| 6  | bn_BD    | 벵골어                |
| 7  | bo       | 티베트어               |
| 8  | br_FR    | 브르타뉴어 - 프랑스        |
| 9  | bs_BA    | 보스니아어              |
| 10 | ca       | 카탈루냐어              |
| 11 | cs_CZ    | 체코어                |
| 12 | da_DK    | 덴마크어               |
| 13 | de       | 독일어                |
| 14 | el_GR    | 그리스어               |
| 15 | en       | 영어                 |
| 16 | es       | 스페인어               |
| 17 | et_EE    | 에스토니아어             |
| 18 | gd_GB    | 게일어                |
| 19 | gl       | 갈리시아어              |
| 20 | gu_IN    | 구자라트어              |
| 21 | gug      | 과라니어               |
| 22 | he_IL    | 히브리어               |
| 23 | hi_IN    | 힌디어                |
| 24 | hr_HR    | 크로아티아어             |
| 25 | id       | 인도네시아어             |
| 26 | it_IT    | 이탈리아어              |
| 27 | kmr_Latn | 쿠르드어 (터키)          |
| 28 | lo_LA    | 라오어                |
| 29 | lt_LT    | 리투아니아어             |
| 30 | lv_LV    | 라트비아어              |
| 31 | ne_NP    | 네팔어                |
| 32 | nl_NL    | 네덜란드어              |
| 33 | no       | 노르웨이어              |
| 34 | oc_FR    | 옥시탄어               |
| 35 | pl_PL    | 폴란드어               |
| 36 | pt_BR    | 브라질 포르투갈어          |
| 37 | pt_PT    | 포르투갈어              |
| 38 | ro       | 루마니아어              |
| 39 | ru_RU    | 러시아어               |
| 40 | si_LK    | 싱할라어               |
| 41 | sk_SK    | 슬로바키아어             |
| 42 | sl_Sl    | 슬로베니아어             |
| 43 | sq_AL    | 알바니아어              |
| 44 | sr       | 세르비아어              |
| sr | sv_SE    | 스웨덴어               |
| 46 | sw_TZ    | 스와힐리어              |
| 47 | te_IN    | 텔루구어               |
| 48 | th_TH    | 태국어                |
| 49 | tr_TR    | 터키어                |
| 51 | uk_UA    | 우크라이나어             |
| 52 | vi       | 베트남어               |
| 53 | zu_ZA    | 줄루 (맞춤법 사용 불가)     |

::: 팁 크레딧 [Hunspell](https://hunspell.github.io/)이 포함된 [Sonnet](https://github.com/KDE/sonnet)은 맞춤법 검사에 사용됩니다:::
