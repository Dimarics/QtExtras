#include "window.h"
#include "ui_window.h"
#include "iconengine.h"

#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QPainter>
#include <QFile>
#include <QStyle>

Window::Window(QWidget *widget, const QString &title, WindowFlags flags) :
    QWidget(nullptr, Qt::Window | Qt::FramelessWindowHint),
    ui(new Ui::Window),
    m_widget(widget),
    //m_panelPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin))
    m_panelPen(Qt::white)
{
    setAttribute(Qt::WA_TranslucentBackground);
    ui->setupUi(this);
    setTitle(title);
    for(QObject *child : children())
        child->installEventFilter(this);
    m_widget->setParent(ui->centralWidget);
    setMinimumSize(m_widget->minimumSize() + QSize(20, 46));

    if (flags & MainWindow)
        connect(ui->quitButton, &QPushButton::clicked, qApp, &QCoreApplication::quit, Qt::QueuedConnection);
    else
        connect(ui->quitButton, &QPushButton::clicked, this, &Window::finished);

    if (flags & DeleteOnClose)
        setAttribute(Qt::WA_DeleteOnClose);

    if (flags & Modal)
        setWindowModality(Qt::ApplicationModal);

    if (flags & NoHidden)
        ui->hideButton->close();
    else
        connect(ui->hideButton, &QPushButton::clicked, this, &QWidget::showMinimized);

    if (flags & NoScreenMode)
    {
        ui->screenModeButton->close();
    }
    else
    {
        connect(ui->screenModeButton, &QPushButton::toggled, this, [this](const bool maximized)
        {
            if (maximized)
            {
                m_normalGeometry = geometry();
                QRect rect = QApplication::primaryScreen()->availableGeometry();
                setGeometry(rect.x() - 10, rect.y() - 10, rect.width() + 20, rect.height() + 20);
                ui->screenModeButton->setToolTip(tr("Свернуть в окно"));
            }
            else
            {
                setGeometry(m_normalGeometry);
                ui->screenModeButton->setToolTip(tr("Развернуть"));
            }
        });
    }
    if (QFile(":/windowStyle.qss").exists()) setStyleFromFile(":/windowStyle.qss");
    emit ui->screenModeButton->toggled(ui->screenModeButton->isChecked());
}

Window::~Window()
{
    delete ui;
}

void Window::setTitle(const QString &title)
{
    ui->title->setText(title);
    //setWindowTitle(title);
}

void Window::setIcon(const QPixmap &pixmap)
{
    ui->icon->setPixmap(pixmap);
}

void Window::setIcon(const QString &path)
{
    setIcon(QPixmap(path));
}

void Window::setSize(const QSize &size)
{
    resize(size + QSize(ui->leftField->width() + ui->rightField->width(),
                        ui->topField->height() + ui->panel->height() + ui->bottomField->height()));
}

void Window::setSize(const int width, const int height)
{
    resize(width + ui->leftField->width() + ui->rightField->width(),
           height + ui->topField->height() + ui->panel->height() + ui->bottomField->height());
}

void Window::setPanelButtonColor(const QColor &color)
{
    m_panelPen.setColor(color);

    IconEngine cross(IconEngine::Cross, ui->quitButton->iconSize() * devicePixelRatio());
    QPen cross_pen(m_panelPen);
    cross_pen.setWidthF(m_panelPen.widthF() * 1.4);
    cross.setPen(cross_pen);
    ui->quitButton->setIcon(QIcon(cross.pixmap(true)));

    IconEngine hide(IconEngine::Hide, ui->hideButton->iconSize() * devicePixelRatio());
    hide.setPen(m_panelPen);
    ui->hideButton->setIcon(QIcon(hide.pixmap()));

    IconEngine windowMode(IconEngine::WindowMode, ui->screenModeButton->iconSize() * devicePixelRatio());
    IconEngine fullScreen(IconEngine::FullScreen, ui->screenModeButton->iconSize() * devicePixelRatio());
    windowMode.setPen(m_panelPen);
    fullScreen.setPen(m_panelPen);
    QIcon windowModeIcon;
    windowModeIcon.addPixmap(windowMode.pixmap(), QIcon::Normal, QIcon::On);
    windowModeIcon.addPixmap(fullScreen.pixmap(), QIcon::Normal, QIcon::Off);
    ui->screenModeButton->setIcon(windowModeIcon);
}

