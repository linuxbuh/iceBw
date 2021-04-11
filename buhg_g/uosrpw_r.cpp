/*$Id: uosrpw_r.c,v 1.19 2013/09/26 09:46:57 sasa Exp $*/
/*19.01.2017	01.01.2008	Белых А.И.	uosrpw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "uosrpw.h"
#define         PL      52  /*Количество строк на первом листе*/
#define         VL      55  /*Количество строк на втором листе*/

class uosrpw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class uosrpw_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  uosrpw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosrpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosrpw_r_data *data);
gint uosrpw_r1(class uosrpw_r_data *data);
void  uosrpw_r_v_knopka(GtkWidget *widget,class uosrpw_r_data *data);

void	sapkasord(short dd,short md,short gd,int nomlist,FILE *ffva);
void   hgrkd(FILE *ff);

extern SQL_baza bd;




extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int uosrpw_r(class uosrpw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class uosrpw_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Реестр проводок по видам операций"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(uosrpw_r_key_press),&data);

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

repl.plus(gettext("Реестр проводок по видам операций"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(), iceb_get_pnk("00",0,wpredok));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(uosrpw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)uosrpw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosrpw_r_v_knopka(GtkWidget *widget,class uosrpw_r_data *data)
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

gboolean   uosrpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosrpw_r_data *data)
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

/*******/
/*Шапка*/
/*******/
void gsapr(short dn,short mn,short gn,short dk,short mk,
short gk,short *sl,short *kst,int skl1,const char *naiskl,FILE *ff)
{
*kst=*kst+1;


if((*kst >= VL && *sl > 1) || (*kst >= PL && *sl == 1) ||
(*kst == 0 && *sl == 0))
 {
  if((*kst >= VL && *sl > 1) || (*kst >= PL && *sl == 1))
      fprintf(ff,"\f");
  *sl=*sl+1;
  *kst=1;

  fprintf(ff,"\
%s N%-2d %-30s %s %02d.%02d.%4d%s %s %02d.%02d.%4d%s                               %s N%d\n",
gettext("Подразделение"),
skl1,naiskl,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
gettext("Лист"),
*sl);

  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff,gettext("\
	        Контрагент                                  |Дата док. |Дата пров.|N документа|  Счета   |       Сумма        |Комментарий\n"));
  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------------\n");
 }

}

/*******************************/
/*Распечатка итога по документа*/
/*******************************/
void rasid(double itd,short dn,short mn,short gn,short dk,
short mk,short gk,int skl1,const char *naiskl,short *sli,short *kst,FILE *ff)
{
 fprintf(ff,"%*s %20s\n",iceb_u_kolbait(105,gettext("Итого по документу:")),gettext("Итого по документу:"),iceb_u_prnbr(itd));
 gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
}

/*******************************/
/*Распечатка итога по организации*/
/*******************************/
void rasio(const char *orgn,double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,
const char *naikont, //Наименование контрагента
const char *naiskl,  //Наименование подразделения
short *sli,short *kst,
class iceb_u_spisok *mso,
class iceb_u_double *mspo,
FILE *ff)
{
short           i;
double          bb;

rasid(itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,sli,kst,ff);

/*Распечатываем результаты по организации*/
bb=0.;
if(mso->kolih() > 0)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%s \"%s\":\n",gettext("Итого по контрагенту"),naikont);
 }

for(i=0; i < mso->kolih() ;i++)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,mso->ravno(i)),mso->ravno(i),iceb_u_prnbr(mspo->ravno(i)));
  bb+=mspo->ravno(i);
 }
if(mso->kolih() > 0)
 {
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(bb));
  kst++;
  fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
 }
mso->free_class();
mspo->free_class();
}
/*******************************/
/*Распечатка итога по складу*/
/*******************************/
void rasis(const char *orgn,double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,const char *naikont,const char *naiskl,
short *sli,short *kst,
class iceb_u_spisok *mso,
class iceb_u_double *mspo,
class iceb_u_spisok *mss,
class iceb_u_double *msps,
FILE *ff)
{
short           i;
double          bb;

rasio(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naikont,naiskl,sli,kst,mso,mspo,ff);

/*Распечатываем результаты по складу*/
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"%s \"%s\":\n",gettext("Итого по подразделению"),naiskl);
bb=0.;
for(i=0; i< msps->kolih() ;i++)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,mss->ravno(i)),mss->ravno(i),iceb_u_prnbr(msps->ravno(i)));
  bb+=msps->ravno(i);
 }
fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(bb));

fprintf(ff,"\
...............................................................................................................................\n");
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
mss->free_class();
msps->free_class();
}

/*************************/
/*Распечатка общего итога*/
/*************************/
void            rasoi(class iceb_u_spisok *mssi,class iceb_u_double *mspsi,FILE *ff)
{
double          k1;
short           i;

k1=0.;
fprintf(ff,"\n%s:\n",gettext("Общий итог"));
for(i=0; i< mssi->kolih() ;i++)
 {
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,mssi->ravno(i)),mssi->ravno(i),iceb_u_prnbr(mspsi->ravno(i)));
  k1+=mspsi->ravno(i);
 }
fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));
mssi->free_class();
mspsi->free_class();
}
/****************************/
/*Печать реквизитов поиска*/
/****************************/
void uosrpw_r_prp(class uosrpw_r_data *data,FILE *ff)
{
if(data->rk->podr.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Подразделение"),data->rk->podr.ravno());
if(data->rk->kontr.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Код контрагента"),data->rk->kontr.ravno());

if(data->rk->prih_rash != 0)
 {
  fprintf(ff,"%s:",gettext("Документы"));
  if(data->rk->prih_rash == 1)
   fprintf(ff,"%s\n",gettext("Только приходные документы"));
  if(data->rk->prih_rash == 2)
   fprintf(ff,"%s\n",gettext("Только расходные документы"));
 }

if(data->rk->kod_op.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Код операции"),data->rk->kod_op.ravno());
if(data->rk->shetu.getdlinna() > 1)
 fprintf(ff,"%s: %s\n",gettext("Счёт"),data->rk->shetu.ravno());
}

/******************************************/
/******************************************/

gint uosrpw_r1(class uosrpw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;




class iceb_u_spisok mso;
class iceb_u_double mspo;
class iceb_u_spisok mss;  /*Массив счетов по складу*/
class iceb_u_double msps; /*Массив сумм по складу*/
class iceb_u_spisok mssi; /*Массив счетов по всем складам*/
class iceb_u_double mspsi; /*Массив сумм проводок по всем складам по операции*/
class iceb_u_spisok mssio; /*Массив счетов по всем складам и всем операциям*/
class iceb_u_double mspsio; /*Массив сумм проводок по всем складам по всем операциям*/

short           kst,sli;
FILE            *ff;
FILE            *ffitog;
class iceb_u_str kop("");
class iceb_u_str bros("");
class iceb_u_str naiskl("");
short           kom;
class iceb_u_str kome(""); /*Комментарий*/
short           dp,mp,gp;
short           dd,md,gd;
class iceb_u_str orgn("");
char            imaf[64],imafitog[64];
class iceb_u_str shett("");
double          itd; /*Итого по счёту*/
int             i;
double          k1;
short           tipzz;
int		tz; /*1-Приход 2-расход*/
short           klr;
short           dn,mn,gn;
short           dk,mk,gk;
double		deb;
int		skl=0,skl1=0;
class iceb_u_str nomdok("");
class iceb_u_str nomdokv("");
class iceb_u_str kprr(""),kprr1("");		
class iceb_u_str naimpr("");
long		kolstr=0;
float kolstr1=0.;
SQL_str         row1;
class iceb_u_str kontr("");
int		tipz;
class iceb_u_str naikont("");
SQLCURSOR curr;



tz=data->rk->prih_rash;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"select datp,sh,shk,nomd,pod,deb,datd,komen,kodkon,oper,tz from Prov \
where val=0 and \
datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and kto='%s' and deb != 0.",
gn,mn,dn,gk,mk,dk,ICEB_MP_UOS);


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

/*Сортировка через временную таблицу делается так как сортировать нужно по коду контрагента
а он может отсутствовать в проводке*/

class iceb_tmptab tabtmp;
const char *imatmptab={"uosrpw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
datp DATE not null,\
sh char(32) not null,\
shk char(32) not null,\
nomd char(32) not null,\
pod int not null,\
deb double(14,2) not null,\
datd DATE not null,\
komen char(255) not null,\
kodkon char(32) not null,\
oper char(32) not null,\
tz int not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  


sprintf(strsql,"%s: %d.%d.%d%s => %d.%d.%d%s\n",
gettext("Дата"),
dn,mn,gn,
gettext("г."),
dk,mk,gk,
gettext("г."));

iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s\n",gettext("Сортируем записи"));

iceb_printw(strsql,data->buffer,data->view);
class iceb_u_str koment("");
kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  /*Подразделение*/
  if(iceb_u_proverka(data->rk->podr.ravno(),row[5],0,0) != 0)
    continue;
   
  /*Читаем документ*/
  sprintf(strsql,"select tipz,kodop,kontr,nomdv \
from Uosdok where datd='%s' and nomd='%s'",
  row[6],row[3]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
   {
    sprintf(strsql,"%s N%s %s %s !",gettext("Не найден документ !"),
    row[3],gettext("Дата"),row[6]);
    iceb_menu_soob(strsql,data->window);
    continue;
   }  
  tipz=atoi(row1[0]);
  kprr.new_plus(row1[1]);
  kontr.new_plus(row1[2]);
  nomdok.new_plus(row[3]);
  nomdok.plus(" ",row1[3]);
  if(tz != 0 && tz != tipz)
   continue;

  /*Организация*/
  if(iceb_u_proverka(data->rk->kontr.ravno(),kontr.ravno(),0,0) != 0)
    continue;

  /*Операция*/
  if(iceb_u_proverka(data->rk->kod_op.ravno(),kprr.ravno(),0,0) != 0)
    continue;


  /*Счёт*/
  if(iceb_u_proverka(data->rk->shetu.ravno(),row[1],0,0) != 0)
    continue;
  koment.new_plus(row[7]);      
  sprintf(strsql,"insert into %s values ('%s','%s','%s','%s',%s,%s,'%s','%s','%s','%s',%s)",
  imatmptab,
  row[0],row[1],row[2],nomdok.ravno_filtr(),row[4],row[5],row[6],koment.ravno_filtr(),kontr.ravno(),kprr.ravno(),row[10]);

  iceb_sql_zapis(strsql,1,0,data->window);    

 }


sprintf(strsql,"select * from %s order by tz asc,oper asc,pod asc,kodkon asc,nomd asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(imaf,"rp%d%d.lst",mn,mk);

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


uosrpw_r_prp(data,ff); /*Печать реквизитов поиска*/


skl=0;
kontr.new_plus("");
nomdok.new_plus("");
nomdokv.new_plus("");

tipzz=kom=0;
klr=sli=0;
kst=-1;
itd=0.;

kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  iceb_u_rsdat(&dp,&mp,&gp,row[0],2);

  shett.new_plus(row[1]);
  shett.plus(",",row[2]);
  nomdok.new_plus(row[3]);
  skl1=atoi(row[4]);
  deb=atof(row[5]);
  iceb_u_rsdat(&dd,&md,&gd,row[6],2);
  kome.new_plus(row[7]);
  
  iceb_u_rsdat(&dd,&md,&gd,row[6],2);
  
  orgn.new_plus(row[8]);

  kprr.new_plus(row[9]);
  tz=atoi(row[10]);
  if(iceb_u_SRAV(kprr.ravno(),kprr1.ravno(),0) != 0 || tipzz != tz)
   {
    if(kprr1.ravno()[0] != '\0' )
     {
      rasis(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl1,naikont.ravno(),naiskl.ravno(),&sli,&kst,&mso,&mspo,&mss,&msps,ff);
      itd=0.;
      rasoi(&mssi,&mspsi,ff);


      fprintf(ff,"\n\n\n\f");
     }
    skl=0;
    nomdokv.new_plus("");
    kontr.new_plus("");

    /*Читаем наименование операции*/
    if(tz == 1)
     sprintf(strsql,"select naik from Uospri where kod='%s'",kprr.ravno());
    if(tz == 2)
     sprintf(strsql,"select naik from Uosras where kod='%s'",kprr.ravno());

    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      sprintf(strsql,"%s %s!\n",gettext("Не нашли код операции"),kprr.ravno());
      iceb_menu_soob(strsql,data->window);

      fprintf(ff,"%s %s!\n",gettext("Не нашли код операции"),kprr.ravno());
      naimpr.new_plus("");
     }
    else
     naimpr.new_plus(row1[0]);
     
    kprr1.new_plus(kprr.ravno());
    tipzz=tz;

    sprintf(strsql,"%d/%s %s\n",tz,kprr.ravno(),naimpr.ravno());
    iceb_printw(strsql,data->buffer,data->view);


    /*Читаем наименование подразделения*/
    sprintf(strsql,"select naik from Uospod where kod=%d",skl1);

    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      sprintf(strsql,"%s %d !\n",gettext("Не найден код подразделения"),skl1);
      iceb_menu_soob(strsql,data->window);
      fprintf(ff,"%s %d !\n",gettext("Не найден код подразделения"),skl1);
      naiskl.new_plus("");
     }
    else
     naiskl.new_plus(row1[0]);

    fprintf(ff,"%s\n",gettext("Учёт основных средств"));
    iceb_u_zagolov(gettext("Реестр проводок по видам операций"),dn,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ff);

    kst=-1;
    klr+=sli;
    sli=0;
    gsapr(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);

   }

  if(skl != skl1)
   {
    if(skl != 0)
     {
      rasis(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl1,naikont.ravno(),naiskl.ravno(),&sli,&kst,&mso,&mspo,&mss,&msps,ff);
      itd=0.;
      kst=-1;
    klr+=sli;
      sli=0;
      fprintf(ff,"\f");
     }
    nomdokv.new_plus("");
    kontr.new_plus("");
    kom=1;


    /*Читаем наименование подразделения*/
    sprintf(strsql,"select naik from Uospod where kod=%d",skl1);

    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %d !\n",gettext("Не найден код подразделения"),skl1);
      iceb_menu_soob(strsql,data->window);
      fprintf(ff,"%s %d !\n",gettext("Не найден код подразделения"),skl1);
      naiskl.new_plus("");
     }
    else
     naiskl.new_plus(row1[0]);

    sprintf(strsql,"%s %d %s\n",gettext("Подразделение"),skl1,naiskl.ravno());
    iceb_printw(strsql,data->buffer,data->view);

    skl=skl1;
    gsapr(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);
   }

  if(iceb_u_SRAV(kontr.ravno(),orgn.ravno(),0) != 0)
   {
    if(kontr.ravno()[0] != '\0')
     {
      rasio(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl1,naikont.ravno(),naiskl.ravno(),&sli,&kst,&mso,&mspo,ff);
      itd=0.;
     }
    nomdokv.new_plus("");
    kom=1;

    if(iceb_u_atof(orgn.ravno()) == 0. && iceb_u_polen(orgn.ravno(),&bros,1,'-') == 0 && orgn.ravno()[0] == '0')
     kop.new_plus(bros.ravno());
    else
     kop.new_plus(orgn.ravno());


    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kop.ravno());

    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %s !\n",gettext("Не нашли код контрагента"),kop.ravno());
      iceb_menu_soob(strsql,data->window);
      naikont.new_plus("");
     }
    else
     naikont.new_plus(row1[0]);

    sprintf(strsql,"%s:%s/%s\n",gettext("Контрагент"),orgn.ravno(),naikont.ravno());
    iceb_printw(strsql,data->buffer,data->view);

    kontr.new_plus(orgn.ravno());
   }

  if(iceb_u_SRAV(nomdok.ravno(),nomdokv.ravno(),0) != 0)
   {
      if(nomdokv.getdlinna() > 1)
       {
	rasid(itd,dn,mn,gn,dk,mk,gk,skl1,naiskl.ravno(),&sli,&kst,ff);
	itd=0.;
       }
      nomdokv.new_plus(nomdok.ravno());
   }



     /*Записываем в массив организаций*/
   if((i=mso.find(shett.ravno())) < 0)
    mso.plus(shett.ravno());
   mspo.plus(deb,i);

   /*Записываем в массив по складу*/
   if((i=mss.find(shett.ravno())) < 0)
    mss.plus(shett.ravno());
   msps.plus(deb,i);

   /*Записываем в массив по складам */
   if((i=mssi.find(shett.ravno())) < 0)
    mssi.plus(shett.ravno());
    
   mspsi.plus(deb,i);

   /*Записываем в массив по складам и операциям*/
   if((i=mssio.find(shett.ravno())) < 0)
    mssio.plus(shett.ravno());
   mspsio.plus(deb,i);

   gsapr(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);

   sprintf(strsql,"%02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %15.2f\n",
   dp,mp,gp,dd,md,gd,
   iceb_u_kolbait(11,nomdok.ravno()),nomdok.ravno(),
   iceb_u_kolbait(10,shett.ravno()),shett.ravno(),
   deb);

   iceb_printw(strsql,data->buffer,data->view);

   if(kom == 1)
     fprintf(ff,"\
%-*s %-*.*s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %20s\n",
     iceb_u_kolbait(7,orgn.ravno()),orgn.ravno(),
     iceb_u_kolbait(52,naikont.ravno()),
     iceb_u_kolbait(52,naikont.ravno()),
     naikont.ravno(),
     dd,md,gd,dp,mp,gp,
     iceb_u_kolbait(11,nomdok.ravno()),nomdok.ravno(),
     iceb_u_kolbait(10,shett.ravno()),shett.ravno(),
     iceb_u_prnbr(deb));

   if(kom == 0)
    fprintf(ff,"\
%60s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %20s %s\n",
    " ",dd,md,gd,dp,mp,gp,
    iceb_u_kolbait(11,nomdok.ravno()),nomdok.ravno(),
    iceb_u_kolbait(10,shett.ravno()),shett.ravno(),
    iceb_u_prnbr(deb),kome.ravno());

   itd+=deb;
   kom=0;

 }
