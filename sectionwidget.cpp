#include "sectionwidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QPropertyAnimation>

SectionWidget::SectionWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addStretch();
    setLayout(layout);
}

void SectionWidget::addTab(const QString &text, QWidget *widget)
{
    QVBoxLayout *l = static_cast<QVBoxLayout*>(layout());
    l->insertWidget(l->count() - 1, new SectionWidgetTab(text, widget));
    l->insertWidget(l->count() - 1, widget);
}

void SectionWidget::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

// вкладка
SectionWidgetTab::SectionWidgetTab(const QString &text, QWidget *content) : QPushButton(text)
{
    setCheckable(true);
    setChecked(true);
    setIconSize(QSize(12, 12));

    QPropertyAnimation *anim = new QPropertyAnimation(content, "size", this);
    anim->setDuration(100);
    connect(anim, &QAbstractAnimation::finished, this, [content, this]
    {
        if (!isChecked())
        {
            content->hide();
        }
    });

    connect(this, &QPushButton::toggled, this, [content, anim](const bool show)
    {
        if (show)
        {
            content->show();
            anim->setStartValue(QSize(content->width(), 0));
            anim->setEndValue(content->size());
        }
        else
        {
            anim->setStartValue(content->size());
            anim->setEndValue(QSize(content->width(), 0));
        }
        anim->start();
    });
}

void SectionWidgetTab::paintEvent(QPaintEvent*)
{
    const qreal left = 15, top = 2, right = 10, bottom = 0;
    const QRect rect(left, top, width() - left - right, height() - top - bottom);

    QStylePainter painter(this);
    QStyleOptionButton option;
    initStyleOption(&option);
    painter.drawControl(QStyle::CE_PushButtonBevel, option);
    painter.drawPrimitive(QStyle::PE_FrameFocusRect, option);
    painter.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text());
    QPixmap icon = QIcon(isChecked() ? ":/images/downArrow.svg" : ":/images/rightArrow.svg").pixmap(iconSize());
    painter.drawItemPixmap(rect, Qt::AlignRight | Qt::AlignVCenter, icon);
}
