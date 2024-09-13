#ifndef PROGRESSTAB_H
#define PROGRESSTAB_H

#include <QLabel>
#include <QProgressBar>

class ProgressTab : public QWidget
{
    Q_OBJECT
public:
    ProgressTab(QWidget *parent = nullptr);
    void setProgress(const int value);
    void addUnit();
    void start(const int limit);
    void resume();
    void stop();

    int progress() const;

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    const int m_duration;
    bool m_run;
    int m_elapsedTime;
    QLabel *m_countdown;
    QProgressBar *m_progressBar;
};

#endif // PROGRESSTAB_H
