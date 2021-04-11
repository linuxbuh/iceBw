/*$Id: sprtabotpw_r.c,v 1.21 2013/09/26 09:46:55 sasa Exp $*/
/*13.01.2020	26.05.2016	Белых А.И.	sprtabotpw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "sprtabotpw.h"
#include "zarrsovw.h"
class sprtabotpw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class sprtabotp_poi *rp;
  class spis_oth *oth;  
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  sprtabotpw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };

class sprtabotp_dat
 {
  public:
   class iceb_u_double sumanmes;
   class iceb_u_spisok DATAI; //Список дат м.год индексации
   class iceb_u_double KOF; //Коффициенты индексации по датам
   class iceb_u_double sumanvr; //Суммы не входящие в расчёт
   class iceb_u_double sumanvrxr; //Суммы не входящие в расчёт хозрасчёт
   class iceb_u_double sumanmesxr; //Сумма начисленная хозрасчёт
   class iceb_u_double sumanvi; //Сумма не входящая в расчёт индексации отпускных
   class iceb_u_double sumanvixr; //Сумма не входящая в расчёт индексации отпускных хозрасчёта
   class iceb_u_double sumanmesbu;
   class iceb_u_double sumanvrbu; //Суммы не входящие в расчёт бюджет
   class iceb_u_double sumanvibu; //Сумма не входящая в расчёт индексации отпускных бюджета
   class iceb_u_int  kolkd; //Количество календарных дней в месяце
   class iceb_u_int  kol_pd; //Количество праздничных дней в месяце
   class iceb_u_int  kolkd_vdr; //Количество календарных дней взятых для расчёта
   class iceb_u_int skodnvr; /*список кодов начислений вошедших в расчёт*/
   class iceb_u_double sum_vr;

   class iceb_u_str fio;
   class iceb_u_str dolg;
   class iceb_u_str kodpodr;     
   class iceb_u_str naipod;
 };


gboolean   sprtabotpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class sprtabotpw_r_data *data);
gint sprtabotpw_r1(class sprtabotpw_r_data *data);
void  sprtabotpw_r_v_knopka(GtkWidget *widget,class sprtabotpw_r_data *data);


extern SQL_baza bd;
extern char	*shetb; /*Бюджетные счета начислений*/

int sprtabotpw_r(class sprtabotp_poi *rek,class spis_oth *oth,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class sprtabotpw_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);
data.oth=oth;
data.rp=rek;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт отпускных"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(sprtabotpw_r_key_press),&data);

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

repl.plus(gettext("Расчёт отпускных"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(sprtabotpw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)sprtabotpw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  sprtabotpw_r_v_knopka(GtkWidget *widget,class sprtabotpw_r_data *data)
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

gboolean   sprtabotpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class sprtabotpw_r_data *data)
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
/**********************************/
/*Вычисление итога по вертикали*/
/********************************/

double 	itogvert(int kolkodnvr,int kolmes,int nomkod,double *sumanvr)
{

double itog=0.;
for(int nommes=0; nommes < kolmes; nommes++)
  itog+=sumanvr[nommes*kolkodnvr+nomkod];

return(itog);

}

/*************************************/
/*поиск в месяце предыдущем началу расчёта отпускных в счёт месяца начала расчёта*/
/*********************************/

double sprtabotp_po(int tabn,class sprtabotp_poi *rp,const char *kod_otp,int metka_mes,
int metka_b, //0-общий 1-бюджет 2-хозрасчёт
FILE *ff_prot,
GtkWidget *wpredok)
{
double suma_otp=0.;
char strsql[1024];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
short dp=1,mp=rp->mn,gp=rp->gn;
short den=1,mes=rp->mn,god=rp->gn;

iceb_u_dpm(&dp,&mp,&gp,4);
if(metka_mes == 1)
 iceb_u_dpm(&den,&mes,&god,3);

sprintf(strsql,"select knah,suma,shet from Zarp where tabn=%d \
and datz >= '%04d-%d-%d' and datz <='%04d-%d-%d' and prn='1' and suma <> 0.\
 and godn=%d and mesn=%d",
tabn,gp,mp,1,gp,mp,31,god,mes);

//printw("%s-%s\n",__FUNCTION__,strsql);
//OSTANOV();

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  return(0.);
 }

while(cur.read_cursor(&row) != 0)
 {
  if(metka_b == 1)
   if(iceb_u_proverka(shetb,row[2],0,1) != 0)
    continue;
  if(metka_b == 2)
   if(iceb_u_proverka(shetb,row[2],0,1) == 0)
    continue;
          
  if(iceb_u_proverka(row[0],kod_otp,0,1) == 0)
   {
    if(ff_prot != NULL)
      fprintf(ff_prot,"%s-Сумма отпускных предыдущего месяца=%s\n",__FUNCTION__,row[1]);

    suma_otp+=atof(row[1]);
   }

 }
 
return(suma_otp);

}

/*************************/
/*определение коэффициента*/
/**************************/
double sprtabotp_koef(int tabnom,short mr,short gr,short mk,short gk,FILE *ff,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
double oklad1=0.;
double oklad2=0.;
if(ff != NULL)
 {
  fprintf(ff,"\n%s-Расчёт коэффициента %d %02d.%04d\n",__FUNCTION__,tabnom,mr,gr);
  fprintf(ff,"%s-Ищем оклады меньше этой даты или равной\n",__FUNCTION__);
 }
 
sprintf(strsql,"select dt,sm,mt from Zarsdo where tn=%d and dt <= '%04d-%02d-01' and km <> '-' order by dt desc limit 1",tabnom,gr,mr);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
 {
  if(ff != NULL)
   fprintf(ff,"%s-Не нашли коэффициент 1\n",__FUNCTION__);
  return(1.);
 }

if(ff != NULL)
 fprintf(ff,"%s-%s %10s %s\n",__FUNCTION__,iceb_u_datzap_mg(row[0]),row[1],row[2]);

oklad1=atof(row[1]);
int metka=atoi(row[2]);
if(metka == 5 || metka == 6)
  oklad1=okladtrw(mr,gr,oklad1,wpredok);

if(ff != NULL)
 fprintf(ff,"%s-Ищем последее повышение зарплаты\n",__FUNCTION__);

//sprintf(strsql,"select dt,sm,mt from Zarsdo where tn=%d and dt > '%04d-%02d-01' and km <> '-' order by dt desc limit 1",tabnom,gr,mr);
sprintf(strsql,"select dt,sm,mt from Zarsdo where tn=%d and dt <= '%04d-%02d-01' and km <> '-' order by dt desc limit 1",tabnom,gk,mk);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
 {
  if(ff != NULL)
   fprintf(ff,"%s-Не нашли коэффициент 1\n",__FUNCTION__);
  return(1.);
 }
if(ff != NULL)
 fprintf(ff,"%s-%s %10s %s\n",__FUNCTION__,iceb_u_datzap_mg(row[0]),row[1],row[2]);

oklad2=atof(row[1]);
metka=atoi(row[2]);
if(metka == 5 || metka == 6)
  oklad2=okladtrw(mr,gr,oklad2,wpredok);


double koef=oklad2/oklad1;
koef=iceb_u_okrug(koef,0.000001);
if(ff != NULL)
 fprintf(ff,"%s-Расчитываем коэффициент %.2f/%.2f=%f\n",__FUNCTION__,oklad2,oklad1,koef);

return(koef);

}



