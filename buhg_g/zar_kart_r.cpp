/*$Id: zar_kart_r.c,v 1.25 2013/09/26 09:47:00 sasa Exp $*/
/*08.12.2016	07.12.2006	Белых А.И.	zar_kart_r.c
Распечатка карточек с начислениями и удержаниями за период
*/
#include <errno.h>
#include "buhg_g.h"
#include "zar_kart.h"

class zarkart_ov
 {
  public:
   class iceb_u_int MVT; /*коды видов табеля*/
   class iceb_u_double MVTRD; /*рабочие дни*/
   class iceb_u_double MVTHS; /*отработанные часы*/
   class iceb_u_double MVTKD; /*календарные дни*/
   int kolmes;
 };

class kk
 {
  public:
  class iceb_u_str fio;
  class iceb_u_str dolg;
  class iceb_u_str inn;
  int		podr;
  int		sovm;
  short		dn,mn,gn;
  short		dk,mk,gk;

  kk()
   {
    clear_data();
   }
  void clear_data()
   {
    fio.new_plus("");
    dolg.new_plus("");
    inn.new_plus("");
    podr=0;
    sovm=0;
    dn=mn=gn=0;
    dk=mk=gk=0;
   }
     
 };

class zar_kart_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class zar_kart_rek *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_kart_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_kart_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_kart_r_data *data);
gint zar_kart_r1(class zar_kart_r_data *data);
void  zar_kart_r_v_knopka(GtkWidget *widget,class zar_kart_r_data *data);


extern SQL_baza bd;





extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_kart_r(class zar_kart_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zar_kart_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать карточки начислений/удержаний по работникам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_kart_r_key_press),&data);

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

