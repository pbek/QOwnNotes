# Traduction

Thank you for helping out!

How can I help to translate QOwnNotes?

## Rejoignez-nous sur Crowdin pour traduire QOwnNotes

![crowdin](/img/crowdin.png)

**Crowdin** est une plate-forme de gestion de localisation populaire pour les applications mobiles, le Web, les logiciels de bureau et les ressources associées.

Vous pouvez vous inscrire sur [Crowdin](https://crowdin.com/project/qownnotes/invite) et **traduire** **QOwnNotes** dans votre langue avec des outils simples.

**[QOwnNotes sur Crowdin](https://crowdin.com/project/qownnotes/invite)**

Just [drop the project owner a message on Crowdin](https://crowdin.com/profile/pbek) or as a [GitHub issue](https://github.com/pbek/QOwnNotes/issues) if you want to translate QOwnNotes to a new language.

If you want to add new translations to **existing languages** you can do so right away.

## Relecture

If you are an excellent or even native speaker of an already translated language that has not been proofread yet, we are grateful if you help to make the QOwnNotes experience more reliable and convenient by checking and if necessary, correcting unapproved translations. You need to ask the owner for the permission to be a proofreader for your language.

Cliquez ici pour voir si votre langue fait partie des langues traduites!

[QOwnNotes sur Crowdin](https://translate.qownnotes.org/)

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

Si `notes trashed` est `fichiers supprimés` en français, vous le traduisez toujours en utilisant cette expression. L'exception serait lorsque vous êtes grammaticalement obligé d'utiliser une alternative, mais restez simple.

![copie d'écran](/img/crowdin/screenshot-7.png)

In order to find all the identical expressions, search Crowdin to see if all the occurrences of a term indeed have the same translation, if you know better, and they don't you may suggest a better translation and vote down (or up) for the better / correct translation.

Localize all occurrences as identically as you can.

`Remarque` doit toujours avoir une seule traduction.

### Plural forms

For plural please add the appropriate translation each in the corresponding block.

![copie d'écran](/img/crowdin/screenshot-4.png)

### Variables

The `%1` variables need to be written without a space, in RTL languages you can either insert the variables from the original or write them into your translation.

![copie d'écran](/img/crowdin/screenshot-1.png)

![copie d'écran](/img/crowdin/screenshot-5.png)

![copie d'écran](/img/crowdin/screenshot-3.png)

### Access keys

Use `&` in front of any letter that should be used in a shortcut, can be the same as in original but does not have to.

![copie d'écran](/img/crowdin/screenshot-4.png)

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
