/*$Id: uosvosw_r.c,v 1.25 2013/09/26 09:46:57 sasa Exp $*/
/*23.05.2016	14.01.2008	Белых А.И.	uosvosw_r.c
Расчёт отчёта 
*/
#include <math.h>
#include <errno.h>
#include "buhg_g.h"
#include "uosvosw.h"
#define         KOLSTSH  6  /*Количество строк в шапке*/

class uosvosw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class uosvosw_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  int metka_r;  
  uosvosw_r_data()
   {
    metka_r=0;
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosvosw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvosw_r_data *data);
gint uosvosw_r1(class uosvosw_r_data *data);
void  uosvosw_r_v_knopka(GtkWidget *widget,class uosvosw_r_data *data);


extern SQL_baza bd;




extern short	startgoduos; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;
//extern char	*spgnunpa; //Список групп налогового учёта с не пообъектной амортизацией
extern char	*spgnu; //Список групп налогового учёта

int uosvosw_r(int metka_r,class uosvosw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class uosvosw_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);
data.rk=datark;
data.metka_r=metka_r;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

if(metka_r == 0)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать ведомость остаточной стоимости (налоговый учёт)"));
if(metka_r == 1)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать ведомость остаточной стоимости (бух. учёт)"));

//sprintf(strsql,"%s %s",_config.system_name(),gettext("Распечатка ведомости аморт. отчислений для налогового учёта"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(uosvosw_r_key_press),&data);

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
if(metka_r == 0)
  repl.plus(gettext("Распечатать ведомость остаточной стоимости (налоговый учёт)"));
