/*
在centos7下运行
gtk版本：3.22
0.1beta版
*/

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "connection.h"
#include "msg.h"
/* 数据变量 */
enum{
    ID_COLUMN,
    TEXT_COLUMN,
    N_COLUMNS
};
typedef struct _treeitem TreeItem;
struct _treeitem{
    gint id;
    gchar text[2048];
};
/* 一些全局变量 */
gchar title[50] = "聊天窗口";
static GtkWidget *fwindow = NULL; //主窗口
static GtkWidget *dwindow = NULL; //对话窗口
static GtkWidget *text;
static GtkTextBuffer *buffer;
static GtkWidget *message_entry;
static GtkWidget *username_entry,*password_entry,*check_password_entry;
gint sd;
gchar target[20],me[20];//目标，用户，内容 
msg msgdata;

/* 群组和用户数据 */
TreeItem group[20];
TreeItem member[20];
/* 一些功能函数 */
void closeApp(GtkWidget *window, gpointer data); //关闭应用
void login_button_clicked(GtkWidget *button, gpointer data);  //登录按键功能函数
void signin_button_clicked(GtkWidget *button, gpointer data); //注册按键功能函数
void signin_button_click(GtkWidget *button, gpointer data);
void showWin(GtkWidget *window);    //显示窗口
// void msendmsg(GtkWidget *window, gpointer data);  //发送信息
// void get_message(void);   //从服务器获取信息
void returnApp(GtkWidget *window, gpointer data);  //返回登陆界面
void tree_selection_changed(GtkTreeSelection *selection, gpointer data); //选项功能控件
int query_list(gint func);  // 查询用户或群组,func=1表示查询群组,func=2表示查询用户
void chat_with_group(gpointer data);   // 与群组进行交流
void chat_with_user(gpointer data);   // 与用户进行交流
void delete_group(gpointer data);   // 删除群组
void quit_group(gpointer data);   //退出群组
void delete_user(gpointer data);   // 删除用户
void Creategroup(gpointer data);   // 创建群聊
void Searchgroup(gpointer data);   // 搜索群聊
void grouphistory(void);  // 群组历史消息
void userhistory(void);  // 用户历史消息
GtkWidget *CreateMenuItem(GtkWidget *MenuBar,char *test);  // 创建目录项
GtkWidget *CreateMenu(GtkWidget *MenuItem);   // 创建目录条
GtkWidget *login_win(GtkWidget *window);  // 登录窗口
GtkWidget *main_win(GtkWidget *window);   // 主窗口
GtkWidget *createTreeView(GtkWidget *treeview,TreeItem *t,gint length,gint type); // 创建一个树形列表
GtkWidget *msgDialog(char *data,gint type); // 询问与当前点击项目有关的信息
void *ansDialog(char *data); // 显示从服务器返回的信息

