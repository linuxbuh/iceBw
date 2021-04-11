/*$Id: zarbolw_r.c,v 1.49 2014/08/31 06:19:20 sasa Exp $*/
/*02.09.2016	02.12.2014	Белых А.И.	zarbolw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"

class zarbol_rk
 {
  public:
  int tabn;
  class iceb_u_spisok mes_god;
  class iceb_u_double suma_bol;
  double itogo[5];

  zarbol_rk()
   {
    clear();
    tabn=0;
    memset(itogo,'\0',sizeof(itogo));
   }

  void clear()
   {
    suma_bol=0.;
    mes_god.free_class();
    suma_bol.free_class();
   }
 };

class zarbolw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class spis_oth *oth;  
  class iceb_u_str mes_god;
  class iceb_u_str kod_bol;
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zarbolw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zarbolw_r_key_press(GtkWidget *widget,GdkEventKey *event,class zarbolw_r_data *data);
gint zarbolw_r1(class zarbolw_r_data *data);
void  zarbolw_r_v_knopka(GtkWidget *widget,class zarbolw_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;

int zarbolw_r(const char *mes_god,const char *kod_bol,class spis_oth *oth,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zarbolw_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);

data.oth=oth;
data.mes_god.new_plus(mes_god);
data.kod_bol.new_plus(kod_bol);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка списка документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zarbolw_r_key_press),&data);

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

repl.plus(gettext("Распечатка списка документов"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zarbolw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);



g_idle_add((GSourceFunc)zarbolw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zarbolw_r_v_knopka(GtkWidget *widget,class zarbolw_r_data *data)
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

gboolean   zarbolw_r_key_press(GtkWidget *widget,GdkEventKey *event,class zarbolw_r_data *data)
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

/***********************************************/
/*Вывод строки*/
/*************************************************/
void zarbol_str(class zarbol_rk *rk,FILE *ff,class zarbolw_r_data *data)
{
char strsql[2048];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str fio("");
double suma_podoh=0.;
double suma_esv=0.;
double suma_vs=0.;
double suma_vid=0;
//class zarrnesvp_rek esvp;
short mes=0,god=0;
//float proc_esv=0;

sprintf(strsql,"select fio from Kartb where tabn=%d",rk->tabn);
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 fio.new_plus(row[0]);

for(int nom=0; nom < rk->mes_god.kolih(); nom++)
 {

  iceb_u_rsdat1(&mes,&god,rk->mes_god.ravno(nom));
/***********  
  zarrnesvpw(mes,god,&esvp,data->window);

  //Проверяем есть ли в списке инвалидов
  if(zarprtnw(mes,god,rk->tabn,2,data->window) == 1)
   proc_esv=esvp.proc_esv_bol_inv;
  else
   proc_esv=esvp.proc_esv_bol;
   
  //вычисляем единый соц взнос
  suma_esv=rk->suma_bol.ravno(nom) * proc_esv/100.;
  suma_esv=iceb_u_okrug(suma_esv,0.01);
******************/
  /*вычисляем подоходный*/
//  suma_podoh=(rk->suma_bol.ravno(nom)-suma_esv)*0.15;
  suma_podoh=rk->suma_bol.ravno(nom)*0.15;
  suma_podoh=iceb_u_okrug(suma_podoh,0.01);

  /*военный сбор*/
  suma_vs=rk->suma_bol.ravno(nom)*0.015;
  suma_vs=iceb_u_okrug(suma_vs,0.01);
  
  suma_vid=rk->suma_bol.ravno(nom)-suma_podoh-suma_esv-suma_vs;
  
  fprintf(ff,"%6d %-*.*s %s %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  rk->tabn,
  iceb_u_kolbait(30,fio.ravno()),
  iceb_u_kolbait(30,fio.ravno()),
  fio.ravno(),
  rk->mes_god.ravno(nom),
  rk->suma_bol.ravno(nom),
  suma_podoh,
  suma_esv,
  suma_vs,
  suma_vid);

  sprintf(strsql,"%6d %-*.*s %s %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  rk->tabn,
  iceb_u_kolbait(30,fio.ravno()),
  iceb_u_kolbait(30,fio.ravno()),
  fio.ravno(),
  rk->mes_god.ravno(nom),
  rk->suma_bol.ravno(nom),
  suma_podoh,
  suma_esv,
  suma_vs,
  suma_vid);
  
  iceb_printw(strsql,data->buffer,data->view);
  
  rk->itogo[0]+=rk->suma_bol.ravno(nom);
  rk->itogo[1]+=suma_podoh;
  rk->itogo[2]+=suma_esv;
  rk->itogo[3]+=suma_vs;
  rk->itogo[4]+=suma_vid;

 } 
rk->clear();
}
/**************************************/
/*Шапка*/
/***************************************/
void zarbol_hap(int nomstr,int *kolstr,FILE *ff,GtkWidget *wpredok)
{
if(kolstr != NULL)
 *kolstr+=4;
 
fprintf(ff,"%80s %s N%d\n","",gettext("Страница"),nomstr);
fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Таб н.|       Фамилия                |  Дата  |Больничный|Подоходный|Ед.соцвзн.|Военный з.|К выдаче  |\n"));
fprintf(ff,"\
------------------------------------------------------------------------------------------------------\n");
}


