#include "vigemclient.h"

namespace Vigem {
VigemClient::VigemClient(QObject *parent):
    QObject(parent),
    library_path(),
    keyMapper()
{
    vigem_library = nullptr;

    vigem_client = nullptr;
    vigem_target = nullptr;

    alloc_function = nullptr;
    free_function = nullptr;
    connect_function = nullptr;
    disconnect_function = nullptr;

    target_add_function = nullptr;
    target_remove_function = nullptr;

    inputGamepad.wButtons = 0;
    inputGamepad.bLeftTrigger = 0;
    inputGamepad.bRightTrigger = 0;
    inputGamepad.sThumbLX = 0;
    inputGamepad.sThumbLY = 0;
    inputGamepad.sThumbRX = 0;
    inputGamepad.sThumbRY = 0;

    keyMapper = VigemKeyMapper::createDefault();
}

VigemClient::VigemClient(const QString & path, QObject * parent):
    QObject(parent),
    library_path(path),
    keyMapper()
{
    vigem_library = nullptr;

    vigem_client = nullptr;
    vigem_target = nullptr;

    alloc_function = nullptr;
    free_function = nullptr;
    connect_function = nullptr;
    disconnect_function = nullptr;

    target_add_function = nullptr;
    target_remove_function = nullptr;

    inputGamepad.wButtons = 0;
    inputGamepad.bLeftTrigger = 0;
    inputGamepad.bRightTrigger = 0;
    inputGamepad.sThumbLX = 0;
    inputGamepad.sThumbLY = 0;
    inputGamepad.sThumbRX = 0;
    inputGamepad.sThumbRY = 0;

    keyMapper = VigemKeyMapper::createDefault();
}

const QString &VigemClient::libraryPath() const
{
    return library_path;
}

bool VigemClient::isLoaded() const
{
    return bool(vigem_library);
}

bool VigemClient::isActive() const
{
    return bool(vigem_client);
}

VigemKeyMapper VigemClient::getKeyMapper() const
{
    return keyMapper;
}

bool VigemClient::isKeyAssociated(Qt::Key key) const
{
    return keyMapper.resolvableKey(key);
}

void VigemClient::setLibraryPath(const QString & path)
{
    library_path = path;
}

/**
 * @brief Load Vigem DLL file
 * @param path Path to Vigem dll, may be omited if already given via constructor.
 * @return True if library successfully loaded, false in case of an error
 */
bool VigemClient::loadLibrary(const QString & path)
{
    if (vigem_library) {
        qWarning() << "VigemClient: DLL already loaded";
        return false;
    }

    if (!path.isEmpty()) {
        library_path = path;
    }

    if (library_path.isEmpty()) {
        qWarning() << "VigemClient: DLL path not specified";
        return false;
    }

    vigem_library = new QLibrary(library_path, this);

    if (!vigem_library->load()) {
        qWarning() << "VigemClient: Failed to load ViGEm DLL";
        return false;
    }

    alloc_function = (VigemAllocFunction) vigem_library->resolve("vigem_alloc");
    if (!alloc_function) {
        qWarning() << "VigemClient: Failed to load vigem_alloc function";
        return false;
    }

    free_function = (VigemFreeFunction) vigem_library->resolve("vigem_free");
    if (!free_function) {
        qWarning() << "VigemClient: Failed to load vigem_free function";
        return false;
    }

    connect_function = (VigemConnectFunction) vigem_library->resolve("vigem_connect");
    if (!connect_function) {
        qWarning() << "VigemClient: Failed to load vigem_connect function";
        return false;
    }

    disconnect_function = (VigemDisconnectFunction) vigem_library->resolve("vigem_disconnect");
    if (!disconnect_function) {
        qWarning() << "VigemClient: Failed to load vigem_disconnect function";
        return false;
    }

    target_is_waitable_add_supported_function = (VigemTargetIsWaitableAddSupportedFunction) vigem_library->resolve("vigem_target_is_waitable_add_supported");
    if (!target_is_waitable_add_supported_function) {
        qWarning() << "VigemClient: Failed to load vigem_target_is_waitable_add_supported function";
        return false;
    }

    target_x360_alloc_function = (VigemTargetX360AllocFunction) vigem_library->resolve("vigem_target_x360_alloc");
    if (!target_x360_alloc_function) {
        qWarning() << "VigemClient: Failed to load vigem_target_x360_alloc function";
        return false;
    }

    target_ds4_alloc_function = (VigemTargetDS4AllocFunction) vigem_library->resolve("vigem_target_ds4_alloc");
    if (!target_ds4_alloc_function) {
        qWarning() << "VigemClient: Failed to load vigem_target_ds4_alloc function";
        return false;
    }

    target_free_function = (VigemTargetFreeFunction) vigem_library->resolve("vigem_target_free");
    if (!target_free_function) {
        qWarning() << "VigemClient: Failed to load vigem_target_free function";
        return false;
    }

    target_add_function = (VigemTargetAddFunction) vigem_library->resolve("vigem_target_add");
    if (!target_add_function) {
        qWarning() << "VigemClient: Failed to load vigem_target_add function";
        return false;
    }

    target_remove_function = (VigemTargetRemoveFunction) vigem_library->resolve("vigem_target_remove");
    if (!target_remove_function) {
        qWarning() << "VigemClient: Failed to load vigem_target_remove function";
        return false;
    }

    target_x360_update_function = (VigemTargetX360UpdateFunction) vigem_library->resolve("vigem_target_x360_update");
    if (!target_x360_update_function) {
        qWarning() << "VigemClient: Failed to load vigem_target_x360_update function";
        return false;
    }

    return true;
}

void VigemClient::setKeyMapper(const VigemKeyMapper &mapper)
{
    keyMapper = mapper;
}

/**
 * @brief VigemClient::startup
 * @param target Тип цели - может быть "x360" или "ds4"
 */
bool VigemClient::startup(const QString & target)
{
    if (isActive() || !isLoaded()) {
        qWarning() << "VigemClient::startup" << "called when already started";

        return false;
    }

    vigem_client = alloc_function();

    if (!vigem_client) {
        qWarning() << "VigemClient::startup" << "alloc_function returned nullptr";
        return false;
    }

    VigemError err = connect_function(vigem_client);

    if (err != ErrorNone) {
        qWarning() << "VigemClient::startup" << "connect_function returned" << err;
        free_function(vigem_client);

        return false;
    }

    if (target.toLower() == QString("x360")) {
        vigem_target = target_x360_alloc_function();
    } else if (target.toLower() == QString("ds4")) {
        vigem_target = target_ds4_alloc_function();
    } else {
        qWarning() << "VigemClient::startup" << "target must be x360 or ds4";

        disconnect_function(vigem_client);
        free_function(vigem_client);

        return false;
    }

    if (!vigem_target) {
        qWarning() << "VigemClient::startup" << "target_alloc_function returns nullptr for target" << target;

        disconnect_function(vigem_client);
        free_function(vigem_client);

        return false;
    }

    err = target_add_function(vigem_client, vigem_target);

    if (err != ErrorNone) {
        qWarning() << "VigemClient::startup" << "target_add_function returned" << err;
        target_free_function(vigem_target);
        disconnect_function(vigem_client);
        free_function(vigem_client);

        return false;
    }

    emit activated();

    return true;
}

bool VigemClient::shutdown()
{
    if (!isLoaded() || !isActive()) {
        qDebug() << "VigemClient::shutdown" << "called while inactive";

        return false;
    }

    target_remove_function(vigem_client, vigem_target);
    target_free_function(vigem_target);
    disconnect_function(vigem_client);
    free_function(vigem_client);

    vigem_target = nullptr;
    vigem_client = nullptr;

    emit deactivated();

    return true;
}

/**
 * @brief Нажать кнопку A эмулятора X360
 */
VigemClient::VigemError VigemClient::pressA()
{
    qDebug() << "VigemClient: trying to press A button";

    inputGamepad.wButtons |= GamepadA;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку A эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseA()
{
    qDebug() << "VigemClient: trying to release A button";

    inputGamepad.wButtons &=~ GamepadA;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Нажать кнопку B эмулятора X360
 */
VigemClient::VigemError VigemClient::pressB()
{
    qDebug() << "VigemClient: trying to press B button";

    inputGamepad.wButtons |= GamepadB;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку B эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseB()
{
    qDebug() << "VigemClient: trying to release B button";

    inputGamepad.wButtons &=~ GamepadB;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Нажать кнопку X эмулятора X360
 */
VigemClient::VigemError VigemClient::pressX()
{
    qDebug() << "VigemClient: trying to press X button";

    inputGamepad.wButtons |= GamepadX;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку X эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseX()
{
    qDebug() << "VigemClient: trying to release X button";

    inputGamepad.wButtons &=~ GamepadX;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Нажать кнопку Y эмулятора X360
 */
VigemClient::VigemError VigemClient::pressY()
{
    qDebug() << "VigemClient: trying to press Y button";

    inputGamepad.wButtons |= GamepadY;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку Y эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseY()
{
    qDebug() << "VigemClient: trying to release Y button";

    inputGamepad.wButtons &=~ GamepadY;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Нажать кнопку Вверх эмулятора X360
 */
VigemClient::VigemError VigemClient::pressUp()
{
    qDebug() << "VigemClient: trying to press Up button";

    inputGamepad.wButtons |= GamepadUp;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку Вверх эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseUp()
{
    qDebug() << "VigemClient: trying to release Up button";

    inputGamepad.wButtons &=~ GamepadUp;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Нажать кнопку Вниз эмулятора X360
 */
VigemClient::VigemError VigemClient::pressDown()
{
    qDebug() << "VigemClient: trying to press Down button";

    inputGamepad.wButtons |= GamepadDown;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку Вниз эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseDown()
{
    qDebug() << "VigemClient: trying to release Down button";

    inputGamepad.wButtons &=~ GamepadDown;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Нажать кнопку Влево эмулятора X360
 */
VigemClient::VigemError VigemClient::pressLeft()
{
    qDebug() << "VigemClient: trying to press Left button";

    inputGamepad.wButtons |= GamepadLeft;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку Влево эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseLeft()
{
    qDebug() << "VigemClient: trying to release Left button";

    inputGamepad.wButtons &=~ GamepadLeft;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Нажать кнопку Вправо эмулятора X360
 */
VigemClient::VigemError VigemClient::pressRight()
{
    qDebug() << "VigemClient: trying to press Right button";

    inputGamepad.wButtons |= GamepadRight;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку Вправо эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseRight()
{
    qDebug() << "VigemClient: trying to release Right button";

    inputGamepad.wButtons &=~ GamepadRight;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Нажать кнопку Start эмулятора X360
 */
VigemClient::VigemError VigemClient::pressStart()
{
    qDebug() << "VigemClient: trying to press Start button";

    inputGamepad.wButtons |= GamepadStart;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку Start эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseStart()
{
    qDebug() << "VigemClient: trying to release Start button";

    inputGamepad.wButtons &=~ GamepadStart;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Нажать кнопку Back эмулятора X360
 */
VigemClient::VigemError VigemClient::pressBack()
{
    qDebug() << "VigemClient: trying to press Back button";

    inputGamepad.wButtons |= GamepadBack;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку Back эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseBack()
{
    qDebug() << "VigemClient: trying to release Back button";

    inputGamepad.wButtons &=~ GamepadBack;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Нажать кнопку LeftThumb эмулятора X360
 */
VigemClient::VigemError VigemClient::pressLeftThumb()
{
    qDebug() << "VigemClient: trying to press Left Thumb button";

    inputGamepad.wButtons |= GamepadLeftThumb;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку LeftThumb эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseLeftThumb()
{
    qDebug() << "VigemClient: trying to release Left Thumb button";

    inputGamepad.wButtons &=~ GamepadLeftThumb;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Нажать кнопку RightThumb эмулятора X360
 */
VigemClient::VigemError VigemClient::pressRightThumb()
{
    qDebug() << "VigemClient: trying to press Right Thumb button";

    inputGamepad.wButtons |= GamepadRightThumb;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку RightThumb эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseRightThumb()
{
    qDebug() << "VigemClient: trying to release Right Thumb button";

    inputGamepad.wButtons &=~ GamepadRightThumb;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Нажать кнопку LeftShoulder эмулятора X360
 */
VigemClient::VigemError VigemClient::pressLeftShoulder()
{
    qDebug() << "VigemClient: trying to press Left Shoulder button";

    inputGamepad.wButtons |= GamepadLeftShoulder;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку LeftShoulder эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseLeftShoulder()
{
    qDebug() << "VigemClient: trying to release Left Shoulder button";

    inputGamepad.wButtons &=~ GamepadLeftShoulder;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Нажать кнопку RightShoulder эмулятора X360
 */
VigemClient::VigemError VigemClient::pressRightShoulder()
{
    qDebug() << "VigemClient: trying to press Right Shoulder button";

    inputGamepad.wButtons |= GamepadRightShoulder;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку RightShoulder эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseRightShoulder()
{
    qDebug() << "VigemClient: trying to release Right Shoulder button";

    inputGamepad.wButtons &=~ GamepadRightShoulder;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Нажать кнопку LeftTrigger эмулятора X360
 */
VigemClient::VigemError VigemClient::pressLeftTrigger()
{
    qDebug() << "VigemClient: trying to press Left Trigger button";

    inputGamepad.bLeftTrigger = 0xff;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку LeftTrigger эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseLeftTrigger()
{
    qDebug() << "VigemClient: trying to release Left Trigger button";

    inputGamepad.bLeftTrigger = 0;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Нажать кнопку RightTrigger эмулятора X360
 */
VigemClient::VigemError VigemClient::pressRightTrigger()
{
    qDebug() << "VigemClient: trying to press Right Trigger button";

    inputGamepad.bRightTrigger = 0xff;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

/**
 * @brief Отпустить кнопку RightTrigger эмулятора X360
 */
VigemClient::VigemError VigemClient::releaseRightTrigger()
{
    qDebug() << "VigemClient: trying to release Right Trigger button";

    inputGamepad.bRightTrigger = 0;

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::pressLeftThumbLeft()
{
    qDebug() << "VigemClient: trying to move left thumb to the left";

    if (inputGamepad.sThumbLX == 0) {
        inputGamepad.sThumbLX -= 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::releaseLeftThumbLeft()
{
    qDebug() << "VigemClient: trying to release left thumb moved to the left";

    if (inputGamepad.sThumbLX < 0) {
        inputGamepad.sThumbLX += 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::pressLeftThumbRight()
{
    qDebug() << "VigemClient: trying to move left thumb to the right";

    if (inputGamepad.sThumbLX == 0) {
        inputGamepad.sThumbLX += 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::releaseLeftThumbRight()
{
    qDebug() << "VigemClient: trying to release left thumb moved to the right";

    if (inputGamepad.sThumbLX > 0) {
        inputGamepad.sThumbLX -= 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::pressLeftThumbUp()
{
    qDebug() << "VigemClient: trying to move left thumb up";

    if (inputGamepad.sThumbLY == 0) {
        inputGamepad.sThumbLY += 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::releaseLeftThumbUp()
{
    qDebug() << "VigemClient: trying to release left thumb moved up";

    if (inputGamepad.sThumbLY > 0) {
        inputGamepad.sThumbLY -= 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::pressLeftThumbDown()
{
    qDebug() << "VigemClient: trying to move left thumb down";

    if (inputGamepad.sThumbLY == 0) {
        inputGamepad.sThumbLY -= 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::releaseLeftThumbDown()
{
    qDebug() << "VigemClient: trying to release left thumb moved down";

    if (inputGamepad.sThumbLY < 0) {
        inputGamepad.sThumbLY += 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::pressRightThumbLeft()
{
    qDebug() << "VigemClient: trying to move right thumb to the left";

    if (inputGamepad.sThumbRX == 0) {
        inputGamepad.sThumbRX -= 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::releaseRightThumbLeft()
{
    qDebug() << "VigemClient: trying to release right thumb moved to the left";

    if (inputGamepad.sThumbRX < 0) {
        inputGamepad.sThumbRX += 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::pressRightThumbRight()
{
    qDebug() << "VigemClient: trying to move right thumb to the right";

    if (inputGamepad.sThumbRX == 0) {
        inputGamepad.sThumbRX += 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::releaseRightThumbRight()
{
    qDebug() << "VigemClient: trying to release right thumb moved to the right";

    if (inputGamepad.sThumbRX > 0) {
        inputGamepad.sThumbRX -= 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::pressRightThumbUp()
{
    qDebug() << "VigemClient: trying to move right thumb up";

    if (inputGamepad.sThumbRY == 0) {
        inputGamepad.sThumbRY += 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::releaseRightThumbUp()
{
    qDebug() << "VigemClient: trying to release right thumb moved up";

    if (inputGamepad.sThumbRY > 0) {
        inputGamepad.sThumbRY -= 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::pressRightThumbDown()
{
    qDebug() << "VigemClient: trying to move right thumb down";

    if (inputGamepad.sThumbRY == 0) {
        inputGamepad.sThumbRY -= 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::releaseRightThumbDown()
{
    qDebug() << "VigemClient: trying to release right thumb moved down";

    if (inputGamepad.sThumbRY < 0) {
        inputGamepad.sThumbRY += 32767;
    }

    VigemError err = target_x360_update_function(vigem_client, vigem_target, inputGamepad);

    if (err != ErrorNone) {
        qWarning() << "VigemClient: vigem_target_x360_update returned" << err;
    }

    return err;
}

VigemClient::VigemError VigemClient::pressButton(Vigem::VigemOperation op)
{
    switch (op) {
    case Vigem::ButtonA:
        return pressA();

    case Vigem::ButtonB:
        return pressB();

    case Vigem::ButtonX:
        return pressX();

    case Vigem::ButtonY:
        return pressY();

    case Vigem::ButtonLeft:
        return pressLeft();

    case Vigem::ButtonRight:
        return pressRight();

    case Vigem::ButtonUp:
        return pressUp();

    case Vigem::ButtonDown:
        return pressDown();

    case Vigem::ButtonStart:
        return pressStart();

    case Vigem::ButtonBack:
        return pressBack();

    case Vigem::ButtonLThumb:
        return pressLeftThumb();

    case Vigem::ButtonRThumb:
        return pressRightThumb();

    case Vigem::ButtonLShoulder:
        return pressLeftShoulder();

    case Vigem::ButtonRShoulder:
        return releaseLeftShoulder();

    case Vigem::ButtonLTrigger:
        return pressLeftTrigger();

    case Vigem::ButtonRTrigger:
        return pressRightTrigger();

    case Vigem::ButtonLThumbLeft:
        return pressLeftThumbLeft();

    case Vigem::ButtonLThumbRight:
        return pressLeftThumbRight();

    case Vigem::ButtonLThumbUp:
        return pressLeftThumbUp();

    case Vigem::ButtonLThumbDown:
        return pressLeftThumbDown();

    case Vigem::ButtonRThumbLeft:
        return pressRightThumbLeft();

    case Vigem::ButtonRThumbRight:
        return pressRightThumbRight();

    case Vigem::ButtonRThumbUp:
        return pressRightThumbUp();

    case Vigem::ButtonRThumbDown:
        return pressRightThumbDown();

    case Vigem::ButtonStartup:
        if (startup()) {
            return ErrorNone;
        } else {
            return ErrorAlreadyConnected;
        }

    case Vigem::ButtonShutdown:
        shutdown();
        return ErrorNone;
    }

    return ErrorNone;
}

VigemClient::VigemError VigemClient::releaseButton(Vigem::VigemOperation op)
{
    switch (op) {
    case Vigem::ButtonA:
        return releaseA();

    case Vigem::ButtonB:
        return releaseB();

    case Vigem::ButtonX:
        return releaseX();

    case Vigem::ButtonY:
        return releaseY();

    case Vigem::ButtonLeft:
        return releaseLeft();

    case Vigem::ButtonRight:
        return releaseRight();

    case Vigem::ButtonUp:
        return releaseUp();

    case Vigem::ButtonDown:
        return releaseDown();

    case Vigem::ButtonStart:
        return releaseStart();

    case Vigem::ButtonBack:
        return releaseBack();

    case Vigem::ButtonLThumb:
        return releaseLeftThumb();

    case Vigem::ButtonRThumb:
        return releaseRightThumb();

    case Vigem::ButtonLShoulder:
        return releaseLeftShoulder();

    case Vigem::ButtonRShoulder:
        return releaseLeftShoulder();

    case Vigem::ButtonLTrigger:
        return releaseLeftTrigger();

    case Vigem::ButtonRTrigger:
        return releaseRightTrigger();

    case Vigem::ButtonLThumbLeft:
        return releaseLeftThumbLeft();

    case Vigem::ButtonLThumbRight:
        return releaseLeftThumbRight();

    case Vigem::ButtonLThumbUp:
        return releaseLeftThumbUp();

    case Vigem::ButtonLThumbDown:
        return releaseLeftThumbDown();

    case Vigem::ButtonRThumbLeft:
        return releaseRightThumbLeft();

    case Vigem::ButtonRThumbRight:
        return releaseRightThumbRight();

    case Vigem::ButtonRThumbUp:
        return releaseRightThumbUp();

    case Vigem::ButtonRThumbDown:
        return releaseRightThumbDown();
    }

    return ErrorNone;
}

VigemClient::VigemError VigemClient::doOperation(Vigem::VigemOperation op, bool press)
{
    if (press) {
        return pressButton(op);
    } else {
        return releaseButton(op);
    }
}

VigemClient::VigemError VigemClient::pressKey(Qt::Key key)
{
    if (keyMapper.resolvableKey(key)) {
        return pressButton(keyMapper.resolveKey(key));
    }

    return ErrorNone;
}

VigemClient::VigemError VigemClient::releaseKey(Qt::Key key)
{
    if (keyMapper.resolvableKey(key)) {
        return releaseButton(keyMapper.resolveKey(key));
    }

    return ErrorNone;
}

}
