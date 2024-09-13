#include "progresstab.h"

#include <QTime>
#include <QHBoxLayout>

ProgressTab::ProgressTab(QWidget *parent) : QWidget(parent),
    m_duration(1000),
    m_run(false),
    m_elapsedTime(0),
    m_countdown(new QLabel("00:00:00")),
    m_progressBar(new QProgressBar)
{
    QLabel *label = new QLabel("Оставшееся время:");
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_countdown->setStyleSheet("padding-right: 10px");
    m_countdown->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_progressBar->setTextVisible(false);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(10);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);
    layout->addWidget(m_countdown);
    layout->addWidget(m_progressBar);
    setLayout(layout);
    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    startTimer(m_duration);
}

void ProgressTab::setProgress(const int value)
{
    m_progressBar->setValue(value);
}

void ProgressTab::addUnit()
{
    m_progressBar->setValue(m_progressBar->value() + 1);
}

void ProgressTab::start(const int limit)
{
    m_run = true;
    m_elapsedTime = 0;
    m_progressBar->setMaximum(limit);
    setProgress(0);
}

void ProgressTab::resume()
{
    m_run = true;
}

void ProgressTab::stop()
{
    m_run = false;
}

int ProgressTab::progress() const
{
    return m_progressBar->value();
}

void ProgressTab::timerEvent(QTimerEvent*)
{
    if (m_run && progress() != 0)
    {
        m_elapsedTime += m_duration;
        int remainingTime = m_elapsedTime * (m_progressBar->maximum() - progress()) / progress();
        m_countdown->setText(QTime(0, 0).addMSecs(remainingTime).toString("hh:mm:ss"));
    }
}
