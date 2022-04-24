#ifndef VIGEMKEYMAPPER_H
#define VIGEMKEYMAPPER_H

#include <QMap>
#include <QDebug>
#include <QKeySequence>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>

#include "vigem_defs.h"

namespace Vigem {

class VigemKeyMapper
{
public:
    /* Creates an ampty key mapper which does not react to any key */
    VigemKeyMapper();

    /* Check whether the key mapper has any reaction to the specified key */
    bool resolvableKey(Qt::Key key) const;
    VigemOperation resolveKey(Qt::Key key) const;

    QJsonObject toJson() const;
    bool writeToFile(const QString & filename) const;

    static VigemKeyMapper createDefault();
    static VigemKeyMapper createFromJson(QJsonObject obj);
    static VigemKeyMapper createFromFile(const QString & filename);

private:
    QJsonObject itemToJson(VigemOperation op) const;
    QString operationToString(VigemOperation op) const;

    VigemOperation operationFromString(const QString & s);

    QMap<VigemOperation, Qt::Key> keyMap;
};

}

#endif // VIGEMKEYMAPPER_H
