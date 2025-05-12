#!/usr/bin/python
#
# This script can be used to add a new translation to QOwnNotes
#

import sys
import re

if len(sys.argv) < 3:
    print('Usage: {} <iso_crowdin> <iso_qon>\ne.g. {} de_AT de'.format(
        sys.argv[0], sys.argv[0]))
    sys.exit(1)

# e.g. "he_IL"
iso_crowdin = sys.argv[1]

# e.g. "he"
iso_qon = sys.argv[2]


def insert_in_file(file_name, text, add_text):
    with open(file_name) as f:
        new_text = f.read()
        if text not in new_text:
            print('"{}" was not found in file "{}"!'.format(text, file_name))
            return
        new_text = re.sub(r'^(%s)' % re.escape(text), r'\1\n%s' % add_text,
                          new_text, flags=re.MULTILINE)

    with open(file_name, "w") as f:
        f.write(new_text)

    return


insert_in_file('crowdin.yaml',
               '        "he_IL" : "he",',
               '        "{}" : "{}",'.format(iso_crowdin, iso_qon))
insert_in_file('scripts/download_translations.sh',
               'mv QOwnNotes_he_IL.ts QOwnNotes_he.ts',
               'mv QOwnNotes_{}.ts QOwnNotes_{}.ts'.format(iso_crowdin,
                                                           iso_qon))
insert_in_file('scripts/download_translations.sh',
               '|he|',
               '|{}|'.format(iso_qon))
# insert_in_file('obs/qownnotes.spec',
#              'install -D -m 0644 languages/QOwnNotes_he.qm $RPM_BUILD_ROOT/%{_datadir}/QOwnNotes/languages/QOwnNotes_he.qm',
#              'install -D -m 0644 languages/QOwnNotes_{}.qm $RPM_BUILD_ROOT/%{{_datadir}}/QOwnNotes/languages/QOwnNotes_{}.qm'.format(iso_qon, iso_qon))
# insert_in_file('obs/qownnotes.spec',
#              '%{_datadir}/QOwnNotes/languages/QOwnNotes_he.qm',
#              '%{{_datadir}}/QOwnNotes/languages/QOwnNotes_{}.qm'.format(iso_qon))
insert_in_file('src/CMakeLists.txt',
               '    languages/QOwnNotes_he.ts',
               '    languages/QOwnNotes_{}.ts'.format(iso_qon))
insert_in_file('src/QOwnNotes.pro',
               '    languages/QOwnNotes_he.ts \\',
               r'    languages/QOwnNotes_{}.ts \\'.format(iso_qon))
insert_in_file('src/debian/qownnotes.install',
               'languages/QOwnNotes_he.qm usr/share/qt5/translations',
               'languages/QOwnNotes_{}.qm usr/share/qt5/translations'.format(
                   iso_qon))
insert_in_file('CHANGELOG.md',
               '# QOwnNotes Changelog',
               '\n- added **XXXX translation** (a big thank you to Fabijenna)\n '
               '   - visit [QOwnNotes Translation]('
               'http://docs.qownnotes.org/en/latest/contributing/translation'
               '/)\n      if you want to make QOwnNotes available in more '
               'languages or help with the\n      current translation')

print(
    'Translation for "{}" was added. Don\'t forget to add the translation to '
    'src/dialogs/settingsdialog.ui!'.format(
        iso_qon))
sys.exit(0)
