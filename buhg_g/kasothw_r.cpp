/*$Id: kasothw_r.c,v 1.49 2014/08/31 06:19:20 sasa Exp $*/
/*26.02.2020	10.03.2006	Белых А.И.	kasothw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "kasothw.h"
#define		KOLSTRHAP 9 //Количество строк в шапке

class kasothw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class kasothw_rr *rk;
  class spis_oth *oth;  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  kasothw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   kasothw_r_key_press(GtkWidget *widget,GdkEventKey *event,class kasothw_r_data *data);
gint kasothw_r1(class kasothw_r_data *data);
void  kasothw_r_v_knopka(GtkWidget *widget,class kasothw_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;

int kasothw_r(class kasothw_rr *datark,class spis_oth *oth,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class kasothw_r_data data;

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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(kasothw_r_key_press),&data);

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(kasothw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)kasothw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  kasothw_r_v_knopka(GtkWidget *widget,class kasothw_r_data *data)
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

gboolean   kasothw_r_key_press(GtkWidget *widget,GdkEventKey *event,class kasothw_r_data *data)
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

void sapkasord(short dd,short md,short gd,int nomlist,const char *zagol,FILE *ffva)
{

class iceb_u_str mesqc("");
iceb_mesc(md,1,&mesqc);

fprintf(ffva,"%s\n",zagol);

fprintf(ffva,"%s \"%02d\" %s %d%s                     %s %d\n",
"Каса за",
dd,mesqc.ravno(),gd,
"р.",
"Сторінка",nomlist);

fprintf(ffva,"\
-------------------------------------------------------------------------\n");
fprintf(ffva,"\
 Номер  |  Від кого отримано чи кому   |  Номер   |  Надход- | Видаток  |\n\
докумен-|         видано               |кореспон- |  ження   |          |\n\
  та    |                              |дуючого   |          |          |\n\
        |                              |рахунку,  |          |          |\n\
        |                              |субрахунку|          |          |\n");
fprintf(ffva,"\
-------------------------------------------------------------------------\n\
   1    |               2              |    3     |    4     |     5    |\n\
-------------------------------------------------------------------------\n");

/*
12345678 123456789012345678901234567890 1234567890 1234567890 1234567890
*/

}
/***************************/
/*Шапка журнала регистрации кассовых документов*/
/***********************************************/

//void   hgrkd(FILE *ff)
void hgrkd(class iceb_rnl_c *rd,int *kolstr,FILE *ff)
{

if(kolstr != NULL)
 *kolstr=*kolstr+8;

fprintf(ff,"%*s N%d\n",iceb_u_kolbait(80,gettext("Лист")),gettext("Лист"),rd->nom_list);

fprintf(ff,"\
------------------------------------------------------------------------------------------\n\
  N  |Прибутковий докум. |          |          |Видатковий документ|          |          |\n\
 з/п ---------------------  Сума    |Примітки  |--------------------  Сума    |Примітки  |\n\
     |  дата    | номер  |          |          |  дата    | номер  |          |          |\n\
------------------------------------------------------------------------------------------\n\
  1  |    2     |   3    |    4     |     5    |     6    |   7    |    8     |    9     |\n\
------------------------------------------------------------------------------------------\n");


}
/*************************/
/*Счётчик строк*/
/*******************/
void kasothr_ss(int metka, //0-счётчик применять 1-не применять
short dd,short md,short gd,int *nomlist,int *kolstr,double itpri,double itras,FILE *ffva,FILE *ffok,FILE *ff_prot)
{
if(metka == 0)
 {
  *kolstr+=1;
  if(*kolstr <= kol_strok_na_liste-1)
   return;
  *kolstr=1;
 }
else
 *kolstr=0;
 
*nomlist+=1;
*kolstr+=KOLSTRHAP;
if(ffva != NULL)
 {
  fprintf(ffva,"%*s:%10.2f %10.2f\n",
  iceb_u_kolbait(50,gettext("Перенос")),gettext("Перенос"),itpri,itras*-1);
  fprintf(ffva,"\f");
  sapkasord(dd,md,gd,*nomlist,gettext("Вкладной лист кассовой книги"),ffva);
 }
if(ffok != NULL)
 {
  fprintf(ffok,"%*s:%10.2f %10.2f\n",
  iceb_u_kolbait(50,gettext("Перенос")),gettext("Перенос"),itpri,itras*-1);
  fprintf(ffok,"\f");
  sapkasord(dd,md,gd,*nomlist,gettext("Отчет кассира"),ffok);
 }
if(ff_prot != NULL)
 fprintf(ff_prot,"Вывели шапку kolstr=%d nomlist=%d\n",*kolstr,*nomlist); 
}
/**********************************************/
/*концовка за день*/
/************************************************/
void kasothr_endday(double itpri,double itras,double isumd,double saldon,FILE *ffva)
{

fprintf(ffva,"\
-------------------------------------------------------------------------\n");

fprintf(ffva,"%*s|%10.2f|%10.2f|\n",iceb_u_kolbait(50,"Разом за день"),"Разом за день",itpri,itras*-1);
fprintf(ffva,"%50s|---------------------|\n"," ");
fprintf(ffva,"%*s|%10.2f|\n",iceb_u_kolbait(50,"Залишок на кінець дня,"),"Залишок на кінець дня,",isumd+saldon);
fprintf(ffva,"%50s|----------|\n"," ");
fprintf(ffva,"%*s|%10s|\n",iceb_u_kolbait(50,"у тому числі на зарплату"),"у тому числі на зарплату"," ");
fprintf(ffva,"%50s|----------|\n"," ");
}