/********************************/
/* Распечатка результата        */
/*возвращает сумму последней колонки в таблице*/
/********************************/
double rasrez(int tabn,
class sprtabotp_poi *rp,
class sprtabotp_dat *dt,
class iceb_u_double *sumanmes,
class iceb_u_double *sumanvr,
int metka, //0-общий 1-бюджет 2-хозрасчёт
class iceb_u_double *sumanvi, //Массив сумм не входящих в расчёт индексации
FILE *ff,
FILE *ff_prot,
GtkWidget *wpredok)
{
int nomkod=0;
int nommes=0;
char strsql[2048];
short d,m,g;
SQL_str row;
int kolkdi=0;
char bros[1024];
double kof=0.;
double  sumaindekc=0.;
SQLCURSOR curr;
//Вычисляем количество колонок
int kolkol=0;
int kolkodnvr=iceb_u_pole2(rp->kodnvr.ravno(),',');

for(nomkod=0; nomkod < kolkodnvr ; nomkod++)
 if(sumanvr->suma_kol(rp->kolmes,kolkodnvr,nomkod) != 0.)
//  if(itogvert(kolkodnvr,rp->kolmes,nomkod,sumanvr) != 0.)
   kolkol++;
int kolkodnvrind=iceb_u_pole2(rp->kodnvrind.ravno(),',');
for(nomkod=0; nomkod < kolkodnvrind ; nomkod++)
 if(sumanvi->suma_kol(rp->kolmes,kolkodnvrind,nomkod) != 0.)
//  if(itogvert(kolkodnvrind,rp->kolmes,nomkod,sumanvi) != 0.)
   kolkol++;

char stroka[51+(kolkol+1)*11+1];
memset(stroka,'\0',sizeof(stroka));
memset(stroka,'-',sizeof(stroka)-1);

class iceb_u_str kod_otp("");
iceb_poldan("Коды начислений отпускных",&kod_otp,"zarotp.alx",wpredok);

time_t  tmm;
time(&tmm);
iceb_zagolov(gettext("Расчётный лист на оплату отпускных"),1,rp->mn,rp->gn,rp->dk,rp->mk,rp->gk,ff,wpredok);

fprintf(ff,"\n%s:%d\n",gettext("Табельный номер"),tabn);
fprintf(ff,"%s:%s\n",gettext("Фамилия Имя Отчество"),dt->fio.ravno());
fprintf(ff,"%s:%s\n",gettext("Должность"),dt->dolg.ravno());
fprintf(ff,"%s:%s %s\n",gettext("Подразделение"),dt->kodpodr.ravno(),dt->naipod.ravno());
fprintf(ff,"%s:%s\n",gettext("Вид отпуска"),rp->vid_otp.ravno());
fprintf(ff,"%s:%s\n",gettext("Приказ"),rp->prikaz.ravno());
fprintf(ff,"%s:%s\n",gettext("Период з"),rp->period.ravno());
fprintf(ff,"%s:%s\n",gettext("Количество дней отпуска"),rp->kol_dnei.ravno());
fprintf(ff,"\n");

if(rp->kodt_nvr.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Коды табеля не входящие в расчёт"),rp->kodt_nvr.ravno());
 
if(metka == 1)
  fprintf(ff,"%s.\n",gettext("Бюджет"));

if(metka == 2)
  fprintf(ff,"%s.\n",gettext("Хозрасчёт"));

int metka_pk=iceb_poldan_vkl("Печать наименований кодов не входящих в расчёт","zarotp.alx",wpredok);

if(metka_pk == 1)
 {
  fprintf(ff,"%s:%.*s\n",
  gettext("Коды, которые не вошли в расчёт"),
  iceb_u_kolbait(60,rp->kodnvr.ravno()),
  rp->kodnvr.ravno());

  for(int nom=60; nom < iceb_u_strlen(rp->kodnvr.ravno()); nom+=90)
   fprintf(ff,"%.*s\n",
   iceb_u_kolbait(90,iceb_u_adrsimv(nom,rp->kodnvr.ravno())),
   iceb_u_adrsimv(nom,rp->kodnvr.ravno()));
      
 }
else
 {
  if(kolkodnvr != 0)
   {
    class iceb_u_str kod("");
    class iceb_u_str naikod("");
    fprintf(ff,"%s:\n",gettext("Коды, которые не вошли в расчёт"));
    short shethik=0;
    char str_prom[1024];
    char str2[1024];
    
    for(nomkod=0; nomkod < kolkodnvr ; nomkod++)
     {
      if(iceb_u_polen(rp->kodnvr.ravno(),&kod,nomkod+1,',') != 0)
        kod.new_plus(rp->kodnvr.ravno());
      sprintf(strsql,"select naik from Nash where kod=%s",kod.ravno());
      if(sql_readkey(&bd,strsql,&row,&curr) == 1)
       naikod.new_plus(row[0]);    
      else
       naikod.new_plus("");  
      if(shethik == 0)
       {
        memset(str_prom,'\0',sizeof(str_prom));
        sprintf(str_prom,"%2s %-*.*s",
        kod.ravno(),
        iceb_u_kolbait(45,naikod.ravno()),iceb_u_kolbait(45,naikod.ravno()),naikod.ravno());
       }

      if(shethik == 1)
       {
        sprintf(str2,"  %2s %-*.*s",
        kod.ravno(),
        iceb_u_kolbait(45,naikod.ravno()),iceb_u_kolbait(45,naikod.ravno()),naikod.ravno());
        strcat(str_prom,str2);
       }


      
      shethik++;
      if(shethik == 2)
       {
        fprintf(ff,"%s\n",str_prom);
        shethik=0;
       }
    
    
     }
    if(shethik != 0)
        fprintf(ff,"%s\n",str_prom);

   }
 }
 
if(kolkodnvrind != 0)
 {
  class iceb_u_str kod("");
  class iceb_u_str naikod("");
  fprintf(ff,"%s:\n",gettext("Коды, которые не вошли в расчёт индексации"));
  for(nomkod=0; nomkod < kolkodnvrind ; nomkod++)
   {
    if(iceb_u_polen(rp->kodnvrind.ravno(),&kod,nomkod+1,',') != 0)
      kod.new_plus(rp->kodnvrind.ravno());

    sprintf(strsql,"select naik from Nash where kod=%s",kod.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
     naikod.new_plus(row[0]);    
    else
     naikod.new_plus("");
    fprintf(ff,"%s %s\n",kod.ravno(),naikod.ravno());
   }

 }

fprintf(ff,"\n");

fprintf(ff,"%s\n",stroka);

fprintf(ff,gettext(" Дата  |К.д|П.д|В.р|Начислено |"));
//Печатаем коды не входящие в расчёт
for(nomkod=0; nomkod < kolkodnvr ; nomkod++)
 {
  if(sumanvr->suma_kol(rp->kolmes,kolkodnvr,nomkod) == 0.)
//  if(itogvert(kolkodnvr,rp->kolmes,nomkod,sumanvr) == 0.)
    continue;
    
  if(iceb_u_polen(rp->kodnvr.ravno(),bros,sizeof(bros),nomkod+1,',') != 0)
    strcpy(bros,rp->kodnvr.ravno());

  sprintf(strsql,"select naik from Nash where kod=%d",atoi(bros));
  if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
   sprintf(strsql,"%s %s",bros,row[0]);
  else
   strcpy(strsql,bros);

  fprintf(ff,"%*.*s|",
  iceb_u_kolbait(10,strsql),
  iceb_u_kolbait(10,strsql),
  strsql);
 }

fprintf(ff,"%*.*s|",
iceb_u_kolbait(10,gettext("В расчёт")),iceb_u_kolbait(10,gettext("В расчёт")),gettext("В расчёт"));

//Печатаем коды не входящие в расчёт индексации
for(nomkod=0; nomkod < kolkodnvrind ; nomkod++)
 {
//  if(itogvert(kolkodnvrind,rp->kolmes,nomkod,sumanvi) == 0.)
  if(sumanvi->suma_kol(rp->kolmes,kolkodnvrind,nomkod) == 0.)
    continue;
    
  if(iceb_u_polen(rp->kodnvrind.ravno(),bros,sizeof(bros),nomkod+1,',') != 0)
    strcpy(bros,rp->kodnvrind.ravno());

  sprintf(strsql,"select naik from Nash where kod=%d",atoi(bros));
  if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
   sprintf(strsql,"%s %s",bros,row[0]);
  else
   strcpy(strsql,bros);

  fprintf(ff,"%*.*s|",
  iceb_u_kolbait(10,strsql),
  iceb_u_kolbait(10,strsql),
  strsql);


 }


fprintf(ff,"%-*.*s|%-*.*s|\n",
iceb_u_kolbait(8,gettext("Коэф.")),iceb_u_kolbait(8,gettext("Коэф.")),gettext("Коэф."),
iceb_u_kolbait(10,gettext("Итого")),iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",stroka);

d=1; m=rp->mn; g=rp->gn;
double itog=0.;
double itog1=0.;
double itogind=0.;
int kold_vdr=0;
int itogo_dni[3];
double v_rashet=0.;
double itogo_v_rashet=0.;
memset(itogo_dni,'\0',sizeof(itogo_dni));

for(nommes=0; nommes < rp->kolmes; nommes++)
 {

  fprintf(ff,"%02d.%04d|%3d|%3d|%3d|%10.2f|",m,g,dt->kolkd.ravno(nommes),dt->kol_pd.ravno(nommes),dt->kolkd_vdr.ravno(nommes)-dt->kol_pd.ravno(nommes),sumanmes->ravno(nommes));
  kolkdi+=dt->kolkd.ravno(nommes);
  kold_vdr+=dt->kolkd_vdr.ravno(nommes);
  itog1+=sumanmes->ravno(nommes);
  itog=0.;
  itogo_dni[0]+=dt->kolkd.ravno(nommes);
  itogo_dni[1]+=dt->kol_pd.ravno(nommes);
  itogo_dni[2]+=dt->kolkd_vdr.ravno(nommes)-dt->kol_pd.ravno(nommes);


  for(nomkod=0; nomkod < kolkodnvr ; nomkod++)
   {
    //if(itogvert(kolkodnvr,rp->kolmes,nomkod,sumanvr) == 0.)
    if(sumanvr->suma_kol(rp->kolmes,kolkodnvr,nomkod) == 0.)
      continue;
    double sss=sumanvr->ravno(nommes*kolkodnvr+nomkod);
    if(sss != 0.)
      fprintf(ff,"%10.2f|",sss);
    else
      fprintf(ff,"%10s|","");
    
    itog+=sss;
   }

  double otp=0.;

  if(nommes == 0 || nommes == 1)/*прибавляем сумму отпускных в счёт этого месяца*/
   {

    otp=sprtabotp_po(tabn,rp,kod_otp.ravno(),nommes,metka,ff_prot,wpredok);
    
   }

  /*печатаем колонку В расчёт*/
  v_rashet=sumanmes->ravno(nommes)-itog+otp;
  itogo_v_rashet+=v_rashet;
  fprintf(ff,"%10.2f|",v_rashet);

  itogind=0.;
  for(nomkod=0; nomkod < kolkodnvrind ; nomkod++)
   {
//    if(itogvert(kolkodnvrind,rp->kolmes,nomkod,sumanvi) == 0.)
    if(sumanvi->suma_kol(rp->kolmes,kolkodnvrind,nomkod) == 0.)
      continue;

    fprintf(ff,"%10.2f|",sumanvi->ravno(nommes*kolkodnvrind+nomkod));

    itogind+=sumanvi->ravno(nommes*kolkodnvrind+nomkod);
   }

  if(rp->metka_ri == 1)
   {
    int nomer=0;
    sprintf(bros,"%d.%d",m,g);
    if((nomer=dt->DATAI.find(bros)) >= 0)
     {
      kof=dt->KOF.ravno(nomer);
      fprintf(ff,"%8.8g|%10.2f|",kof,kof*(v_rashet-itogind));
     }  
    else
     {
      kof=1.;
      fprintf(ff,"%8s|%10.2f|","",kof*(v_rashet-itogind));
     }
   }
  else
   {
    kof=sprtabotp_koef(tabn,m,g,rp->mk,rp->gk,ff_prot,wpredok);
    fprintf(ff,"%8.8g|%10.2f|",kof,kof*(v_rashet-itogind));
   }

  sumaindekc+=kof*(v_rashet-itogind);
  if(otp != 0.)
   fprintf(ff," %s %.2f",gettext("отпускные"),otp);
  fprintf(ff,"\n");
  
  iceb_u_dpm(&d,&m,&g,3);
 }
fprintf(ff,"%s\n",stroka);

//Печатаем итоговую строку
fprintf(ff,"%*s:%3d %3d %3d %10.2f",iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"),itogo_dni[0],itogo_dni[1],itogo_dni[2],itog1);
for(nomkod=0; nomkod < kolkodnvr ; nomkod++)
 {
//  if(itogvert(kolkodnvr,rp->kolmes,nomkod,sumanvr) == 0.)
   if(sumanvr->suma_kol(rp->kolmes,kolkodnvr,nomkod) == 0.)
    continue;
  itog=0.;
  for(nommes=0; nommes < rp->kolmes; nommes++)
    itog+=sumanvr->ravno(nommes*kolkodnvr+nomkod);
  fprintf(ff," %10.2f",itog);
  itog1-=itog;
 }

itogind=0.;
for(nomkod=0; nomkod < kolkodnvrind ; nomkod++)
 {
//  if(itogvert(kolkodnvrind,rp->kolmes,nomkod,sumanvi) == 0.)
  if(sumanvi->suma_kol(rp->kolmes,kolkodnvrind,nomkod) == 0.)
    continue;
  itog=0.;
  for(nommes=0; nommes < rp->kolmes; nommes++)
    itog+=sumanvi->ravno(nommes*kolkodnvrind+nomkod);
  fprintf(ff," %10.2f",itog);
  itogind+=itog;
 }

fprintf(ff," %10.2f",itogo_v_rashet);


fprintf(ff," %8s %10.2f"," ",sumaindekc);
fprintf(ff,"\n");

if(itogind != 0.)
 {
  fprintf(ff,"\n%.2f+%.2f=%.2f\n",itogind,sumaindekc,itogind+sumaindekc);
 }
double suma_otp=0.;
class iceb_u_str kol_d(""); 

int kol_pol=iceb_u_pole2(rp->kol_dnei.ravno(),',');

if(kol_pol > 0)
 {
  fprintf(ff,"\n");
  for(int nom=1;nom <= kol_pol ; nom++)
   {
    iceb_u_polen(rp->kol_dnei.ravno(),&kol_d,nom,',');
    if(kol_d.ravno_atoi() == 0)
     continue;
    suma_otp=sumaindekc/itogo_dni[2]*kol_d.ravno_atoi();
    fprintf(ff,"%.2f/%d*%d=%.2f\n",sumaindekc,itogo_dni[2],kol_d.ravno_atoi(),suma_otp);
    
   }
 }
else
 if(rp->kol_dnei.getdlinna() > 1)
  {
   suma_otp=sumaindekc/itogo_dni[2]*rp->kol_dnei.ravno_atoi();
   fprintf(ff,"\n%.2f/%d*%d=%.2f\n",sumaindekc,itogo_dni[2],rp->kol_dnei.ravno_atoi(),suma_otp);
  }
fprintf(ff,"\n-----------------------------------------------------\n");
fprintf(ff,"-----------------------------------------------------\n");
fprintf(ff,"-----------------------------------------------------\n");
fprintf(ff,"-----------------------------------------------------\n");
fprintf(ff,"-----------------------------------------------------\n");


return(sumaindekc);

}
/*********************************/
/*Чтение коєффициентов индексации*/
/*********************************/
void readki(iceb_u_spisok *DATAI,iceb_u_double *KOF,GtkWidget *wpredok)
{
class iceb_u_str bros("");
class iceb_u_str bros1("");
short m,g;
int   nomer;
double kof=0.;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='zarotp.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarotp.alx");
  iceb_menu_soob(strsql,wpredok);
  return;
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],&bros,1,'|') != 0)
    continue;
  if(iceb_u_polen(bros.ravno(),&bros1,1,'.') != 0)
    continue;
  m=atoi(bros.ravno());

  if(iceb_u_polen(bros.ravno(),&bros1,2,'.') != 0)
    continue;
  g=bros1.ravno_atoi();  

  if(iceb_u_polen(row_alx[0],&bros1,2,'|') != 0)
    continue;
  kof=bros1.ravno_atof();  

  sprintf(strsql,"%d.%d",m,g);
  if((nomer=DATAI->find(strsql)) < 0)
    DATAI->plus(strsql); 

  KOF->plus(kof,nomer);
//  printw("%s**%s\n",row_alx[0],bros);    

 }


}


