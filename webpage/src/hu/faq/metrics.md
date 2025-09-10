# Mire használjuk a mérőszámokat?

## Miért hasznosak a mutatók?

QOwnNotes (as in the application itself) is **developed by mostly one person in his spare time**.

A kiadások sok platformhoz és disztribúcióhoz készültek (gyakran ezek nagyon régi verziói). Fenntartásuk rengeteg munka.

A mérőszámok jelentik az egyetlen megbízható információforrást arról, hogy a QOwnNotes mely platformok és disztribúciók verzióin fut. Azt is megmutatják, hogy mely szolgáltatások a legnépszerűbbek, így eldönthető, hogy mely verziókat kell visszavonni, mely funkciók igényelnek több munkát, és melyeket alig használnak.

Ugyanez vonatkozik a preferált nyelvek felhasználó általi kiválasztására, ami elősegíti a megfelelő fordítások javítását.

## Hol tárolják a mutatókat?

A mérőszámokat a rendszer egy saját üzemeltetésű [Matomo](https://matomo.org/) szerverre továbbítja, és azon tárolja. Csak a két fő fejlesztő fér hozzá az adatokhoz. **Nem osztanak meg adatokat harmadik felekkel.**

## Hogyan történik a mutatók gyűjtése?

A mérőszámokat a Matomo szerver a beérkezésük után **anonimizálja** (például az IP-címet töröljük).

A szerveren nem gyűjtenek vagy tárolnak személyes adatokat, például az Ön által beírt szöveget vagy a megjegyzések nevét!

## Mi kerül összegyűjtésre?

Néhány alapvető információ, például a képernyő felbontása, az operációs rendszer és a QOwnNotes használt verziója továbbítható.

Továbbá például eseményeket küld a szervernek, amikor az alkalmazásokban műveleteket használnak.

A QOwnNotes egy ingyenes, nyílt forráskódú szoftver. A mérőszámokat folyamatosan áttekintheti, ha például a forráskódban a [sendVisitIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendVisitIfEnabled) és a [sendEventIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendEventIfEnabled) kifejezésekre keres, hogy megtudja, pontosan mit küld a rendszer.

**Nem gyűjtünk személyes adatokat, például az Ön által beírt szöveget vagy a megjegyzések nevét!**

## Kikapcsolhatom a mérőszámokat?

Kikapcsolhatja őket az alkalmazás első indításakor az üdvözlő párbeszédpanelen, vagy később a beállítások párbeszédpanelen.

## Miért nincsenek alapértelmezésben kikapcsolva a mérőszámok?

Az alapértelmezések a királyok. A mutatók alapértelmezés szerinti kikapcsolása egyáltalán nem eredményez értelmes mérőszámokat.

A legtöbb embert nem érdeklik a mérőszámok. Azok, akiknek fontosak a mérőszámok, és nem akarnak szavazni a mérőszámaikkal arról, hogy min dolgoznak vagy mi támogatott, az alkalmazás első indításakor könnyen kikapcsolhatják azokat.

Ha túl korán fejezte be az üdvözlő párbeszédpanelt, még egy második párbeszédablakot is kap, amely tájékoztat a mérőszámokról.
