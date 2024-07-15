#include "ONSFactory.h"
#include "ONSClientException.h"

#include <iostream>
#include <thread>
#include <mutex>

using namespace ons;
std::mutex console_mtx;

int main0()
{

    //创建Producer，并配置发送消息所必需的信息。
    ONSFactoryProperty factoryInfo;
    //您在消息队列RocketMQ版控制台创建的Group ID。
    factoryInfo.setFactoryProperty(ONSFactoryProperty::ProducerId, "CID_ONSAPI_OWNER");
    //设置TCP接入域名，进入消息队列RocketMQ版控制台实例详情页面的接入点区域查看。
    factoryInfo.setFactoryProperty(ONSFactoryProperty::NAMESRV_ADDR, "192.168.0.55:9876");
    // 您在消息队列RocketMQ版控制台创建的Topic。
    factoryInfo.setFactoryProperty(ONSFactoryProperty::PublishTopics, "spiecs");
    //消息内容。
    factoryInfo.setFactoryProperty(ONSFactoryProperty::MsgContent, "XXX");
    //请确保环境变量ALIBABA_CLOUD_ACCESS_KEY_ID、ALIBABA_CLOUD_ACCESS_KEY_SECRET已设置。
    //AccessKey ID，阿里云身份验证标识。
    //factoryInfo.setFactoryProperty(ONSFactoryProperty::AccessKey, getenv("ALIBABA_CLOUD_ACCESS_KEY_ID"));
    //AccessKey Secret，阿里云身份验证密钥。
    //factoryInfo.setFactoryProperty(ONSFactoryProperty::SecretKey, getenv("ALIBABA_CLOUD_ACCESS_KEY_SECRET"));


    //create producer;
    Producer* pProducer = ONSFactory::getInstance()->createProducer(factoryInfo);

    //在发送消息前，必须调用start方法来启动Producer，只需调用一次即可。
    pProducer->start();

    Message msg(
        //普通消息所属的Topic，切勿使用普通消息的Topic来收发其他类型的消息。
        //factoryInfo.getPublishTopics(),
        "spiecs",
        //Message Tag，可理解为Gmail中的标签，对消息进行再归类，方便Consumer指定过滤条件在消息队列RocketMQ版的服务器过滤。       
        "*",
        //Message Body，不能为空，消息队列RocketMQ版不做任何干预，需要Producer与Consumer协商好一致的序列化和反序列化方式。
        //factoryInfo.getMessageContent()
        "            This is a common Message             "
    );

    // 设置代表消息的业务关键属性，请尽可能全局唯一。
    // 以方便您在无法正常收到消息情况下，可通过消息队列RocketMQ版控制台查询消息并补发。
    // 注意：不设置也不会影响消息正常收发。
    msg.setKey("ORDERID_100");

    //发送消息，只要不抛出异常，就代表发送成功。     
    try
    {
           SendResultONS sendResult = pProducer->send(msg);
        
    }
    catch (ONSClientException& e)
    {
        //自定义处理exception的细节。
    }
    // 在应用退出前，必须销毁Producer对象，否则会导致内存泄露等问题。
    pProducer->shutdown();

    return 0;
}

class ExampleMessageListener : public MessageListener {
public:
    Action consume(Message& message, ConsumeContext& context) {
        //此处为具体的消息处理过程，确认消息被处理成功请返回CommitMessage。
        //如果有消费异常，或者期望重新消费，可以返回ReconsumeLater，消息将会在一段时间后重新投递。
        std::lock_guard<std::mutex> lk(console_mtx);
        std::cout << "Received a message. Topic: " << message.getTopic() << ", MsgId: "
            << message.getMsgID() << message.getBody() << std::endl;
        return CommitMessage;
    }
};

int main1() {
    std::cout << "=======Before consuming messages=======" << std::endl;
    ONSFactoryProperty factoryInfo;
    //请填写在阿里云消息队列RocketMQ版控制台上申请的Group ID，从实例化的版本开始，ProducerId和CounsumerId已经统一，此处设置是为了接口保持向前兼容。
    factoryInfo.setFactoryProperty(ONSFactoryProperty::ConsumerId, "j");
    //请确保环境变量ALIBABA_CLOUD_ACCESS_KEY_ID、ALIBABA_CLOUD_ACCESS_KEY_SECRET已设置。
    //AccessKey ID，阿里云身份验证标识。
    //factoryInfo.setFactoryProperty(ONSFactoryProperty::AccessKey, getenv("ALIBABA_CLOUD_ACCESS_KEY_ID"));
    //AccessKey Secret，阿里云身份验证密钥。
    //factoryInfo.setFactoryProperty(ONSFactoryProperty::SecretKey, getenv("ALIBABA_CLOUD_ACCESS_KEY_SECRET"));
    //请填写阿里云消息队列RocketMQ版控制台上对应实例的接入点。
    factoryInfo.setFactoryProperty(ONSFactoryProperty::NAMESRV_ADDR, "183.6.118.24:9876");

    PushConsumer* consumer = ONSFactory::getInstance()->createPushConsumer(factoryInfo);

    //请填写阿里云消息队列RocketMQ版控制台上申请的Topic。
    const char* topic_1 = "ztdThermometryData";
    // 订阅topic-1中Tag消息属性为tag-1的所有消息。
    const char* tag_1 = "*";

    //请注册自定义侦听函数用来处理接收到的消息，并返回响应的处理结果。
    ExampleMessageListener* message_listener = new ExampleMessageListener();
    consumer->subscribe(topic_1, tag_1, message_listener);
    //consumer->subscribe(topic_2, tag_2, message_listener);

    //准备工作完成，必须调用启动函数，才可以正常工作。
    consumer->start();

    //请保持线程常驻，不要执行shutdown操作。
    //std::this_thread::sleep_for(std::chrono::milliseconds(60 * 1000));
    while (1) {}
    consumer->shutdown();
    delete message_listener;
    std::cout << "=======After consuming messages======" << std::endl;
    return 0;
}

int main()
{

    //std::thread t0(main0);
    std::thread t1(main1);

    //t0.join();
    t1.join();
}

