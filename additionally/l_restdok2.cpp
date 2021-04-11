/*$Id: l_restdok2.c,v 1.34 2013/09/26 09:47:12 sasa Exp $*/
/*23.05.2016	23.02.2004	Белых А.И.	l_restdok2.c
Работа с записями документа (Новый вариант программы)
Если вернули 0-перечитывать вызывающий список
             1-не перечитывать
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "i_rest.h"
#include "l_restdok2.h"
#include "../xpm/cod_tovara.xpm"
#include "../xpm/tovar.xpm"
#include "../xpm/korektir.xpm"
#include "../xpm/udalit.xpm"
#include "../xpm/print.xpm"
#include "../xpm/usluga.xpm"
#include "../xpm/oplata.xpm"
#include "../xpm/hidden.xpm"
#include "../xpm/vihod.xpm"
#include "../xpm/cod_uslugi.xpm"

void restdok2_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class restdok2_data *data);
gboolean   restdok2_key_press(GtkWidget *widget,GdkEventKey *event,class restdok2_data *data);
void  restdok2_knopka(GtkWidget *widget,class restdok2_data *data);
void restdok2_add_columns(GtkTreeView *treeview);
void restdok2_vibor(GtkTreeSelection *selection,class restdok2_data *data);
void restdok2_udzap(class restdok2_data *data);
void restdok2_create_list (class restdok2_data *data);


void     restdok2_vrp(class restdok2_data *data);
void restdok2_rskl(short dd,short md,short gd,const char *nomdok,GtkWidget *wpredok);
void restdok2_rskl1(short dd,short md,short gd,const char *nomdok,const char *skl,const char *imaf,int,const char*,GtkWidget *wpredok);
void  restdok2_sklad_get(GtkWidget *widget,class restdok2_data *data);
void  restdok2_sp_sklad(GtkWidget **windowvoditel,iceb_u_str*,int,GtkWidget *wpredok);
void  restdok2_sp_podr(GtkWidget **widget_podr,iceb_u_str *podrv,GtkWidget *wpredok);
void  restdok2_podr_get(GtkWidget *widget,class restdok2_data *data);
int restdok2_prov_opl(class restdok2_data *data);

void l_restdok2_vgr(class restdok2_data *data);
void restdo2_hbox_gr_mu(class restdok2_data *data);
void l_restdok2_znm(class restdok2_data *data);


extern short metka_crk; //0-расчёт клиентов по подразделениям 1-централизованный
extern SQL_baza	bd;
extern char *name_system;
extern char *organ;
extern char *imabaz;
extern char *putnansi;
extern class iceb_get_dev_data config_dev;
extern uid_t kod_operatora;
extern int  nomer_kas;  //номер кассы в настоящий момент всегда равно нолю так как работа с кассовыми регистраторами не реализована
                          //Эта переменная необходима при записи в таблицу Restkas при списании 

int  l_restdok2(short god,iceb_u_str *nomdok,
GtkWidget *wpredok)
{
printf("%s\n",__FUNCTION__);
restdok2_data data;
char strsql[1024];
SQLCURSOR cur;
SQL_str   row;

data.gd=god;
data.nomdok.new_plus(nomdok->ravno());

//читаем реквизиты шапки документа
sprintf(strsql,"select * from Restdok where god=%d and nomd='%s'",
data.gd,data.nomdok.ravno());

if(iceb_sql_readkey(strsql,&row,&cur,NULL) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"%s %s %s %d",
  gettext("Не найден документ"),
  nomdok->ravno(),
  gettext("Год"),
  god);
  
  repl.plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return(0);
 }

iceb_u_rsdat(&data.dd,&data.md,&data.gd,row[2],2); 
data.mo=atoi(row[3]);
data.nstol.new_plus(row[4]);
data.kodkl.new_plus(row[5]);
data.fio.new_plus(row[6]);
data.koment.new_plus(row[7]);
data.podr=atoi(row[11]);
data.naimpodr.new_plus("");
data.vremz=atol(row[12]);
data.proc_sk=atof(row[14]);

//Выставляем флаг
memset(strsql,'\0',sizeof(strsql));
if(data.kodkl.getdlinna() > 1)
  sprintf(strsql,"rest|%s|%s",imabaz,data.kodkl.ravno());
iceb_sql_flag kl_flag;
if(data.mo == 0)
  rest_flag_kl(&kl_flag,data.kodkl.ravno(),wpredok);
  
sprintf(strsql,"select naik from Restpod where kod=%d",data.podr);
if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
 data.naimpodr.new_plus(row[0]);


//читаем заданные размеры окна
iceb_u_str imaf_nast("restnast.alx");
//iceb_imafn("restnast.alx",&imaf_nast);
iceb_poldan("Размеры окна в пикселах для режима выписки счетов",strsql,imaf_nast.ravno(),wpredok);
char bros[512];

int goriz;
int vert;

iceb_u_polen(strsql,bros,sizeof(bros),1,',');
goriz=atoi(bros);

iceb_u_polen(strsql,bros,sizeof(bros),2,',');
vert=atoi(bros);
int metka_klris=0;
if(iceb_poldan("В режиме выписки счетов клавиши с рисунком",strsql,imaf_nast.ravno(),wpredok) == 0)
 if(iceb_u_SRAV("Вкл",strsql,1) == 0)
  metka_klris=1; /*Клавиши с рисунком*/


data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(vert != 0 && goriz != 0)
 gtk_window_set_default_size (GTK_WINDOW  (data.window),goriz,vert);

//gdk_color_parse("black",&color);
//gtk_widget_modify_bg(data.window,GTK_STATE_NORMAL,&color);


sprintf(strsql,"%s %s",name_system,gettext("Просмотр документа"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 0);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(restdok2_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxv = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxv),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);

data.label_kolstr=gtk_label_new (gettext("Стартовые сальдо по счетам"));


gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),hboxv,FALSE, FALSE, 0);

//читаем настройку разделителя для вертикальных панелей
iceb_u_poldan("Количество пикселов для разделения вертикальных панелей",strsql,".iceb_l_restdok2.dat");
int kol_pix=atoi(strsql);

data.vert_panel=gtk_paned_new(GTK_ORIENTATION_VERTICAL);
gtk_box_pack_start (GTK_BOX (vbox2),data.vert_panel,TRUE, TRUE, 0);
if(kol_pix > 0)
 gtk_paned_set_position(GTK_PANED(data.vert_panel),kol_pix);

gtk_widget_show(data.vert_panel);


//читаем настройку разделителя для горизонтальных панелей
iceb_u_poldan("Количество пикселов для разделения горизонтальных панелей",strsql,".iceb_l_restdok2.dat");
kol_pix=atoi(strsql);

data.gor_panel=gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
//gtk_box_pack_start (GTK_BOX (vbox2),data.gor_panel,TRUE, TRUE, 0);
gtk_paned_add1(GTK_PANED(data.vert_panel),data.gor_panel);

if(kol_pix > 0)
 gtk_paned_set_position(GTK_PANED(data.gor_panel),kol_pix);

//Вставляем в горизонтальный бокс виджеты
restdo2_hbox_gr_mu(&data);



data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_paned_add2(GTK_PANED(data.vert_panel),data.sw);

//Кнопки