/*************************************************************/
/*Шапка реестра */
/*************************************************************/

void kasothr_sapreestr(int *nomlist,int *kolstr,FILE *ff)
{

*nomlist+=1;
*kolstr+=4;

if(ff != NULL)
 {
  fprintf(ff,"%95s %s N%d\n","",gettext("Страница"),*nomlist);

  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff,gettext("\
Касса|+/-|Номер док.|Дата док. |Сумм док.|Номер бланка |            Фамилия           |Операция \n"));
  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
 }
}
/******************************/
/*счётчик строк*/
/********************************/
int kasothr_shet(int *kolstr,FILE *ff)
{
*kolstr+=1;
if(*kolstr <= kol_strok_na_liste)
 return(0);

if(ff != NULL)
  fprintf(ff,"\f");

*kolstr=1;
return(1);
}
/********************************************/
/*начальная страница рeестра*/
/**************************************/
void kasothr_start_reestr(const char *dat_start,
const char *dat_end,
int vpr,const char *kassa,const char *kodop,const char *shet,int *nomlist,int *kolstr,FILE *ff,GtkWidget *wpredok)
{
class iceb_u_str nadpis("");

if(vpr == 0)
  nadpis.new_plus(gettext("Реестр кассовых ордеров"));
if(vpr == 1)
  nadpis.new_plus(gettext("Реестр приходних кассовых ордеров"));
if(vpr == 2)
  nadpis.new_plus(gettext("Реестр расходных кассовых ордеров"));
  
*kolstr=5;
iceb_zagolov(nadpis.ravno(),dat_start,dat_end,ff,wpredok);


if(kassa[0] != '\0')
 {
  *kolstr+=1;
  if(ff != NULL)
    fprintf(ff,"%s:%s\n",gettext("Касса"),kassa);
 }
if(kodop[0] != '\0')
 {
  *kolstr+=1;
  if(ff != NULL)
    fprintf(ff,"%s:%s\n",gettext("Код операции"),kodop);
 }
if(shet[0] != '\0')
 {
  *kolstr+=1;
  if(ff != NULL)
    fprintf(ff,"%s:%s\n",gettext("Счёт"),shet);
 }
kasothr_sapreestr(nomlist,kolstr,ff);
}



/******************************************/
/******************************************/

gint kasothw_r1(class kasothw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
class iceb_clock sss(data->window);
class iceb_u_str kod_edr("");
SQL_str row,row1;
class SQLCURSOR cur,curr;

class iceb_u_str vrem_start("");
class iceb_u_str vrem_end("");

short dn,mn,gn;
short dk,mk,gk;
short hasn=0,minn=0,sekn=0;
short hask=23,mink=59,sekk=59;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  gtk_widget_destroy(data->window);
  sss.clear_data();
  return(FALSE);

 }

