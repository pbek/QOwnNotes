# -*- Mode:Python; indent-tabs-mode:nil; tab-width:4 -*-
#
# Copyright (C) 2016 Canonical Ltd
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""The qmake plugin is useful for building qmake-based parts.

These are projects that are built using .pro files.

This plugin uses the common plugin keywords as well as those for "sources".
For more information check the 'plugins' topic for the former and the
'sources' topic for the latter.

Additionally, this plugin uses the following plugin-specific keywords:

    - options:
      (list of strings)
      additional options to pass to the qmake invocation.
    - qt-version:
      (enum, 'qt4' or 'qt5')
      Version of Qt to use with qmake.
    - project-files:
      (list of strings)
      list of .pro files to pass to the qmake invocation.
"""

import os
import shutil

import snapcraft
from snapcraft import common


class QmakePlugin(snapcraft.BasePlugin):

    @classmethod
    def schema(cls):
        schema = super().schema()
        schema['properties']['options'] = {
            'type': 'array',
            'minitems': 1,
            'uniqueItems': True,
            'items': {
                'type': 'string',
            },
            'default': [],
        }
        schema['properties']['qt-version'] = {
            'enum': ['qt4', 'qt5'],
        }
        schema['properties']['project-files'] = {
            'type': 'array',
            'minitems': 1,
            'uniqueItems': True,
            'items': {
                'type': 'string',
            },
            'default': [],
        }

        # Qt version must be specified
        schema['required'].append('qt-version')

        # Inform Snapcraft of the properties associated with building and
        # pulling so it can mark those steps dirty if they change in the YAML.
        schema['build-properties'].append('options')
        schema['pull-properties'].append('qt-version')

        return schema

    def __init__(self, name, options, project):
        super().__init__(name, options, project)

        self.build_packages.append('make')
        if self.options.qt_version == 'qt5':
            self.build_packages.extend(['qt5-qmake', 'qtbase5-dev'])
        elif self.options.qt_version == 'qt4':
            self.build_packages.extend(['qt4-qmake', 'libqt4-dev'])
        else:
            raise RuntimeError('Unsupported Qt version: {!r}'.format(
                self.options.qt_version))

    def build(self):
        super().build()

        env = self._build_environment()

        sources = []
        if self.options.project_files:
            sources = [os.path.join(self.sourcedir, project_file)
                       for project_file in self.options.project_files]

        self.run(['qmake'] + self._extra_config() + self.options.options +
                 sources, env=env)

        self.run(['make', '-j{}'.format(
            self.project.parallel_build_count)], env=env)

        self.run(['make', 'install', 'INSTALL_ROOT=' + self.installdir],
                 env=env)

    def _extra_config(self):
        extra_config = []

        for root in [self.installdir, self.project.stage_dir]:
            paths = common.get_library_paths(root, self.project.arch_triplet)
            for path in paths:
                extra_config.append("LIBS+=\"-L{}\"".format(path))

            paths = common.get_include_paths(root, self.project.arch_triplet)
            for path in paths:
                extra_config.append("INCLUDEPATH+=\"{}\"".format(path))

        return extra_config

    def _build_environment(self):
        env = os.environ.copy()
        env['QT_SELECT'] = self.options.qt_version

        return env
