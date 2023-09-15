# Markdown

Questo bigino Markdown è inteso come un riferimento rapido e una vetrina della sintassi markdown in QOwnNotes.

## Intestazioni

Usa i titoli per strutturare i tuoi testi.

```markdown
# H1
## H2
### H3
#### H4
##### H5
###### H6
```

::: tip
Il **pannello di navigazione** mostra la struttura delle intestazioni.
:::

In alternativa, per H1 e H2, uno stile sottolineato:

```markdown
Alt-H1
======

Alt-H2
------
```

::: tip
Per impostazione predefinita QOwnNotes crea il **nome file di una nota** dall'intestazione **1** (h1).
:::

## Enfasi

```markdown
Enfasi, alias corsivo, con *asterischi*.

Forte enfasi, ovvero grassetto, con **asterischi**.
```

::: tip
Puoi utilizzare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + B</kbd> per rendere il testo in grassetto e <kbd>Ctrl + I</kbd> per renderlo corsivo.
:::

## Sottolineato

Esiste anche un'impostazione opzionale per abilitare il rendering sottolineato nelle *Impostazioni anteprima*.

```markdown
_sottolineato_
```

::: tip
Puoi utilizzare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + U</kbd> per sottolineare un testo.
:::

## Cancellato

```markdown
~~cancellato~~
```

::: tip
Puoi utilizzare la [scorciatoia](./shortcuts.md) <kbd>Alt + Maiusc + S</kbd> per cancellare un testo.
:::

## Liste

Esistono molti modi per creare elenchi.

```markdown
1. Primo elemento di una lista ordinata
2. Un altro elemento
  * Sotto-lista non ordinata
1. I numeri usati non sono importanti, basta che sia un numero
  1. Sotto-lista ordinata (funziona solo nell'editor, non nell'anteprima)
4. E un altro elemento.

* Le lista non ordinate possono usare gli asterichi
- O i meno
+ o i più
```

::: tip
Se si preme <kbd>Invio</kbd> alla fine di un elenco verrà creato un nuovo elemento dell'elenco.
:::

## Collegamenti

Two of the simpler uses links can be put to is in pointing to webpages and other notes. There are multiple ways each of these can be done.

### External links

```markdown
[I'm an inline-style link](https://www.google.com)

[I'm an inline-style link with title](https://www.google.com "Google's Homepage")

[You can use numbers for reference-style link definitions][1]

Plain URLs and URLs in angle brackets will automatically get turned into links in the preview. 
http://www.example.com oppure <http://www.example.com>

[1]: https://www.qownnotes.org
```

### Internal links

```markdown
[I link to the Journal.md note](Journal.md)

<Journal.md> works similarly.
```

::: tip
Puoi utilizzare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + L</kbd> per **creare collegamenti a pagine web o altre note**.

Usando <kbd>Ctrl + Maiusc + X</kbd> verrà visualizzata una finestra di dialogo che ti aiuterà a **inserire allegati** nella nota.

Puoi premere <kbd>Ctrl + Spazio</kbd> mentre il cursore si trova su un collegamento nella modifica della nota per seguire il collegamento.
:::

### Bookmarks

I segnalibri usati dall'estensione del browser [QOwnNotes Web Companion](./browser-extension.md) usano collegamenti in elenchi.

```markdown
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags
```

## Immagini

Le immagini possono essere incorporate in QOwnNotes. Verranno mostrate nell'anteprima.

```markdown
![alt text](media/my-image.jpg)
```

::: tip
Puoi usare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + Maiusc + I</kbd> per inserire un'immagine in una nota. L'immagine può anche essere negli appunti, la finestra di dialogo la rileverà e mostrerà un'anteprima.

Puoi anche incollare direttamente un'immagine dagli appunti nella tua nota con <kbd>Ctrl + Maiusc + V</kbd>.
:::


## Codice in linea e blocchi di codice

```markdown
Inline `code` has `backticks around` it.
```

::: tip
Puoi utilizzare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + Maiusc + C</kbd> sul testo in linea selezionato o solo all'interno del testo per creare un blocco di codice in linea.
:::

I blocchi di codice sono delimitati da linee con tre apici inversi o sono rientrati con quattro spazi.

### 4-Spaces fence

Aggiungi quattro spazi davanti al codice per contrassegnarlo come blocco di codice.

```markdown
    s = "Code with space indent"
    print s
```

### Backtick fence

Puoi anche usare tre apici inversi per creare un blocco di codice.
~~~markdown
```
Code goes here
Code goes here
```
~~~

