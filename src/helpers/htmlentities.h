#ifndef HTMLENTITIES_H
#define HTMLENTITIES_H

#include <QMap>
#include <QString>



class HTMLEntities
{
private:
    QMap<QString, QString> entityMap;
    void setupEntityMap();

public:
    HTMLEntities();
    QString decodeHtmlEntities( QString text );
};

#endif // HTMLENTITIES_H