if(iceb_u_polen(data->rk->datan.ravno(),&vrem_start,2,' ') == 0)
 iceb_u_rstime(&hasn,&minn,&sekn,vrem_start.ravno());

if(iceb_u_polen(data->rk->datak.ravno(),&vrem_end,2,' ') == 0)
 iceb_u_rstime(&hask,&mink,&sekk,vrem_end.ravno());


sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(strsql,data->buffer,data->view);


//Читаем код
sprintf(strsql,"select kod from Kontragent where kodkon='00'");
if(iceb_sql_readkey(strsql,&row,&curr,data->window) == 1)
 kod_edr.new_plus(row[0]);

char imaf[64];
class iceb_fopen reestr;
sprintf(imaf,"kassa%d.lst",getpid());

if(reestr.start(imaf,"w",data->window) != 0)
 return(1);

class iceb_fopen prot;
char imaf_prot[64];
sprintf(imaf_prot,"kassapr%d.lst",getpid());
if(prot.start(imaf_prot,"w",data->window) != 0)
 return(1);
 
class iceb_fopen vlkk;
char imaf1[64];
sprintf(imaf1,"kasg%d.lst",getpid());

if(vlkk.start(imaf1,"w",data->window) != 0)
 return(1);

class iceb_fopen zvitk;
char imafok[64]; 
sprintf(imafok,"kasok%d.lst",getpid());
if(zvitk.start(imafok,"w",data->window) != 0)
 return(1);
 
class iceb_fopen gurr;
char imaf2[64];
sprintf(imaf2,"kassf%d.lst",getpid());
if(gurr.start(imaf2,"w",data->window) != 0)
 return(1);
 
fprintf(gurr.ff,"\
                                     Додаток 4\n\
                                     до Положення про ведення касових операцій\n\
                                     у національній валюті в Україні\n\
                                     (пункт 33 розділу III)\n\
                                                         Типова форма N КО-3a\n");
fprintf(gurr.ff,"\n\
%s\n\
(найменування підприємства/установи/організації)\n\n\
Ідентифікаційний код в Єдиному державному реєстрі підприємств та організацій України %s\n",
iceb_get_pnk("00",0,data->window),kod_edr.ravno());  

fprintf(gurr.ff,"\n%30s %s\n","","ЖУРНАЛ");
fprintf(gurr.ff,"%7s %s\n","","реєстрації прибуткових і видаткових касових документів");

iceb_u_zagolov("",data->rk->datan.ravno(),data->rk->datak.ravno(),"",gurr.ff);
  

//Вычисляем сальдо на начало периода
double saldon=0.;

sprintf(strsql,"select kassa,tp,datd,nomd,suma from Kasord1 where \
datd >= '%04d-01-01' and datd < '%04d-%d-%d' and datp <> '0000-00-00' order by datd,nomd asc",
gn,gn,mn,dn);
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
 
short d=0,m=0,g=0,dd=0,md=0,gd=0;
int nomlist_reestr=0;
int kolstr_reestr=0;