::: tip
You can use the [shortcut](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> on
multiple selected lines of text or in an empty line to create a code block. 
:::

### Backtick fence with code highlighting

There also is some syntax highlighting with code blocks in QOwnNotes.

~~~markdown
```bash
# I am a comment
cd Notes
```
~~~

Currently, supported languages (and code block identifiers) are:

* BASh scripting, `bash`
* C, `c`
* C++, `cpp`
* C++, `cxx`
* C++, `c++`
* C#, `c#`
* CMake, `cmake`
* C#, `csharp`
* CSS, `css`
* Go, `go`
* HTML, `html`
* INI, `ini`
* Java, `java`
* JavaScript, `javascript`
* JavaScript, `js`
* JSON, `json`
* Makefile, `make`
* PHP, `php`
* Python, `py`
* Python, `python`
* QML, `qml`
* Rust, `rust`
* Shell scripting, `sh`
* SQL, `sql`
* TypeScript, `ts`
* TypeScript, `typescript`
* V, `v`
* Vex, `vex`
* XML, `xml`
* YAML, `yml`
* YAML, `yaml`

## Tables

Tables aren't part of the core Markdown spec, but the QOwnNotes preview supports them. 

~~~markdown
Colons can be used to align columns.

| Tables        | Are           | Cool  |
| ------------- |:-------------:| -----:|
| col 3 is      | right-aligned | $1600 |
| col 2 is      | centered      |   $12 |
| zebra stripes | are neat      |    $1 |

There must be at least 3 dashes separating each header cell.

You can also use inline Markdown.

| Markdown | Less | Pretty |
| --- | --- | --- |
| *Still* | `renders` | **nicely** |
| 1 | 2 | 3 |
~~~

::: tip
Premi <kbd>Alt + Maiusc + T</kbd> per attivare una finestra di dialogo che può aiutarti a creare tabelle. Puoi persino importare file CSV in quella finestra di dialogo.

Usa <kbd>Ctrl + Spazio</kbd> all'interno di una tabella di markdown per formattarla automaticamente.
:::

## Citazioni in blocco

```markdown
> Blockquotes are very handy in email to emulate reply text.
> This line is part of the same quote.

Quote break.

> This is a very long line that will still be quoted properly when it wraps. Oh boy let's keep writing to make sure this is long enough to actually wrap for everyone. Oh, you can *put* **Markdown** into a blockquote. 
```

::: tip
Puoi dire a QOwnNotes di evidenziare completamente le citazioni o solo il carattere della citazione nelle *Impostazioni dell'editor*

Puoi usare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + Maiusc + B</kbd> per segnare il testo come citazione.
:::

## Riga orizzontale

Esistono tre modi per ottenere una regola orizzontale: trattini, asterischi o trattini bassi.

```markdown
Three or more...

Hyphens

---

Asterisks

***

Underscores

___
```

## Interruzioni di riga

- Puoi dividere un paragrafo in più linee per modificarlo più facilmente, saranno comunque rese come un singolo paragrafo senza interruzioni.
- Puoi forzare un'interruzione di linea all'interno di un paragrafo terminando la linea con due spazi.
- Puoi creare un paragrafo separato delimintandolo con linee vuote.

::: tip
Puoi inserire due spazi e un a capo con <kbd>⇧ Shift</kbd> + <kbd>Invio</kbd>.
:::

```markdown
Here's a line for us to start with.

This line is separated from the one above by two newlines, so it will be a *separate paragraph*.

This line is also begins a separate paragraph, but...  
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the *same paragraph*.
```

::: tip
Gli spazi finali sono evidenziati per impostazione predefinita nell'editor.
:::

## Commenti

I commenti non vengono visualizzati nell'anteprima.

```markdown
[comment]: # (This comment will not appear in the preview)

<!-- HTML comments are also hidden -->
```

::: tip
Anche un blocco di commenti HTML iniziale in una nota verrà ignorato nella generazione automatica del nome del file della nota.
:::

## Elenchi di caselle di controllo

Puoi creare semplici elenchi di cose da fare con elenchi di caselle di controllo.

```markdown
- [x] done
- [ ] todo
```

::: tip
Puoi selezionare / deselezionare le caselle di controllo nell'anteprima.
:::

## Pagine preliminari

In QOwnNotes puoi usare un frontmatter (es.YAML) per aggiungere alcune meta informazioni extra. **Non verrà mostrato nell'anteprima** e **non disturberà la generazione automatica del nome del file della nota**.

```markdown
---
title: Some name
description: Some description
---

# Note headline starts here

Some text
```

Il nome del file di questa nota di esempio sarebbe `Il titolo della nota inizia qui.md`.
