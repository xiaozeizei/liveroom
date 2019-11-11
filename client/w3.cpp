#include "w3.h"
#include "ui_w3.h"
#include <stdlib.h>
#include <QWaitCondition>
#include <QMessageBox>

//extern int id_global;

W3::W3(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::W3)
{
    ui->setupUi(this);
    m_room_id = 0;


    m_camera=new QCamera;//摄像头
    m_viewfinder=new QCameraViewfinder(this);
    m_imageCapture=new QCameraImageCapture(m_camera);//截图部件

    ui->layouto->setGeometry(QRect(50,50,300,300));

}

W3::~W3()
{
    delete ui;
}

W3 &W3::get_instance()
{
    static W3 instance;
    return instance;
}

void W3::start_camera_selfie()
{
    ui->layouto->addWidget(m_viewfinder);
    m_camera->setViewfinder(m_viewfinder);
    m_camera->start();
}

void W3::being_caster()
{
    m_is_caster = true;
}

void W3::being_audience()
{
    m_is_caster = false;
}

void W3::closeEvent(QCloseEvent *event)
{
    QCloseEvent* slience = event;
    event = slience;



    //断开直播
    if(m_is_caster)
    {
        W3::get_instance().get_thread().exit();
        if(!W3::get_instance().get_thread().wait(1))
        {
            W3::get_instance().get_thread().terminate();
            W3::get_instance().get_thread().wait();
        }
    }

    //告诉UDP 爷走了， 把爷删了
    Udp_pro updu;
    memset(&updu, 0, sizeof(Udp_pro));
    updu.id = W1::get_instance().get_id();
    updu.is_getout = true;
    W3::get_instance().send_udp_to_server(&updu);

    //告诉服务器，登出
    Protocol pdu;
    memset(&pdu, 0 ,sizeof(pdu));
    pdu.msg_type = QUIT_ROOM_TYPE;
    pdu.id = W1::get_instance().get_id();
    pdu.result = m_is_caster;
    pdu.room_id = m_room_id;
    pdu.isevent = true;
    W1::get_instance().get_socket_tcp().write((char*)&pdu, sizeof(pdu));

    //QWaitCondition wait;
    //wait.wait(1);
    //std::usleep(5000);
    //登出
    W1::get_instance().logout_fun();
    //向服务器发送离开room

}

int W3::get_room_id()
{
    return m_room_id;
}

void W3::set_room_id(int id)
{
    m_room_id = id;
}

bool W3::get_is_caster()
{
    return m_is_caster;
}

QCamera *W3::get_camera()
{
    return m_camera;
}

void W3::add_chat_text(QString data)
{
    ui->msg_tb->append(data);
}

void W3::clear_chat_text()
{
    ui->msg_tb->clear();
}

QCameraViewfinder *W3::get_viewfinder()
{
    return m_viewfinder;
}

QString W3::get_room_name()
{
    return m_room_name;
}

void W3::set_room_name(QString room_name)
{
    m_room_name = room_name;
}

void W3::add_room_name_test(QString room_name)
{
    ui->msg_tb->append(QString("欢迎来到")+room_name+QString("的直播间"));
}

void W3::send_udp_to_server(Udp_pro* updu)
{
    //qDebug() << "send"<< updu->id;
    Udp_socket::get_instance().get_udp_socket().writeDatagram((char*)(updu), sizeof(Udp_pro),
                                                              QHostAddress(IP_ADDRESS), QString(UDPPORT).toUShort());
}

My_thread &W3::get_thread()
{
    return m_thread;
}

My_thread_audience &W3::get_thread_audience()
{
    return m_thread_audience;
}

My_thread_audience_read &W3::get_thread_audience_read()
{
    return m_thread_audience_read;
}

void W3::show_live_data(Udp_pro& updu)
{
    add_chat_text(QString("%1").arg(updu.id));
}

void W3::on_quit_pb_clicked()
{
    //向服务器发送离开room
    Protocol pdu;
    memset(&pdu, 0 ,sizeof(pdu));
    pdu.msg_type = QUIT_ROOM_TYPE;
    pdu.id = W1::get_instance().get_id();
    pdu.result = m_is_caster;
    pdu.room_id = m_room_id;
    pdu.isevent = false;
    W1::get_instance().get_socket_tcp().write((char*)&pdu, sizeof(pdu));



    //杀死线程
    if(m_is_caster)
    {
        W3::get_instance().get_thread().exit();
        if(!W3::get_instance().get_thread().wait(1))
        {
            W3::get_instance().get_thread().terminate();
            W3::get_instance().get_thread().wait();
        }
    }
    //告诉UDP 爷走了， 把爷删了
    Udp_pro updu;
    memset(&updu, 0, sizeof(Udp_pro));
    updu.id = W1::get_instance().get_id();
    updu.is_getout = true;
    W3::get_instance().send_udp_to_server(&updu);

    W3::get_instance().clear_chat_text();

}

void W3::on_send_pb_clicked()
{
    QString msg = ui->msg_le->text();
    if(msg.size() > 100)
    {
        QMessageBox::information(this, "发送", "请输入少于100字节");
    }

    Protocol pdu;
    memset(&pdu, 0 ,sizeof(pdu));
    pdu.msg_type = GROUP_CHAT_TYPE;
    pdu.id = W1::get_instance().get_id();
    pdu.result = m_is_caster;
    pdu.room_id = m_room_id;
    strcpy(pdu.username, W1::get_instance().get_username());
    strcpy(pdu.data, msg.toStdString().c_str());

    W1::get_instance().get_socket_tcp().write((char*)&pdu, sizeof(pdu));
    ui->msg_le->clear();
}

void W3::on_rocket_pb_clicked()
{
    Protocol pdu;
    memset(&pdu, 0 ,sizeof(pdu));
    pdu.msg_type = SEND_ROCKET_TYPE;
    pdu.id = W1::get_instance().get_id();
    pdu.result = m_is_caster;
    pdu.room_id = m_room_id;
    strcpy(pdu.username, W1::get_instance().get_username());

    W1::get_instance().get_socket_tcp().write((char*)&pdu, sizeof(pdu));
}
