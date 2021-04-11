/*$Id: rasttn2w_m.c,v 1.1 2014/01/31 12:13:00 sasa Exp $*/
/*12.07.2015	30.09.2013	Белых А.И.	rasttn2w_m.c
Меню для ввода реквизитов 
*/
#include "buhg_g.h"
#include "rasttnw.h"

enum
 {
  E_AVTOPREDP,
  E_FIO_VOD,
//  E_NOM_PUT_LIST,
  E_MARKA_AVT,
  E_GOS_NOMER,
  E_PUNKT_ZAGR,
  E_PUNKT_RAZG,
  E_ZAKAZCHIK,
  E_GOS_NOMER_PRIC,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class rasttn2w_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class rasttn_rek *rk;
  
  
  rasttn2w_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->marka_avt.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MARKA_AVT])));
    rk->gos_nomer.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GOS_NOMER])));
//    rk->nom_put_list.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOM_PUT_LIST])));
    rk->fio_vod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_FIO_VOD])));
    rk->avtopredp.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_AVTOPREDP])));
    rk->punkt_zagr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PUNKT_ZAGR])));
    rk->punkt_razg.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PUNKT_RAZG])));
    rk->zakazchik.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_ZAKAZCHIK])));
    rk->gos_nomer_pric.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GOS_NOMER_PRIC])));
    
   }

  void clear_data()
   {
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void rasttn2w_m_clear(class rasttn2w_m_data *data);
void    rasttn2w_m_vvod(GtkWidget *widget,class rasttn2w_m_data *data);
void  rasttn2w_m_knopka(GtkWidget *widget,class rasttn2w_m_data *data);
gboolean   rasttn2w_m_key_press(GtkWidget *widget,GdkEventKey *event,class rasttn2w_m_data *data);
int rasttn2w_m_zap(class rasttn2w_m_data *data);


int rasttn2w_m(class rasttn_rek *rek_poi,GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
class rasttn2w_m_data data;
char strsql[1024];
data.rk=rek_poi;

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=10",data.rk->gd,data.rk->sklad,data.rk->nomdok.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  data.rk->gos_nomer.new_plus(row[0]);
/*******************
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=15",data.rk->gd,data.rk->sklad,data.rk->nomdok.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  data.rk->nom_put_list.new_plus(row[0]);
*****************/
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=16",data.rk->gd,data.rk->sklad,data.rk->nomdok.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  data.rk->marka_avt.new_plus(row[0]);

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=17",data.rk->gd,data.rk->sklad,data.rk->nomdok.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  data.rk->gos_nomer_pric.new_plus(row[0]);

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=18",data.rk->gd,data.rk->sklad,data.rk->nomdok.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  data.rk->avtopredp.new_plus(row[0]);


sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=19",data.rk->gd,data.rk->sklad,data.rk->nomdok.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  data.rk->zakazchik.new_plus(row[0]);

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=20",data.rk->gd,data.rk->sklad,data.rk->nomdok.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  data.rk->fio_vod.new_plus(row[0]);

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=21",data.rk->gd,data.rk->sklad,data.rk->nomdok.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  data.rk->punkt_zagr.new_plus(row[0]);

sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d and nomd='%s' and nomerz=22",data.rk->gd,data.rk->sklad,data.rk->nomdok.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  data.rk->punkt_razg.new_plus(row[0]);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_default_size (GTK_WINDOW  (data.window),600,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка товарно-транспортной накладной"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
/**************
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rasttnw_m_key_press),&data);
****************/
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rasttn2w_m_key_press),&data);
if(wpredok != NULL)
 {

//  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
//gtk_widget_get_window(wpredok); 
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

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
 gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 0);


GtkWidget *label=gtk_label_new(gettext("Автопредприятие"));
gtk_box_pack_start (GTK_BOX (hbox[E_AVTOPREDP]), label, FALSE, FALSE, 0);

