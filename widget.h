#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDir>

namespace Ui {
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
	void on_pushButton_clicked();

private:
    Ui::Widget *ui;

    void CreateMissionDir(QString missionFileName, QString CamDir);
    void CreateCutsceneDir(QString missionFileName, QString CamDir);
    void CreateOverviewHTML(QDir file, QString CamDir);
    QString injectMissionDigit(QString missionClassName, int i);
};

#endif // WIDGET_H
