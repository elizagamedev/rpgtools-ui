#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QProcess>
#include <QString>

#define TOOL(tool) private slots: void tool##Clicked(); private: QPushButton *tool##Btn; QString tool##Path;

class QDir;
class QPushButton;
class QProgressDialog;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QDir &tooldir);

private slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processCanceled();

private:
    void startProcess(const QString &name, const QStringList &args);

    QProcess *process;
    QProgressDialog *progress;
    bool processWasCanceled;

    TOOL(mapdump)
    TOOL(rpgconv)
    TOOL(xyz)
    TOOL(_2k2xp)
};

#endif // MAINWINDOW_H
