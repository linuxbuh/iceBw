/*$Id: admin_alterw_m.c,v 1.5 2013/08/25 08:26:30 sasa Exp $*/
/*02.08.2015	19.02.2012	Белых А.И.	admin_alter_m.c
Меню для преобразования баз
*/
#include <errno.h>
#include "buhg_g.h"

enum
 {
  E_IMABAZ,
  E_FROMNSI,
  E_FROMDOC,
  KOLENTER  
 };

enum
 {
  FK1,
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class admin_alter_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_link;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str imabaz;
  static class iceb_u_str fromnsi;
  static class iceb_u_str fromdoc;    
  
  admin_alter_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear();
   }

  void read_rek()
   {
    imabaz.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_IMABAZ])));
    fromnsi.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_FROMNSI])));
    fromdoc.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_FROMDOC])));
 
   }

  void clear_data()
   {
    clear();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
  void clear()
   {
    imabaz.new_plus("");
    fromnsi.new_plus("/usr/share/iceBw/alx");
    fromdoc.new_plus("/usr/share/iceBw/doc");
   }
 };

class iceb_u_str admin_alter_m_data::fromnsi;
class iceb_u_str admin_alter_m_data::fromdoc;

void admin_alter_m_clear(class admin_alter_m_data *data);
void    admin_alter_m_vvod(GtkWidget *widget,class admin_alter_m_data *data);
void  admin_alter_m_knopka(GtkWidget *widget,class admin_alter_m_data *data);
gboolean   admin_alter_m_key_press(GtkWidget *widget,GdkEventKey *event,class admin_alter_m_data *data);
void admin_alter_m_e_knopka(GtkWidget *widget,class admin_alter_m_data *data);

extern SQL_baza bd;

int admin_alter_m(class iceb_u_str *imabaz,
class iceb_u_str *fromnsi,
class iceb_u_str *fromdoc,
GtkWidget *wpredok)
{
static class admin_alter_m_data data;
char strsql[1024];
class iceb_u_str version;
SQL_str row;
class SQLCURSOR cur;



sql_readkey(&bd,"select VERSION()",&row,&cur);
version.plus(row[0]);


if(data.fromnsi.getdlinna() <= 1)
 data.fromnsi.plus("");
if(data.fromdoc.getdlinna() <= 1)
 data.fromdoc.plus("");
 
 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Преобразование баз данных для новой версии системы"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(admin_alter_m_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label=NULL;
sprintf(strsql,"%s\n%s MySQL:%s\n%s 7.18 %s %s\n%s",
gettext("Преобразование баз данных для новой версии системы"),
gettext("Верия"),
version.ravno(),
gettext("Преобразования будут выполнены от версии"),
gettext("до"),
VERSION,
gettext("Если имя базы не введено, то преобразование будет выполнено для всех баз"));

label=gtk_label_new(strsql);


GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


data.knopka_enter[E_IMABAZ]=gtk_button_new_with_label(gettext("Имя базы данных"));
gtk_box_pack_start (GTK_BOX (hbox[E_IMABAZ]), data.knopka_enter[E_IMABAZ], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_IMABAZ],"clicked",G_CALLBACK(admin_alter_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_IMABAZ],iceb_u_inttochar(E_IMABAZ));
gtk_widget_set_tooltip_text(data.knopka_enter[E_IMABAZ],gettext("Выбор базы"));

data.entry[E_IMABAZ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_IMABAZ]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_IMABAZ]), data.entry[E_IMABAZ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_IMABAZ], "activate",G_CALLBACK(admin_alter_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMABAZ]),data.imabaz.ravno());
gtk_widget_set_name(data.entry[E_IMABAZ],iceb_u_inttochar(E_IMABAZ));


data.knopka_enter[E_FROMNSI]=gtk_button_new_with_label(gettext("Каталог откуда импортировать настроечные файлы"));
gtk_box_pack_start (GTK_BOX (hbox[E_FROMNSI]), data.knopka_enter[E_FROMNSI], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_FROMNSI],"clicked",G_CALLBACK(admin_alter_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_FROMNSI],iceb_u_inttochar(E_FROMNSI));
gtk_widget_set_tooltip_text(data.knopka_enter[E_FROMNSI],gettext("Выбор каталога"));

data.entry[E_FROMNSI] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_FROMNSI]), data.entry[E_FROMNSI], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FROMNSI], "activate",G_CALLBACK(admin_alter_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FROMNSI]),data.fromnsi.ravno());
gtk_widget_set_name(data.entry[E_FROMNSI],iceb_u_inttochar(E_FROMNSI));