if(metka_r == 1)
  repl.plus(gettext("Распечатать ведомость остаточной стоимости (бух. учёт)"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(uosvosw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)uosvosw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosvosw_r_v_knopka(GtkWidget *widget,class uosvosw_r_data *data)
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

gboolean   uosvosw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvosw_r_data *data)
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

/****************/
/*Итого по шифру и подразделению для износа*/
/****************/
void uosvos_ithz(const char *sh,double k1,double s1,
short mt,  //0-по подразделению 1- по шифру 2-общий итог 3-по счету учёта
int *kst,
double *iznos_per,
class bsiz_per *data_i,
FILE *ff,FILE *ffhpz)
{
char		bros[512];
char		str[512];

memset(str,'\0',sizeof(str));
memset(str,'-',93);

if(mt == 2)
 {
  sprintf(bros,gettext("Общий итог"));
 }

if(mt == 1)
 {
  if(sh[0] == '\0')
   return;
  sprintf(bros,"%s %s",gettext("Итого по шифру"),sh);
  if(ffhpz != NULL)
   fprintf(ffhpz,"%s\n\
%11s %-*s %10s %12.2f %12.2f %12.2f\n",str," ",iceb_u_kolbait(30,bros),bros," ",k1,s1,k1-s1);
 }

if(mt == 0)
 {
  if(k1+s1+(k1-s1) < 0.0001)
   return;
  sprintf(bros,"%s %s",gettext("Итого по подразделению"),sh);
 }

if(mt == 3)
 {
  if(k1+s1+(k1-s1) < 0.0001)
   return;
  sprintf(bros,"%s %s",gettext("Итого по счёту"),sh);
 }

fprintf(ff,"%s\n\
%11s %-*s %10s %12.2f %12.2f %12.2f\n",str," ",iceb_u_kolbait(30,bros),bros," ",k1,s1,k1-s1);

 
if(mt == 2 || mt == 3)
 if(ffhpz != NULL)
  fprintf(ffhpz,"%s\n\
%11s %-*s %10s %12.2f %12.2f %12.2f\n",str," ",iceb_u_kolbait(30,bros),bros," ",k1,s1,k1-s1);

*kst=*kst+2;
/***********
printw("\n%s:\n\
%s %12.2f %12.2f %12.2f\n",
bros,
gettext("Баланс. ст./Износ/Ост.ст.:"),
k1,s1,k1-s1);
************/
data_i->clear_data(); //обнуляем массив

}
/********/
/*Шапка*/
/*******/

void uosvos_hhap(short dk,short mk,short gk,
int sl, //Счетчик листов
int *kst,  //Счетчик строк
int skl,
const char *nai,
short mt,  //0-по подразделению 1-по материально-ответств 
FILE *ff)
{
char		str[512];
char		bros[512];

if(mt == 0)
 sprintf(bros,"%s %d %s",gettext("Подразделение"),skl,nai);

if(mt == 1)
 sprintf(bros,"%s %d %s",gettext("Мат.-ответственный"),skl,nai);

if(mt == 2)
 sprintf(bros,"%s",nai);

memset(str,'\0',sizeof(str));
memset(str,'-',93);

fprintf(ff,"%-*s %s N%d\n%s\n",
iceb_u_kolbait(71,bros),bros,
gettext("Лист"),
sl,str);

fprintf(ff,gettext("\
Iнвентарный|     Наименование             |          | Балансовая |   Износ    | Остаточная |\n\
  номер    |                              |  Номер   |стоимость на|    на      |стоимость на|\n\
	   |                              |          |%02d.%02d.%dг.|%02d.%02d.%dг.|%02d.%02d.%dг.|\n"),
dk,mk,gk,dk,mk,gk,dk,mk,gk);

fprintf(ff,"%s\n",str);

 
*kst+=KOLSTSH;

}

/*******/
/*Счетчик строк*/
/*******/
void uosvos_gsap(short dk,short mk,short gk,
int *sl,  //Счетчик листов
int *kst, //Счетчик строк
int skl,const char *nai,
short mt,   //0-по подразделению 1-по материально-ответств
FILE *ff)
{

*kst=*kst+1;

if(*kst <= kol_strok_na_liste)
 return;

*sl+=1;
*kst=1;
fprintf(ff,"\f");

uosvos_hhap(dk,mk,gk,*sl,kst,skl,nai,mt,ff);


}
/****************/
/*Распечатка одной группы налогового учёта для которой амартизация считается в целом по группе*/
/******************************************/

void uosvos_porgup(char *kodgr,
short dk,short mk,short gk,
double *suma_pogr,
FILE *ff,FILE *ffhpz,
GtkWidget *wpredok)
{
class iceb_u_str naigrup("");
SQL_str row;
SQLCURSOR cur;
char strsql[512];


sprintf(strsql,"select naik from Uosgrup where kod='%s'",kodgr);
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 naigrup.new_plus(row[0]);
  
class bsiz_data data;
bsiz23pw(kodgr,dk,mk,gk,dk,mk,gk,&data,wpredok);

fprintf(ff,"\
%11s %-*.*s %-10.10s %12.2f %12.2f %12.2f\n",
kodgr,
iceb_u_kolbait(30,naigrup.ravno()),iceb_u_kolbait(30,naigrup.ravno()),naigrup.ravno(),
"",

data.nu.bs_nah_per,
data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per,
data.nu.bs_nah_per-(data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per));




fprintf(ffhpz,"\
%11s %-*.*s %-10.10s %12.2f %12.2f %12.2f\n",
kodgr,
iceb_u_kolbait(30,naigrup.ravno()),iceb_u_kolbait(30,naigrup.ravno()),naigrup.ravno(),
"",

data.nu.bs_nah_per,
data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per,
data.nu.bs_nah_per-(data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per));


suma_pogr[0]+=data.nu.bs_nah_per;
suma_pogr[1]+=data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per;
suma_pogr[2]+=data.nu.bs_nah_per-(data.nu.izmen_iznosa_nah_per+data.nu.amort_nah_per);

}

/********************************/
/*Распечатка в целом по группе по списку групп*/
/*******************************/

void uosvos_pogurp_s(short dk,short mk,short gk,
short sli,
int *kst,
const char *hnaz,
double *suma_pogr,
FILE *ff,FILE *ffhpz,
GtkWidget *wpredok)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
sprintf(strsql,"select kod from Uosgrup where ta=1");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
  return;
fprintf(ff,"\n\n");
uosvos_hhap(dk,mk,gk,sli,kst,0,gettext("Группы с амортизацией в целом по группе"),2,ff);

fprintf(ffhpz,"\n\n");
uosvos_hhap(dk,mk,gk,sli,kst,0,gettext("Группы с амортизацией в целом по группе"),2,ffhpz);
while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(hnaz,row[0],0,0) != 0)
   continue;

  uosvos_porgup(row[0],dk,mk,gk,suma_pogr,ff,ffhpz,wpredok);
 }