data.entry[E_AVTOPREDP] = gtk_entry_new();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_AVTOPREDP]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_AVTOPREDP]), data.entry[E_AVTOPREDP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_AVTOPREDP], "activate",G_CALLBACK(rasttn2w_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_AVTOPREDP]),data.rk->avtopredp.ravno());
gtk_widget_set_name(data.entry[E_AVTOPREDP],iceb_u_inttochar(E_AVTOPREDP));/******************/

label=gtk_label_new(gettext("Фамилия водителя"));
gtk_box_pack_start (GTK_BOX (hbox[E_FIO_VOD]), label, FALSE, FALSE, 0);

data.entry[E_FIO_VOD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_FIO_VOD]), data.entry[E_FIO_VOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FIO_VOD], "activate",G_CALLBACK(rasttn2w_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO_VOD]),data.rk->fio_vod.ravno());
gtk_widget_set_name(data.entry[E_FIO_VOD],iceb_u_inttochar(E_FIO_VOD));/******************/
/***********************************
label=gtk_label_new(gettext("Номер путевого листа"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_PUT_LIST]), label, FALSE, FALSE, 0);

data.entry[E_NOM_PUT_LIST] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_PUT_LIST]), data.entry[E_NOM_PUT_LIST], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOM_PUT_LIST], "activate",G_CALLBACK(rasttn2w_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOM_PUT_LIST]),data.rk->nom_put_list.ravno());
gtk_widget_set_name(data.entry[E_NOM_PUT_LIST],iceb_u_inttochar(E_NOM_PUT_LIST));
****************************/
label=gtk_label_new(gettext("Марка автомобиля"));
gtk_box_pack_start (GTK_BOX (hbox[E_MARKA_AVT]), label, FALSE, FALSE, 0);

data.entry[E_MARKA_AVT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_MARKA_AVT]), data.entry[E_MARKA_AVT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_MARKA_AVT], "activate",G_CALLBACK(rasttn2w_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MARKA_AVT]),data.rk->marka_avt.ravno());
gtk_widget_set_name(data.entry[E_MARKA_AVT],iceb_u_inttochar(E_MARKA_AVT));/******************/


label=gtk_label_new(gettext("Гос. номер автомобиля"));
gtk_box_pack_start (GTK_BOX (hbox[E_GOS_NOMER]), label, FALSE, FALSE, 0);

data.entry[E_GOS_NOMER] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GOS_NOMER]), data.entry[E_GOS_NOMER], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GOS_NOMER], "activate",G_CALLBACK(rasttn2w_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOS_NOMER]),data.rk->gos_nomer.ravno());
gtk_widget_set_name(data.entry[E_GOS_NOMER],iceb_u_inttochar(E_GOS_NOMER));/******************/


label=gtk_label_new(gettext("Пунк загрузки"));
gtk_box_pack_start (GTK_BOX (hbox[E_PUNKT_ZAGR]), label, FALSE, FALSE, 0);

data.entry[E_PUNKT_ZAGR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PUNKT_ZAGR]), data.entry[E_PUNKT_ZAGR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PUNKT_ZAGR], "activate",G_CALLBACK(rasttn2w_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PUNKT_ZAGR]),data.rk->punkt_zagr.ravno());
gtk_widget_set_name(data.entry[E_PUNKT_ZAGR],iceb_u_inttochar(E_PUNKT_ZAGR));/******************/

label=gtk_label_new(gettext("Пунк разгрузки"));
gtk_box_pack_start (GTK_BOX (hbox[E_PUNKT_RAZG]), label, FALSE, FALSE, 0);

data.entry[E_PUNKT_RAZG] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PUNKT_RAZG]), data.entry[E_PUNKT_RAZG], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PUNKT_RAZG], "activate",G_CALLBACK(rasttn2w_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PUNKT_RAZG]),data.rk->punkt_razg.ravno());
gtk_widget_set_name(data.entry[E_PUNKT_RAZG],iceb_u_inttochar(E_PUNKT_RAZG));/******************/

label=gtk_label_new(gettext("Заказчик"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZAKAZCHIK]), label, FALSE, FALSE, 0);

