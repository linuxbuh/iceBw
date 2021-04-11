/*$Id: uosrbsw_r.c,v 1.15 2013/09/26 09:46:57 sasa Exp $*/
/*16.04.2017	15.01.2008	Белых А.И.	uosrbsw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "uosrbsw.h"

class uosrbsw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class uosrbsw_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  uosrbsw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosrbsw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosrbsw_r_data *data);
gint uosrbsw_r1(class uosrbsw_r_data *data);
void  uosrbsw_r_v_knopka(GtkWidget *widget,class uosrbsw_r_data *data);


extern SQL_baza bd;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;
extern char	*spgnunpa; //Список групп налогового учёта с не пообъектной амортизацией
extern char	*spgnu; //Список групп налогового учёта

int uosrbsw_r(class uosrbsw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class uosrbsw_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт балансовой стоимости по мат.ответственным и счетам учёта"));

//sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка ведомости аморт-отчислений для налогового учёта"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(uosrbsw_r_key_press),&data);

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

repl.plus(gettext("Расчёт балансовой стоимости по мат.ответственным и счетам учёта"));

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(uosrbsw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)uosrbsw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosrbsw_r_v_knopka(GtkWidget *widget,class uosrbsw_r_data *data)
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

gboolean   uosrbsw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosrbsw_r_data *data)
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
/*****************************/
/*Строка подчёркивания*/
/***************************/

void uosrbs_sp(int kol,FILE *ff)
{

fprintf(ff,"---------------------------");
//          123456789012345678901234567
for(int ii=0; ii < kol+1; ii++)
 fprintf(ff,"---------------------");
//           123456789012345678901
fprintf(ff,"\n");
}
/*******************************/
/*Распечатка */
/****************************/

void uosrbs_ras(class iceb_u_int *sp_kod_ol,class iceb_u_spisok *sp_shet,
class iceb_u_double *masiv_sum,
class iceb_u_int *m_kolih,
FILE *ff,
GtkWidget *wpredok)
{
char strsql[1024];
class iceb_u_str naim("");
SQL_str row;
SQLCURSOR cur;


int kolih_shet=sp_shet->kolih();
int kolih_ol=sp_kod_ol->kolih();

uosrbs_sp(kolih_shet,ff); //Строка подчёркивания


fprintf(ff,"%-*.*s|%-*.*s|",
iceb_u_kolbait(5,gettext("Код")),iceb_u_kolbait(5,gettext("Код")),gettext("Код"),
iceb_u_kolbait(20,gettext("Мат.ответственный")),iceb_u_kolbait(20,gettext("Мат.ответственный")),gettext("Мат.ответственный"));

//Пишем первую строку со счетами
for(int ii=0; ii < kolih_shet; ii++)
 {
  //узнаём наименование 

  sprintf(strsql,"select nais from Plansh where ns='%s'",sp_shet->ravno(ii));  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");

  sprintf(strsql,"%s %s",sp_shet->ravno(ii),naim.ravno());

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,strsql),iceb_u_kolbait(20,strsql),strsql);
 }
fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,gettext("Итого")),iceb_u_kolbait(20,gettext("Итого")),gettext("Итого"));
fprintf(ff,"\n");

fprintf(ff,"     |                    |");

//строка разделения

for(int ii=0; ii < kolih_shet+1; ii++)
 {
  fprintf(ff,"---------------------");
 }
fprintf(ff,"\n");


fprintf(ff,"     |                    |");


for(int ii=0; ii < kolih_shet+1; ii++)
 {
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_u_kolbait(9,gettext("Количество")),
  iceb_u_kolbait(9,gettext("Количество")),
  gettext("Количество"),
  iceb_u_kolbait(10,gettext("Сумма")),
  iceb_u_kolbait(10,gettext("Сумма")),
  gettext("Сумма"));
 }
fprintf(ff,"\n");

uosrbs_sp(kolih_shet,ff); //Строка подчёркивания

//Распечатываем массив
for(int s_kol=0; s_kol < kolih_ol; s_kol++)
 {
  //узнаём наименование 

  sprintf(strsql,"select naik from Uosol where kod=%d",sp_kod_ol->ravno(s_kol));  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");
  
  fprintf(ff,"%-5d|%-*.*s|",
  sp_kod_ol->ravno(s_kol),
  iceb_u_kolbait(20,naim.ravno()),iceb_u_kolbait(20,naim.ravno()),naim.ravno());
  
  int itog_str_k=0;
  double itog_str_s=0.;
  for(int s_shet=0; s_shet < kolih_shet; s_shet++)
   {
    int kolih=m_kolih->ravno(s_kol*kolih_shet+s_shet);
    double suma=masiv_sum->ravno(s_kol*kolih_shet+s_shet);

    if(kolih == 0.)
      fprintf(ff,"%9s|","");
    else
      fprintf(ff,"%9d|",kolih);

    if(suma == 0.)
      fprintf(ff,"%10s|","");
    else
      fprintf(ff,"%10.2f|",suma);
    itog_str_k+=kolih;
    itog_str_s+=suma;
   }
  fprintf(ff,"%9d|%10.2f|",itog_str_k,itog_str_s);
  fprintf(ff,"\n");    
   
 }