repl.plus(gettext("Распечатать карточки начислений/удержаний по работникам"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_kart_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zar_kart_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_kart_r_v_knopka(GtkWidget *widget,class zar_kart_r_data *data)
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

gboolean   zar_kart_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_kart_r_data *data)
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
/********************************/
/*Распечатка конкретного массива счет*/
/********************************/
void   zarkartprs(short mn,short gn,
int kolmes,
class iceb_u_spisok *MKN,
class iceb_u_double *MKND,
double *saldo,
double *saldob,
int metka, //1-начисления 2-удержания
int metkab, //1-бюджет 2-небюджет
FILE *ff)
{
//char		stroka[40+(kolmes+1)*9];
class iceb_u_str stroka;
short		d,m,g;
int		kon=0;
int		i,ii;
char		strsql[1024];
SQL_str		row;
class iceb_u_str naim("");
double		suma;
double		sumai[kolmes];
double		sumagor=0.,sumagori=0.;
class iceb_u_str bros("");
class iceb_u_str knah("");
class iceb_u_str shet("");
SQLCURSOR curr;
  

stroka.plus("-----------------------------------");

for(i=0 ; i < kolmes+1; i++)
  stroka.plus("---------");


fprintf(ff,"%s\n",stroka.ravno());
if(metka == 1)
  fprintf(ff,gettext("Код|Наименование начисления |Счёт |"));
if(metka == 2)
  fprintf(ff,gettext("Код|Наименование удержания  |Счёт |"));
d=1; m=mn; g=gn;

for(i=0 ; i < kolmes; i++)
 {
  fprintf(ff," %.02d.%d|",m,g);
  iceb_u_dpm(&d,&m,&g,3);
 }
fprintf(ff,"%*s|\n",iceb_u_kolbait(8,gettext("Итого")),gettext("Итого"));
memset(&sumai,'\0',sizeof(sumai));

fprintf(ff,"%s\n",stroka.ravno());
if(metka == 1)
 {
  fprintf(ff,"%*.*s|",
  iceb_u_kolbait(34,gettext("Сальдо начальное")),
  iceb_u_kolbait(34,gettext("Сальдо начальное")),
  gettext("Сальдо начальное"));
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
   { 
    if(metkab == 1)
     {
      fprintf(ff,"%8.2f|",saldob[ii]);
      suma+=saldob[ii];
     }
    if(metkab == 2)
     {
      fprintf(ff,"%8.2f|",saldo[ii]-saldob[ii]);
      suma+=saldo[ii]-saldob[ii];
     }
   }
  fprintf(ff,"%8.2f|\n",suma);
  fprintf(ff,"%s\n",stroka.ravno());
 }

kon=MKN->kolih();
for(i=0; i < kon; i++)
 {
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
    if((suma=MKND->ravno(kolmes*i+ii)) != 0.)
     break;
  if(suma == 0.)
    continue;

  bros.new_plus(MKN->ravno(i));
  iceb_u_polen(bros.ravno(),&knah,1,'|');
  iceb_u_polen(bros.ravno(),&shet,2,'|');

  if(metka == 1) //начисления
   if(metkab == 1) //бюджет
    if(iceb_u_proverka(shetb,shet.ravno(),0,0) != 0)
      continue;
  if(metka == 1) //начисления
   if(metkab == 2) //не бюджет
    if(iceb_u_proverka(shetb,shet.ravno(),0,0) == 0)
      continue;

  if(metka == 2) //удержания
  if(metkab == 1) //бюджет
    if(iceb_u_proverka(shetbu,shet.ravno(),0,0) != 0)
      continue;

  if(metka == 2) //удержания
  if(metkab == 2) //Не бюджет
    if(iceb_u_proverka(shetbu,shet.ravno(),0,0) == 0)
      continue;

    
  if(metka == 1)
    sprintf(strsql,"select naik from Nash where kod=%s",knah.ravno());
  if(metka == 2)
    sprintf(strsql,"select naik from Uder where kod=%s",knah.ravno());
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");
  
  sumagor=0.;
  fprintf(ff,"%3s %-*.*s|%-*s|",
  knah.ravno(),
  iceb_u_kolbait(24,naim.ravno()),iceb_u_kolbait(24,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(5,shet.ravno()),shet.ravno());
  
  for(ii=0 ; ii < kolmes; ii++)
   {
    suma=MKND->ravno(kolmes*i+ii);
    if(metka == 2)
     suma*=-1;
    sumai[ii]+=suma;
    sumagor+=suma;
    fprintf(ff,"%8.2f|",suma);
   }
  fprintf(ff,"%8.2f|\n",sumagor);
  sumagori+=sumagor;
 }
fprintf(ff,"%s\n",stroka.ravno());

if(metka == 1)
 fprintf(ff,"%*s|",iceb_u_kolbait(34,gettext("Итого начислено")),gettext("Итого начислено"));
if(metka == 2)
 fprintf(ff,"%*s|",iceb_u_kolbait(34,gettext("Итого удержано")),gettext("Итого удержано"));

for(ii=0 ; ii < kolmes; ii++)
 {
  fprintf(ff,"%8.2f|",sumai[ii]);
  if(metka == 1)
   {
    saldo[ii]+=sumai[ii];
    if(metkab == 1)
      saldob[ii]+=sumai[ii];
   }
  if(metka == 2)
   {
    saldo[ii]-=sumai[ii];
    if(metkab == 1)
      saldob[ii]-=sumai[ii];
   }
 }
fprintf(ff,"%8.2f|\n\n",sumagori);

if(metka == 2)
 {
  fprintf(ff,"%s\n",stroka.ravno());
  fprintf(ff,"%*.*s|",
  iceb_u_kolbait(34,gettext("Сальдо конечное")),
  iceb_u_kolbait(34,gettext("Сальдо конечное")),
  gettext("Сальдо конечное"));
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
   { 
    if(metkab == 1)
     {
      fprintf(ff,"%8.2f|",saldob[ii]);
      suma+=saldo[ii];
     }
    if(metkab == 2)
     {
      fprintf(ff,"%8.2f|",saldo[ii]-saldob[ii]);
      suma+=saldo[ii]-saldob[ii];
     }
   }
  fprintf(ff,"%8.2f|\n",suma);
  fprintf(ff,"%s\n",stroka.ravno());
 }

}

/********************************/
/*Распечатка конкретного массива*/
/********************************/
void zarkartpr(short mn,short gn,
int kolmes,
class iceb_u_int *MKN,
class iceb_u_double *MKND,
double *saldo,
int metka, //1-начисления 2-удержания
FILE *ff)
{
class iceb_u_str stroka;
short		d,m,g;
int		kon=0;
int		i,ii;
int		nome=0;
char		strsql[1024];
SQL_str		row;
class iceb_u_str naim("");
double		suma;
double		sumai[kolmes];
double		sumagor=0.,sumagori=0.;
SQLCURSOR curr;

stroka.plus("-----------------------------------");

for(i=0 ; i < kolmes+1; i++)
  stroka.plus("---------");


fprintf(ff,"%s\n",stroka.ravno());
if(metka == 1)
  fprintf(ff,gettext("Код|   Наименование начисления    |"));
if(metka == 2)
  fprintf(ff,gettext("Код|   Наименование удержания     |"));
d=1; m=mn; g=gn;

for(i=0 ; i < kolmes; i++)
 {
  fprintf(ff," %.02d.%d|",m,g);
  iceb_u_dpm(&d,&m,&g,3);
 }
fprintf(ff,"%*s|\n",iceb_u_kolbait(8,gettext("Итого")),gettext("Итого"));
memset(&sumai,'\0',sizeof(sumai));

fprintf(ff,"%s\n",stroka.ravno());
if(metka == 1)
 {
  fprintf(ff,"%*.*s|",
  iceb_u_kolbait(34,gettext("Сальдо начальное")),iceb_u_kolbait(34,gettext("Сальдо начальное")),gettext("Сальдо начальное"));
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
   { 
    fprintf(ff,"%8.2f|",saldo[ii]);
    suma+=saldo[ii];
   }
  fprintf(ff,"%8.2f|\n",suma);
  fprintf(ff,"%s\n",stroka.ravno());
 }

kon=MKN->kolih();
for(i=0; i < kon; i++)
 {
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
    if((suma=MKND->ravno(kolmes*i+ii)) != 0.)
     break;
  if(suma == 0.)
    continue;

  nome=MKN->ravno(i);
  if(metka == 1)
    sprintf(strsql,"select naik from Nash where kod=%d",nome);
  if(metka == 2)
    sprintf(strsql,"select naik from Uder where kod=%d",nome);
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
   naim.new_plus(row[0]);      
  else
   naim.new_plus("");
  
  sumagor=0.;
  fprintf(ff,"%3d %-*.*s|",nome,iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno());
  for(ii=0 ; ii < kolmes; ii++)
   {
    suma=MKND->ravno(kolmes*i+ii);
    if(metka == 2)
     suma*=-1;
    sumai[ii]+=suma;
    sumagor+=suma;
    fprintf(ff,"%8.2f|",suma);
   }
  fprintf(ff,"%8.2f|\n",sumagor);
  sumagori+=sumagor;
 }
fprintf(ff,"%s\n",stroka.ravno());

if(metka == 1)
 fprintf(ff,"%*s|",iceb_u_kolbait(34,gettext("Итого начислено")),gettext("Итого начислено"));
if(metka == 2)
 fprintf(ff,"%*s|",iceb_u_kolbait(34,gettext("Итого удержано")),gettext("Итого удержано"));

for(ii=0 ; ii < kolmes; ii++)
 {
  fprintf(ff,"%8.2f|",sumai[ii]);
  if(metka == 1)
   saldo[ii]+=sumai[ii];
  if(metka == 2)
   saldo[ii]-=sumai[ii];
 }
fprintf(ff,"%8.2f|\n\n",sumagori);

if(metka == 2)
 {
  fprintf(ff,"%s\n",stroka.ravno());
  fprintf(ff,"%*.*s|",
  iceb_u_kolbait(34,gettext("Сальдо конечное")),iceb_u_kolbait(34,gettext("Сальдо конечное")),gettext("Сальдо конечное"));
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
   { 
    fprintf(ff,"%8.2f|",saldo[ii]);
    suma+=saldo[ii];
   }
  fprintf(ff,"%8.2f|\n",suma);
  fprintf(ff,"%s\n",stroka.ravno());
 }

}
/************************/
/*Распечатка содержимого*/
/************************/
void zarkartp(long tabn,
class kk *kart,
short mn,short gn,
int kolmes,
class iceb_u_int *MKN,
class iceb_u_int *MKU,
class iceb_u_double *MKND,
class iceb_u_double *MKUD,
double *saldon,
class iceb_u_spisok *MKNS,
class iceb_u_double *MKNSD,
double *saldono1,
double *saldonb1,
class iceb_u_spisok *MKUS,
class iceb_u_double *MKUSD,
double *saldono2,
double *saldonb2,
int nomer_zap,
FILE *ff,
FILE *ffb,
FILE *ffo,
FILE *ff_spiso,
class zar_kart_r_data *data)
{
char strsql[1024];
sprintf(strsql,"\n%5ld %s\n",tabn,kart->fio.ravno());
iceb_printw(strsql,data->buffer,data->view);


fprintf(ff,"%s\n%s: %ld %s\n",iceb_get_pnk("00",0,data->window),gettext("Табельный номер"),
tabn,kart->fio.ravno());

fprintf(ff_spiso,"%6d %8ld %10s %s\n",nomer_zap,tabn,kart->inn.ravno(),kart->fio.ravno());

fprintf(ff,"%s:%s\n",gettext("Индивидуальный налоговый номер"),
kart->inn.ravno());
fprintf(ff,"%s:%s\n",gettext("Должность"),kart->dolg.ravno());
if(kart->sovm == 1)
  fprintf(ff,"%s\n",gettext("Совместитель"));

if(kart->dn != 0)
  fprintf(ff,"%s:%02d.%02d.%d%s\n",gettext("Дата начала работы"),
  kart->dn,kart->mn,kart->gn,gettext("г."));

if(kart->dk != 0)
  fprintf(ff,"%s:%02d.%02d.%d%s\n",gettext("Дата увольнения"),
  kart->dk,kart->mk,kart->gk,gettext("г."));
  
zarkartpr(mn,gn,kolmes,MKN,MKND,saldon,1,ff);
zarkartpr(mn,gn,kolmes,MKU,MKUD,saldon,2,ff);

if(shetb != NULL &&  shetbu != NULL)
 {
  fprintf(ffb,"%s: %ld %s\n",gettext("Табельный номер"),
  tabn,kart->fio.ravno());

  fprintf(ffb,"%s:%s\n",gettext("Индивидуальный налоговый номер"),
  kart->inn.ravno());
  fprintf(ffb,"%s:%s\n",gettext("Должность"),kart->dolg.ravno());
  if(kart->sovm == 1)
    fprintf(ffb,"%s\n",gettext("Совместитель"));

  if(kart->dn != 0)
    fprintf(ffb,"%s:%02d.%02d.%d%s\n",gettext("Дата начала работы"),
    kart->dn,kart->mn,kart->gn,gettext("г."));

  if(kart->dk != 0)
    fprintf(ffb,"%s:%02d.%02d.%d%s\n",gettext("Дата увольнения"),
    kart->dk,kart->mk,kart->gk,gettext("г."));
  fprintf(ffb,"%s.\n",gettext("Бюджет"));
  zarkartprs(mn,gn,kolmes,MKNS,MKNSD,saldono1,saldonb1,1,1,ffb);
  zarkartprs(mn,gn,kolmes,MKUS,MKUSD,saldono1,saldonb1,2,1,ffb);

  fprintf(ffo,"%s: %ld %s\n",gettext("Табельный номер"),
  tabn,kart->fio.ravno());

  fprintf(ffo,"%s:%s\n",gettext("Индивидуальный налоговый номер"),
  kart->inn.ravno());
  fprintf(ffo,"%s:%s\n",gettext("Должность"),kart->dolg.ravno());
  if(kart->sovm == 1)
    fprintf(ffo,"%s\n",gettext("Совместитель"));

  if(kart->dn != 0)
    fprintf(ffo,"%s:%02d.%02d.%d%s\n",gettext("Дата начала работы"),
    kart->dn,kart->mn,kart->gn,gettext("г."));

  if(kart->dk != 0)
    fprintf(ffo,"%s:%02d.%02d.%d%s\n",gettext("Дата увольнения"),
    kart->dk,kart->mk,kart->gk,gettext("г."));
  fprintf(ffo,"%s.\n",gettext("Хозрасчёт"));
  zarkartprs(mn,gn,kolmes,MKNS,MKNSD,saldono2,saldonb2,1,2,ffo);
  zarkartprs(mn,gn,kolmes,MKUS,MKUSD,saldono2,saldonb2,2,2,ffo);

  MKNSD->clear_class();
  MKUSD->clear_class();
 }

}
/***********************************/
/*Определение отработанного времени*/
/***********************************/

void	vidtab(short mr,short gr,long tabnom,
class iceb_u_int *MVT,
class iceb_u_double *MVTRD,
class iceb_u_double *MVTHS,
class iceb_u_double *MVTKD,
int nommes,
int kolmes,
GtkWidget *wpredok)
{
char		strsql[1024];
SQL_str		row;
int		kolstr;
int		kodt=0;
int		nomq=0;

sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where god=%d and \
mes=%d and tabn=%ld",gr,mr,tabnom);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 return;

while(cur.read_cursor(&row) != 0)
 {
  kodt=atoi(row[0]);
  nomq=MVT->find(kodt);
  MVTRD->plus(atof(row[1]),kolmes*nomq+nommes-1);
  MVTHS->plus(atof(row[2]),kolmes*nomq+nommes-1);
  MVTKD->plus(atof(row[3]),kolmes*nomq+nommes-1);
 }

}
/**********************************/
/*Распечатка отработанного времени*/
/**********************************/
void rasotrvr(short mn,short gn,int kolmes,
class iceb_u_int *MVT,
class iceb_u_double *MVTRD,
int metka, //1-рабочие дни 2-часы 3-календарные дни
FILE *ff)
{
class iceb_u_str stroka;
short		d,m,g;
double		sumai[kolmes];
double		sumagor=0.,sumagori=0.;
int		i=0,ii=0;
int		koltb;
double		suma=0.;
char		strsql[1024];
SQL_str		row;
class iceb_u_str naim("");
int		nome;
SQLCURSOR curr;

koltb=MVT->kolih();
for(i=0; i < koltb*kolmes; i++)
 if((suma=MVTRD->ravno(i)) != 0.)
     break;
if(suma == 0.)
  return;
suma=0.;  
if(metka == 1)
 fprintf(ff,"%s:\n",gettext("Свод отработанных рабочих дней"));
if(metka == 2)
 fprintf(ff,"%s:\n",gettext("Свод отработанных часов"));
if(metka == 3)
 fprintf(ff,"%s:\n",gettext("Свод календарных дней"));

stroka.plus("-----------------------------------");

for(i=0 ; i < kolmes+1; i++)
  stroka.plus("--------");


fprintf(ff,"%s\n",stroka.ravno());
fprintf(ff,gettext("Код|   Наименование табеля        |"));

d=1; m=mn; g=gn;

for(i=0 ; i < kolmes; i++)
 {
  fprintf(ff,"%.02d.%d|",m,g);
  iceb_u_dpm(&d,&m,&g,3);
 }
fprintf(ff,"%*s|\n",iceb_u_kolbait(7,gettext("Итого")),gettext("Итого"));
fprintf(ff,"%s\n",stroka.ravno());
memset(&sumai,'\0',sizeof(sumai));


for(i=0; i < koltb; i++)
 {
  suma=0.;
  for(ii=0 ; ii < kolmes; ii++)
    if((suma=MVTRD->ravno(kolmes*i+ii)) != 0.)
     break;
  if(suma == 0.)
    continue;

  nome=MVT->ravno(i);
  sprintf(strsql,"select naik from Tabel where kod=%d",nome);
  if(sql_readkey(&bd,strsql,&row,&curr) > 0)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");
  
  sumagor=0.;
  fprintf(ff,"%3d %-*.*s|",nome,iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno());
  for(ii=0 ; ii < kolmes; ii++)
   {
    suma=MVTRD->ravno(kolmes*i+ii);
    sumai[ii]+=suma;
    sumagor+=suma;
    fprintf(ff,"%7.f|",suma);
   }
  fprintf(ff,"%7.f|\n",sumagor);
  sumagori+=sumagor;
 }
fprintf(ff,"%s\n",stroka.ravno());

fprintf(ff,"%*s|",iceb_u_kolbait(34,gettext("Итого")),gettext("Итого"));

for(ii=0 ; ii < kolmes; ii++)
 {
  fprintf(ff,"%7.f|",sumai[ii]);
 }
fprintf(ff,"%7.f|\n\n",sumagori);

}
/***********************************/
/*распечатка типовой формы 6*/
/*********************************/
void zarkart_tf6(int tabn,
short mn,short gn,
short mk,short gk,
int kolmes,
class iceb_u_int *MKN,
class iceb_u_double *MKND,
class iceb_u_int *MKU,
class iceb_u_double *MKUD,
class zarkart_ov *otr_vrem,
FILE *ff,
GtkWidget *wpredok)
{
double suma=0.;
class iceb_u_int kod_nah;
class iceb_u_int kod_uder;
class iceb_u_double suma_nah;
class iceb_u_double suma_uder;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str naim("");

int kon=MKN->kolih();

for(int i=0; i < kon; i++)
 {
  suma=0.;

  for(int ii=0 ; ii < kolmes; ii++)
    suma+=MKND->ravno(kolmes*i+ii);

  if(suma == 0.)
    continue;
  kod_nah.plus(MKN->ravno(i));
  suma_nah.plus(suma);
 }

kon=MKU->kolih();

for(int i=0; i < kon; i++)
 {
  suma=0.;
  for(int ii=0 ; ii < kolmes; ii++)
    suma+=MKUD->ravno(kolmes*i+ii);
  if(suma == 0.)
    continue;
  kod_uder.plus(MKN->ravno(i));
  suma_uder.plus(suma);
 }

class iceb_u_str naim_kontr(iceb_get_pnk("00",0,wpredok));

fprintf(ff,"%-*s Типова форма N П-6\n",iceb_u_kolbait(60,naim_kontr.ravno()),naim_kontr.ravno());

fprintf(ff,"%60s ЗАТВЕРДЖЕНО\n","");
fprintf(ff,"%60s наказом Деркомстату України\n","");
fprintf(ff,"%60s від 5 грудня 2008 р. N 489\n\n","");

class iceb_u_str fio;
class iceb_u_str dolg("");
class iceb_u_str indkod("");
  
fprintf(ff,"%30sРОЗРАХУНКОВО-ПЛАТІЖНА ВІДОМІСТЬ ПРАЦІВНИКА\n","");


sprintf(strsql,"select fio,dolg,inn from Kartb where tabn=%d",tabn);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  fio.new_plus(row[0]);
  dolg.new_plus(row[1]);
  indkod.new_plus(row[2]);
 }

fprintf(ff,"\x1B\x45"); //режим выделенного шрифта включён
fprintf(ff,"\x0E"); /*Включение режима удвоенной ширины*/
fprintf(ff,"%14s %s\n","",fio.ravno());
fprintf(ff,"\x14"); /*Выключение режима удвоенной ширины*/
fprintf(ff,"\x1B\x46"); //режим выделенного шрифта выключен

fprintf(ff,"%30s Табельний номер:%d Ідентифікаційний код:%s\n","",tabn,indkod.ravno());
fprintf(ff,"%30s Посада:%s\n","",dolg.ravno());

short d;
iceb_u_dpm(&d,&mk,&gk,5);

fprintf(ff,"%30sза період з 01.%02d.%d р. по %02d.%02d.%d р.\n","",mn,gn,d,mk,gk);

double suma_str[3];

class iceb_u_str naim_tabel("");
for(int nom_kod_tab=0; nom_kod_tab < otr_vrem->MVT.kolih(); nom_kod_tab++)
 {
   suma=0.;
   for(int nom_mes=0 ; nom_mes < otr_vrem->kolmes; nom_mes++)
     suma+=otr_vrem->MVTRD.ravno(otr_vrem->kolmes*nom_kod_tab+nom_mes)+otr_vrem->MVTHS.ravno(otr_vrem->kolmes*nom_kod_tab+nom_mes)+otr_vrem->MVTKD.ravno(otr_vrem->kolmes*nom_kod_tab+nom_mes);

   if(suma == 0.)
     continue;

   suma_str[0]=suma_str[1]=suma_str[2]=0.;

   for(int nom_mes=0 ; nom_mes < otr_vrem->kolmes; nom_mes++)
    {
     suma_str[0]+=otr_vrem->MVTRD.ravno(otr_vrem->kolmes*nom_kod_tab+nom_mes);
     suma_str[1]+=otr_vrem->MVTHS.ravno(otr_vrem->kolmes*nom_kod_tab+nom_mes);
     suma_str[2]+=otr_vrem->MVTKD.ravno(otr_vrem->kolmes*nom_kod_tab+nom_mes);
    }


  /*Читаем наименование табеля*/
  sprintf(strsql,"select naik from Tabel where kod=%d",otr_vrem->MVT.ravno(nom_kod_tab));

  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код табеля"),otr_vrem->MVT.ravno(nom_kod_tab));
    iceb_menu_soob(strsql,wpredok);
    naim_tabel.new_plus("");
    continue;
   }
  else
    naim_tabel.new_plus(row[0]);
  



  fprintf(ff,"%*.*s: %4.4g/%6.6g/%4.4g\n",
  iceb_u_kolbait(40,naim_tabel.ravno()),  
  iceb_u_kolbait(40,naim_tabel.ravno()),  
  naim_tabel.ravno(),
  suma_str[0],
  suma_str[1],
  suma_str[2]);
 }
double saldo_start_b=0.;

double saldo_start=zarsaldw(1,mn,gn,tabn,&saldo_start_b,wpredok);
fprintf(ff,"Caльдо на 01.%02d.%d: %.2f\n",mn,gn,saldo_start);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n\
| N |    Нараховано за видами оплат                 | N |    Утримано                                   |\n\
|п/п|-----------------------------------------------|п/п|-----------------------------------------------|\n\
|   |Код|вид оплати                    | нараховано |   |Код|вид утримання                 |  утримано  |\n\
|   |   |                              |    грн.    |   |   |                              |    грн.    |\n\
---------------------------------------------------------------------------------------------------------\n");

int kolih_nah=kod_nah.kolih();
int kolih_uder=kod_uder.kolih();

int kol=kolih_nah;
if(kolih_nah < kolih_uder)
 kol=kolih_uder;


double itogo_n=0;
double itogo_u=0;
for(int nom=0; nom < kol; nom++)
 {
  class iceb_u_spisok sn;
  class iceb_u_spisok su;
  
  if(nom < kolih_nah)
   {
    sprintf(strsql,"select naik from Nash where kod=%d",kod_nah.ravno(nom));

    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
      naim.new_plus(row[0]);
    else
      naim.new_plus("");

    sprintf(strsql,"|%3d|%3d|%-*.*s|%12.2f|",
    nom+1,
    kod_nah.ravno(nom),
    iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
    suma_nah.ravno(nom));

    sn.plus(strsql);
    for(int nom1=30; nom1 < iceb_u_strlen(naim.ravno()); nom1+=30)
     {
      sprintf(strsql,"|%3s|%3s|%-*.*s|%12s|",
      "",
      "",
      iceb_u_kolbait(30,iceb_u_adrsimv(nom1,naim.ravno())),
      iceb_u_kolbait(30,iceb_u_adrsimv(nom1,naim.ravno())),
      iceb_u_adrsimv(nom1,naim.ravno()),
      "");
  
      sn.plus(strsql);
     }   

    itogo_n+=suma_nah.ravno(nom);
   
       
   }  
  else
   {
    sprintf(strsql,"|%3s|%3s|%30s|%12s|","","","",""); sn.plus(strsql);
    sn.plus(strsql);
   }

  if(nom < kolih_uder)
   {
    sprintf(strsql,"select naik from Uder where kod=%d",kod_uder.ravno(nom));
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim.new_plus(row[0]);
    else
     naim.new_plus("");

    sprintf(strsql,"%3d|%3d|%-*.*s|%12.2f|",
    nom+1,
    kod_uder.ravno(nom),
    iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
    suma_uder.ravno(nom));

    su.plus(strsql);

    for(int nom1=30; nom1 < iceb_u_strlen(naim.ravno()); nom1+=30)
     {
      sprintf(strsql,"%3s|%3s|%-*.*s|%12s|",
      "",
      "",
      iceb_u_kolbait(30,iceb_u_adrsimv(nom1,naim.ravno())),
      iceb_u_kolbait(30,iceb_u_adrsimv(nom1,naim.ravno())),
      iceb_u_adrsimv(nom1,naim.ravno()),
      "");
  
      su.plus(strsql);
     }   
    itogo_u+=suma_uder.ravno(nom);



   }  
  else
   {
    sprintf(strsql,"%3s|%3s|%30s|%12s|","","","","");
    su.plus(strsql);    
   } 

//  fprintf(ff,"\n");
  int kolstrn=sn.kolih();
  int kolstru=su.kolih();
  int kolstr=kolstrn;
  if(kolstrn < kolstru)
   kolstr=kolstru;
  for(int nom1=0; nom1 < kolstr; nom1++)
   {
    if(nom1 < kolstrn)
     fprintf(ff,"%s",sn.ravno(nom1));
    else
     fprintf(ff,"|%3s|%3s|%30s|%12s|","","","","");
    if(nom1 < kolstru)
     fprintf(ff,"%s\n",su.ravno(nom1));
    else
     fprintf(ff,"%3s|%3s|%30s|%12s|\n","","","","");
   }   

  fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n");

 }
fprintf(ff,"|%*s|%12.2f|%*s|%12.2f|\n",
iceb_u_kolbait(38,"Разом"),
"Разом",
itogo_n,
iceb_u_kolbait(38,"Разом"),
"Разом",
itogo_u);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"Cальдо на %02d.%02d.%d: %.2f\n",d,mk,gk,saldo_start+itogo_n+itogo_u);