int kollist=1; 
int kolstrok=KOLSTRHAP;
float kolstr1=0.;
int koldok=0;
class iceb_u_str nomdz("");
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
//  strzag(LINES-1,0,kolstr,++kolstr1);
  if(iceb_u_proverka(data->rk->kassa.ravno(),row[0],0,0) != 0)
    continue;
    
  iceb_u_rsdat(&dd,&md,&gd,row[2],2);

  if(iceb_u_SRAV("000",row[3],0) == 0)
   {
    if(gd == gn)
     {
      saldon+=atof(row[4]);
     }
    continue;
   }

  if(row[1][0] == '0')
   {
    class iceb_u_str repl;

    sprintf(strsql,"%s:%s",gettext("Дата документа"),iceb_u_datzap(row[2]));
    repl.plus(strsql);

    sprintf(strsql,"%s tp %s Каsord1 %s !!!",gettext("Колонка"),gettext("в таблице"),gettext("равна нолю"));
    repl.ps_plus(strsql);

    iceb_menu_soob(repl.ravno(),data->window);
    continue;
   }

  if(iceb_u_sravmydat(d,m,g,dd,md,gd) != 0)
   {
    if(prot.ff != NULL)
     fprintf(prot.ff,"Сальдо на %d.%d.%d=%.2f\n",dd,md,gd,saldon);
    if(d != 0)
     {
      /*Вывод шапки так как поменялься день*/
      kasothr_ss(1,dd, md, gd,&kollist,&kolstrok,0.,0.,NULL,NULL,prot.ff);
      if(d != dd)
        kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,0.,0.,NULL,NULL,prot.ff);/*строка сальдо*/
     
     }
    else
      kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,0.,0.,NULL,NULL,prot.ff); /*остаток на начало дня*/
  

    kasothr_start_reestr(data->rk->datan.ravno(),data->rk->datak.ravno(),data->rk->metka_pr,data->rk->kassa.ravno(),data->rk->kodop.ravno(),data->rk->shet.ravno(),&nomlist_reestr,&kolstr_reestr,NULL,data->window);
     
    d=dd; m=md; g=gd;
   }      

  if(data->rk->metka_pr != 0)
   {
    if(data->rk->metka_pr == 1 && row[1][0] == '2')
      continue;
    if(data->rk->metka_pr == 2 && row[1][0] == '1')
      continue;
   }

  if(iceb_u_SRAV(nomdz.ravno(),row[3],0) != 0)
   {
    koldok++;
    kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,0.,0.,NULL,NULL,prot.ff);

    if(kasothr_shet(&kolstr_reestr,NULL) == 1)
      kasothr_sapreestr(&nomlist_reestr,&kolstr_reestr,NULL);

    if(prot.ff != NULL)
     fprintf(prot.ff,"%s %s %s %s %s kolstrok=%d\n",row[0],row[1],iceb_u_datzap(row[2]),row[3],row[4],kolstrok);

    nomdz.new_plus(row[3]);
   }    



  if(row[1][0] == '1')
   {
    saldon+=atof(row[4]);
   }
  else
   {
    saldon-=atof(row[4]);
   }


 }

if(prot.ff != NULL)
 {
  fprintf(prot.ff,"Сальдо на начало периода=%.2f\n",saldon);
  fprintf(prot.ff,"\n\n===Количество строк=%d Номер листа =%d\n",kolstrok,kollist);
 }
double saldoden=saldon;

sprintf(strsql,"%s %02d.%02d.%d :%.2f\n",
gettext("Сальдо на"),dn,mn,gn,saldon);
iceb_printw(strsql,data->buffer,data->view);


struct  tm      bf;
time_t time_start=0;
time_t time_end=0;

bf.tm_mday=(int)dn;
bf.tm_mon=(int)(mn-1);
bf.tm_year=(int)(gn-1900);
bf.tm_hour=(int)hasn;
bf.tm_min=(int)minn;
bf.tm_sec=sekn;
    
time_start=mktime(&bf);

bf.tm_mday=(int)dk;
bf.tm_mon=(int)(mk-1);
bf.tm_year=(int)(gk-1900);
bf.tm_hour=(int)hask;
bf.tm_min=(int)mink;
bf.tm_sec=sekk;

time_end=mktime(&bf);

sprintf(strsql,"select kassa,tp,datd,nomd,shetk,kodop,fio,osnov,dopol,nb,vrem \
from Kasord where datd >= '%04d-%d-%d' and datd <= '%04d-%d-%d' and podt=1 \
order by datd,tp,nomd asc",gn,mn,dn,gk,mk,dk);
 
//printw("%s-%s\n",__FUNCTION__,strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  sss.clear_data();
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolstr <= 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  fprintf(reestr.ff,"%s\n",gettext("Не найдено ни одной записи !"));
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

//hgrkd(gurr.ff);
class iceb_rnl_c rh;
hgrkd(&rh,NULL,gurr.ff);

kolstr_reestr=0;

