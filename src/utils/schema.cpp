/*
 * Copyright (c) 2014-2025 Patrizio Bekerle -- <patrizio@bekerle.com>
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

#include "schema.h"

#include <QColorDialog>
#include <QDebug>
#include <QFontDatabase>
#include <QInputDialog>
#include <QStringList>
#include <QTextEdit>
#include <QTreeWidgetItem>
#include <cmath>

#include "services/settingsservice.h"

Utils::Schema::Settings* Utils::Schema::schemaSettings = nullptr;

/** The default schema table is initialized here, precisely once, as loading
 * a settings object from a resource is expensive, unlike most other uses of
 * QSettings. Only call this once, when setting Utils::Schema::schemaSettings */
Utils::Schema::Settings::Settings()
    : _defaultSchemaSettings(QStringLiteral(":/configurations/schemes.conf"),
                             QSettings::IniFormat) {
    _defaultFontSet = false;

    _defaultSchemaKeysList =
        _defaultSchemaSettings.value(QStringLiteral("Editor/DefaultColorSchemes")).toStringList();

    // Determine the keys for a given schema
    _defaultSchemaSubkeylists.reserve(_defaultSchemaKeysList.size());
    Q_FOREACH (const QString& schema, _defaultSchemaKeysList) {
        QStringList groupKeys;
        Q_FOREACH (const QString& key, _defaultSchemaSettings.allKeys()) {
            if (key.startsWith(schema)) {
                groupKeys.append(QString(key).remove(0, schema.size() + 1));
            }
        }
        _defaultSchemaSubkeys.insert(schema, _defaultSchemaSubkeylists.size());
        _defaultSchemaSubkeylists.append(groupKeys);
    }
}

/**
 * Returns the default schema keys
 *
 * @return
 */
const QStringList& Utils::Schema::Settings::defaultSchemaKeys() const {
    return _defaultSchemaKeysList;
}

/**
 * Returns the default schema settings object
 *
 * @return
 */
const QSettings& Utils::Schema::Settings::defaultSchemaSettings() const {
    return _defaultSchemaSettings;
}

/**
 * Returns the current schema key
 *
 * @return
 */
QString Utils::Schema::Settings::currentSchemaKey() const {
    SettingsService settings;
    return settings
        .value(QStringLiteral("Editor/CurrentSchemaKey"), _defaultSchemaKeysList.length() > 0
                                                              ? _defaultSchemaKeysList[0]
                                                              : QStringLiteral("DefaultSchema"))
        .toString();
}

/**
 * Checks if the current schema is a default schema
 *
 * @return
 */
bool Utils::Schema::Settings::currentSchemaIsDefault() const {
    return _defaultSchemaSubkeys.contains(currentSchemaKey());
}

/**
 * Returns the list of keys for a given schema.
 *
 * @param schema
 * @return
 */
QStringList Utils::Schema::Settings::getSchemaKeys(const QString& schema) const {
    if (_defaultSchemaSubkeys.contains(schema)) {
        return _defaultSchemaSubkeylists[_defaultSchemaSubkeys[schema]];
    } else {
        QStringList groupKeys;
        SettingsService s;
        s.beginGroup(schema);
        return s.allKeys();
    }
}

/**
 * Returns a value of the current schema
 *
 * @param key
 * @param defaultValue
 * @return
 */
QVariant Utils::Schema::Settings::getSchemaValue(const QString& key, const QVariant& defaultValue,
                                                 QString schemaKey) const {
    const bool schemaNotSet = schemaKey.isEmpty();

    if (schemaNotSet) {
        schemaKey = currentSchemaKey();
    }

    const bool isDefaultSchema = _defaultSchemaSubkeys.contains(schemaKey);

    QVariant value =
        isDefaultSchema
            ? _defaultSchemaSettings.value(schemaKey + QStringLiteral("/") + key, defaultValue)
            : SettingsService().value(schemaKey + QStringLiteral("/") + key, defaultValue);

    if (!value.isValid() && schemaNotSet) {
        QString fallbackSchemaKey =
            isDefaultSchema
                ? _defaultSchemaSettings.value(schemaKey + QStringLiteral("/FallbackSchema"))
                      .toString()
                : SettingsService().value(schemaKey + QStringLiteral("/FallbackSchema")).toString();

        if (!fallbackSchemaKey.isEmpty()) {
            value = getSchemaValue(key, defaultValue, fallbackSchemaKey);
        }
    }

    return value;
}