if(config_dev.metka_screen == 0) //Нет сенсорного экрана
 {
  GtkWidget *label=gtk_label_new(gettext("Склад"));
  gtk_box_pack_start(GTK_BOX (hboxv),label,FALSE,FALSE,0);

  //Создаём список складов
  restdok2_sp_sklad(&data.windowvoditel,&data.skladv,data.podr,data.window);

  gtk_box_pack_start (GTK_BOX (hboxv), data.windowvoditel, TRUE, TRUE, 0);
  g_signal_connect(GTK_COMBO_BOX(data.windowvoditel), "changed",G_CALLBACK(restdok2_sklad_get),&data);
//  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(data.windowvoditel)->entry),data.skladv.ravno());

  label=gtk_label_new(gettext("Подразделение"));
  gtk_box_pack_start(GTK_BOX (hboxv),label,FALSE,FALSE,0);

  //Создаём список подразделений
  restdok2_sp_podr(&data.widget_podr,&data.podrv,data.window);

  gtk_box_pack_start (GTK_BOX (hboxv), data.widget_podr, TRUE, TRUE, 0);
  g_signal_connect(GTK_COMBO_BOX(data.widget_podr), "changed",G_CALLBACK(restdok2_podr_get),&data);
//  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(data.widget_podr)->entry),data.podrv.ravno());
 }
else
 {
  data.knopka[F_SKLAD]=gtk_button_new_with_label(gettext("Склад"));
  gtk_box_pack_start (GTK_BOX (hboxv), data.knopka[F_SKLAD], TRUE, TRUE, 0);
  g_signal_connect(data.knopka[F_SKLAD], "clicked",G_CALLBACK(restdok2_knopka),&data);

  gtk_widget_set_tooltip_text(data.knopka[F_SKLAD],gettext("Выбор склада"));
  gtk_widget_set_name(data.knopka[F_SKLAD],iceb_u_inttochar(F_SKLAD));
  gtk_widget_show(data.knopka[F_SKLAD]);

  class iceb_u_str spis_skl;
  l_restdok_spis_skl(&spis_skl,data.podr);
  strncpy(bros,spis_skl.ravno(),sizeof(bros)-1);
  
  if(bros[0] != '\0')
   {
    int kolih=iceb_u_pole2(bros,',');
    if(kolih == 0)
     sprintf(strsql,"select kod,naik from Sklad where kod=%s",bros);
    else
     {
      char bros1[50];
      iceb_u_polen(bros,bros1,sizeof(bros1),1,',');
      sprintf(strsql,"select kod,naik from Sklad where kod=%s",bros1);
     }    

    data.skladv.new_plus("");
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     {
      data.skladv.new_plus(row[0]);
      data.skladv.plus(" ");
      data.skladv.plus(row[1]);
     }
   }  
  //склад или равен нулевому байту или имеет значение
  data.label_naim_skl=gtk_label_new (data.skladv.ravno());
  gtk_box_pack_start(GTK_BOX (hboxv), data.label_naim_skl, TRUE, TRUE, 0);

  //***********услуги**********************

  data.knopka[F_PODR]=gtk_button_new_with_label(gettext("Подразделение"));
  gtk_box_pack_start (GTK_BOX (hboxv), data.knopka[F_PODR], TRUE, TRUE, 0);
//  gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[F_PODR], TRUE, TRUE, 0);
  g_signal_connect(data.knopka[F_PODR], "clicked",G_CALLBACK(restdok2_knopka),&data);

  gtk_widget_set_tooltip_text(data.knopka[F_PODR],gettext("Выбор подразделения"));
  gtk_widget_set_name(data.knopka[F_PODR],iceb_u_inttochar(F_PODR));
  gtk_widget_show(data.knopka[F_PODR]);

  memset(bros,'\0',sizeof(bros));
  iceb_u_str imafil("restnast.alx");

  iceb_poldan("Список подразделений предоставляющих услуги",bros,imafil.ravno(),wpredok);

  if(bros[0] != '\0')
   {
    int kolih=iceb_u_pole2(bros,',');
    if(kolih == 0)
     sprintf(strsql,"select kod,naik from Uslpodr where kod=%s",bros);
    else
     {
      char bros1[50];
      iceb_u_polen(bros,bros1,sizeof(bros1),1,',');
      sprintf(strsql,"select kod,naik from Uslpodr where kod=%s",bros1);
     }    

    data.podrv.new_plus("");
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     {
      data.podrv.new_plus(row[0]);
      data.podrv.plus(" ");
      data.podrv.plus(row[1]);
     }
   }  
  //подразделение равно нулевому байту или имеет значение
  data.label_naim_podr=gtk_label_new (data.podrv.ravno());
  gtk_box_pack_start(GTK_BOX (hboxv), data.label_naim_podr, TRUE, TRUE, 0);


 }
 
gtk_widget_show_all(hboxv);
gtk_widget_show(vbox1);
gtk_widget_show(vbox2);


gtk_widget_realize(data.window);
if(metka_klris == 0)
 {
  sprintf(strsql,"F2 %s",gettext("Ввод"));
  data.knopka[FK2]=gtk_button_new_with_label(strsql);
 }
else
 {
  GdkPixbuf *pixbuf=gdk_pixbuf_new_from_xpm_data(tovar_xpm);
  GtkWidget *znak=gtk_image_new_from_pixbuf(pixbuf);
  data.knopka[FK2]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[FK2]),znak);

 }


gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2], TRUE, TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(restdok2_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод нового товарa в счёт"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show_all(data.knopka[FK2]);

if(metka_klris == 0)
 {
  sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
  data.knopka[SFK2]=gtk_button_new_with_label(strsql);
 }
else
 {
  GdkPixbuf *pixbuf=gdk_pixbuf_new_from_xpm_data(korektir_xpm);
  GtkWidget *znak=gtk_image_new_from_pixbuf(pixbuf);
  data.knopka[SFK2]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[SFK2]),znak);
 }

gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(restdok2_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной запси"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show_all(data.knopka[SFK2]);


if(metka_klris == 0)
 {
  sprintf(strsql,"F3 %s",gettext("Удалить"));
  data.knopka[FK3]=gtk_button_new_with_label(strsql);
 }
else
 {
  GdkPixbuf *pixbuf=gdk_pixbuf_new_from_xpm_data(udalit_xpm);
  GtkWidget *znak=gtk_image_new_from_pixbuf(pixbuf);
  data.knopka[FK3]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[FK3]),znak);
 }


gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(restdok2_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удаление записи или документа"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show_all(data.knopka[FK3]);

if(metka_klris == 0)
 {
  sprintf(strsql,"F5 %s",gettext("Печать"));
  data.knopka[FK5]=gtk_button_new_with_label(strsql);
 }
else
 {
  GdkPixbuf *pixbuf=gdk_pixbuf_new_from_xpm_data(print_xpm);
  GtkWidget *znak=gtk_image_new_from_pixbuf(pixbuf);
  data.knopka[FK5]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[FK5]),znak);
 }

gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(restdok2_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка записей"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show_all(data.knopka[FK5]);

if(metka_crk == 0)
 {
  if(metka_klris == 0)
   {
    sprintf(strsql,"F6 %s",gettext("Оплата"));
    data.knopka[FK6]=gtk_button_new_with_label(strsql);
   }
  else
   {
    GdkPixbuf *pixbuf=gdk_pixbuf_new_from_xpm_data(oplata_xpm);
    GtkWidget *znak=gtk_image_new_from_pixbuf(pixbuf);
    data.knopka[FK6]=gtk_button_new();
    gtk_container_add(GTK_CONTAINER(data.knopka[FK6]),znak);
   }

  gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
  g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(restdok2_knopka),&data);
  gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Отметить документ как оплаченный"));
  gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
  gtk_widget_show_all(data.knopka[FK6]);
 }

if(metka_klris == 0)
 {
  sprintf(strsql,"F7 %s",gettext("Послуга"));
  data.knopka[FK7]=gtk_button_new_with_label(strsql);
 }
else
 {
  GdkPixbuf *pixbuf=gdk_pixbuf_new_from_xpm_data(usluga_xpm);
  GtkWidget *znak=gtk_image_new_from_pixbuf(pixbuf);
  data.knopka[FK7]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[FK7]),znak);
 }

gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK7], "clicked",G_CALLBACK(restdok2_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Ввод новой услуги в счет"));
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));
gtk_widget_show_all(data.knopka[FK7]);

if(metka_klris == 0)
 {
  sprintf(strsql,"F8 %s",gettext("Код товара"));
  data.knopka[FK8]=gtk_button_new_with_label(strsql);
 }
else
 {
  GdkPixbuf *pixbuf=gdk_pixbuf_new_from_xpm_data(cod_tovara_xpm);
  GtkWidget *znak=gtk_image_new_from_pixbuf(pixbuf);
  data.knopka[FK8]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[FK8]),znak);
 }

gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK8], "clicked",G_CALLBACK(restdok2_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK8],gettext("Ввод товара по коду товара"));
gtk_widget_set_name(data.knopka[FK8],iceb_u_inttochar(FK8));
gtk_widget_show_all(data.knopka[FK8]);

if(metka_klris == 0)
 {
  sprintf(strsql,"F9 %s",gettext("Код услуги"));
  data.knopka[FK9]=gtk_button_new_with_label(strsql);
 }
else
 {
  GdkPixbuf *pixbuf=gdk_pixbuf_new_from_xpm_data(cod_uslugi_xpm);
  GtkWidget *znak=gtk_image_new_from_pixbuf(pixbuf);
  data.knopka[FK9]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[FK9]),znak);
 }
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK9], "clicked",G_CALLBACK(restdok2_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK9],gettext("Ввод услуги по коду услуги"));
gtk_widget_set_name(data.knopka[FK9],iceb_u_inttochar(FK9));
gtk_widget_show_all(data.knopka[FK9]);

if(metka_klris == 0)
 {
  sprintf(strsql,"%sF9 %s",RFK,gettext("Спрятать"));
  data.knopka[SFK9]=gtk_button_new_with_label(strsql);
 }
else
 {
  GdkPixbuf *pixbuf=gdk_pixbuf_new_from_xpm_data(hidden_xpm);
  GtkWidget *znak=gtk_image_new_from_pixbuf(pixbuf);
  data.knopka[SFK9]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[SFK9]),znak);
 }
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK9],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK9], "clicked",G_CALLBACK(restdok2_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK9],gettext("Спрятать/показать шапку документа"));
gtk_widget_set_name(data.knopka[SFK9],iceb_u_inttochar(SFK9));
gtk_widget_show_all(data.knopka[SFK9]);

if(metka_klris == 0)
 {
  sprintf(strsql,"F10 %s",gettext("Выход"));
  data.knopka[FK10]=gtk_button_new_with_label(strsql);
 }
else
 {
  GdkPixbuf *pixbuf=gdk_pixbuf_new_from_xpm_data(vihod_xpm);
  GtkWidget *znak=gtk_image_new_from_pixbuf(pixbuf);
  data.knopka[FK10]=gtk_button_new();
  gtk_container_add(GTK_CONTAINER(data.knopka[FK10]),znak);
 }
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(restdok2_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show_all(data.knopka[FK10]);


gtk_widget_realize(data.window);

gtk_widget_grab_focus(data.knopka[FK10]);

restdok2_create_list(&data);

gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


return(data.voz);

}


/***********************************/
/*Создаем список для просмотра */
/***********************************/
void restdok2_create_list (class restdok2_data *data)
{
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row;
SQL_str row1;
double suma=0.;

//printf("restdok2_create_list %d\n",data->snanomer);
//data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(restdok2_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(restdok2_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);

model = gtk_list_store_new (NUM_COLUMNS+3,
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_INT,
G_TYPE_INT,
G_TYPE_INT);


sprintf(strsql,"select * from Restdok1 where datd='%d-%d-%d' and \
nomd='%s' order by mz asc,skl asc",
data->gd,data->md,data->dd,data->nomdok.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;
double kolih=0.;
double cena=0.;
double cena_sk=0.;
//int nomerzap=0;
char metka_ras[5];
double suma_skidki=0.;
float proc_sk=0.;
data->suma_k_oplate=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  


  if(data->kodn == atoi(row[4]))
     data->snanomer=data->kolzap;

  //Метка записи
  if(row[3][0] == '0')
   ss[COL_MZ].new_plus("Т");
  else
   ss[COL_MZ].new_plus("П");
  

  //узнаём наименование
  if(atoi(row[3]) == 0) //материал
   {
    
    sprintf(strsql,"select naimat from Material where kodm=%s",
    row[4]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     ss[COL_NAIM].new_plus(row1[0]);
    else
     ss[COL_NAIM].new_plus(" ");
            
   }
  else //услуга
   {
    
    sprintf(strsql,"select naius from Uslugi where kodus=%s",
    row[4]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     ss[COL_NAIM].new_plus(row1[0]);
    else
     ss[COL_NAIM].new_plus(" ");
            
   }
  

  //количество
  kolih=atof(row[6]);
  sprintf(strsql,"%.10g",kolih);
  ss[COL_KOLIH].new_plus(strsql);

  // цена
  cena=atof(row[7]);
  sprintf(strsql,"%.10g",cena);
  ss[COL_CENA].new_plus(strsql);

  // Процент скидки
  proc_sk=atof(row[16]);
  sprintf(strsql,"%.10g%%",proc_sk);
  ss[COL_PROC_SK].new_plus(strsql);

  //Цена со скидкой
  cena_sk=cena-cena*proc_sk/100.;  
  cena_sk=iceb_u_okrug(cena_sk,0.01);
  ss[COL_CENA_SK].new_plus(cena_sk);

  suma_skidki+=kolih*(cena-cena_sk);
  
  // сумма
  suma=kolih*cena_sk;
  data->suma_k_oplate+=suma;
  sprintf(strsql,"%.2f",suma);  
  ss[COL_SUMA].new_plus(strsql);


  //МЕТКА СПИСАНИЯ
  if(atoi(row[9]) == 1)
   ss[COL_SPISANIE].new_plus(gettext("Списано"));
  else
   ss[COL_SPISANIE].new_plus("");
  

  
  memset(metka_ras,'\0',sizeof(metka_ras));
  if(row[14][0] == '1')
   metka_ras[0]='*';
   
  gtk_list_store_append (model, &iter);


   gtk_list_store_set (model, &iter,
   COL_SKL,row[2],
   COL_MZ,ss[COL_MZ].ravno(),
   COL_KOD,row[4],
   COL_MR,metka_ras,
   COL_NAIM,ss[COL_NAIM].ravno(),
   COL_EI,row[5],
   COL_KOLIH,ss[COL_KOLIH].ravno(),
   COL_CENA,ss[COL_CENA].ravno(),
   COL_CENA_SK,ss[COL_CENA_SK].ravno(),
   COL_PROC_SK,ss[COL_PROC_SK].ravno(),
   COL_SUMA,ss[COL_SUMA].ravno(),
   COL_KOMENT,row[8],
   COL_SPISANIE,ss[COL_SPISANIE].ravno(),
   COL_VREM,iceb_u_vremzap(row[11]),
   COL_KTO,iceb_u_kszap(row[10],0),
   NUM_COLUMNS,data->kolzap,
   NUM_COLUMNS+1,atoi(row[3]),
   NUM_COLUMNS+2,atoi(row[13]),
   -1);

  data->kolzap++;
 }
data->kodn=0;
gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

restdok2_add_columns (GTK_TREE_VIEW (data->treeview));

if(data->mo == 1)
 {
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна

    if(data->kolzap == 0)
      gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
    if(metka_crk == 0)
      gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK7]),FALSE);//Недоступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK8]),FALSE);//Недоступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),FALSE);//Недоступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
 }
