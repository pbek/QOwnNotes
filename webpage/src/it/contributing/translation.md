# Traduzione

Thank you for helping out!

How can I help to translate QOwnNotes?

## Unisciti a noi su Crowdin per tradurre QOwnNotes

![crowdin](/img/crowdin.png)

**Crowdin** è una popolare piattaforma di gestione della localizzazione per app mobili, web, software desktop e risorse correlate.

Puoi iscriverti a [Crowdin](https://crowdin.com/project/qownnotes/invite) e **tradurre** **QOwnNotes** nella tua lingua con strumenti semplici.

**[QOwnNotes su Crowdin](https://crowdin.com/project/qownnotes/invite)**

Devi solo [mandare un messaggio all'autore del progetto su Crowdin](https://crowdin.com/profile/pbek) o aprire un [problema su GitHub](https://github.com/pbek/QOwnNotes/issues) se vuoi tradurre QOwnNotes in una nuova lingua.

If you want to add new translations to **existing languages** you can do so right away.

## Correzione di bozze

If you are an excellent or even native speaker of an already translated language that has not been proofread yet, we are grateful if you help making the QOwnNotes experience more reliable and convenient by checking and if necessary, correcting unapproved translations. You need to ask the owner for the permission to be a proofreader for your language.

Clicca qui per vedere se la tua lingua è tra quelle tradotte!

[QOwnNotes su Crowdin](https://translate.qownnotes.org/)

Dopo aver [contattato il proprietario del progetto su Crowdin](https://crowdin.com/profile/pbek) e ottenuto il permesso di correggere le bozze nella tua lingua, sei pronto per partire!

## General tips

Please consider a few things while working on the project.

### Accuracy

Please localize with maximal possible accuracy while caring for fluency in your target language.

### Precision

The target needs to represent the source exactly and with no omissions or additions of any part of the string.

### Context

If you need more context, or help with a certain string and you happen to know another language, you can change the language and see what other translators made out of the segment. But feel free to leave a message and ask for clarification on the right side of the page.

### Frequently used key terms

It is necessary to check and use the same translations for the frequently used key terms, for example, "note" must be translated always in the same way.

Se `note cestinate` è `file eliminati` in italiano, verrà sempre tradotto utilizzando questa espressione. Ci sarà un'eccezione quando si è grammaticalmente obbligati a usare un'alternativa, ma è opportuno cercare di mantenere semplice il tutto.

![screenshot](/img/crowdin/screenshot-7.png)

In order to find all the identical expressions, search Crowdin to see if all the occurrences of a term indeed have the same translation, if you know better, and they don't you may suggest a better translation and vote down (or up) for the better / correct translation.

Localize all occurrences as identically as you can.

`Nota` dovrebbe chiaramente avere una sola traduzione.

### Plural forms

For plural please add the appropriate translation each in the corresponding block.

![screenshot](/img/crowdin/screenshot-4.png)

### Variables

The `%1` variables need to be written without a space, in RTL languages you can either insert the variables from the original or write them into your translation.

![screenshot](/img/crowdin/screenshot-1.png)

![screenshot](/img/crowdin/screenshot-5.png)

![screenshot](/img/crowdin/screenshot-3.png)

### Access keys

Use `&` in front of any letter that should be used in a shortcut, can be the same as in original but does not have to.

![screenshot](/img/crowdin/screenshot-4.png)

### Custom containers

Strings like `::: tip`, `::: warning` and `::: info` in the documentation are markers for [custom containers](https://vuepress.vuejs.org/guide/markdown.html#custom-containers) in VuePress and shouldn't be translated.

### Other strings not to be translated

Some segments we don't translate in the Documentation:

- Never translate codes or links, tags must be left as they are (and without added spaces, as some MT does that, those must be deleted)
- We don't translate date and time (leave blank)
- And we also don't translate references (which QON got in numerous languages), so you can simply skip them

## Translating and proofreading can be exciting!

You will deal with the language of this software, learn new terms and receive a lot of inside information about the project.

If you are thus ready to help to proofread the translation of your language, [Amy Fabijenna](https://crowdin.com/profile/rawfreeamy) welcomes you to a short briefing in the practice of translating and proofreading. You can ask her anything that helps you have fun contributing.

Thanks a lot for your help! 🙂