/**
 * Returns the text settings key for an index
 *
 * @param key
 * @param index
 * @return
 */
QString Utils::Schema::textSettingsKey(const QString& key, int index) {
    return key + QStringLiteral("_") + QString::number(index);
}

/**
 * Tries to fetch the correct font for an index
 *
 * @param index
 * @return
 */
QFont Utils::Schema::Settings::getFont(int index) const {
    // get the foreground color
    bool enabled = getSchemaValue(textSettingsKey(QStringLiteral("FontEnabled"), index)).toBool();
    QFont font = getEditorFont(index);

    // if the font is enabled try to fetch it
    if (enabled) {
        QFont overrideFont =
            getSchemaValue(textSettingsKey(QStringLiteral("Font"), index)).value<QFont>();
        font.setFamily(overrideFont.family());
    }

    return font;
}

/**
 * Tries to fetch the correct foreground color for an index
 *
 * @param index
 * @return
 */
QColor Utils::Schema::Settings::getForegroundColor(int index) const {
    // get the foreground color
    bool enabled =
        getSchemaValue(textSettingsKey(QStringLiteral("ForegroundColorEnabled"), index)).toBool();
    QColor color;

    // if the foreground color is enabled try to fetch it
    if (enabled) {
        color = getSchemaValue(textSettingsKey(QStringLiteral("ForegroundColor"), index))
                    .value<QColor>();
    }

    // if the color was not valid, try to fetch the color for "Text"
    if (!color.isValid() && (index >= 0)) {
        color = getForegroundColor(TextPresetIndex);
    }

    // if the color still was not valid, try to fetch the color from a QTextEdit
    if (!color.isValid()) {
        QTextEdit textEdit;
        color = textEdit.textColor();
    }

    // if the color still was not valid, use black
    if (!color.isValid()) {
        color = QColor(Qt::black);
    }

    return color;
}

/**
 * Tries to fetch the correct background color for an index
 *
 * @param index
 * @return
 */
QColor Utils::Schema::Settings::getBackgroundColor(int index) const {
    // get the foreground color
    bool enabled =
        getSchemaValue(textSettingsKey(QStringLiteral("BackgroundColorEnabled"), index)).toBool();
    QColor color;

    // if the foreground color is enabled try to fetch it
    if (enabled) {
        color = getSchemaValue(textSettingsKey(QStringLiteral("BackgroundColor"), index))
                    .value<QColor>();
    }

    // if the color was not valid, try to fetch the color for "Text"
    if (!color.isValid() && (index >= 0)) {
        color = getBackgroundColor(TextPresetIndex);
    }

    // if the color still was not valid, use black
    if (!color.isValid()) {
        color = QColor(Qt::white);
    }

    return color;
}

/**
 * Sets the foreground and background color for an format
 *
 * @param format
 * @param index
 */
void Utils::Schema::Settings::setFormatStyle(MarkdownHighlighter::HighlighterState index,
                                             QTextCharFormat& format) const {
    // get the correct font
    QFont font = getFont(index);

    // this fixes issues rendering monospaced fonts bold when they
    // are set to bold by setFontWeight below
    // https://github.com/pbek/QOwnNotes/issues/2338
    font.setStyleName(QString());

    // set the font
    format.setFont(font);

    // adapt the font size
    adaptFontSize(index, font);

    // override the font size
    format.setFontPointSize(font.pointSize());

    // set the foreground color
    format.setForeground(QBrush(getForegroundColor(index)));

    bool backgroundColorEnabled =
        getSchemaValue(textSettingsKey(QStringLiteral("BackgroundColorEnabled"), index)).toBool();

    // set the background (color) only if the background color is enabled,
    // otherwise we get troubles with the background overwriting the foreground
    // of neighboring text (e.g. for italic text)
    format.setBackground(backgroundColorEnabled ? QBrush(getBackgroundColor(index)) : QBrush());

    // set the bold state
    format.setFontWeight(
        getSchemaValue(Utils::Schema::textSettingsKey(QStringLiteral("Bold"), index)).toBool()
            ? QFont::Bold
            : QFont::Normal);

    // set the italic state
    format.setFontItalic(
        getSchemaValue(Utils::Schema::textSettingsKey(QStringLiteral("Italic"), index)).toBool());

    // set the underline state
    format.setFontUnderline(
        getSchemaValue(Utils::Schema::textSettingsKey(QStringLiteral("Underline"), index))
            .toBool());
}

