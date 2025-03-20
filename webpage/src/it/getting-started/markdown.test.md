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
# Alt-H1

## Alt-H2
```

::: tip
Per impostazione predefinita QOwnNotes crea il **nome file di una nota** dall'intestazione **1** (h1).
:::

## Enfasi

```markdown
Enfasi, alias corsivo, con _asterischi_.

Forte enfasi, ovvero grassetto, con **asterischi**.
```

::: tip
Puoi utilizzare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + B</kbd> per rendere il testo in grassetto e <kbd>Ctrl + I</kbd> per renderlo corsivo.
:::

## Sottolineato

Esiste anche un'impostazione opzionale per abilitare il rendering sottolineato nelle _Impostazioni anteprima_.

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

- Sotto-lista non ordinata

1. I numeri usati non sono importanti, basta che sia un numero
1. Sotto-lista ordinata (funziona solo nell'editor, non nell'anteprima)
1. E un altro elemento.

- Le lista non ordinate possono usare gli asterichi

* O i meno

- o i più
```

::: tip
Se si preme <kbd>Invio</kbd> alla fine di un elenco verrà creato un nuovo elemento dell'elenco.
:::

## Collegamenti

Esistono diversi modi per creare collegamenti.

```markdown
[Sono un collegamento in linea](https://www.google.com)

[Sono un collegamento in linea, con titolo](https://www.google.com "Homepage di Google")

[Puoi usare i numeri per i collegamenti a riferimenti][1]

URL e URL tra virgolette basse singole saranno automaticamente trasformati in collegamenti nell'anteprima.
http://www.example.com oppure <http://www.example.com>

[1]: https://www.qownnotes.org
```

::: tip
Puoi utilizzare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + L</kbd> per **creare collegamenti a pagine web o altre note**.

Usando <kbd>Ctrl + Maiusc + X</kbd> verrà visualizzata una finestra di dialogo che ti aiuterà a **inserire allegati** nella nota.

Puoi premere <kbd>Ctrl + Spazio</kbd> mentre il cursore si trova su un collegamento nella modifica della nota per seguire il collegamento.
:::

### Segnalibri

I segnalibri usati dall'estensione del browser [QOwnNotes Web Companion](./browser-extension.md) usano collegamenti in elenchi.

```markdown
- [Nome pagina web](https://www.example.com) #etichetta1 #etichetta2 una descrizione e etichette
```

## Immagini

Le immagini possono essere incorporate in QOwnNotes. Verranno mostrate nell'anteprima.

```markdown
![testo alternativo](media/immagine.jpg)
```

::: tip
Puoi usare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + Maiusc + I</kbd> per inserire un'immagine in una nota. L'immagine può anche essere negli appunti, la finestra di dialogo la rileverà e mostrerà un'anteprima.

Puoi anche incollare direttamente un'immagine dagli appunti nella tua nota con <kbd>Ctrl + Maiusc + V</kbd>.
:::

## Codice in linea e blocchi di codice

```markdown
Il `codice` inline ha `apici inversi` intorno ad esso.
```

::: tip
Puoi utilizzare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + Maiusc + C</kbd> sul testo in linea selezionato o solo all'interno del testo per creare un blocco di codice in linea.
:::

I blocchi di codice sono delimitati da linee con tre apici inversi o sono rientrati con quattro spazi.

### Blocco a 4 spazi

Aggiungi quattro spazi davanti al codice per contrassegnarlo come blocco di codice.

```markdown
    s = "Codice con rientro a spazi"
    print s
```

### Blocco con apice inverso

Puoi anche usare tre apici inversi per creare un blocco di codice.

````markdown
```
Il codice va qui
Il codice va qui
```
````

::: tip
Puoi usare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> su più linee di testo selezionate o sua una linea vuota per creare un blocco di codice.
:::

### Blocco con apice inverso con evidenziazione del codice