class iceb_fioruk_rk gb;

iceb_fioruk(2,&gb,wpredok);


//fprintf(ff,"\n%20sГоловний бухгалтер __________________________%s\n","",gb.fio.ravno());
fprintf(ff,"\n%20s         Бухгалтер __________________________%s\n","",iceb_getfioop(wpredok));

iceb_podpis(ff,wpredok);


}

/******************************************/
/******************************************/

gint zar_kart_r1(class zar_kart_r_data *data)
{
time_t tmmn;
time(&tmmn);
char strsql[1024];
class iceb_clock sss(data->window);



int	        kon=0,kou=0;
SQL_str		row,row1;
int		kolstr=0;
short		prn=0;
int		knah=0;
short		d,m,g;
double		suma;
int		nommes=0;
//int		kolmes=0;
int		tabn=0,tabnz=0;
short		mesqc=0;
int		nomq=0;
FILE		*ff,*ffo=NULL,*ffb=NULL;
char		imaf[64],imafo[64],imafb[64];
char imaf_spiso[64];
class kk kart;
int		kolns=0,kolus=0;
short mn,gn;
short mk,gk;
iceb_u_rsdat1(&mn,&gn,data->rk->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->rk->datak.ravno());
if(mk == 0)
 {
  mk=mn;
  gk=gn;
 }
 
sprintf(strsql,"%d.%d => %d.%d\n",mn,gn,mk,gk);
iceb_printw(strsql,data->buffer,data->view);

/*Определяем количество начислений и удержаний*/
sprintf(strsql,"select kod from Nash order by kod asc");
SQLCURSOR cur;
SQLCURSOR cur1;
if((kon=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
if(kon == 0)
 {
  iceb_menu_soob(gettext("Не введены начисления !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_int MKN; //Массив кодов начислений
while(cur.read_cursor(&row) != 0)
  MKN.plus(atoi(row[0]),-1);

sprintf(strsql,"select kod from Uder order by kod asc");

if((kou=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kou == 0)
 {
  iceb_menu_soob(gettext("Не введены удержания !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_int MKU(0); //Массив кодов удержаний

while(cur.read_cursor(&row) != 0)
  MKU.plus(atoi(row[0]),-1);

class iceb_u_double MKND; //Данные начислений
class iceb_u_double MKUD; //Данные удержаний

class zarkart_ov otr_vrem;
otr_vrem.kolmes=iceb_u_period(1,mn,gn,30,mk,gk,1);

MKND.make_class(kon*otr_vrem.kolmes);
MKUD.make_class(kou*otr_vrem.kolmes);
sprintf(strsql,"select kod from Tabel order by kod asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s !",gettext("Не введён список видов табелей"));
  iceb_menu_soob(strsql,data->window);
 }


while(cur.read_cursor(&row) != 0)
  otr_vrem.MVT.plus(atoi(row[0]),-1);

otr_vrem.MVTRD.make_class(kolstr*otr_vrem.kolmes);
otr_vrem.MVTHS.make_class(kolstr*otr_vrem.kolmes);
otr_vrem.MVTKD.make_class(kolstr*otr_vrem.kolmes);

class iceb_u_spisok MKNS; //массив начислений/счет
class iceb_u_spisok MKUS; //массив удержаний/счет
class iceb_u_double MKNSD; //данные начислений/счет
class iceb_u_double MKUSD; //данные удержания/счет

memset(imafo,'\0',sizeof(imafo));
memset(imafb,'\0',sizeof(imafb));

if(shetb != NULL &&  shetbu != NULL)
 {

  sprintf(strsql,"select distinct prn,knah,shet from Zarp where \
datz >= '%04d-%d-1' and datz <= '%04d-%d-31' and suma <> 0.",gn,mn,gk,mk);

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

  while(cur.read_cursor(&row) != 0)
   {
    prn=atoi(row[0]);
    sprintf(strsql,"%s|%s",row[1],row[2]);
    if(prn == 1)
     { 
      MKNS.plus(strsql);
      kolns++;
     }
    if(prn == 2)
     {
      MKUS.plus(strsql);
      kolus++;
     }
   }
  MKNSD.make_class(kolns*otr_vrem.kolmes);
  MKUSD.make_class(kolus*otr_vrem.kolmes);

  sprintf(imafo,"krb%d-%d_%d.lst",mn,mk,getpid());

  if((ffo = fopen(imafo,"w")) == NULL)
   {
    iceb_er_op_fil(imafo,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  sprintf(imafb,"krn%d-%d_%d.lst",mn,mk,getpid());

  if((ffb = fopen(imafb,"w")) == NULL)
   {
//    error_op_nfil(imafb,errno,"");
    iceb_er_op_fil(imafb,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }
  
 }

class iceb_tmptab tabtmp;
const char *imatmptab={"zarkart"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
fio char(112) not null,\
datz DATE not null,\
tbn int not null,\
prn smallint not null,\
knah int not null,\
suma double(14,2) not null,\
shet char(32) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  return(1);
 }  

sprintf(strsql,"select datz,tabn,prn,knah,suma,shet from Zarp where \
datz >= '%d-%d-1' and datz <= '%d-%d-31' and suma <> 0. \
order by tabn,datz asc",gn,mn,gk,mk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_u_str fio("");
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  fio.new_plus("");
  sprintf(strsql,"select fio from Kartb where tabn=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    fio.new_plus(row1[0]);

  sprintf(strsql,"insert into %s values ('%s','%s',%s,%s,%s,%s,'%s')",
  imatmptab,
  fio.ravno_filtr(),row[0],row[1],row[2],row[3],row[4],row[5]);

  iceb_sql_zapis(strsql,1,0,data->window);

 }

sprintf(strsql,"select * from %s order by fio asc,tbn asc,datz asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }



sprintf(imaf,"krt%d-%d_%d.lst",mn,mk,getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
//    error_op_nfil(imaf,errno,"");
    iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(imaf_spiso,"krtso%d-%d_%d.lst",mn,mk,getpid());
FILE *ff_spiso;
if((ff_spiso = fopen(imaf_spiso,"w")) == NULL)
 {
//  error_op_nfil(imaf_spiso,errno,"");
  iceb_er_op_fil(imaf_spiso,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf_tf6[64];
sprintf(imaf_tf6,"tf6%d.lst",getpid());

class iceb_fopen tf6;
if(tf6.start(imaf_tf6,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 } 
short dkm=1;
iceb_u_dpm(&dkm,&mk,&gk,5);

iceb_zagolov(gettext("Список работников"),1,mn,gn,dkm,mk,gk,ff_spiso,data->window);

fprintf(ff_spiso,"\
---------------------------------------------------\n");

fprintf(ff_spiso,gettext("\
  N   |Таб.ном.|Инд.нал.но| Фамилия  Имя  Отчество\n"));
fprintf(ff_spiso,"\
---------------------------------------------------\n");


double	saldon[otr_vrem.kolmes];
double	saldono1[otr_vrem.kolmes];
double	saldonb1[otr_vrem.kolmes];
double	saldono2[otr_vrem.kolmes];
double	saldonb2[otr_vrem.kolmes];
memset(&saldon,'\0',sizeof(saldon));
memset(&saldono1,'\0',sizeof(saldono1));
memset(&saldonb1,'\0',sizeof(saldonb1));
memset(&saldono2,'\0',sizeof(saldono2));
memset(&saldonb2,'\0',sizeof(saldonb2));

double saldb=0;
kolstr1=0;
class iceb_u_str tabnom1("");
class iceb_u_str shet("");
SQLCURSOR curr;
int nomer_zap=0;
kolstr1=0;
//class iceb_u_str tabnom1("");

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  tabnom1.new_plus(row[2]);
  if(iceb_u_proverka(data->rk->tabnom.ravno(),tabnom1.ravno(),0,0) != 0)
    continue;
  tabn=atol(tabnom1.ravno());

  iceb_u_rsdat(&d,&m,&g,row[1],2);    

  if(tabnz != tabn)
   {
    if(tabnz != 0)
     {
      zarkartp(tabnz,&kart,mn,gn,otr_vrem.kolmes,&MKN,&MKU,&MKND,&MKUD,saldon,
      &MKNS,&MKNSD,saldono1,saldonb1,&MKUS,&MKUSD,saldono2,saldonb2,++nomer_zap,
      ff,ffb,ffo,ff_spiso,data);
      rasotrvr(mn,gn,otr_vrem.kolmes,&otr_vrem.MVT,&otr_vrem.MVTRD,1,ff);
      rasotrvr(mn,gn,otr_vrem.kolmes,&otr_vrem.MVT,&otr_vrem.MVTHS,2,ff);
      rasotrvr(mn,gn,otr_vrem.kolmes,&otr_vrem.MVT,&otr_vrem.MVTKD,3,ff);

      fprintf(ff,"\f");


      zarkart_tf6(tabnz,mn,gn,mk,gk,otr_vrem.kolmes,&MKN,&MKND,&MKU,&MKUD,&otr_vrem,tf6.ff,data->window);
      fprintf(tf6.ff,"\f");
      
      if(ffo != NULL)
       fprintf(ffo,"\f");
      if(ffb != NULL)
       fprintf(ffb,"\f");

      MKND.clear_class();
      MKUD.clear_class();
      otr_vrem.MVTRD.clear_class();
      otr_vrem.MVTHS.clear_class();
      otr_vrem.MVTKD.clear_class();

      memset(&saldon,'\0',sizeof(saldon));
      memset(&saldono1,'\0',sizeof(saldono1));
      memset(&saldonb1,'\0',sizeof(saldonb1));
      memset(&saldono2,'\0',sizeof(saldono2));
      memset(&saldonb2,'\0',sizeof(saldonb2));
     }
    kart.clear_data();
    sprintf(strsql,"select * from Kartb where tabn=%s",tabnom1.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) < 1)
     {
      sprintf(strsql,"%s %s!",gettext("Не найден табельный номер"),tabnom1.ravno());
      iceb_menu_soob(strsql,data->window);
     }  
    else
     {    
      if(iceb_u_proverka(data->rk->podr.ravno(),row1[4],0,0) != 0)
       {
        tabnz=0;
        continue;
       }
      if(iceb_u_proverka(data->rk->kod_kat.ravno(),row1[5],0,0) != 0)
       {
        tabnz=0;
        continue;
       }    
      kart.fio.new_plus(row1[1]);
      kart.dolg.new_plus(row1[2]);
      kart.podr=atoi(row1[4]);
      iceb_u_rsdat(&kart.dn,&kart.mn,&kart.gn,row1[6],2);
      iceb_u_rsdat(&kart.dk,&kart.mk,&kart.gk,row1[7],2);
      kart.inn.new_plus(row1[10]);
      kart.sovm=atoi(row1[9]);    
     }    
    tabnz=tabn;
    mesqc=0;
   }
  if(mesqc != m)
   {
    nommes=iceb_u_period(1,mn,gn,30,m,g,1);
    saldon[nommes-1]=zarsaldw(1,m,g,tabn,&saldb,data->window);
    saldono1[nommes-1]=saldon[nommes-1];
    saldonb1[nommes-1]=saldb;
    saldono2[nommes-1]=saldon[nommes-1];
    saldonb2[nommes-1]=saldb;
    vidtab(m,g,tabn,&otr_vrem.MVT,&otr_vrem.MVTRD,&otr_vrem.MVTHS,&otr_vrem.MVTKD,nommes,otr_vrem.kolmes,data->window);
    mesqc=m;
   }


  prn=atoi(row[3]);
  knah=atoi(row[4]);
  suma=atof(row[5]);
  shet.new_plus(row[6]);

  if(prn == 1)
   {
    nomq=MKN.find(knah);
    MKND.plus(suma,otr_vrem.kolmes*nomq+nommes-1);
    if(shetb != NULL &&  shetbu != NULL)
     {
      sprintf(strsql,"%d|%s",knah,shet.ravno());
      nomq=MKNS.find(strsql);
      MKNSD.plus(suma,otr_vrem.kolmes*nomq+nommes-1);
     }
   }
  if(prn == 2)
   {
    nomq=MKU.find(knah);
    MKUD.plus(suma,otr_vrem.kolmes*nomq+nommes-1);
    if(shetb != NULL &&  shetbu != NULL)
     {
      sprintf(strsql,"%d|%s",knah,shet.ravno());
      nomq=MKUS.find(strsql);
      MKUSD.plus(suma,otr_vrem.kolmes*nomq+nommes-1);
     }
   }
 }


if(tabnz != 0)
 {
  zarkartp(tabnz,&kart,mn,gn,otr_vrem.kolmes,&MKN,&MKU,&MKND,&MKUD,saldon,
  &MKNS,&MKNSD,saldono1,saldonb1,&MKUS,&MKUSD,saldono2,saldonb2,++nomer_zap,
  ff,ffb,ffo,ff_spiso,data);
  rasotrvr(mn,gn,otr_vrem.kolmes,&otr_vrem.MVT,&otr_vrem.MVTRD,1,ff);
  rasotrvr(mn,gn,otr_vrem.kolmes,&otr_vrem.MVT,&otr_vrem.MVTHS,2,ff);
  rasotrvr(mn,gn,otr_vrem.kolmes,&otr_vrem.MVT,&otr_vrem.MVTKD,3,ff);

  zarkart_tf6(tabnz,mn,gn,mk,gk,otr_vrem.kolmes,&MKN,&MKND,&MKU,&MKUD,&otr_vrem,tf6.ff,data->window);

 }

iceb_podpis(ff,data->window);
iceb_podpis(ff_spiso,data->window);
iceb_podpis(tf6.ff,data->window);

fclose(ff);
fclose(ff_spiso);
tf6.end();

if(shetb != NULL &&  shetbu != NULL)
 {
  iceb_podpis(ffo,data->window);

  iceb_podpis(ffb,data->window);

  fclose(ffo);
  fclose(ffb);
 }




data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Распечатка карточек с начислениями и удержаниями"));

data->rk->imaf.plus(imaf_tf6);
data->rk->naimf.plus(gettext("Типовая форма N6"));

data->rk->imaf.plus(imaf_spiso);
data->rk->naimf.plus(gettext("Распечатка списка работников"));

if(shetb != NULL &&  shetbu != NULL)
 {
  data->rk->imaf.plus(imafo);
  data->rk->imaf.plus(imafb);
  data->rk->naimf.plus(gettext("Распечатка карточек с начислениями и удержаниями (хозрасчёт)"));
  data->rk->naimf.plus(gettext("Распечатка карточек с начислениями и удержаниями (бюджет)"));
 }
  
for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(tmmn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