/* 注册界面以及相关操作 */
GtkWidget *signin_win(GtkWidget *window){
    GtkWidget *username_label;
    GtkWidget *password_label,*password_label1;
    GtkWidget *signin_button;
    GtkWidget *hbox1,*hbox2,*hbox3,*vbox;
    gtk_init(NULL,NULL);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(window,"注册窗口");
    gtk_window_set_position(window,GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(window,FALSE);
    g_signal_connect(window,"destroy",closeApp,NULL);
    username_label = gtk_label_new("用 户 名：");
    password_label = gtk_label_new("密    码：");
    password_label1 = gtk_label_new("确定密码：");
    username_entry = gtk_entry_new();
    password_entry = gtk_entry_new();
    check_password_entry = gtk_entry_new();
    gtk_entry_set_visibility(password_entry,FALSE);
    gtk_entry_set_visibility(check_password_entry,FALSE);
    signin_button = gtk_button_new_with_label("注册");
    g_signal_connect(signin_button,"clicked",signin_button_click,window);

    hbox1 = gtk_hbox_new(TRUE,5);
    hbox2 = gtk_hbox_new(TRUE,5);
    hbox3 = gtk_hbox_new(TRUE,5);
    vbox = gtk_vbox_new(FALSE,10);
    gtk_box_pack_start(hbox1,username_label,TRUE,FALSE,5);
    gtk_box_pack_start(hbox1,username_entry,TRUE,FALSE,5);

    gtk_box_pack_start(hbox2,password_label,TRUE,FALSE,5);
    gtk_box_pack_start(hbox2,password_entry,TRUE,FALSE,5);
    
    gtk_box_pack_start(hbox3,password_label1,TRUE,FALSE,5);
    gtk_box_pack_start(hbox3,check_password_entry,TRUE,FALSE,0);

    gtk_box_pack_start(vbox,hbox1,FALSE,FALSE,5);
    gtk_box_pack_start(vbox,hbox2,FALSE,FALSE,5);
    gtk_box_pack_start(vbox,hbox3,FALSE,FALSE,5);
    gtk_box_pack_start(vbox,signin_button,FALSE,FALSE,5);

    gtk_container_add(window,vbox);
    if (!gtk_widget_get_visible (window)){
        gtk_widget_show(window);
        gtk_widget_show_all (window);
    }else
        gtk_widget_destroy (window);
    return window;
}
void signin_button_click(GtkWidget *window,gpointer data){
    const char *password_text = gtk_entry_get_text(password_entry);
    const char *check_text = gtk_entry_get_text(check_password_entry);
    if(strcmp(password_text,check_text) == 0){
        const char *username_text = gtk_entry_get_text(username_entry);
        if(registe(sd,username_text,password_text)){
            receivemsg(sd,&msgdata);
            if(!strcmp(msgdata.buf,"创建成功")){
                printf("Signin granted!\n");
                ansDialog("注册成功!");
                gtk_widget_destroy(data);
                fwindow = login_win(fwindow);
                showWin(fwindow);
            }else{
                gchar wrong[100] = "注册失败!问题是：";
                strcat(wrong,msgdata.buf);
                ansDialog(wrong);
            }
        }
    }else{
        printf("Please Check your input!\n");
        ansDialog("请检查你的密码输入是否正确!");
    }
}

/* 登录界面以及相关操作 */
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
    g_signal_connect(signin_button,"clicked",signin_button_clicked,window);
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
    gtk_box_pack_start(vbox,signin_button,FALSE,FALSE,5);

    gtk_container_add(window,vbox);
    // Generak purpose modle
    if (!gtk_widget_get_visible (window)){
        gtk_widget_show(window);
        gtk_widget_show_all (window);
        
    }else
        gtk_widget_destroy (window);
    return window;
}
void login_button_clicked(GtkWidget *window, gpointer data){
    const char *password_text = gtk_entry_get_text(password_entry);
    const char *username_text = gtk_entry_get_text(username_entry);
    if(login(sd,username_text,password_text)){
        receivemsg(sd,&msgdata);
        if(!strcmp(msgdata.buf,"success")){
            printf("Access granted!\n");
            memset(me,0,sizeof(gchar));
            strcpy(me,username_text);
            gtk_widget_destroy(data);
            // gdk_threads_enter();
            // islogined = TRUE;
            fwindow = main_win(fwindow);
            showWin(fwindow);
            // gdk_threads_leave();
        }else{
            printf("Access denied!\n");
            ansDialog(msgdata.buf);
        } 
    }else{
        ansDialog("与服务器连接失败!");
    }
}
void signin_button_clicked(GtkWidget *window, gpointer data){
    gtk_widget_destroy(data);
    fwindow = signin_win(fwindow);
    showWin(fwindow);
}

