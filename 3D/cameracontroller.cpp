#include "cameracontroller.h"

CameraController::CameraController(QNode *parent) :
    Qt3DCore::QEntity(parent),
    m_leftButtonPressed(false),
    m_keyUpPressed(false),

    m_dx(0),
    m_dy(0),
    m_scale(1),
    m_lookSpeed(180),
    m_linearSpeed(50),

    m_mouseDevice(new Qt3DInput::QMouseDevice(this)),
    m_keyboardDevice(new Qt3DInput::QKeyboardDevice(this)),
    m_logicalDevice(new Qt3DInput::QLogicalDevice(this)),

    m_keyUpAction(new Qt3DInput::QAction(this)),
    m_leftButtonAction(new Qt3DInput::QAction(this)),

    m_keyUpInput(new Qt3DInput::QActionInput(this)),
    m_leftButtonInput(new Qt3DInput::QActionInput(this)),

    m_xAxis(new Qt3DInput::QAxis(this)),
    m_yAxis(new Qt3DInput::QAxis(this)),
    m_wheelAxis(new Qt3DInput::QAxis(this)),

    m_mouseXInput(new Qt3DInput::QAnalogAxisInput(this)),
    m_mouseYInput(new Qt3DInput::QAnalogAxisInput(this)),
    m_wheelAxisInput(new Qt3DInput::QAnalogAxisInput(this)),

    m_frameAction(new Qt3DLogic::QFrameAction(this))
{
    m_leftButtonInput->setButtons({Qt::LeftButton});
    m_leftButtonInput->setSourceDevice(m_mouseDevice);
    m_leftButtonAction->addInput(m_leftButtonInput);

    m_keyUpInput->setButtons({Qt::Key_Up});
    m_keyUpInput->setSourceDevice(m_keyboardDevice);
    m_keyUpAction->addInput(m_keyUpInput);

    m_mouseXInput->setAxis(Qt3DInput::QMouseDevice::X);
    m_mouseXInput->setSourceDevice(m_mouseDevice);
    m_xAxis->addInput(m_mouseXInput);

    m_mouseYInput->setAxis(Qt3DInput::QMouseDevice::Y);
    m_mouseYInput->setSourceDevice(m_mouseDevice);
    m_yAxis->addInput(m_mouseYInput);

    m_wheelAxisInput->setAxis(Qt3DInput::QMouseDevice::WheelY);
    m_wheelAxisInput->setSourceDevice(m_mouseDevice);
    m_wheelAxis->addInput(m_wheelAxisInput);
    //m_mouseDevice->setUpdateAxesContinuously(true);

    m_logicalDevice->addAction(m_keyUpAction);
    m_logicalDevice->addAction(m_leftButtonAction);
    m_logicalDevice->addAxis(m_xAxis);
    m_logicalDevice->addAxis(m_yAxis);
    m_logicalDevice->addAxis(m_wheelAxis);

    connect(m_keyUpAction, &Qt3DInput::QAction::activeChanged, this, [this](bool active)
    {
        m_keyUpPressed = active;
    });
    connect(m_leftButtonAction, &Qt3DInput::QAction::activeChanged, this, [this](bool active)
    {
        m_leftButtonPressed = active;
    });
    connect(m_xAxis, &Qt3DInput::QAxis::valueChanged, this, [this](float value){ m_dx = value; });
    connect(m_yAxis, &Qt3DInput::QAxis::valueChanged, this, [this](float value){ m_dy = value; });
    connect(m_wheelAxis, &Qt3DInput::QAxis::valueChanged, this, [this](float value)
    {
        m_scale = value > 0 ? 1.25 : value < 0 ? 0.8 : 1;
        m_camera->setPosition(m_camera->position() * m_scale);
    });
    connect(m_frameAction, &Qt3DLogic::QFrameAction::triggered, this, &CameraController::frameActionTriggered);

    connect(this, &Qt3DCore::QEntity::enabledChanged, m_logicalDevice, &Qt3DInput::QLogicalDevice::setEnabled);
    connect(this, &Qt3DCore::QEntity::enabledChanged, m_frameAction, &Qt3DLogic::QFrameAction::setEnabled);
    connect(this, &Qt3DCore::QEntity::enabledChanged, m_xAxis, &Qt3DInput::QAxis::setEnabled);
    connect(this, &Qt3DCore::QEntity::enabledChanged, m_yAxis, &Qt3DInput::QAxis::setEnabled);
    connect(this, &Qt3DCore::QEntity::enabledChanged, m_wheelAxis, &Qt3DInput::QAxis::setEnabled);

    addComponent(m_frameAction);
    addComponent(m_logicalDevice);
}

void CameraController::setCamera(Qt3DRender::QCamera *camera)
{
    m_camera = camera;
}

void CameraController::setLookSpeed(float lookSpeed)
{
    m_lookSpeed = lookSpeed;
}

float CameraController::lookSpeed() const
{
    return m_lookSpeed;
}

void CameraController::setLinearSpeed(float linearSpeed)
{
    m_linearSpeed = linearSpeed;
}

void CameraController::frameActionTriggered(float dt)
{
    if (!m_camera) return;

    if (m_leftButtonPressed)
    {
        m_camera->panAboutViewCenter(-m_dx, QVector3D(0, 0, 1));
        m_camera->tiltAboutViewCenter(-m_dy);
    }

    if (m_keyUpPressed)
        m_camera->rollAboutViewCenter(0.5 * m_lookSpeed * dt);
}

float CameraController::linearSpeed() const
{
    return m_linearSpeed;
}
