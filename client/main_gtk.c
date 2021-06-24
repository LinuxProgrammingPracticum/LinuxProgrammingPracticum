/*
在centos7下运行
gtk版本：3.22
0.1beta版
*/

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#define OURPORT 8088 //测试用端口
/* 一些全局变量 */
const char *password = "passwd";
static GtkWidget *fwindow = NULL;
static GtkWidget *text;
static GtkTextBuffer *buffer;
static GtkWidget *message_entry;
static GtkWidget *username_entry,*password_entry;
struct sockaddr_in s_in;
gboolean islogined = FALSE;
gchar username[64],buf[1024];
gint sd; 

/* 一些功能函数 */
void closeApp(GtkWidget *window, gpointer data); //关闭应用
void login_button_clicked(GtkWidget *button, gpointer data);  //登录按键功能函数
void signin_button_clicked(GtkWidget *button, gpointer data); //注册按键功能函数
void showWin(GtkWidget *window);    //显示窗口
void sendMsg(GtkWidget *window, gpointer data);  //发送信息
void get_message(void);   //从服务器获取信息
void returnApp(GtkWidget *window, gpointer data);  //返回登陆界面
GtkWidget *CreateMenuItem(GtkWidget *MenuBar,char *test);  //创建目录项
GtkWidget *CreateMenu(GtkWidget *MenuItem);   //创建目录条
//测试用与服务器连接函数
gboolean do_connect(void){
    GtkTextIter iter; //一个访问gtktextbuffer的变量
    gint slen;
    sd = socket(AF_INET,SOCK_STREAM,0);
    if(sd < 0){
        gtk_text_buffer_get_end_iter(buffer,&iter); //对gtktextbuffer进行修改的变量
        gtk_text_buffer_insert(buffer,&iter,"打开套接字时出错!\n",-1);
        return FALSE;
    }
    s_in.sin_family = AF_INET;
    s_in.sin_port = OURPORT;
    slen = sizeof(s_in);
    if(connect(sd,&s_in,slen) < 0){
        gtk_text_buffer_get_end_iter(buffer,&iter);
        gtk_text_buffer_insert(buffer,&iter,"连接服务器时出错！\n",-1);
        return FALSE;
    }else{
        gtk_text_buffer_get_end_iter(buffer,&iter);
        gtk_text_buffer_insert(buffer,&iter,username,-1);
        gtk_text_buffer_get_end_iter(buffer,&iter);
        gtk_text_buffer_insert(buffer,&iter,"\n成功与服务器连接\n",-1);
        write(sd,username,64);
        islogined = TRUE;
        return TRUE;
    }
}