else
  if(data->kolzap == 0)
   {
//    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
    if(metka_crk == 0)
      gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),FALSE);//Недоступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
   }
  else
   {
//    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
    if(metka_crk == 0)
     gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK6]),TRUE);//Доступна
    gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
   }


gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

if(data->kolzap > 0)
 {
  if(data->snanomer  > data->kolzap-1)
     data->snanomer= data->kolzap-1;
      
//  printf("poz=%d\n",data->snanomer);

  sprintf(strsql,"%d",data->snanomer);
  GtkTreePath *path=gtk_tree_path_new_from_string(strsql);

//  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,TRUE,0,0);
  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

  gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
  gtk_tree_path_free(path);
 }

iceb_u_str stroka;
iceb_u_str zagolov;

sprintf(strsql,"%s:%s %s %d.%d.%d %s:%d",
gettext("Счёт"),
data->nomdok.ravno(),
gettext("от"),
data->dd,data->md,data->gd,
gettext("Количество записей"),
data->kolzap);

zagolov.plus(strsql);
//printf("data->vremz=%ld\n",data->vremz);
sprintf(strsql,"%s:%s",gettext("Дата заказа"),iceb_u_vremzap(data->vremz));

zagolov.ps_plus(strsql);

sprintf(strsql," %s: %d %s",
gettext("Подразделение"),
data->podr,data->naimpodr.ravno());
zagolov.plus(strsql);

if(data->nstol.getdlinna() > 1)
 {
  zagolov.ps_plus(gettext("Номер столика"));
  zagolov.plus(":");
  zagolov.plus(data->nstol.ravno());
 }

if(data->kodkl.getdlinna() > 1)
 {
  zagolov.ps_plus(gettext("Код клиента"));
  zagolov.plus(":");
  zagolov.plus(data->kodkl.ravno());
 }

if(data->fio.getdlinna() > 1)
 {
  zagolov.ps_plus(gettext("Фамилия клиента"));
  zagolov.plus(":");
  zagolov.plus(data->fio.ravno());
 }


if(data->koment.getdlinna() > 1)
 {
  zagolov.ps_plus(gettext("Коментарий"));
  zagolov.plus(":");
  zagolov.plus(data->koment.ravno());
 }


if(data->proc_sk != 0.)
 {
  zagolov.ps_plus(gettext("Процент скидки"));
  zagolov.plus(":");
  zagolov.plus(data->proc_sk);
  suma_skidki=iceb_u_okrug(suma_skidki,0.01);
 }

if(suma_skidki == 0.)
  sprintf(strsql,"%s:%.2f",gettext("Сумма к оплате"),data->suma_k_oplate);
else
  sprintf(strsql,"%s:%.2f %s:%.2f",
  gettext("Сумма к оплате"),data->suma_k_oplate,
  gettext("Сумма скидки"),suma_skidki);

zagolov.ps_plus(strsql);
zagolov.plus("  ");
if(data->mo == 0)
  zagolov.plus(gettext("Счёт не оплачено"));
if(data->mo == 1)
  zagolov.plus(gettext("Счёт оплачено"));
data->saldo=0.;
if(data->kodkl.getdlinna() > 1)
 {
  zagolov.ps_plus(gettext("Сальдо"));
  zagolov.plus(":");

  data->saldo=rest_sal_kl(data->kodkl.ravno(),data->window);
  zagolov.plus(data->saldo);
 }
 
gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());


gtk_widget_show(data->label_kolstr);

if(data->metka_hapki == 1)
  gtk_widget_hide(data->label_kolstr);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));


}

/*****************/
/*Создаем колонки*/
/*****************/

void restdok2_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;
//GtkTreeModel *model = gtk_tree_view_get_model (treeview);
//GdkColor color;

//printf("restdok2_add_columns\n");

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("П/д"), renderer,"text", COL_SKL,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,"M", renderer,"text", COL_MZ,NULL);

renderer = gtk_cell_renderer_text_new ();
//g_object_set(G_OBJECT(renderer),"foreground","white",NULL);
//g_object_set(G_OBJECT(renderer),"background","black",NULL);


gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Код"), renderer,"text", COL_KOD,NULL);
renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
"*", renderer,"text", COL_MR,NULL);

gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Наименование"), renderer,
"text", COL_NAIM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Е/и"), renderer,
"text", COL_EI,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Количество"), renderer,
"text", COL_KOLIH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Цена"), renderer,
"text", COL_CENA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("П/c"), renderer,
"text", COL_PROC_SK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Цена со скидкой"), renderer,
"text", COL_CENA_SK,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Сумма"), renderer,
"text", COL_SUMA,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Коментарий"), renderer,
"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Метка списания"), renderer,"text", COL_SPISANIE,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

//printf("restdok2_add_columns end\n");

}

/****************************/
/*Выбор строки*/
/**********************/

