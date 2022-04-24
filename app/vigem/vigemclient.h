#ifndef VIGEMCLIENT_H
#define VIGEMCLIENT_H

#include <QDebug>
#include <QObject>
#include <QLibrary>
#include <QDebug>
#include <QFunctionPointer>

#include "vigem_defs.h"
#include "vigemkeymapper.h"

namespace Vigem {

typedef void * VigemClientPointer;
typedef void * VigemTargetPointer;

/**
 * @brief ViGEm virtual joystick library connection
 */
class VigemClient: public QObject
{
    Q_OBJECT

public:
    /* Possible errors returned by ViGEm client library */
    enum VigemError {
        ErrorNone = 0x20000000,
        ErrorBusNotFound = 0xe0000001,
        ErrorNoFreeSlot = 0xe0000002,
        ErrorInvalidTarget = 0xe0000003,
        ErrorRemovalFailed = 0xe0000004,
        ErrorAlreadyConnected = 0xe0000005,
        ErrorTargetUninitialized = 0xe0000006,
        ErrorTargetNotPluggedIn = 0xe0000007,
        ErrorBusVersionMismatch = 0xe0000008,
        ErrorBusAccessFailed = 0xe0000009,
        ErrorCallbackAlreadyRegistered = 0xe0000010,
        ErrorCallbackNotFound = 0xe0000011,
        ErrorBusAlreadyConnected = 0xe0000012,
        ErrorBusInvalidHandle = 0xe0000013,
        ErrorXUsbUserIndexOurOfRange = 0xe0000014,
        ErrorInvalidParameter = 0xe0000015,
        ErrorNotSupported = 0xe0000016,
    };

    enum VigemButton {
        GamepadUp = 0x0001,
        GamepadDown = 0x0002,
        GamepadLeft = 0x0004,
        GamepadRight = 0x0008,
        GamepadStart = 0x0010,
        GamepadBack = 0x0020,
        GamepadLeftThumb = 0x0040,
        GamepadRightThumb = 0x0080,
        GamepadLeftShoulder = 0x0100,
        GamepadRightShoulder = 0x0200,
        GamepadA = 0x1000,
        GamepadB = 0x2000,
        GamepadX = 0x4000,
        GamepadY = 0x8000
    };

private:
    typedef VigemClientPointer (*VigemAllocFunction)();
    typedef void (*VigemFreeFunction)(VigemClientPointer);
    typedef VigemError (*VigemConnectFunction)(VigemClientPointer);
    typedef void (*VigemDisconnectFunction)(VigemClientPointer);

    typedef bool (*VigemTargetIsWaitableAddSupportedFunction)(VigemTargetPointer);
    typedef VigemTargetPointer (*VigemTargetX360AllocFunction)();
    typedef VigemTargetPointer (*VigemTargetDS4AllocFunction)();
    typedef void (*VigemTargetFreeFunction)(VigemTargetPointer);

    typedef VigemError (*VigemTargetAddFunction)(VigemClientPointer, VigemTargetPointer);
    /* FIXME: There is also vigem_target_add_async function */
    typedef VigemError (*VigemTargetRemoveFunction)(VigemClientPointer, VigemTargetPointer);
    /* FIXME: vigem_target_x360_register_notification */
    /* FIXME: vigem_target_ds4_register_notification */
    /* FIXME: vigem_target_x360_unregister_notification */
    /* FIXME: vigem_target_ds4_unregister_notification */

    typedef VigemError (*VigemTargetX360UpdateFunction)(VigemClientPointer, VigemTargetPointer, XInputGamepad);

public:
    explicit VigemClient(QObject *parent = nullptr);
    explicit VigemClient(const QString & path, QObject *parent = nullptr);

    const QString & libraryPath() const;
    bool isLoaded() const;
    bool isActive() const;

    VigemKeyMapper getKeyMapper() const;

    bool isKeyAssociated(Qt::Key key) const;

signals:
    void activated();
    void deactivated();

public slots:
    void setLibraryPath(const QString & path);
    bool loadLibrary(const QString & path = QString());

    void setKeyMapper(const VigemKeyMapper & mapper);

    /* Запустить эмулятор x360 или ds4 */
    bool startup(const QString & target = QString("x360"));
    /* Остановить эмулятор */
    bool shutdown();

    /* Нажатие на кнопку A */
    VigemError pressA();
    /* Отпустить кнопку A */
    VigemError releaseA();
    /* Нажатие на кнопку B */
    VigemError pressB();
    /* Отпустить кнопку B */
    VigemError releaseB();
    /* Нажатие на кнопку X */
    VigemError pressX();
    /* Отпустить кнопку X */
    VigemError releaseX();
    /* Нажатие на кнопку Y */
    VigemError pressY();
    /* Отпустить кнопку Y */
    VigemError releaseY();

