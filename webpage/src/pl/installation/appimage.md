# Instalacja jako AppImage

Możesz pobrać najnowszą aplikację AppImage ze [strony wydań QOwnNotes](https://github.com/pbek/QOwnNotes/releases). Powinien mieć na tej stronie nazwę jak `QOwnNotes-x86_64.AppImage`.

::: tip
Jeśli masz zainstalowane [jq](https://stedolan.github.io/jq/), możesz również pobrać najnowszy AppImage bezpośrednio:

```bash
# query the latest Linux release from the QOwnNotes API, parse the JSON for the URL and download it
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```
:::

Następnie możesz zmienić uprawnienia uruchomienia pliku:

```bash
chmod a+x QOwnNotes-*.AppImage
```

Dzięki temu będziesz mógł uruchomić AppImage, aby otworzyć QOwnNotes.

::: warning
Jeśli chcesz użyć **automatycznego aktualizatora**, upewnij się, że umieściłeś swój obraz aplikacji w miejscu, w którym Twoje konto użytkownika ma dostęp do zapisu, na przykład gdzieś w katalogu domowym.
:::

::: tip
Jeśli masz problemy z uruchomieniem AppImage, ponieważ twoja wersja glibc jest za stara możesz wypróbować [AppImage zbudowany na OBS](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage), powinien być zbudowany z glibc 2.16.
:::