/**
 * Adapts the font size of a font for an index
 *
 * @param index
 * @param font
 */
void Utils::Schema::Settings::adaptFontSize(int index, QFont& font) const {
    int adaption =
        getSchemaValue(textSettingsKey(QStringLiteral("FontSizeAdaption"), index), 100).toInt();
    double fontSize = round(font.pointSize() * adaption / 100);

    if (fontSize > 0) {
        font.setPointSize(fontSize);
    }
}

/**
 * Returns the editor text font
 *
 * @return
 */
QFont Utils::Schema::Settings::getEditorTextFont() const {
    if (!_defaultFontSet) {
        _defaultTextEditFont = QTextEdit().font();
        _defaultFontSet = true;
    }
    SettingsService settings;
    QString fontString = settings.value(QStringLiteral("MainWindow/noteTextEdit.font")).toString();

    QFont font(_defaultTextEditFont);
    if (!fontString.isEmpty()) {
        // set the note text edit font
        font.fromString(fontString);
    } else {
        // store the default settings
        fontString = _defaultTextEditFont.toString();
        settings.setValue(QStringLiteral("MainWindow/noteTextEdit.font"), fontString);
    }

    return font;
}

/**
 * Returns the editor fixed font
 *
 * @return
 */
QFont Utils::Schema::Settings::getEditorFixedFont() const {
    if (!_defaultFontSet) {
        _defaultTextEditFont = QTextEdit().font();
        _defaultFontSet = true;
    }

    SettingsService settings;
    QString fontString =
        settings.value(QStringLiteral("MainWindow/noteTextEdit.code.font")).toString();

    QFont font(_defaultTextEditFont);
    if (!fontString.isEmpty()) {
        // set the code font
        font.fromString(fontString);
    } else {
        font = QFontDatabase::systemFont(QFontDatabase::FixedFont);

        // store the default settings
        settings.setValue(QStringLiteral("MainWindow/noteTextEdit.code.font"), font.toString());
    }

    return font;
}

/**
 * Returns the correct editor font
 *
 * @return
 */
QFont Utils::Schema::Settings::getEditorFont(int index) const {
    if (index == MarkdownHighlighter::CodeBlock || index == MarkdownHighlighter::InlineCodeBlock ||
        index == MarkdownHighlighter::CodeKeyWord || index == MarkdownHighlighter::CodeType ||
        index == MarkdownHighlighter::CodeComment || index == MarkdownHighlighter::CodeString ||
        index == MarkdownHighlighter::CodeNumLiteral || index == MarkdownHighlighter::CodeBuiltIn ||
        index == MarkdownHighlighter::CodeOther || index == MarkdownHighlighter::Table) {
        return getEditorFixedFont();
    } else {
        return getEditorTextFont();
    }
}

