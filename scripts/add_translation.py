#!/usr/bin/python
#
# This script can be used to add a new translation to QOwnNotes
#

import sys


if len(sys.argv) < 3:
    print 'Usage: {} <iso_crowdin> <iso_qon>\ne.g. {} de_AT de'.format(sys.argv[0], sys.argv[0])
    sys.exit(1)

# e.g. "he_IL"
iso_crowdin = sys.argv[1]

# e.g. "he"
iso_qon = sys.argv[2]


def insert_in_file(fileName, text, addText):
    with open(fileName) as f:
        new_text=f.read()
        if text not in new_text:
            print '"{}" was not found in file "{}"!'.format(text, fileName)
            return
        new_text = new_text.replace(text, text + "\n" + addText)

    with open(fileName, "w") as f:
        f.write(new_text)

    return


insert_in_file('crowdin.yaml',
             '"he_IL" : "he",',
             '        "{}" : "{}",'.format(iso_crowdin, iso_qon))
insert_in_file('scripts/download_translations.sh',
             'mv QOwnNotes_he_IL.ts QOwnNotes_he.ts',
             'mv QOwnNotes_{}.ts QOwnNotes_{}.ts'.format(iso_crowdin, iso_qon))
insert_in_file('obs/qownnotes.spec',
             'install -D -m 0644 languages/QOwnNotes_he.qm $RPM_BUILD_ROOT/%{_datadir}/QOwnNotes/languages/QOwnNotes_he.qm',
             'install -D -m 0644 languages/QOwnNotes_{}.qm $RPM_BUILD_ROOT/%{{_datadir}}/QOwnNotes/languages/QOwnNotes_{}.qm'.format(iso_qon, iso_qon))
insert_in_file('obs/qownnotes.spec',
             '%{_datadir}/QOwnNotes/languages/QOwnNotes_he.qm',
             '%{{_datadir}}/QOwnNotes/languages/QOwnNotes_{}.qm'.format(iso_qon))
insert_in_file('src/CMakeLists.txt',
             '    languages/QOwnNotes_he.ts',
             '    languages/QOwnNotes_{}.ts'.format(iso_qon))
insert_in_file('src/QOwnNotes.pro',
             '    languages/QOwnNotes_he.ts \\',
             '    languages/QOwnNotes_{}.ts \\'.format(iso_qon))
insert_in_file('src/debian/qownnotes.install',
             'languages/QOwnNotes_he.qm usr/share/QOwnNotes/languages',
             'languages/QOwnNotes_{}.qm usr/share/QOwnNotes/languages'.format(iso_qon))
insert_in_file('CHANGELOG.md',
             '# QOwnNotes Changelog',
             '\n- added **XXXX** (a big thank you to YYYYY)\n    - join us at [QOwnNotes on Crowdin](https://crowdin.com/project/qownnotes)\n      to make QOwnNotes available in more languages or help with the current\n      translation')

print 'Translation for "{}" was added'.format(iso_qon)
sys.exit(0)
