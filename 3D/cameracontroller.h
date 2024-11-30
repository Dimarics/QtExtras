#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <Qt3DCore/QEntity>
#include <Qt3DInput/QMouseDevice>
#include <Qt3DInput/QKeyboardDevice>
#include <Qt3DInput/QLogicalDevice>
#include <Qt3DInput/QAction>
#include <Qt3DInput/QActionInput>
#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAnalogAxisInput>
#include <Qt3DRender/QCamera>
#include <Qt3DLogic/QFrameAction>

class CameraController : public Qt3DCore::QEntity
{
public:
    CameraController(Qt3DCore::QNode *parent = nullptr);
    void setCamera(Qt3DRender::QCamera *camera);
    float lookSpeed() const;
    void setLookSpeed(float lookSpeed);
    float linearSpeed() const;
    void setLinearSpeed(float linearSpeed);

private:
    bool m_leftButtonPressed;
    bool m_keyUpPressed;

    float m_dx;
    float m_dy;
    float m_scale;
    float m_lookSpeed;
    float m_linearSpeed;

    Qt3DInput::QMouseDevice *m_mouseDevice;
    Qt3DInput::QKeyboardDevice *m_keyboardDevice;
    Qt3DInput::QLogicalDevice *m_logicalDevice;

    Qt3DInput::QAction *m_keyUpAction;
    Qt3DInput::QAction *m_leftButtonAction;
    Qt3DInput::QActionInput *m_keyUpInput;
    Qt3DInput::QActionInput *m_leftButtonInput;
    Qt3DInput::QAxis *m_xAxis;
    Qt3DInput::QAxis *m_yAxis;
    Qt3DInput::QAxis *m_wheelAxis;
    Qt3DInput::QAnalogAxisInput *m_mouseXInput;
    Qt3DInput::QAnalogAxisInput *m_mouseYInput;
    Qt3DInput::QAnalogAxisInput *m_wheelAxisInput;

    Qt3DLogic::QFrameAction *m_frameAction;

    Qt3DRender::QCamera *m_camera;

protected slots:
    void frameActionTriggered(float dt);
};

#endif // CAMERACONTROLLER_H
