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

Due degli usi più semplici sono i collegamenti che puntano a pagine Web o ad altre note. Esistono diversi modi in cui ciascuno di questi collegamenti può apparire.

### Collegamenti esterni

```markdown
[Sono un collegamento in stile in linea](https://www.google.com)

[Sono un collegamento in stile in linea con titolo](https://www.google.com "Home page di Google")

[È possibile utilizzare i numeri per le definizioni dei collegamenti in stile riferimento] [1]

Gli URL semplici e gli URL tra parentesi angolari verranno automaticamente trasformati in collegamenti nell'anteprima. 
http://www.example.com oppure <http://www.example.com>

[1]: https://www.qownnotes.org
```

### Collegamenti interni

```markdown
[Collego la nota per il diario](Diario.md).
<Journal.md> Funziona similmente.
```

::: tip
Puoi utilizzare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + L</kbd> per **creare collegamenti a pagine web o altre note**.

Usando <kbd>Ctrl + Maiusc + X</kbd> verrà visualizzata una finestra di dialogo che ti aiuterà a **inserire allegati** nella nota.

Puoi premere <kbd>Ctrl + Spazio</kbd> mentre il cursore si trova su un collegamento nella modifica della nota per seguire il collegamento.
:::

### Segnalibri

I segnalibri usati dall'estensione del browser [QOwnNotes Web Companion](./browser-extension.md) usano collegamenti in elenchi.

```markdown
- [Nome della pagina web](https://www.example.com) #tag1 #tag2 alcune descrizioni e tag
```

## Immagini

Le immagini possono essere incorporate in QOwnNotes. Verranno mostrate nell'anteprima.

```markdown
![testo alternativo](media/la-mia-immagine.jpg)
```

::: tip
Puoi usare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + Maiusc + I</kbd> per inserire un'immagine in una nota. L'immagine può anche essere negli appunti, la finestra di dialogo la rileverà e mostrerà un'anteprima.

Puoi anche incollare direttamente un'immagine dagli appunti nella tua nota con <kbd>Ctrl + Maiusc + V</kbd>.
:::


## Codice in linea e blocchi di codice

```markdown
Il `codice` in linea è circondato da ``backtick''.
```

::: tip
Puoi utilizzare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + Maiusc + C</kbd> sul testo in linea selezionato o solo all'interno del testo per creare un blocco di codice in linea.
:::

I blocchi di codice sono delimitati da linee con tre apici inversi o sono rientrati con quattro spazi.

### Delimitazioni con 4 spazi

Aggiungi quattro spazi davanti al codice per contrassegnarlo come blocco di codice.

```markdown
    s = "Codice con spazio identato"
    print s
```

### Delimitazioni con apici inversi

Puoi anche usare tre apici inversi per creare un blocco di codice.
~~~markdown
```
Metti qui il tuo codice sorgente
Metti qui il tuo codice sorgente
```
~~~

::: tip
Puoi usare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + Shift + C</kbd> su più linee di testo selezionate o sua una linea vuota per creare un blocco di codice. 
:::

### Blocco con apice inverso con evidenziazione del codice

C'è anche un po' di evidenziazione della sintassi nei blocchi di codice di QOwnNotes.

~~~markdown
```bash
# Questo è un commento
cd Note
```
~~~

Al momento i linguaggi supportati (e gli identificatori dei codici) sono:

* codice BASH, `bash`
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
* codice Shell, `sh`
* SQL, `sql`
* TypeScript, `ts`
* TypeScript, `typescript`
* V, `v`
* Vex, `vex`
* XML, `xml`
* YAML, `yml`
* YAML, `yaml`

## Tabelle

Le tabelle non sono parte integrante di Markdown, ma l'anteprima di QOwnNotes le supporta. 

~~~markdown
I due punti possono essere usati per allineare le colonne.

| Le Tabelle              | Sono               | Belle |
| ----------------------- |:------------------:| -----:|
| la colonna 3 è          | allineata-a-destra | $1600 |
| la colonna 2 è          | centrata           |   $12 |
| Le strisce delle zebre  | sono eccezionali   |    $1 |

Ci devono essere almeno tre trattini per separare ogni intestazione per cella.

Puoi anche usare il Markdown in linea.

| Markdown | Meno | Carino |
| --- | --- | --- |
| *Tuttavia* | `si mostra` | **correttamente** |
| 1 | 2 | 3 |
~~~

::: tip
Premi <kbd>Alt + Maiusc + T</kbd> per attivare una finestra di dialogo che può aiutarti a creare tabelle. Puoi persino importare file CSV in quella finestra di dialogo.

Usa <kbd>Ctrl + Spazio</kbd> all'interno di una tabella di markdown per formattarla automaticamente.
:::

## Citazioni in blocco

```markdown
> Le citazioni in blocco sono davvero utili nelle mail per simulare il testo di risposta.
> Questa linea fa parte della stessa citazione.

Interruzione della citazione.

> Questa è una linea molto lunga che verrà comunque citata correttamente nonostante venga avvolta. Oh mamma mia, continuiamo a scrivere per essere sicuri sia abbastanza lunga per farla avvolgere a chiunque. Ah, inoltre puoi *inserire* **Markdown** in un blocco di citazione. 
```

::: tip
Puoi dire a QOwnNotes di evidenziare completamente le citazioni o solo il carattere della citazione nelle *Impostazioni dell'editor*

Puoi usare la [scorciatoia](./shortcuts.md) <kbd>Ctrl + Maiusc + B</kbd> per segnare il testo come citazione.
:::

## Riga orizzontale

Esistono tre modi per ottenere una regola orizzontale: trattini, asterischi o trattini bassi.

```markdown
Tre o più...

Trattini

---

Asterischi

***

Trattini bassi

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
Questa è una riga con cui possiamo cominciare.

Questa linea è separata dalla precedente con 2 invii.

Anche questa riga inizia un nuovo paragrafo, ma...  
Questa riga è separata solo da 2 spazi e un singolo invio, quindi è una riga separata nello *stesso paragrafo*.
```

::: tip
Gli spazi finali sono evidenziati per impostazione predefinita nell'editor.
:::

## Commenti

I commenti non vengono visualizzati nell'anteprima.

```markdown
[commento]: # (Questo commento non apparirà nell'anteprima)

<!-- Anche i commenti in HTML vengono nascosti -->
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
titolo: Un titolo
descrizione: Qualche descrizione
---

# Il titolo della nota inizia qui

Del testo
```

Il nome del file di questa nota di esempio sarebbe `Il titolo della nota inizia qui.md`.
