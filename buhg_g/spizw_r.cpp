/*$Id:$*/
/*22.03.2017	17.05.2005	Белых А.И. 	spizw_r.c
Расчёт списания материалов изделия с карточек материального учёта
*/
#include <math.h>
#include "buhg_g.h"
#include "spizw.h"

class spizw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class spizw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  spizw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   spizw_r_key_press(GtkWidget *widget,GdkEventKey *event,class spizw_r_data *data);
gint spizw_r1(class spizw_r_data *data);
void  spizw_r_v_knopka(GtkWidget *widget,class spizw_r_data *data);


extern SQL_baza bd;

int spizw_r(class spizw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class spizw_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Списание материалов изделия с карточек"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(spizw_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

repl.plus(gettext("Списание материалов изделия с карточек"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,wpredok));
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(spizw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)spizw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  spizw_r_v_knopka(GtkWidget *widget,class spizw_r_data *data)
{
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   spizw_r_key_press(GtkWidget *widget,GdkEventKey *event,class spizw_r_data *data)
{
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}

/******************************************/
/******************************************/

gint spizw_r1(class spizw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
short ds,ms,gs;

float pnds=iceb_pnds(data->window);

if(iceb_u_rsdat(&ds,&ms,&gs,data->rk->datas.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

//Проверяем код склада

sprintf(strsql,"select naik from Sklad where kod=%s",data->rk->sklad.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код склада"),data->rk->sklad.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

//Проверяем есть ли этот код операции в расходах
sprintf(strsql,"select kod from Rashod where kod='%s'",data->rk->kodop.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код операции"),data->rk->kodop.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(data->rk->nomdok.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён номер документа !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(data->rk->koliz.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Не введёно количество !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


//Проверяем номер документа
//Уникальный номер документа для прихода и расхода
sprintf(strsql,"select nomd from Dokummat where \
datd >= '%d-01-01' and datd <= '%d-12-31' and sklad=%s and nomd='%s'",
gs,gs,data->rk->sklad.ravno(),data->rk->nomdok.ravno());
if(iceb_sql_readkey(strsql,data->window) >= 1)
 {
  sprintf(strsql,gettext("С номером %s документ уже есть !"),data->rk->nomdok.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



SQL_str row;

if(zap_s_mudokw(2,ds,ms,gs,data->rk->sklad.ravno_atoi(),
"00",data->rk->nomdok.ravno(),
"",data->rk->kodop.ravno(),0,0,"",0,0,0,1,"00",pnds,data->window) != 0)
 {
  iceb_msql_error(&bd,"",strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
 



double  kolndet=0.;
int     kodm;
double  kolih=0;

class iceb_razuz_data m_det;
m_det.kod_izd=data->rk->kod_izdel.ravno_atoi();
if((kolndet=iceb_razuz_kod(&m_det,data->window)) == 0)
 {
  sprintf(strsql,"%s %s !",gettext("Не найдено ни одной детали для изделия"),
  data->rk->kod_izdel.ravno());
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int kolkar=0;
double cena=0.;
class iceb_u_str ei("");
double ndss=pnds;
SQLCURSOR cur;
kolndet=m_det.kod_det_ei.kolih();

for(int sss=0; sss < kolndet; sss++)
 {
  if(m_det.metka_mu.ravno(sss) == 1)
   continue;

   
  iceb_u_polen(m_det.kod_det_ei.ravno(sss),&kodm,1,'|');
  iceb_u_polen(m_det.kod_det_ei.ravno(sss),&ei,2,'|');
  kolih=m_det.kolih_det_ei.ravno(sss)*data->rk->koliz.ravno_atof();;

  cena=0.;
  //Читаем цену
  sprintf(strsql,"select cenapr from Material where kodm=%d",kodm);
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    cena=atof(row[0]);



  iceb_u_int nomkars;
  iceb_u_double kolpriv;
    
  if((kolkar=findkarw(ds,ms,gs,data->rk->sklad.ravno_atoi(),kodm,kolih,&nomkars,&kolpriv,0,cena,"",data->window)) <= 0)
    zapvdokw(ds,ms,gs,data->rk->sklad.ravno_atoi(),0,kodm,data->rk->nomdok.ravno(),kolih,cena,ei.ravno(),ndss,0,0,2,0,"","",0,"",data->window);
  else
   {
    double kolihpi=0.;
    for(int sht=0; sht < kolkar; sht++)
     {
      int nomkar=nomkars.ravno(sht);
      double kolihp=kolpriv.ravno(sht);
      kolihpi+=kolihp;
      zapvdokw(ds,ms,gs,data->rk->sklad.ravno_atoi(),nomkar,kodm,data->rk->nomdok.ravno(),kolihp,cena,ei.ravno(),ndss,0,0,2,0,"","",0,"",data->window);
     }
    if(fabs(kolih-kolihpi) > 0.000001)
      zapvdokw(ds,ms,gs,data->rk->sklad.ravno_atoi(),0,kodm,data->rk->nomdok.ravno(),kolih-kolihpi,cena,ei.ravno(),ndss,0,0,2,0,"","",0,"",data->window);
   }


 }

podtdok1w(ds,ms,gs,data->rk->sklad.ravno_atoi(),data->rk->nomdok.ravno(),2,ds,ms,gs,1,data->window);
podvdokw(ds,ms,gs,data->rk->nomdok.ravno(),data->rk->sklad.ravno_atoi(),data->window);






gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