/*********************************************/
/*расчёт одного табельного номера*/
/**********************************************/
double sprtabotp_r1tn(class sprtabotpw_r_data *data,
int tabn,
class sprtabotp_dat *dt,
class iceb_zarrsov *svod_otvr, /*свод по отработанному времени*/
FILE *ff,
FILE *ff_prot)
{
char strsql[1024];
int kolstr=0;
class SQLCURSOR cur,cur1,curr;
SQL_str row,row1;


int metka_dkn=0; /*метка присутствия начислений для дополнительных кодов*/



sprintf(strsql,"select fio,dolg,podr,datn from Kartb where tabn=%d",tabn);
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabn);
  iceb_menu_soob(strsql,data->window);
  return(-1.);
 }
dt->fio.new_plus(row[0]);
dt->dolg.new_plus(row[1]);
dt->kodpodr.new_plus(row[2]);

//Читаем наименование подразделения
sprintf(strsql,"select naik from Podr where kod=%s",dt->kodpodr.ravno());
if(sql_readkey(&bd,strsql,&row,&curr) == 1)
 dt->naipod.new_plus(row[0]);
else
 dt->naipod.new_plus("");



int kol_kd=iceb_u_pole2(data->rp->kodn_d_dk.ravno(),',');
if(kol_kd == 0 && data->rp->kodn_d_dk.ravno()[0] != '\0')
  kol_kd=1;