QString Utils::Schema::getSchemaStyles() {
    // reset background color of code blocks
    QString schemaStyles = QStringLiteral("code {background-color: transparent;}");

    schemaStyles += encodeCssStyleForState(MarkdownHighlighter::NoState, QStringLiteral("body"));
    schemaStyles += encodeCssStyleForState(MarkdownHighlighter::H1, QStringLiteral("h1"));
    schemaStyles += encodeCssStyleForState(MarkdownHighlighter::H2, QStringLiteral("h2"));
    schemaStyles += encodeCssStyleForState(MarkdownHighlighter::H3, QStringLiteral("h3"));
    schemaStyles += encodeCssStyleForState(MarkdownHighlighter::H4, QStringLiteral("h4"));
    schemaStyles += encodeCssStyleForState(MarkdownHighlighter::H5, QStringLiteral("h5"));
    schemaStyles += encodeCssStyleForState(MarkdownHighlighter::H6, QStringLiteral("h6"));
    schemaStyles += encodeCssStyleForState(MarkdownHighlighter::Link, QStringLiteral("a"));

    // We are adding also style combinations for bold and italic, because QTextBrowser doesn't
    // inherit the styles correctly (https://github.com/pbek/QOwnNotes/issues/3218)
    schemaStyles += encodeCssStyleForState(
        MarkdownHighlighter::Bold,
        QStringLiteral(
            "b, strong, i b, em b, i strong, em strong, b i, strong i, b em, strong em"));
    schemaStyles += encodeCssStyleForState(
        MarkdownHighlighter::Italic,
        QStringLiteral("i, em, i b, em b, i strong, em strong, b i, strong i, b em, strong em"));

    schemaStyles += encodeCssStyleForState(MarkdownHighlighter::CodeBlock,
                                           QStringLiteral("code, pre > code, pre"));
    schemaStyles += encodeCssStyleForState(MarkdownHighlighter::InlineCodeBlock,
                                           QStringLiteral("p > code, li > code"));

    // code block styles
    schemaStyles +=
        encodeCssStyleForState(MarkdownHighlighter::CodeKeyWord, QStringLiteral(".code-keyword"));
    schemaStyles +=
        encodeCssStyleForState(MarkdownHighlighter::CodeString, QStringLiteral(".code-string"));
    schemaStyles +=
        encodeCssStyleForState(MarkdownHighlighter::CodeComment, QStringLiteral(".code-comment"));
    schemaStyles +=
        encodeCssStyleForState(MarkdownHighlighter::CodeType, QStringLiteral(".code-type"));
    schemaStyles +=
        encodeCssStyleForState(MarkdownHighlighter::CodeOther, QStringLiteral(".code-other"));
    schemaStyles += encodeCssStyleForState(MarkdownHighlighter::CodeNumLiteral,
                                           QStringLiteral(".code-literal"));
    schemaStyles +=
        encodeCssStyleForState(MarkdownHighlighter::CodeBuiltIn, QStringLiteral(".code-builtin"));

    // enforce blockquotes styles (best effort, may not override all styles of other tags)
    // https://github.com/pbek/QOwnNotes/issues/2669
    if (SettingsService().value(QStringLiteral("fullyHighlightedBlockquotes")).toBool()) {
        schemaStyles +=
            encodeCssStyleForState(MarkdownHighlighter::BlockQuote, QStringLiteral("blockquote"));

        QTextCharFormat format;
        Utils::Schema::schemaSettings->setFormatStyle(MarkdownHighlighter::BlockQuote, format);

        // override foreground colors of sub-elements
        schemaStyles += QStringLiteral(
                            "blockquote strong, blockquote em, "
                            "blockquote i, blockquote b, blockquote a, blockquote code, "
                            "blockquote pre, blockquote h1, blockquote h2, blockquote h3, "
                            "blockquote h4, blockquote h5, blockquote h6 {color: ") +
                        format.foreground().color().name() + QStringLiteral("}");
    }

    return schemaStyles;
}

QString Utils::Schema::encodeCssTextCharFormat(const QTextCharFormat& format) {
    auto css = QStringLiteral("%1; color: %2;")
                   .arg(encodeCssFont(format.font()), format.foreground().color().name());

    auto brush = format.background();
    if (brush.isOpaque()) {
        css += QStringLiteral(" background-color: %1").arg(brush.color().name());
    }

    return css;
}

QString Utils::Schema::encodeCssStyleForState(MarkdownHighlighter::HighlighterState index,
                                              const QString& htmlTag) {
    QTextCharFormat format;
    Utils::Schema::schemaSettings->setFormatStyle(index, format);
    QString cssString = encodeCssTextCharFormat(format);

    // Allow italic inside bold tags, like `**bold *and italic***`
    // https://github.com/pbek/QOwnNotes/issues/3218
    // Unfortunately, that's not enough for the QTextBrowser to render it correctly
    if (index == MarkdownHighlighter::Italic) {
        cssString.remove(QStringLiteral("font-weight: normal;"));
    }

    return QStringLiteral("%1 {%2}").arg(htmlTag, cssString);
}