rasis(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl1,naikont.ravno(),naiskl.ravno(),&sli,&kst,&mso,&mspo,&mss,&msps,ff);
rasoi(&mssi,&mspsi,ff);

if(data->rk->kod_op.getdlinna() <= 1 || ( data->rk->kod_op.getdlinna() > 1 && iceb_u_pole(data->rk->kod_op.ravno(),&shett,2,',') == 0))
 {
  k1=0.;
  fprintf(ff,"\n==================================================\n\
%s:\n",
  gettext("Общий итог по всем операциям"));
  for(i=0; i< mssio.kolih() ;i++)
   {
    fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,mssio.ravno(i)),mssio.ravno(i),iceb_u_prnbr(mspsio.ravno(i)));
    k1+=mspsio.ravno(i);
   }
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));
 }




iceb_podpis(ff,data->window);
fclose(ff);

sprintf(imafitog,"rpit%d.lst",getpid());

if((ffitog = fopen(imafitog,"w")) == NULL)
 {
  iceb_er_op_fil(imafitog,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

fprintf(ffitog,"%s\n",gettext("Учёт основных средств"));
iceb_u_zagolov(gettext("Реестр проводок по видам операций"),dn,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ffitog);

uosrpw_r_prp(data,ffitog); /*Печать реквизитов поиска*/



k1=0.;
fprintf(ffitog,"\n==================================================\n\
%s:\n",
gettext("Общий итог по всем операциям"));
for(i=0; i< mssio.kolih() ;i++)
 {
  fprintf(ffitog,"%-*s - %20s\n",iceb_u_kolbait(10,mssio.ravno(i)),mssio.ravno(i),iceb_u_prnbr(mspsio.ravno(i)));
  k1+=mspsio.ravno(i);
 }
fprintf(ffitog,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));

iceb_podpis(ffitog,data->window);
fclose(ffitog);


klr+=sli;

sprintf(strsql,"%s: %d\n",gettext("Количество листов"),klr);
iceb_printw(strsql,data->buffer,data->view);



data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Реестр проводок по видам операций"));
data->rk->imaf.plus(imafitog);
data->rk->naim.plus(gettext("Общий итог"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),1,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