/******************************************/
/******************************************/

gint zarbolw_r1(class zarbolw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
float kolstr1=0.;
class zarbol_rk rk;
short m=0,g=0;

if(iceb_u_rsdat1(&m,&g,data->mes_god.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);

 }

/*читаем наименование кода*/
sprintf(strsql,"select naik from Nash where kod=%d\n",data->kod_bol.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  sprintf(strsql,"%s %d %s!",gettext("Не найден код начисления"),data->kod_bol.ravno_atoi(),gettext("в справочнике начислений"));
  iceb_menu_soob(strsql,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 } 

class iceb_u_str naim(row[0]);

sprintf(strsql,"select tabn,knah,suma,godn,mesn from Zarp where datz >= '%04d-%02d-%02d' and datz <= '%04d-%02d-%02d' and knah=%d and suma <> 0. and prn='1' \
order by tabn asc",g,m,1,g,m,31,data->kod_bol.ravno_atoi());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf[64];
sprintf(imaf,"bol11.lst");
class iceb_fopen fil;

if(fil.start(imaf,"w",data->window) != 0)
 {

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  return(1);
 }
iceb_zagolov(gettext("Расчёт удержаний с больничного"),fil.ff,data->window);
fprintf(fil.ff,"%s:%s\n",gettext("Дата"),data->mes_god.ravno());
fprintf(fil.ff,"%s:%d %s\n",gettext("Начисление"),data->kod_bol.ravno_atoi(),naim.ravno());

zarbol_hap(1,NULL,fil.ff,data->window);
double suma=0.;
int tabnz=0;
int nom=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  rk.tabn=atoi(row[0]);
  suma=atof(row[2]);
  if(tabnz != rk.tabn)
   {
    if(tabnz != 0)
      zarbol_str(&rk,fil.ff,data);
    tabnz=rk.tabn;
   }

  sprintf(strsql,"%02d.%04d",atoi(row[4]),atoi(row[3]));
  if((nom=rk.mes_god.find(strsql)) < 0)
   rk.mes_god.plus(strsql);

  rk.suma_bol.plus(suma,nom);

 }

fprintf(fil.ff,"ICEB_LST_END\n\
------------------------------------------------------------------------------------------------------\n");

fprintf(fil.ff,"%*s %10.2f %10.2f %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(45,gettext("Итого")),gettext("Итого"),
rk.itogo[0],rk.itogo[1],rk.itogo[2],rk.itogo[3],rk.itogo[4]);

iceb_podpis(fil.ff,data->window);

fil.end();

data->oth->spis_imaf.plus(imaf);
data->oth->spis_naim.plus(gettext("Расчёт удержаний с больничного"));

int orient=iceb_ustpeh(data->oth->spis_imaf.ravno(0),3,data->window);

iceb_rnl(data->oth->spis_imaf.ravno(0),orient,NULL,&zarbol_hap,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