/*опрeделяем есть ли начисления для дополнительных кодов*/
for(int nom=0; nom < kol_kd; nom++)
 {
  int kdn=0;
  
  if(kol_kd == 1)
   kdn=data->rp->kodn_d_dk.ravno_atoi();
  else
   iceb_u_polen(data->rp->kodn_d_dk.ravno(),&kdn,nom+1,',');
    
  sprintf(strsql,"select datz,knah,suma,shet from Zarp where tabn=%d \
and datz >= '%04d-%d-%d' and datz <='%04d-%d-%d' and prn='1' and suma <> 0. and knah=%d limit 1",
  tabn,data->rp->gn,data->rp->mn,1,data->rp->gk,data->rp->mk,data->rp->dk,kdn);
  if(iceb_sql_readkey(strsql,data->window) == 1)
   {
    metka_dkn=1;
    break;
   }
 }

if(metka_dkn == 1)
 {
  data->rp->kodnvr.z_plus(data->rp->kodnvr_d.ravno());
 }



int kolkodnvr=iceb_u_pole2(data->rp->kodnvr.ravno(),',');
if(kolkodnvr == 0 && data->rp->kodnvr.ravno()[0] != '\0')
  kolkodnvr=1;

int kolkodnvrind=iceb_u_pole2(data->rp->kodnvrind.ravno(),',');
if(kolkodnvrind == 0 && data->rp->kodnvrind.ravno()[0] != '\0')
  kolkodnvrind=1;
  

