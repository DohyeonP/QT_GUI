#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket> // ADD_DH
//#include <QTextStream> // ADD_DH message 를 handling 하기 위한 library

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void temperature_add_point(double x, double y); // ADD_DH
    void humidity_add_point(double x, double y); // ADD_DH
    void temperature_clear_data(); // ADD_DH
    void humidity_clear_data(); // ADD_DH
    void temperature_plot(); // ADD_DH
    void humidity_plot(); // ADD_DH

public slots: // call back function public slots --> QT 에서 정의된 기능
    void readyRead();

private slots:
    void on_pushButtonSend_clicked();

    void on_checkBoxLED1_stateChanged(int arg1);

    void on_checkBoxLED2_stateChanged(int arg1);

    void on_checkBoxLED3_stateChanged(int arg1);

    void on_dialServo_valueChanged(int value);

    void on_pushButtonClear_clicked();

    void on_horizontalSliderDHT11Interval_valueChanged(int value);

    void on_dialLED_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    QUdpSocket *socket = nullptr; // ADD_DH
    QVector <double> temperature_qv_x, temperature_qv_y; // ADD_DH
    QVector <double> humidity_qv_x, humidity_qv_y; // ADD_DH
    QString temperature; // ADD_DH
    QString humidity; // ADD_DH

    int temperature_time; // ADD_DH
    int humidity_time; // ADD_DH
};

#endif // MAINWINDOW_H