data.entry[E_ZAKAZCHIK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_ZAKAZCHIK]), data.entry[E_ZAKAZCHIK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ZAKAZCHIK], "activate",G_CALLBACK(rasttn2w_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZAKAZCHIK]),data.rk->zakazchik.ravno());
gtk_widget_set_name(data.entry[E_ZAKAZCHIK],iceb_u_inttochar(E_ZAKAZCHIK));/******************/

label=gtk_label_new(gettext("Гос. номер прицепа"));
gtk_box_pack_start (GTK_BOX (hbox[E_GOS_NOMER_PRIC]), label, FALSE, FALSE, 0);

data.entry[E_GOS_NOMER_PRIC] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GOS_NOMER_PRIC]), data.entry[E_GOS_NOMER_PRIC], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GOS_NOMER_PRIC], "activate",G_CALLBACK(rasttn2w_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOS_NOMER_PRIC]),data.rk->gos_nomer_pric.ravno());
gtk_widget_set_name(data.entry[E_GOS_NOMER_PRIC],iceb_u_inttochar(E_GOS_NOMER_PRIC));/******************/



sprintf(strsql,"F2 %s",gettext("Ввод"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввести информацию"));
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(rasttn2w_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(rasttn2w_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Не распечатывать накладную"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(rasttn2w_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rasttn2w_m_key_press(GtkWidget *widget,GdkEventKey *event,class rasttn2w_m_data *data)
{
//char  bros[512];

//printf("vl_zarkateg_key_press\n");
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
void  rasttn2w_m_knopka(GtkWidget *widget,class rasttn2w_m_data *data)
{
int knop=atoi(gtk_widget_get_name(widget));
switch (knop)
 {
  case FK2:
    data->read_rek();
    if(rasttn2w_m_zap(data) != 0)
     return;
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

void    rasttn2w_m_vvod(GtkWidget *widget,class rasttn2w_m_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
switch (enter)
 {
  case E_AVTOPREDP:
    data->rk->avtopredp.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_AVTOPREDP])));
    break;
  case E_FIO_VOD:
    data->rk->fio_vod.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_FIO_VOD])));
    break;
 }

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/********************************/
/*Запись*/
/********************************/
int rasttn2w_m_zap(class rasttn2w_m_data *data)
{
char strsql[1024];

sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",data->rk->gd,data->rk->sklad,data->rk->nomdok.ravno_filtr(),10,data->rk->gos_nomer.ravno_filtr()); 
if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
 return(1);
/*******************
sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",data->rk->gd,data->rk->sklad,data->rk->nomdok.ravno_filtr(),15,data->rk->nom_put_list.ravno_filtr()); 
iceb_sql_zapis(strsql,1,0,data->window);
*******************/

sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",data->rk->gd,data->rk->sklad,data->rk->nomdok.ravno_filtr(),16,data->rk->marka_avt.ravno_filtr()); 
iceb_sql_zapis(strsql,1,0,data->window);


sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",data->rk->gd,data->rk->sklad,data->rk->nomdok.ravno_filtr(),18,data->rk->avtopredp.ravno_filtr()); 
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",data->rk->gd,data->rk->sklad,data->rk->nomdok.ravno_filtr(),19,data->rk->zakazchik.ravno_filtr()); 
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",data->rk->gd,data->rk->sklad,data->rk->nomdok.ravno_filtr(),20,data->rk->fio_vod.ravno_filtr()); 
iceb_sql_zapis(strsql,1,0,data->window);


sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",data->rk->gd,data->rk->sklad,data->rk->nomdok.ravno_filtr(),21,data->rk->punkt_zagr.ravno_filtr()); 
iceb_sql_zapis(strsql,1,0,data->window);


sprintf(strsql,"replace into Dokummat2 values(%d,%d,'%s',%d,'%s')",data->rk->gd,data->rk->sklad,data->rk->nomdok.ravno_filtr(),22,data->rk->punkt_razg.ravno_filtr()); 
iceb_sql_zapis(strsql,1,0,data->window);
return(0);
}