//kasothr_start_reestr(dat_start,dat_end,vpr,kassa,kodop,shet,&nomlist_reestr,&kolstr_reestr,reestr.ff);
kasothr_start_reestr(data->rk->datan.ravno(),data->rk->datak.ravno(),data->rk->metka_pr,data->rk->kassa.ravno(),data->rk->kodop.ravno(),data->rk->shet.ravno(),&nomlist_reestr,&kolstr_reestr,reestr.ff,data->window);

d=m=g=dd=md=gd=0;
class SQLCURSOR cur1;

double itpri=0.,itras=0.,isumd=0.,isumpod=0.;
kolstr1=0;
if(koldok > 0)
  kollist+=1;

int metka_sap=0;

kolstrok=KOLSTRHAP;
int kollist_nah=kollist;
int nom_zap=0;
double sumd=0.,sumpod=0.;
double suma_r=0.,suma_p=0.;
double kolih_prih_dok=0; //Количество приходных документов
double kolih_rash_dok=0; //Количество расходных документов
double itogp=0.,itogr=0.;

//gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace","foreground","red",NULL);

gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);

while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
  if(row[1][0] == '0')
   {
    class iceb_u_str repl;

    sprintf(strsql,"%s:%s",gettext("Дата документа"),iceb_u_datzap(row[2]));
    repl.plus(strsql);

    sprintf(strsql,"%s tp %s Каsord1 %s !!!",gettext("Колонка"),gettext("в таблице"),gettext("равна нолю"));
    repl.ps_plus(strsql);

    iceb_menu_soob(repl.ravno(),data->window);
    continue;
   }

  if(iceb_u_proverka(data->rk->kassa.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->kodop.ravno(),row[5],0,0) != 0)
    continue;

  iceb_u_rsdat(&dd,&md,&gd,row[2],2);

  int tipz=atoi(row[1]);

  if(vrem_start.getdlinna() > 1 || vrem_end.getdlinna() > 1)
   {
    if(atoi(row[10]) < time_start || atoi(row[10]) > time_end)
     {
      sumkasorw(row[0],tipz,row[3],gd,&sumd,&sumpod,data->window);
      if(tipz == 2)
       sumd*=-1;    
      saldoden+=sumd;

      if(atoi(row[10]) < time_start)
        saldon+=sumd;
      continue;     
     }
   }

  if(metka_sap == 0) /*Нужно взять дату первого документа для первой шапки*/
   {
    sapkasord(dd,md,gd,kollist,gettext("Вкладной лист кассовой книги"),vlkk.ff);
    sapkasord(dd,md,gd,kollist,gettext("Отчёт кассира"),zvitk.ff);
    metka_sap++;
   }  

  if(data->rk->kod_kontr.ravno()[0] != '\0')
   {
    sprintf(strsql,"select kontr from Kasord1 where kassa=%s and god=%d and tp=%s and nomd='%s'",
    row[0],gd,row[1],row[3]);

    if(cur1.make_cursor(&bd,strsql) < 0)
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    int metka_kontr=0;
    while(cur1.read_cursor(&row1) != 0)
     {
      if(iceb_u_proverka(data->rk->kod_kontr.ravno(),row1[0],0,0) == 0)
       {
        metka_kontr=1;
        break;
        
       } 
     }
    if(metka_kontr == 0)
     continue;
   }
  
  
  if(iceb_u_sravmydat(d,m,g,dd,md,gd) != 0)
   {
    if(d != 0)
     {
      /*Выводим шапку так как поменялся день*/
      kasothr_ss(1,dd, md, gd,&kollist,&kolstrok,itpri,itras,vlkk.ff,zvitk.ff,prot.ff);
      if(d != dd)
       {
        kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,itpri,itras,vlkk.ff,zvitk.ff,prot.ff);
        fprintf(vlkk.ff,"%*s:%10.2f\n",
        iceb_u_kolbait(50,gettext("Сальдо")),gettext("Сальдо"),isumd+saldon);
        fprintf(zvitk.ff,"%*s:%10.2f\n",
        iceb_u_kolbait(50,gettext("Сальдо")),gettext("Сальдо"),isumd+saldon);
       }      
     }

    if(d == 0)    
     {
      if(iceb_u_SRAV("000",row[3],0) == 0)
       {
        sumkasorw(row[0],tipz,row[3],gd,&sumd,&sumpod,data->window);
        if(tipz == 2)
          sumd*=-1;

        saldoden+=sumd;
        saldon+=sumd;
       }

      kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,itpri,itras,vlkk.ff,zvitk.ff,prot.ff);
      fprintf(vlkk.ff,"%*s:%10.2f\n",iceb_u_kolbait(50,gettext("Остаток на начало дня")),gettext("Остаток на начало дня"),saldoden);
      fprintf(zvitk.ff,"%*s:%10.2f\n",iceb_u_kolbait(50,gettext("Остаток на начало дня")),gettext("Остаток на начало дня"),saldoden);
     }

    d=dd; m=md; g=gd;
    if(iceb_u_SRAV("000",row[3],0) == 0)
      continue;
   }      

  
  
  if(data->rk->metka_pr != 0)
   {
    if(data->rk->metka_pr == 1 && tipz == 2)
      continue;
    if(data->rk->metka_pr == 2 && tipz == 1)
      continue;
   }

  if(iceb_u_proverka(data->rk->shet.ravno(),row[4],0,0) != 0)
    continue;
 
  char vpr1[32]; 
  memset(vpr1,'\0',sizeof(vpr1));
  if(tipz == 1)
   vpr1[0]='+';
  if(tipz == 2)
   vpr1[0]='-';
  iceb_u_rsdat(&dd,&md,&gd,row[2],2);
  class iceb_u_str fio(row[6]);  
  if(fio.ravno()[0] == '\0')
   {
    sprintf(strsql,"select kontr from Kasord1 where kassa=%s and \
god=%d and tp=%d and nomd='%s'",row[0],gd,tipz,row[3]);
    if(sql_readkey(&bd,strsql,&row1,&curr) >= 1)
     {
       /*Читаем наименование контрагента*/
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row1[0]);
      if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
        fio.new_plus(row1[0]);
     }
   }

  sumkasorw(row[0],tipz,row[3],gd,&sumd,&sumpod,data->window);

  if(tipz == 2)
   {
    suma_r+=sumd;

    fprintf(gurr.ff,"%5d %42s %02d.%02d.%d %-*s %10.2f %-*.*s\n",
    ++nom_zap," ",dd,md,gd,
    iceb_u_kolbait(8,row[3]),row[3],
    sumd,
    iceb_u_kolbait(10,row[7]),iceb_u_kolbait(10,row[7]),row[7]);

    for(int ii=10; ii < iceb_u_strlen(row[7]); ii+=10)
     fprintf(gurr.ff,"%79s %-*.*s\n",
     "",
     iceb_u_kolbait(10,iceb_u_adrsimv(ii,row[7])),
     iceb_u_kolbait(10,iceb_u_adrsimv(ii,row[7])),
     iceb_u_adrsimv(ii,row[7]));

    sumd*=-1;
    itras+=sumd;
    kolih_rash_dok++;
   } 
  else
   {
    suma_p+=sumd;

    fprintf(gurr.ff,"%5d %02d.%02d.%d %-*s %10.2f %-*.*s\n",
    ++nom_zap,
    dd,md,gd,
    iceb_u_kolbait(8,row[3]),row[3],
    sumd,
    iceb_u_kolbait(10,row[7]),iceb_u_kolbait(10,row[7]),row[7]);

    for(int ii=10; ii < iceb_u_strlen(row[7]); ii+=10)
     fprintf(gurr.ff,"%36s %-*.*s\n",
     "",
     iceb_u_kolbait(10,iceb_u_adrsimv(ii,row[7])),
     iceb_u_kolbait(10,iceb_u_adrsimv(ii,row[7])),
     iceb_u_adrsimv(ii,row[7]));

    itpri+=sumd;
    kolih_prih_dok++;
   }
  saldoden+=sumd;    
  isumd+=sumd;
  isumpod+=sumpod;  

  if(tipz == 1)
   itogp+=sumd;

  if(tipz == 2)
   itogr+=sumd;
   
  if(kasothr_shet(&kolstr_reestr,reestr.ff) == 1)
    kasothr_sapreestr(&nomlist_reestr,&kolstr_reestr,reestr.ff);

  fprintf(reestr.ff,"%-5s %-3s %-*s %02d.%02d.%d %10.2f %-*s %-*.*s %s %s\n",
  row[0],
  vpr1,
  iceb_u_kolbait(10,row[3]),row[3],
  dd,md,gd,sumd,
  iceb_u_kolbait(13,row[9]),row[9],
  iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno(),
  row[7],
  row[8]);


  kasothr_ss(0,dd, md, gd,&kollist,&kolstrok,itpri,itras,vlkk.ff,zvitk.ff,prot.ff);
   



  sprintf(strsql,"%s %s %-*s %-*s %10.2f %s\n",
  vpr1,
  iceb_u_datzap(row[2]),
  iceb_u_kolbait(10,row[3]),
  row[3],
  iceb_u_kolbait(8,row[5]),
  row[5],
  sumd,
  iceb_u_vremzap(row[10]));
  
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
  
      
  if(prot.ff != NULL)
   fprintf(prot.ff,"%s %s %s %s %s %s Дата записи-%s kolstrok=%d\n",row[0],row[1],row[2],row[3],row[4],row[5],iceb_u_vremzap(row[10]),kolstrok);

  if(tipz == 1)
   {
    fprintf(vlkk.ff,"%-*s %-*.*s %-10.10s %10.2f\n",
    iceb_u_kolbait(8,row[3]),row[3],
    iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno(),
    row[4],sumd);

    fprintf(zvitk.ff,"%-*s %-*.*s %-*.*s %10.2f\n",
    iceb_u_kolbait(8,row[3]),row[3],
    iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno(),
    iceb_u_kolbait(10,row[4]),iceb_u_kolbait(10,row[4]),row[4],
    sumd);
   }
  if(tipz == 2)
   {
    fprintf(vlkk.ff,"%-*s %-*.*s %-*.*s %10s %10.2f\n",
    iceb_u_kolbait(8,row[3]),row[3],
    iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno(),
    iceb_u_kolbait(10,row[4]),iceb_u_kolbait(10,row[4]),row[4],
    " ",sumd*-1);

    fprintf(zvitk.ff,"%-*s %-*.*s %-*.*s %10s %10.2f\n",
    iceb_u_kolbait(8,row[3]),row[3],
    iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno(),
    iceb_u_kolbait(10,row[4]),iceb_u_kolbait(10,row[4]),row[4],
    " ",sumd*-1);
   }
 } 

