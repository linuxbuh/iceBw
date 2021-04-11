/*$Id: iceb_spks_r.c,v 1.49 2014/08/31 06:19:20 sasa Exp $*/
/*21.05.2016	10.03.2006	Белых А.И.	iceb_spks_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "iceb_libbuh.h"
#include "iceb_spks.h"

class iceb_spks_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class iceb_spks_data *rk;
  class spis_oth *oth;  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  iceb_spks_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   iceb_spks_r_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_spks_r_data *data);
gint iceb_spks_r1(class iceb_spks_r_data *data);
void  iceb_spks_r_v_knopka(GtkWidget *widget,class iceb_spks_r_data *data);


extern SQL_baza bd;
extern iceb_u_str shrift_ravnohir;
extern int      kol_strok_na_liste;

int iceb_spks_r(class iceb_spks_data *datark,class spis_oth *oth,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class iceb_spks_r_data data;

printf("%s\n",__FUNCTION__);

data.rk=datark;
data.oth=oth;
data.name_window.plus(__FUNCTION__);

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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(iceb_spks_r_key_press),&data);

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(iceb_spks_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)iceb_spks_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  iceb_spks_r_v_knopka(GtkWidget *widget,class iceb_spks_r_data *data)
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

gboolean   iceb_spks_r_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_spks_r_data *data)
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
/**************/
/*Вывод сальдо*/
/**************/
void salorksl(double dd, double kk,double ddn,double kkn,
double debm,double krem,
FILE *ff1,FILE *ffact)
{

double brr=dd+ddn;
double krr=kk+kkn;
fprintf(ff1,"\
---------------------------------------------------------------------------------------\n");

if(debm != 0. || krem != 0.)
 {
  fprintf(ff1,"\
%*s:%12s",iceb_u_kolbait(37,gettext("Оборот за месяц")),gettext("Оборот за месяц"),iceb_u_prnbr(debm));

  fprintf(ff1," %12s\n",iceb_u_prnbr(krem));
  if(ffact != NULL)
   fprintf(ffact,"%*s %10.2f %10.2f| |\n",
   iceb_u_kolbait(31,gettext("Оборот за месяц")),gettext("Оборот за месяц"),debm,krem);
 }

fprintf(ff1,"%*s:%12s",
iceb_u_kolbait(37,gettext("Оборот за период")),
gettext("Оборот за период"),
iceb_u_prnbr(dd));

fprintf(ff1," %12s\n",iceb_u_prnbr(kk));

if(ffact != NULL)
 fprintf(ffact,"%*s %10.2f %10.2f| |\n",
 iceb_u_kolbait(31,gettext("Оборот за период")),gettext("Оборот за период"),dd,kk);

fprintf(ff1,"%*s:%12s",
iceb_u_kolbait(37,gettext("Сальдо раз-нутое")),
gettext("Сальдо раз-нутое"),
iceb_u_prnbr(brr));
fprintf(ff1," %12s\n",iceb_u_prnbr(brr));

if(ffact != NULL)
 fprintf(ffact,"%*s %10.2f %10.2f| |\n",
 iceb_u_kolbait(31,gettext("Сальдо раз-нутое")),gettext("Сальдо раз-нутое"),brr,krr);

if(dd+ ddn >= kk+kkn)
 {
  brr=(dd+ddn)-(kk+kkn);
  fprintf(ff1,"%*s:%12s\n",
  iceb_u_kolbait(37,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),iceb_u_prnbr(brr));

  if(ffact != NULL)
   fprintf(ffact,"%*s %10.2f %10s| |\n",
   iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое"),brr," ");
 }

 {
  brr=(kk+kkn)-(dd+ddn);
  fprintf(ff1,"%*s:%12s %12s\n",
  iceb_u_kolbait(37,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",iceb_u_prnbr(brr));


  if(ffact != NULL)
   fprintf(ffact,"%*s %10s %10.2f| |\n",
   iceb_u_kolbait(31,gettext("Сальдо свернутое")),gettext("Сальдо свернутое")," ",brr);

 }
}
/*****************************/
/*Заголовок акта сверки      */
/*****************************/

void   zagacts(short dn,short mn,short gn,
short dk,short mk,short gk,
short d,short m,short g,
const char *kodkontr,const char *naimkont,FILE *ff)
{


fprintf(ff,gettext("\
                                       АКТ СВЕРКИ ВЗАИМНЫХ РАСЧЕТОВ.\n\n\
   Мы ниже подписавшиеся, главный бухгалтер %s\n\
________________________________________________________________________________________________________\n\
                    (наименование учереждения, фамилия и инициалы)\n\
и главный бухгалтер %s\n\
_________________________________________________________________________________________________________\n\
                   (наименование учереждения, фамилия и инициалы)\n\
составили настоящий акт о том, что проведена проверка взаимных расчётов.\n"),
iceb_get_pnk("00",0,NULL),naimkont);

fprintf(ff,"%s %02d.%02d.%d%s %s %02d.%02d.%d%s\n",
gettext("За период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."));

fprintf(ff,"%s:%02d.%02d.%d %s\n\n",
gettext("По состоянию на"),
d,m,g,gettext("г."));

fprintf(ff,"%s:\n",gettext("При проверке установлено"));

fprintf(ff,"\
%s %-51s %s %s %s\n",
gettext("По"),
iceb_get_pnk("00",0,NULL),
gettext("По"),
kodkontr,naimkont);



}
/*********************************/
/*Концовка акта сверки           */
/*********************************/

void iceb_spks_r_konact(FILE *ff)
{
fprintf(ff,"\n");
fprintf(ff,gettext("\
После отражения в учете указанных дополнительных проводок сальдо на ________________________\n\
выражается в сумме__________________________________________________________________________\n\
____________________________________________________________________________________________\n\
                  (сумма прописью и цифрами)\n\
в пользу____________________________________________________________________________________\n\
                   (Указать наименование учереждения)\n\n\
    Главный бухгалтер________________________        Главный бухгалтер________________________\n"));
                                     
fprintf(ff,"\n");

SQL_str row;
SQLCURSOR cur;
class iceb_u_str telef("");
if(iceb_sql_readkey("select telef from Kontragent where kodkon='00'",&row,&cur,NULL) == 1)
 telef.new_plus(row[0]);

fprintf(ff,gettext("\
Вышлите нам, пожалуйста, один экземпляр акта с Вашими подписью и печатью.\n\
В случае несоответствия сумм, просим выслать розшифровку или провести сверку\n\
по телефону"));

fprintf(ff," %s\n",telef.ravno()); 

fprintf(ff,gettext("\
Просим в течении 5 дней вернуть подписанный акт.\n\
При невозвращении акта, сумма будет зачтена, как согласованная."));

iceb_podpis(ff,NULL);

}
/*********************************/
/*Шака                           */
/*********************************/

void	zagsh(const char *shet,FILE *ff)
{
char	strsql[512];
SQL_str row;
SQLCURSOR cur;
class iceb_u_str naim("");

sprintf(strsql,"select nais from Plansh where ns='%s'",shet);
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 naim.new_plus(row[0]);
 
fprintf(ff,"\n%s:%s %s\n",gettext("Счет"),shet,naim.ravno());
fprintf(ff,"\
------------------------------------------------------ ---------------------------------------------------\n");
fprintf(ff,gettext("\
  Дата    |   Коментарий       | Дебет    | Кредит   | |  Дата    |   Коментарий       |  Дебет | Кредит |\n"));

fprintf(ff,"\
------------------------------------------------------ ---------------------------------------------------\n");

}
/**********************************/
/*Распечатка акта сверки с общей суммой по документу*/
/*****************************************************/
void  salork_akt_sv(class akt_sverki *akt_svr,short dn,short mn,short gn,
short dk,short mk,short gk,
const char *kor,const char *nai,
const char *imaf,
const char *imaf_bi,
GtkWidget *wpredok)
{
class iceb_fopen ff;
class iceb_fopen ff_bi;

if(ff.start(imaf,"w",wpredok) != 0)
 return;
if(ff_bi.start(imaf_bi,"w",wpredok) != 0)
 return;

short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

zagacts(dn,mn,gn,dk,mk,gk,dt,mt,gt,kor,nai,ff.ff);
zagacts(dn,mn,gn,dk,mk,gk,dt,mt,gt,kor,nai,ff_bi.ff);

iceb_akt_sv(akt_svr,ff.ff,ff_bi.ff);

iceb_spks_r_konact(ff.ff);

iceb_podpis(ff.ff,wpredok);
ff.end();

iceb_spks_r_konact(ff_bi.ff);

iceb_podpis(ff_bi.ff,wpredok);
ff_bi.end();

}

/******************************************/
/******************************************/

gint iceb_spks_r1(class iceb_spks_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;

time_t          tmm;
struct  tm      *bf;
short           d,m,g;
//FILE            *ff1=NULL,*ffact=NULL;
class iceb_fopen ff1;
class iceb_fopen ffact;
double          brr;
class iceb_u_str shh("");
double          ddn,kkn;
double		dd,kk;
short           msv; /*0-без сверки 1-со сверкой*/
short           mvp; /*Метка второго прохода*/
short           mvs; /*Метка выдачи сальдо*/
long		kolstr;
short		godn;
short		shett=0;
short		mes=0,god=0;
double		deb=0.,kre=0.;
double		debm=0.,krem=0.;
class iceb_u_str koment("");
short	startgodb=0; /*Стартовый год*/

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datn.ravno(),&dk,&mk,&gk,data->rk->datk.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);

 }




if(iceb_poldan("Стартовый год",strsql,"nastrb.alx",data->window) == 0)
 startgodb=atoi(strsql);

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;

mvp=msv=0;
if(data->rk->shets.ravno()[0] != '\0')
  msv=1;

shh.new_plus(data->rk->shetsrs.ravno());

char imaf[64];
sprintf(imaf,"slo%d.lst",getpid());
if(ff1.start(imaf,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);

 }

char imafact[64];
sprintf(imafact,"act%d.lst",getpid());
if(ffact.start(imafact,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);

 }

naz:;

ddn=kkn=0.;
dd=kk=0.;
sprintf(strsql,"select deb,kre from Saldo where kkk='1' and gs=%d and \
ns='%s' and kodkon='%s'",godn,shh.ravno(),data->rk->kontr.ravno());

if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  ddn=atof(row[0]);
  kkn=atof(row[1]);
 }

sprintf(strsql,"%s %s\n%s %s %s\n",
gettext("Расчёт сальдо по счёту"),
shh.ravno(),
gettext("для контрагента"),
data->rk->kontr.ravno(),
data->rk->naikontr.ravno());

iceb_printw(strsql,data->buffer,data->view);

time(&tmm);
bf=localtime(&tmm);

fprintf(ff1.ff,"%s\n\
%s %s %s %s %s\n\
%s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d  %s - %d:%d\n",
iceb_get_pnk("00",0,data->window),
gettext("Распечатка проводок по счёту"),
shh.ravno(),
gettext("для контрагента"),
data->rk->kontr.ravno(),data->rk->naikontr.ravno(),
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("до"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("Время"),
bf->tm_hour,bf->tm_min);

fprintf(ff1.ff,"\
--------------------------------------------------------------------------------------------------\n");

fprintf(ff1.ff,gettext("  Дата    |Сче.к.|Записал|Номер доку.|    Дебет   |   Кредит   |      К о м е н т а р и й       |\n"));
/*******
                                                                                               12345678901234567890123456789012
********/
fprintf(ff1.ff,"\
--------------------------------------------------------------------------------------------------\n");


//Заголовок акта сверки
zagacts(dn,mn,gn,dk,mk,gk,bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,data->rk->kontr.ravno(),data->rk->naikontr.ravno(),ffact.ff);
zagsh(data->rk->shetsrs.ravno(),ffact.ff);
  

mvs=0;
g=godn;
m=1;
d=1;
class akt_sverki akt_sv;
akt_sv.shet.new_plus(data->rk->shetsrs.ravno());

int nomer=0;

int kol_dnei=iceb_u_period(dn,mn,gn,dk,mk,gk,0);
float kolstr1=0.;
while(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
  {
   iceb_pbar(data->bar,kol_dnei,++kolstr1);    
   if(iceb_u_sravmydat(d,m,g,dn,mn,gn) >= 0 && mvs == 0)
    {
     akt_sv.dn=d;
     akt_sv.mn=m;
     akt_sv.gn=g;
     if(ddn > kkn)    
      {
       akt_sv.start_saldo_deb=ddn-kkn;
       ddn=ddn-kkn;
       kkn=0.;

         fprintf(ff1.ff,"%*s:%12s\n",
         iceb_u_kolbait(37,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
         iceb_u_prnbr(ddn));
         
         sprintf(strsql,"%*s:%12s\n",
         iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),iceb_u_prnbr(ddn));
         iceb_printw(strsql,data->buffer,data->view);

         fprintf(ffact.ff,"%02d.%02d.%4d %-20.20s %10.2f %10s| |\n",
         d,m,g,gettext("Сальдо начальное"),ddn," ");

         fprintf(ffact.ff,"\
------------------------------------------------------ ---------------------------------------------------\n");

      }
     else
      {
       akt_sv.start_saldo_kre=kkn-ddn;
       kkn=kkn-ddn;
       ddn=0.;

         fprintf(ff1.ff,"\
%*s:%12s %12s\n",
         iceb_u_kolbait(37,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",iceb_u_prnbr(kkn));

         sprintf(strsql,"\
%*s:%12s %12s\n",
         iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное")," ",iceb_u_prnbr(kkn));
         iceb_printw(strsql,data->buffer,data->view);

         fprintf(ffact.ff,"%02d.%02d.%4d %-*.*s %10s %10.2f| |\n",
         d,m,g,
         iceb_u_kolbait(20,gettext("Сальдо начальное")),iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
         " ",kkn);

         fprintf(ffact.ff,"\
------------------------------------------------------ ---------------------------------------------------\n");
      }     

     mvs=1;
    }

   sprintf(strsql,"select * from Prov \
where val=0 and datp='%d-%02d-%02d' and sh='%s' and kodkon='%s'",
   g,m,d,shh.ravno(),data->rk->kontr.ravno());

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
     iceb_msql_error(&bd,"",strsql,data->window);
     iceb_u_dpm(&d,&m,&g,1);
     continue;
    }
   if(kolstr == 0)
    {
     iceb_u_dpm(&d,&m,&g,1);
     continue;
    }
   shett=0;
   while(cur.read_cursor(&row) != 0)
    {

     if(data->rk->mks == 1)
       if(iceb_u_SRAV(row[2],data->rk->shetsrs.ravno(),1) == 0 && iceb_u_SRAV(row[3],data->rk->shets.ravno(),1) == 0)
         continue;
     deb=atof(row[9]);
     kre=atof(row[10]);

     if(iceb_u_sravmydat(d,m,g,dn,mn,gn) >= 0)
     if(iceb_u_sravmydat(dk,mk,gk,d,m,g) >= 0)
      {
         if(shett == 0)
          {
           if(iceb_u_sravmydat(1,m,g,1,mes,god) != 0)
            {
             if(mes != 0)
              {
               salorksl(dd,kk,ddn,kkn,debm,krem,ff1.ff,ffact.ff);
               debm=krem=0.;
              }
             mes=m;
             god=g;
            }
           shett=1;
          }

         debm+=deb;
         krem+=kre;

         fprintf(ff1.ff,"%02d.%02d.%4d %-*s %-3s%4s %-*s %12s",
         d,m,g,
         iceb_u_kolbait(6,row[3]),row[3],
         row[5],row[11],
         iceb_u_kolbait(11,row[6]),row[6],
         iceb_u_prnbr(deb));

         fprintf(ff1.ff," %12s %-*.*s\n",iceb_u_prnbr(kre),iceb_u_kolbait(32,row[13]),iceb_u_kolbait(32,row[13]),row[13]);

         koment.new_plus(row[6]);
         koment.plus(" ",row[13]);
         
         fprintf(ffact.ff,"%02d.%02d.%4d %-*.*s %10.2f %10.2f| |\n",
         d,m,g,
         iceb_u_kolbait(20,koment.ravno()),iceb_u_kolbait(20,koment.ravno()),koment.ravno(),
         deb,kre);

         //заряжаем массивы
         sprintf(strsql,"%d.%d.%d|%s",d,m,g,row[6]);
         if(row[6][0] == '\0')
           nomer=-1;
         else
           nomer=akt_sv.data_nomd.find(strsql);

         if(nomer < 0)
          {
           akt_sv.data_nomd.plus(strsql);
           akt_sv.koment.plus(koment.ravno());
          }
         akt_sv.suma_deb.plus(deb,nomer);
         akt_sv.suma_kre.plus(kre,nomer);
         
         fprintf(ffact.ff,"\
------------------------------------------------------ ---------------------------------------------------\n");

       dd+=deb;
       kk+=kre;

      }

     if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0)
      {
       ddn+=deb;
       kkn+=kre;
      }

    }
   iceb_u_dpm(&d,&m,&g,1);
  }


brr=dd+ddn;

gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family", "monospace","foreground", "red",  NULL);

sprintf(strsql,"%*s:%12s",iceb_u_kolbait(20,gettext("Оборот за период")),gettext("Оборот за период"),iceb_u_prnbr(dd));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql," %12s\n",iceb_u_prnbr(kk));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

if(dd+ ddn >= kk+kkn)
 {
  brr=(dd+ddn)-(kk+kkn);
  sprintf(strsql,"%*s:%12s\n",iceb_u_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное"),iceb_u_prnbr(brr));
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
 }
else
 {
  brr=(kk+kkn)-(dd+ddn);
  sprintf(strsql,"%*s:%12s %12s\n",
  iceb_u_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное")," ",iceb_u_prnbr(brr));
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
 }
salorksl(dd,kk,ddn,kkn,debm,krem,ff1.ff,ffact.ff);

if(data->rk->shets.ravno()[0] == '\0')
 {
  iceb_podpis(ff1.ff,data->window);
  ff1.end();
  iceb_spks_r_konact(ffact.ff);
  ffact.end(); 
 }
char imaf_act[64];
char imaf_act_bi[64];

sprintf(imaf_act,"asv%d.lst",getpid());
sprintf(imaf_act_bi,"asvbi%d.lst",getpid());

salork_akt_sv(&akt_sv,dn,mn,gn,dk,mk,gk,data->rk->kontr.ravno(),data->rk->naikontr.ravno(),imaf_act,imaf_act_bi,data->window);

double db=dd+ddn;
double kr=kk+kkn;

if(data->rk->shets.ravno()[0] != '\0' && mvp == 0)
 {
  shh.new_plus(data->rk->shets.ravno());
  mvp=1;
  goto naz;
 }

if(msv == 1)
 {
  fprintf(ff1.ff,"\
---------------------------------------------------------------------------------------\n\
%*s:%12s",
  iceb_u_kolbait(37,gettext("Общий итог")),gettext("Общий итог"),iceb_u_prnbr(db));
  fprintf(ff1.ff," %12s\n",iceb_u_prnbr(kr));

  if(db >= kr)
   {
    brr=db-kr;
    fprintf(ff1.ff,"\
%37s %12s\n"," ",iceb_u_prnbr(brr));
   }
  else
   {
    brr=kr-db;
    fprintf(ff1.ff,"\
%37s %12s %12s\n"," "," ",iceb_u_prnbr(brr));
   }
  iceb_podpis(ff1.ff,data->window);
  ff1.end();
  iceb_spks_r_konact(ffact.ff);
  ffact.end();
   
 }




/**************
data->oth->spis_imaf.new_plus(imaf);
data->oth->spis_naim.new_plus(gettext("Реестр кассовых ордеров"));

data->oth->spis_imaf.plus(imaf1);
data->oth->spis_naim.plus(gettext("Вкладной лист кассовой книги"));

data->oth->spis_imaf.plus(imafok);
data->oth->spis_naim.plus(gettext("Отчёт кассира"));

data->oth->spis_imaf.plus(imaf2);
data->oth->spis_naim.plus(gettext("Типовая форма КО-3а"));

data->oth->spis_imaf.plus(imaf_prot);
data->oth->spis_naim.plus(gettext("Протокол хода расчёта"));

for(int nomer=0; nomer < data->oth->spis_imaf.kolih(); nomer++)
 iceb_ustpeh(data->oth->spis_imaf.ravno(nomer),3,data->window);
*************/

data->oth->spis_imaf.plus(imaf);
data->oth->spis_naim.plus(gettext("Распечатка проводок по контрагенту"));
data->oth->spis_imaf.plus(imafact);
data->oth->spis_naim.plus(gettext("Акт сверки по контрагенту"));
data->oth->spis_imaf.plus(imaf_act);
data->oth->spis_naim.plus(gettext("Акт сверки с общими суммами по документу"));
data->oth->spis_imaf.plus(imaf_act_bi);
data->oth->spis_naim.plus(gettext("Акт сверки с общими суммами по документу без месячных итогов"));

for(int nomer=0; nomer < data->oth->spis_imaf.kolih(); nomer++)
 iceb_ustpeh(data->oth->spis_imaf.ravno(nomer),3,data->window);



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
