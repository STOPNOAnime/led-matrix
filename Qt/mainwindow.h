#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QTimer>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_connect_button_clicked();
    void on_send_button_clicked();
    void recived_data();
    void send_frame();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QTimer *timer;

    QString display_string;
    uint16_t framebuffer[8];
    uint current_frame = 0;
    int current_string_pos = 0;

    bool displaying_string = false;
};
#endif // MAINWINDOW_H
