# Traduction

Merci de votre aide!

Comment puis-je aider à traduire QOwnNotes?

## Rejoignez-nous sur Crowdin pour traduire QOwnNotes

![crowdin](/img/crowdin.png)

**Crowdin** est une plate-forme de gestion de localisation populaire pour les applications mobiles, le Web, les logiciels de bureau et les ressources associées.

Vous pouvez vous inscrire sur [Crowdin](https://crowdin.com/project/qownnotes/invite) et **traduire** **QOwnNotes** dans votre langue avec des outils simples.

**[QOwnNotes sur Crowdin](https://crowdin.com/project/qownnotes/invite)**

Il suffit de [envoyer un message au propriétaire du projet sur Crowdin](https://crowdin.com/profile/pbek) ou en tant que [problème GitHub](https://github.com/pbek/QOwnNotes/issues) si vous souhaitez traduire QOwnNotes dans une nouvelle langue.

Si vous souhaitez ajouter de nouvelles traductions aux **langues existantes**, vous pouvez le faire immédiatement.

## Relecture

Si vous êtes un excellent locuteur ou même natif d'une langue déjà traduite qui n'a pas encore été relue, nous vous remercions de contribuer à rendre l'expérience QOwnNotes plus fiable et plus pratique en vérifiant et si nécessaire, en corrigeant les traductions non approuvées. Vous devez demander au propriétaire la permission d'être un relecteur pour votre langue.

Cliquez ici pour voir si votre langue fait partie des langues traduites!

[QOwnNotes sur Crowdin](https://translate.qownnotes.org/)

Une fois que vous avez [contacté le propriétaire du projet sur Crowdin](https://crowdin.com/profile/pbek) et obtenu l’autorisation de relire dans votre langue, vous êtes prêt à commencer!

## Conseils généraux

Veuillez prendre en compte quelques éléments lorsque vous travaillez sur le projet.

### Précision

Veuillez localiser avec une précision maximale tout en veillant à la maîtrise de votre langue cible.

### Précision

La cible doit représenter la source exactement et sans omission ni ajout d'aucune partie de la chaîne.

### Le contexte

Si vous avez besoin de plus de contexte ou d'aide avec une certaine chaîne et que vous connaissez une autre langue, vous pouvez changer la langue et voir ce que les autres traducteurs ont fait à partir du segment. Mais n'hésitez pas à laisser un message et à demander des éclaircissements sur le côté droit de la page.

### Termes clés fréquemment utilisés

Il est nécessaire de vérifier et d'utiliser les mêmes traductions pour les termes clés fréquemment utilisés, par exemple, "note" doit être traduit toujours de la même manière.

Si `notes trashed` est `fichiers supprimés` en français, vous le traduisez toujours en utilisant cette expression. L'exception serait lorsque vous êtes grammaticalement obligé d'utiliser une alternative, mais restez simple.

![copie d'écran](/img/crowdin/screenshot-7.png)

Afin de trouver toutes les expressions identiques, recherchez Crowdin pour voir si toutes les occurrences d'un terme ont effectivement la même traduction, si vous connaissez mieux, et ce n'est pas le cas, vous pouvez suggérer une meilleure traduction et voter pour la meilleure/correcte traduction .

Localisez toutes les occurrences de la manière la plus identique possible.

`Remarque` doit toujours avoir une seule traduction.

### Formes du pluriel

Pour le pluriel, veuillez ajouter la traduction appropriée chacun dans le bloc correspondant.

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
