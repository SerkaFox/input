#include "vigemkeymapper.h"

namespace Vigem {

VigemKeyMapper::VigemKeyMapper():
    keyMap()
{

}

bool VigemKeyMapper::resolvableKey(Qt::Key key) const
{
    return keyMap.values().contains(key);
}

VigemOperation VigemKeyMapper::resolveKey(Qt::Key key) const
{
    return keyMap.key(key, ButtonInvalid);
}

QJsonObject VigemKeyMapper::toJson() const
{
    QJsonArray array;

    for (const VigemOperation & key: keyMap.keys()) {
        array.append(QJsonValue(itemToJson(key)));
    }

    QJsonObject obj;

    obj.insert("keys", QJsonValue(array));

    return obj;
}

bool VigemKeyMapper::writeToFile(const QString &filename) const
{
    QFile file(filename);

    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc;

        doc.setObject(toJson());

        file.write(doc.toJson());
        file.close();

        return true;
    }

    return false;
}

VigemKeyMapper VigemKeyMapper::createDefault()
{
    VigemKeyMapper mapper;

    mapper.keyMap[ButtonA] = Qt::Key_A;
    mapper.keyMap[ButtonB] = Qt::Key_B;
    mapper.keyMap[ButtonX] = Qt::Key_C;
    mapper.keyMap[ButtonY] = Qt::Key_D;
    mapper.keyMap[ButtonLeft] = Qt::Key_E;
    mapper.keyMap[ButtonRight] = Qt::Key_F;
    mapper.keyMap[ButtonUp] = Qt::Key_G;
    mapper.keyMap[ButtonDown] = Qt::Key_H;
    mapper.keyMap[ButtonStart] = Qt::Key_I;
    mapper.keyMap[ButtonBack] = Qt::Key_J;
    mapper.keyMap[ButtonLThumb] = Qt::Key_K;
    mapper.keyMap[ButtonRThumb] = Qt::Key_L;
    mapper.keyMap[ButtonLShoulder] = Qt::Key_M;
    mapper.keyMap[ButtonRShoulder] = Qt::Key_N;
    mapper.keyMap[ButtonLTrigger] = Qt::Key_O;
    mapper.keyMap[ButtonRTrigger] = Qt::Key_P;
    mapper.keyMap[ButtonLThumbLeft] = Qt::Key_Q;
    mapper.keyMap[ButtonLThumbRight] = Qt::Key_R;
    mapper.keyMap[ButtonLThumbUp] = Qt::Key_S;
    mapper.keyMap[ButtonLThumbDown] = Qt::Key_T;
    mapper.keyMap[ButtonRThumbLeft] = Qt::Key_U;
    mapper.keyMap[ButtonRThumbRight] = Qt::Key_V;
    mapper.keyMap[ButtonRThumbUp] = Qt::Key_W;
    mapper.keyMap[ButtonRThumbDown] = Qt::Key_X;
  //  mapper.keyMap[ButtonStartup] = Qt::Key_Y;
    mapper.keyMap[ButtonShutdown] = Qt::Key_Z;

    return mapper;
}

VigemKeyMapper VigemKeyMapper::createFromJson(QJsonObject obj)
{
    VigemKeyMapper mapper;

    QJsonArray arr = obj.value("keys").toArray();

    for (int i = 0; i < arr.count(); i++) {
        QJsonObject item = arr.at(i).toObject();

        VigemOperation op = mapper.operationFromString(item.value("operation").toString());
        Qt::Key key = (Qt::Key) QKeySequence(item.value("key").toString(), QKeySequence::PortableText)[0];

        if (op != ButtonInvalid) {
            mapper.keyMap[op] = key;
        }
    }

    return mapper;
}

VigemKeyMapper VigemKeyMapper::createFromFile(const QString &filename)
{
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        return createFromJson(doc.object());
    }

    return createDefault();
}

QJsonObject VigemKeyMapper::itemToJson(VigemOperation op) const
{
    QJsonObject obj;

    obj.insert("operation", operationToString(op));
    obj.insert("key", QKeySequence(keyMap.value(op)).toString(QKeySequence::PortableText));

    return obj;
}

QString VigemKeyMapper::operationToString(VigemOperation op) const
{
    if (op == ButtonA) {
        return QString("buttonA");
    } else if (op == ButtonB) {
        return QString("buttonB");
    } else if (op == ButtonX) {
        return QString("buttonX");
    } else if (op == ButtonY) {
        return QString("buttonY");
    } else if (op == ButtonLeft) {
        return QString("buttonLeft");
    } else if (op == ButtonRight) {
        return QString("buttonRight");
    } else if (op == ButtonUp) {
        return QString("buttonUp");
    } else if (op == ButtonDown) {
        return QString("buttonDown");
    } else if (op == ButtonStart) {
        return QString("buttonStart");
    } else if (op == ButtonBack) {
        return QString("buttonBack");
    } else if (op == ButtonLThumb) {
        return QString("buttonLThumb");
    } else if (op == ButtonRThumb) {
        return QString("buttonRThumb");
    } else if (op == ButtonLShoulder) {
        return QString("buttonLShoulder");
    } else if (op == ButtonRShoulder) {
        return QString("buttonRShoulder");
    } else if (op == ButtonLTrigger) {
        return QString("buttonLTrigger");
    } else if (op == ButtonRTrigger) {
        return QString("buttonRTrigger");
    } else if (op == ButtonLThumbLeft) {
        return QString("buttonLThumbLeft");
    } else if (op == ButtonLThumbRight) {
        return QString("buttonLThumbRight");
    } else if (op == ButtonLThumbUp) {
        return QString("buttonLThumbUp");
    } else if (op == ButtonLThumbDown) {
        return QString("buttonLThumbDown");
    } else if (op == ButtonRThumbLeft) {
        return QString("buttonRThumbLeft");
    } else if (op == ButtonRThumbRight) {
        return QString("buttonRThumbRight");
    } else if (op == ButtonRThumbUp) {
        return QString("buttonRThumbUp");
    } else if (op == ButtonRThumbDown) {
        return QString("buttonRThumbDown");
    } else if (op == ButtonStartup) {
        return QString("buttonStartup");
    } else if (op == ButtonShutdown) {
        return QString("buttonShutdown");
    }

    return QString();
}

VigemOperation VigemKeyMapper::operationFromString(const QString &s)
{
    for (int op = ButtonA; op < ButtonInvalid; op++) {
        if (s.toLower() == operationToString((VigemOperation) op).toLower()) {
            return (VigemOperation) op;
        }
    }

    return ButtonInvalid;
}

}
