//
// Created by ilong on 2022/4/19.
//

#include "mqttwrapper.h"
#include "log.h"

#define SERVER_URL "tcp://101.32.7.128:1883"

static void onDelivered(void *context, MQTTClient_deliveryToken dt) {
    Log(DEBUG,"Message with token value %d delivery confirmed", dt);
    MQTTClientWrapper *client = static_cast<MQTTClientWrapper*>(context);
    if(client){
        if(auto observer = client->observer_.lock()){
            observer->onDelivered(dt);
        }
    }
}

static int onMsgArrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message){
    Log(DEBUG,"Message arrived");
    Log(DEBUG,"topic: %s", topicName);
    Log(DEBUG,"message: <%d>%s", message->payloadlen, (char *)message->payload);
    MQTTClientWrapper *client = static_cast<MQTTClientWrapper*>(context);
    if(client){
        if(auto observer = client->observer_.lock()){
            observer->onRecvMsg(message);
        }
    }
    // 释放内存
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

static void onConnLost(void *context, char *cause) {
    Log(DEBUG,"\nConnection lost\n");
    Log(DEBUG," cause: %s", cause);
    MQTTClientWrapper *client = static_cast<MQTTClientWrapper*>(context);
    if(client){
        if(auto observer = client->observer_.lock()){
            std::string str(cause);
            observer->onDisconnect(str);
        }
    }
}

MQTTClientWrapper::MQTTClientWrapper(){

    int rc = MQTTCLIENT_SUCCESS;
    /* 创建 mqtt 客户端对象 */
    if (MQTTCLIENT_SUCCESS !=
        (rc = MQTTClient_create(&client_,
                                SERVER_URL,
                                "dt_mqtt_2_id",
                                MQTTCLIENT_PERSISTENCE_NONE,
                                NULL))) {
        Log(ERROR,"Failed to create client, return code %d", rc);
    }

    /* 设置回调 */
    if (MQTTCLIENT_SUCCESS !=
        (rc = MQTTClient_setCallbacks(client_,
                                      this,
                                      onConnLost,
                                      onMsgArrvd,
                                      onDelivered))) {
        Log(ERROR,"Failed to set callbacks, return code %d", rc);
    }
}

MQTTClientWrapper::~MQTTClientWrapper(){
    MQTTClient_destroy(&client_);
}

void MQTTClientWrapper::setObserver(std::shared_ptr<MQTTClientObserver> observer){
    observer_ = observer;
}

int MQTTClientWrapper::connect(){
    int rc = MQTTCLIENT_SUCCESS;
    MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;
    /* 连接服务器 */
    will_opts.topicName = "dt2914/willTopic";
    will_opts.message = "Abnormally dropped";
    will_opts.retained = 1;
    will_opts.qos = 0;

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.will = &will_opts;
    conn_opts.keepAliveInterval = 30;
    conn_opts.cleansession = 0;
    conn_opts.username = "ilong";
    conn_opts.password = "881014";

    if (MQTTCLIENT_SUCCESS != (rc = MQTTClient_connect(client_, &conn_opts))) {
        Log(ERROR,"Failed to connect, return code %d", rc);
        return EXIT_FAILURE;
    }
}

int MQTTClientWrapper::disconnect(int timeout){
    int rc = MQTTCLIENT_SUCCESS;
    if (MQTTCLIENT_SUCCESS != (rc = MQTTClient_disconnect(client_, timeout))) {
        Log(ERROR,"Failed to connect, return code %d", rc);
        return EXIT_FAILURE;
    }
}

int MQTTClientWrapper::publishMsg(std::string & topicName,
                                  std::string &payload,
                                  int qos,
                                  int retained){
    int rc = MQTTCLIENT_SUCCESS;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    /* 发布消息 */
    pubmsg.payload = (void *) payload.c_str(); //消息内容
    pubmsg.payloadlen = payload.length(); //消息的长度
    pubmsg.qos = qos; //QoS 等级
    pubmsg.retained = retained; //消息的保留标志

    if (MQTTCLIENT_SUCCESS !=
        (rc = MQTTClient_publishMessage(client_,
                                        topicName.c_str(),
                                        &pubmsg,
                                        &token))) {
        Log(ERROR,"Failed to publish message, return code %d", rc);
        return EXIT_FAILURE;
    }
}

int MQTTClientWrapper::subscribe(std::string &topicName, int qos){
    int rc = MQTTCLIENT_SUCCESS;
    if (MQTTCLIENT_SUCCESS !=
        (rc = MQTTClient_subscribe(client_, topicName.c_str(), qos))) {
        Log(ERROR,"Failed to subscribe, return code %d\n", rc);
        return EXIT_FAILURE;
    }
}

int MQTTClientWrapper::unSubscribe(std::string &topicName){
    int rc = MQTTCLIENT_SUCCESS;
    if (MQTTCLIENT_SUCCESS !=
        (rc =  MQTTClient_unsubscribe(client_, topicName.c_str()))) {
        Log(ERROR,"Failed to subscribe, return code %d\n", rc);
        return EXIT_FAILURE;
    }
}
