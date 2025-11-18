# Fordítás

Köszönjük a segítségét!

Hogyan segíthetek a QOwnNotes fordításában?

::: warning
Please don't attempt to edit the translation files in `src/languages` directly. They are all generated automatically via [Crowdin](https://crowdin.com/project/qownnotes).
:::

## Csatlakozzon hozzánk Crowdin webalkalmazáson a QOwnNotes fordításához

![crowdin](/img/crowdin.png)

A **Crowdin** egy népszerű Lokalizációkezelési felületen a mobilalkalmazásokhoz, webes, asztali szoftverekhez és kapcsolódó eszközökhez.

You can sign up at [Crowdin](https://crowdin.com/project/qownnotes) and **translate** **QOwnNotes** to your language with easy tools.

**[QOwnNotes a Crowdinról](https://crowdin.com/project/qownnotes)**

Csak [küldjön üzenetet a projekt tulajdonosának a Crowdin](https://crowdin.com/profile/pbek) on vagy [GitHub kérdésként](https://github.com/pbek/QOwnNotes/issues), ha új nyelvre szeretné fordítani a QOwnNotes programot.

Ha fordítást szeretne hozzáadni **meglévő fordításokhoz**, a fenti linken keresztül azonnal megteheti.

## Lektorálás

Ha Ön egy már lefordított, még le nem olvasott nyelv kiváló vagy akár anyanyelvi beszélője, hálásan köszönjük, ha a nem jóváhagyott fordítások ellenőrzésével és szükség esetén javításával segíti a QOwnNotes élményét megbízhatóbbá és kényelmesebbé tenni. Kérjen engedélyt a tulajdonostól, hogy lektorálhasson a saját nyelvén.

Kattintson ide, hogy lássa, az Ön nyelve a lefordítottak között van-e!

[QOwnNotes a Crowdin-on](https://translate.qownnotes.org/)

Miután [felvette a kapcsolatot a projekt tulajdonosával a Crowdinon](https://crowdin.com/profile/pbek), és engedélyt kapott az Ön nyelvén történő lektorálásra, készen áll!

## Általános tippek

Kérjük, vegyen figyelembe néhány szükséges feltételt a projekt során.

### Pontosság

Kérjük, lokalizálja a lehető legnagyobb pontossággal, miközben ügyel a célnyelv folyékony használatára.

### Precizitás

A célnak pontosan kell képviselnie a forrást, a karakterlánc bármely részének kihagyása vagy kiegészítése nélkül.

### Kontextus

Ha több kontextusra vagy segítségre van szüksége egy bizonyos karakterlánchoz, és történetesen tud egy további nyelven is, megváltoztathatja a nyelvet, és megnézheti, hogy más fordítók mit hoztak létre a szegmensből. De nyugodtan hagyjon üzenetet, és kérjen pontosítást az oldal jobb oldalán.

### Gyakran használt kulcsfogalmak

A gyakran használt kulcsfogalmaknál ellenőrizni és ugyanazokat a fordításokat kell használni, például a "jegyzet" kifejezést mindig azonosan szükséges fordítani.

Ha a `trashed notes` magyarul `törölt fájlok`, akkor mindig így fordítsa! Kivétel, ha nyelvtanilag kötelezvén használ egy alternatívát.

![screenshot](/img/crowdin/screenshot-7.png)

In order to find all the identical expressions, search Crowdin to see if all the occurrences of a term indeed have the same translation. You may suggest a better translation and vote on the others.

Lokalizálja a kifejezések összes előfordulását olyan egyformán, amennyire csak lehet.

A `jegyzet` szó mindig, feltétlenül csak egy azonos fordítást eredményezzen.

### Többes számú alakok

Többes szám esetén adja hozzá a megfelelő fordítást a megfelelő blokkhoz.

![screenshot](/img/crowdin/screenshot-4.png)

### Változók

A `%1` változókat szóköz nélkül kell írni, az RTL nyelveken vagy beillesztheti a változókat az eredetiből, vagy beírhatja a fordításba.

![screenshot](/img/crowdin/screenshot-1.png)

![screenshot](/img/crowdin/screenshot-5.png)

![screenshot](/img/crowdin/screenshot-3.png)

### Hozzáférési kulcsok

Használja a `&` jelet minden olyan betű előtt, amelyet a parancsikonban kell használni. Ugyanaz lehet, mint az eredetiben, de nem kötelező.

![screenshot](/img/crowdin/screenshot-4.png)

### Conteneurs personnalisés

Az olyan karakterláncok, mint az `::: tip`, `::: warning` and `::: info` a dokumentációban, az [custom containers](https://vuepress.vuejs.org/guide/markdown.html#custom-containers) jelölői a VuePressben, és nem szabad lefordítani őket.

### Más karakterláncok, amelyeket nem kell lefordítani

Néhány szegmens, amelyet nem fordítunk le a Dokumentációban:

- Soha ne fordítson le kódokat vagy linkeket, a címkéket úgy kell hagyni, ahogy vannak (és szóközök nélkül, mivel egyes gépi fordítások ezt teszik, ezeket törölni kell)
- Nem fordítjuk le a dátumot és az időt (hagyja üresen)
- És nem fordítunk hivatkozásokat (amit a QOwnNotes számos fordítási nyelven kapott), így egyszerűen kihagyhatja őket

## A fordítás és a lektorálás izgalmas lehet!

Foglalkozni fog ennek a szoftvernek a nyelvével, új kifejezéseket tanul, és sok bennfentes információt kap a projektről.

Ha így készen áll a nyelve fordításának lektorálásában, lépjen kapcsolatba a [Patrizio](https://crowdin.com/profile/pbek)-val, és kérdezze meg, ha bármilyen kérdése van, és/vagy szerezze meg az anyanyelvén végzett lektorálási jogokat.

Nagyon köszönjük a segítségét! 🙂
