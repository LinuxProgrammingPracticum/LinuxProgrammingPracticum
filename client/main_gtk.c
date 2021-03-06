#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
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
gchar title[50] = "聊天窗口";  //标题
static GtkWidget *fwindow = NULL; //主窗口
static GtkWidget *dwindow = NULL; //对话窗口 
static GtkTextBuffer *buffer,*hbuffer;
static GtkWidget *username_entry,*password_entry,*check_password_entry;
GtkWidget *treeView1,*treeView2;
gint sd,kind = 0;
gchar target[20],me[20];//目标,用户
gchar hname[100]; //历史消息文件名
msg msgdata;
gchar ipaddr[20]; // ip地址
/* 群组和用户数据 */
TreeItem group[20];
TreeItem member[20];
/* 一些功能函数 */
void closeApp(GtkWidget *window, gpointer data); //关闭应用
void login_button_clicked(GtkWidget *button, gpointer data);  //登录按键功能函数
void signin_button_clicked(GtkWidget *button, gpointer data); //注册按键功能函数
void signin_button_click(GtkWidget *button, gpointer data);
void showWin(GtkWidget *window);    //显示窗口
void returnApp(GtkWidget *window, gpointer data);  //返回登陆界面
void tree_selection_changed(GtkTreeSelection *selection, gpointer data); //选项功能控件
int query_list(gint func);  // 查询用户或群组,func=1表示查询群组,func=2表示查询用户
void chat_with_group(GtkWidget *button,gpointer data);   // 与群组进行交流
void chat_with_user(GtkWidget *button,gpointer data);   // 与用户进行交流
void send_user(GtkWidget *button, gpointer data);   // 与群组聊天时发送消息
void send_group(GtkWidget *button, gpointer data);     // 与用户聊天时发送消息
void delete_group(GtkWidget *button, gpointer data);   // 删除群组
void quit_group(GtkWidget *button, gpointer data);   //退出群组
void delete_user(GtkWidget *button, gpointer data);   // 删除用户
void Creategroup(GtkWidget *button, gpointer data);   // 创建群聊
void Search(GtkWidget *button, gpointer data);   // 加好友/群
gchar *grouphistory(void);  // 群组历史消息
gchar *userhistory(void);  // 用户历史消息
void groupchat(void);  // 群组实时聊天
void userchat(void);  // 用户实时聊天
void ansDialog(char *data); // 显示从服务器返回的信息
void create_win(void);   // 创建群聊
void search_win(void);   // 加好友/群
void change(GtkWidget *window, gpointer data);
GtkWidget *CreateMenuItem(GtkWidget *MenuBar,char *test);  // 创建目录项
GtkWidget *CreateMenu(GtkWidget *MenuItem);   // 创建目录条
GtkWidget *login_win(GtkWidget *window);  // 登录窗口
GtkWidget *signin_win(GtkWidget *window); // 注册窗口
GtkWidget *main_win(GtkWidget *window);   // 主窗口
GtkWidget *chat_win(GtkWidget *window,gint chat_with);  //聊天窗口
GtkWidget *createTreeView(GtkWidget *treeview,TreeItem *t,gint length,gint type); // 创建一个树形列表
void msgDialog(GtkWidget *window,char *data,gint type); // 询问与当前点击项目有关的信息
void showPage(GtkWidget *window,gpointer data);  // 展示历史消息

/* 通用功能函数 */
/**
 * 展示窗口
 * 参数： window
 * 返回值：无
 */
void showWin(GtkWidget *window){
    gtk_init(NULL,NULL);
    gtk_widget_show(window);
    gtk_widget_show_all(window);
    gtk_main();
}
/**
 * 关闭应用
 * 参数： 无
 * 返回值：无
 */
void closeApp(GtkWidget *window, gpointer data){
        gtk_main_quit();
}
/**
 * 回到主界面
 * 参数： 要关闭的窗口指针
 * 返回值：无
 */
void returnApp(GtkWidget *window, gpointer data){
        gtk_widget_destroy(data);
        gtk_main_quit();
}


