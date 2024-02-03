#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h" // ADD_DH
#include <QDebug> // ADD_DH stdio.h 와 비슷
#include <QString> // ADD_DH c++ 의 string 을 진화시킨 것

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    temperature_time(0),
    humidity_time(0)
{
    ui->setupUi(this);
//-----------------------
// DH Strat

    // 1. socekt 을 생성
    socket = new QUdpSocket(this);
    // 2. IP, SW PORT 번호 등록
    bool result = socket->bind(QHostAddress::AnyIPv4, 9999);
    qDebug() << result; // qDebug --> c++ 의 cout
    if (result)
    {
        qDebug() << "PASS";
    }
    else
    {
        qDebug() << "FAIL";
    }
    // SIGNAL : event 발생
    // SLOT : Inturrupt service routine (함수를 인터럽트 서비스 루틴이라고 한다.)
    // connent : mapping (연결 시켜주는 것)
    // socket 으로 부터 읽을 데이터가 존재하면 나(this)의 readyRead() 를 호출 한다.
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    // temperature
    ui->plotTemperature->setInteraction(QCP::iRangeDrag, true);
    ui->plotTemperature->setInteraction(QCP::iRangeZoom, true);
    ui->plotTemperature->addGraph();
    //ui->plot->xAxis->setLabel("time(s)");
    ui->plotTemperature->yAxis->setLabel("temperature");
    ui->plotTemperature->yAxis->setRange(0.0, 80.0);
    ui->plotTemperature->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle); //점찍는 스타일 결정.
    ui->plotTemperature->graph(0)->setLineStyle(QCPGraph::lsLine); //라인 스타일 결정.
    connect(ui->plotTemperature, SIGNAL(mouseDoubleClickEvent(QMouseEvent*)), SLOT(QMouseEvent*));

    //humidity
    ui->plotHumidity->setInteraction(QCP::iRangeDrag, true);
    ui->plotHumidity->setInteraction(QCP::iRangeZoom, true);
    ui->plotHumidity->addGraph();
    //ui->plot->xAxis->setLabel("time(s)");
    ui->plotHumidity->yAxis->setLabel("humidity");
    ui->plotHumidity->yAxis->setRange(0.0, 60.0);
    ui->plotHumidity->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle); //점찍는 스타일 결정.
    ui->plotHumidity->graph(0)->setLineStyle(QCPGraph::lsLine); //라인 스타일 결정.
    connect(ui->plotHumidity, SIGNAL(mouseDoubleClickEvent(QMouseEvent*)), SLOT(QMouseEvent*));

// DH End
//-----------------------

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readyRead()
{
    QByteArray raw_Buffer;
    QByteArray temperature_Buffer;
    QByteArray humidity_Buffer;
    double temperature_digit; // 온도값
    double humidity_digit; // 습도값

    QHostAddress sender; // 송신자의 IP
    quint16 senderPort; // 송신자의 SW PORT

    raw_Buffer.resize(socket->pendingDatagramSize());

    // 3. socket 으로 부터 data 를 읽어온다.
    socket->readDatagram(raw_Buffer.data(), raw_Buffer.size(), &sender, &senderPort);
    if (raw_Buffer.startsWith("[Wet]"))
    {
        // 온도 출력
        raw_Buffer.chop(1); // [Tmp]25\n 이 왔을 때 \n 을 제거하는 함수 즉, 지정한 숫자만큼 마지막 바이트를 제거
        ui->textEditRxData->append(raw_Buffer); // 창의 맨 끝에 붙여 넣기
        ui->textEditRxData->show(); // display

        // LCD 창에 출력
        temperature_Buffer = raw_Buffer.right(2); // [Tmp]25 가 Buffer 안에 들어있는데 맨 오른쪽부터 지정된 숫자만큼 자른다. 즉, 숫자만 취한다.
        // Buffer 에는 25가 들어 있다. 25는 UTF-8 모드
        temperature_digit = temperature_Buffer.toDouble();
        ui->lcdNumberTemperature->display(temperature_digit);

        qDebug() <<"Message From :: " << sender.toString();
        qDebug() <<"Port From :: "<< senderPort;
        qDebug() <<"Buffer :: " << temperature_Buffer;

        temperature_add_point(temperature_time, temperature_digit);
        temperature_time+=3;
        ui->plotTemperature->xAxis->setRange(0, temperature_time+3);
        temperature_plot();

        // 습도 출력
        raw_Buffer.chop(8);

        // LCD 창에 출력
        humidity_Buffer = raw_Buffer.right(2); // [Tmp]25 가 Buffer 안에 들어있는데 맨 오른쪽부터 지정된 숫자만큼 자른다. 즉, 숫자만 취한다.
        // Buffer 에는 25가 들어 있다. 25는 UTF-8 모드
        humidity_digit = humidity_Buffer.toDouble();
        ui->lcdNumberHumidity->display(humidity_digit);

        qDebug() <<"Message From :: " << sender.toString();
        qDebug() <<"Port From :: "<< senderPort;
        qDebug() <<"Buffer :: " << humidity_Buffer;

        humidity_add_point(humidity_time, humidity_digit);
        humidity_time+=3;
        ui->plotHumidity->xAxis->setRange(0, humidity_time+3);
        humidity_plot();
    }
}

