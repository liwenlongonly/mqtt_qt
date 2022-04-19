//
// Created by ilong on 2022/4/19.
//

#include "mainwindow.h"
#include <QGridLayout>
#include "log.h"
#include <QThread>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent):
QMainWindow(parent),
isConnected_{false},
isSubscribe_(false){
    this->resize(800, 400);
    this->setWindowTitle("MQTT");

    connetBtn = new QPushButton(this);
    connetBtn->setGeometry(350, 0, 100, 40);
    connetBtn->setText("connect");
    connect(connetBtn, SIGNAL(clicked()), this, SLOT(connetButtonClicked()));
    connect(this, SIGNAL(recvMsg(QString)), this, SLOT(changeTextEdit(QString)));

    mainWidget = new  QWidget(this);
    mainWidget->setGeometry(0, 40, 800, 360);

    QGridLayout *gridLayout = new QGridLayout();

    publishBtn = new QPushButton("publish");
    connect(publishBtn, SIGNAL(clicked()), this, SLOT(publishBtnClicked()));
    publishLineText = new QLineEdit();

    QBoxLayout *boxLayout = new QVBoxLayout();
    boxLayout->addWidget(publishLineText);
    boxLayout->addWidget(publishBtn);

    Qt::HANDLE handle =  QThread::currentThreadId();
    Log(DEBUG, "Main thread id: %d", (int *)handle);

    gridLayout->addLayout(boxLayout, 0, 0);

    publishTextEdit = new QTextEdit();
    gridLayout->addWidget(publishTextEdit, 0, 1);

    subscribeBtn = new QPushButton("subscribe");
    connect(subscribeBtn, SIGNAL(clicked()), this, SLOT(subscribeBtnClicked()));
    subscribeLineText = new QLineEdit();

    QBoxLayout *boxLayout1 = new QVBoxLayout();
    boxLayout1->addWidget(subscribeLineText);
    boxLayout1->addWidget(subscribeBtn);

    gridLayout->addLayout(boxLayout1, 1, 0);

    subscribeTextEdit = new QTextEdit();
    subscribeTextEdit->setEnabled(false);

    gridLayout->addWidget(subscribeTextEdit, 1, 1);

    /* 设置第 0 列与第 1 列的列比例系数 */
    gridLayout->setColumnStretch(0, 2);
    gridLayout->setColumnStretch(1, 3);

    mainWidget->setLayout(gridLayout);

    mqttClientWrapper_ = std::make_shared<MQTTClientWrapper>();

}

MainWindow::~MainWindow() noexcept {

}

void MainWindow::onDisconnect(std::string &cause){

}

void MainWindow::onRecvMsg(MQTTClient_message *message){
    std::string msg((char *)message->payload);
    QString payload = QString::fromStdString(msg);
    emit recvMsg(payload);
}

void MainWindow::onDelivered(int token){

}

void MainWindow::changeTextEdit(QString msg) {
    subscribeTextEdit->append(msg);
}

void MainWindow::connetButtonClicked() {
    Log(DEBUG, "MainWindow::connetButtonClicked()");
    if(mqttClientWrapper_){
        mqttClientWrapper_->setObserver(shared_from_this());
        if(isConnected_){
            mqttClientWrapper_->disconnect(1000);
            connetBtn->setText("connect");
        } else{
            mqttClientWrapper_->connect();
            connetBtn->setText("disconnect");
        }
        isConnected_ = !isConnected_;
    }
}

void MainWindow::publishBtnClicked() {
    if(!isConnected_){
        QMessageBox *msgBox;
        msgBox = new QMessageBox("title",		///--这里是设置消息框标题
                                 "请先链接",						///--这里是设置消息框显示的内容
                                 QMessageBox::Warning,							///--这里是在消息框显示的图标
                                 QMessageBox::Cancel | QMessageBox::Default,		///---这里是显示消息框上的按钮情况
                                 QMessageBox::NoButton,	///---这里与 键盘上的 escape 键结合。当用户按下该键，消息框将执行cancel按钮事件
                                 QMessageBox::NoButton);														///---这里是 定义第三个按钮， 该例子 只是 了显示2个按钮

        msgBox->show();
        return;
    }
    Log(DEBUG, "MainWindow::publishBtnClicked()");
    QString topic = publishLineText->text();
    QString payload = publishTextEdit->toPlainText();
    if(mqttClientWrapper_){
        std::string topicStr = topic.toStdString();
        std::string payloadStr = payload.toStdString();
        mqttClientWrapper_->publishMsg(topicStr, payloadStr, 1, 1);
    }
}

void MainWindow::subscribeBtnClicked() {
    if(!isConnected_){
        QMessageBox *msgBox;
        msgBox = new QMessageBox("title",		///--这里是设置消息框标题
                                 "请先链接",						///--这里是设置消息框显示的内容
                                 QMessageBox::Critical,							///--这里是在消息框显示的图标
                                 QMessageBox::Cancel | QMessageBox::Default,		///---这里是显示消息框上的按钮情况
                                 QMessageBox::NoButton,	///---这里与 键盘上的 escape 键结合。当用户按下该键，消息框将执行cancel按钮事件
                                 QMessageBox::NoButton);														///---这里是 定义第三个按钮， 该例子 只是 了显示2个按钮

        msgBox->show();
        return;
    }
    Log(DEBUG, "MainWindow::subscribeBtnClicked()");
    if(mqttClientWrapper_){
        QString topic = subscribeLineText->text();
        std::string topicStr = topic.toStdString();
        if(!isSubscribe_){
            mqttClientWrapper_->subscribe(topicStr, 1);
            subscribeBtn->setText("unSubscribe");
        } else{
            mqttClientWrapper_->unSubscribe(topicStr);
            subscribeBtn->setText("subscribe");
        }
        isSubscribe_ = !isSubscribe_;
    }
}