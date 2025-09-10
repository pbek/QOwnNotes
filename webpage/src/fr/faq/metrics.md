# Pourquoi des statistiques d'utilisation ?

## En quoi les statistiques d'utilisation sont-elles utiles ?

QOwnNotes (as in the application itself) is **developed by mostly one person in his spare time**.

Les versions sont compilées pour un grand nombre de plate-formes (souvent de très vieilles versions de ces dernières). Les garder à jour représente beaucoup de travail.

Les statistiques d'utilisation sont la seule source fiable d'information concernant les versions des plate-formes et des distributions sur lesquelles QOwnNotes peut être utilisé. Elles montrent également quelques fonctions sont les plus populaires, ceci afin que des décisions puissent être prises concernant les version à retirer, les fonctions à peaufiner ou encore lesquelles sont rarement utilisées.

Le même principe s'applique au choix des langues les plus populaires par les utilisateurs, choix qui encourage l'amélioration des traductions correspondantes.

## Où les statistiques d'utilisation sont-elles stockées ?

Les statistiques d'utilisation sont transmises et stockées sur un serveur [Matomo](https://matomo.org/) auto-hébergé. Seules les deux développeurs principaux ont accès aux données. **Aucunes données ne sont partagées avec des tierces parties.**

## Comment les statistiques d'utilisation sont-elles collectées ?

Les statistiques d'utilisation sont **anonymisées** sur le serveur Matomo après réception (l'adresse IP en est, par exemple, ôtée).

Aucune information personnelle - telle que du texte que vous avez entré ou les noms des notes - ne sont collectées ni stockées sur le serveur !

## Qu'est-ce qui est collecté ?

Sont transmises des informations basiques telles que la résolution de l'écran, le système d'exploitation et la version de QOwnNotes utilisée.

De plus des évènements sont, par exemple, envoyés au serveur quand des actions sont utilisées dans les applications.

QOwnNotes est un logiciel libre open-source. Vous pouvez contrôler les statistiques d'utilisation à n'importe quel moment en recherchant par exemple [sendVisitIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendVisitIfEnabled) et [sendEventIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendEventIfEnabled) dans le code source pour voir ce qui est envoyé.

**Aucune information personnelle - telle que du texte que vous avez entré ou les noms des notes - ne sont collectées ni stockées sur le serveur !**

## Puis-je désactiver la collecte des statistiques d'utilisation ?

Vous pouvez les désactiver au premier démarrage de l'application dans la boîte de dialogue de bienvenue, ou plus tard dans les paramètres.

## Pourquoi les statistiques d’utilisation ne sont-elles pas désactivées par défaut ?

Les réglages par défaut sont rois. Désactiver par défaut les statistiques d'utilisation aurait pour conséquence l'absence de collecte de statistiques utilisables.

La plupart des personnes se fichent des statistiques d'utilisation. Celles qui se préoccupent des statistiques d'utilisation et ne veulent pas voter avec les leurs - concernant ce qui fera l'objet d'un travail ou d'une prise en charge - peuvent facilement les désactiver au premier démarrage de l'application.

Vous verrez même une seconde boîte de dialogue qui vous informera sur les statistiques d’utilisation si vous avez fermé la boîte de dialogue de bienvenue trop tôt.