char str[512];
memset(str,'\0',sizeof(str));
memset(str,'-',93);

fprintf(ff,"%s\n",str);
fprintf(ff,"\
%*s %12.2f %12.2f %12.2f\n",
iceb_u_kolbait(53,gettext("Итого")),gettext("Итого"),
suma_pogr[0],
suma_pogr[1],
suma_pogr[2]);

fprintf(ffhpz,"%s\n",str);
fprintf(ffhpz,"\
%*s %12.2f %12.2f %12.2f\n",
iceb_u_kolbait(53,gettext("Итого")),gettext("Итого"),
suma_pogr[0],
suma_pogr[1],
suma_pogr[2]);
 


}
/**********************************/
/*Начало отчётов*/
/***********************************/

void uosvosii_start(short dk,short mk,short gk,
int viduh,
int metr,
int metka_sort,
int *kst,
const char *podrz,
const char *hnaz,
const char *hnabyz,
const char *kodolz,
const char *hauz,
const char *shetupoi,
const char *innomp,
const char *hztz,
int sosob,
int metkaost,
FILE *ff)
{

*kst=5;

iceb_u_zagolov(gettext("Ведомость остаточной стоимости основных средств"),0,0,0,dk,mk,gk, iceb_get_pnk("00",0,NULL),ff);

*kst+=1;

if(viduh == 0)
 {
  fprintf(ff,"%s\n",gettext("Налоговый учёт"));
 }
if(viduh == 1)
 {
  fprintf(ff,"%s\n",gettext("Бухгалтерский учёт"));
 }

int klst=0;
if(podrz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),podrz);
  iceb_printcod(ff,"Uospod","kod","naik",0,podrz,&klst);
  *kst+=klst+1;
 }

if(hnaz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа налогового учёта"),hnaz);
  iceb_printcod(ff,"Uosgrup","kod","naik",1,hnaz,&klst);
  *kst+=klst+1;
 }
if(hnabyz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа бухгалтерского учёта"),hnabyz);
  iceb_printcod(ff,"Uosgrup1","kod","naik",1,hnabyz,&klst);
  *kst+=klst+1;
 }
if(kodolz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Мат.-ответственный"),kodolz);
  iceb_printcod(ff,"Uosol","kod","naik",1,kodolz,&klst);
  *kst+=klst+1;
 }
if(hauz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр аналитического учета"),hauz);
  iceb_printcod(ff,"Uoshau","kod","naik",0,hauz,&klst);
  *kst+=klst+1;
 }
