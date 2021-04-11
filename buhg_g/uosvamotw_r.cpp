/*$Id: uosvamotw_r.c,v 1.21 2014/04/30 06:14:58 sasa Exp $*/
/*23.05.2016	01.01.2008	Белых А.И.	uosvamotw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "uosvamotw.h"

class uosvamotw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class uosvamotw_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  uosvamotw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosvamotw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvamotw_r_data *data);
gint uosvamotw_r1(class uosvamotw_r_data *data);
void  uosvamotw_r_v_knopka(GtkWidget *widget,class uosvamotw_r_data *data);

void	sapkasord(short dd,short md,short gd,int nomlist,FILE *ffva);
void   hgrkd(FILE *ff);

extern SQL_baza bd;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;
extern float    nemi; /*Необлагаемый минимум*/
extern short    metkabo; //Если равна 1-бюджетная организация
extern class iceb_u_str spis_print_a3; /*Список принтеров формата А4*/

int uosvamotw_r(class uosvamotw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class uosvamotw_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка ведомости аморт-отчислений для налогового учёта"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(uosvamotw_r_key_press),&data);

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

repl.plus(gettext("Распечатка ведомости аморт-отчислений для налогового учёта"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(uosvamotw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)uosvamotw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosvamotw_r_v_knopka(GtkWidget *widget,class uosvamotw_r_data *data)
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

gboolean   uosvamotw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvamotw_r_data *data)
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

/*************************************************/
/*Шапка*/
/*************************************************/

void	uosshap(short mn,short gn,short mk,short gk,struct tm *bf,
int *sl,int mr,FILE *ffi,int *kst)
{
*sl+=1;
*kst+=5;

fprintf(ffi,"\
%s %d.%d.%d %s %s - %d:%d %100s%s N%d\n",
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min,
"",gettext("Лист"),
*sl);

fprintf(ffi,"\
-------------------------------------------------------------------");
if(mr == 1)
  fprintf(ffi,"\
--------------------------------------------------------------------------------");

strpod(mn,mk,11,ffi);

fprintf(ffi,gettext("\
Инв-рный|        Н а и м е н о в а н и е         | Номерной |Шифр |"));
if(mr == 1)
  fprintf(ffi,gettext("\
Норма|Балансовая|  Износ   |Остаточная|Ликвидаци.|Годов. сумма|Мес. сума |Коэфи|"));

strms(0,mn,mk,ffi);
fprintf(ffi,gettext("\
 номер  |             объекта                    |   знак   |н.ам.|"));
if(mr == 1)
  fprintf(ffi,gettext("\
а.отч|стоимость |          |бал. с-сть|стоимость |амортотчисл.|амортотчи.|циент|"));

strms(1,mn,mk,ffi);

fprintf(ffi,"\
-------------------------------------------------------------------");
if(mr == 1)
  fprintf(ffi,"\
--------------------------------------------------------------------------------");
strpod(mn,mk,11,ffi);
}

/*****************/
/*Шапка документа*/
/*****************/
void shvam(short mn,short gn,short mk,short gk,int *sl,
int *kst,int mr,FILE *ff,struct tm *bf)
{

*kst+=1;
if(*kst <= kol_strok_na_liste)
   return;
    
fprintf(ff,"\f");

*kst=1;

uosshap(mn,gn,mk,gk,bf,sl,mr,ff,kst);

}



/*******************************/
/*Выдача итога по подразделению*/
/*******************************/
void itam(short mn,short gn,short mk,short gk,double itg[],const char *nai,int mr,
double *maotp,FILE *ff,FILE *ffi,int *sli,int *kst,
struct tm *bf)

{
short           i;
double          vs;
char		bros[512];

shvam(mn,gn,mk,gk,sli,kst,mr,ff,bf);
fprintf(ff,"\
-------------------------------------------------------------------");
fprintf(ffi,"\
-------------------------------------------------------------------");
if(mr == 1)
 {
  fprintf(ff,"\
--------------------------------------------------------------------------------");
  fprintf(ffi,"\
--------------------------------------------------------------------------------");
 }

strpod(mn,mk,11,ff);
strpod(mn,mk,11,ffi);

memset(bros,'\0',sizeof(bros));
sprintf(bros,"%s %s",gettext("Итого по подразделению"),nai);

shvam(mn,gn,mk,gk,sli,kst,mr,ff,bf);
fprintf(ff,"%-*.*s|",iceb_u_kolbait(66,bros),iceb_u_kolbait(66,bros),bros);

fprintf(ffi,"%-*.*s|",iceb_u_kolbait(66,bros),iceb_u_kolbait(66,bros),bros);

if(mr == 1)
 {
  fprintf(ff,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
  fprintf(ffi,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
 }
vs=0.;
for(i=mn-1 ;i<mk ;i++)
 {
  vs+=maotp[i];
  fprintf(ff,"%10.2f|",maotp[i]);
  fprintf(ffi,"%10.2f|",maotp[i]);
 }
if(mn != mk)
 {
  fprintf(ff,"%10.2f|",vs);
  fprintf(ffi,"%10.2f|",vs);
 }
fprintf(ff,"\n");
fprintf(ffi,"\n");

}
/*******************************/
/*Выдача итога по шифру нормы амортотчислений или группе*/
/*******************************/
void itamh(short mn,short gn,short mk,short gk,double itg[],
const char *kod,
const char *nai,
short mr,
double *maotph,FILE *ff,FILE *ffi,
int metkaitog,  //0-по группе 1-по шифру
int *sli,int *kst,
struct tm *bf)
{
short           i;
double          vs;
char		bros[512];

shvam(mn,gn,mk,gk,sli,kst,mr,ff,bf);
fprintf(ff,"\
-------------------------------------------------------------------");
fprintf(ffi,"\
-------------------------------------------------------------------");

if(mr == 1)
 {
  fprintf(ff,"\
--------------------------------------------------------------------------------");
  fprintf(ffi,"\
--------------------------------------------------------------------------------");
 }
strpod(mn,mk,11,ff);
strpod(mn,mk,11,ffi);
if(metkaitog == 0)
  sprintf(bros,"%s %s %s",gettext("Итого по группе"),kod,nai);
if(metkaitog == 1)
  sprintf(bros,"%s %s %s",gettext("Итого по шифру"),kod,nai);

shvam(mn,gn,mk,gk,sli,kst,mr,ff,bf);
fprintf(ff,"%-*.*s|",
iceb_u_kolbait(66,bros),iceb_u_kolbait(66,bros),bros);
fprintf(ffi,"%-*.*s|",
iceb_u_kolbait(66,bros),iceb_u_kolbait(66,bros),bros);

if(mr == 1)
 {
  fprintf(ff,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
  fprintf(ffi,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
 }
vs=0.;
for(i=mn-1 ;i<mk ;i++)
 {
  vs+=maotph[i];
  fprintf(ff,"%10.2f|",maotph[i]);
  fprintf(ffi,"%10.2f|",maotph[i]);
 }

if(mn != mk)
 {
  fprintf(ff,"%10.2f|",vs);
  fprintf(ffi,"%10.2f|",vs);
 }
fprintf(ff,"\n");
fprintf(ffi,"\n");

}
/*******************************/
/*Выдача итога по организации*/
/*******************************/
void itamo(short mn,short mk,double itg[],short mr,
double *maotpo,FILE *ff,FILE *ffi)
{
short           i;
double          vs;
char		bros[512];

fprintf(ff,"\
-------------------------------------------------------------------");
fprintf(ffi,"\
-------------------------------------------------------------------");
if(mr == 1)
 {
  fprintf(ff,"\
--------------------------------------------------------------------------------");
  fprintf(ffi,"\
--------------------------------------------------------------------------------");
 }
strpod(mn,mk,11,ff);
strpod(mn,mk,11,ffi);

sprintf(bros,gettext("Общий итог"));

fprintf(ff,"%-*.*s|",iceb_u_kolbait(66,bros),iceb_u_kolbait(66,bros),bros);

fprintf(ffi,"%-*.*s|",iceb_u_kolbait(66,bros),iceb_u_kolbait(66,bros),bros);

if(mr == 1)
 {
  fprintf(ff,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
  fprintf(ffi,"%5s|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5s|",
" ",itg[0],itg[1],itg[2],itg[3],itg[4],itg[5]," ");
 }
vs=0.;
for(i=mn-1 ;i<mk ;i++)
 {
  vs+=maotpo[i];
  fprintf(ff,"%10.2f|",maotpo[i]);
  fprintf(ffi,"%10.2f|",maotpo[i]);
 }

if(mn != mk)
 {
  fprintf(ff,"%10.2f|",vs);
  fprintf(ffi,"%10.2f|",vs);
 }
fprintf(ff,"\n");
fprintf(ffi,"\n");

}





/******************************************/
/******************************************/

gint uosvamotw_r1(class uosvamotw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;

short mn,gn;
short mk,gk;

mn=data->rk->mesn.ravno_atoi();
mk=data->rk->mesk.ravno_atoi();
gn=gk=data->rk->god.ravno_atoi();
if(mk == 0)
 mk=mn;




int metka23=0;

if(iceb_sql_readkey("select kod from Uosgrup where ta=1 limit 1",data->window) >= 1)
 {
  metka23=1;
 }
else
 metka23=2;
 

short mr=0;
int kol_mes_v_per=iceb_u_period(1,mn,gn,1,mk,gk,1);

if(kol_mes_v_per <=3)
 mr=1;
else
 mr=0;

short d=1;
short mp=mn;
short gp=gn;
iceb_u_dpm(&d,&mp,&gp,2);


sprintf(strsql,"%s %d.%d%s %s %d.%d%s (%d.%d)\n",
gettext("Период с"),
mn,gn,
gettext("г."),
gettext("по"),
mk,gk,
gettext("г."),
mp,gp);

iceb_printw(strsql,data->buffer,data->view);

//sprintf(strsql,"select innom,mes,god,podr,hzt,hna,suma from Uosamor where god = %d and mes >= %d and mes <= %d order by hna,podr,innom,god,mes asc",gn,mn,mk);
sprintf(strsql,"select innom,da,podr,hzt,hna,suma from Uosamor where da >= '%04d-%02d-01' and da <= '%04d-%02d-31 order by hna,podr,innom,da asc",gn,mn,gk,mk);
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
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
sprintf(imaf,"vao%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  sss.clear_data();
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
/***************
int orient=1;
//if(iceb_u_proverka(spis_print_a3.ravno(),getenv("PRINTER"),0,1) == 0)
  orient=0; //Если принтер А4 то ориентация всегда портрет
*******************/


iceb_u_zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,31,mk,gk,iceb_get_pnk("00",0,data->window),ff);


fprintf(ff,"%s.\n",gettext("Налоговый учёт"));
int kst=6;

if(data->rk->grupa.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Група"),data->rk->grupa.ravno());
  kst++;
 }

if(data->rk->podr.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),data->rk->podr.ravno());
  kst++;
 }
if(data->rk->hzt.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр затрат"),data->rk->hzt.ravno());
  kst++;
 }
if(data->rk->innom.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Инвентарный номер"),data->rk->innom.ravno());
  kst++;
 }
if(data->rk->mat_ot.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Мат.ответственный"),data->rk->mat_ot.ravno());
  kst++;
 }
if(data->rk->sost_ob != 0)
 {
  fprintf(ff,"%s: %d\n",gettext("Состояние объекта"),data->rk->sost_ob);
  kst++;
 }
if(data->rk->shetu.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Счёт"),data->rk->shetu.ravno());
  kst++;
 }
if(data->rk->hau.getdlinna() > 1)
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр аналитического учёта"),data->rk->hau.ravno());
  kst++;
 }
char imafi[64];
sprintf(imafi,"vaoi%d.lst",getpid());
FILE *ffi;
if((ffi = fopen(imafi,"w")) == NULL)
 {
  sss.clear_data();
  iceb_er_op_fil(imafi,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_u_zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,31,mk,gk,iceb_get_pnk("00",0,data->window),ffi);


fprintf(ffi,"%s\n",gettext("Налоговый учёт"));
int ksti=6;

if(data->rk->grupa.ravno()[0] != '\0')
 {
  ksti++;
  fprintf(ffi,"%s: %s\n",gettext("Група"),data->rk->grupa.ravno());
 }
if(data->rk->podr.ravno()[0] != '\0')
 {
  ksti++;
  fprintf(ffi,"%s: %s\n",gettext("Подразделение"),data->rk->podr.ravno());
 }
if(data->rk->hzt.ravno()[0] != '\0')
 {
  ksti++;
  fprintf(ffi,"%s: %s\n",gettext("Шифр затрат"),data->rk->hzt.ravno());
 }
if(data->rk->innom.ravno()[0] != '\0')
 {
  ksti++;
  fprintf(ffi,"%s: %s\n",gettext("Инвентарный номер"),data->rk->innom.ravno());
 }
if(data->rk->shetu.ravno()[0] != '\0')
 {
  ksti++;
  fprintf(ffi,"%s: %s\n",gettext("Счёт"),data->rk->shetu.ravno());
 }
if(data->rk->hau.getdlinna() > 1)
 {
  ksti++;
  fprintf(ffi,"%s: %s\n",gettext("Шифр аналитического учёта"),data->rk->hau.ravno());
 }
if(data->rk->sost_ob != 0)
 {
  ksti++;
  fprintf(ffi,"%s: %d\n",gettext("Состояние объекта"),data->rk->sost_ob);
 }


struct  tm      *bf;
bf=localtime(&vremn);
int slii=0;
uosshap(mn,gn,mk,gk,bf,&slii,mr,ffi,&ksti);


double          maot[12];
double          maotp[12];
double          maotph[12];
double          maotpo[12];
int innom=0;
for(int i=0; i< 12; i++)
    maot[i]=maotp[i]=maotph[i]=maotpo[i]=0.;
int mpg=0;
const int RAZMER_MAS=6;
double          itg[RAZMER_MAS];
double          itg1[RAZMER_MAS];
double          itg2[RAZMER_MAS];

for(int i=0;i< RAZMER_MAS;i++)
  itg[i]=itg1[i]=itg2[i]=0.;
class iceb_u_str naipod("");
class iceb_u_str naimhz("");

naipod.new_plus(iceb_get_pnk("00",0,data->window));
int pod1=0;
float kolstr1=0.;
int in1=0,mvr=0;
class iceb_u_str hna(""),hnaz1("");
class iceb_u_str filnast("uosnast.alx");
double          ostt=0.;
int sli=0;
int kolos=0;
short m=0,g=0;
double sao=0.;
SQL_str row1;
class SQLCURSOR curr;
class iceb_u_str naios("");
class iceb_u_str bros("");
class iceb_u_str bros1("");
float ppkf=1;   /*Поправочный коэффициент*/
class iceb_u_str hnap("");
float		kof;
double          god=0,mes=0;
class iceb_u_str invn("");
class poiinpdw_data rekin;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->innom.ravno(),row[0],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->hzt.ravno(),row[3],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->podr.ravno(),row[2],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->grupa.ravno(),row[4],0,0) != 0)
    continue;

  innom=atol(row[0]);

  if(metka23 == 1)
   if(uosprovgrw(row[4],data->window) == 1)
     if(innom > 0)
      continue;

  if(metka23 == 2 )
    if(uosprovgrw(row[4],data->window) == 1)
     if(innom < 0)
      continue;
     
  if(data->rk->mat_ot.ravno()[0] != '\0')
   {
    int pdd=0;
    int kol=0;
    poiinw(innom,1,mn,gn,&pdd,&kol,data->window);
    if(iceb_u_proverka(data->rk->mat_ot.ravno(),kol,0,0) != 0)
      continue;
   }

 iceb_u_rsdat(&d,&m,&g,row[1],2);
//  m=atoi(row[1]);
//  g=atoi(row[2]);
  int pod=atoi(row[2]);
  hna.new_plus(row[4]);
  sao=atof(row[5]);

  if(mpg == 0)  
   if(iceb_u_SRAV(hna.ravno(),"1",0) == 0)
    mpg=1;

  if(iceb_u_SRAV(hnaz1.ravno(),hna.ravno(),0) != 0)
   {

    if(hnaz1.ravno()[0] != '\0')
     {
      if(mvr > 0)
       {
        rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff);
        mvr=0;
        if(uosprovgrw(hnaz1.ravno(),data->window) != 1)
         {
          itam(mn,gn,mk,gk,itg1,naipod.ravno(),mr,maotp,ff,ffi,&sli,&kst,bf);
          pod1=0;
         }
       }
      itamh(mn,gn,mk,gk,itg2,hnaz1.ravno(),naimhz.ravno(),mr,maotph,ff,ffi,0,&sli,&kst,bf);
      for(int i=0;i< RAZMER_MAS;i++)
	itg1[i]=itg2[i]=0.;

      fprintf(ff,"\f");
      kst=0;
     }

    /*Читаем группу налогового учёта*/
    sprintf(strsql,"select naik from Uosgrup where kod='%s'",hna.ravno());
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %s !\n",gettext("Не найдена группа"),hna.ravno());
      iceb_menu_soob(strsql,data->window);
      naimhz.new_plus("");
     }
    else
     naimhz.new_plus(row1[0]);
          
    sprintf(strsql,"%s: %s %s\n",gettext("Группа"),hna.ravno(),naimhz.ravno());
    iceb_printw(strsql,data->buffer,data->view);

    shvam(mn,gn,mk,gk,&sli,&kst,mr,ff,bf);

    fprintf(ff,"%s: %s %s\n",gettext("Группа"),hna.ravno(),naimhz.ravno());
    uosshap(mn,gn,mk,gk,bf,&sli,mr,ff,&kst);

    for(int i=0; i< 12; i++)
     maotp[i]=maotph[i]=0.;

    hnaz1.new_plus(hna.ravno());
   }
  if(pod1 != pod)
   {
    if(pod1 != 0 && mvr > 0)
     {
      rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff);
      mvr=0;
      if(uosprovgrw(hnaz1.ravno(),data->window) != 1)
         itam(mn,gn,mk,gk,itg1,naipod.ravno(),mr,maotp,ff,ffi,&sli,&kst,bf);
     }
    for(int i=0;i< RAZMER_MAS;i++)
      itg1[i]=0.;
    /*Читаем наименование подразделения*/
    if(innom > 0)
     {
      sprintf(strsql,"select naik from Uospod where kod=%d",pod);
      if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
       {
        sprintf(strsql,"%s %d !\n",gettext("Не найден код подразделения"),pod);
        iceb_menu_soob(strsql,data->window);
        naipod.new_plus("");
       }
      else
       naipod.new_plus(row1[0]);
     }
    else
      naipod.new_plus(iceb_get_pnk("00",0,data->window));

    sprintf(strsql,"%s %d %s\n",gettext("Подразделение"),pod,naipod.ravno());
    iceb_printw(strsql,data->buffer,data->view);

    shvam(mn,gn,mk,gk,&sli,&kst,mr,ff,bf);
    
    fprintf(ff,"%s %d %s\n",gettext("Подразделение"),pod,naipod.ravno());

    for(int i=0; i< 12; i++)
     maotp[i]=0.;

    pod1=pod;
   }
  if(innom > 0)
   {
    /*Узнаем поправочный коэффициент счёт учёта в этом месяце*/
    if(poiinpdw(innom,m,g,&rekin,data->window) != 0)
     {
      
      shvam(mn,gn,mk,gk,&sli,&kst,mr,ff,bf);
      fprintf(ff,"%d инвентарный номер не числится %d.%dг. !!!\n",innom,m,g);

      sprintf(strsql,"%d инвентарный номер не числится %d.%dг. !!!",innom,m,g);
      iceb_menu_soob(strsql,data->window);
     }
    if(iceb_u_proverka(data->rk->shetu.ravno(),rekin.shetu.ravno(),0,0) != 0)
      continue;
    if(data->rk->sost_ob == 1)
     if(rekin.soso == 1 || rekin.soso == 2)
       continue;
    if(data->rk->sost_ob == 2)
     if(rekin.soso == 0 || rekin.soso == 3)
       continue;
    if(iceb_u_proverka(data->rk->hau.ravno(),rekin.hau.ravno(),0,0) != 0)
     continue;
    hnap.new_plus(rekin.hna.ravno());         
    ppkf=rekin.popkf;
   }

  if(in1 != innom)
   {
    if(mvr > 0 && in1 != 0)
     {
      rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff);
      mvr=0;
     }
    in1=innom;

    for(int i=0; i< 12; i++)
     maot[i]=0.;


    if(innom > 0)
     {
      /*Читаем наименование объекта*/
      sprintf(strsql,"select naim from Uosin where innom=%d",innom);
      if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
       {
        sprintf(strsql,"%s %d !",
        gettext("Не найден инвентарный номер"),innom);
        iceb_menu_soob(strsql,data->window);
        naios.new_plus("");
       }
      else
       naios.new_plus(row1[0]);

     }
    else
     {
//      sprintf(bros,"Поправочный коэффициент для группы %d",innom*-1);
      bros.new_plus("Поправочный коэффициент для группы ");
      bros.plus(innom*-1);
      
      if(iceb_poldan(bros.ravno(),&bros1,filnast.ravno(),data->window ) != 0)
       {
        sprintf(strsql,"Не найден %s !!!\nВ файле %s",bros.ravno(),filnast.ravno());
        iceb_menu_soob(strsql,data->window);
       }
      ppkf=bros1.ravno_atof(); 
     } 

    /*Читаем коэффициент*/

    if(innom < 0)
     hnap.new_plus(innom*(-1));


    sprintf(strsql,"select naik,kof from Uosgrup where kod='%s'",hnap.ravno());

    kof=0;
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найдено группу"),bros.ravno());
      iceb_menu_soob(strsql,data->window); 
      continue;
     }
    else
     {
      kof=atof(row1[1]);
      if(innom < 0)
       naios.new_plus(row1[0]);
     }

    /*На начало периода*/
    double bs=0.,iz=0.;
    if(innom > 0)
     {
      class bsizw_data bal_st;
      bsizw(innom,pod1,1,mn,gn,&bal_st,NULL,data->window);
      
      bs=bal_st.sbs+bal_st.bs;
/**********
      if(bs == 0.) //Произошел расход
       {
        continue;      
       }
************/
      iz=bal_st.iz+bal_st.iz1+bal_st.siz;
     }

    if(innom < 0)
     {
//      sprintf(bros,"Стартовая балансовая стоимость для группы %d",innom*-1);
      bros.new_plus("Стартовая балансовая стоимость для группы ");
      bros.plus(innom*-1);
      if(iceb_poldan(bros.ravno(),&bros1,filnast.ravno(),data->window) != 0)
       {
        sprintf(strsql,"Не найдена %s",bros.ravno());
        iceb_menu_soob(strsql,data->window);
       }
      bs=bros1.ravno_atof();
//      sprintf(bros,"Стартовый износ для группы %d",innom*-1);
      bros.new_plus("Стартовый износ для группы ");
      bros.plus(innom*-1);
      if(iceb_poldan(bros.ravno(),&bros1,filnast.ravno(),data->window) != 0)
       {
        sprintf(strsql,"Не найден %s",bros.ravno());
        iceb_menu_soob(strsql,data->window);
       }

      iz=bros1.ravno_atof();

      sprintf(strsql,"Группа %d bs=%f iz=%f\n",innom*-1,bs,iz);
      iceb_printw(strsql,data->buffer,data->view);

      double iz1=0.,iz11=0.,bs1=0.;
      bsiz23w(hnap.ravno(),1,mn,gn,&bs1,&iz1,&iz11,NULL,data->window);
      iz+=iz1+iz11;
      bs+=bs1;
      sprintf(strsql,"hnap=%s bs1=%f iz1=%f iz11=%f iz=%f\n",hnap.ravno(),bs1,iz1,iz11,iz);
      iceb_printw(strsql,data->buffer,data->view);
     }
    double lik_st=uosgetlsw(innom,1,mn,gn,0,data->window);
    if(uosprovarw(hnap.ravno(),0,data->window) == 1)
     {
      ostt=bs-iz;
      if(metkabo == 0)
       god=(bs-iz)*kof/100.;
      if(metkabo == 1)
        god=(bs-lik_st)*kof/100.;
     }
    else
     {
      ostt=bs-iz;
      god=(bs-lik_st)*kof/100.;
     }

    mes=0.;
    if(god != 0.)
      mes=god/12.;

    itg[0]+=bs;
    itg[1]+=iz;
    itg[2]+=ostt;
    itg[3]+=lik_st;
    itg[4]+=god;
    itg[5]+=mes;

    itg1[0]+=bs;
    itg1[1]+=iz;
    itg1[2]+=ostt;
    itg1[3]+=lik_st;
    itg1[4]+=god;
    itg1[5]+=mes;

    itg2[0]+=bs;
    itg2[1]+=iz;
    itg2[2]+=ostt;
    itg2[3]+=lik_st;
    itg2[4]+=god;
    itg2[5]+=mes;

    if(innom > 0)
     invn.new_plus(innom);
    else
     {
//      sprintf(invn,"%d %s",innom*(-1),gettext("группа"));
      invn.new_plus(innom*-1);
      invn.plus(gettext("группа"));
     }     

    shvam(mn,gn,mk,gk,&sli,&kst,mr,ff,bf);

    fprintf(ff,"%-8s|%-*.*s|%-*.*s|%-*s|",
    invn.ravno(),
    iceb_u_kolbait(40,naios.ravno()),iceb_u_kolbait(40,naios.ravno()),naios.ravno(),
    iceb_u_kolbait(10,rekin.nomz.ravno()),iceb_u_kolbait(10,rekin.nomz.ravno()),rekin.nomz.ravno(),
    iceb_u_kolbait(5,hnap.ravno()),hnap.ravno());
    
    if(mr == 1)
      fprintf(ff,"%5.2f|%10.2f|%10.2f|%10.2f|%10.2f|%12.2f|%10.2f|%5.2f|",
      kof,bs,iz,ostt,lik_st,god,mes,ppkf);
    kolos++;
   }
  mvr++;

  maot[m-1]=sao;
  maotp[m-1]=maotp[m-1]+sao;
  maotph[m-1]=maotph[m-1]+sao;
  maotpo[m-1]=maotpo[m-1]+sao;

 }

rasmes(mn,mk,maot,ostt,hnaz1.ravno(),ff);

if(uosprovgrw(hnaz1.ravno(),data->window) != 1)
   itam(mn,gn,mk,gk,itg1,naipod.ravno(),mr,maotp,ff,ffi,&sli,&kst,bf);
itamh(mn,gn,mk,gk,itg2,hnaz1.ravno(),naimhz.ravno(),mr,maotph,ff,ffi,0,&sli,&kst,bf);
itamo(mn,mk,itg,mr,maotpo,ff,ffi);

if(mpg == 1)
 fprintf(ff,gettext("* Остаточная стоимость меньше (равняется) 100 необлагаемых минимумов.\n"));

sprintf(strsql,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
iceb_printw(strsql,data->buffer,data->view);

fprintf(ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(ffi,"\n%s:%d\n",gettext("Количество основных средств"),kolos);

iceb_podpis(ff,data->window);
fclose(ff);

iceb_podpis(ffi,data->window);
fclose(ffi);



data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Ведомость амортизационных отчислений"));
data->rk->imaf.plus(imafi);
data->rk->naim.plus(gettext("Общий итог"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);









gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