data.knopka_enter[E_FROMDOC]=gtk_button_new_with_label(gettext("Каталог откуда импортировать документацию"));
gtk_box_pack_start (GTK_BOX (hbox[E_FROMDOC]), data.knopka_enter[E_FROMDOC], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_FROMDOC],"clicked",G_CALLBACK(admin_alter_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_FROMDOC],iceb_u_inttochar(E_FROMDOC));
gtk_widget_set_tooltip_text(data.knopka_enter[E_FROMDOC],gettext("Выбор каталога"));

data.entry[E_FROMDOC] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_FROMDOC]), data.entry[E_FROMDOC], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FROMDOC], "activate",G_CALLBACK(admin_alter_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FROMDOC]),data.fromdoc.ravno());
gtk_widget_set_name(data.entry[E_FROMDOC],iceb_u_inttochar(E_FROMDOC));

sprintf(strsql,"<a href=\"%s/i_admin5.html\" title=\"%s\">F1 %s</a>",ICEB_PUT_NA_HTML,gettext("Просмотр документации"),gettext("Помощь"));
data.label_link=gtk_label_new(strsql);
gtk_label_set_use_markup (GTK_LABEL (data.label_link), TRUE);
g_signal_connect(data.label_link, "activate-current-link", G_CALLBACK (iceb_activate_link), NULL);

data.knopka[FK1]=gtk_button_new();
gtk_container_add (GTK_CONTAINER (data.knopka[FK1]), data.label_link);
g_signal_connect(data.knopka[FK1],"clicked",G_CALLBACK(admin_alter_m_knopka),&data);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK1], TRUE, TRUE, 0);


sprintf(strsql,"F2 %s",gettext("Преобразовать"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать преобразование баз"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(admin_alter_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(admin_alter_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Не перобразовывать базы"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(admin_alter_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);


gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  imabaz->new_plus(data.imabaz.ravno());
  fromnsi->new_plus(data.fromnsi.ravno());
  fromdoc->new_plus(data.fromdoc.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void admin_alter_m_e_knopka(GtkWidget *widget,class admin_alter_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_IMABAZ:

    if(admin_bases(1,&data->imabaz,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_IMABAZ]),data->imabaz.ravno());
      
    return;  
  case E_FROMNSI:

    iceb_dir_select(&data->fromnsi,gettext("Выбор каталога"),data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_FROMNSI]),data->fromnsi.ravno());
      
    return;  


  case E_FROMDOC:

    iceb_dir_select(&data->fromdoc,gettext("Выбор каталога"),data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_FROMDOC]),data->fromdoc.ravno());
      
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   admin_alter_m_key_press(GtkWidget *widget,GdkEventKey *event,class admin_alter_m_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F1:
    g_signal_emit_by_name(data->knopka[FK1],"clicked");
    return(FALSE);

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(FALSE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  admin_alter_m_knopka(GtkWidget *widget,class admin_alter_m_data *data)
{
FILE *ff;
class iceb_u_str imafilprov("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("admin_alter_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK1:
    g_signal_emit_by_name(data->label_link,"activate-current-link");
    return;
  case FK2:
    data->read_rek();
    if(data->imabaz.getdlinna() > 1)
    if(admin_provbaz(data->imabaz.ravno(),data->window) != 0)
     {
      iceb_menu_soob(gettext("Нет базы с таким именем!"),data->window);
      return;       
     }

    /*Проверяем возможность открытия настроечного файла*/   
    imafilprov.new_plus(data->fromnsi.ravno());
    imafilprov.plus(G_DIR_SEPARATOR_S,"nastsys.alx");
    
    
    if((ff = fopen(imafilprov.ravno(),"r")) == NULL)
     {
      iceb_er_op_fil(imafilprov.ravno(),gettext("Неправильно указан путь откуда брать настроечные файлы!"),errno,data->window);
      return;
     }
    fclose(ff);

    /*Проверяем возможность открытия файла с документацией*/   
    imafilprov.new_plus(data->fromdoc.ravno());
    imafilprov.plus(G_DIR_SEPARATOR_S,"buhg.txt");
    
    
    if((ff = fopen(imafilprov.ravno(),"r")) == NULL)
     {
      iceb_er_op_fil(imafilprov.ravno(),gettext("Неправильно указан путь на файлы с документацией!"),errno,data->window);
      return;
     }
    fclose(ff);


    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_data();
    return;  


  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    admin_alter_m_vvod(GtkWidget *widget,class admin_alter_m_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_IMABAZ:
    data->imabaz.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_IMABAZ])));
    break;
 }

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