/**
 * Returns the CSS code for a QFont
 * Thank you to Phil Weinstein for the code
 */
QString Utils::Schema::encodeCssFont(const QFont& refFont) {
    //-----------------------------------------------------------------------
    // This function assembles a CSS Font specification string from
    // a QFont. This supports most of the QFont attributes settable in
    // the Qt 4.8 and Qt 5.3 QFontDialog.
    //
    // (1) Font Family
    // (2) Font Weight (just bold or not)
    // (3) Font Style (possibly Italic or Oblique)
    // (4) Font Size (in either pixels or points)
    // (5) Decorations (possibly Underline or Strikeout)
    //
    // Not supported: Writing System (e.g. Latin).
    //
    // See the corresponding decode function, below.
    // QFont decodeCssFontString (const QString cssFontStr)
    //-----------------------------------------------------------------------

    QStringList fields;    // CSS font attribute fields

    // ***************************************************
    // *** (1) Font Family: Primary plus Substitutes ***
    // ***************************************************

    const QString family = refFont.family();

    // NOTE [9-2014, Qt 4.8.6]: This isn't what I thought it was. It
    // does not return a list of "fallback" font faces (e.g. Georgia,
    // Serif for "Times New Roman"). In my testing, this is always
    // returning an empty list.
    //
    QStringList famSubs = QFont::substitutes(family);

    if (!famSubs.contains(family)) famSubs.prepend(family);

    static const QChar DBL_QUOT('"');
    const int famCnt = famSubs.count();
    QStringList famList;
    for (int inx = 0; inx < famCnt; ++inx) {
        // Place double quotes around family names having space characters,
        // but only if double quotes are not already there.
        //
        const QString fam = famSubs[inx];
        if (fam.contains(' ') && !fam.startsWith(DBL_QUOT))
            famList << (DBL_QUOT + fam + DBL_QUOT);
        else
            famList << fam;
    }

    const QString famStr = QStringLiteral("font-family: ") + famList.join(QStringLiteral(", "));
    fields << famStr;

    // reset font-weight to "normal" so headers and bold tags can be
    // "normal" if they are not set to "bold"
    fields << QStringLiteral("font-weight: normal");

    // **************************************
    // *** (2) Font Weight: Bold or Not ***
    // **************************************

    const bool bold = refFont.bold();
    if (bold) fields << QStringLiteral("font-weight: bold");

    // ****************************************************
    // *** (3) Font Style: possibly Italic or Oblique ***
    // ****************************************************

    const QFont::Style style = refFont.style();
    switch (style) {
        case QFont::StyleNormal:
            break;
        case QFont::StyleItalic:
            fields << QStringLiteral("font-style: italic");
            break;
        case QFont::StyleOblique:
            fields << QStringLiteral("font-style: oblique");
            break;
    }

    // ************************************************
    // *** (4) Font Size: either Pixels or Points ***
    // ************************************************

    const double sizeInPoints = refFont.pointSizeF();    // <= 0 if not defined.
    const int sizeInPixels = refFont.pixelSize();        // <= 0 if not defined.
    if (sizeInPoints > 0.0)
        fields << QStringLiteral("font-size: %1pt").arg(sizeInPoints);
    else if (sizeInPixels > 0)
        fields << QStringLiteral("font-size: %1px").arg(sizeInPixels);

    // ***********************************************
    // *** (5) Decorations: Underline, Strikeout ***
    // ***********************************************

    const bool underline = refFont.underline();
    const bool strikeOut = refFont.strikeOut();

    if (underline && strikeOut)
        fields << QStringLiteral("text-decoration: underline line-through");
    else if (underline)
        fields << QStringLiteral("text-decoration: underline");
    else if (strikeOut)
        fields << QStringLiteral("text-decoration: line-through");

    const QString cssFontStr = fields.join(QStringLiteral("; "));
    return cssFontStr;
}
