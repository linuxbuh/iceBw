/*$Id: glkniw_r.c,v 1.34 2014/08/31 06:19:19 sasa Exp $*/
/*23.05.2016	03.03.2009	Белых А.И.	glkniw_r.c
Расчёт отчёта 
*/
#include <math.h>
#include <errno.h>
#include "buhg_g.h"
#include "glkni.h"
#include "glktmpw.h"

class glkniw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class glkni_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  glkniw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   glkniw_r_key_press(GtkWidget *widget,GdkEventKey *event,class glkniw_r_data *data);
gint glkniw_r1(class glkniw_r_data *data);
void  glkniw_r_v_knopka(GtkWidget *widget,class glkniw_r_data *data);

void  balans_f1w(int metkabd,short dk,short mk,short gk,char *imaf,class GLKTMP *prom,GtkWidget *wpredok);
void  balans_f2w(int metkabd,short dk,short mk,short gk,char *imaf,class GLKTMP *prom, GtkWidget *wpredok);

extern int kol_strok_na_liste_u;//количество строк на листе с уменьшенным межстрочным расстоянием
extern int kol_strok_na_liste_l_u; //Количесво строк на стандартном листе в ориентации ландшафт с уменьшенным межстрочным расстоянием

extern SQL_baza bd;
extern short	startgodb; /*Стартовый год*/
extern char     *sbshet; //Список бюджетных счетов
extern short	koolk; /*Корректор отступа от левого края*/

int glkniw_r(class glkni_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class glkniw_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт главной книги"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(glkniw_r_key_press),&data);

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

repl.plus(gettext("Расчёт главной книги"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,wpredok));
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста
/***********
PangoFontDescription *font_pango=pango_font_description_from_string(shrift_ravnohir.ravno());
gtk_widget_override_font(GTK_WIDGET(data.view),font_pango);
pango_font_description_free(font_pango);
*****************/
//gtk_text_view_set_monospace (data.view,TRUE); /*TRUE- моноширинный текст с версии 3.16*/

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));


GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
//gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_ALWAYS,GTK_POLICY_ALWAYS);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/


gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(glkniw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

//g_idle_add((GSourceFunc)glkniw_r1,&data);
g_idle_add((GSourceFunc)glkniw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  glkniw_r_v_knopka(GtkWidget *widget,class glkniw_r_data *data)
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

gboolean   glkniw_r_key_press(GtkWidget *widget,GdkEventKey *event,class glkniw_r_data *data)
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


/**************************/
/*Шапка оборотного баланса*/
/****************************/

void gsao1_s(int *sl,int *kst,struct tm *bf,FILE *ff1)
{
 fprintf(ff1,"\
%s %d.%d.%d  %s - %d:%02d",
 gettext("По состоянию на"),
 bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
 gettext("Время"),
 bf->tm_hour,bf->tm_min);
 *sl+=1;
 fprintf(ff1,"%*s N%d\n",iceb_u_kolbait(55,gettext("Лист")),gettext("Лист"),*sl);

 fprintf(ff1,"\
-------------------------------------------------------------------------------------------------------------\n");

fprintf(ff1,gettext("             |          C а л ь д о          |         О б о р о т           |         С а л ь д о          |\n"));
fprintf(ff1,gettext("    Счёт     ------------------------------------------------------------------------------------------------\n"));
fprintf(ff1,gettext("             |    Дебет      |    Кредит     |    Дебет      |     Кредит    |    Дебет      |    Кредит    |\n"));

  fprintf(ff1,"\
-------------------------------------------------------------------------------------------------------------\n");

*kst+=6;
}
/***********************/
/*Шапка главной книги портретной ориентации*/
/**********************/
void  gsap_s(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,int *kst,struct tm *bf,FILE *ff)
{

  fprintf(ff,"\
%s %d.%d.%d  %s - %d:%d",
  gettext("По состоянию на"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("Время"),
  bf->tm_hour,bf->tm_min);
  *sl+=1;
  fprintf(ff,"%*s N%d\n",iceb_u_kolbait(70,gettext("Лист")),gettext("Лист"),*sl);

  fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff,gettext("        |      Наименование       |   Сальдо до %02d.%02d.%4dг.      |Счёт |     Оборот за период          |   Сальдо на %02d.%02d.%4dг.     |\n"),
  dn,mn,gn,dk,mk,gk);
  fprintf(ff,gettext("  Счёт  |         счета           --------------------------------|коре-|---------------------------------------------------------------\n"));
  fprintf(ff,gettext("        |                         |    Дебет      |    Кредит     |спон.|    Дебет      |     Кредит    |    Дебет      |    Кредит    |\n"));

  fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------------\n");
*kst+=6;
}

/***********************/
/*Шапка главной книги ландшафтной ориентации*/
/**********************/
void  gklni_gk_slo(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,int *kst,struct tm *bf,FILE *ff)
{

  fprintf(ff,"\
%s %d.%d.%d  %s - %d:%d",
  gettext("По состоянию на"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("Время"),
  bf->tm_hour,bf->tm_min);
  *sl+=1;
  fprintf(ff,"%*s N%d\n",iceb_u_kolbait(70,gettext("Лист")),gettext("Лист"),*sl);

  fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff,gettext("        |      Наименование       |   Сальдо до %02d.%02d.%4dг.  |Счёт |     Оборот за период      |   Сальдо на %02d.%02d.%4dг. |\n"),  dn,mn,gn,dk,mk,gk);
  fprintf(ff,gettext("  Счёт  |         счета           ----------------------------|коре-|-------------------------------------------------------\n"));
  fprintf(ff,gettext("        |                         |    Дебет    |    Кредит   |спон.|    Дебет    |     Кредит  |    Дебет    |    Кредит  |\n"));

  fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");
*kst+=6;
}
/***********************/
/*Счётчик главной книги для портретной ориентации*/
/**********************/
void shet_gk_p(short dn,short mn,short gn,short dk,short mk,
short gk,
int *sl,int *kst,
struct tm *bf,
FILE *ff)
{
*kst+=1;
if(*kst > kol_strok_na_liste_u)
 {
  fprintf(ff,"\f");
  *kst=0;
  gsap_s(dn,mn,gn,dk,mk,gk,sl,kst,bf,ff);
  *kst+=1;
 }

}
/***********************/
/*Счётчик главной книги для ландшафтной ориентации*/
/**********************/
void            shet_gk_l(short dn,short mn,short gn,short dk,short mk,
short gk,
int *sl,int *kst,
struct tm *bf,
FILE *ff)
{
*kst+=1;
if(*kst > kol_strok_na_liste_l_u)
 {
  fprintf(ff,"\f");
  *kst=1;
  gklni_gk_slo(dn,mn,gn,dk,mk,gk,sl,kst,bf,ff);
 }

}
/****************************/
/*счётчик оборотного баланса для портретной ориентации*/
/***************************/
void            shet_ob_p(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,int *kst,
struct tm *bf,
FILE *ff1)
{
*kst+=1;

if(*kst > kol_strok_na_liste_u)
 {
  fprintf(ff1,"\f");

  *kst=0;
  gsao1_s(sl,kst,bf,ff1);
  *kst+=1;

 }

}

/****************************/
/*счётчик оборотного баланса для ландшафтной ориентации*/
/***************************/
void            shet_ob_l(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,int *kst,
struct tm *bf,
FILE *ff1)
{
*kst+=1;

if(*kst > kol_strok_na_liste_l_u)
 {
  fprintf(ff1,"\f");

  *kst=0;
  gsao1_s(sl,kst,bf,ff1);
  *kst+=1;

 }

}

/***************************/
/*Распечатка обычных счетов*/
/***************************/

void ras1(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sli,int *kst,
int *sli1,int *kst1,
short i,
const char *ns, //Номер счета
const char *nash, //Наіменование
class GLKTMP *PROM,
double *dns,
double *kns,
double *dpp,
double *kpp,
double *di,
double *ki,
struct tm *bf,
int *kollist_gk_l,
int *kolstr_gk_l,
int *kollist_ob_l,
int *kolstr_ob_l,
int metka_naim_shet,
FILE *ff,
FILE *ff1,
FILE *ff_gk_l,
FILE *ff_ob_l,
FILE *ffcsv,
GtkWidget *wpredok)
{
short           i1;
class iceb_u_str shk("");
short		mos; /*Метка оборотов по счетам*/
double          sd,sk,dpe,kpe;
/*
printw("%d %s %s\n",i,ns,nash);
OSTANOV();
*/
mos=0;
sd=sk=0.;
dpe=kpe=0.;
int ks=PROM->masf.kolih();
for(i1=0;i1< ks ;i1++)
 {
  sd+=PROM->sl.ravno(i*ks+i1);
  sk+=PROM->sl.ravno(i+ks*i1);

  dpe+=PROM->pe.ravno(i*ks+i1);
  kpe+=PROM->pe.ravno(i+ks*i1);
  if(fabs(PROM->pe.ravno(i*ks+i1)) > 0.009 || fabs(PROM->pe.ravno(i+ks*i1)) > 0.009)
    mos=1;
 }

if(mos == 0)
 {
  if(fabs(sd) < 0.009 && fabs(sk) < 0.009 && fabs(dpe) < 0.009 && fabs(kpe) < 0.009
  && fabs(PROM->ssd.ravno(i)) < 0.009 && fabs(PROM->ssk.ravno(i)) < 0.009)
    return;

  if(fabs(sd+PROM->ssd.ravno(i)-sk-PROM->ssk.ravno(i)) < 0.009 && fabs(dpe) < 0.009 && fabs(kpe) < 0.009)
   return;
 }

*dpp+=dpe;
*kpp+=kpe;
/*
if(SRAV(ns,"02/1",0) == 0)
  printw("%s dpp=%f kpp=%f sd=%f sk=%f\n",ns,dpp,kpp,sd,sk);
*/
shet_gk_p(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);
shet_ob_p(dn,mn,gn,dk,mk,gk,sli1,kst1,bf,ff1);

shet_gk_l(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
shet_ob_l(dn,mn,gn,dk,mk,gk,kollist_ob_l,kolstr_ob_l,bf,ff_ob_l);

double          kii,dii;
int kolbait=iceb_u_kolbait(25,nash);
int kolbns8=iceb_u_kolbait(8,ns);
int kolbns13=iceb_u_kolbait(13,ns);
class iceb_u_str suma_char[4];
suma_char[1].new_plus(dpe,2);
suma_char[2].new_plus(kpe,2);

if(PROM->ssd.ravno(i)+sd + dpe >= PROM->ssk.ravno(i)+sk+kpe)
 {
  dii=(PROM->ssd.ravno(i)+sd + dpe)-(PROM->ssk.ravno(i)+sk+kpe);
  suma_char[3].new_plus(dii,2);
  
  *di+=dii;
  PROM->deb_end.plus(dii,i);
  if(PROM->ssd.ravno(i)+sd >= PROM->ssk.ravno(i)+sk)
   {
    kii=(PROM->ssd.ravno(i)+sd)-(PROM->ssk.ravno(i)+sk);
    suma_char[0].new_plus(kii,2);
    *dns+=kii;

    fprintf(ff,"%-*s %-*.*s %15.2f %15s %-5s %15.2f %15.2f %15.2f\n",
    kolbns8,ns,kolbait,kolbait,nash,kii," "," ",dpe,kpe,dii);

    fprintf(ffcsv,"%s|%s|%s|%s|%s|%s|%s|%s|\n",
    ns,nash,suma_char[0].ravno_dtczp()," "," ",suma_char[1].ravno_dtczp(),suma_char[2].ravno_dtczp(),suma_char[3].ravno_dtczp());

    fprintf(ff_gk_l,"%-*s %-*.*s %13.2f %13s %-5s %13.2f %13.2f %13.2f\n",
    kolbns8,ns,kolbait,kolbait,nash,kii," "," ",dpe,kpe,dii);

    fprintf(ff1,"%-*.*s %15.2f %15s %15.2f %15.2f %15.2f\n",
    kolbns13,kolbns13,ns,kii," ",dpe,kpe,dii);


    fprintf(ff_ob_l,"%-*.*s %15.2f %15s %15.2f %15.2f %15.2f\n",
    kolbns13,kolbns13,ns,kii," ",dpe,kpe,dii);

   }
  else
   {
    kii=(PROM->ssk.ravno(i)+sk)-(PROM->ssd.ravno(i)+sd);
    suma_char[0].new_plus(kii,2);
    *kns+=kii;

    fprintf(ff,"%-*s %-*.*s %15s %15.2f %-5s %15.2f %15.2f %15.2f\n",
    kolbns8,ns,kolbait,kolbait,nash," ",kii," ",dpe,kpe,dii);

    fprintf(ffcsv,"%s|%s|%s|%s|%s|%s|%s|%s|\n",
    ns,nash," ",suma_char[0].ravno_dtczp()," ",suma_char[1].ravno_dtczp(),suma_char[2].ravno_dtczp(),suma_char[3].ravno_dtczp());

    fprintf(ff_gk_l,"%-*s %-*.*s %13s %13.2f %-5s %13.2f %13.2f %13.2f\n",
    kolbns8,ns,kolbait,kolbait,nash," ",kii," ",dpe,kpe,dii);

    fprintf(ff1,"%-*.*s %15s %15.2f %15.2f %15.2f %15.2f\n",
    kolbns13,kolbns13,ns," ",kii,dpe,kpe,dii);

    fprintf(ff_ob_l,"%-*.*s %15s %15.2f %15.2f %15.2f %15.2f\n",
    kolbns13,kolbns13,ns," ",kii,dpe,kpe,dii);
   }
 }
else
 {
  dii=(PROM->ssk.ravno(i)+sk+kpe)-(PROM->ssd.ravno(i)+sd + dpe);
  suma_char[3].new_plus(dii,2);
  *ki+=dii;
  PROM->kre_end.plus(dii,i);
  if(PROM->ssd.ravno(i)+sd >= PROM->ssk.ravno(i)+sk)
   {
    kii=(PROM->ssd.ravno(i)+sd)-(PROM->ssk.ravno(i)+sk);
    suma_char[0].new_plus(kii,2);
    *dns+=kii;

    fprintf(ff,"%-*s %-*.*s %15.2f %15s %-5s %15.2f %15.2f %15s %15.2f\n",
    kolbns8,ns,kolbait,kolbait,nash,kii," "," ",dpe,kpe," ",dii);

    fprintf(ffcsv,"%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
    ns,nash,suma_char[0].ravno_dtczp()," "," ",suma_char[1].ravno_dtczp(),suma_char[2].ravno_dtczp()," ",suma_char[3].ravno_dtczp());

    fprintf(ff_gk_l,"%-*s %-*.*s %13.2f %13s %-5s %13.2f %13.2f %13s %13.2f\n",
    kolbns8,ns,kolbait,kolbait,nash,kii," "," ",dpe,kpe," ",dii);

    fprintf(ff1,"%-*.*s %15.2f %15s %15.2f %15.2f %15s %15.2f\n",
    kolbns13,kolbns13,ns,kii," ",dpe,kpe," ",dii);

   
    fprintf(ff_ob_l,"%-*.*s %15.2f %15s %15.2f %15.2f %15s %15.2f\n",
    kolbns13,kolbns13,ns,kii," ",dpe,kpe," ",dii);
   }
  else
   {
    kii=(PROM->ssk.ravno(i)+sk)-(PROM->ssd.ravno(i)+sd);
    suma_char[0].new_plus(kii,2);
    *kns+=kii;

    fprintf(ff,"%-*s %-*.*s %15s %15.2f %-5s %15.2f %15.2f %15s %15.2f\n",
    kolbns8,ns,kolbait,kolbait,nash," ",kii," ",dpe,kpe," ",dii);

    fprintf(ffcsv,"%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
    ns,nash," ",suma_char[0].ravno_dtczp()," ",suma_char[1].ravno_dtczp(),suma_char[2].ravno_dtczp()," ",suma_char[3].ravno_dtczp());

    fprintf(ff_gk_l,"%-*s %-*.*s %13s %13.2f %-5s %13.2f %13.2f %13s %13.2f\n",
    kolbns8,ns,kolbait,kolbait,nash," ",kii," ",dpe,kpe," ",dii);

    fprintf(ff1,"%-*.*s %15s %15.2f %15.2f %15.2f %15s %15.2f\n",
    kolbns13,kolbns13,ns," ",kii,dpe,kpe," ",dii);


    fprintf(ff_ob_l,"%-*.*s %15s %15.2f %15.2f %15.2f %15s %15.2f\n",
    kolbns13,kolbns13,ns," ",kii,dpe,kpe," ",dii);

   }
 }

if(iceb_u_strlen(nash) > 25)
 {
  shet_gk_p(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);
  fprintf(ff,"%-8s %s\n"," ",iceb_u_adrsimv(25,nash));

  shet_gk_l(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
  fprintf(ff_gk_l,"%-8s %s\n"," ",iceb_u_adrsimv(25,nash));
 }

char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
//class iceb_u_str str_shet("");

class iceb_u_str naim_shet("");
/*Распечатываем по счетам*/
for(i1=0; i1<ks ; i1++)
 {

  shk.new_plus(PROM->masf.ravno(i1));
     
  if(PROM->pe.ravno(i*ks+i1) != 0. || PROM->pe.ravno(i+ks*i1) != 0.)
   {
    naim_shet.new_plus("");
    if(metka_naim_shet == 1)
     {
      sprintf(strsql,"select nais from Plansh where ns='%s'",shk.ravno());
      if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
       naim_shet.new_plus(row[0]);
      
//      str_shet.new_plus(naim_shet.ravno());
//      str_shet.plus(" ",shk.ravno());
      int razmer_naim=71-iceb_u_strlen(shk.ravno());

      sprintf(strsql,"%.*s %s",
      iceb_u_kolbait(razmer_naim,naim_shet.ravno()),
      naim_shet.ravno(),
      shk.ravno());
     }
    else
     strcpy(strsql,shk.ravno());
//      str_shet.new_plus(shk.ravno());
     
    shet_gk_p(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);

    kolbait=iceb_u_kolbait(72,strsql);

    fprintf(ff,"%*.*s %15.2f %15.2f\n",kolbait,kolbait,strsql,PROM->pe.ravno(i*ks+i1),PROM->pe.ravno(i+ks*i1));

    suma_char[0].new_plus(PROM->pe.ravno(i*ks+i1));
    suma_char[1].new_plus(PROM->pe.ravno(i+ks*i1));

    fprintf(ffcsv,"|%s|||%s|%s|%s|||||\n",naim_shet.ravno(),shk.ravno(),suma_char[0].ravno_dtczp(),suma_char[1].ravno_dtczp());

    shet_gk_l(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);

    kolbait=iceb_u_kolbait(68,strsql);
    fprintf(ff_gk_l,"%*.*s %13.2f %13.2f\n",kolbait,kolbait,strsql,PROM->pe.ravno(i*ks+i1),PROM->pe.ravno(i+ks*i1));

   }
 }

shet_gk_p(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);
fprintf(ff,"\
........................................................................................................................................\n");

shet_gk_l(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
fprintf(ff_gk_l,"\
............................................................................................................................\n");

}
/***********************************************/
/* запись в промежуточную таблицу*/
/********************************************/
int glkni_zvt(int per,const char *shet,const char *kod_kontr,double deb,double kre,const char *imatabl,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;



sprintf(strsql,"select debs,kres,debp,krep from %s where shet='%s' and kod_kontr='%s'",imatabl,shet,kod_kontr);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
 {
  if(per == 0)
   sprintf(strsql,"insert into %s values ('%s','%s',%.2f,%.2f,0.,0.)",imatabl,shet,kod_kontr,deb,kre);
  if(per == 1)
   sprintf(strsql,"insert into %s values ('%s','%s',0.,0.,%.2f,%.2f)",imatabl,shet,kod_kontr,deb,kre);
 }
else
 {

  double debz=0.;
  double krez=0.;

  if(per == 0)
   {
  
    debz=deb+atof(row[0]);
    krez=kre+atof(row[1]);

    sprintf(strsql,"update %s \
set \
debs=%.2f,\
kres=%.2f \
where shet='%s'and kod_kontr='%s'",imatabl,debz,krez,shet,kod_kontr);
   }

  if(per == 1)
   {
    debz=deb+atof(row[2]);
    krez=kre+atof(row[3]);
   sprintf(strsql,"update %s \
set \
debp=%.2f,\
krep=%.2f \
where shet='%s'and kod_kontr='%s'",imatabl,debz,krez,shet,kod_kontr);
   }
 }

if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return(1);

return(0);
}
/******************************************/
/*Работаем со счетами с развернутым сальдо сформированных с таблицей*/
/******************************************/

void ras2n_tab(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sli,int *kst,
int *sli1,int *kst1,
int nomer_sheta, //Номер счета в массиве счетов
const char *sh,
const char *nash,  //Наименование счета
GLKTMP *PROM,
double *dns,
double *kns,
double *dpp,
double *kpp,
double *di,
double *ki,
struct tm *bf,
int *kollist_gk_l,
int *kolstr_gk_l,
int *kollist_ob_l,
int *kolstr_ob_l,
int metka_naim_shet,
const char *imatabl,
FILE *ff,
FILE *ff1,
FILE *ff_gk_l,
FILE *ff_ob_l,
FILE *ffcsv,
GtkWidget *wpredok)
{
double          d1=0.,k1=0.;
double          d3=0.,k3=0.;
int         i1;
short		mos=0; /*Метка оборотов по счетам*/
SQL_str         row;
char		strsql[1024];
double		debs=0.,kres=0.;
double		debp=0.,krep=0.;
double          sd=0.,sk=0.,dpe=0.,kpe=0.;
SQLCURSOR cur;


sprintf(strsql,"select debs,kres,debp,krep from %s where shet='%s'",imatabl,sh);

if(cur.make_cursor(&bd,strsql) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {

  debs=atof(row[0]);
  kres=atof(row[1]);
  debp=atof(row[2]);
  krep=atof(row[3]);

  if(debs > kres)
   sd+=debs-kres;
  else
   sk+=kres-debs;

  if(debs+debp > kres+krep)
   d1+=debs+debp-kres-krep;  
  else
   k1+=kres+krep-debs-debp;

  dpe+=debp;
  kpe+=krep;
 }
//printw("sd=%f sk=%f\n",sd,sk);
//OSTANOV();
*dns+=sd;
*kns+=sk;

*dpp+=dpe;
*kpp+=kpe;
/*printw("dpp=%f kpp=%d\n",dpp,kpp);*/

*di+=d1; 
*ki+=k1;
PROM->kre_end.plus(k1,nomer_sheta);
PROM->deb_end.plus(d1,nomer_sheta);

/*Проверяем были ли обороты за период*/

d3=k3=0.;
mos=0;
int ks=PROM->masf.kolih();
for(i1=0; i1<ks;i1++)
 {
  d3+=PROM->pe.ravno(i1+ks*nomer_sheta);
  k3+=PROM->pe.ravno(i1*ks+nomer_sheta);
  if(fabs(PROM->pe.ravno(i1+ks*nomer_sheta)) > 0.009 || fabs(PROM->pe.ravno(i1*ks+nomer_sheta)) > 0.009)
    mos=1;
 }

if(mos == 0)
if(fabs(sd) < 0.009 && fabs(sk) < 0.009 && fabs(dpe) < 0.009
&& fabs(kpe) < 0.009 && fabs(d1) < 0.009 && fabs(k1) < 0.009
&& fabs(d3) < 0.009 && fabs(k3) < 0.009)
  return;

shet_gk_p(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);
shet_ob_p(dn,mn,gn,dk,mk,gk,sli1,kst1,bf,ff1);

shet_gk_l(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
shet_ob_l(dn,mn,gn,dk,mk,gk,kollist_ob_l,kolstr_ob_l,bf,ff_ob_l);

fprintf(ff,"%-*s %-*.*s %15.2f %15.2f %-5s %15.2f %15.2f %15.2f %15.2f\n",
iceb_u_kolbait(8,sh),sh,
iceb_u_kolbait(25,nash),iceb_u_kolbait(25,nash),nash,
sd,sk," ",dpe,kpe,d1,k1);

class iceb_u_str suma_char[6];
suma_char[0].new_plus(sd,2);
suma_char[1].new_plus(sk,2);
suma_char[2].new_plus(dpe,2);
suma_char[3].new_plus(kpe,2);
suma_char[4].new_plus(d1,2);
suma_char[5].new_plus(k1,2);

fprintf(ffcsv,"%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
sh,
nash,
suma_char[0].ravno_dtczp(),suma_char[1].ravno_dtczp()," ",suma_char[2].ravno_dtczp(),suma_char[3].ravno_dtczp(),suma_char[4].ravno_dtczp(),suma_char[5].ravno_dtczp());

fprintf(ff_gk_l,"%-*s %-*.*s %13.2f %13.2f %-5s %13.2f %13.2f %13.2f %13.2f\n",
iceb_u_kolbait(8,sh),sh,
iceb_u_kolbait(25,nash),iceb_u_kolbait(25,nash),nash,
sd,sk," ",dpe,kpe,d1,k1);

if(iceb_u_strlen(nash) > 25)
 {
  shet_gk_p(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);
  fprintf(ff,"%-8s %s\n"," ",iceb_u_adrsimv(25,nash));
  
  shet_gk_l(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
  fprintf(ff_gk_l,"%-8s %s\n"," ",iceb_u_adrsimv(25,nash));
 }

fprintf(ff1,"%-*s %15.2f %15.2f %15.2f %15.2f %15.2f %15.2f\n",
iceb_u_kolbait(13,sh),sh, sd, sk, dpe, kpe , d1 , k1);

fprintf(ff_ob_l,"%-*s %15.2f %15.2f %15.2f %15.2f %15.2f %15.2f\n",
iceb_u_kolbait(13,sh),sh, sd, sk, dpe, kpe , d1 , k1);

//class iceb_u_str str_shet("");

class iceb_u_str naim_shet("");

/*Распечатываем по счетам*/
for(i1=0; i1<ks;i1++)
 {

  d3=PROM->pe.ravno(i1+ks*nomer_sheta);
  k3=PROM->pe.ravno(i1*ks+nomer_sheta);
  if(d3 != 0. || k3 != 0.)
   {
    naim_shet.new_plus("");
    if(metka_naim_shet == 1)
     {
  
      sprintf(strsql,"select nais from Plansh where ns='%s'",PROM->masf.ravno(i1));
      if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
       naim_shet.new_plus(row[0]);

      int razmer_naim=71-iceb_u_strlen(PROM->masf.ravno(i1));

      sprintf(strsql,"%.*s %s",
      iceb_u_kolbait(razmer_naim,naim_shet.ravno()),
      naim_shet.ravno(),
      PROM->masf.ravno(i1));
     }
    else
      strncpy(strsql,PROM->masf.ravno(i1),sizeof(strsql)-1);
     

    shet_gk_p(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);

    fprintf(ff,"%*.*s %15.2f %15.2f\n",
    iceb_u_kolbait(72,strsql),iceb_u_kolbait(72,strsql),strsql,d3,k3);

    suma_char[0].new_plus(d3,2);
    suma_char[1].new_plus(k3,2);

    fprintf(ffcsv,"|%s|||%s|%s|%s|||||\n",
    naim_shet.ravno(),PROM->masf.ravno(i1),suma_char[0].ravno_dtczp(),suma_char[1].ravno_dtczp());

    shet_gk_l(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
    fprintf(ff_gk_l,"%*.*s %13.2f %13.2f\n",
    iceb_u_kolbait(68,strsql),iceb_u_kolbait(68,strsql),strsql,d3,k3);

   }
 }
shet_gk_p(dn,mn,gn,dk,mk,gk,sli,kst,bf,ff);

fprintf(ff,"\
........................................................................................................................................\n");

shet_gk_l(dn,mn,gn,dk,mk,gk,kollist_gk_l,kolstr_gk_l,bf,ff_gk_l);
fprintf(ff_gk_l,"\
............................................................................................................................\n");

}

/******************************************/
/******************************************/

gint glkniw_r1(class glkniw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;

short dn,mn,gn;
short dk,mk,gk;

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






int metka_r=0;
if(data->rk->metka_r == 0)
 metka_r=1; //По субсчетам
else
 metka_r=0; //По счетам



int metka_naim_shet=0;

if(iceb_poldan("Печатать наименование счёта корреспондента в Главной книге",strsql,"nastrb.alx",data->window) == 0)
 if(iceb_u_SRAV("Вкл",strsql,1) == 0)
  metka_naim_shet=1;


if(sbshet != NULL)
 {
  if(data->rk->metka_bd == 0)
   sprintf(strsql,"%s\n",gettext("Хозрасчёт"));
  if(data->rk->metka_bd == 1)
   sprintf(strsql,"%s\n",gettext("Бюджет"));
   
  iceb_printw(strsql,data->buffer,data->view);
 }


class GLKTMP PROM;
char imaf[64];
sprintf(imaf,"g%d-%d_%d.lst",mn,mk,getpid());
if(metka_r == 1)
    sprintf(imaf,"gs%d-%d_%d.lst",mn,mk,getpid());
char imaf3[64];
sprintf(imaf3,"ob%d-%d_%d.lst",mn,mk,getpid());
FILE *ff_gk_l;
FILE *ff_ob_l;

char imaf_gk_l[64];
char imaf_ob_l[64];
sprintf(imaf_gk_l,"l_g%d.lst",getpid());
sprintf(imaf_ob_l,"l_ob%d.lst",getpid());
FILE *ff;
FILE *ff1;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if((ff1 = fopen(imaf3,"w")) == NULL)
 {
  iceb_er_op_fil(imaf3,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if((ff_gk_l = fopen(imaf_gk_l,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_gk_l,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
//дважды для ланшафтной ориентации

if((ff_ob_l = fopen(imaf_ob_l,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_ob_l,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf_csv[64];
sprintf(imaf_csv,"gkcsv%d.csv",getpid());
class iceb_fopen ffcsv;
if(ffcsv.start(imaf_csv,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
struct  tm      *bf;
bf=localtime(&vremn);
iceb_zagolov(gettext("Главная книга"),dn,mn,gn,dk,mk,gk,ffcsv.ff,data->window);


fprintf(ffcsv.ff,"%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",
gettext("Счёт"),
gettext("Наименование счёта"),
gettext("Дебет"),
gettext("Кредит"),
gettext("Счёт корреспондент"),
gettext("Дебет"),
gettext("Кредит"),
gettext("Дебет"),
gettext("Кредит"));


short godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;
sprintf(strsql,"%s:%d\n",gettext("Стаpтовый год"),godn);
iceb_printw(strsql,data->buffer,data->view);


sprintf(strsql,"%s\n",gettext("Формируем массив счетов"));
iceb_printw(strsql,data->buffer,data->view);

int msrv=0;
if(metka_r == 0)
 msrv=1;
/*************************/
/*Формируем массив счетов*/
/*************************/
int kolstr=0;
sprintf(strsql,"select ns,vids,saldo from Plansh where stat=0 order by ns asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не введён план счетов !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int pozz=0;
class iceb_u_str sh("");
while(cur.read_cursor(&row) != 0)
 {
  pozz++;

  if(atoi(row[1]) == 1 && metka_r == 0)
    continue;

  sh.new_plus(row[0]);
  if(metka_r == 1 && atoi(row[1]) == 0 && pozz < kolstr-1)
   {
    cur.read_cursor(&row);
    /*Если нет субсчетов запоминаем счет*/
    if(iceb_u_SRAV(sh.ravno(),row[0],1) != 0)
     {
      cur.poz_cursor(pozz-1);
      cur.read_cursor(&row);
     }
    else
     pozz++;
   }
  PROM.masf.plus(row[0]);

  if(atoi(row[2]) == 3)
     PROM.msr.plus(row[0]);
 }
int ks=PROM.masf.kolih();

sprintf(strsql,"%s: %d\n",gettext("Количество счетов"),ks);
iceb_printw(strsql,data->buffer,data->view);
/********************
sprintf(strsql,"%s:",gettext("Счета"));
iceb_printw(strsql,data->buffer,data->view);

for(int ish=0; ish < PROM.masf.kolih() ; ish++)
 {
  sprintf(strsql,"%s\n",PROM.masf.ravno(ish));
  iceb_printw(strsql,data->buffer,data->view);
 }
********************/

class iceb_tmptab tabtmp;
const char *imatmptab={"glkni"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shet char(32) not null DEFAULT '',\
kod_kontr char(32) not null DEFAULT '',\
debs double(16,2) not null DEFAULT 0.,\
kres double(16,2) not null DEFAULT 0.,\
debp double(16,2) not null DEFAULT 0.,\
krep double(16,2) not null DEFAULT 0.,\
unique(shet,kod_kontr)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  

int ll1=ks*ks; /*Шахматка для заполнения*/
PROM.ssd.make_class(ks);
PROM.ssk.make_class(ks);
PROM.deb_end.make_class(ks);
PROM.kre_end.make_class(ks);
PROM.sl.make_class(ll1);
PROM.pe.make_class(ll1);

int ksrs=PROM.msr.kolih();
sprintf(strsql,"%s:%d\n",gettext("Количество счетов с развернутым сальдо"),ksrs);
iceb_printw(strsql,data->buffer,data->view);

/*printw("Счета %s\n",msr);*/
/***********************************/
/*Узнаем стартовые сальдо по счетам*/
/***********************************/
sprintf(strsql,"%s\n",gettext("Вычисляем стартовые сальдо по счетам"));
iceb_printw(strsql,data->buffer,data->view);
int ts=0;
int vids=0;
int i=0;
double deb=0.,kre=0.;
for(i=0; i < ks ; i++)
 {
  sh.new_plus(PROM.masf.ravno(i));
  sprintf(strsql,"select saldo,vids from Plansh where ns='%s'",sh.ravno());
  
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,"%s %s",gettext("Не найден счёт %s в плане счетов !"),sh.ravno());
    iceb_menu_soob(strsql,data->window);
    continue;
   }

  ts=0;
  if(atoi(row[0]) == 3)
    ts=1;
  vids=atoi(row[1]);
  
  if(vids == 0) /*Счёт*/
   sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='%d' and gs=%d \
and ns like '%s%%'",ts,godn,sh.ravno());
  if(vids == 1) /*Субсчёт*/
   sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='%d' and gs=%d \
and ns='%s'",ts,godn,sh.ravno());

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
     continue;
    }
   if(kolstr == 0)
     continue;

  while(cur.read_cursor(&row) != 0)
   {
    deb=atof(row[1]);
    kre=atof(row[2]);
    if(row[0][0] != '\0')
     {
      glkni_zvt(0,sh.ravno(),row[0],deb,kre,imatmptab,data->window);
     }

    PROM.ssd.plus(deb,i);
    PROM.ssk.plus(kre,i);
   }
/*  
  printw("%s %.2f %.2f\n",sh,ssd[i],ssk[i]);
  getch();
*/
  if(ts == 0)  /*Счета с не развернутым сальдо*/
   {
    if(PROM.ssd.ravno(i) >= PROM.ssk.ravno(i))
     {
      PROM.ssd.plus(PROM.ssk.ravno(i)*-1,i);
      PROM.ssk.new_plus(0.,i);
     }
    else
     {
      PROM.ssk.plus(PROM.ssd.ravno(i)*-1,i);
      PROM.ssd.new_plus(0.,i);
     }
   }
 }

/******************************************/
/*Просматриваем записи и заполняем массивы*/
/******************************************/

sprintf(strsql,"%s\n",gettext("Просматриваем проводки, заполняем массивы"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov \
where val=0 and datp >= '%d-%02d-%02d' and \
datp <= '%d-%02d-%02d'",godn,1,1,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено проводок !"),data->window);
 }
else
 {
  double cel=kolstr;
  double drob=0.;

  drob=modf(cel/2.,&drob);
  if(drob > 0.000001)
   {
    class iceb_u_str repl;
    
    sprintf(strsql,"%s !!!",gettext("Внимание"));
    repl.plus(strsql);
    
    repl.ps_plus(gettext("Не парное количество проводок"));
    repl.ps_plus(gettext("Проверте базу данных на логическую целосность"));
    iceb_menu_soob(&repl,data->window);
   }
 }
sprintf(strsql,"%s:%d\n",gettext("Количество проводок"),kolstr);
iceb_printw(strsql,data->buffer,data->view);

int mpe=0;
float kolstr1=0.;
short den,mes,god;
class iceb_u_str kor("");
class iceb_u_str shk("");
double sm=0.;
int i1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  iceb_u_rsdat(&den,&mes,&god,row[0],2);
  kor.new_plus(row[3]);
  deb=atof(row[4]);   
  kre=atof(row[5]);
  

  if(iceb_u_sravmydat(dn,mn,gn,den,mes,god) > 0) /*До периода*/
   {
     mpe=0;
   }

  if(iceb_u_sravmydat(dn,mn,gn,den,mes,god) <= 0) /*период*/
  if(iceb_u_sravmydat(dk,mk,gk,den,mes,god) >= 0) /*период*/
   {
    mpe=1;
   }
  if(row[3][0] != '\0')
   {
    glkni_zvt(mpe,row[1],row[3],deb,kre,imatmptab,data->window);
   
   }
  sh.new_plus("");
  if(fabs(deb) > 0.009)
   {
    sh.new_plus(row[1]);
    shk.new_plus(row[2]);
    sm=deb;
   }
  else
   {
    sh.new_plus(row[2]);
    shk.new_plus(row[1]);
    sm=kre;
   }
  if(sh.ravno()[0] == '\0')
   {
    class iceb_u_str repl;
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    repl.plus(strsql);
    
    repl.ps_plus(gettext("Не введён счёт !"));
    iceb_menu_soob(&repl,data->window);
    continue;
   }

  if(shk.ravno()[0] == '\0')
   {
    class iceb_u_str repl;
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    repl.plus(strsql);
    repl.ps_plus(gettext("Не введён коресподирующий счёт !"));
    iceb_menu_soob(&repl,data->window);
    continue;
   }

  if((i=PROM.masf.find(sh.ravno(),msrv)) == -1)
   {
    class iceb_u_str repl;
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    repl.plus(strsql);
    
    sprintf(strsql,"%s %s",gettext("Не найден счёт в массиве счетов !"),sh.ravno());
    repl.ps_plus(strsql);

    iceb_menu_soob(&repl,data->window);


    continue;
   }

  if((i1=PROM.masf.find(shk.ravno(),msrv)) == -1)
   {
    class iceb_u_str repl;
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    repl.plus(strsql);

    sprintf(strsql,gettext("Не найден счёт кореспондент %s в масиве счетов !"),shk.ravno());
    repl.ps_plus(strsql);
    repl.ps_plus("rozraxw");
    iceb_menu_soob(&repl,data->window);

    continue;
   }

  if(mpe == 0)
    PROM.sl.plus(sm,i*ks+i1);
  else
    PROM.pe.plus(sm,i*ks+i1);
  
/*Проверяем есть ли неправильные проводки*/
  if(ksrs != 0)
   if(PROM.msr.find(row[2],msrv) >= 0)
   {

    if(kor.ravno()[0] == '\0')
     {
      if(ff != NULL)
       fprintf(ff,"%d.%d.%d %-5s %-5s %-5s %10s %10s\n",
       den,mes,god,row[1],row[2],row[3],row[4],row[5]);

      class iceb_u_str repl;
      sprintf(strsql,"%d.%d.%d %-5s %-5s %-5s %10s %10s",
      den,mes,god,row[1],row[2],row[3],row[4],row[5]);
      repl.plus(strsql);

      repl.ps_plus(gettext("В проводке нет кода контрагента !"));
      repl.ps_plus(strsql);
      repl.ps_plus("rozraxw");

      iceb_menu_soob(&repl,data->window);

     } 
   }

  /*Проверяем есть ли счёт в списке счетов с развернутым сальдо*/
  if(ksrs != 0)
   if(PROM.msr.find(row[1],msrv) >= 0)
   {
   
    if(kor.ravno()[0] == '\0')
     {
      if(ff != NULL)
       fprintf(ff,"%d.%d.%d %-5s %-5s %-5s %10s %10s\n",
       den,mes,god,row[1],row[2],row[3],row[4],row[5]);

      class iceb_u_str repl;
      sprintf(strsql,"%d.%d.%d %-5s %-5s %-5s %10s %10s",
      den,mes,god,row[1],row[2],row[3],row[4],row[5]);
      repl.plus(strsql);

      repl.ps_plus(gettext("В проводке нет кода контрагента !"));
      repl.ps_plus(strsql);
      repl.ps_plus("rozraxw");

      iceb_menu_soob(&repl,data->window);

     } 

   }



 }





sprintf(strsql,"%s\n",gettext("Расчёт закончен"));
iceb_printw(strsql,data->buffer,data->view);

/************************/
/*Распечатка результатов*/
/************************/
/*Так как все проводки встречаются дважды то все делим на 2*/
ks=PROM.masf.kolih();
sm=0.;
for(int i=0;i<ks*ks;i++)
 {
   if(PROM.sl.ravno(i) != 0.)
    {
     sm=PROM.sl.ravno(i)/2.;
     PROM.sl.new_plus(sm,i);
    }
   if(PROM.pe.ravno(i) != 0.)
    {
     sm=PROM.pe.ravno(i)/2.;
     PROM.pe.new_plus(sm,i);
    }
 }

sprintf(strsql,"%s\n",gettext("Распечатываем"));
iceb_printw(strsql,data->buffer,data->view);


int sli=0,sli1=0;
int kst=0,kst1=0; //Один перевод строки при назначении режимов печати
int kolstr_gk_l=0;
int kolstr_ob_l=0;
int kollist_gk_l=0;
int kollist_ob_l=0;

//printf("%s-kolstr=%d  %d\n",__FUNCTION__,kol_strok_na_liste_u,kol_strok_na_liste_l_u);
fprintf(ff1,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff,"\x1B\x45"); //режим выделенного шрифта
fprintf(ff,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/
fprintf(ff1,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/

fprintf(ff_gk_l,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/
fprintf(ff_ob_l,"\x1B\x33%c\n",30); /*Уменьшаем межстрочный интервал*/
if(data->rk->kod_subbal.ravno()[0] == '\0')
 {
  fprintf(ff,"%s\n\n",iceb_get_pnk("00",0,data->window));
  fprintf(ff1,"%s\n\n",iceb_get_pnk("00",0,data->window));
  fprintf(ff_gk_l,"%s\n\n",iceb_get_pnk("00",0,data->window));
  fprintf(ff_ob_l,"%s\n\n",iceb_get_pnk("00",0,data->window));
  kst+=1;
  kst1+=1;
  kolstr_gk_l+=1;
  kolstr_ob_l+=1;
 }
else
 {
  int kolih_kod_subbal=0;
  iceb_printcod(ff,"Glksubbal","kod","naik",0,data->rk->kod_subbal.ravno(),&kolih_kod_subbal);
  fprintf(ff,"\n");

  kst+=kolih_kod_subbal;
  kst1+=kolih_kod_subbal;
  kolstr_gk_l+=kolih_kod_subbal;
  kolstr_ob_l+=kolih_kod_subbal;

  iceb_printcod(ff1,"Glksubbal","kod","naik",0,data->rk->kod_subbal.ravno(),&kolih_kod_subbal);
  fprintf(ff1,"\n");
  iceb_printcod(ff_gk_l,"Glksubbal","kod","naik",0,data->rk->kod_subbal.ravno(),&kolih_kod_subbal);
  fprintf(ff_gk_l,"\n");
  iceb_printcod(ff_ob_l,"Glksubbal","kod","naik",0,data->rk->kod_subbal.ravno(),&kolih_kod_subbal);
  fprintf(ff_ob_l,"\n");
 } 

fprintf(ff,"\x0E"); /*Включение режима удвоенной ширины на одну строку*/
fprintf(ff,"\
                %s\n",
gettext("Главная бухгалтерськая книга"));

fprintf(ff,"\x0E"); /*Включение режима удвоенной ширины на одну строку*/
fprintf(ff,"\
            %s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."));


fprintf(ff1,"\x0E"); /*Включение режима удвоенной ширины на одну строку*/
fprintf(ff1,"\
               %s\n",
gettext("Б а л а н с"));

fprintf(ff1,"\x0E"); /*Включение режима удвоенной ширины на одну строку*/
fprintf(ff1,"\
        %s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."));



fprintf(ff_gk_l,"\
%35s %s\n\
%30s %s %d.%d.%d%s %s %d.%d.%d%s\n",
" ",
gettext("Главная бухгалтерськая книга"),
" ",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."));



fprintf(ff_ob_l,"\
%30s %s\n\
%25s %s %d.%d.%d%s %s %d.%d.%d%s\n",
" ",
gettext("Б а л а н с"),
" ",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."));


kst+=4;
kst1+=4; //Один перевод строки при назначении режимов печати
kolstr_gk_l+=4;
kolstr_ob_l+=4;


gsap_s(dn,mn,gn,dk,mk,gk,&sli,&kst,bf,ff);
gsao1_s(&sli1,&kst1,bf,ff1);

gklni_gk_slo(dn,mn,gn,dk,mk,gk,&kollist_gk_l,&kolstr_gk_l,bf,ff_gk_l);
gsao1_s(&kollist_ob_l,&kolstr_ob_l,bf,ff_ob_l);

double di=0,ki=0.;
double dns=0.,kns=0.,dpp=0.,kpp=0.;
for(int i=0; i < PROM.masf.kolih() ; i++)
 {
  iceb_pbar(data->bar,PROM.masf.kolih(),i+1);    

  sh.new_plus(PROM.masf.ravno(i));

  if(iceb_u_proverka(data->rk->shet.ravno(),sh.ravno(),1,0) != 0)
    continue;

  if(sbshet != NULL)
   {
    if(data->rk->metka_bd == 0) //Хозрасчёт
     if(iceb_u_proverka(sbshet,sh.ravno(),1,0) == 0)
       continue;

    if(data->rk->metka_bd == 1) //Бюджет
     if(iceb_u_proverka(sbshet,sh.ravno(),1,0) != 0)
      continue;
   }

  sprintf(strsql,"select saldo,nais,vids,ksb from Plansh where ns='%s'",sh.ravno());
  
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,gettext("Нет счета %s в плане счетов !"),sh.ravno());
    iceb_menu_soob(strsql,data->window);
    continue;
   }

  if(iceb_u_proverka(data->rk->kod_subbal.ravno(),row[3],0,0) != 0)
   continue;
   
  int saldo=atoi(row[0]);
  class iceb_u_str nais(row[1]);
/************
  if(saldo == 3)
   sprintf(strsql,"%-*s R %s\n",iceb_u_kolbait(6,sh.ravno()),sh.ravno(),nais.ravno());
  else
   sprintf(strsql,"%-*s   %s\n",iceb_u_kolbait(6,sh.ravno()),sh.ravno(),nais.ravno());

  iceb_printw(strsql,data->buffer,data->view);
************/
  
  if(saldo == 3)
   {

    ras2n_tab(dn,mn,gn,dk,mk,gk,&sli,&kst,&sli1,&kst1,i,sh.ravno(),nais.ravno(),&PROM,&dns,&kns,&dpp,&kpp,&di,&ki,bf,
    &kollist_gk_l,&kolstr_gk_l,&kollist_ob_l,&kolstr_ob_l,metka_naim_shet,
    imatmptab,
    ff,ff1,ff_gk_l,ff_ob_l,ffcsv.ff,data->window);
   }
  else
   {
    ras1(dn,mn,gn,dk,mk,gk,&sli,&kst,&sli1,&kst1,i,sh.ravno(),nais.ravno(),&PROM,&dns,&kns,&dpp,&kpp,&di,&ki,bf,
    &kollist_gk_l,&kolstr_gk_l,&kollist_ob_l,&kolstr_ob_l,metka_naim_shet,ff,ff1,ff_gk_l,ff_ob_l,ffcsv.ff,data->window);
   }
 }
int kolbit=iceb_u_kolbait(35,gettext("Итого"));
fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------------\n\
%*s%15.2f %15.2f %-5s %15.2f %15.2f %15.2f %15.2f\n",
kolbit,gettext("Итого"),dns,kns," ",dpp,kpp,di,ki);
kolbit=iceb_u_kolbait(13,gettext("Итого"));

class iceb_u_str itogo_char[6];
itogo_char[0].new_plus(dns,2);
itogo_char[1].new_plus(kns,2);
itogo_char[2].new_plus(dpp,2);
itogo_char[3].new_plus(kpp,2);
itogo_char[4].new_plus(di,2);
itogo_char[5].new_plus(ki,2);

fprintf(ffcsv.ff,"\
|%s|%s|%s|%s|%s|%s|%s|%s|\n",
gettext("Итого"),itogo_char[0].ravno_dtczp(),itogo_char[1].ravno_dtczp(),
" ",itogo_char[2].ravno_dtczp(),itogo_char[3].ravno_dtczp(),
itogo_char[4].ravno_dtczp(),itogo_char[5].ravno_dtczp());

fprintf(ff1,"\
-------------------------------------------------------------------------------------------------------------\n\
%-*s %15.2f %15.2f %15.2f %15.2f %15.2f %15.2f\n",
kolbit,gettext("Итого"),dns,kns,dpp,kpp,di,ki);

fprintf(ff,"\n       %s______________________\n",gettext("Главный бухгалтер"));
fprintf(ff1,"\n       %s______________________\n",gettext("Главный бухгалтер"));

iceb_podpis(ff,data->window);
iceb_podpis(ff1,data->window);
kolbit=iceb_u_kolbait(35,gettext("Итого"));

fprintf(ff_gk_l,"\
----------------------------------------------------------------------------------------------------------------------------\n\
%*s%13.2f %13.2f %-5s %13.2f %13.2f %13.2f %13.2f\n",
kolbit,gettext("Итого"),dns,kns," ",dpp,kpp,di,ki);

kolbit=iceb_u_kolbait(13,gettext("Итого"));

fprintf(ff_ob_l,"\
-------------------------------------------------------------------------------------------------------------\n\
%-*s %15.2f %15.2f %15.2f %15.2f %15.2f %15.2f\n",
kolbit,gettext("Итого"),dns,kns,dpp,kpp,di,ki);

fprintf(ff_gk_l,"\n       %s______________________\n",gettext("Главный бухгалтер"));
fprintf(ff_ob_l,"\n       %s______________________\n",gettext("Главный бухгалтер"));

iceb_podpis(ff_gk_l,data->window);
iceb_podpis(ff_ob_l,data->window);


gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace","foreground","red",NULL);



char bros[512];
sprintf(bros,"%s %02d.%02d.%d",gettext("Сальдо на"),dn,mn,gn);
sprintf(strsql,"\n\
%*s - %18s",iceb_u_kolbait(20,bros),bros,iceb_u_prnbr(dns));

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql," %18s\n",iceb_u_prnbr(kns));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"%-*s - %18s",
iceb_u_kolbait(iceb_u_strlen(bros),gettext("Оборот за период")),
gettext("Оборот за период"),iceb_u_prnbr(dpp));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql," %18s\n",iceb_u_prnbr(kpp));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(bros,"%s %02d.%02d.%d",gettext("Сальдо на"),dk,mk,gk);

sprintf(strsql,"%*s - %18s",iceb_u_kolbait(20,bros),bros,iceb_u_prnbr(di));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql," %18s\n",iceb_u_prnbr(ki));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

iceb_podpis(ffcsv.ff,data->window);
ffcsv.end();
fclose(ff);
fclose(ff1);
fclose(ff_gk_l);
fclose(ff_ob_l);


sprintf(strsql,"%s - %d\n",gettext("Количество листов"),sli);
iceb_printw(strsql,data->buffer,data->view);


char imaf_bal_f1[64];
memset(imaf_bal_f1,'\0',sizeof(imaf_bal_f1));
char imaf_bal_f2[64];
memset(imaf_bal_f2,'\0',sizeof(imaf_bal_f2));
if(metka_r == 1)
 {
  balans_f1w(data->rk->metka_bd,dk,mk,gk,imaf_bal_f1,&PROM,data->window);
  balans_f2w(data->rk->metka_bd,dk,mk,gk,imaf_bal_f2,&PROM,data->window);
 }


data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Главная книга (портрет)"));

data->rk->imaf.plus(imaf3);
data->rk->naimf.plus(gettext("Оборотный баланс (портрет)"));




if(imaf_bal_f1[0] != '\0')
 {
  data->rk->imaf.plus(imaf_bal_f1);
  data->rk->naimf.plus(gettext("Баланс, форма N1"));

  data->rk->imaf.plus(imaf_bal_f2);
  data->rk->naimf.plus(gettext("Отчёт про финансовые результаты, форма N2"));
 }

for(int nom=0; nom < data->rk->imaf.kolih();nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);
 
data->rk->imaf.plus(imaf_gk_l);
data->rk->naimf.plus(gettext("Главная книга (ландшафт)"));
iceb_ustpeh(imaf_gk_l,2,data->window);


data->rk->imaf.plus(imaf_ob_l);
data->rk->naimf.plus(gettext("Оборотный баланс (ландшафт)"));

iceb_ustpeh(imaf_ob_l,2,data->window);

data->rk->imaf.plus(imaf_csv);
data->rk->naimf.plus(gettext("Главная книга для импорта в электронные таблицы"));

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);
/**********
GtkTextIter iter;

gtk_text_buffer_get_iter_at_offset(data->buffer,&iter,-1);
gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(data->view),&iter,0.0,TRUE,0.,1.);
************/



iceb_beep();

data->voz=0;
return(FALSE);

}
