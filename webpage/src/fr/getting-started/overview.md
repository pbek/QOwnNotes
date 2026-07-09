# Aperçu

<ScreenshotCarousel />

<v-divider />

**Q** pour le [framework Qt](https://www.qt.io/) utilisé   
**Own** parce que vous possédez (« to own ») vos propres notes   
**Notes** pour les notes que vous prenez

<v-divider />

[QOwnNotes](https://www.qownnotes.org/) est une application **open-source** (GPL) de **prise de notes en MarkDown** dans des fichiers en texte brut pour **GNU / Linux**, **MacOS** et **Windows**, qui peut fonctionner (en option) avec [**l'application Notes**](https://github.com/nextcloud/notes) de [Nextcloud](https://nextcloud.com/) et [ownCloud](https://owncloud.org/).

::: tip
Rejoignez notre [Groupe Telegram](https://t.me/QOwnNotes) pour discuter, apprendre et vous connecter avec la communauté QOwnNotes.
:::

[Comprendre le concept de QOwnNotes](concept.md)

## Caractéristiques

- Vous pouvez utiliser vos **fichiers texte ou Markdown existants**, pas besoin de les importer
- QOwnNotes est écrit en C++ et optimisé pour une **faible consommation des ressources** (ce n'est pas une app Electron gourmande en CPU et en mémoire)
- **Import depuis Evernote et Joplin**
- **Compatible** avec l’application [Notes](https://apps.nextcloud.com/apps/notes) de **Nextcloud / ownCloud** et leur application **mobile** ainsi que Nextcloud Text
- Créez **des cartes [Nextcloud Deck](https://apps.nextcloud.com/apps/deck)** et liez-les à vos notes
- Support du **partage de notes** sur votre serveur **ownCloud** / **Nextcloud**
- Gérez vos **listes de tâches** Nextcloud / ownCloud (**Nextcloud tasks** ou **Tasks Plus** / **Calendar Plus**) ou utilisez un autre **Serveur CalDAV** avec lequel synchroniser vos tâches
- Les anciennes **versions** de vos notes peuvent être **restaurées** depuis votre serveur **Nextcloud / ownCloud**
- Les **notes mises à la corbeille** peuvent être **restaurées** depuis votre serveur **Nextcloud / ownCloud**
  - Les modes **corbeille locale** et **corbeille système** sont également disponibles
- Les **modifications externes** des fichiers de notes sont **surveillées** (les notes ou la liste de notes sont rechargées)
- Prise en charge du **balisage hiérarchique des notes** et des **sous-dossiers de notes**
  - Y compris le déplacement et la réorganisation des sous-dossiers avec mise à jour des liens ainsi que la visibilité ou non des sous-dossiers
- **Liens de notes style-wiki** optionnels tels que `[[Note]]` avec complétion automatique, ancres de titres, alias, liens en retour et mises à jour automatiques
- **Chiffrement** optionnel des notes. AES-256 est fourni par défaut, et vous pouvez utiliser des méthodes de chiffrement personnalisées comme **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml)) ou **PGP** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml))
- Le **dossier de notes** peut être **librement choisi** (plusieurs dossiers de notes peuvent être utilisés)
- **La recherche de parties de mots** dans les notes est possible et les résultats de la recherche sont mis en évidence dans les notes
- [**Extension de navigateur**](browser-extension.md) pour **ajouter des notes à partir du texte sélectionné**, **faire des captures d'écran** ou **gérer vos signets**
- [**Vérification orthographique**](../editor/spellchecking.md), et prise en charge optionnelle de [**LanguageTool**](../editor/languagetool.md), [**Harper**](../editor/harper.md) et [**un LSP Markdown**](../editor/markdownlsp.md) (par exemple [Marksman](https://github.com/artempyanykh/marksman) pour les complétions et diagnostics ou [Rumdl](https://github.com/rvben/rumdl) pour le lintage)
- **Mode portable** pour transporter QOwnNotes sur des clés USB
- **Prise en charge des scripts** et [**dépôt de scripts**](https://github.com/qownnotes/scripts) en ligne depuis lequel vous pouvez installer des scripts dans l’application
- **Prise en charge de l’IA** avec intégration via des scripts pour des fournisseurs tels que **OpenAI** et **Groq**
  - Inclut un **serveur MCP** afin que des agents IA externes puissent chercher et récupérer des notes de manière sécurisée
- **Mode Vim**
- **[Application Web](web-app.md)** pour **insérer des photos depuis votre téléphone mobile** dans la note courante de QOwnNotes sur votre ordinateur

## Apparence de l’interface

- **Mise en évidence du Markdown** des notes et un **mode de prévisualisation de Markdown**
  - Inclut des **aperçus d’image en-ligne**, **le repliement des titres Markdown** et le masquage optionnel de la syntaxe de formatage Markdown
- **Prise en charge du thème sombre** avec activation/désactivation en temps réel et des **modes de couleur** personnalisés
- **Prise en charge de la personnalisation des couleurs** pour la **mise en évidence de la syntaxe Markdown**
- Tous les panneaux **peuvent être placés où vous voulez**, ils peuvent même **flotter** ou **s'empiler** (entièrement ancrable)
- Prise en charge des **icônes de thème freedesktop**, vous pouvez utiliser QOwnNotes avec vos **icônes de bureau natives** et avec votre **thème de bureau sombre préféré**
- **Mode sans distractions**, **mode plein écran** et **mode machine à écrire**
- **Numéros de lignes**
- **Sélection par bloc** et autres fonctionnalités pour les utilisateurs avancés
- Prise en charge des **raccourcis claviers globaux sur Wayland**
- **Les différences** entre la note actuelle et la note modifiée depuis l'extérieur sont **affichées** dans une boîte de dialogue

## Langues

- **Disponible dans plus de 60 langues différentes** comme l'anglais, l'allemand, le français, le polonais, le chinois, le japonais, le russe, le portugais, le hongrois, le néerlandais et l'espagnol
  - [Votre aide](../contributing/translation.md) est très appréciée pour améliorer ces traductions ou pour traduire QOwnNotes dans plus de langues

<style>
.v-divider {
  margin: 20px;
}

.sheet {
  position: absolute;
  bottom: 50px;
  background-color: rgba(0,0,0, 0.5);
  color: white;
  text-align: center;
  display: flex;
  align-items:center;
  justify-content:center;
  height: 50px;
  width: 100%;
}

.v-window__next {
  right: 0;
}

@media (max-width: 500px) {
  .v-carousel {
    height: 400px!important;
  }
}

@media (max-width: 350px) {
  .v-carousel {
    height: 250px!important;
  }
}

@media (max-width: 200px) {
  .v-carousel {
    height: 150px!important;
  }
}
</style>