    /* Нажатие на кнопку Up */
    VigemError pressUp();
    /* Отпустить кнопку Up */
    VigemError releaseUp();
    /* Нажатие на кнопку Down */
    VigemError pressDown();
    /* Отпустить кнопку Down */
    VigemError releaseDown();
    /* Нажатие на кнопку Left */
    VigemError pressLeft();
    /* Отпустить кнопку Left */
    VigemError releaseLeft();
    /* Нажатие на кнопку Right */
    VigemError pressRight();
    /* Отпустить кнопку Right */
    VigemError releaseRight();

    /* Нажатие на кнопку Start */
    VigemError pressStart();
    /* Отпустить кнопку Start */
    VigemError releaseStart();
    /* Нажатие на кнопку Back */
    VigemError pressBack();
    /* Отпустить кнопку Back */
    VigemError releaseBack();

    /* Нажатие на кнопку LeftThumb */
    VigemError pressLeftThumb();
    /* Отпустить кнопку LeftThumb */
    VigemError releaseLeftThumb();
    /* Нажатие на кнопку RightThumb */
    VigemError pressRightThumb();
    /* Отпустить кнопку RightThumb */
    VigemError releaseRightThumb();

    /* Нажатие на кнопку LeftShoulder */
    VigemError pressLeftShoulder();
    /* Отпустить кнопку LeftShoulder */
    VigemError releaseLeftShoulder();
    /* Нажатие на кнопку RightShoulder */
    VigemError pressRightShoulder();
    /* Отпустить кнопку RightShoulder */
    VigemError releaseRightShoulder();

    /* Нажатие на кнопку LeftTrigger */
    VigemError pressLeftTrigger();
    /* Отпустить кнопку LeftTrigger */
    VigemError releaseLeftTrigger();
    /* Нажатие на кнопку RightTrigger */
    VigemError pressRightTrigger();
    /* Отпустить кнопку RightTrigger */
    VigemError releaseRightTrigger();

    /* Сдвинуть рычаг Left Thumb влево */
    VigemError pressLeftThumbLeft();
    /* Отпустить рычаг Left Thumb сдвинутый влево */
    VigemError releaseLeftThumbLeft();
    /* Сдвинуть рычаг Left Thumb вправо */
    VigemError pressLeftThumbRight();
    /* Отпустить рычаг Left Thumb сдвинутый вправо */
    VigemError releaseLeftThumbRight();
    /* Сдвинуть рычаг Left Thumb вверх */
    VigemError pressLeftThumbUp();
    /* Отпустить рычаг Left Thumb сдвинутый вверх */
    VigemError releaseLeftThumbUp();
    /* Сдвинуть рычаг Left Thumb вниз */
    VigemError pressLeftThumbDown();
    /* Отпустить рычаг Left Thumb сдвинутый вниз */
    VigemError releaseLeftThumbDown();

    /* Сдвинуть рычаг Right Thumb влево */
    VigemError pressRightThumbLeft();
    /* Отпустить рычаг Right Thumb сдвинутый влево */
    VigemError releaseRightThumbLeft();
    /* Сдвинуть рычаг Right Thumb вправо */
    VigemError pressRightThumbRight();
    /* Отпустить рычаг Right Thumb сдвинутый вправо */
    VigemError releaseRightThumbRight();
    /* Сдвинуть рычаг Right Thumb вверх */
    VigemError pressRightThumbUp();
    /* Отпустить рычаг Right Thumb сдвинутый вверх */
    VigemError releaseRightThumbUp();
    /* Сдвинуть рычаг Right Thumb вниз */
    VigemError pressRightThumbDown();
    /* Отпустить рычаг Right Thumb сдвинутый вниз */
    VigemError releaseRightThumbDown();

    VigemError pressButton(VigemOperation op);
    VigemError releaseButton(VigemOperation op);
    VigemError doOperation(VigemOperation op, bool press = true);
    VigemError pressKey(Qt::Key key);
    VigemError releaseKey(Qt::Key key);

private:
    QString library_path;

    VigemKeyMapper keyMapper;

    QLibrary * vigem_library;

    XInputGamepad inputGamepad;

    VigemClientPointer vigem_client;
    VigemTargetPointer vigem_target;

    VigemAllocFunction alloc_function;
    VigemFreeFunction free_function;
    VigemConnectFunction connect_function;
    VigemDisconnectFunction disconnect_function;

    VigemTargetIsWaitableAddSupportedFunction target_is_waitable_add_supported_function;
    VigemTargetX360AllocFunction target_x360_alloc_function;
    VigemTargetDS4AllocFunction target_ds4_alloc_function;
    VigemTargetFreeFunction target_free_function;

    VigemTargetAddFunction target_add_function;
    VigemTargetRemoveFunction target_remove_function;

    VigemTargetX360UpdateFunction target_x360_update_function;
};

}

#endif // VIGEMCLIENT_H