//Читаем коэффициенты индексации

readki(&dt->DATAI,&dt->KOF,data->window);


//double sumanmes[kolmes]; //Сумма начисленная
//memset(&sumanmes,'\0',sizeof(sumanmes));
dt->sumanmes.make_class(data->rp->kolmes);

//double sumanvr[kolmes*kolkodnvr]; //Суммы не входящие в расчёт
//memset(&sumanvr,'\0',sizeof(sumanvr));
dt->sumanvr.make_class(data->rp->kolmes*kolkodnvr);

//double sumanmesxr[data->rp->kolmes]; //Сумма начисленная хозрасчёт
//memset(&sumanmesxr,'\0',sizeof(sumanmesxr));
dt->sumanmesxr.make_class(data->rp->kolmes);

//double sumanvrxr[data->rp->kolmes*kolkodnvr]; //Суммы не входящие в расчёт хозрасчёт
//memset(&sumanvrxr,'\0',sizeof(sumanvrxr));
dt->sumanvrxr.make_class(data->rp->kolmes*kolkodnvr);


//double sumanmesbu[data->rp->kolmes]; //Сумма начисленная бюджет
//memset(&sumanmesbu,'\0',sizeof(sumanmesbu));
dt->sumanmesbu.make_class(data->rp->kolmes);

//double sumanvrbu[data->rp->kolmes*kolkodnvr]; //Суммы не входящие в расчёт бюджет
//memset(&sumanvrbu,'\0',sizeof(sumanvrbu));
dt->sumanvrbu.make_class(data->rp->kolmes*kolkodnvr);

//double sumanvi[data->rp->kolmes*kolkodnvrind]; //Сумма не входящая в расчёт индексации отпускных
//memset(sumanvi,'\0',sizeof(sumanvi));
dt->sumanvi.make_class(data->rp->kolmes*kolkodnvrind);

//double sumanvixr[data->rp->kolmes*kolkodnvrind]; //Сумма не входящая в расчёт индексации отпускных хозрасчёта
//memset(sumanvixr,'\0',sizeof(sumanvixr));
dt->sumanvixr.make_class(data->rp->kolmes*kolkodnvrind);

// double sumanvibu[data->rp->kolmes*kolkodnvrind]; //Сумма не входящая в расчёт индексации отпускных бюджета
//memset(sumanvibu,'\0',sizeof(sumanvibu));
dt->sumanvibu.make_class(data->rp->kolmes*kolkodnvrind);

dt->kolkd.make_class(data->rp->kolmes);

dt->kol_pd.make_class(data->rp->kolmes);

dt->kolkd_vdr.make_class(data->rp->kolmes);




sprintf(strsql,"select datz,knah,suma,shet from Zarp where tabn=%d \
and datz >= '%04d-%d-%d' and datz <='%04d-%d-%d' and prn='1' and suma <> 0.",
tabn,data->rp->gn,data->rp->mn,1,data->rp->gk,data->rp->mk,data->rp->dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  return(1);
 }
int kolstr1=0;
short d=0,m=0,g=0;
short dnr=0,mnr=0,gnr=0;
double suma=0.;

while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(dnr != 0 && dnr != 1) //Если первый месяц отработан не полностью в расчёт его не берем
    if(mnr == m && gnr == g)
      continue;

  suma=atof(row[2]);

  int nommes=iceb_u_period(1,data->rp->mn,data->rp->gn,d,m,g,1)-1;    