if(shetupoi[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Счет"),shetupoi);
  *kst+=1;
 }

if(innomp[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Инвентарный номер"),innomp);
  *kst+=1;
 }

if(hztz[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр затрат"),hztz);
  *kst+=1;
 }

if(sosob != 0)
 {
  
  fprintf(ff,"%s:%d",gettext("Состояние объекта"),sosob);
  if(sosob == 1)
   fprintf(ff,"%s\n",gettext("Основные средства, амортизация для которых считается"));
  if(sosob == 2)
   fprintf(ff,"%s\n",gettext("Основные средства, амортизация для которых не считается"));
   
  *kst+=1;
 }

if(metkaost == 1)
 {
  fprintf(ff,"%s\n",gettext("Основные средства с нулевою остаточной стоимостью"));
  *kst+=1;
 }
if(metkaost == 2)
 {
  fprintf(ff,"%s\n",gettext("Основные средства с ненулевой остаточной стоимостью"));
  *kst+=1;
 }
*kst+=1;
if(metr == 0)
 fprintf(ff,"%s\n",gettext("По подразделениям"));
if(metr == 1)
 fprintf(ff,"%s\n",gettext("По материально-ответственным"));

*kst+=1;
if(metka_sort == 0)
 fprintf(ff,"%s\n",gettext("Сортировка по группам"));
if(metka_sort == 1)
 fprintf(ff,"%s\n",gettext("Сортировать по счетам учёта"));

}


/******************************************/
/******************************************/

gint uosvosw_r1(class uosvosw_r_data *data)
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
sprintf(strsql,"select innom,naim from Uosin");

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
class iceb_tmptab tabtmp;
const char *imatmptab={"uosvosw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shu char(24) not null,\
kmo int not null,\
innom int not null,\
naim char(112) not null,\
pod int not null,\
nomz char(56) not null,\
hna char(32) not null,\
hnaby char(32) not null,\
unique(kmo,innom,pod)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  


float kolstr1=0;
class SQLCURSOR cur1;
int in=0;
int poddz=0,kodotl=0;
int pod=0,podz=0;
SQL_str row1 = NULL;
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s\n",row[0],row[1]);
  
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->innom.ravno(),row[0],0,0) != 0)
    continue;
    
  in=atoi(row[0]);

  if(poiinw(in,dk,mk,gk,&poddz,&kodotl,data->window) != 0) 
     continue; //Произошел расход или на эту дату небыло прихода

  if(iceb_u_proverka(data->rk->podr.ravno(),poddz,0,0) != 0)
      continue;

  if(iceb_u_proverka(data->rk->mat_ot.ravno(),kodotl,0,0) != 0)
      continue;

  class poiinpdw_data rekin;
  poiinpdw(in,mk,gk,&rekin,data->window);

  if(data->metka_r == 0) //Пропускаем все основные средства если для них амортизазия считается в целом по группе
   if(uosprovgrw(rekin.hna.ravno(),data->window) == 1)
    continue;

  if(data->metka_r == 0 ) //Налоговый учёт
   {
    if(rekin.soso == 0 || rekin.soso == 3) //амортизация считается
     if(data->rk->sost_ob == 2)
      continue;
    if(rekin.soso == 1 || rekin.soso == 2) //амортизация не считаестя
     if(data->rk->sost_ob == 1)
      continue;
    
   }
  if(data->metka_r == 1 ) //Бух.учёт
   {
    if(rekin.soso == 0 || rekin.soso == 2) //амортизация считается
     if(data->rk->sost_ob == 2)
      continue;
    if(rekin.soso == 1 || rekin.soso == 3) //амортизация не считаестя
     if(data->rk->sost_ob == 1)
      continue;
    
   }
  if(iceb_u_proverka(data->rk->shetu.ravno(),rekin.shetu.ravno(),0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->hzt.ravno(),rekin.hzt.ravno(),0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->grupa_nu.ravno(),rekin.hna.ravno(),0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->grupa_bu.ravno(),rekin.hnaby.ravno(),0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->hau.ravno(),rekin.hau.ravno(),0,0) != 0)
    continue;

  sprintf(strsql,"insert into %s values ('%s',%d,%d,'%s',%d,'%s','%s','%s')",
  imatmptab,
  rekin.shetu.ravno(),kodotl,atoi(row[0]),iceb_u_sqlfiltr(row[1]),poddz,rekin.nomz.ravno(),rekin.hna.ravno(),rekin.hnaby.ravno());

  iceb_sql_zapis(strsql,1,1,data->window);    

 }


if(data->rk->metka_mat_podr == 0) //По подразделениям
 {
  if(data->rk->metka_sort == 0)
   {
    if(data->metka_r == 0) //налоговый учёт
      sprintf(strsql,"select * from %s order by hna asc,pod asc,innom asc",imatmptab);
    if(data->metka_r == 1) //бух. учёт
      sprintf(strsql,"select * from %s order by hnaby asc,pod asc,innom asc",imatmptab);
   }
  if(data->rk->metka_sort == 1)
      sprintf(strsql,"select * from %s order by shu asc,pod asc,innom asc",imatmptab);
 }
