# Installer en tant qu'AppImage

Vous pouvez télécharger la dernière AppImage à partir de la [page des versions de QOwnNotes](https://github.com/pbek/QOwnNotes/releases). Elle devrait être nommée `QOwnNotes-x86_64.AppImage` sur cette page.

::: tip
Si vous avez [jq](https://stedolan.github.io/jq/) installé, vous pouvez aussi télécharger la dernière AppImage directement :

```bash
# récupérer la version Linux la plus récente depuis l'API QOwnNotes, analyser le JSON pour obtenir son URL et la télécharger
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```

:::

Vous pouvez ensuite modifier les autorisations d'exécution du fichier :

```bash
chmod a+x QOwnNotes-*.AppImage
```

::: tip
Les AppImages ont besoin de `libfuse2` pour s’exécuter ! Par exemple, sous Ubuntu, vous pouvez installer la bibliothèque avec `sudo apt install libfuse2`.
:::

Vous devriez ensuite être en mesure d'exécuter l'AppImage pour lancer QOwnNotes.

::: warning
Si vous souhaitez utiliser la **mise à jour automatique**, assurez-vous de placer votre AppImage à un endroit où votre compte utilisateur a un accès en écriture, par exemple quelque part dans votre répertoire personnel.
:::

::: tip
Si vous rencontrez des difficultés pour exécuter l'AppImage parce que votre version de glibc est trop ancienne vous pouvez essayer l'[AppImage d'OBS](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage), qui devrait avoir été constuite en utilisant glibc 2.16.
:::

## Mode portable

Vous pouvez lancer l’AppImage en **mode portable** en passant l’argument `--portable` :

```bash
./QOwnNotes-x86_64.AppImage --portable
```

En mode portable, toutes les données de l’application sont sauvegardées dans un dossier `Data` à côté du fichier AppImage au lieu de votre répertoire home. Cela inclut :

- Les préférences (`Data/QOwnNotes.ini`)
- La base de données des notes (`Data/QOwnNotes.sqlite`)
- Vos notes (`Data/Notes/`)
- Les scripts, les dictionnaires de vérification orthographique, et d’autres données de l’application

Ainsi, il est très facile de transporter QOwnNotes avec toutes vos notes sur une clé USB ou n’importe quel autre périphérique amovible de stockage.

::: tip
Assurez-vous que le dossier contenant le fichier AppImage est accessible en écriture, afin que le dossier `Data` puisse être créé à côté.
:::

::: tip
Si QOwnNotes affiche `Could not write secret to keychain`, installez les paquets Secret Service manquants sur votre distribution Linux et redémarrez votre session de bureau.

Pour GNOME et les autres environnements reposant sur Secret Service, installez `gnome-keyring`, `libsecret` et `seahorse`.

Pour KDE Plasma, installez un support de KWallet tel que `kwalletmanager`.

QOwnNotes repassera sur le chiffrement obsolète si le porte-clés de l’environnement n’est pas disponible.
:::
