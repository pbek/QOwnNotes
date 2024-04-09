/*
 * Copyright (c) 2014-2024 Patrizio Bekerle -- <patrizio@bekerle.com>
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
void Utils::Cli::generateFishCompletionScript(const QList<QCommandLineOption>& options, const QString& appName) {
//    std::cout << "# Fish shell completion script for " << appName.toStdString() << std::endl;
//    std::cout << "complete -c " << appName.toStdString() << " -f";
//    for (const QCommandLineOption& option : options) {
//        QString namesConcatenated;
//        for (const QString& name : option.names()) {
//            namesConcatenated += QString("-") + QString(name.length() == 1 ? " " : "-") + name + " ";
//        }
//        std::cout << " -a \"" << namesConcatenated.trimmed().toStdString() << "\"";
//        if (!option.description().isEmpty()) {
//            std::cout << " -d \"" << option.description().toStdString() << "\"";
//        }
//    }
//    std::cout << std::endl;

//    // Introduction of the function in Fish script - replace {appName} with your actual app name
//    std::cout << "function __fish_" << appName.toStdString() << "_complete" << std::endl;
//    std::cout << "    complete -c " << appName.toStdString() << " -f -n \"__fish_use_subcommand\"";
//
//    // Extract options and generate completion entries
//    for (const QCommandLineOption &option : options) {
//        // For simplicity, only using the first name of the option as Fish option
//        QString fishOption = option.names().first();
//        QString description = option.description().replace("\"", "\\\"");
//
//        // Adjust if it's a short or long option
//        if (fishOption.length() > 1) {
//            fishOption = "--" + fishOption;  // long option
//        } else {
//            fishOption = "-" + fishOption;   // short option
//        }
//
//        std::cout << " -a \"" << fishOption.toStdString() << "\" -d \"" << description.toStdString() << "\"";
//    }
//
//    // Ending the Fish completion function
//    std::cout << std::endl << "end" << std::endl;

//    // Register the completion function for the application command
//    std::cout << "complete -c " << appName.toStdString() << " -f -a \"(__fish_" << appName.toStdString() << "_complete)\"" << std::endl;
//
//    std::cout << "# Fish shell completion script for " << appName.toStdString() << std::endl;
//    std::cout << "function __fish_" << appName.toStdString() << "_complete" << std::endl;
//    std::cout << "    for i in (commandline -opc) (commandline -ct)" << std::endl;
//    std::cout << "        switch $i" << std::endl;
//
//    for (const QCommandLineOption& option : options) {
//        for (const QString& name : option.names()) {
//            // Outputs both short and long options
//            std::string optionName = name.length() > 1 ? "--" + name.toStdString() : "-" + name.toStdString();
//            std::cout << "        case '" << optionName << "'" << std::endl;
//        }
//    }
//
//    std::cout << "            return 0" << std::endl;
//    std::cout << "        end" << std::endl;
//    std::cout << "    end" << std::endl;
//
//    // Outputs the actual completion options
//    std::cout << "    complete -c " << appName.toStdString() << " -f -n '__fish_seen_subcommand_from ;' -a '";
//    for (const QCommandLineOption& option : options) {
//        for (const QString& name : option.names()) {
//            std::string optionName = name.length() > 1 ? "--" + name.toStdString() : "-" + name.toStdString();
//            std::cout << optionName << " ";
//        }
//    }
//    std::cout << "'" << std::endl;
//
//    std::cout << "end" << std::endl;

    std::cout << "# Fish shell completion script for " << appName.toStdString() << std::endl;

    for (const QCommandLineOption& option : options) {
        // Handle long options
        for (const QString& longName : option.names().filter(QRegExp("^[^-].*"))) {
            std::cout << "complete -c " << appName.toStdString()
                      << " -l " << longName.toStdString();
            if (!option.description().isEmpty()) {
                std::cout << " -d \"" << option.description().toStdString() << "\"";
            }
            if (!option.valueName().isEmpty()) {
                std::cout << " -a \"(" << appName.toStdString() << " " << longName.toStdString() << ")\"";
            }
            std::cout << std::endl;
        }

        // Handle short options
        for (const QString& shortName : option.names().filter(QRegExp("^-.$"))) {
            std::cout << "complete -c " << appName.toStdString()
                      << " -s " << shortName.toStdString().substr(1);  // Remove the '-' prefix
            if (!option.description().isEmpty()) {
                std::cout << " -d \"" << option.description().toStdString() << "\"";
            }
            std::cout << std::endl;
        }
    }
}

// Generates Bash shell completion script
void Utils::Cli::generateBashCompletionScript(const QList<QCommandLineOption>& options, const QString& appName) {
    std::cout << "# Bash completion script for " << appName.toStdString() << std::endl;
    std::cout << "_"<< appName.toStdString() << "() {" << std::endl;
    std::cout << "  COMPREPLY=()" << std::endl;
    std::cout << "  local current=\"${COMP_WORDS[COMP_CWORD]}\"" << std::endl;
    std::cout << "  local commands=\"" ;
    for (const QCommandLineOption& option : options) {
        for (const QString& name : option.names()) {
            std::cout << "--" << name.toStdString() << " ";
        }
    }
    std::cout << "\"" << std::endl;
    std::cout << "  COMPREPLY=( $(compgen -W \"${commands}\" -- ${current}) )" << std::endl;
    std::cout << "  return 0" << std::endl;
    std::cout << "}" << std::endl;
    std::cout << "complete -F _"<< appName.toStdString() << " " << appName.toStdString() << std::endl;
}

// Generates Zsh shell completion script
void Utils::Cli::generateZshCompletionScript(const QList<QCommandLineOption>& options, const QString& appName) {
    std::cout << "#compdef " << appName.toStdString() << std::endl;
    std::cout << "local -a _1" << std::endl;
    std::cout << "_1=(";
    for (const QCommandLineOption& option : options) {
        for (const QString& name : option.names()) {
            std::cout << "'" << "--" << name.toStdString() << "'";
        }
    }
    std::cout << ")" << std::endl;
    std::cout << "_describe 'command' _1" << std::endl;
}