//  sumanmes[nommes]+=suma;
  dt->sumanmes.plus(suma,nommes);

  if(iceb_u_proverka(shetb,row[3],0,1) == 0)
    dt->sumanmesbu.plus(suma,nommes);
  else 
    dt->sumanmesxr.plus(suma,nommes);
  int nomkod=0;
  if(iceb_u_proverka(data->rp->kodnvr.ravno(),row[1],0,1) == 0)
   {
    iceb_u_pole1(data->rp->kodnvr.ravno(),row[1],',',0,&nomkod);
//    sumanvr[nommes*kolkodnvr+nomkod]+=suma;
    dt->sumanvr.plus(suma,nommes*kolkodnvr+nomkod);
    if(iceb_u_proverka(shetb,row[3],0,1) == 0)
      dt->sumanvrbu.plus(suma,nommes*kolkodnvr+nomkod);
    else 
      dt->sumanvrxr.plus(suma,nommes*kolkodnvr+nomkod);
//      sumanvrxr[nommes*kolkodnvr+nomkod]+=suma;

   }
  else
   {
    if(dt->skodnvr.find(row[1]) < 0)
     dt->skodnvr.plus(row[1]);
   }

  if(iceb_u_proverka(data->rp->kodnvrind.ravno(),row[1],0,1) == 0)
   {
    iceb_u_pole1(data->rp->kodnvrind.ravno(),row[1],',',0,&nomkod);
    dt->sumanvi.plus(suma,nommes*kolkodnvrind+nomkod);
    if(iceb_u_proverka(shetb,row[3],0,1) == 0)
      dt->sumanvibu.plus(suma,nommes*kolkodnvrind+nomkod);
    else 
      dt->sumanvixr.plus(suma,nommes*kolkodnvrind+nomkod);
//      sumanvixr[nommes*kolkodnvrind+nomkod]+=suma;

   }
 }
/*Создаём масив для сумм начислений вошедших в расчёт*/
int kolih_kod=dt->skodnvr.kolih();
dt->sum_vr.make_class(kolih_kod*data->rp->kolmes);
kolstr1=0;
cur.poz_cursor(0);
int nommes=0,nomkod=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(dnr != 0 && dnr != 1) //Если первый месяц отработан не полностью в расчёт его не берем
    if(mnr == m && gnr == g)
      continue;

  suma=atof(row[2]);

  nommes=iceb_u_period(1,data->rp->mn,data->rp->gn,d,m,g,1)-1;    
  if((nomkod=dt->skodnvr.find(row[1])) < 0)
   continue;
  dt->sum_vr.plus(suma,nommes*kolih_kod+nomkod);
 }

 
//Узнаем количество календарных дней
if(data->rp->gn != data->rp->gk)
  sprintf(strsql,"select kodt,god,mes,dnei,kdnei,has from Ztab where tabn=%d and \
((god > %d and god < %d) \
or (god=%d and mes >= %d) \
or (god=%d and mes <= %d)) order by god asc,mes asc",
  tabn,data->rp->gn,data->rp->gk,data->rp->gn,data->rp->mn,data->rp->gk,data->rp->mk);
else
  sprintf(strsql,"select kodt,god,mes,dnei,kdnei,has from Ztab where tabn=%d and \
god=%d and mes >= %d and mes <= %d order by god asc,mes asc",
  tabn,data->rp->gn,data->rp->mn,data->rp->mk);

//printw("%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
kolstr1=0;
d=1;
int nomvs=0;
int mes_z=0;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
  g=atoi(row[1]);
  m=atoi(row[2]);
  nommes=iceb_u_period(1,data->rp->mn,data->rp->gn,d,m,g,1)-1;    
  dt->kolkd.plus(atoi(row[4]),nommes);

  if(iceb_u_proverka(data->rp->kodt_nvr.ravno(),row[0],0,1) != 0)
   dt->kolkd_vdr.plus(atoi(row[4]),nommes);

  /*узнаём количество праздничных дней в месяце*/
  if(mes_z != m)
   {
    sprintf(strsql,"select kpd from Zarskrdh where data='%04d-%02d-01'",g,m);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     dt->kol_pd.plus(atoi(row1[0]),nommes);
    mes_z=m;
   }

  if(svod_otvr->kod.find(row[0]) < 0)
     svod_otvr->kod.plus(row[0]);

  sprintf(strsql,"%s|%02d.%04d",row[0],m,g);

  if((nomvs=svod_otvr->kod_mes_god.find(strsql)) < 0)
   svod_otvr->kod_mes_god.plus(strsql);

  svod_otvr->kolrd.plus(row[3],nomvs);    
  svod_otvr->kolkd.plus(row[4],nomvs);    
  svod_otvr->kolhs.plus(atof(row[5]),nomvs);    

 }

//GDITE();


//Распечатываем общий итог
double sumai=rasrez(tabn,data->rp,dt,&dt->sumanmes,&dt->sumanvr,0,&dt->sumanvi,ff,ff_prot,data->window);

return(sumai);
}

/******************************************/
/*Распечатка начислений вошедших в расчёт*/
/******************************************/
void sprtabotp_rnvvr(int tabn,short mn,short gn,
short dk,short mk,short gk,
class iceb_u_int *skodnvr,
class iceb_u_double *sum_vr,FILE *ff,
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str naik;
char strsql[512];
double itogo_gor=0.;
double suma=0.;
int kolmes=iceb_u_period(1,mn,gn,dk,mk,gk,1);

short d=1,m=mn,g=gn;

iceb_zagolov(gettext("Начисления взятые в расчёт отпускных"),1,mn,gn,dk,mk,gk,ff,wpredok);

int kolih_kod=skodnvr->kolih();
class iceb_u_double itogo_kol;

itogo_kol.make_class(kolmes);
sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);
naik.new_plus("");
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naik.new_plus(row[0]);

fprintf(ff,"%d %s\n",tabn,naik.ravno());


class iceb_u_str liniq;
liniq.plus("-------------------------------------");

for(int nom=0; nom <= kolmes; nom++)
 liniq.plus("-----------");

fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff," Код |   Наименование начисления    |");
/*          12345 123456789012345678901234567890*/

for(int nom_mes=0; nom_mes < kolmes; nom_mes++)
 {
  fprintf(ff," %02d.%04d  |",m,g);
  iceb_u_dpm(&d,&m,&g,3);
 }