uosrbs_sp(kolih_shet,ff); //Строка подчёркивания

fprintf(ff,"%*s|",iceb_u_kolbait(26,gettext("Итого")),gettext("Итого"));

//Распечатка итоговой строки
int itogo_po_kol_kolih=0;
double itogo_po_kol_suma=0.;
double itogo_k=0.;
double itogo_s=0.;

for(int ii=0; ii < kolih_shet; ii++)
 {
  itogo_po_kol_kolih=0;
  itogo_po_kol_suma=0.;
  for(int skontr=0; skontr < kolih_ol; skontr++)
   {
    itogo_po_kol_kolih+=m_kolih->ravno(skontr*kolih_shet+ii);
    itogo_po_kol_suma+=masiv_sum->ravno(skontr*kolih_shet+ii);
   }
  fprintf(ff,"%9d|%10.2f|",itogo_po_kol_kolih,itogo_po_kol_suma);
  itogo_k+=itogo_po_kol_kolih;
  itogo_s+=itogo_po_kol_suma;  
 }
fprintf(ff,"%9.9g|%10.2f|",itogo_k,itogo_s);
fprintf(ff,"\n");

}



/******************************************/
/******************************************/

gint uosrbsw_r1(class uosrbsw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;







short dk,mk,gk;

iceb_u_rsdat(&dk,&mk,&gk,data->rk->datak.ravno(),1);

int kolstr=0;

//создаём список кодов материально ответственных
sprintf(strsql,"select kod from Uosol");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
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
class iceb_u_int sp_kod_ol;

while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(data->rk->mat_ot.ravno(),row[0],0,0) != 0)
   continue;
  sp_kod_ol.plus(atoi(row[0]));
 }
//создаём список счетов учёта
sprintf(strsql,"select distinct shetu from Uosinp");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
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

class iceb_u_spisok sp_shet;
while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(data->rk->shetu.ravno(),row[0],0,0) != 0)
   continue;
  sp_shet.plus(row[0]);
 }
/********
printw("%s:%d\n",gettext("Количество счетов"),sp_shet.kolih());
printw("%s:%d\n",gettext("Количество материально-ответственных"),sp_kod_ol.kolih());
***********/

//масив сумм
class iceb_u_double mas_sum;
//массив количества
class iceb_u_int mas_kolih;
mas_sum.make_class(sp_shet.kolih()*sp_kod_ol.kolih());
mas_kolih.make_class(sp_shet.kolih()*sp_kod_ol.kolih());

sprintf(strsql,"select innom from Uosin");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
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
int podr;
int ikod_ol;
long inn;
class bsiz_data data_per;
int nomer_kod_ol=0;
int nomer_shet=0;

int kol_shet=sp_shet.kolih();
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  inn=atol(row[0]);
  if(poiinw(inn,dk,mk,gk,&podr,&ikod_ol,data->window) != 0)
   continue;  

  sprintf(strsql,"%d",ikod_ol);
  if(iceb_u_proverka(data->rk->mat_ot.ravno(),strsql,0,0) != 0)
    continue;

  class poiinpdw_data rekin;
  if(poiinpdw(inn,mk,gk,&rekin,data->window) != 0)
   continue;

  if(iceb_u_proverka(data->rk->shetu.ravno(),rekin.shetu.ravno(),0,0) != 0)
   continue;

  
  bsizpw(inn,podr,0,1,1,gk,dk,mk,gk,&data_per,NULL,data->window);
  
  if((nomer_kod_ol=sp_kod_ol.find(ikod_ol)) < 0)
   {
    sprintf(strsql,"Не найден код ответственного лица %d в массиве !",ikod_ol);
    iceb_menu_soob(strsql,data->window);    
    continue;
   }
  
  if((nomer_shet=sp_shet.find(rekin.shetu.ravno())) < 0)
   {
    sprintf(strsql,"Не найден счёт %s в массиве !",rekin.shetu.ravno());
    iceb_menu_soob(strsql,data->window);
    continue;
   }
  mas_sum.plus(data_per.bu.bs_kon_per,nomer_kod_ol*kol_shet+nomer_shet);
  mas_kolih.plus(1,nomer_kod_ol*kol_shet+nomer_shet);
  
 }

FILE *ff;
char imaf[56];
sprintf(imaf,"rbs%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_zagolov(gettext("Расчёт балансовой стоимости по мат.ответственным и счетам учёта"),0,0,0,dk,mk,gk,ff,data->window);
if(data->rk->mat_ot.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Материально-ответственное лицо (лица)"),data->rk->mat_ot.ravno());
if(data->rk->shetu.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),data->rk->shetu.ravno());
 


uosrbs_ras(&sp_kod_ol,&sp_shet,&mas_sum,&mas_kolih,ff,data->window);

iceb_podpis(ff,data->window);
fclose(ff);


data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Расчёт балансовой стоимости по мат.ответственным и счетам учёта"));
iceb_ustpeh(imaf,0,data->window);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