void restdok2_vibor(GtkTreeSelection *selection,class restdok2_data *data)
{
//printf("restdok2_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *skl;
gchar *kod;
gint  mz;
gint  nomer;
gint  nomerzap;


gtk_tree_model_get(model,&iter,COL_SKL,&skl,COL_KOD,&kod,NUM_COLUMNS,&nomer,\
NUM_COLUMNS+1,&mz,NUM_COLUMNS+2,&nomerzap,-1);

data->sklv=atoi(skl);
data->kodv=atoi(kod);
data->mzv=mz;
data->snanomer=nomer;
data->nz=nomerzap;

g_free(skl);
g_free(kod);

//printf("sklv=%d %d %d %d %d\n",data->sklv,data->kodv,nomer,mz,nomerzap);

}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  restdok2_knopka(GtkWidget *widget,class restdok2_data *data)
{
char    strsql[300];
iceb_u_str repl;
iceb_u_str kodv;
//int kodvskl=0;
iceb_u_str kod;
SQL_str row;
SQLCURSOR cur;
time_t vremo=0;
kod.plus("");
kodv.plus("");
int kodt=0;

int knop=atoi(gtk_widget_get_name(widget));
//g_print("restdok2_knopka knop=%d\n",knop);
//data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch ((gint)knop)
 {
  case F_SKLAD:

      if(l_spisok(0,0,&kodv,data->podr,data->window) == 0)
       {
        sprintf(strsql,"select kod,naik from Sklad where kod=%d",kodv.ravno_atoi());

        data->skladv.new_plus("");
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
         {
          data->skladv.new_plus(row[0]);
          if(data->metka_show == 1)
            l_restdok2_vgr(data);

          data->skladv.plus(" ");
          data->skladv.plus(row[1]);
         }
        gtk_label_set_text(GTK_LABEL(data->label_naim_skl),data->skladv.ravno());
       }
      return;

  case F_PODR:

      if(l_spisok(2,0,&kodv,0,data->window) == 0)
       {
        sprintf(strsql,"select kod,naik from Uslpodr where kod=%d",kodv.ravno_atoi());

        data->podrv.new_plus("");
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
         {
          data->podrv.new_plus(row[0]);
          if(data->metka_show == 2)
            l_restdok2_vgr(data);
          data->podrv.plus(" ");
          data->podrv.plus(row[1]);
         }
        gtk_label_set_text(GTK_LABEL(data->label_naim_podr),data->podrv.ravno());
       }
      return;

  case FK2:
    if(restdok2_prov_opl(data) != 0)
      return;

    if(data->metka_show == 1)
     {
      data->metka_show=0;
      gtk_widget_hide(data->gor_panel);
      return;
     }
    gtk_label_set_text(GTK_LABEL(data->label_mat_usl),gettext("Мат"));

    data->metka_show=1;
    if(config_dev.metka_screen == 0)
      g_signal_emit_by_name(GTK_COMBO_BOX(data->windowvoditel),"changed");
    
    data->metka_mat_usl=0;
    l_restdok2_vgr(data);
    
  
    return;  

  case SFK2:
    if(restdok2_prov_opl(data) != 0)
      return;
    if(data->kolzap == 0)
      return;
    if(l_restdok_v(data->dd,data->md,data->gd,data->nomdok.ravno(),data->sklv,data->kodv,data->podr,1,data->mzv,data->nz,data->proc_sk,data->kodkl.ravno(),data->window) == 0)
     {
      if(data->metka_kontr_ost == 1)
        l_restdok2_vmu(data);
      restdok2_create_list(data);
     }
    return;  

  case FK3:
    if(restdok2_prov_opl(data) != 0)
      return;
    restdok2_udzap(data);
    return;  


  case FK5:
    if(data->kolzap == 0)
      return;
    restdok2_vrp(data);
    return;  

  case FK6:
    if(metka_crk == 1)
      return;
    if(data->mo == 1)
      return;
    if(data->kolzap == 0)
      return;
    
    repl.new_plus(gettext("Счёт оплачен ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
     return;
    
    time(&vremo); 
    
    sprintf(strsql,"update Restdok set mo=1,vremo=%ld where god=%d and nomd='%s'",
    vremo,data->gd,data->nomdok.ravno());
    printf("strsql=%s\n",strsql);
    if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
     return;

    if(data->kodkl.getdlinna() > 1)
     {    
      //Если сальдо отрицательное - значит он оплатил эту сумму делаем запись внос налички
      if(data->saldo < -0.009)
        zap_v_kas(nomer_kas,data->kodkl.ravno(),data->saldo*-1,0,\
        "",0,0,0,data->podr,kod_operatora,"");
       
      //Записываем списание суммы если в счёте введён код клиента для того чтобы можно было получить 
      //реальное сальдо по клиенту
       
        zap_v_kas(nomer_kas,data->kodkl.ravno(),data->suma_k_oplate*-1,2,\
        data->nomdok.ravno(),data->dd,data->md,data->gd,data->podr,kod_operatora,data->koment.ravno());
      }
    data->mo=1;
    data->voz=1;

    restdok2_create_list(data);

    return;  


  case FK7:
    if(data->mo == 1)
      return;

    if(data->metka_show == 2)
     {
      data->metka_show=0;
//      gtk_widget_hide(data->hbox_gr_mu);
      gtk_widget_hide(data->gor_panel);
      return;
     }
    data->metka_show=2;
  
    if(config_dev.metka_screen == 0)
      g_signal_emit_by_name(GTK_COMBO_BOX(data->widget_podr),"changed");

    gtk_label_set_text(GTK_LABEL(data->label_mat_usl),gettext("Усл."));
    data->metka_mat_usl=1;
    l_restdok2_vgr(data);

    return;  

  case FK8:
    if(data->mo == 1)
      return;
    
    if(config_dev.metka_screen == 0)
     {
      g_signal_emit_by_name(GTK_COMBO_BOX(data->windowvoditel),"changed");
      repl.new_plus(gettext("Введите код товара"));

      if(iceb_menu_vvod1(&repl,&kod,20,"",data->window) != 0)    
        return;  
     }

    if(config_dev.metka_screen == 1)
     {
      if(iceb_mous_klav(gettext("Введите код товара"),&kod,10,0,0,0,data->window) != 0)
          return;
     }
    if(l_restdok_v(data->dd,data->md,data->gd,data->nomdok.ravno(),data->skladv.ravno_atoi(),kod.ravno_atoi(),data->podr,0,0,0,data->proc_sk,data->kodkl.ravno(),data->window) == 0)
      {
       if(data->metka_kontr_ost == 1)
         l_restdok2_vmu(data);
       data->kodn=kodt;
       restdok2_create_list(data);
      }
    return;  

  case FK9:
    if(data->mo == 1)
      return;


    if(config_dev.metka_screen == 0)
     {
      g_signal_emit_by_name(GTK_COMBO_BOX(data->widget_podr),"changed");
      repl.new_plus(gettext("Введите код услуги"));
    
      if(iceb_menu_vvod1(&repl,&kod,20,"",data->window) != 0)
        return;  
     }
    if(config_dev.metka_screen == 1)
     if(iceb_mous_klav(gettext("Введите код услуги"),&kod,10,0,0,0,data->window) != 0)
       return;

    if(l_restdok_v(data->dd,data->md,data->gd,data->nomdok.ravno(),data->skladv.ravno_atoi(),kod.ravno_atoi(),data->podr,0,1,0,data->proc_sk,data->kodkl.ravno(),data->window) == 0)
       restdok2_create_list(data);

    return;  
  
  case SFK9: //Спрятать шапку

    data->metka_hapki++;

    if(data->metka_hapki == 2)
     data->metka_hapki=0;      

    if(data->metka_hapki == 1)
      gtk_widget_hide(data->label_kolstr);
    else
      gtk_widget_show(data->label_kolstr);
      
    return;
           
  case FK10:
//    printf("restdok2_knopka F10\n");
    l_restdok2_znm(data);
    
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   restdok2_key_press(GtkWidget *widget,GdkEventKey *event,class restdok2_data *data)
{
iceb_u_str repl;
//printf("restdok2_key_press keyval=%d state=%d\n",
//event->keyval,event->state);

switch(event->keyval)
 {

  case GDK_KEY_F2:
    if(data->kl_shift == 0)   
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    if(data->kl_shift == 1)   
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");
    return(TRUE);
   
  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
    return(TRUE);

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");
    return(TRUE);

  case GDK_KEY_F6:
    if(metka_crk == 0)
      g_signal_emit_by_name(data->knopka[FK6],"clicked");
    return(TRUE);

  case GDK_KEY_F7:
    g_signal_emit_by_name(data->knopka[FK7],"clicked");
    return(TRUE);

  case GDK_KEY_F8:
    g_signal_emit_by_name(data->knopka[FK8],"clicked");
    return(TRUE);


  case GDK_KEY_F9:
    if(data->kl_shift == 0)   
      g_signal_emit_by_name(data->knopka[FK9],"clicked");
    if(data->kl_shift == 1)   
      g_signal_emit_by_name(data->knopka[SFK9],"clicked");
    return(TRUE);

  
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(TRUE);

  case ICEB_REG_L:
  case ICEB_REG_R:
  //  printf("restdok2_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
  //  printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void restdok2_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class restdok2_data *data)
{
//printf("restdok2_v_row корректировка\n");
g_signal_emit_by_name(data->knopka[SFK2],"clicked");
}

/****************************/
/*Удалить запись*/
/****************************/

void restdok2_udzap(class restdok2_data *data)
{
char strsql[512];

iceb_u_str titl;
iceb_u_str zagol;

sprintf(strsql,"%s %s",name_system,gettext("Удалить"));
titl.plus(strsql);
zagol.plus(gettext("Выберите нужное"));
iceb_u_spisok mv;

mv.plus(gettext("Удалить выбранную запись"));
mv.plus(gettext("Удалить документ"));

int nomer=iceb_menu_mv(&titl,&zagol,&mv,0,data->window);

iceb_u_str repl;

switch(nomer)
 {
  case -1:
    return;
    
  case 0:
    if(data->kolzap == 0)
      return;
    repl.plus(gettext("Удалить запись ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return;



    sprintf(strsql,"delete from Restdok1 where datd='%d-%d-%d' and \
    nomd='%s' and skl=%d and mz=%d and kodz=%d",
    data->gd,data->md,data->dd,
    data->nomdok.ravno(),
    data->sklv,data->mzv,data->kodv);

//    printf("restdok2_v_udzap %s\n",strsql);

    if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
     return;
    
    break;
  
  case 1:

    repl.plus(gettext("Удалить документ ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return;
  
    if(restdok_uddok(data->dd,data->md,data->gd,data->nomdok.ravno(),data->window) == 0)
     {
      g_signal_emit_by_name(data->knopka[FK10],"clicked");
      data->voz=1;
      return;
     }    
        
    break;

}

restdok2_create_list(data);

}
/********************************/
/*меню выбора режима печати */
/*****************************/
void     restdok2_vrp(class restdok2_data *data)
{

//char bros[300];

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Распечатка счёта"));//0
punkt_m.plus(gettext("Распечатка по складам"));//1


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);

switch (nomer)
 {
  case 0:
    restdok_rshet(data->dd,data->md,data->gd,data->nomdok.ravno(),data->window);
    return;
  case 1:
    restdok2_rskl(data->dd,data->md,data->gd,data->nomdok.ravno(),data->window);
    restdok2_create_list(data); //после распечатки по складам выставляються метки
    return;
 }

}
/*************************************/
/*Распечатка по складам              */
/*************************************/

void restdok2_rskl(short dd,short md,short gd,
const char *nomdok,
GtkWidget *wpredok)
{

SQLCURSOR cur;
SQL_str   row;
int       kolskl;
char      strsql[400];
iceb_u_str podr;
iceb_u_str naimpodr;


podr.new_plus("");  
naimpodr.new_plus("");

//читаем шапку

sprintf(strsql,"select podr from Restdok where datd='%d-%d-%d' and \
nomd='%s'",gd,md,dd,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  podr.new_plus(row[0]);  

  sprintf(strsql,"select naik from Restpod where kod=%s",podr.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
   naimpodr.new_plus(row[0]);

 }

//Определяем склады

sprintf(strsql,"select distinct skl from Restdok1 where datd='%d-%d-%d' and \
nomd='%s' and mz=0 and mr=0 order by skl asc",gd,md,dd,nomdok);
if((kolskl=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolskl == 0)
 return;

iceb_u_spisok skladi;

while(cur.read_cursor(&row) != 0)
  skladi.plus(row[0]);

char imaf[100];

for(int i=0; i < kolskl; i++)
 {
  sprintf(imaf,"skl%s-%d.lst",skladi.ravno(i),getpid());  
  restdok2_rskl1(dd,md,gd,nomdok,skladi.ravno(i),imaf,podr.ravno_atoi(),naimpodr.ravno(),wpredok);

 }  

}
/****************************************/
/*Распечатка по конкретному складу*/
/***********************************/
void restdok2_rskl1(short dd,short md,short gd,
const char *nomdok,
const char *skl,
const char *imaf,
int podr,
const char *naimpodr,
GtkWidget *wpredok)
{
SQLCURSOR cur;
SQLCURSOR cur1;
SQL_str   row;
SQL_str   row1;
int       kolstr;
char      strsql[512];

char name_printer[512];
sprintf(strsql,"Принтер для склада %s",skl);
iceb_u_str nast("restnast.alx");

if(iceb_poldan(strsql,name_printer,nast.ravno(),wpredok) != 0)
 {  
//  printf("restdok2_rskl1 %s \n%s\n",strsql,nast.ravno());
  iceb_u_str repl;
  repl.plus(gettext("Не найдена настройка"));
  repl.plus("!!!");
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return;
  
 }

sprintf(strsql,"select kodz,ei,kolih from Restdok1 where datd='%d-%d-%d' and \
nomd='%s' and skl=%s and mz=0 and mr=0",gd,md,dd,nomdok,skl);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 return;
 
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;

 }
iceb_u_startfil(ff);
time_t vrem;
struct tm *bf;
time(&vrem);
bf=localtime(&vrem);

fprintf(ff,"\n%s N%s\n",gettext("Счёт"),nomdok);

fprintf(ff,"%s:%d %s\n",gettext("Подразделение"),podr,naimpodr);
fprintf(ff,"%s %d.%d.%d%s %s:%02d:%02d\n",
gettext("Распечатано"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

class iceb_u_str naim("");

fprintf(ff,"\
----------------------------------------------\n");

fprintf(ff,"%s\n",gettext(" Код |Наименование             |Ед.из.|Колич.|"));
/*
12345 1234567890123456789012345 123456 123456
*/
fprintf(ff,"\
----------------------------------------------\n");

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"select naimat from Material where kodm=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    naim.new_plus(row1[0]);            
  else
    naim.new_plus("");  
  fprintf(ff,"%-5s %-*.*s %-*.*s %6.6g\n",
  row[0],
  iceb_u_kolbait(25,naim.ravno()),
  iceb_u_kolbait(25,naim.ravno()),
  naim.ravno(),
  iceb_u_kolbait(6,row[1]),
  iceb_u_kolbait(6,row[1]),
  row[1],
  atof(row[2]));
  
  if(strlen(naim.ravno()) > 15)
   fprintf(ff,"%-5s %s\n","",iceb_u_adrsimv(15,naim.ravno()));
 }
fprintf(ff,"\
----------------------------------------------\n");

iceb_podpis(kod_operatora,ff,wpredok);

fclose(ff);

if(iceb_print(imaf,1,name_printer,"",wpredok) == 0)
 {
  sprintf(strsql,"update Restdok1 set mr=1 where datd='%d-%d-%d' and \
nomd='%s' and skl=%s and mr=0 and mz=0",gd,md,dd,nomdok,skl);

  iceb_sql_zapis(strsql,1,0,wpredok);
 }

unlink(imaf);

}
/*************************************/
/*Обработчик сигнала выбора склада*/
/************************************/
void  restdok2_sklad_get(GtkWidget *widget,class restdok2_data *data)
{
printf("%s\n",__FUNCTION__);

char strsql[512];
SQL_str row;
class iceb_u_str kod("");


data->skladv.new_plus(gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT(widget)));

if(data->skladv.getdlinna() <= 1)
 return;

if(iceb_u_polen(data->skladv.ravno(),&kod,1,' ') == 0)
 data->skladv.new_plus(kod.ravno());
  
sprintf(strsql,"select naik from Sklad where kod=%s",data->skladv.ravno_filtr_dpp());
printf("%s-%s\n",__FUNCTION__,strsql);
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s-%s %s !!!",__FUNCTION__,gettext("Не найден склад"),data->skladv.ravno());
  iceb_menu_soob(strsql,NULL);
  data->skladv.new_plus("");
//  gtk_entry_set_text(GTK_ENTRY(widget),"");
  return; 
 }

if(data->metka_show == 1)
  l_restdok2_vgr(data);
//printf("restdok2_sklad_get:data->metka_show=%d\n",data->metka_show);
data->skladv.plus(" ");
data->skladv.plus(row[0]);
 
//gtk_entry_set_text(GTK_ENTRY(widget),data->skladv.ravno());
//gtk_editable_select_region(GTK_EDITABLE(widget),0,strlen(data->skladv.ravno()));

}
/***********************/
/*создание списка складов*/
/**************************/

void  restdok2_sp_sklad(GtkWidget **windowvoditel,iceb_u_str *skladv,int podr,GtkWidget *wpredok)
{
printf("%s\n",__FUNCTION__);

//GList *glist=NULL;

class iceb_u_str spisok("");
char strsql[512];

l_restdok_spis_skl(&spisok,podr);

if(spisok.getdlinna() <= 1)
  iceb_menu_soob(gettext("В файле настройки не введён список складов!"),wpredok);

int kolih=iceb_u_pole2(spisok.ravno(),',');

if(kolih == 0 && spisok.ravno()[0] != '\0')
  kolih=1;

class iceb_u_str kod("");
class iceb_u_spisok sklad;
class iceb_u_str    naim;
SQLCURSOR cur;
SQL_str   row;

for(int i=1; i <= kolih; i++)
 {
  
  if(kolih > 1)
    iceb_u_polen(spisok.ravno(),&kod,i,',');
  else
   kod.new_plus(spisok.ravno());
   
  sprintf(strsql,"select naik from Sklad where kod=%s",kod.ravno());
  naim.new_plus(kod.ravno());        

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    naim.plus(" ",row[0]);        

  if(i == 1)
   skladv->new_plus(naim.ravno());

  sklad.plus(naim.ravno());

 }

iceb_pm_vibor(&sklad,windowvoditel,0);
printf("%s\n",__FUNCTION__);

}

/*************************************/
/*Обработчик сигнала выбора подразделения*/
/************************************/
void  restdok2_podr_get(GtkWidget *widget,class restdok2_data *data)
{
printf("%s\n",__FUNCTION__);
char strsql[512];
SQL_str row;
class iceb_u_str kod("");


data->podrv.new_plus(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget)));
if(iceb_u_polen(data->podrv.ravno(),&kod,1,' ') == 0)
 data->podrv.new_plus(kod.ravno());
 
sprintf(strsql,"select naik from Uslpodr where kod=%s",data->podrv.ravno_filtr_dpp());
SQLCURSOR cur;
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"Не найденo подразделение %s !!!",data->podrv.ravno());
  repl.plus(strsql);
  iceb_menu_soob(&repl,NULL);
  data->podrv.new_plus("");
//  gtk_entry_set_text(GTK_ENTRY(widget),"");
  return; 
 }
if(data->metka_show == 2)
  l_restdok2_vgr(data);

data->podrv.plus(" ",row[0]);
 
//gtk_entry_set_text(GTK_ENTRY(widget),data->podrv.ravno());
//gtk_editable_select_region(GTK_EDITABLE(widget),0,strlen(data->podrv.ravno()));

}

/***********************/
/*создание списка подразделений*/
/**************************/

void  restdok2_sp_podr(GtkWidget **widget_podr,iceb_u_str *podrv,GtkWidget *wpredok)
{
printf("%s\n",__FUNCTION__);

//GList *glist=NULL;

iceb_u_str imafil("restnast.alx");
class iceb_u_str spisok("");

iceb_poldan("Список подразделений предоставляющих услуги",&spisok,imafil.ravno(),NULL);
if(spisok.getdlinna() <= 1)
 iceb_menu_soob(gettext("В файле настройки не введён \"Список подразделений предоставляющих услуги\""),wpredok);
int kolih=iceb_u_pole2(spisok.ravno(),',');

if(kolih == 0 && spisok.ravno()[0] != '\0')
  kolih=1;
class iceb_u_str kod("");
class iceb_u_spisok sklad;
iceb_u_str    naim;
char strsql[512];
SQLCURSOR cur;
SQL_str   row;

for(int i=1; i <= kolih; i++)
 {

  if(kolih > 1)
    iceb_u_polen(spisok.ravno(),&kod,i,',');
  else
   kod.new_plus(spisok.ravno());
   
  sprintf(strsql,"select naik from Uslpodr where kod=%s",kod.ravno());
  naim.new_plus(kod.ravno());        

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    naim.plus(" ",row[0]);        

  if(i == 1)
   podrv->new_plus(naim.ravno());

  sklad.plus(naim.ravno());

 }

/*******************
*widget_podr=gtk_combo_new();

for(int i=0 ; i < kolih; i++)
  glist=g_list_append(glist,(void*)sklad.ravno(i));

if(kolih == 0)
 {
  podrv->new_plus("");
  glist=g_list_append(glist,spisok.ravno());
 }

gtk_combo_set_popdown_strings(GTK_COMBO(*widget_podr),glist);
gtk_combo_disable_activate(GTK_COMBO(*widget_podr));
**********************/

iceb_pm_vibor(&sklad,widget_podr,0);

}
/************************/
/*Проверка оплаты документа*/
/*****************************/
//Если вернули 0-не оплачен 1-оплачен
int restdok2_prov_opl(class restdok2_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
sprintf(strsql,"select mo from Restdok where god=%d and nomd='%s'",
data->gd,data->nomdok.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s-%s !",__FUNCTION__,gettext("Не найдена шапка документа"));
  iceb_menu_soob(strsql,data->window);
  return(0);
 } 
data->mo=atoi(row[0]);
if(data->mo == 1)
 {
  restdok2_create_list(data);
  return(1);   
 }

return(0);


}

/*******************************************************************/

/*********************************************************************/

/***********************************/
/*Стат на нужную строку*/
/************************************/
void l_restdok2_setstr(class restdok2_data_sp *data)
{
//printf("l_restdok2_setstr-Стать на нужную строку\n");

if(data->snanomer  > data->kolzap-1)
   data->snanomer= data->kolzap-1;
if(data->snanomer < 0)
   data->snanomer=0;
      
char strsql[56];
sprintf(strsql,"%d",data->snanomer);
GtkTreePath *path=gtk_tree_path_new_from_string(strsql);

//  gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,TRUE,0,0);
gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE,0,0);

gtk_tree_view_set_cursor(GTK_TREE_VIEW (data->treeview),path,NULL,FALSE);
gtk_tree_path_free(path);
}


/*************************/
/*Кнопка вниз нажата*/
/**********************************/
void  l_restdok2_knopka_vniz_press(GtkWidget *widget,class restdok2_data_sp *data)
{
//printf("кнопка вниз нажата\n");
data->metka_released=1;
while(data->metka_released == 1)
 {
  data->snanomer++;
//  printf("вниз-%d\n",data->snanomer);

  l_restdok2_setstr(data); //стать на нужную строку

  g_usleep(SLEEP_STR);

  iceb_refresh();  //обязательно после sleep
 }


}
/*************************/
/*Кнопка вверх нажата*/
/**********************************/

void  l_restdok2_knopka_vverh_press(GtkWidget *widget,class restdok2_data_sp *data)
{
//printf("кнопка вверх нажата\n");
data->metka_released=1;
while(data->metka_released == 1)
 {
  data->snanomer--;
//  printf("вверх-%d\n",data->snanomer);

  l_restdok2_setstr(data); //стать на нужную строку


  g_usleep(SLEEP_STR);

  iceb_refresh();  //обязательно после sleep
 }


}
/*************************/
/*Кнопка вниз/вверх отжата*/
/**********************************/
void  l_restdok2_knopka_released(GtkWidget *widget,class restdok2_data_sp *data)
{
//printf("кнопка отжата\n");
data->metka_released=0;
}
/*****************************/
/*Обработчик нажатия кнопки выбора из списка материалов и услуг */
/*****************************/
void  restdok2_mu_knopka(GtkWidget *widget,class restdok2_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("restdok2_knopka knop=%d\n",knop);
//data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается
int kod=0;

if(data->metka_mat_usl == 0)
 kod=data->skladv.ravno_atoi();
if(data->metka_mat_usl == 1)
 kod=data->podrv.ravno_atoi();
 
switch ((gint)knop)
 {
  case MU_FK_VIBOR:

//      printf("restdok2_mu_knopka:kod=%d data->kod_matus=%d\n",kod,data->kod_matus);
      
      if(l_restdok_v(data->dd,data->md,data->gd,data->nomdok.ravno(),kod,data->kod_matus,data->podr,0,data->metka_mat_usl,0,data->proc_sk,data->kodkl.ravno(),data->window) == 0)
       {
        if(data->metka_mat_usl == 0)
         if(data->metka_kontr_ost == 1)
           l_restdok2_vmu(data);
        data->kodn=data->kod_matus;
        restdok2_create_list(data);
       }

      return;

  case MU_FK_OSTATOK:

    data->metka_kontr_ost++;

    if(data->metka_kontr_ost > 1)
     data->metka_kontr_ost=0;

    l_restdok2_vmu(data);
    return;
 }
}

/********************************************************/
/*Создаём горизонтальный бокс со списками групп и материалов/услуг*/
/*******************************************************************/

void restdo2_hbox_gr_mu(class restdok2_data *data)
{
GtkWidget *hbox_gr = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1); //Горизонтальный бокс для размещения групп
gtk_box_set_homogeneous (GTK_BOX(hbox_gr),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox_mu = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1); //Горизонтальный бокс для размещения материаллов/услуг
gtk_box_set_homogeneous (GTK_BOX(hbox_mu),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_paned_add1(GTK_PANED(data->gor_panel),hbox_gr);
gtk_paned_add2(GTK_PANED(data->gor_panel),hbox_mu);

GtkWidget *vbox_kn1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1); //Вертикальный бокс для кнопок выбора группы
gtk_box_set_homogeneous (GTK_BOX(vbox_kn1),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox_kn2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1); //Вертикальный бокс для кнопок выбора материала/услуги
gtk_box_set_homogeneous (GTK_BOX(vbox_kn2),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


//вставляем окно для отображения списка групп
data->spisok_gr.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data->spisok_gr.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data->spisok_gr.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (hbox_gr), data->spisok_gr.sw, TRUE, TRUE, 0);


//вставляем вертикальный бокс для кнопок выбора записи
gtk_box_pack_start(GTK_BOX (hbox_gr),vbox_kn1,FALSE,FALSE,0);


//вставляем окно для отображения списка материалов/услуг
data->spisok_mu.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data->spisok_mu.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data->spisok_mu.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (hbox_mu), data->spisok_mu.sw, TRUE, TRUE, 0);

//вставляем вертикальный бокс для кнопок выбора материала/услуги
gtk_box_pack_start(GTK_BOX (hbox_mu),vbox_kn2,FALSE,FALSE,0);

//вставляем кнопки для выбора групп в вертикальный бокс

data->label_mat_usl=gtk_label_new ("***");


gtk_box_pack_start (GTK_BOX (vbox_kn1),data->label_mat_usl,FALSE, FALSE, 0);

GtkWidget *label=gtk_label_new ("+");

data->knopka_gr[GR_FK_VVERH]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data->knopka_gr[GR_FK_VVERH]),label);

gtk_box_pack_start(GTK_BOX(vbox_kn1), data->knopka_gr[GR_FK_VVERH], TRUE, TRUE, 0);
g_signal_connect(data->knopka_gr[GR_FK_VVERH], "pressed",G_CALLBACK(l_restdok2_knopka_vverh_press),&data->spisok_gr);
g_signal_connect(data->knopka_gr[GR_FK_VVERH], "released",G_CALLBACK(l_restdok2_knopka_released),&data->spisok_gr);
//gtk_widget_set_tooltip_text(data->knopka_gr[GR_FK_VVERH],gettext("Выбор отмеченной записи"));
gtk_widget_set_name(data->knopka_gr[GR_FK_VVERH],iceb_u_inttochar(GR_FK_VVERH));
gtk_widget_show(data->knopka_gr[GR_FK_VVERH]);
gtk_widget_show(label);

label=gtk_label_new ("-");


data->knopka_gr[GR_FK_VNIZ]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data->knopka_gr[GR_FK_VNIZ]),label);

