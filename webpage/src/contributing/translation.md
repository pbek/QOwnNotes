# Translation

Thank you for helping out!

How can I help to translate QOwnNotes?

## Join us on Crowdin to translate QOwnNotes

![crowdin](/img/crowdin.png)

**Crowdin** is a popular Localization Management Platform for mobile apps, web,
desktop software and related assets.

You can sign up at [Crowdin](https://crowdin.com/project/qownnotes) and
**translate** **QOwnNotes** to your language with easy tools.

**[QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)**

Just [drop the project owner a message on Crowdin](https://crowdin.com/profile/pbek)
or as a [GitHub issue](https://github.com/pbek/QOwnNotes/issues) if you want to
translate QOwnNotes to a new language.

If you want to add new translations to **existing languages** you can do so right away.

## Proofreading

If you are an excellent or even native speaker of an already translated language
that has not been proofread yet, we are grateful if you help to make the QOwnNotes
experience more reliable and convenient by checking and if necessary, correcting
unapproved translations. You need to ask the owner for the permission to be a
proofreader for your language.

Click here to see if your language is among the translated ones!

[QOwnNotes on Crowdin](https://translate.qownnotes.org/)

Once you’ve [contacted the project owner on Crowdin](https://crowdin.com/profile/pbek)
and received permission to proofread in your language, you’re ready to go!

## General tips

Please consider a few things while working on the project.

### Accuracy

Please localize with maximal possible accuracy while caring for fluency in your target language.

### Precision

The target needs to represent the source exactly and with no omissions or additions of any part of the string.

### Context

If you need more context, or help with a certain string, and you happen to know another language,
you can change the language and see what other translators made out of the segment.
But feel free to leave a message and ask for clarification on the right side of the page.

### Frequently used key terms

It is necessary to check and use the same translations for the frequently used key terms,
for example, "note" must be translated always in the same way.

If `trashed notes` is `gelöschte Dateien` in German, you would always translate it using this expression.
Exception would be when you grammatically are bound to use an alternative, but keep it simple.

![screenshot](/img/crowdin/screenshot-7.png)

In order to find all the identical expressions, search Crowdin to see if all the occurrences
of a term indeed have the same translation, if you know better, and they don't you may
suggest a better translation and vote down (or up) for the better / correct translation.

Localize all occurrences as identically as you can.

`Note` should definitely always have just one translation.

### Plural forms

For plural please add the appropriate translation each in the corresponding block.

![screenshot](/img/crowdin/screenshot-4.png)

### Variables

The `%1` variables need to be written without a space, in RTL languages you can either
insert the variables from the original or write them into your translation.

![screenshot](/img/crowdin/screenshot-1.png)

![screenshot](/img/crowdin/screenshot-5.png)

![screenshot](/img/crowdin/screenshot-3.png)

### Access keys

Use `&` in front of any letter that should be used in a shortcut, can be the same
as in original but does not have to.

![screenshot](/img/crowdin/screenshot-4.png)

### Custom containers

Strings like `::: tip`, `::: warning` and `::: info` in the documentation are markers for
[custom containers](https://vuepress.vuejs.org/guide/markdown.html#custom-containers) in VuePress
and shouldn't be translated.

### Other strings not to be translated

Some segments we don't translate in the Documentation:

- Never translate codes or links, tags must be left as they are
  (and without added spaces, as some machine translation does that, those must be deleted)
- We don't translate date and time (leave blank)
- And we also don't translate references (which QOwnNotes got in numerous
  translation languages), so you can simply skip them

## Translating and proofreading can be exciting!

You will deal with the language of this software, learn new terms and receive
a lot of inside information about the project.

If you are thus ready to help to proofread the translation of your language, contact
[Patrizio](https://crowdin.com/profile/pbek) and ask away if you have any questions and/or obtain the rights for proofreading in your native language.

Thanks a lot for your help! 🙂
