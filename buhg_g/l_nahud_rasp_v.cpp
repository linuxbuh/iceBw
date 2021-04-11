/*$Id:$*/
/*19.12.2017	28.03.2017	Белых А.И.	l_nahud_rasp_v.cpp
Меню для ввода реквизитов
*/
#include "buhg_g.h"
enum
 {
  E_DATA,
  E_SHET,
  E_METKA_VD,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_nahud_rasp_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *opt;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str data_zar;
  class iceb_u_str shet_zar;  
  int metka_vd;
    
  l_nahud_rasp_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    metka_vd=0;
   }

  void read_rek()
   {
    data_zar.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA])));
    shet_zar.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    metka_vd=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));
   }

  void clear_data()
   {
    for(int i=0; i< KOLENTER-1; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
    metka_vd=0;
   }
 };

void l_nahud_rasp_v_clear(class l_nahud_rasp_v_data *data);
void    l_nahud_rasp_v_vvod(GtkWidget *widget,class l_nahud_rasp_v_data *data);
void  l_nahud_rasp_v_knopka(GtkWidget *widget,class l_nahud_rasp_v_data *data);
gboolean   l_nahud_rasp_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_nahud_rasp_v_data *data);


int l_nahud_rasp_v(class iceb_u_str *data_zar,class iceb_u_str *shet_zar,int *metka_vd,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class l_nahud_rasp_v_data data;
char strsql[512];

data.name_window.plus(__FUNCTION__);
data.shet_zar.plus(shet_zar->ravno());
data.data_zar.plus(data_zar->ravno());
data.metka_vd=*metka_vd;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Введите данные для платёжной ведомости"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_nahud_rasp_v_key_press),&data);

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
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

GtkWidget *label=gtk_label_new(gettext("Введите данные для платёжной ведомости"));

gtk_container_add (GTK_CONTAINER (data.window), vbox);

gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 0);


for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);

class iceb_u_spisok spmenu;
spmenu.plus(gettext("Ведомости по подразделениям"));
spmenu.plus(gettext("Всех в одной ведомости"));
spmenu.plus(gettext("Каждого в отдельной ведомости"));

iceb_pm_vibor(&spmenu,&data.opt,data.metka_vd);
gtk_box_pack_start (GTK_BOX (vbox),data.opt, TRUE, TRUE,1);


gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 0);


label=gtk_label_new(gettext("Дата зарплаты (м.г)"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), label, FALSE, FALSE, 0);

data.entry[E_DATA] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA]),8);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.entry[E_DATA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATA], "activate",G_CALLBACK(l_nahud_rasp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA]),data.data_zar.ravno());
gtk_widget_set_name(data.entry[E_DATA],iceb_u_inttochar(E_DATA));/******************/

label=gtk_label_new(gettext("Счёт"));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), label, FALSE, FALSE, 0);

data.entry[E_SHET] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(l_nahud_rasp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet_zar.ravno());

gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));/******************/


sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Продолжить расчёт"));
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(l_nahud_rasp_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(l_nahud_rasp_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Выход с прекращением расчёта"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(l_nahud_rasp_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.voz == 0)
 {
  data_zar->new_plus(data.data_zar.ravno());
  shet_zar->new_plus(data.shet_zar.ravno());
  *metka_vd=data.metka_vd;
 }
return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_nahud_rasp_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_nahud_rasp_v_data *data)
{

switch(event->keyval)
 {

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
void  l_nahud_rasp_v_knopka(GtkWidget *widget,class l_nahud_rasp_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(data->data_zar.prov_dat1() != 0)
     {
      iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
      return;
     }
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_data();
    return;  


  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_nahud_rasp_v_vvod(GtkWidget *widget,class l_nahud_rasp_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
switch (enter)
 {
  case E_DATA:
    data->data_zar.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATA])));
    break;
  case E_SHET:
    data->shet_zar.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_SHET])));
    break;
 }

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