C'è anche un po' di evidenziazione della sintassi nei blocchi di codice di QOwnNotes.

````markdown
```bash
# Questo è un commento
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
I due punti possono essere utilizzati per allineare le colonne.

| Tabelle        |        Sono        | Belle |
| -------------- | :----------------: | ----: |
| colonna 3 è    | allineata a destra | $1600 |
| colonna 2 è    |      centrata      |   $12 |
| le zebre hanno |     le strisce     |    $1 |

Ci devono essere almeno 3 trattini per separare le celle di intestazione.

Puoi anche utilizzare Markdown in linea.

| Markdown | Meno   | Carino            |
| -------- | ------ | ----------------- |
| _Ma_     | `reso` | **correttamente** |
| 1        | 2      | 3                 |
```

::: tip
Premi <kbd>Alt + Maiusc + T</kbd> per attivare una finestra di dialogo che può aiutarti a creare tabelle. Puoi persino importare file CSV in quella finestra di dialogo.

Usa <kbd>Ctrl + Spazio</kbd> all'interno di una tabella di markdown per formattarla automaticamente.
:::

## Citazioni in blocco

```markdown
> Le citazioni in blocco sono molto utili nelle e-mail per emulare il testo di risposta.
> Questa riga fa parte della stessa citazione.

Pausa citazione.

> Questa è una linea molto lunga che verrà comunque citata correttamente quando si concluderà. Oh, okay, continuiamo a scrivere per assicurarci che sia abbastanza a lungo da andare a capo davvero per tutti. Oh, puoi _mettere_ **Markdown** in una citazione.
```

````

::: tip
Puoi dire a QOwnNotes di evidenziare completamente le citazioni o solo il carattere della citazione nelle _Impostazioni dell'editor_

Puoi usare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + Maiusc + B</kbd> per segnare il testo come citazione.
:::

## Riga orizzontale

Esistono tre modi per ottenere una regola orizzontale: trattini, asterischi o trattini bassi.

```markdown
Tre o più ...

Trattini

---

Asterischi

---

Trattini bassi

---
```

## Interruzioni di riga

- **Due nuove righe** ti daranno un **nuovo paragrafo**.
- **Una nuova riga** ti darà una **nuova riga nello stesso paragrafo**.
- Per avere una **interruzione di riga senza un paragrafo**, dovrai usare **due spazi finali**.

```markdown
Ecco una riga con cui iniziare.

Questa riga è separata da quella sopra da due nuove righe, quindi sarà un _ paragrafo separato _.

Anche questa riga inizia un paragrafo separato, ma ...
Questa riga è separata solo da due spazi finali e da un singolo ritorno a capo, quindi è una riga separata nello _ stesso paragrafo _.
```

::: tip
Gli spazi finali sono evidenziati per impostazione predefinita nell'editor.
:::

## Commenti

I commenti non vengono visualizzati nell'anteprima.

```markdown
[comment]: # "Questo commento non apparirà nell'anteprima"

<!-- Anche i commenti HTML sono nascosti -->
```

::: tip
Anche un blocco di commenti HTML iniziale in una nota verrà ignorato nella generazione automatica del nome del file della nota.
:::

## Elenchi di caselle di controllo

Puoi creare semplici elenchi di cose da fare con elenchi di caselle di controllo.

```markdown
- [x] fatto
- [ ] da fare
```

::: tip
Puoi selezionare / deselezionare le caselle di controllo nell'anteprima.
:::

## Pagine preliminari

In QOwnNotes puoi usare un frontmatter (es.YAML) per aggiungere alcune meta informazioni extra. **Non verrà mostrato nell'anteprima** e **non disturberà la generazione automatica del nome del file della nota**.

```markdown
---
title: Un qualche titolo
description: Una descrizione
---

# Il titolo della nota inizia qui

Del testo
```

Il nome del file di questa nota di esempio sarebbe `Il titolo della nota inizia qui.md`.
````
