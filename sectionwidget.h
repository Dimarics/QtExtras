#ifndef SECTIONWIDGET_H
#define SECTIONWIDGET_H

#include <QPushButton>

class SectionWidget : public QWidget
{
    Q_OBJECT
public:
    SectionWidget(QWidget* = nullptr);
    void addTab(const QString&, QWidget*);
    void paintEvent(QPaintEvent*) override;
};

class SectionWidgetTab : public QPushButton
{
    Q_OBJECT
public:
    SectionWidgetTab(const QString&, QWidget*);
    void paintEvent(QPaintEvent*) override;
};

#endif // SECTIONWIDGET_H