void Window::showMaximum(const bool full)
{
    ui->screenModeButton->setChecked(full);
}

void Window::setStyleFromFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Не удалось открыть файл стилей окна";
        return;
    }
    setStyleSheet(file.readAll());
}

QString Window::title() const
{
    return ui->title->text();
}

QPixmap Window::icon() const
{
    return ui->icon->pixmap();
}

QPen Window::panelPen() const
{
    return m_panelPen;
}

/*QColor Window::getPanelButtonColor() const
{
    return m_panelPen.color();
}*/

bool Window::eventFilter(QObject* target, QEvent* event)
{
    static QPointF mouse_pos;
    static bool mouse_pressed = false;
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

    switch (event->type())
    {
    case QEvent::MouseButtonPress:
        if (mouseEvent->button() == Qt::LeftButton)
        {
            mouse_pressed = true;
            mouse_pos = mouseEvent->pos();
            mouse_pos += target == ui->panel ? QPointF(10, 10) :
                                               target == ui->rightTopField ? QPointF(-10, 0) :
                                                                             target == ui->rightField ? QPointF(-10, 0) :
                                                                                                        target == ui->rightBottomField ? QPointF(-10, -10) :
                                                                                                                                         target == ui->bottomField ? QPointF(0, -10) :
                                                                                                                                                                     target == ui->leftBottomField ? QPointF(0, -10) : QPointF();
        }
        break;

    case QEvent::MouseMove:
        if (mouse_pressed)
        {
            QPointF pos = mouseEvent->globalPosition() - mouse_pos;
            if (target == ui->panel)
            {
                if (ui->screenModeButton->isChecked())
                {
                    mouse_pos.setX(mouse_pos.x() * (m_normalGeometry.width()) / width());
                    pos = mouseEvent->globalPosition() - mouse_pos;
                    ui->screenModeButton->setChecked(false);
                }
                move(pos.x(), pos.y());
            }
            else if (target == ui->leftField && width() + x() - pos.x() >= minimumWidth())
                setGeometry(pos.x(), y(), width() + x() - pos.x(), height());
            else if (target == ui->leftTopField)
            {
                const int _width = width() + x() - pos.x();
                const int _height = height() + y() - pos.y();

                if (_width >= minimumWidth())
                    setGeometry(pos.x(), y(), _width, height());
                if (_height >= minimumHeight())
                    setGeometry(x(), pos.y(), width(), _height);
            }
            else if (target == ui->topField && height() + y() - pos.y() >= minimumHeight())
                setGeometry(x(), pos.y(), width(), height() + y() - pos.y());
            else if (target == ui->rightTopField)
            {
                const int _width = pos.x() - x();
                const int _height = height() + y() - pos.y();

                if (_width >= minimumWidth())
                    resize(_width, height());
                if (_height >= minimumHeight())
                    setGeometry(x(), pos.y(), width(), _height);
            }
            else if (target == ui->rightField && pos.x() - x() >= minimumWidth())
                resize(pos.x() - x(), height());
            else if (target == ui->rightBottomField)
            {
                const int _width = pos.x() - x();
                const int _height = pos.y() - y();

                if (_width >= minimumWidth())
                    resize(_width, height());
                if (_height >= minimumHeight())
                    resize(width(), _height);
            }
            else if (target == ui->bottomField && pos.y() - y() >= minimumHeight())
                resize(width(), pos.y() - y());
            else if (target == ui->leftBottomField)
            {
                const int _width = width() + x() - pos.x();
                const int _height = pos.y() - y();

                if (_width >= minimumWidth())
                    setGeometry(pos.x(), y(), _width, height());
                if (_height >= minimumHeight())
                    setGeometry(x(), y(), width(), _height);
            }
        }
        break;

    case QEvent::Resize:
        if (target == ui->centralWidget) m_widget->resize(ui->centralWidget->size());
        break;

    case QEvent::MouseButtonRelease:
        if (mouseEvent->button() == Qt::LeftButton)
            mouse_pressed = false;
        break;

    default:
        break;
    }
    return QWidget::eventFilter(target, event);
}
