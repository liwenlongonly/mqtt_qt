//
// Created by ilong on 2022/4/19.
//

#ifndef MQTT_QT_MAINWINDOW_H
#define MQTT_QT_MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include "mqttwrapper.h"

class MainWindow : public QMainWindow,
                   public std::enable_shared_from_this<MQTTClientObserver>,
                   public MQTTClientObserver{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

    void onDisconnect(std::string &cause);
    void onRecvMsg(MQTTClient_message *message);
    void onDelivered(int token);

    signals:
    void recvMsg(QString msg);

public slots:
    void changeTextEdit(QString msg);
    void connetButtonClicked();
    void publishBtnClicked();
    void subscribeBtnClicked();

private:
    QPushButton * connetBtn;

    QWidget* mainWidget;

    QPushButton * publishBtn;
    QLineEdit *publishLineText;
    QTextEdit *publishTextEdit;

    QPushButton * subscribeBtn;
    QLineEdit *subscribeLineText;
    QTextEdit *subscribeTextEdit;

    std::shared_ptr<MQTTClientWrapper> mqttClientWrapper_;

    bool isConnected_;
    bool isSubscribe_;
};

#endif //MQTT_QT_MAINWINDOW_H
