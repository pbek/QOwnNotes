#ifndef TESTHTMLENTITIES_H
#define TESTHTMLENTITIES_H

#include <QObject>
#include "helpers/htmlentities.h"

class TestHTMLEntities : public QObject
{
    Q_OBJECT

private:
    HTMLEntities htmlEntities;

private Q_SLOTS:
    void initTestCase();
    void testEntities();
};

#endif // TESTHTMLENTITIES_H
