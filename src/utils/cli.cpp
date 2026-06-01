/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#include "cli.h"

#include <QCoreApplication>
#include <QStringList>
#include <iostream>

// Generates Fish shell completion script
void Utils::Cli::generateFishCompletionScript(const QList<QCommandLineOption>& options,
                                              const QString& appName) {
    std::cout << "# Fish shell completion script for " << appName.toStdString() << std::endl;

    for (const QCommandLineOption& option : options) {
        for (const QString& name : option.names()) {
            const bool shortOption = name.length() == 1;
            QString optionPrefix = shortOption ? "-s " : "-l ";
            //            QString cleanedName = name.startsWith("--") ? name.mid(2) : name.mid(1);
            QString cleanedName = name;

            // Check if it's a long option
            if (!shortOption) {
                std::cout << "complete -c " << appName.toStdString() << " "
                          << optionPrefix.toStdString() << cleanedName.toStdString();

                if (!option.description().isEmpty()) {
                    std::cout << " -d \"" << option.description().toStdString() << "\"";
                }

                // Optionally, handle expected value completion
                if (!option.valueName().isEmpty()) {
                    // Handle completion for expected value. This is a placeholder.
                    // Specific completion suggestions require more context.
                    std::cout << " -r";
                }

                std::cout << std::endl;
            }
            // Check if it's a short option
            else {
                std::cout << "complete -c " << appName.toStdString() << " "
                          << optionPrefix.toStdString() << cleanedName.toStdString();

                if (!option.description().isEmpty()) {
                    std::cout << " -d \"" << option.description().toStdString() << "\"";
                }

                std::cout << std::endl;
            }
        }
    }
}

// Generates Bash shell completion script
void Utils::Cli::generateBashCompletionScript(const QList<QCommandLineOption>& options,
                                              const QString& appName) {
    std::cout << "# Bash completion script for " << appName.toStdString() << std::endl;
    std::cout << "_" << appName.toStdString() << "() {" << std::endl;
    std::cout << "  COMPREPLY=()" << std::endl;
    std::cout << "  local current=\"${COMP_WORDS[COMP_CWORD]}\"" << std::endl;
    std::cout << "  local prev=\"${COMP_WORDS[COMP_CWORD-1]}\"" << std::endl;
    std::cout << "  if declare -F _init_completion >/dev/null; then" << std::endl;
    std::cout << "    _init_completion || return" << std::endl;
    std::cout << "  fi" << std::endl;
    std::cout << "  local commands=\"";
    for (const QCommandLineOption& option : options) {
        for (const QString& name : option.names()) {
            const bool shortOption = name.length() == 1;
            std::cout << (shortOption ? "-" : "--") << name.toStdString() << " ";
        }
    }
    std::cout << "\"" << std::endl;

    std::cout << "  case \"$prev\" in" << std::endl;
    for (const QCommandLineOption& option : options) {
        if (option.valueName().isEmpty()) {
            continue;
        }

        QStringList optionNames;
        for (const QString& name : option.names()) {
            const bool shortOption = name.length() == 1;
            optionNames << (shortOption ? QStringLiteral("-") : QStringLiteral("--")) + name;
        }

        std::cout << "    " << optionNames.join(QStringLiteral("|")).toStdString() << ")"
                  << std::endl;

        if (option.names().contains(QStringLiteral("completion"))) {
            std::cout << "      COMPREPLY=( $(compgen -W \"bash fish\" -- \"$current\") )"
                      << std::endl;
        } else if (option.valueName() == QStringLiteral("file")) {
            std::cout << "      compopt -o filenames 2>/dev/null" << std::endl;
            std::cout << "      if declare -F _filedir >/dev/null; then" << std::endl;
            std::cout << "        _filedir" << std::endl;
            std::cout << "      else" << std::endl;
            std::cout << "        COMPREPLY=( $(compgen -f -- \"$current\") )" << std::endl;
            std::cout << "      fi" << std::endl;
        }

        std::cout << "      return 0" << std::endl;
        std::cout << "      ;;" << std::endl;
    }
    std::cout << "  esac" << std::endl;

    std::cout << "  COMPREPLY=( $(compgen -W \"${commands}\" -- \"$current\") )" << std::endl;
    std::cout << "  return 0" << std::endl;
    std::cout << "}" << std::endl;
    std::cout << "complete -F _" << appName.toStdString() << " " << appName.toStdString()
              << std::endl;
}

// Generates Zsh shell completion script
// TODO: Doesn't work yet
void Utils::Cli::generateZshCompletionScript(const QList<QCommandLineOption>& options,
                                             const QString& appName) {
    std::cout << "#compdef " << appName.toStdString() << std::endl << std::endl;

    std::cout << "_arguments -s \\" << std::endl;
    for (int i = 0; i < options.size(); ++i) {
        const QCommandLineOption& option = options.at(i);

        for (const QString& name : option.names()) {
            QString optionPattern;
            if (name.length() == 1) {
                // Short option
                optionPattern = "-" + name;
            } else {
                // Long option
                optionPattern = "--" + name;
            }

            // Check if the option expects a value
            if (!option.valueName().isEmpty()) {
                // Assume the value can be any string, for specifics you might need to adjust
                optionPattern += QStringLiteral("=") + "'[:" + option.description() + ":]'";
            } else {
                optionPattern += "[:" + option.description() + ":]";
            }

            // Handle comma separation for the last option
            if (i != options.size() - 1 || &name != &option.names().back()) {
                optionPattern += " \\";
            }

            std::cout << "  \"" << optionPattern.toStdString() << "\"" << std::endl;
        }
    }

    std::cout << std::endl;
}
