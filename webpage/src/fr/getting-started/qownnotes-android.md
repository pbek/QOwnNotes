# QOwnNotes Android

<div style="display: flex; gap: 16px; margin-bottom: 16px;">
  <img src="/img/qownnotes-android-note-list.webp" alt="QOwnNotes Android note list" style="flex: 1; min-width: 0;" />
  <img src="/img/qownnotes-android-note-view.webp" alt="QOwnNotes Android note view" style="flex: 1; min-width: 0;" />
  <img src="/img/qownnotes-android-note-edit.webp" alt="QOwnNotes Android note edit" style="flex: 1; min-width: 0;" /></div>

[QOwnNotes pour Android](https://github.com/qownnotes/qownnotes-android) est une **application de notes Markdown pouvant fonctionner hors-ligne** pour Android qui se synchronise avec votre serveur Nextcloud à travers l’**API Nextcloud Notes** et l’application **QOwnNotesAPI**.

Elle interagit directement avec **Nextcloud Notes** et **QOwnNotesAPI** sur votre serveur Nextcloud,
vous donnant accès à la synchronisation des notes, à l’historique distant de versions de notes et à la corbeille distante, le tout depuis votre téléphone.

## Fonctionnalités

- **Orienté hors-ligne** : utilisation d’une base de données Room comme source locale de vérité,
  les modifications sont immédiatement mises en caches et sont synchronisées quand le serveur
  est atteignable
- **API Nextcloud Notes 1.2+** avec poussages automatiques, ETags, et mises à jour sans conflits
- Import de l’**authentification unique** (SSO) depuis l’application Android Nextcloud Files
- **Historique de versions de notes** et **exploration de la corbeille distante** via l’application
  Nextcloud optionnelle QOwnNotesAPI
- **Favoris** avec possibilité de les garder hors-ligne et d’effectuer un tri avec les favoris en premier
- **Prévisualisation Markdown** supportant CommonMark, GitHub Flavored Markdown, les listes
  de cases à cocher à trois états (à faire, fait et partiel), les liens-wiki, les tableaux, les blocs de
  code délimités et les images distantes
- **Éditeur Markdown** avec des actions de formatage, annuler/rétablir, préservation de curseur
  et persistance des brouillons locaux
- **Partager pour créer** des notes à partir de texte partagé par les autres applications Android
- **Thèmes clair et sombre** sur Android 9+

## Installation

[![Get it on F-Droid](https://f-droid.org/badge/get-it-on.png)](https://f-droid.org/en/packages/org.qownnotes.mobile/) [![Get it on Obtainium](https://raw.githubusercontent.com/ImranR98/Obtainium/main/assets/graphics/badge_obtainium.png)](https://apps.obtainium.imranr.dev/redirect?r=obtainium://app/%7B%22id%22%3A%22org.qownnotes.mobile%22%2C%22url%22%3A%22https%3A%2F%2Fgithub.com%2Fqownnotes%2Fqownnotes-android%22%2C%22author%22%3A%22qownnotes%22%2C%22name%22%3A%22QOwnNotes%22%7D)

Install QOwnNotes for Android from
[F-Droid](https://f-droid.org/en/packages/org.qownnotes.mobile/), or visit the
[latest release page](https://github.com/qownnotes/qownnotes-android/releases/latest)
and download the APK.

Des versions de développement sont construites et publiées en parallèle des versions stables et peuvent
être installées simultanément (avec deux identifiants d’application différents).

## Configuration de compte Nextcloud

1. Installez l’application Android **Nextcloud Files** et connectez-vous à votre serveur.
2. Ouvrez QOwnNotes pour Android et choisissez **Ajouter un compte Nextcloud**.
3. L’application requiert l’application **Nextcloud Notes** sur votre serveur avec l’API
   Notes 1.2 ou une version ultérieure.
4. Si vous installez aussi l’application Nextcloud **QOwnNotesAPI**, vous obtenez l’accès à
   votre historique de versions de notes et à la corbeille distante.

::: tip
Les identifiants/mots de passe restent dans l’intégration SSO de Nextcloud et ne sont pas copiés dans
la base de données de QOwnNotes.
:::

## Différences principales avec l’application mobile Nextcloud Notes

| Fonctionnalité                                            | QOwnNotes Android | Nextcloud Notes mobile |
| --------------------------------------------------------- | ----------------- | ---------------------- |
| Modification hors-ligne avec détection de conflits        | Oui               | Limité                 |
| Historique de versions de notes via QOwnNotesAPI          | Oui               | Non                    |
| Corbeille distante et restauration                        | Oui               | Non                    |
| Favoris avec option hors-ligne                            | Oui               | Non                    |
| Liste de cases à cocher à trois états                     | Oui               | Non                    |
| Liens-wiki et prise en charge des anciers liens `note://` | Oui               | Non                    |

## Plus d’informations

- [Dépôt GitHub](https://github.com/qownnotes/qownnotes-android)
- [Journal de modifications](https://github.com/qownnotes/qownnotes-android/blob/main/CHANGELOG.md)