/* 主窗口 */
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
    GtkWidget *treeView1,*treeView2;
    int length;
    /*
        main window
    */
    // title = "聊天窗口";
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
    length = query_list(1);
    treeView1 = createTreeView(treeView1,group,length,1);
    gtk_container_add(frame2,treeView1);
    /*
        friends frame
    */
    frame3 = gtk_frame_new("您的好友：");
    gtk_box_pack_start(vbox2,frame3,FALSE,FALSE,5);
    gtk_widget_set_size_request(frame3,200,250);
    length = query_list(2);
    treeView2 = createTreeView(treeView2,member,length,2);
    gtk_container_add(frame3,treeView2);
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
    gtk_widget_show_all(window);
    gtk_main();
}
void closeApp(GtkWidget *window, gpointer data){
    if(data != NULL){
        gtk_widget_destroy(data);
    }else{
        gtk_main_quit();
    }
}
void returnApp(GtkWidget *window, gpointer data){
    gtk_widget_destroy(fwindow);
    fwindow = login_win(fwindow);
    showWin(fwindow);
}
GtkWidget *CreateMenuItem(GtkWidget *MenuBar,char *test){
    GtkWidget *MenuItem;
    MenuItem = gtk_menu_item_new_with_label(test);
    gtk_menu_shell_append(MenuBar,MenuItem);
    return MenuItem;
}
GtkWidget *CreateMenu(GtkWidget *MenuItem){
    GtkWidget *Menu;
    GtkWidget *exit,*creategroup,*searchgroup;
    Menu = gtk_menu_new();
    exit = CreateMenuItem(Menu,"退出登录");
    creategroup = CreateMenuItem(Menu,"创建群聊");
    searchgroup = CreateMenuItem(Menu,"搜索群聊");
    g_signal_connect(exit,"activate",returnApp,fwindow);
    gtk_menu_item_set_submenu(MenuItem,Menu);
}
GtkWidget *createTreeView(GtkWidget *treeview,TreeItem t[],gint length,gint type){
    GtkListStore *store;
    GtkTreeIter *iter;
    GtkCellRenderer *renderer;
    GtkTreeSelection *select;
    GtkTreeViewColumn *column;
    gint i;
    store = gtk_list_store_new(N_COLUMNS,G_TYPE_INT,G_TYPE_STRING);
    for(i=0;i<length;i++){
        gtk_list_store_append(store,&iter);
        gtk_list_store_set(store,&iter,
                            ID_COLUMN,t[i].id,
                            TEXT_COLUMN,t[i].text,-1);
    }
    treeview = gtk_tree_view_new_with_model(store);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
                                                "ID",renderer,
                                                "text",ID_COLUMN,
                                                NULL);
    gtk_tree_view_append_column(treeview,column);
    renderer = gtk_cell_renderer_text_new();
    
    column = gtk_tree_view_column_new_with_attributes(
                                                "NAME",renderer,
                                                "text",TEXT_COLUMN,
                                                NULL);
    gtk_tree_view_append_column(treeview,column);
    select = gtk_tree_view_get_selection(treeview);
    gtk_tree_selection_set_mode(select,GTK_SELECTION_BROWSE);
    g_signal_connect(select,"changed",tree_selection_changed,type);
    return treeview;
}
void tree_selection_changed(GtkTreeSelection *selection, gpointer data){
    GtkTreeIter iter;
    GtkTreeModel *model;
    gchar *title;

    if(gtk_tree_selection_get_selected(selection,&model,&iter)){
        gtk_tree_model_get(model,&iter,TEXT_COLUMN,&title,-1);
        printf("你选择了:%s\n",title);
        fwindow = msgDialog(title,data);
        showWin(fwindow);
        g_free(title);
    }
}