if(data->rk->metka_mat_podr == 1) //По материально-ответственным
 {
  if(data->rk->metka_sort == 0)
   {
    if(data->metka_r == 0) //налоговый учёт
      sprintf(strsql,"select * from %s order by hna asc,kmo asc,innom asc",imatmptab);
    if(data->metka_r == 1) //бух.учёт
      sprintf(strsql,"select * from %s order by hnaby asc,kmo asc,innom asc",imatmptab);
   }
  if(data->rk->metka_sort == 1)
      sprintf(strsql,"select * from %s order by shu asc,kmo asc,innom asc",imatmptab);

 }

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



char imaf[64];
sprintf(imaf,"vos%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imafhpz[64];
sprintf(imafhpz,"vosi%d.lst",getpid());
FILE *ffhpz;
if((ffhpz = fopen(imafhpz,"w")) == NULL)
 {
  iceb_er_op_fil(imafhpz,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


char imaf_br[64];

sprintf(imaf_br,"vosbr%d.lst",getpid());
FILE *ff_br;

if((ff_br = fopen(imaf_br,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_br,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int sli=0,kst=0;

uosvosii_start(dk,mk,gk,data->metka_r,data->rk->metka_mat_podr,data->rk->metka_sort,&kst,data->rk->podr.ravno(),
data->rk->grupa_nu.ravno(),data->rk->grupa_bu.ravno(),data->rk->mat_ot.ravno(),data->rk->hau.ravno(),data->rk->shetu.ravno(),data->rk->innom.ravno(),data->rk->hzt.ravno(),data->rk->sost_ob,data->rk->metka_ost,ff);

uosvosii_start(dk,mk,gk,data->metka_r,data->rk->metka_mat_podr,data->rk->metka_sort,&kst,data->rk->podr.ravno(),
data->rk->grupa_nu.ravno(),data->rk->grupa_bu.ravno(),data->rk->mat_ot.ravno(),data->rk->hau.ravno(),data->rk->shetu.ravno(),data->rk->innom.ravno(),data->rk->hzt.ravno(),data->rk->sost_ob,data->rk->metka_ost,ffhpz);


int kolstr_br=0;
int kol_list_br=1;

uosvosii_start(dk,mk,gk,data->metka_r,data->rk->metka_mat_podr,data->rk->metka_sort,&kolstr_br,data->rk->podr.ravno(),
data->rk->grupa_nu.ravno(),data->rk->grupa_bu.ravno(),data->rk->mat_ot.ravno(),data->rk->hau.ravno(),data->rk->shetu.ravno(),data->rk->innom.ravno(),data->rk->hzt.ravno(),data->rk->sost_ob,data->rk->metka_ost,ff_br);

 
int kstt=0;
uosvos_hhap(dk,mk,gk,1,&kstt,0,"",2,ffhpz);

uosvos_hhap(dk,mk,gk,kol_list_br,&kolstr_br,0,"",2,ff_br);

double bs1=0.,bss1=0.;
double s1=0.,ss1=0.;
double bs11=0.,s11=0.;
int mvsi=0,mvsh=0;
in=0;
sli=1;
podz=0;
class iceb_u_str hna1("");
class iceb_u_str hna("");
class iceb_u_str naipod("");
double iznos_per[2];
double i_iznos_per[2];
double iznos_per_pod[2];
double iznos_per_hna[2];
memset(&i_iznos_per,'\0',sizeof(i_iznos_per));
memset(&iznos_per,'\0',sizeof(iznos_per));
memset(&iznos_per_pod,'\0',sizeof(iznos_per_pod));
memset(&iznos_per_hna,'\0',sizeof(iznos_per_hna));

class bsiz_per data_podr; //Итоги по подразделению
class bsiz_per data_hz;   //Итоги по шифру затрат
class bsiz_per data_oi;   //Общий итог по всему отчёту
class bsiz_per data_shet; //Итог по счёту
class iceb_u_str nomz("");
class iceb_u_str bros("");
int podrp=0;
class iceb_u_str naimos("");
class SQLCURSOR curr;
class iceb_u_str naigrup("");
int kolos=0;
kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(data->rk->metka_sort == 0)
   {
    if(data->metka_r == 0) //налоговый учёт
     hna.new_plus(row[6]);
    if(data->metka_r == 1) //бух.учёт
     hna.new_plus(row[7]);
   }
  if(data->rk->metka_sort == 1)
    hna.new_plus(row[0]);

  if(data->rk->metka_mat_podr == 0) //По подразделениям
   {
    podrp=pod=atoi(row[4]);
   }
  if(data->rk->metka_mat_podr == 1) //По материально-ответственным
   {
    pod=atoi(row[1]);
    podrp=atoi(row[4]);
   }
  in=atoi(row[2]);
  naimos.new_plus(row[3]);
  nomz.new_plus(row[5]);

  memset(iznos_per,'\0',sizeof(iznos_per));

  
  class bsiz_data data_per;
  
  bsizpw(in,podrp,0,dk,mk,gk,dk,mk,gk,&data_per,NULL,data->window);
  
  double pr1=0.,pr2=0.;  
  if(data->metka_r == 0)
   {
    pr1=data_per.nu.bs_nah_per; //Балансовая стоимость на начало периода
    pr2=data_per.nu.izmen_iznosa_nah_per+data_per.nu.amort_nah_per;  //Износ на начало периода
      
   }
  
  if(data->metka_r == 1)
   {
    pr1=data_per.bu.bs_nah_per; //Балансовая стоимость на начало периода
    pr2=data_per.bu.izmen_iznosa_nah_per+data_per.bu.amort_nah_per;  //Износ на начало периода
    
   }
    
  
  
  if(fabs(pr1) < 0.01 ) //Значит уже продан или списан или передан в другое подразделение
   {
//    printw("in=%d %d.%d.%d %d.%d.%d\n",in,dn,mn,gn,dp,mp,gp);
//    printw("pr1=%.2f pr2=%.2f pr11=%.2f pr22=%.2f\n",
//    pr1,pr2,pr11,pr22);
//    OSTANOV();
    continue;
   }


  if(podz != pod)
   {
    if(podz != 0)
     {
      bros.new_plus(podz);
      uosvos_ithz(bros.ravno(),bss1,ss1,0,&kst,iznos_per_pod,&data_podr,ff,ffhpz);
      memset(&iznos_per_pod,'\0',sizeof(iznos_per_pod));
      bss1=0.;
      ss1=0.;
     }

    if(data->rk->metka_mat_podr == 0)
     {
      /*Читаем наименование подразделения*/
      sprintf(strsql,"select naik from Uospod where kod=%d",pod);
      if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
       {
        sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),pod);
        iceb_menu_soob(strsql,data->window);
        fprintf(ff,"%s %d !\n",gettext("Не найден код подразделения"),pod);
        naipod.new_plus("");
       }
      else
        naipod.new_plus(row1[0]);
     }

    if(data->rk->metka_mat_podr == 1)
     {
      /*Читаем мат.-ответственного*/
      sprintf(strsql,"select naik from Uosol where kod=%d",pod);
      if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
       {
        sprintf(strsql,"%s %d !",gettext("Не найден мат.-ответственный"),pod);
        iceb_menu_soob(strsql,data->window);
        
        fprintf(ff,"%s %d !\n",gettext("Не найден мат.-ответственный"),pod);
        naipod.new_plus("");
       }
      else
        naipod.new_plus(row1[0]);
     }

    mvsh++;
             
    podz=pod;
    if(data->rk->metka_mat_podr == 0)
     sprintf(strsql,"\n%s - %d/%s\n",gettext("Подразделение"),podz,naipod.ravno());
    if(data->rk->metka_mat_podr == 1)
     sprintf(strsql,"\n%s - %d/%s\n",gettext("Мат.-ответственный"),podz,naipod.ravno());

    iceb_printw(strsql,data->buffer,data->view);
   }

  if(iceb_u_SRAV(hna1.ravno(),hna.ravno(),0) != 0)
   {
    if(hna1.ravno()[0] != '\0')
     {
      if(data->rk->metka_sort == 0)
       uosvos_ithz(hna1.ravno(),bs1,s1,1,&kst,iznos_per_hna,&data_hz,ff,ffhpz);
      if(data->rk->metka_sort == 1)
       uosvos_ithz(hna1.ravno(),bs1,s1,3,&kst,iznos_per_hna,&data_shet,ff,ffhpz);
      memset(&iznos_per_hna,'\0',sizeof(iznos_per_hna));
      bs1=s1=0.;
     }
    
    if(data->rk->metka_sort == 0)
     {
      /*Уэнаем наименование шифра нормы аморт. отчисления*/
      if(iceb_u_SRAV(hna.ravno(),"???",0) != 0)
       {
        if(data->metka_r == 0)
          sprintf(strsql,"select naik from Uosgrup where kod='%s'",hna.ravno());
        if(data->metka_r == 1)
          sprintf(strsql,"select naik from Uosgrup1 where kod='%s'",hna.ravno());

        if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
         {
          sprintf(strsql,"%s %s !",gettext("Не найдено группу"),hna.ravno());
          iceb_menu_soob(strsql,data->window);
          fprintf(ff,"%s %s !\n",gettext("Не найдено группу"),hna.ravno());
          naigrup.new_plus("");
         }
        else
          naigrup.new_plus(row1[0]);
       }
      else
       naigrup.new_plus(gettext("Не введена в карточку группа"));
      sprintf(strsql,"\n%s %s \"%s\"\n",gettext("Группа"),hna.ravno(),naigrup.ravno());

      iceb_printw(strsql,data->buffer,data->view);
     }
    if(data->rk->metka_sort == 1)
     {
      sprintf(strsql,"select nais from Plansh where ns='%s'",hna.ravno());
      if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
        naigrup.new_plus(row1[0]);
      else
        naigrup.new_plus("");
     
      sprintf(strsql,"\n%s %s \"%s\"\n",
      gettext("Счёт учёта"),hna.ravno(),naigrup.ravno());

      iceb_printw(strsql,data->buffer,data->view);
     }
    mvsi++;


    hna1.new_plus(hna.ravno());

   }
  if(mvsh > 0 || mvsi > 0) 
   {  
    if(kst > kol_strok_na_liste-KOLSTSH-5)
     {
      fprintf(ff,"\f");
      sli++;
      kst=0;
     }
    if(mvsi > 0)
     {
      if(data->rk->metka_sort == 0)
       {
        fprintf(ff,"\n%s %s \"%s\"\n",gettext("Группа"),hna.ravno(),naigrup.ravno());
       }
      if(data->rk->metka_sort == 1)
       {
        fprintf(ff,"\n%s %s \"%s\"\n",
        gettext("Счёт учёта"),hna.ravno(),naigrup.ravno());
       }
      mvsi=0;
      kst+=2;
     } 
    uosvos_hhap(dk,mk,gk,sli,&kst,pod,naipod.ravno(),data->rk->metka_mat_podr,ff);
    mvsh=0;
   }

  if(data->metka_r == 0)
   {
    data_podr.plus(&data_per.nu);
    data_oi.plus(&data_per.nu);
    data_hz.plus(&data_per.nu);
    data_shet.plus(&data_per.nu);
   }
  if(data->metka_r == 1)
   {
    data_podr.plus(&data_per.bu);
    data_oi.plus(&data_per.bu);
    data_hz.plus(&data_per.bu);
    data_shet.plus(&data_per.bu);
   }
  double izn=pr2;
  double bsn=pr1;
  double osts=bsn-izn;

  if(osts >  0.009 && data->rk->metka_ost == 1)
   continue;

  if(osts ==  0. && data->rk->metka_ost == 2)
   continue;

  bss1+=bsn;
  ss1+=izn;
  
  bs1+=bsn;
  s1+=izn;
  
  bs11+=bsn;
  s11+=izn;


  if(fabs(bsn+izn+osts) > 0.009)
   {

    uosvos_gsap(dk,mk,gk,&sli,&kst,pod,naipod.ravno(),data->rk->metka_mat_podr,ff);
    fprintf(ff,"%11d %-*.*s %-*.*s %12.2f %12.2f %12.2f\n",
    in,
    iceb_u_kolbait(30,naimos.ravno()),iceb_u_kolbait(30,naimos.ravno()),naimos.ravno(),
    iceb_u_kolbait(10,nomz.ravno()),iceb_u_kolbait(10,nomz.ravno()),nomz.ravno(),
    bsn,izn,osts);

    uosvos_gsap(dk,mk,gk,&kol_list_br,&kolstr_br,0,"",2,ff_br);
    fprintf(ff_br,"%11d %-*.*s %-*.*s %12.2f %12.2f %12.2f\n",
    in,
    iceb_u_kolbait(30,naimos.ravno()),iceb_u_kolbait(30,naimos.ravno()),naimos.ravno(),
    iceb_u_kolbait(10,nomz.ravno()),iceb_u_kolbait(10,nomz.ravno()),nomz.ravno(),
    bsn,izn,osts);

    if(iceb_u_strlen(naimos.ravno()) > 30)
     {
    
      uosvos_gsap(dk,mk,gk,&sli,&kst,pod,naipod.ravno(),data->rk->metka_mat_podr,ff);
      fprintf(ff,"%11s %s\n","",iceb_u_adrsimv(30,naimos.ravno()));
  
      uosvos_gsap(dk,mk,gk,&kol_list_br,&kolstr_br,0,"",2,ff_br);
      fprintf(ff_br,"%11s %s\n","",iceb_u_adrsimv(30,naimos.ravno()));
     }
    kolos++;
   }
 }



bros.new_plus(pod);
uosvos_ithz(bros.ravno(),bss1,ss1,0,&kst,iznos_per_pod,&data_podr,ff,ffhpz);

if(data->rk->metka_sort == 0)
  uosvos_ithz(hna1.ravno(),bs1,s1,1,&kst,iznos_per_hna,&data_hz,ff,ffhpz);
if(data->rk->metka_sort == 1)
  uosvos_ithz(hna1.ravno(),bs1,s1,3,&kst,iznos_per_hna,&data_shet,ff,ffhpz);

/*Итог Общий*/
uosvos_ithz(" ",bs11,s11,2,&kst,i_iznos_per,&data_oi,ff,ffhpz);
uosvos_ithz(" ",bs11,s11,2,&kst,i_iznos_per,&data_oi,ff_br,NULL);


if(data->metka_r == 0) //распечатка по группам с общегрупповой амортизацией
 {
  double suma_pogr[3];
  memset(suma_pogr,'\0',sizeof(suma_pogr));

  uosvos_pogurp_s(dk,mk,gk,sli,&kst,data->rk->grupa_nu.ravno(),suma_pogr,ff,ffhpz,data->window);

  suma_pogr[0]+=bs11;
  suma_pogr[1]+=s11;
  suma_pogr[2]+=bs11-s11;
  
  char str[1024];
  memset(str,'\0',sizeof(str));
  memset(str,'-',93);

  fprintf(ff,"%s\n",str);
  fprintf(ff,"\
%*s %12.2f %12.2f %12.2f\n",
  iceb_u_kolbait(53,gettext("Общий итог")),gettext("Общий итог"),
  suma_pogr[0],
  suma_pogr[1],
  suma_pogr[2]);


  fprintf(ffhpz,"%s\n",str);
  fprintf(ffhpz,"\
%*s %12.2f %12.2f %12.2f\n",
  iceb_u_kolbait(53,gettext("Общий итог")),gettext("Общий итог"),
  suma_pogr[0],
  suma_pogr[1],
  suma_pogr[2]);

 }





sprintf(strsql,"\n%s:%d\n",gettext("Количество основных средств"),kolos);

iceb_printw(strsql,data->buffer,data->view);

fprintf(ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(ffhpz,"\n%s:%d\n",gettext("Количество основных средств"),kolos);

iceb_podpis(ff,data->window);
iceb_podpis(ffhpz,data->window);
iceb_podpis(ff_br,data->window);

fclose(ff);
fclose(ff_br);
fclose(ffhpz);


data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Ведомость остаточной стоимости c сортировкой"));
data->rk->imaf.plus(imaf_br);
data->rk->naim.plus(gettext("Ведомость остаточной стоимости без сортировки"));
data->rk->imaf.plus(imafhpz);
data->rk->naim.plus(gettext("Общий итог"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),0,data->window);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
