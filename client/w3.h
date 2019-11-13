#ifndef W3_H
#define W3_H

#include <QWidget>
#include "w2.h"


#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QCameraInfo>
#include <QList>
#include <QDebug>
#include <QImage>
#include "udp_socket.h"
#include "udp_socket_audience.h"
#include <my_thread.h>
//#include <my_thread_audience.h>
//#include <my_thread_audience_read.h>
#include <QPixmap>
#include <QByteArray>
#include <QBuffer>

namespace Ui {
class W3;
}

class W3 : public QWidget
{
    Q_OBJECT

signals:
    void please_stop_capture();
    void go_on_process_picpic();
public slots:
    void when_captured(int id, QImage image);
public:
    Udp_pro m_pdup;
    QPixmap* m_pic_pic;
    QByteArray* m_bytearray;
    QBuffer* m_buffer;
    bool is_capture_done;

    explicit W3(QWidget *parent = nullptr);
    ~W3();
    static W3 &get_instance();
    void start_camera_selfie();

    void being_caster();
    void being_audience();
    virtual void closeEvent(QCloseEvent *event);
    int get_room_id();
    void set_room_id(int id);
    bool get_is_caster();
    QCamera* get_camera();
    void add_chat_text(QString data);
    void clear_chat_text();
    QCameraViewfinder* get_viewfinder();
    QString get_room_name();
    void set_room_name(QString room_name);
    void add_room_name_test(QString room_name);

    void send_udp_to_server(Udp_pro* updu);
    My_thread* get_thread();
    //My_thread_audience& get_thread_audience();
    //My_thread_audience_read& get_thread_audience_read();
    void show_live_data(QByteArray b);
    void get_now_pic();
    void modify_updu_to_have_picdata(Udp_pro& updu);
    void better_go_out();
    void pic_pic_handler_and_send();
private slots:
    void on_quit_pb_clicked();

    void on_send_pb_clicked();

    void on_rocket_pb_clicked();




private:
    Ui::W3 *ui;
    QCamera* m_camera;
    QCameraViewfinder* m_viewfinder;
    QCameraImageCapture* m_imageCapture;
    bool m_is_caster;
    int m_room_id;//房间主播id
    QString m_room_name;
    My_thread* m_thread;
    //My_thread_audience m_thread_audience;
    //My_thread_audience_read m_thread_audience_read;

};

#endif // W3_H
