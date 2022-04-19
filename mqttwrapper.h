//
// Created by ilong on 2022/4/19.
//

#ifndef MQTT_QT_MQTTWRAPPER_H
#define MQTT_QT_MQTTWRAPPER_H

#include <string>
#include <MQTTClient.h>

class MQTTClientObserver{
public:
    virtual void onDisconnect(std::string &cause) = 0;
    virtual void onRecvMsg(MQTTClient_message *message) = 0;
    virtual void onDelivered(int token) = 0;
};

class MQTTClientWrapper{
public:
    MQTTClientWrapper();
    ~MQTTClientWrapper();

    void setObserver(std::shared_ptr<MQTTClientObserver> observer);

    int connect();

    int disconnect(int timeout);

    int publishMsg(std::string &topicName, std::string &payload, int qos, int retained);

    int subscribe(std::string &topicName, int qos);

    int unSubscribe(std::string &topicName);

public:
    std::weak_ptr<MQTTClientObserver> observer_;
private:

    MQTTClient client_;
};

#endif //MQTT_QT_MQTTWRAPPER_H