fprintf(reestr.ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
fprintf(reestr.ff,"%*s:%10.2f\n",iceb_u_kolbait(31,gettext("Итого приход")),gettext("Итого приход"),itogp);
fprintf(reestr.ff,"%*s:%10.2f\n",iceb_u_kolbait(31,gettext("Итого расход")),gettext("Итого расход"),itogr);

kasothr_endday(itpri,itras,isumd,saldon,vlkk.ff);

kasothr_endday(itpri,itras,isumd,saldon,zvitk.ff);

int koldn=iceb_u_period(dn,mn,gn,dk,mk,gk,0);

if(koldn == 1)
 {
  fprintf(vlkk.ff,"%s:%d\n",gettext("Количество листов"),kollist-kollist_nah+1);
  fprintf(zvitk.ff,"%s:%d\n",gettext("Количество листов"),kollist-kollist_nah+1);
 }
else
 {
  fprintf(vlkk.ff,"%s %02d.%02d.%d%s %s %02d.%02d.%d%s\n",
  gettext("С"),
  dn,mn,gn,
  gettext("г."),
  gettext("по"),
  dk,mk,gk,
  gettext("г."));

  fprintf(vlkk.ff,"%s %d %s.\n",
  gettext("распечатано"),
  kollist,
  gettext("листов кассовой книги"));

  fprintf(zvitk.ff,"%s %02d.%02d.%d%s %s %02d.%02d.%d%s\n",
  gettext("С"),
  dn,mn,gn,
  gettext("г."),
  gettext("по"),
  dk,mk,gk,
  gettext("г."));

  fprintf(zvitk.ff,"%s %d %s.\n",
  gettext("распечатано"),
  kollist,
  gettext("листов кассовой книги"));
 }  
class iceb_u_str propis_prih("");
class iceb_u_str propis_rash("");

iceb_u_preobr(kolih_prih_dok,&propis_prih,2);
iceb_u_preobr(kolih_rash_dok,&propis_rash,2);

fprintf(vlkk.ff,"\n%*s_________________\n",
iceb_u_kolbait(10,gettext("Кассир")),
gettext("Кассир"));
fprintf(vlkk.ff,"%13s(підпис)\n\n","");

fprintf(vlkk.ff,"%s\n","Залишок у касовій книзі перевірив і документи в кількості");

fprintf(vlkk.ff,"%s %s\n",propis_prih.ravno(),"прибуткових та");
fprintf(vlkk.ff,"(словами)\n");
fprintf(vlkk.ff,"%s %s\n",propis_rash.ravno(),"видаткових одержав");
fprintf(vlkk.ff,"(словами)\n");

fprintf(vlkk.ff,"\n%*s_______________\n",
iceb_u_kolbait(10,gettext("Бухгалтер")),
gettext("Бухгалтер"));
fprintf(vlkk.ff,"%13s(підпис)\n","");

fprintf(zvitk.ff,"\n%*s_________________\n",
iceb_u_kolbait(10,gettext("Кассир")),
gettext("Кассир"));
fprintf(zvitk.ff,"%13s(підпис)\n\n","");

fprintf(zvitk.ff,"%s\n","Залишок у касовій книзі перевірив і документи в кількості");

fprintf(zvitk.ff,"%s %s\n",propis_prih.ravno(),"прибуткових та");
fprintf(zvitk.ff,"(словами)\n");
fprintf(zvitk.ff,"%s %s\n",propis_rash.ravno(),"видаткових одержав");
fprintf(zvitk.ff,"(словами)\n");


fprintf(zvitk.ff,"\n%*s_______________\n",
iceb_u_kolbait(10,gettext("Бухгалтер")),
gettext("Бухгалтер"));
fprintf(zvitk.ff,"%13s(підпис)\n\n","");

fprintf(gurr.ff,"ICEB_LST_END\n");

fprintf(gurr.ff,"\
------------------------------------------------------------------------------------------\n");
fprintf(gurr.ff,"%*s:%10.2f %31s %10.2f\n",iceb_u_kolbait(25,gettext("Итого")),gettext("Итого"),suma_p," ",suma_r);

iceb_podpis(reestr.ff,data->window);

iceb_podpis(vlkk.ff,data->window);
iceb_podpis(zvitk.ff,data->window);

iceb_podpis(gurr.ff,data->window);


reestr.end();
vlkk.end();

zvitk.end();
gurr.end();

prot.end();



data->oth->spis_imaf.new_plus(imaf);
data->oth->spis_naim.new_plus(gettext("Реестр кассовых ордеров"));

data->oth->spis_imaf.plus(imaf1);
data->oth->spis_naim.plus(gettext("Вкладной лист кассовой книги"));

data->oth->spis_imaf.plus(imafok);
data->oth->spis_naim.plus(gettext("Отчёт кассира"));

for(int nomer=0; nomer < data->oth->spis_imaf.kolih(); nomer++)
 iceb_ustpeh(data->oth->spis_imaf.ravno(nomer),3,data->window);

data->oth->spis_imaf.plus(imaf2);
data->oth->spis_naim.plus(gettext("Типовая форма КО-3а"));

iceb_ustpeh(imaf2,3,&rh.orient,data->window);
iceb_rnl(imaf2,&rh,&hgrkd,data->window);

data->oth->spis_imaf.plus(imaf_prot);
data->oth->spis_naim.plus(gettext("Протокол хода расчёта"));

iceb_ustpeh(imaf_prot,3,data->window);






gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