gtk_box_pack_start(GTK_BOX(vbox_kn1), data->knopka_gr[GR_FK_VNIZ], TRUE, TRUE, 0);
g_signal_connect(data->knopka_gr[GR_FK_VNIZ], "pressed",G_CALLBACK(l_restdok2_knopka_vniz_press),&data->spisok_gr);
g_signal_connect(data->knopka_gr[GR_FK_VNIZ], "released",G_CALLBACK(l_restdok2_knopka_released),&data->spisok_gr);
//gtk_widget_set_tooltip_text(data->knopka_gr[GR_FK_VNIZ],gettext("Выбор отмеченной записи"));
gtk_widget_set_name(data->knopka_gr[GR_FK_VNIZ],iceb_u_inttochar(GR_FK_VNIZ));
gtk_widget_show(data->knopka_gr[GR_FK_VNIZ]);
gtk_widget_show(label);



//вставляем кнопки для выбора материаллов/услуг


data->knopka_mu[MU_FK_VIBOR]=gtk_button_new_with_label(gettext("Выбор"));
gtk_box_pack_start(GTK_BOX(vbox_kn2), data->knopka_mu[MU_FK_VIBOR], TRUE, TRUE, 0);
g_signal_connect(data->knopka_mu[MU_FK_VIBOR], "clicked",G_CALLBACK(restdok2_mu_knopka),data);
gtk_widget_set_name(data->knopka_mu[MU_FK_VIBOR],iceb_u_inttochar(MU_FK_VIBOR));
gtk_widget_set_tooltip_text(data->knopka_mu[MU_FK_VIBOR],gettext("Ввод нового товара в счёт"));


