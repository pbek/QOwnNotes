# Markdown

Ez a Markdown-cheatsheet gyors leírása és bemutatása a QOwnNotes markdown szintaxisában.

## Fejlécek

Használja a fejléceket a szövegek strukturálásához.

```markdown
# H1

## H2

### H3

#### H4

##### H5

###### H6
```

::: tip
A **Navigációs panel** mutatja a címsorok felépítését.
:::

Alternatív megoldásként H1 és H2 esetében aláhúzás-stílus stílus:

```markdown
# Alt-H1

## Alt-H2
```

::: tip
Alapértelmezés szerint a QOwnNotes létrehozza a jegyzet **fájlnevét** az **1 fejlécből** (h1).
:::

## Hangsúly

```markdown
Kiemelés, más néven dőlt betű, _csillaggal_.

Erős hangsúly, más néven félkövér, **csillaggal**.
```

::: tip
Használhatja a [parancsikont](./shortcuts.md) <kbd>Ctrl + B</kbd> a félkövér szöveg készítéséhez, és a <kbd>Ctrl + I</kbd> dőlt betűséhez.
:::

## Aláhúzás

Van egy opcionális beállítás is, amely lehetővé teszi az aláhúzás megjelenítését az _Előnézet beállításai_ alatt.

```markdown
_underline_
```

::: tip
A szöveg aláhúzásához használhatja a [parancsikont](./shortcuts.md) <kbd>Ctrl + U</kbd>.
:::

## Kihúz

```markdown
~~strike out~~
```

::: tip
Használhatja az [parancsikon](./shortcuts.md) <kbd>Alt + Shift + S</kbd> a szöveg törléséhez.
:::

## Lists

A listák létrehozásának számos módja van.

```markdown
1. Első rendezett listaelem
2. Egy másik elem

- Rendezetlen allista.

1. A tényleges számok nem számítanak, csak az, hogy ez egy szám
1. Rendezett allista (csak a szerkesztőben működik, az előnézetben nem)
1. És még egy elem.

- A rendezetlen lista csillagokat használhat

* Vagy mínuszok

- Vagy pluszok
```

::: tip
Ha megnyomja az <kbd>Enter</kbd> gombot a lista végén, akkor egy új listaelem jön létre.
:::

## Linkek

A linkek létrehozásának többféle módja van.

```markdown
[Inline stílusú link vagyok] (https://www.google.com)

[Inline stílusú link vagyok címmel] (https://www.google.com "Google kezdőlapja")

[Használhat számokat hivatkozási stílusú hivatkozások meghatározásához] [1]

Az URL-ek és a szögletes zárójelben lévő URL-ek automatikusan linkekké alakulnak az előnézetben.
http://www.example.com or <http://www.example.com>

[1]: https://www.qownnotes.org
```

::: tip
Használhatja a [parancsikont](./shortcuts.md) <kbd>Ctrl + L</kbd> **hozzon létre linkeket weboldalakra vagy egyéb jegyzetekre**.

A <kbd>Ctrl + Shift + X</kbd> használatával megjelenik egy párbeszédpanel, amely segít **csatolni a mellékleteket** a jegyzetbe.

Nyomja meg a <kbd>Ctrl + szóköz</kbd> billentyűt, miközben a kurzor a jegyzet szerkesztésében egy linken van, hogy kövesse a linket.
:::

### Könyvjelzők

A [QOwnNotes Web Companion böngészőbővítmény](./browser-extension.md) által használt könyvjelzők hivatkozásokat használnak a listákban.

```markdown
- [Webpage name] (https://www.example.com) # tag1 # tag2 néhány leírás és címke
```

## Képek

A képek beágyazhatók a QOwnNotes programba. Megjelennek az előnézetben.

```markdown
![alt text](media/my-image.jpg)
```

::: tip
Használhatja a [parancsikont](./shortcuts.md) <kbd>Ctrl + Shift + I</kbd> a kép beszúrásához egy jegyzetbe. A kép a vágólapon is lehet, a párbeszédpanel észleli és előnézetet mutat.

A vágólapról közvetlenül beilleszthet egy képet a jegyzetébe a <kbd>Ctrl + Shift + V</kbd> gombbal.
:::

## Beépített kód és kódblokkok

```markdown
A soron belüli `kód` rendelkezik `back-tics`-el.
```