void MainWindow::temperature_add_point(double x, double y){
    temperature_qv_x.append(x);
    temperature_qv_y.append(y);
}

void MainWindow::temperature_clear_data(){
    temperature_qv_x.clear();
    temperature_qv_y.clear();
}

void MainWindow::temperature_plot(){
    ui->plotTemperature->graph(0)->setData(temperature_qv_x, temperature_qv_y);
    ui->plotTemperature->replot();
    ui->plotTemperature->update();
}

void MainWindow::humidity_add_point(double x, double y){
    humidity_qv_x.append(x);
    humidity_qv_y.append(y);
}

void MainWindow::humidity_clear_data(){
    humidity_qv_x.clear();
    humidity_qv_y.clear();
}

void MainWindow::humidity_plot(){
    ui->plotHumidity->graph(0)->setData(humidity_qv_x, humidity_qv_y);
    ui->plotHumidity->replot();
    ui->plotHumidity->update();
}

// 생성 경로 : send 마우스 우클릭 --> goto slot --> click
void MainWindow::on_pushButtonSend_clicked()
{
    QByteArray Data;

    Data = ui->lineEditSendData->text().toUtf8();
    socket->writeDatagram(Data, QHostAddress("10.10.15.84"), 9999);
    // 10.10.15.84 는 STM32 IP Address
}

void MainWindow::on_checkBoxLED1_stateChanged(int arg1)
{
    QString Buffer;
    QByteArray SendData;

    Buffer.sprintf("LED001\n");
    SendData = Buffer.toUtf8(); // ascii 는 utf8 과 값이 동일하므로 변환을 굳이 할 필요가 없다.
    socket->writeDatagram(SendData, QHostAddress("10.10.15.84"), 9999);
}

void MainWindow::on_checkBoxLED2_stateChanged(int arg1)
{
    QString Buffer;
    QByteArray SendData;

    Buffer.sprintf("LED002\n");
    SendData = Buffer.toUtf8(); // ascii 는 utf8 과 값이 동일하므로 변환을 굳이 할 필요가 없다.
    socket->writeDatagram(SendData, QHostAddress("10.10.15.84"), 9999);
}

void MainWindow::on_checkBoxLED3_stateChanged(int arg1)
{
    QString Buffer;
    QByteArray SendData;

    Buffer.sprintf("LED003\n");
    SendData = Buffer.toUtf8(); // ascii 는 utf8 과 값이 동일하므로 변환을 굳이 할 필요가 없다.
    socket->writeDatagram(SendData, QHostAddress("10.10.15.84"), 9999);
}

// SERVO:999
void MainWindow::on_dialServo_valueChanged(int value)
{
    QByteArray servo_data = "SERVO:";

    servo_data.append(QString::number(ui->dialServo->value()));
    ui->lcdNumberServo->display(ui->dialServo->value());
    socket->writeDatagram(servo_data, QHostAddress("10.10.15.84"), 9999);

    qDebug() << "servo data: " << servo_data << endl;
}

void MainWindow::on_pushButtonClear_clicked()
{
    ui->lcdNumberTemperature->display("0");
    ui->lcdNumberHumidity->display("0");

    ui->textEditRxData->clear();
    temperature_clear_data();
    humidity_clear_data();
    temperature_time = 0;
    humidity_time = 0;
}

void MainWindow::on_horizontalSliderDHT11Interval_valueChanged(int value)
{
    QByteArray dht11_interval = "DHT11:";

    dht11_interval.append(QString::number(ui->horizontalSliderDHT11Interval->value()));
    ui->lcdNumberHorizontalSliderDHT11Interval->display(ui->horizontalSliderDHT11Interval->value());
    socket->writeDatagram(dht11_interval, QHostAddress("10.10.15.84"), 9999);

    qDebug() << "DHT11 interval: " << dht11_interval << endl;
}

void MainWindow::on_dialLED_valueChanged(int value)
{
    QByteArray led_pwm = "LED:";

    led_pwm.append(QString::number(ui->dialLED->value()));
    ui->lcdNumberLED->display(ui->dialLED->value());
    socket->writeDatagram(led_pwm, QHostAddress("10.10.15.84"), 9999);

    qDebug() << "LED PWM: " << led_pwm << endl;
}
