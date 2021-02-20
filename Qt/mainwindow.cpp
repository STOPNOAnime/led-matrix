#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "font8x8_basic.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->send_button->setEnabled(false);
    ui->string_input->setEnabled(false);

    serial = new QSerialPort(this);
    serial->setBaudRate(115200);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::recived_data);
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        ui->serial_port_list->addItem(serialPortInfo.portName());
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(send_frame()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::recived_data()
{
    QByteArray data = serial->readAll();
    for(int i=0;i<data.size();i++){
        if(data.at(i)!=0x00) {
            ui->statusbar->showMessage("Error in comuniction.");
            serial->close();

            timer->stop();

            ui->send_button->setText("Send String");
            ui->send_button->setEnabled(false);
            ui->string_input->setEnabled(false);
            ui->connect_button->setEnabled(true);
            ui->serial_port_list->setEnabled(true);
        }
    }
}


void MainWindow::on_connect_button_clicked()
{
    serial->close();
    serial->setPortName(ui->serial_port_list->currentText());

    if(!serial->open(QIODevice::ReadWrite)) {
        ui->statusbar->showMessage("Error opening serial port.");
    }
    else {
        ui->statusbar->showMessage("Sucesfully opened serial port.");
        ui->send_button->setEnabled(true);
        ui->string_input->setEnabled(true);
    }
}

void MainWindow::on_send_button_clicked()
{
    if(displaying_string == false){
        displaying_string = true;
        display_string = ui->string_input->text();
        display_string = display_string.trimmed();
        display_string.append(" ");

        for(int i=0;i<8;i++) framebuffer[i] = 0;
        current_string_pos = 0;

        timer->start(40);

        ui->send_button->setText("Stop Sending");
        ui->connect_button->setEnabled(false);
        ui->serial_port_list->setEnabled(false);
    }
    else{
        displaying_string = false;

        timer->stop();

        ui->send_button->setText("Send String");
        ui->connect_button->setEnabled(true);
        ui->serial_port_list->setEnabled(true);
    }
}

void MainWindow::send_frame(){
    if(current_frame%3 == 0) for(int i=0;i<8;i++) framebuffer[i] = framebuffer[i] >> 1;

    if(current_frame == 23){ //Next Char
        for(int i=0;i<8;i++){
            framebuffer[i] = framebuffer[i] | (font8x8_basic[display_string.at(current_string_pos).unicode()][i]<<8);
        }
        current_string_pos++;
        if(current_string_pos>=display_string.size()) current_string_pos = 0;
    }

    QByteArray data;
    data.append(0x55);
    for(int i=0;i<8;i++) data.append(framebuffer[i] & 0xFF);
    serial->write(data);

    current_frame++;
    if(current_frame == 24) current_frame = 0;
}
