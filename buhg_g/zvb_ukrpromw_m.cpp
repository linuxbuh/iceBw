/*$Id: zvb_ukrpromw_m.c,v 1.16 2013/08/25 08:27:02 sasa Exp $*/
/*22.06.2006	05.05.2004	Белых А.И.	zvb_ukrpromw_m.c
Меню для ввода реквизитов поиска 
*/
#include "buhg_g.h"
#include "zvb_ukrpromw.h"
enum
 {
  E_NOMER_FIL,
  E_KOD_OR,
  E_IMA_MAH,
  E_IMA_SET_DISK,
  E_PAROL,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK5,
  FK10,
  KOL_FK
 };

class zvb_ukrprom_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class zvb_ukrpromw_rek *rk;
  
  
  zvb_ukrprom_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      g_signal_emit_by_name(entry[i],"activate");
     }
   }

  void clear_data()
   {
    rk->clear_rek();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void zvb_ukrprom_m_clear(class zvb_ukrprom_m_data *data);
void    zvb_ukrprom_m_vvod(GtkWidget *widget,class zvb_ukrprom_m_data *data);
void  zvb_ukrprom_m_knopka(GtkWidget *widget,class zvb_ukrprom_m_data *data);
gboolean   zvb_ukrprom_m_key_press(GtkWidget *widget,GdkEventKey *event,class zvb_ukrprom_m_data *data);

void zvb_ukrpromw_read_rek(class zvb_ukrprom_m_data *data);

extern SQL_baza bd;

int zvb_ukrpromw_m(class zvb_ukrpromw_rek *rek_poi,GtkWidget *wpredok)
{
class zvb_ukrprom_m_data data;
char strsql[512];
data.rk=rek_poi;


zvb_ukrpromw_read_rek(&data);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Укрпромбанк"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zvb_ukrprom_m_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }



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
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


GtkWidget *label=gtk_label_new(gettext("Номер файла за текущий день"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_FIL]), label, FALSE, FALSE, 0);

data.entry[E_NOMER_FIL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMER_FIL]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_FIL]), data.entry[E_NOMER_FIL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMER_FIL], "activate",G_CALLBACK(zvb_ukrprom_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER_FIL]),data.rk->nomer_fil.ravno());
gtk_widget_set_name(data.entry[E_NOMER_FIL],iceb_u_inttochar(E_NOMER_FIL));


label=gtk_label_new(gettext("Код организации"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OR]), label, FALSE, FALSE, 0);

data.entry[E_KOD_OR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_OR]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OR]), data.entry[E_KOD_OR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_OR], "activate",G_CALLBACK(zvb_ukrprom_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_OR]),data.rk->kod_or.ravno());
gtk_widget_set_name(data.entry[E_KOD_OR],iceb_u_inttochar(E_KOD_OR));


label=gtk_label_new(gettext("Имя машины"));
gtk_box_pack_start (GTK_BOX (hbox[E_IMA_MAH]), label, FALSE, FALSE, 0);

data.entry[E_IMA_MAH] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_IMA_MAH]), data.entry[E_IMA_MAH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_IMA_MAH], "activate",G_CALLBACK(zvb_ukrprom_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMA_MAH]),data.rk->ima_mah.ravno());
gtk_widget_set_name(data.entry[E_IMA_MAH],iceb_u_inttochar(E_IMA_MAH));

label=gtk_label_new(gettext("Имя сетевого диска"));
gtk_box_pack_start (GTK_BOX (hbox[E_IMA_SET_DISK]), label, FALSE, FALSE, 0);

data.entry[E_IMA_SET_DISK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_IMA_SET_DISK]), data.entry[E_IMA_SET_DISK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_IMA_SET_DISK], "activate",G_CALLBACK(zvb_ukrprom_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_IMA_SET_DISK]),data.rk->ima_set_disk.ravno());
gtk_widget_set_name(data.entry[E_IMA_SET_DISK],iceb_u_inttochar(E_IMA_SET_DISK));

label=gtk_label_new(gettext("Пароль"));
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL]), label, FALSE, FALSE, 0);

data.entry[E_PAROL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PAROL]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_PAROL]), data.entry[E_PAROL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PAROL], "activate",G_CALLBACK(zvb_ukrprom_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PAROL]),data.rk->parol.ravno());
gtk_widget_set_name(data.entry[E_PAROL],iceb_u_inttochar(E_PAROL));



sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(zvb_ukrprom_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(zvb_ukrprom_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F5 %s",gettext("Настройка"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Настройка расчёта"));
g_signal_connect(data.knopka[FK5],"clicked",G_CALLBACK(zvb_ukrprom_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK5], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(zvb_ukrprom_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zvb_ukrprom_m_key_press(GtkWidget *widget,GdkEventKey *event,class zvb_ukrprom_m_data *data)
{
//char  bros[512];

//printf("vl_gruppod_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(FALSE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");
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
void  zvb_ukrprom_m_knopka(GtkWidget *widget,class zvb_ukrprom_m_data *data)
{
//char bros[512];

int knop=atoi(gtk_widget_get_name(widget));
//g_print("zvb_ukrprom_m_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_data();
    return;  

  case FK5:
    if(iceb_f_redfil("zvb_ukrprom.alx",0,data->window) == 0)
       zvb_ukrpromw_read_rek(data);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMER_FIL]),data->rk->nomer_fil.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_OR]),data->rk->kod_or.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_IMA_MAH]),data->rk->ima_mah.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_IMA_SET_DISK]),data->rk->ima_set_disk.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PAROL]),data->rk->parol.ravno());
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

void    zvb_ukrprom_m_vvod(GtkWidget *widget,class zvb_ukrprom_m_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("zvb_ukrprom_m_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_NOMER_FIL:
    data->rk->nomer_fil.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOD_OR:
    data->rk->kod_or.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_IMA_MAH:
    data->rk->ima_mah.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_IMA_SET_DISK:
    data->rk->ima_set_disk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_PAROL:
    data->rk->parol.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
 }
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************/
/*Чтение настроек*/
/***********************/
void zvb_ukrpromw_read_rek(class zvb_ukrprom_m_data *data)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"zvb_ukrprom.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,data->window);
  return;
 }

class iceb_u_str naim_str("");

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;


  iceb_u_polen(row_alx[0],&naim_str,1,'|');
  if(iceb_u_SRAV(naim_str.ravno(),"Имя машины",0) == 0)
   {
    iceb_u_polen(row_alx[0],&data->rk->ima_mah,2,'|');
    continue;   
   }
  if(iceb_u_SRAV(naim_str.ravno(),"Имя сетевого диска",0) == 0)
   {
    iceb_u_polen(row_alx[0],&data->rk->ima_set_disk,2,'|');
    continue;   
   }
  if(iceb_u_SRAV(naim_str.ravno(),"Пароль",0) == 0)
   {
    iceb_u_polen(row_alx[0],&data->rk->parol,2,'|');
    continue;   
   }
  if(iceb_u_SRAV(naim_str.ravno(),"Код организации",0) == 0)
   {
    iceb_u_polen(row_alx[0],&data->rk->kod_or,2,'|');
    continue;   
   }
 }



}