label=gtk_label_new ("+");

data->knopka_mu[MU_FK_VVERH]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data->knopka_mu[MU_FK_VVERH]),label);

gtk_box_pack_start(GTK_BOX(vbox_kn2), data->knopka_mu[MU_FK_VVERH], TRUE, TRUE, 0);
g_signal_connect(data->knopka_mu[MU_FK_VVERH], "pressed",G_CALLBACK(l_restdok2_knopka_vverh_press),&data->spisok_mu);
g_signal_connect(data->knopka_mu[MU_FK_VVERH], "released",G_CALLBACK(l_restdok2_knopka_released),&data->spisok_mu);
gtk_widget_set_name(data->knopka_mu[MU_FK_VVERH],iceb_u_inttochar(MU_FK_VVERH));
gtk_widget_show(data->knopka_mu[MU_FK_VVERH]);
gtk_widget_show(label);
gtk_widget_set_tooltip_text(data->knopka_mu[MU_FK_VVERH],gettext("Вверх на одну строку"));

label=gtk_label_new ("-");


data->knopka_mu[MU_FK_VNIZ]=gtk_button_new();
gtk_container_add(GTK_CONTAINER(data->knopka_mu[MU_FK_VNIZ]),label);

gtk_box_pack_start(GTK_BOX(vbox_kn2), data->knopka_mu[MU_FK_VNIZ], TRUE, TRUE, 0);
g_signal_connect(data->knopka_mu[MU_FK_VNIZ], "pressed",G_CALLBACK(l_restdok2_knopka_vniz_press),&data->spisok_mu);
g_signal_connect(data->knopka_mu[MU_FK_VNIZ], "released",G_CALLBACK(l_restdok2_knopka_released),&data->spisok_mu);
gtk_widget_set_name(data->knopka_mu[MU_FK_VNIZ],iceb_u_inttochar(MU_FK_VNIZ));
gtk_widget_show(data->knopka_mu[MU_FK_VNIZ]);
gtk_widget_set_tooltip_text(data->knopka_mu[MU_FK_VNIZ],gettext("Вниз на одну строку"));

gtk_widget_show(label);

//if(data->metka_mat_usl == 0)
// {
  data->knopka_mu[MU_FK_OSTATOK]=gtk_button_new_with_label(gettext("Остаток"));
  gtk_box_pack_start(GTK_BOX(vbox_kn2), data->knopka_mu[MU_FK_OSTATOK], TRUE, TRUE, 0);
  g_signal_connect(data->knopka_mu[MU_FK_OSTATOK], "clicked",G_CALLBACK(restdok2_mu_knopka),data);
  gtk_widget_set_name(data->knopka_mu[MU_FK_OSTATOK],iceb_u_inttochar(MU_FK_OSTATOK));
  gtk_widget_set_tooltip_text(data->knopka_mu[MU_FK_OSTATOK],gettext("Включить/выключить показ записей с остатками"));
// }
 
}




