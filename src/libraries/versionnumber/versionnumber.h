/*
    Copyright (C) 2008  Tim Fechtner < urwald at users dot sourceforge dot net >

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VERSIONNUMBER_H
#define VERSIONNUMBER_H

#include <QStringList>
#include <QMetaType>

/** \brief A data type for version numbers.
  *
  * This class provides a data type for version numbers. Think of it
  * as a \e QString which provides special behavior for version
  * numbers in the six relational operators (\<, \<=, \>, \>=, ==, !=).
  *
  * The behavior of the relational operators is similar to the behavior
  * of RPM when comparing versions. "Similar" means that it is \e not
  * \e equal! See http://rpm.org/wiki/PackagerDocs/Dependencies for a
  * good description of the algorithm used by RPM to determinate
  * version ordering.
  *
  * You can assign values of the type \e QString and even \e qint64
  * (which will be converted to a QString) and of course of the
  * type \e %VersionNumber itself
  * to it. You can use the assignment operator or the constructor
  * for initiation. The data type is made available to QMetaType and
  * is this way available in for example QVariant. If you want to use
  * it in in non-template based functions like \e queued signal
  * and slot connections, do something like
  * <tt>int id = qRegisterMetaType<VersionNumber>("VersionNumber");</tt>
  * at the begin of your main function.
  * This will register the type also for this use case. <tt>id</tt>
  * will contain the type identifier used by QMetaObject. However,
  * for most cases <tt>id</tt> isn't intresting.
  *
  * You can convert to a string with toString(). This function returns
  * always exactly the string which was used to initialize this object.
  *
  * To compare version numbers, the QString is segmented into small
  * parts. See http://rpm.org/wiki/PackagerDocs/Dependencies for details.
  * The algorithm of \e %VersionNumber differs in some points from the
  * algorithm of RPM:
  * \li It accepts \e all strings, also with special characters.
  * \li You can use not only "." but also "-" as often as you want. (Each
  *     new dash separates a new part in the number.)
  * \li You can safely use special characters. If they aren't "." or "-",
  *     then they are treated as normal characters. The very last
  *     segmentation (e.g. "12#rc1" to "12", "#", "rc", "1") does not only
  *     differ between QChar::isDigit() and QChar::isLetter (like RPM does), but has
  *     a third category for characters who are neither digit nor letter.
  * \li The very first occurrence of ":" is treated as separator for the epoch.
  *     Each following occurrence of ":" is treated as normal character.
  * */
class VersionNumber
{

  public:
    VersionNumber();
    VersionNumber(const VersionNumber & value); // copy constructor
    VersionNumber(const QString & value);
    VersionNumber(const qint64 value);
    virtual ~VersionNumber();
    QString toString() const;
    VersionNumber operator=(const VersionNumber & value);
    VersionNumber operator=(const QString & value);
    VersionNumber operator=(qint64 value);
    bool operator<(const VersionNumber & value) const;
    bool operator<=(const VersionNumber & value) const;
    bool operator>(const VersionNumber & value) const;
    bool operator>=(const VersionNumber & value) const;
    bool operator==(const VersionNumber & value) const;
    bool operator!=(const VersionNumber & value) const;

  private:
    // typedefs
    /** Internally used data type for parts of the version number like "12rc1". */
    typedef QStringList simpleNumber;
    /** Internally used data type for parts of the version number like "2.12rc1.3". */
    typedef QList< simpleNumber > numberWithPoints;
    /** Internally used data type for parts of the version number like "2.12rc1.3-9.7". */
    typedef QList< numberWithPoints > numberWithPointsAndDashes;
    /** Internally used data type for determinating if a <tt>QChar</tt> is
    *   \li <tt>QChar::isLetter()</tt> \e or
    *   \li <tt>QChar::isDigit</tt> \e or
    *   \li neither the one nor the other */
    enum characterType {
      letter,
      digit,
      other
    };
    /** Internally used data type for determinating if when comparing two values
    *   \li the first one is bigger \e or
    *   \li the second one is bigger \e or
    *   \li both are equal */
    enum type_whichIsBigger {
      first_one,
      second_one,
      both_are_equal
    };

    // members
    /** Internally used to hold the string who contents the version number.
    *
    *   If this class gets initialized with a <tt>qint64</tt>, than this
    *   number is converted to a string. */
    QString theString;
    /** Internally used to hold a segmented version of the first part
    *   of the version number (means: the part before the first ":"). */
    numberWithPointsAndDashes epoch;
    /** Internally used to hold a segmented version of the second part
    *   of the version number (means: the part after the first ":"). */
    numberWithPointsAndDashes version_and_release;

    // methods
    /** Internally used to copy another object to \e this object. */
    void helper_copyFromHere(const VersionNumber & value);
    /** Internally used to set the value of \e this object to
    *   a specified string and create the segmented version of the string in #epoch
    *   and #version_and_release. */
    void helper_setValue(const QString & value);
    /** Internally used to determinate the type of a QChar.
    *   \sa #VersionNumber::characterType */
    static VersionNumber::characterType helper_characterType(const QChar & value);
    /** Internally used to create a
    *   #VersionNumber::simpleNumber from a <tt>QString</tt>. */
    static VersionNumber::simpleNumber helper_createSimpleNumber(const QString & value);
    /** Internally used to create a
    *   #VersionNumber::numberWithPoints from a <tt>QString</tt>. */
    static VersionNumber::numberWithPoints helper_createNumberWithPoints(const QString & value);
    /** Internally used to create a
    *   #VersionNumber::numberWithPointsAndDashes from a <tt>QString</tt>. */
    static VersionNumber::numberWithPointsAndDashes helper_createNumberWithPointsAndDashes(
      const QString & value);
    /** Internally used to compare 2 \e %VersionNumber. */
    static VersionNumber::type_whichIsBigger whichIsBigger(const VersionNumber & firstValue,
                                                           const VersionNumber & secondValue);
    /** Internally used to compare 2 #VersionNumber::numberWithPointsAndDashes. */
    static VersionNumber::type_whichIsBigger whichIsBigger(
      const numberWithPointsAndDashes & firstValue,
      const numberWithPointsAndDashes & secondValue);
    /** Internally used to compare 2 #VersionNumber::numberWithPoints. */
    static VersionNumber::type_whichIsBigger whichIsBigger(const numberWithPoints & firstValue,
                                                           const numberWithPoints & secondValue);
    /** Internally used to compare 2 #VersionNumber::simpleNumber. */
    static VersionNumber::type_whichIsBigger whichIsBigger(const simpleNumber & firstValue,
                                                           const simpleNumber & secondValue);
    /** Internally used to compare 2 segments (list entries, QStrings) of a
    *   #VersionNumber::simpleNumber.
    *   \warning Both strings must have <tt>size() > 0</tt>! Otherwise this function will crash! */
    static VersionNumber::type_whichIsBigger whichIsBigger(const QString & firstValue,
                                                           const QString & secondValue);
    /** Internally used to compare 2 QStrings which are assumed to contain digits.
    * \note This function assumes the values to be strings
    * which contain numbers. If not, this function will not have a useful result.*/
    static VersionNumber::type_whichIsBigger helper_whichNumberIsBigger(
      const QString & firstValue,
      const QString & secondValue);
    /** Internally used to compare 2 QStrings which are assumed to \e not contain digits. */
    static VersionNumber::type_whichIsBigger helper_whichStringIsBigger(
      const QString & firstValue,
      const QString & secondValue);

};

Q_DECLARE_METATYPE(VersionNumber)

#endif
