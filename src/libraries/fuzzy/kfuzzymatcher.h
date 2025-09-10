/*
    This file is part of the KDE libraries

    SPDX-FileCopyrightText: 2017 Forrest Smith <forrestthewoods@gmail.com>
    SPDX-FileCopyrightText: 2021 Waqar Ahmed <waqar.17a@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <qglobal.h>

class QString;

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
class QStringView;
using StringType = QStringView;
#else
using StringType = const QString&;
#endif

/**
 * This code is ported to Qt from lib_fts:
 * https://github.com/forrestthewoods/lib_fts
 * which tries to replicate SublimeText like fuzzy matching.
 *
 * @note
 * All character matches will happen sequentially. That means that this function is not
 * typo tolerant i.e., "gti" will not match "git", but "gt" will. All methods in here are
 * stateless i.e., the input string will not be modified. Also note that strings in all the
 * functions in this namespace will be matched case-insensitively.
 *
 * Limitations:
 * - Currently this will match only strings with length < 256 correctly. This is because we
 * intend on matching a pattern against words / short strings and not paragraphs.
 * - No more than 256 matches will happen
 *
 * If you are using this with @c QSortFilterProxyModel, you need to override both
 * @c QSortFilterProxyModel::lessThan and @c QSortFilterProxyModel::filterAcceptsRow.
 * A simple example:
 *
 * \code
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override
    {
        int score = 0;
        const auto idx = sourceModel()->index(sourceRow, 0, sourceParent);
        const auto actionName = idx.data().toString().splitRef(QLatin1Char(':')).at(1);
        const bool res = kfts::fuzzy_match_sequential(m_pattern, actionName, score);
        // store the score in the source model
        sourceModel()->setData(idx, score, ScoreRole);
        return res;
    }

    bool lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const override
    {
        // use the score here to sort
        const int l = sourceLeft.data(ScoreRole).toInt();
        const int r = sourceRight.data(ScoreRole).toInt();
        return l < r;
    }
 * \endcode
 *
 * Additionally you must not use @c invalidateFilter() if you go with the above approach. Instead
 * use @c beginResetModel()/@c endResetModel():
 *
 * \code
 *  Q_SLOT void setFilterString(const QString &string)
    {
        beginResetModel();
        m_pattern = string;
        endResetModel();
    }
 * \endcode
 *
 * @short Namespace for fuzzy matching of strings
 * @author Waqar Ahmed <waqar.17a@gmail.com>
 */
namespace KFuzzyMatcher {

/**
 * @brief The result of a fuzzy match
 */
struct Result {
    /** Score of this match. This can be negative. if matched is @c false
        then the score will be zero.
    */
    int score = 0;
    /** @c true if match was successful */
    bool matched = false;
};

/**
 * @brief Simple fuzzy matching of chars in @p pattern with chars in @p str
 * sequentially. If there is a match, it will return true and false otherwise.
 * There is no scoring. You should use this if score is not important for you
 * and only matching is important.
 *
 * If @p pattern is empty, the function will return @c true
 *
 * @param pattern to search for. For e.g., text entered by a user to filter a
 * list
 * @param str the current string from your list of strings
 * @return @c true on sucessful match
 *
 * @since 5.79
 */
bool matchSimple(StringType pattern, StringType str);

/**
 * @brief This is the main function which does scored fuzzy matching.
 *
 * The return value of this function contains Result#score which should be used to
 * sort the results. Without sorting of the results this function won't very effective.
 *
 * If @p pattern is empty, the function will return @c true
 *
 * @param pattern to search for. For e.g., text entered by a user to filter a
 * list or model
 * @param str the current string from your list of strings
 * @return A Result type with score of this match and whether the match was
 * successful. If there is no match, score is zero. If the match is successful,
 * score must be used to sort the results.
 *
 * @since 5.79
 */
Result match(StringType pattern, StringType str);

QString toFuzzyMatchedDisplayString(const QString& pattern, const QString& str,
                                    const QString& htmlTag, const QString& htmlTagClose);

}    // namespace KFuzzyMatcher