fprintf(ff,"%-*.*s|\n",
iceb_u_kolbait(10,gettext("Итого")),
iceb_u_kolbait(10,gettext("Итого")),
gettext("Итого"));
fprintf(ff,"%s\n",liniq.ravno());

for(int nom_kod=0; nom_kod < kolih_kod; nom_kod++)
 {
  naik.new_plus("");
  
  sprintf(strsql,"select naik from Nash where kod=%d",skodnvr->ravno(nom_kod));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naik.new_plus(row[0]);

  fprintf(ff,"%5d %-*.*s|",skodnvr->ravno(nom_kod),
  iceb_u_kolbait(30,naik.ravno()),
  iceb_u_kolbait(30,naik.ravno()),
  naik.ravno());
  itogo_gor=0.;
  for(int nom_mes=0; nom_mes < kolmes; nom_mes++)
   {
    suma=sum_vr->ravno(nom_mes*kolih_kod+nom_kod);
    if(suma != 0.)
     {
      fprintf(ff,"%10.2f|",suma);
      itogo_gor+=suma;    
      itogo_kol.plus(suma,nom_mes);
     }
    else
      fprintf(ff,"%10s|","");
     
   }
  fprintf(ff,"%10.2f|\n",itogo_gor);
  iceb_u_dpm(&d,&m,&g,3);

 }
fprintf(ff,"%s\n",liniq.ravno());
fprintf(ff,"%*.*s|",
iceb_u_kolbait(36,gettext("Итого")),
iceb_u_kolbait(36,gettext("Итого")),
gettext("Итого"));

for(int nom_mes=0; nom_mes < kolmes; nom_mes++)
 fprintf(ff,"%10.2f|",itogo_kol.ravno(nom_mes));
fprintf(ff,"%10.2f|\n",itogo_kol.suma());


}




/************************************************/

/***********************************************/
int sprtabotp_r(class sprtabotpw_r_data *data)
{
char		imaf[64],imafbu[64],imafxr[64];
class SQLCURSOR cur,cur1;
short		dk=31;
class SQLCURSOR curr;

class sprtabotp_dat dt;


iceb_u_dpm(&data->rp->dk,&data->rp->mk,&data->rp->gk,5); //получить последний день месяца



class iceb_fopen fil_prot;
char imaf_prot[64];
sprintf(imaf_prot,"otpp%d.lst",getpid());
if(fil_prot.start(imaf_prot,"w",data->window) != 0)
 return(1);

iceb_zagolov(gettext("Протокол хода расчёта отпускных"),1,data->rp->mn,data->rp->gn,data->rp->dk,data->rp->mk,data->rp->gk,fil_prot.ff,data->window);


class iceb_fopen fil;
sprintf(imaf,"otp%d.lst",getpid());
if(fil.start(imaf,"w",data->window) != 0)
 return(1);

fprintf(fil.ff,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/


class iceb_zarrsov svod_otvr; /*свод по отработанному времени*/


sprtabotp_r1tn(data,data->rp->tabnom.ravno_atoi(),&dt,&svod_otvr,fil.ff,fil_prot.ff);


iceb_podpis(fil.ff,data->window);
fil.end();


char imaf_sn[64];
sprintf(imaf_sn,"otpn%d.lst",getpid());
class iceb_fopen fil_rsvod;
if(fil_rsvod.start(imaf_sn,"w",data->window) != 0)
 return(1);
 
sprtabotp_rnvvr(data->rp->tabnom.ravno_atoi(),data->rp->mn,data->rp->gn,dk,data->rp->mk,data->rp->gk,&dt.skodnvr,&dt.sum_vr,fil_rsvod.ff,data->window);
zarrsovw(data->rp->mn,data->rp->gn,data->rp->mk,data->rp->gk,data->rp->tabnom.ravno_atoi(),&svod_otvr,fil_rsvod.ff,data->window);

iceb_podpis(fil_rsvod.ff,data->window);
fil_rsvod.end();

data->oth->spis_imaf.plus(imaf);
data->oth->spis_naim.plus(gettext("Расчёт отпускных"));
data->oth->spis_imaf.plus(imaf_sn);
data->oth->spis_naim.plus(gettext("Свод начислений и отработанного времени"));

if(shetb != NULL)
 {
  sprintf(imafbu,"otpbu%d.lst",getpid());

  if(fil.start(imafbu,"w",data->window) != 0)
   return(1);


  rasrez(data->rp->tabnom.ravno_atoi(),data->rp,&dt,&dt.sumanmesbu,&dt.sumanvrbu,1,&dt.sumanvibu,fil.ff,fil_prot.ff,data->window);

  iceb_podpis(fil.ff,data->window);
  fil.end();


  sprintf(imafxr,"otpxr%d.lst",getpid());
  if(fil.start(imafxr,"w",data->window) != 0)
   return(1);

  rasrez(data->rp->tabnom.ravno_atoi(),data->rp,&dt,&dt.sumanmesxr,&dt.sumanvrxr,2,&dt.sumanvixr,fil.ff,fil_prot.ff,data->window);
  iceb_podpis(fil.ff,data->window);

  fil.end();

  
  data->oth->spis_imaf.plus(imafbu);
  data->oth->spis_naim.plus(gettext("Расчёт бюджетных отпускных"));
  
  data->oth->spis_imaf.plus(imafxr);
  data->oth->spis_naim.plus(gettext("Расчёт хозрасчётных отпускных"));
 }
iceb_podpis(fil_prot.ff,data->window);
fil_prot.end();

data->oth->spis_imaf.plus(imaf_prot);
data->oth->spis_naim.plus(gettext("Протокол хода расчёта"));


for(int nom=0; nom < data->oth->spis_imaf.kolih(); nom++)
 iceb_ustpeh(data->oth->spis_imaf.ravno(nom),3,data->window);

 
return(0);

}


/**************************************************/
/*расчёт по нескольким табельным номерам*/
/***********************************************/
int sprtabotp_rntn(class sprtabotpw_r_data *data)
{
char strsql[1024];
class SQLCURSOR cur,cur1;
SQL_str row,row1;
int kolstr=0;

sprintf(strsql,"select distinct tabn from Zarp where datz >= '%04d-%02d-01' and \
datz <= '%04d-%02d-%02d' and prn = '1' and suma <> 0. order by tabn asc",
data->rp->gn,data->rp->mn,
data->rp->gk,data->rp->mk,data->rp->dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  return(1);
 }
int kolstr1=0;


class iceb_fopen fil_prot;
char imaf_prot[64];
sprintf(imaf_prot,"otpp%d.lst",getpid());
if(fil_prot.start(imaf_prot,"w",data->window) != 0)
 return(1);

iceb_zagolov(gettext("Протокол хода расчёта отпускных"),1,data->rp->mn,data->rp->gn,data->rp->dk,data->rp->mk,data->rp->gk,fil_prot.ff,data->window);

class iceb_fopen fil_sp;
char imaf_sp[64];
sprintf(imaf_sp,"otsp%d.lst",getpid());
if(fil_sp.start(imaf_sp,"w",data->window) != 0)
 return(1);

iceb_zagolov(gettext("Расчёт отпускных"),1,data->rp->mn,data->rp->gn,data->rp->dk,data->rp->mk,data->rp->gk,fil_sp.ff,data->window);


if(data->rp->metka_ri == 0)
 fprintf(fil_sp.ff,"%s\n",gettext("Расчёт с использованием должностных окладов"));
if(data->rp->metka_ri == 1)
 fprintf(fil_sp.ff,"%s\n",gettext("Расчёт с использованием коэффициентов"));

if(data->rp->tabnom.getdlinna() > 1)
 fprintf(fil_sp.ff,"%s:%s\n",gettext("Табельный номер"),data->rp->tabnom.ravno());

if(data->rp->kod_podr.getdlinna() > 1)
 fprintf(fil_sp.ff,"%s:%s\n",gettext("Код подразделения"),data->rp->kod_podr.ravno());
 
class sprtabotp_dat dt;

class iceb_fopen fil;
char imaf[64];
sprintf(imaf,"otp%d.lst",getpid());
if(fil.start(imaf,"w",data->window) != 0)
 return(1);

fprintf(fil.ff,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/

gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);

double itogo=0.;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
  if(iceb_u_proverka(data->rp->tabnom.ravno(),row[0],0,0) != 0)
   continue;

  sprintf(strsql,"select podr from Kartb where tabn=%d",atoi(row[0]));
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s-%s %d !",__FUNCTION__,gettext("Не найден табельный номер"),atoi(row[0]));
    iceb_menu_soob(strsql,data->window);
   }
  else
   {
    if(iceb_u_proverka(data->rp->kod_podr.ravno(),row1[0],0,0) != 0)
     continue;
   }

  class sprtabotp_dat dt;
  
  class iceb_zarrsov svod_otvr; /*свод по отработанному времени*/

  if(kolstr1 > 1)
   fprintf(fil.ff,"\f");
   
  double suma=sprtabotp_r1tn(data,atoi(row[0]),&dt,&svod_otvr,fil.ff,fil_prot.ff);
  itogo+=suma;
  
  sprintf(strsql,"%6s %-*s %12.2f \n",row[0],
  iceb_u_kolbait(40,dt.fio.ravno()),
  dt.fio.ravno(),suma);
  
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
  
  
  fprintf(fil_sp.ff,"%6s %-*s %12.2f\n",row[0],
  iceb_u_kolbait(40,dt.fio.ravno()),
  dt.fio.ravno(),suma);
 }