/* 注册界面以及相关操作 */
/**
 * 注册窗口
 * 参数： 无
 * 返回值：GtkWidget指针
 */
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
/**
 * 注册用户
 * 参数： 窗口指针
 * 返回值：无
 */
void signin_button_click(GtkWidget *button,gpointer data){
    const char *password_text = gtk_entry_get_text(password_entry);
    const char *check_text = gtk_entry_get_text(check_password_entry);
    if(strcmp(password_text,check_text) == 0){
        const char *username_text = gtk_entry_get_text(username_entry);
        if(registe(sd,username_text,password_text)){
            receivemsg(sd,&msgdata);
            if(!strcmp(msgdata.buf,"创建成功")){
                // printf("Signin granted!\n");
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
/**
 * 登录界面
 * 参数： 窗口指针
 * 返回值：GtkWidget指针
 */
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
/**
 * 登录
 * 参数： 窗口指针
 * 返回值：无
 */
void login_button_clicked(GtkWidget *button, gpointer data){
    const char *password_text = gtk_entry_get_text(password_entry);
    const char *username_text = gtk_entry_get_text(username_entry);
    if(login(sd,username_text,password_text)){
        receivemsg(sd,&msgdata);
        if(!strcmp(msgdata.buf,"success")){
            // printf("Access granted!\n");
            memset(me,0,sizeof(gchar));
            strcpy(me,username_text);
            if(queryunheard(sd,me)){
                receivemsg(sd,&msgdata);
                ansDialog(msgdata.buf);
            }
            gtk_widget_destroy(data);
            fwindow = main_win(fwindow);
            showWin(fwindow);
        }else{
            // printf("Access denied!\n");
            ansDialog(msgdata.buf);
        } 
    }else{
        ansDialog("与服务器连接失败!");
    }
}
/**
 * 注册
 * 参数： 窗口指针
 * 返回值：无
 */
void signin_button_clicked(GtkWidget *button, gpointer data){
    gtk_widget_destroy(data);
    fwindow = signin_win(fwindow);
    showWin(fwindow);
}

/* 主窗口 */
/**
 * 主窗口
 * 参数： 窗口指针
 * 返回值：GtkWidget指针
 */
GtkWidget *main_win(GtkWidget *window){
    GtkWidget *frame1,*frame2;
    GtkWidget *vbox1,*vbox2;
    GtkWidget *box,*mbox;
    GtkWidget *menubar,*menuFile,*submenu,*menuRefresh;
    GtkWidget *menuQuit,*menuChange;
    GtkWidget *view;
    
    int length;
    /*
        main window
    */
    memset(title,0,sizeof(gchar));
    sprintf(title,"%s",me);
    strcat(title,",欢迎您");
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(window,title);
    gtk_window_set_default_size(window,200,550);
    gtk_window_set_position(window,GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(window,FALSE);
    g_signal_connect(window,"destroy",closeApp,NULL);
    mbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_container_add(window,mbox);
    /*
        toolmenu
    */ 
    vbox1 = gtk_vbox_new(FALSE,0);
    gtk_container_add(mbox,vbox1);
    menubar = gtk_menu_bar_new();
    gtk_box_pack_start(vbox1,menubar,FALSE,FALSE,0);
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
    /*
        right box
    */
    vbox2 = gtk_vbox_new(FALSE,0);
    gtk_container_add(box,vbox2);
    gtk_container_set_border_width(vbox2,10);
    /*
        qun zu frame
    */
    frame1 = gtk_frame_new("您的群组：");
    gtk_box_pack_start(vbox2,frame1,FALSE,FALSE,5);
    gtk_widget_set_size_request(frame1,200,250);
    length = query_list(1);
    treeView1 = createTreeView(treeView1,group,length,1);
    gtk_container_add(frame1,treeView1);
    /*
        friends frame
    */
    frame2 = gtk_frame_new("您的好友：");
    gtk_box_pack_start(vbox2,frame2,FALSE,FALSE,5);
    gtk_widget_set_size_request(frame2,200,250);
    length = query_list(2);
    treeView2 = createTreeView(treeView2,member,length,2);
    gtk_container_add(frame2,treeView2);
    /* Generak purpose modle */
    if (!gtk_widget_get_visible (window)){
        gtk_widget_show(window);
        gtk_widget_show_all (window);
    }else
        gtk_widget_destroy (window);
    return window; 
}
/**
 * 创建目录项
 * 参数： 目录条MenuBar,文本test
 * 返回值：GtkWidget指针
 */
GtkWidget *CreateMenuItem(GtkWidget *MenuBar,char *test){
    GtkWidget *MenuItem;
    MenuItem = gtk_menu_item_new_with_label(test);
    gtk_menu_shell_append(MenuBar,MenuItem);
    return MenuItem;
}
/**
 * 创建目录条
 * 参数： 目录条指针MenuItem
 * 返回值：GtkWidget指针
 */
GtkWidget *CreateMenu(GtkWidget *MenuItem){
    GtkWidget *Menu;
    GtkWidget *exit,*creategroup,*searchgroup;
    Menu = gtk_menu_new();
    exit = CreateMenuItem(Menu,"退出");
    creategroup = CreateMenuItem(Menu,"创建群聊");
    searchgroup = CreateMenuItem(Menu,"加好友/群");
    g_signal_connect(exit,"activate",closeApp,NULL);
    g_signal_connect(creategroup,"activate",create_win,NULL);
    g_signal_connect(searchgroup,"activate",search_win,NULL);
    gtk_menu_item_set_submenu(MenuItem,Menu);
}
/**
 * 创建树形列表
 * 参数： 树形列表指针treeview,数据项数组t,数据规模length,数据类型type
 * 返回值：GtkWidget指针
 */
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
/**
 * 选择项功能函数
 * 参数： GtkTreeSelection指针,参数data
 * 返回值：无
 */
void tree_selection_changed(GtkTreeSelection *selection, gpointer data){
    GtkTreeIter iter;
    GtkTreeModel *model;
    gchar *title;
    if(gtk_tree_selection_get_selected(selection,&model,&iter)){
        gtk_tree_model_get(model,&iter,TEXT_COLUMN,&title,-1);
        msgDialog(dwindow,title,data);
        g_free(title);
    }
}
/**
 * 创建群聊窗口
 * 参数： 无
 * 返回值：无
 */
void create_win(void){
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *group_name;
    GtkWidget *vbox,*hbox;
    GtkWidget *label;

    gtk_init(NULL,NULL);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(window,"创建群聊");
    gtk_window_set_resizable(window,FALSE);
    gtk_window_set_position(window,GTK_WIN_POS_CENTER);
    g_signal_connect(window,"destroy",gtk_main_quit,NULL);

    label = gtk_label_new("请填写您要创建的群名：");
    hbox = gtk_hbox_new(FALSE,5);
    vbox = gtk_vbox_new(FALSE,5);
    button = gtk_button_new_with_label("确 定");
    group_name = gtk_entry_new();
    g_signal_connect(button,"clicked",Creategroup,group_name);
    gtk_box_pack_start(hbox,label,FALSE,FALSE,5);
    gtk_box_pack_start(hbox,group_name,FALSE,FALSE,5);
    gtk_box_pack_start(vbox,hbox,TRUE,FALSE,5);
    gtk_box_pack_start(vbox,button,TRUE,FALSE,5);
    
    gtk_container_add(window,vbox);
    if (!gtk_widget_get_visible(window)){
        gtk_widget_show(window);
        gtk_widget_show_all (window);   
    }else
        gtk_widget_destroy (window);
    gtk_main();
}
/**
 * 创建群聊的功能函数
 * 参数： 输入框指针
 * 返回值：无
 */
void Creategroup(GtkWidget *button, gpointer data){
    gchar *group_name = gtk_entry_get_text(data);
    if(addgroup(sd,me,group_name)){
        receivemsg(sd,&msgdata);
        ansDialog(msgdata.buf);
        gtk_widget_destroy(dwindow);
        gtk_widget_destroy(fwindow);
        fwindow = main_win(fwindow);
        showWin(fwindow);
    }else{
        receivemsg(sd,&msgdata);
        ansDialog(msgdata.buf);
    }
}
/**
 * 搜索好友窗口
 * 参数： 无
 * 返回值：无
 */
void search_win(void){
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *frame;
    GtkWidget *vbox,*hbox;
    GtkWidget *radio_group;
    GtkWidget *radio_user,*radio_useless;
    GtkWidget *entry;
    GSList *group;
    gchar *stitle = "加好友/群";

    gtk_init(NULL,NULL);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(window,stitle);
    gtk_container_set_border_width(window,10);
    gtk_window_set_position(window,GTK_WIN_POS_CENTER);
    g_signal_connect(window,"destroy",gtk_main_quit,NULL);
    vbox = gtk_vbox_new(FALSE,0);
    gtk_container_add(window,vbox);

    frame = gtk_frame_new("请您选择");
    gtk_frame_set_shadow_type(frame,GTK_SHADOW_ETCHED_OUT);
    gtk_box_pack_start(vbox,frame,FALSE,FALSE,5);

    entry = gtk_entry_new();
    gtk_box_pack_start(vbox,entry,FALSE,FALSE,0);

    hbox = gtk_hbox_new(FALSE,10);
    gtk_container_set_border_width(hbox,10);
    gtk_container_add(frame,hbox);

    radio_useless = gtk_radio_button_new_with_label(NULL,"请选择类型");
    gtk_box_pack_start(hbox,radio_useless,FALSE,FALSE,5);

    group = gtk_radio_button_get_group(radio_useless);
    radio_group = gtk_radio_button_new_with_label(group,"找群");
    g_signal_connect(radio_group,"released",change,(gpointer)1);
    gtk_box_pack_start(hbox,radio_group,FALSE,FALSE,5);

    group = gtk_radio_button_get_group(radio_group);
    radio_user = gtk_radio_button_new_with_label(group,"找人");
    g_signal_connect(radio_user,"released",change,(gpointer)2);
    gtk_box_pack_start(hbox,radio_user,FALSE,FALSE,5);

    button = gtk_button_new_from_stock(GTK_STOCK_OK);
    g_signal_connect(button,"clicked",Search,entry);
    gtk_box_pack_start(vbox,button,FALSE,FALSE,5);
    if (!gtk_widget_get_visible(window)){
        gtk_widget_show(window);
        gtk_widget_show_all (window);   
    }else
        gtk_widget_destroy (window);
    gtk_main();
}
/**
 * 更改聊天对象
 * 参数： 选项1或2
 * 返回值：无
 */
void change(GtkWidget *window, gpointer data){
    switch((int)data){
        case 1:
            kind = 1;
            break;
        case 2:
            kind = 2;
            break; 
    }
}
/**
 * 加入群聊或添加好友
 * 参数： 输入框指针
 * 返回值：无
 */
void Search(GtkWidget *button, gpointer data){
    gchar *name = gtk_entry_get_text(data);
    switch(kind){
        case 1:
            if(joingroup(sd,me,name)){
                receivemsg(sd,&msgdata);
                ansDialog(msgdata.buf);
                gtk_widget_destroy(fwindow);
                fwindow = main_win(fwindow);
                showWin(fwindow);
            }else{
                receivemsg(sd,&msgdata);
                ansDialog(msgdata.buf);
            }
            break;
        case 2:
            if(addfriend(sd,me,name)){
                receivemsg(sd,&msgdata);
                ansDialog(msgdata.buf);
                gtk_widget_destroy(fwindow);
                fwindow = main_win(fwindow);
                showWin(fwindow);
            }else{
                receivemsg(sd,&msgdata);
                ansDialog(msgdata.buf);
            }
            break;
    }
}
/**
 * 操作窗口
 * 参数： 群名或用户名data,操作对象类型type
 * 返回值：无
 */
void msgDialog(GtkWidget *window,char *data,gint type){
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
    gtk_main();
}
/**
 * 聊天窗口
 * 参数： 聊天对象类型chat_with
 * 返回值：GtkWidget指针
 */
GtkWidget *button1;
GtkWidget *chat_win(GtkWidget *window,gint chat_with){
    GtkWidget *frame;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *button;
    GtkWidget *view;
    GtkWidget *message_entry;
    GtkWidget *text;
    if(!g_thread_supported()){
        g_thread_init(NULL);
    }
    gtk_init(NULL,NULL);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(window,title);
    gtk_window_set_default_size(window,700,550);
    gtk_window_set_position(window,GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(window,FALSE);
    g_signal_connect(window,"destroy",closeApp,window);
    vbox = gtk_vbox_new(FALSE,5);
    gtk_container_add(window,vbox);
    /* 
        message display box
    */
    frame = gtk_frame_new("聊天信息：");
    gtk_box_pack_start(vbox,frame,FALSE,FALSE,5);
    gtk_widget_set_size_request(frame,500,400);
    gtk_container_set_border_width(frame,10);
    view = gtk_scrolled_window_new(NULL,NULL);
    gtk_scrolled_window_set_policy(view,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
    text = gtk_text_view_new();
    gtk_container_add(frame,view);
    gtk_container_add(view,text);
    buffer = gtk_text_view_get_buffer(text);
    /* 
        input box
    */
    hbox =  gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    gtk_box_pack_start(vbox,hbox,FALSE,FALSE,5);
    gtk_container_set_border_width(hbox,10);
    message_entry = gtk_entry_new();
    gtk_box_pack_start(hbox,message_entry,FALSE,FALSE,0);
    gtk_widget_set_size_request(message_entry,400,25);
    
    button = gtk_button_new_with_label("确定发送");
    gtk_box_pack_start(hbox,button,FALSE,FALSE,5);
    button1 = gtk_button_new_with_label("查看消息记录");
    gtk_box_pack_start(hbox,button1,FALSE,FALSE,5);
    gtk_widget_set_sensitive(button1,TRUE);
    if(chat_with == 1){
        g_signal_connect(button,"clicked",send_group,message_entry);
        g_signal_connect(button1,"clicked",showPage,(gpointer)chat_with);
    }else if(chat_with == 2){
        g_signal_connect(button,"clicked",send_user,message_entry);
        g_signal_connect(button1,"clicked",showPage,(gpointer)chat_with);
    }
    
    if (!gtk_widget_get_visible (window)){
        gtk_widget_show(window);
        gtk_widget_show_all (window);
    }else
        gtk_widget_destroy (window);
    if(chat_with == 1){
        gdk_threads_init();
        g_thread_create((GThreadFunc)groupchat,NULL,FALSE,NULL);
    }else if(chat_with == 2){
        gdk_threads_init();
        g_thread_create((GThreadFunc)userchat,NULL,FALSE,NULL);
    }
    gtk_main();
}
/**
 * 展示聊天记录
 * 参数： 聊天对象类型
 * 返回值：无
 */
void showPage(GtkWidget *window,gpointer data){
    gtk_widget_set_sensitive(button1,FALSE);
    gchar *name;
    switch((int)data){
        case 1:
            name = grouphistory();
            break;
        case 2:
            name = userhistory();
            break;
    }
    printf("%s\n",name);
    GtkWidget *mtext;
    GtkWidget *view;
    GtkWidget *frame;
    GtkTextIter iter;
    if(!g_thread_supported()){
        g_thread_init(NULL);
    }
    gtk_init(NULL,NULL);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(window,"历史记录");
    gtk_window_set_default_size(window,400,300);
    gtk_window_set_position(window,GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(window,FALSE);
    g_signal_connect(window,"destroy",closeApp,window);
    frame = gtk_frame_new("历史记录：");
    gtk_widget_set_size_request(frame,350,280);
    gtk_container_set_border_width(frame,10);
    gtk_container_add(window,frame);
    view = gtk_scrolled_window_new(NULL,NULL);
    gtk_scrolled_window_set_policy(view,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
    mtext = gtk_text_view_new();
    gtk_container_add(view,mtext);
    hbuffer = gtk_text_view_get_buffer(mtext);
    FILE *fp;
    fp = fopen(name,"r");
    char s[1024];
    while(!feof(fp)){
        fgets(s,sizeof(s),fp);
        gtk_text_buffer_get_iter_at_offset(hbuffer,&iter,0);
        gtk_text_buffer_insert(hbuffer,&iter,s,-1);
    }
    fclose(fp);
    gtk_container_add(frame,view);
    if (!gtk_widget_get_visible (window)){
        gtk_widget_show(window);
        gtk_widget_show_all (window);
    }else
        gtk_widget_destroy (window);
    gtk_main();
}
/**
 * 查找历史记录
 * 参数： 无
 * 返回值：历史记录文件名
 */
gchar *grouphistory(void){
    gchar get_buf[1024];
    gchar buf[1024];
    int sd1;
    sd1 = getconnection(ipaddr);
    if(sd1 > 0){
        printf("success\n");
    }else{
        printf("failed\n");
    }
    if(queryhistoryfromgroup(sd1,me,target)){
        receivemsg(sd1,&msgdata);
        memset(hname,0,sizeof(gchar));
        sprintf(hname,"%s",msgdata.target);
        strcat(hname,"Msg.txt");
        FILE *fp;
        fp = fopen(hname,"w");
        while(msgdata.command != Info){
            memset(buf,0,sizeof(gchar));
            memset(get_buf,0,sizeof(gchar));
            sprintf(buf,"%s ",msgdata.me);
            strcat(buf," : ");
            strcat(buf,msgdata.buf);
            sprintf(get_buf,"%s\n",buf);
            int len = strlen(get_buf);
            fputs(get_buf,fp);
            receivemsg(sd1,&msgdata);
        }
        sprintf(get_buf,"%s\n","-----记录截止到本次打开聊天窗口-----");
        fputs(get_buf,fp);
        fclose(fp);
    } 
    close(sd1);
    return hname; 
}
/**
 * 群组实时聊天
 * 参数： 无
 * 返回值：无
 */
void groupchat(void){ 
    GtkTextIter iter;
    gchar get_buf[1024];
    gchar buf[1024];
    while(read(sd,&msgdata,sizeof(msg)) != -1){
        if(strcmp(msgdata.buf,"查询完毕") == -1){
            continue;
        }
        memset(buf,0,sizeof(gchar));
        sprintf(buf,"%s  ",msgdata.me);
        strcat(buf," : ");
        strcat(buf,msgdata.buf);
        sprintf(get_buf,"%s\n",buf);
        gdk_threads_enter();
        gtk_text_buffer_get_end_iter(buffer,&iter);
        gtk_text_buffer_insert(buffer,&iter,get_buf,-1);    
        gdk_threads_leave();
    }
}
gchar *userhistory(void){
    gchar get_buf[1024];
    gchar buf[1024];
    int sd1;
    sd1 = getconnection(ipaddr);
    if(sd1 > 0){
        printf("success\n");
    }else{
        printf("failed\n");
    }
    if(queryhistoryfromuser(sd1,me,target)){
        receivemsg(sd1,&msgdata);
        memset(hname,0,sizeof(gchar));
        sprintf(hname,"%s",msgdata.me);
        strcat(hname,"_with_");
        strcat(hname,msgdata.target);
        strcat(hname,"Msg.txt");
        FILE *fp;
        fp = fopen(hname,"w");
        while(msgdata.command != Info){
            memset(buf,0,sizeof(gchar));
            memset(get_buf,0,sizeof(gchar));
            sprintf(buf,"%s ",msgdata.me);
            strcat(buf," : ");
            strcat(buf,msgdata.buf);
            sprintf(get_buf,"%s\n",buf);
            int len = strlen(get_buf);
            fputs(get_buf,fp);
            receivemsg(sd1,&msgdata);
        }
        sprintf(get_buf,"%s\n","-----记录截止到本次打开聊天窗口-----");
        fputs(get_buf,fp);
        fclose(fp);
    } 
    close(sd1);
    return hname;
}
void userchat(void){  // 用户实时聊天
    GtkTextIter iter;
    gchar get_buf[1024];
    gchar buf[1024];
    while(read(sd,&msgdata,sizeof(msg)) != -1){
        if(strcmp(msgdata.buf,"查询完毕") == -1){
            continue;
        }
        memset(buf,0,sizeof(gchar));
        sprintf(buf,"%s  ",msgdata.me);
        strcat(buf," : ");
        strcat(buf,msgdata.buf);
        sprintf(get_buf,"%s\n",buf);
        gdk_threads_enter();
        gtk_text_buffer_get_end_iter(buffer,&iter);
        gtk_text_buffer_insert(buffer,&iter,get_buf,-1);    
        gdk_threads_leave();
    }
}
/**
 * 创建群组聊天窗口
 * 参数： 窗口指针
 * 返回值：无
 */
void chat_with_group(GtkWidget *button, gpointer data){   
    gtk_widget_destroy(data);
    memset(title,0,sizeof(char*));
    strcat(title,"在群");
    strcat(title,target);
    strcat(title,"内进行聊天");
    GtkWidget *window;
    chat_win(window,1);
}
/**
 * 向群组发送消息
 * 参数： 输入框指针
 * 返回值：无
 */
void send_group(GtkWidget *button, gpointer data){
    gchar *message = gtk_entry_get_text(data);
    if(sendtogroup(sd,me,target,message)){
        gtk_entry_set_text(data,"");
    }
}
void chat_with_user(GtkWidget *button, gpointer data){   
    gtk_widget_destroy(data);
    memset(title,0,sizeof(char*));
    strcat(title,"与");
    strcat(title,target);
    strcat(title,"进行聊天");
    GtkWidget *window;
    chat_win(window,2);
}
void send_user(GtkWidget *button, gpointer data){
    gchar *message = gtk_entry_get_text(data);
    if(sendtouser(sd,me,target,message)){
        gtk_entry_set_text(data,"");     
    }
}
/**
 * 删除群组
 * 参数： 无
 * 返回值：无
 */
void delete_group(GtkWidget *button, gpointer data){   
    if(deletegroup(sd,me,target)){
        receivemsg(sd,&msgdata);
        ansDialog(msgdata.buf);
        gtk_widget_destroy(dwindow);
        gtk_widget_destroy(fwindow);
        fwindow = main_win(fwindow);
        showWin(fwindow);
    }else{
        receivemsg(sd,&msgdata);
        ansDialog(msgdata.buf);
    }
}
/**
 * 退出群组
 * 参数： 无
 * 返回值：无
 */
void quit_group(GtkWidget *button, gpointer data){    
    if(quitgroup(sd,me,target)){
        receivemsg(sd,&msgdata); 
        ansDialog(msgdata.buf);
        gtk_widget_destroy(dwindow);
        gtk_widget_destroy(fwindow);
        fwindow = main_win(fwindow);
        showWin(fwindow);
    }else{
        receivemsg(sd,&msgdata);
        ansDialog(msgdata.buf);
    }  
}
/**
 * 删除用户
 * 参数： 无
 * 返回值：无
 */
void delete_user(GtkWidget *button, gpointer data){   
    if(deletefriend(sd,me,target)){
        receivemsg(sd,&msgdata);
        ansDialog(msgdata.buf);
        gtk_widget_destroy(dwindow);
        gtk_widget_destroy(fwindow);
        fwindow = main_win(fwindow);
        showWin(fwindow);
    }else{
        receivemsg(sd,&msgdata);
        ansDialog(msgdata.buf);
    }
}
/**
 * 消息提示窗口
 * 参数： 提示文字
 * 返回值：无
 */
void ansDialog(char *data){
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
    g_signal_connect(btn,"clicked",returnApp,window);
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
/**
 * 查询群组或者用户列表
 * 参数： 类型标识func
 * 返回值：数据规模
 */
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
/**
 * 主函数
 * 参数： 服务器ip地址
 * 返回值：FALSE
 */
int main(int argc,char * argv[]){
    if(argc !=2){
        printf("please input <IPADDRESS>\n");
        return EXIT_SUCCESS;
    }
    if(!g_thread_supported()){
        g_thread_init(NULL);
    }
    sd = getconnection(argv[1]);
    if(sd==EXIT_FAILURE){
        printf("<IPADDRESS> error\n");
        return EXIT_SUCCESS;
    }
    sprintf(ipaddr,"%s",argv[1]);
    memset(me,0,sizeof(me));
    memset(target,0,sizeof(target));
    memset(member,0,sizeof(member));
    memset(group,0,sizeof(group));
    fwindow = login_win(fwindow);
    showWin(fwindow);
    return FALSE;
}