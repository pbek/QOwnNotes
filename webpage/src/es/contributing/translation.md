# Traducción

Thank you for helping out!

How can I help to translate QOwnNotes?

## Únase a nosotros en Crowdin para traducir QOwnNotes

![crowdin](/img/crowdin.png)

**Crowdin** es una popular plataforma de gestión de localización para aplicaciones móviles, web, software de escritorio y activos relacionados.

Puede registrarse en [Crowdin](https://crowdin.com/project/qownnotes/invite) y **traducir** **QOwnNotes** a su idioma con herramientas sencillas.

**[QOwnNotes en Crowdin](https://crowdin.com/project/qownnotes/invite)**

Just [drop the project owner a message on Crowdin](https://crowdin.com/profile/pbek) or as a [GitHub issue](https://github.com/pbek/QOwnNotes/issues) if you want to translate QOwnNotes to a new language.

If you want to add new translations to **existing languages** you can do so right away.

## Corrección de pruebas

If you are an excellent or even native speaker of an already translated language that has not been proofread yet, we are grateful if you help to make the QOwnNotes experience more reliable and convenient by checking and if necessary, correcting unapproved translations. You need to ask the owner for the permission to be a proofreader for your language.

Haga clic aquí para ver si su idioma está entre los traducidos.

[QOwnNotes en Crowdin](https://translate.qownnotes.org/)

Once you’ve [contacted the project owner on Crowdin](https://crowdin.com/profile/pbek) and received permission to proofread in your language, you’re ready to go!

## General tips

Please consider a few things while working on the project.

### Accuracy

Please localize with maximal possible accuracy while caring for fluency in your target language.

### Precision

The target needs to represent the source exactly and with no omissions or additions of any part of the string.

### Context

If you need more context, or help with a certain string, and you happen to know another language, you can change the language and see what other translators made out of the segment. But feel free to leave a message and ask for clarification on the right side of the page.

### Frequently used key terms

It is necessary to check and use the same translations for the frequently used key terms, for example, "note" must be translated always in the same way.

Si `notas en la papelera` es `gelöschte Dateien` en alemán, siempre lo traducirías usando esta expresión. La excepción sería cuando gramaticalmente esté obligado a usar una alternativa, pero manténgalo simple.

![captura de pantalla](/img/crowdin/screenshot-7.png)

In order to find all the identical expressions, search Crowdin to see if all the occurrences of a term indeed have the same translation, if you know better, and they don't you may suggest a better translation and vote down (or up) for the better / correct translation.

Localize all occurrences as identically as you can.

`Nota` definitivamente siempre debería tener una sola traducción.

### Plural forms

For plural please add the appropriate translation each in the corresponding block.

![captura de pantalla](/img/crowdin/screenshot-4.png)

### Variables

The `%1` variables need to be written without a space, in RTL languages you can either insert the variables from the original or write them into your translation.

![captura de pantalla](/img/crowdin/screenshot-1.png)

![captura de pantalla](/img/crowdin/screenshot-5.png)

![captura de pantalla](/img/crowdin/screenshot-3.png)

### Access keys

Use `&` in front of any letter that should be used in a shortcut, can be the same as in original but does not have to.

![captura de pantalla](/img/crowdin/screenshot-4.png)

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