sprintf(strsql,"%*s:%12.2f\n",
iceb_u_kolbait(47,gettext("Итого")),
gettext("Итого"),itogo);
  
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

fprintf(fil_sp.ff,"%*s:%12.2f\n",
iceb_u_kolbait(47,gettext("Итого")),
gettext("Итого"),itogo);


iceb_podpis(fil.ff,data->window);
fil.end();

iceb_podpis(fil_prot.ff,data->window);
fil_prot.end();

iceb_podpis(fil_sp.ff,data->window);
fil_sp.end();

data->oth->spis_imaf.plus(imaf);
data->oth->spis_naim.plus(gettext("Расчёт отпускных"));
data->oth->spis_imaf.plus(imaf_sp);
data->oth->spis_naim.plus(gettext("Итоговый список"));
data->oth->spis_imaf.plus(imaf_prot);
data->oth->spis_naim.plus(gettext("Протокол хода расчёта"));

for(int nom=0; nom < data->oth->spis_imaf.kolih(); nom++)
 iceb_ustpeh(data->oth->spis_imaf.ravno(nom),3,data->window);

 
return(0);

}







/******************************************/
/******************************************/

gint sprtabotpw_r1(class sprtabotpw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);

if(iceb_rsdatp1(&data->rp->mn,&data->rp->gn,data->rp->datan.ravno(),&data->rp->mk,&data->rp->gk,data->rp->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(strsql,"%d.%d%s => %d.%d%s\n",
data->rp->mn,data->rp->gn,gettext("г."),
data->rp->mk,data->rp->gk,gettext("г."));

iceb_printw(strsql,data->buffer,data->view);


data->rp->kolmes=iceb_u_period(1,data->rp->mn,data->rp->gn,data->rp->dk,data->rp->mk,data->rp->gk,1);

iceb_poldan("Коды не входящие в расчёт отпускных",&data->rp->kodnvr,"zarotp.alx",data->window);
iceb_poldan("Коды не входящие в расчёт индексации отпускных",&data->rp->kodnvrind,"zarotp.alx",data->window);
iceb_poldan("Коды табеля не входящие в расчёт",&data->rp->kodt_nvr,"zarotp.alx",data->window);
iceb_poldan("Дополнительные коды не входящие в расчёт",&data->rp->kodnvr_d,"zarotp.alx",data->window);
iceb_poldan("Коды начислений для дополнительных кодов",&data->rp->kodn_d_dk,"zarotp.alx",data->window);




if(data->rp->tabnom.getdlinna() <= 1 || iceb_u_pole2(data->rp->tabnom.ravno(),',') > 0)
 {
  /*расчёт отпускных по нескольким табельных номерам*/
  data->voz=sprtabotp_rntn(data);
 }
else
 {
  /*расчёт отпускных по одному табельному номеру*/
  data->voz=sprtabotp_r(data);
 }











gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

return(FALSE);

}