GtkWidget *login_win(GtkWidget *window){
    GtkWidget *username_label;
    GtkWidget *password_label;
    GtkWidget *login_button,*signin_button;
    GtkWidget *hbox1,*hbox2,*vbox;

    gtk_init(NULL,NULL);
    // main window setting
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(window,"登录窗口");
    gtk_window_set_position(window,GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(window,FALSE);
    g_signal_connect(window,"destroy",closeApp,NULL);
    // component setting
    username_label = gtk_label_new("用户名：");
    password_label = gtk_label_new("密  码：");
    username_entry = gtk_entry_new();
    password_entry = gtk_entry_new();
    gtk_entry_set_visibility(password_entry,FALSE);
    
    login_button = gtk_button_new_with_label("登录");
    signin_button = gtk_button_new_with_label("注册");
    g_signal_connect(login_button,"clicked",login_button_clicked,window);

    hbox1 = gtk_hbox_new(TRUE,5);
    hbox2 = gtk_hbox_new(TRUE,5);
    vbox = gtk_vbox_new(FALSE,10);
    gtk_box_pack_start(hbox1,username_label,TRUE,FALSE,5);
    gtk_box_pack_start(hbox1,username_entry,TRUE,FALSE,5);
    gtk_box_pack_start(hbox2,password_label,TRUE,FALSE,5);
    gtk_box_pack_start(hbox2,password_entry,TRUE,FALSE,5);

    gtk_box_pack_start(vbox,hbox1,FALSE,FALSE,5);
    gtk_box_pack_start(vbox,hbox2,FALSE,FALSE,5);
    gtk_box_pack_start(vbox,login_button,FALSE,FALSE,5);

    gtk_container_add(window,vbox);
    // Generak purpose modle
    if (!gtk_widget_get_visible (window)){
        gtk_widget_show(window);
        gtk_widget_show_all (window);
        
    }else
        gtk_widget_destroy (window);
    return window;
}
GtkWidget *main_win(GtkWidget *window){
    GtkWidget *frame1,*frame2,*frame3;
    GtkWidget *vbox1,*vbox2,*vbox3;
    GtkWidget *hbox1,*hbox;
    GtkWidget *box,*mbox;
    GtkWidget *button;
    GtkWidget *msg[10];
    GtkWidget *menubar,*menuFile,*submenu;
    GtkWidget *menuQuit,*menuChange;
    GtkWidget *view;
    /*
        main window
    */
    char *title = "聊天窗口";
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(window,title);
    gtk_window_set_default_size(window,700,550);
    gtk_window_set_position(window,GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(window,FALSE);
    // gtk_container_set_border_width(window,20);
    g_signal_connect(window,"destroy",closeApp,NULL);
    mbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_container_add(window,mbox);
    /*
        toolmenu
    */ 
    vbox3 = gtk_vbox_new(FALSE,0);
    gtk_container_add(mbox,vbox3);
    menubar = gtk_menu_bar_new();
    gtk_box_pack_start(vbox3,menubar,FALSE,FALSE,0);
    // menuFile = gtk_menu_item_new_with_label("菜单");
    // gtk_menu_shell_append(menubar,menuFile);
    menuFile = CreateMenuItem(menubar,"菜单");
    /*
        submenu
    */
    CreateMenu(menuFile);
    /* 
        message main box 
    */
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_container_add(mbox,box);
    // gtk_box_pack_start(window,box,FALSE,FALSE,0);
    /* 
        left box 
    */
    vbox1 = gtk_vbox_new(FALSE,0);
    gtk_container_add(box,vbox1);
    /* 
        message display box
    */    
    frame1 = gtk_frame_new("消息记录：");
    gtk_box_pack_start(vbox1,frame1,FALSE,FALSE,5);
    gtk_widget_set_size_request(frame1,500,400);
    gtk_container_set_border_width(frame1,10);
    // hbox1 = gtk_hbox_new(FALSE,0);
    view = gtk_scrolled_window_new(NULL,NULL);
    gtk_scrolled_window_set_policy(view,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
    text = gtk_text_view_new();
    // gtk_box_pack_start(frame1,view,TRUE,TRUE,5);
    gtk_container_add(frame1,view);
    gtk_container_add(view,text);
    buffer = gtk_text_view_get_buffer(text);
    /* 
        input box
    */
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_box_pack_start(vbox1,hbox,FALSE,FALSE,5);
    gtk_container_set_border_width(hbox,10);
    // gtk_container_add(vbox1,hbox);
    message_entry = gtk_entry_new();
    gtk_box_pack_start(hbox,message_entry,FALSE,FALSE,0);
    gtk_widget_set_size_request(message_entry,400,25);
    
    button = gtk_button_new_with_label("确定发送");
    gtk_box_pack_start(hbox,button,FALSE,FALSE,5);  
    g_signal_connect(button,"clicked",sendMsg,NULL);
    /*
        right box
    */
    vbox2 = gtk_vbox_new(FALSE,0);
    gtk_container_add(box,vbox2);
    gtk_container_set_border_width(vbox2,10);
    /*
        qun zu frame
    */
    frame2 = gtk_frame_new("您的群组：");
    gtk_box_pack_start(vbox2,frame2,FALSE,FALSE,5);
    gtk_widget_set_size_request(frame2,200,250);

    /*
        friends frame
    */
    frame3 = gtk_frame_new("您的好友：");
    gtk_box_pack_start(vbox2,frame3,FALSE,FALSE,5);
    gtk_widget_set_size_request(frame3,200,250);

    /* Generak purpose modle */
    if (!gtk_widget_get_visible (window)){
        gtk_widget_show(window);
        gtk_widget_show_all (window);
    }else
        gtk_widget_destroy (window);
    return window; 
}
void showWin(GtkWidget *window){
    gtk_init(NULL,NULL);
    gtk_widget_show(window);
    gdk_threads_enter();
    gtk_main();
    gdk_threads_leave();
}
void closeApp(GtkWidget *window, gpointer data){
    gtk_main_quit();
}
void login_button_clicked(GtkWidget *window, gpointer data){
    const char *password_text = gtk_entry_get_text(password_entry);
    const char *username_text = gtk_entry_get_text(username_entry);
    if(strcmp(password_text,password) == 0){
        printf("Access granted!\n");
        sprintf(username,"%s",username_text);
        gtk_widget_destroy(data);
        gdk_threads_enter();
        islogined = TRUE;
        fwindow = main_win(fwindow);
        showWin(fwindow);
        gdk_threads_leave();
    }else{
        printf("Access denied!\n");
    }
    
}
void sendMsg(GtkWidget *window, gpointer data){
    const char *message;
    if(islogined == FALSE) return ;
    message = gtk_entry_get_text(message_entry);
    sprintf(buf,"%s\n",message);
    write(sd,buf,1024);
    gtk_entry_set_text(message_entry,"");
}
void get_message(void){
    GtkTextIter iter;
    gchar get_buf[1024];
    gchar buf[1024];
    while(read(sd,buf,1024) != -1){
        sprintf(get_buf,"%s",buf);
        gdk_threads_enter();    //进入 
        gtk_text_buffer_get_end_iter(buffer,&iter);
        gtk_text_buffer_insert(buffer,&iter,get_buf,-1);    //插入数据

        gdk_threads_leave(); //离开
    }
}
void returnApp(GtkWidget *window, gpointer data){
    gtk_widget_destroy(data);
    fwindow = login_win(fwindow);
    showWin(fwindow);
}
GtkWidget *CreateMenuItem(GtkWidget *MenuBar,char *test){
    GtkWidget *MenuItem;
    MenuItem = gtk_menu_item_new_with_label(test);
    gtk_menu_shell_append(MenuBar,MenuItem);
    // gtk_widget_show(MenuItem);
    return MenuItem;
}
GtkWidget *CreateMenu(GtkWidget *MenuItem){
    GtkWidget *Menu;
    GtkWidget *exit,*change;
    Menu = gtk_menu_new();
    CreateMenuItem(Menu,"修改密码");
    exit = CreateMenuItem(Menu,"退出登录");
    g_signal_connect(exit,"activate",returnApp,fwindow);
    gtk_menu_item_set_submenu(MenuItem,Menu);
    // gtk_widget_show(Menu);
}
int main(){
    fwindow = login_win(fwindow);
    showWin(fwindow);
}