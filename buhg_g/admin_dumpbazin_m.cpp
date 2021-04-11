/*$Id: admin_dumpbazin_m.c,v 1.1 2014/05/07 10:58:06 sasa Exp $*/
/*10.07.2015	21.06.2006	Белых А.И.	admin_dumpbazin_m.c
Ввод и корректировка категорий работников
*/
#include        <sys/stat.h>
#include "buhg_g.h"
#include "l_zarkateg.h"
enum
 {
  E_PAROL,
  E_IMAFIL,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class admin_dumpbazin_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *opt;
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str parol;
  class iceb_u_str imafil;
  int metka_zag; /*0-загрузка базы до первой ошибки 1-до конца*/
  
  admin_dumpbazin_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    parol.plus("");
    imafil.plus("");
    metka_zag=0;
   }

  void read_rek()
   {
    parol.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PAROL])));
    imafil.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_IMAFIL])));
    metka_zag=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    
   }
 };


gboolean   admin_dumpbazin_m_key_press(GtkWidget *widget,GdkEventKey *event,class admin_dumpbazin_m_data *data);
void  admin_dumpbazin_m_knopka(GtkWidget *widget,class admin_dumpbazin_m_data *data);
void    admin_dumpbazin_m_vvod(GtkWidget *widget,class admin_dumpbazin_m_data *data);
void  admin_dumpbazin_m_v_e_knopka(GtkWidget *widget,class admin_dumpbazin_m_data *data);


extern SQL_baza  bd;

int admin_dumpbazin_m(class iceb_u_str *imafil,int *metka_zag,GtkWidget *wpredok)
{

class admin_dumpbazin_m_data data;
char strsql[1024];
class iceb_u_str kikz;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;
sprintf(strsql,"%s",gettext("Загрузка базы"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

sprintf(strsql,"%s\n%s",gettext("Внимание!!!"),gettext("Если имя загружаемой базы совпадёт с уже имеющимся в списке баз,\n\
то содержимое новой базы данных заменит содержимое существующей базы."));

label=gtk_label_new(strsql);


g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(admin_dumpbazin_m_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hbox_rz = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox_rz),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 0);

for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);

label=gtk_label_new(gettext("Режим загрузки"));
gtk_box_pack_start (GTK_BOX (hbox_rz),label, TRUE, TRUE,1);


class iceb_u_spisok spmenu;
spmenu.plus(gettext("Прекратить загрузку после первой ошибки"));
spmenu.plus(gettext("Грузить до конца"));

iceb_pm_vibor(&spmenu,&data.opt,data.metka_zag);
gtk_box_pack_start (GTK_BOX (hbox_rz),data.opt, TRUE, TRUE,1);

gtk_box_pack_start (GTK_BOX (vbox),hbox_rz, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


label=gtk_label_new(gettext("Пароль"));
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL]), label, FALSE, FALSE, 0);

data.entry[E_PAROL] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PAROL]),4);
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL]), data.entry[E_PAROL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PAROL], "activate",G_CALLBACK(admin_dumpbazin_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PAROL]),data.parol.ravno());
gtk_widget_set_name(data.entry[E_PAROL],iceb_u_inttochar(E_PAROL));/******************/
gtk_entry_set_visibility(GTK_ENTRY(data.entry[E_PAROL]),FALSE); /*звёздочки*/


data.knopka_enter[E_IMAFIL]=gtk_button_new_with_label(gettext("Имя файла с дампом базы"));
gtk_box_pack_start (GTK_BOX (hbox[E_IMAFIL]), data.knopka_enter[E_IMAFIL], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_IMAFIL],"clicked",G_CALLBACK(admin_dumpbazin_m_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_IMAFIL],iceb_u_inttochar(E_IMAFIL));
gtk_widget_set_tooltip_text(data.knopka_enter[E_IMAFIL],gettext("Поиск файла"));

data.entry[E_IMAFIL] = gtk_entry_new();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_IMAFIL]),60);
gtk_box_pack_start (GTK_BOX (hbox[E_IMAFIL]), data.entry[E_IMAFIL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_IMAFIL], "activate",G_CALLBACK(admin_dumpbazin_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMAFIL]),data.imafil.ravno());
gtk_widget_set_name(data.entry[E_IMAFIL],iceb_u_inttochar(E_IMAFIL));/******************/


sprintf(strsql,"F2 %s",gettext("Загрузить"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать загрузку дампа базы"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(admin_dumpbazin_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введённой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(admin_dumpbazin_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Не загружать дамп базы"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(admin_dumpbazin_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  imafil->new_plus(data.imafil.ravno());
  *metka_zag=data.metka_zag;
 }
if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  admin_dumpbazin_m_v_e_knopka(GtkWidget *widget,class admin_dumpbazin_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_IMAFIL:

    iceb_file_selection(&data->imafil,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_IMAFIL]),data->imafil.ravno());
      
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   admin_dumpbazin_m_key_press(GtkWidget *widget,GdkEventKey *event,class admin_dumpbazin_m_data *data)
{
//char  bros[512];

//printf("admin_dumpbazin_m_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");

    return(TRUE);
  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");

    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");

    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
  //  printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  admin_dumpbazin_m_knopka(GtkWidget *widget,class admin_dumpbazin_m_data *data)
{
char strsql[1024];
short dt=0,mt=0,gt=0;
iceb_u_poltekdat(&dt,&mt,&gt);
//int knop=atoi(gtk_widget_get_name(widget));
//g_print("admin_dumpbazin_m_knopka knop=%d\n",knop);
int knop=atoi(gtk_widget_get_name(widget));
switch (knop)
 {
  case FK2:
    data->read_rek();
    iceb_u_poltekdat(&dt,&mt,&gt);
    if(dt+mt+gt != data->parol.ravno_atoi())
     {
      iceb_menu_soob(gettext("Пароль введён неправильно!"),data->window);
      return;
     }
    struct stat work;

    if(stat(data->imafil.ravno(),&work) != 0)
     {
      sprintf(strsql,"%s: %s !",gettext("Не найден файл"),data->imafil.ravno());
      iceb_menu_soob(strsql,data->window);
      return;
     }

    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    admin_dumpbazin_m_vvod(GtkWidget *widget,class admin_dumpbazin_m_data *data)
{
//int enter=atoi(gtk_widget_get_name(widget));
//g_print("admin_dumpbazin_m_vvod enter=%d\n",enter);
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_PAROL:
    data->parol.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_IMAFIL:
    data->imafil.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
