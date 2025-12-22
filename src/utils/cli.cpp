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
    std::cout << "  local commands=\"";
    for (const QCommandLineOption& option : options) {
        for (const QString& name : option.names()) {
            std::cout << "--" << name.toStdString() << " ";
        }
    }
    std::cout << "\"" << std::endl;
    std::cout << "  COMPREPLY=( $(compgen -W \"${commands}\" -- ${current}) )" << std::endl;
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
