#include "ONSFactory.h"
#include "ONSClientException.h"

#include <iostream>
#include <thread>
#include <mutex>

using namespace ons;
std::mutex console_mtx;

int main0()
{

    //����Producer�������÷�����Ϣ���������Ϣ��
    ONSFactoryProperty factoryInfo;
    //������Ϣ����RocketMQ�����̨������Group ID��
    factoryInfo.setFactoryProperty(ONSFactoryProperty::ProducerId, "CID_ONSAPI_OWNER");
    //����TCP����������������Ϣ����RocketMQ�����̨ʵ������ҳ��Ľ��������鿴��
    factoryInfo.setFactoryProperty(ONSFactoryProperty::NAMESRV_ADDR, "192.168.0.55:9876");
    // ������Ϣ����RocketMQ�����̨������Topic��
    factoryInfo.setFactoryProperty(ONSFactoryProperty::PublishTopics, "spiecs");
    //��Ϣ���ݡ�
    factoryInfo.setFactoryProperty(ONSFactoryProperty::MsgContent, "XXX");
    //��ȷ����������ALIBABA_CLOUD_ACCESS_KEY_ID��ALIBABA_CLOUD_ACCESS_KEY_SECRET�����á�
    //AccessKey ID�������������֤��ʶ��
    //factoryInfo.setFactoryProperty(ONSFactoryProperty::AccessKey, getenv("ALIBABA_CLOUD_ACCESS_KEY_ID"));
    //AccessKey Secret�������������֤��Կ��
    //factoryInfo.setFactoryProperty(ONSFactoryProperty::SecretKey, getenv("ALIBABA_CLOUD_ACCESS_KEY_SECRET"));


    //create producer;
    Producer* pProducer = ONSFactory::getInstance()->createProducer(factoryInfo);

    //�ڷ�����Ϣǰ���������start����������Producer��ֻ�����һ�μ��ɡ�
    pProducer->start();

    Message msg(
        //��ͨ��Ϣ������Topic������ʹ����ͨ��Ϣ��Topic���շ��������͵���Ϣ��
        //factoryInfo.getPublishTopics(),
        "spiecs",
        //Message Tag�������ΪGmail�еı�ǩ������Ϣ�����ٹ��࣬����Consumerָ��������������Ϣ����RocketMQ��ķ��������ˡ�       
        "*",
        //Message Body������Ϊ�գ���Ϣ����RocketMQ�治���κθ�Ԥ����ҪProducer��ConsumerЭ�̺�һ�µ����л��ͷ����л���ʽ��
        //factoryInfo.getMessageContent()
        "            This is a common Message             "
    );

    // ���ô�����Ϣ��ҵ��ؼ����ԣ��뾡����ȫ��Ψһ��
    // �Է��������޷������յ���Ϣ����£���ͨ����Ϣ����RocketMQ�����̨��ѯ��Ϣ��������
    // ע�⣺������Ҳ����Ӱ����Ϣ�����շ���
    msg.setKey("ORDERID_100");

    //������Ϣ��ֻҪ���׳��쳣���ʹ����ͳɹ���     
    try
    {
           SendResultONS sendResult = pProducer->send(msg);
        
    }
    catch (ONSClientException& e)
    {
        //�Զ��崦��exception��ϸ�ڡ�
    }
    // ��Ӧ���˳�ǰ����������Producer���󣬷���ᵼ���ڴ�й¶�����⡣
    pProducer->shutdown();

    return 0;
}

class ExampleMessageListener : public MessageListener {
public:
    Action consume(Message& message, ConsumeContext& context) {
        //�˴�Ϊ�������Ϣ������̣�ȷ����Ϣ������ɹ��뷵��CommitMessage��
        //����������쳣�����������������ѣ����Է���ReconsumeLater����Ϣ������һ��ʱ�������Ͷ�ݡ�
        std::lock_guard<std::mutex> lk(console_mtx);
        std::cout << "Received a message. Topic: " << message.getTopic() << ", MsgId: "
            << message.getMsgID() << message.getBody() << std::endl;
        return CommitMessage;
    }
};

int main1() {
    std::cout << "=======Before consuming messages=======" << std::endl;
    ONSFactoryProperty factoryInfo;
    //����д�ڰ�������Ϣ����RocketMQ�����̨�������Group ID����ʵ�����İ汾��ʼ��ProducerId��CounsumerId�Ѿ�ͳһ���˴�������Ϊ�˽ӿڱ�����ǰ���ݡ�
    factoryInfo.setFactoryProperty(ONSFactoryProperty::ConsumerId, "j");
    //��ȷ����������ALIBABA_CLOUD_ACCESS_KEY_ID��ALIBABA_CLOUD_ACCESS_KEY_SECRET�����á�
    //AccessKey ID�������������֤��ʶ��
    //factoryInfo.setFactoryProperty(ONSFactoryProperty::AccessKey, getenv("ALIBABA_CLOUD_ACCESS_KEY_ID"));
    //AccessKey Secret�������������֤��Կ��
    //factoryInfo.setFactoryProperty(ONSFactoryProperty::SecretKey, getenv("ALIBABA_CLOUD_ACCESS_KEY_SECRET"));
    //����д��������Ϣ����RocketMQ�����̨�϶�Ӧʵ���Ľ���㡣
    factoryInfo.setFactoryProperty(ONSFactoryProperty::NAMESRV_ADDR, "183.6.118.24:9876");

    PushConsumer* consumer = ONSFactory::getInstance()->createPushConsumer(factoryInfo);

    //����д��������Ϣ����RocketMQ�����̨�������Topic��
    const char* topic_1 = "ztdThermometryData";
    // ����topic-1��Tag��Ϣ����Ϊtag-1��������Ϣ��
    const char* tag_1 = "*";

    //��ע���Զ���������������������յ�����Ϣ����������Ӧ�Ĵ�������
    ExampleMessageListener* message_listener = new ExampleMessageListener();
    consumer->subscribe(topic_1, tag_1, message_listener);
    //consumer->subscribe(topic_2, tag_2, message_listener);

    //׼��������ɣ�������������������ſ�������������
    consumer->start();

    //�뱣���̳߳�פ����Ҫִ��shutdown������
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