GtkWidget *msgDialog(char *data,gint type){
    GtkWidget *window;
    GtkWidget *hbox1,*hbox2,*vbox;
    GtkWidget *btn1,*btn2;
    GtkWidget *label;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(window,"请选择您的操作");
    gtk_window_set_default_size(window,280,175);
    gtk_window_set_position(window,GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(window,FALSE);
    g_signal_connect(window,"destroy",closeApp,NULL);

    vbox = gtk_vbox_new(FALSE,10);
    hbox1 = gtk_hbox_new(TRUE,5);
    hbox2 = gtk_hbox_new(TRUE,5);
    btn1 = gtk_button_new_with_label("聊天");
    printf("%s\n",me);
    memset(target,0,sizeof(gchar));
    strcpy(target,data);
    if(type == 1){
        g_signal_connect(btn1,"clicked",chat_with_group,window);
    }else if(type == 2){
        g_signal_connect(btn1,"clicked",chat_with_user,window);
    }
    btn2 = gtk_button_new_with_label("删除");
    if(type == 1){
        g_signal_connect(btn2,"clicked",delete_group,NULL);
    }else if(type == 2){
        g_signal_connect(btn2,"clicked",delete_user,NULL);
    }
    gchar str1[100] = "请选择您要对";
    gchar *str2 = "的操作";
    strcat(str1,data);
    strcat(str1,str2);
    label = gtk_label_new(str1);

    gtk_box_pack_start(hbox1,label,TRUE,FALSE,5);
    gtk_box_pack_start(hbox2,btn1,TRUE,FALSE,5);
    gtk_box_pack_start(hbox2,btn2,TRUE,FALSE,5);
    if(type == 1){
        GtkWidget *btn3;
        btn3 = gtk_button_new_with_label("退群");
        g_signal_connect(btn3,"clicked",quit_group,NULL);
        gtk_box_pack_start(hbox2,btn3,TRUE,FALSE,5);
    }
    gtk_box_pack_start(vbox,hbox1,FALSE,FALSE,5);
    gtk_box_pack_start(vbox,hbox2,FALSE,FALSE,5);
    gtk_container_add(window,vbox);
    if (!gtk_widget_get_visible (window)){
        gtk_widget_show(window);
        gtk_widget_show_all (window);
    }else
        gtk_widget_destroy (window);
    return window;
}
void grouphistory(void){ // 群组历史消息
    GtkTextIter iter;
    gchar get_buf[2048];
    while(queryhistoryfromgroup(sd,me,target)){
        receivemsg(sd,&msgdata);
        sprintf(get_buf,"%s",msgdata.buf);
        gdk_threads_enter();
        gtk_text_buffer_get_end_iter(buffer,&iter);
        gtk_text_buffer_insert(buffer,&iter,get_buf,-1);
        gdk_threads_leave();
    }
}
void userhistory(void){  // 用户历史消息
    GtkTextIter iter;
    gchar get_buf[2048];
    while(queryhistoryfromuser(sd,me,target)){
        receivemsg(sd,&msgdata);
        sprintf(get_buf,"%s",msgdata.buf);
        gdk_threads_enter();
        gtk_text_buffer_get_end_iter(buffer,&iter);
        gtk_text_buffer_insert(buffer,&iter,get_buf,-1);
        gdk_threads_leave();
    }
}
void chat_with_group(gpointer data){   // 与群组进行交流
    gtk_widget_destroy(data);
    memset(title,0,sizeof(char*));
    strcat(title,"在群");
    strcat(title,target);
    strcat(title,"内进行聊天");
    fwindow = main_win(fwindow);
    showWin(fwindow);
    g_thread_create((GThreadFunc)grouphistory,NULL,FALSE,NULL);
    gchar *message = gtk_entry_get_text(message_entry);
    if(sendtogroup(sd,me,target,message)){
        gtk_entry_set_text(message_entry,"");
    }
}
void chat_with_user(gpointer data){   // 与用户进行交流
    gtk_widget_destroy(data);
    memset(title,0,sizeof(char*));
    strcat(title,"与");
    strcat(title,target);
    strcat(title,"进行聊天");
    fwindow = main_win(fwindow);
    showWin(fwindow);
    g_thread_create((GThreadFunc)userhistory,NULL,FALSE,NULL);
    gchar *message = gtk_entry_get_text(message_entry);
    if(sendtouser(sd,me,target,message)){
        gtk_entry_set_text(message_entry,"");
    }
}
void delete_group(gpointer data){   // 删除或退出群组
    if(deletegroup(sd,me,target)){
        receivemsg(sd,&msgdata);
    }else{
        receivemsg(sd,&msgdata);
    }
    ansDialog(msgdata.buf);
}
void quit_group(gpointer data){    //退出群组
    if(quitgroup(sd,me,target)){
        receivemsg(sd,&msgdata); 
    }else{
        receivemsg(sd,&msgdata);
    }
    ansDialog(msgdata.buf);
}
void delete_user(gpointer data){   // 删除用户
    if(deletefriend(sd,me,target)){
        receivemsg(sd,&msgdata);
    }else{
        receivemsg(sd,&msgdata);
    }
    ansDialog(msgdata.buf);
}
void *ansDialog(char *data){
    GtkWidget *window;
    GtkWidget *label;
    GtkWidget *btn;
    GtkWidget *vbox;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(window,"提示");
    gtk_window_set_default_size(window,150,70);
    gtk_window_set_position(window,GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(window,FALSE);
    g_signal_connect(window,"destroy",closeApp,NULL);
    vbox = gtk_vbox_new(FALSE,10);
    label = gtk_label_new(data);
    btn = gtk_button_new_with_label("确定");
    g_signal_connect(btn,"clicked",closeApp,window);
    gtk_box_pack_start(vbox,label,TRUE,FALSE,5);
    gtk_box_pack_start(vbox,btn,TRUE,FALSE,5);
    gtk_container_add(window,vbox);
    if (!gtk_widget_get_visible (window)){
        gtk_widget_show(window);
        gtk_widget_show_all (window);
    }else
        gtk_widget_destroy (window);
    showWin(window);
}
/* 查询群组或者用户列表 */
int query_list(gint func){
    int i;
    switch(func){
        case 1:
            if(querygrouplist(sd,me)){
                i = 0;
                receivemsg(sd,&msgdata);
                while(msgdata.command != Info){
                    group[i].id = i + 1;
                    strcpy(group[i].text,msgdata.buf);
                    i++;
                    receivemsg(sd,&msgdata);
                }
            }
            break;
        case 2:
            if(queryfriendlist(sd,me)){
                i = 0;
                receivemsg(sd,&msgdata);
                while(msgdata.command != Info){
                    member[i].id = i + 1;
                    strcpy(member[i].text,msgdata.buf); 
                    i++;
                    receivemsg(sd,&msgdata);
                }
            }
            break;   
    }
    return i;
}
int main(int argc,char * argv[]){
    if (argc != 2){
        printf("please input ./client <IPADDRESS>\n");
        return EXIT_SUCCESS;
    }
    if(!g_thread_supported()){
        g_thread_init(NULL);
    }
    sd = getconnection(argv[1]);
    memset(me,0,sizeof(me));
    memset(target,0,sizeof(target));
    memset(member,0,sizeof(member));
    memset(group,0,sizeof(group));
    fwindow = login_win(fwindow);
    showWin(fwindow);
}
