#ifndef WINDOW_H
#define WINDOW_H

#include "qpen.h"
#include <QWidget>
#include <QPen>

namespace Ui { class Window; }

class Window : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor panelButtonColor WRITE setPanelButtonColor)
public:
    enum WindowFlag
    {
        Default       = 0,
        MainWindow    = 1,
        DeleteOnClose = 2,
        Modal         = 4,
        NoHidden      = 8,
        NoScreenMode  = 16,
        FixedSize     = 32
    };
    Q_DECLARE_FLAGS(WindowFlags, WindowFlag);
    Window(QWidget*, const QString& = QString(), WindowFlags = Default);
    ~Window();
    void setTitle(const QString&);
    void setIcon(const QPixmap &pixmap);
    void setIcon(const QString &path);
    void setSize(const QSize&);
    void setSize(const int, const int);
    void setPanelButtonColor(const QColor&);
    void showMaximum(const bool);
    void setStyleFromFile(const QString&);

    QString title() const;
    QPixmap icon() const;
    QPen panelPen() const;
    //QColor getPanelButtonColor() const;

private:
    Ui::Window *ui;
    QWidget* m_widget;
    QRect m_normalGeometry;
    QPen m_panelPen;
    //QColor m_panelButtonColor;
    bool eventFilter(QObject*, QEvent*);

signals:
    void finished();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Window::WindowFlags);

#endif // WINDOW_H
