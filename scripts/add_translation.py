#!/usr/bin/python
#
# This script can be used to add a new translation to QOwnNotes
#

import sys


if len(sys.argv) < 3:
    print 'Usage: {} <isoCrowdin> <isoQON>\ne.g. {} de_AT de'.format(sys.argv[0], sys.argv[0])
    sys.exit(1)

# e.g. "he_IL"
isoCrowdin = sys.argv[1]

# e.g. "he"
isoQON = sys.argv[2]


def insertInFile( fileName, text, addText ):
    with open(fileName) as f:
        newText=f.read()
        if text not in newText:
            print '"{}" was not found in file "{}"!'.format(text, fileName)
            return
        newText = newText.replace(text, text + "\n" + addText)

    with open(fileName, "w") as f:
        f.write(newText) 

    return


insertInFile('crowdin.yaml',
             '"he_IL" : "he",',
             '        "{}" : "{}",'.format(isoCrowdin, isoQON))
insertInFile('scripts/download_translations.sh',
             'mv QOwnNotes_he_IL.ts QOwnNotes_he.ts',
             'mv QOwnNotes_{}.ts QOwnNotes_{}.ts'.format(isoCrowdin, isoQON))
insertInFile('obs/qownnotes.spec',
             'install -D -m 0644 languages/QOwnNotes_he.qm $RPM_BUILD_ROOT/%{_datadir}/QOwnNotes/languages/QOwnNotes_he.qm',
             'install -D -m 0644 languages/QOwnNotes_{}.qm $RPM_BUILD_ROOT/%{{_datadir}}/QOwnNotes/languages/QOwnNotes_{}.qm'.format(isoQON, isoQON))
insertInFile('obs/qownnotes.spec',
             '%{_datadir}/QOwnNotes/languages/QOwnNotes_he.qm',
             '%{{_datadir}}/QOwnNotes/languages/QOwnNotes_{}.qm'.format(isoQON))
insertInFile('src/CMakeLists.txt',
             '    languages/QOwnNotes_he.ts',
             '    languages/QOwnNotes_{}.ts'.format(isoQON))
insertInFile('src/QOwnNotes.pro',
             '    languages/QOwnNotes_he.ts \\',
             '    languages/QOwnNotes_{}.ts \\'.format(isoQON))
insertInFile('src/debian/qownnotes.install',
             'languages/QOwnNotes_he.qm usr/share/QOwnNotes/languages',
             'languages/QOwnNotes_{}.qm usr/share/QOwnNotes/languages'.format(isoQON))
insertInFile('CHANGELOG.md',
             '# QOwnNotes Changelog',
             '\n- added **XXXX** (a big thank you to YYYYY)\n    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)\n      to make QOwnNotes available in more languages or help with the current\n      translation')

print 'Translation for "{}" was added'.format(isoQON)
sys.exit(0)