::: tip
Használhatja a [parancsikont](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> a kiválasztott szöveges szövegen vagy közvetlenül a szöveg belsejében, hogy létrehozzon egy inline kódblokkot.
:::

A kódblokkok vagy három hátsó pipával ellátott vonalakkal vannak elhatárolva, vagy négy szóközzel vannak behúzva.

### 4-Spaces kerítés

Tegyen négy szóközt a kód elé, hogy kódblokkként jelölje meg.

```markdown
    s = "Kód szóközzel"
     nyomtatás s
```

### Backtick kerítés

Három backticket is használhat egy kódblokk létrehozásához.

````markdown
```
A kód ide írandó
A kód ide írandó
```
````

::: tip
Kódblokk létrehozásához használhatja a [shortcut] (./shortcuts.md) <kbd>Ctrl + Shift + C </kbd>elemeket több kijelölt szövegsoron vagy egy üres sorban.
:::

### Backtick kerítés kódkiemeléssel

Van néhány szintaxis kiemelés a kódblokkokkal a QOwnNotes-ban.

````leírás
"bash
# Kommentár vagyok
cd Notes
```
````

Currently supported languages (and code block identifiers) are:

- BASh scripting, `bash`
- C, `c`
- C++, `cpp`
- C++, `cxx`
- C++, `c++`
- C#, `c#`
- CMake, `cmake`
- C#, `csharp`
- CSS, `css`
- Go, `go`
- HTML, `html`
- INI, `ini`
- Java, `java`
- JavaScript, `javascript`
- JavaScript, `js`
- JSON, `json`
- Makefile, `make`
- PHP, `php`
- Python, `py`
- Python, `python`
- QML, `qml`
- Rust, `rust`
- Shell scripting, `sh`
- SQL, `sql`
- TypeScript, `ts`
- TypeScript, `typescript`
- V, `v`
- Vex, `vex`
- XML, `xml`
- YAML, `yml`
- YAML, `yaml`

## Tables

Tables aren't part of the core Markdown spec, but the QOwnNotes preview supports them.

```markdown
A kettőspontok használhatók az oszlopok igazítására.

| Tables        |      Are      |  Cool |
| ------------- | :-----------: | ----: |
| col 3 is      | right-aligned | $1600 |
| col 2 is      |   centered    |   $12 |
| zebra stripes |   are neat    |    $1 |

Legalább 3 kötőjelnek kell elválasztania az egyes fejléccellákat.

Használhatja az inline Markdown funkciót is.

| Markdown | Less      | Pretty     |
| -------- | --------- | ---------- |
| _Still_  | `renders` | **nicely** |
| 1        | 2         | 3          |
```

::: tip
Nyomja meg az <kbd>Alt + Shift + T</kbd> billentyűkombinációt egy párbeszédpanel aktiválásához, amely segíthet táblázatok létrehozásában. Akár CSV fájlokat is importálhat abban a párbeszédpanelen.

Használja a <kbd>Ctrl + szóköz</kbd>t egy jelölési táblázatban az automatikus formázáshoz.
:::

## Idézetek

```markdown
> A blokk idézetek nagyon hasznosak az e-mailben a válaszszöveg utánzásához.
> Ez a sor ugyanannak az idézetnek a része.

Idézet szünet.

> Ez egy nagyon hosszú sor, amelyet még mindig megfelelően idézünk, amikor megtörik. Ó, fiú, írjunk tovább, hogy megbizonyosodjunk arról, hogy ez elég hosszú ahhoz, hogy mindenkinek beburkolhasson. Ó, _beteheti_ a **Markdown**-t egy blokk idézetbe.
```

````

::: tip
Azt mondhatja a QOwnNotes-nak, hogy a _Szerkesztő beállításai_-ban jelölje ki teljesen a blokk idézeteket vagy csak a blokk idézet karaktert.

Használhatja a [parancsikont](./shortcuts.md) <kbd>Ctrl + Shift + B</kbd> a szöveg blokk idézetként történő megjelölésére.
:::

## Vízszintes vonal

Három módon lehet vízszintes szabályt kapni: kötőjel, csillag vagy aláhúzás.

```markdown
Három vagy több ...

Kötőjelek

---

Csillagok

---

Aláhúzások

---
```

## Sortörések

- **Két új volnal** szerez Önnek egy **új bekezdést**.
- **Egy új sor** egy **új sort kap ugyanabban a bekezdésben**.
- **Sortörés bekezdés nélkül**, **két zárójelet kell használnia**.

```markdown
Itt egy sor, amellyel kezdhetjük.

Ezt a sort két új sor választja el a fentiektől, tehát _külön bekezdés_ lesz.

Ez a sor egy külön bekezdést is elkezd, de ...
Ezt a sort csak két zárójel és egyetlen új sor választja el, tehát külön sor az _ugyanabban a bekezdésben_.
```

::: tip
A szóközöket a szerkesztő alapértelmezés szerint kiemeli.
:::

## Hozzászólások

A jegyzet nem jelenik meg az előnézetben.

```markdown
[comment]: # "This comment will not appear in the preview"

<!-- HTML comments are also hidden -->
```

::: tip
A jegyzetek vezető html megjegyzésblokkja az automatikus jegyzetfájl-generálás során sem kerül figyelembe.
:::

## Checkbox lists

Létrehozhat egyszerű todo listákat jelölőnégyzet listákkal.

```markdown
- [x] done
- [ ] todo
```

::: tip
Az előnézetben bejelölheti / eltávolíthatja a jelölőnégyzeteket.
:::

## Frontmatter

A QOwnNotes alkalmazásban frontmatterrel (pl. YAML) adhat további meta-információkat. **nem jelenik meg az előnézetben**, és **nem zavarja az automatikus jegyzetfájlnév előállítását**.

```markdown
---
cím: Valami név
leírás: Néhány leírás
---

# A megjegyzés címsora itt kezdődik

Némi szöveg
```

Ennek a példafájlnak a neve `Note headline starts here.md` lenne.
````
