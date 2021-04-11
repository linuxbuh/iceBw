/*$Id: ukrkontr_r.c,v 1.21 2013/09/26 09:46:56 sasa Exp $*/
/*25.03.2020	28.02.2008	Белых А.И.	ukrkontr_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "ukrkontr.h"

class ukrkontr_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class ukrkontr_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  ukrkontr_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   ukrkontr_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrkontr_r_data *data);
gint ukrkontr_r1(class ukrkontr_r_data *data);
void  ukrkontr_r_v_knopka(GtkWidget *widget,class ukrkontr_r_data *data);


extern SQL_baza bd;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int ukrkontr_r(class ukrkontr_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class ukrkontr_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт ведомости по контрагентам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(ukrkontr_r_key_press),&data);

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

repl.plus(gettext("Расчёт ведомости по контрагентам"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(ukrkontr_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)ukrkontr_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  ukrkontr_r_v_knopka(GtkWidget *widget,class ukrkontr_r_data *data)
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

gboolean   ukrkontr_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrkontr_r_data *data)
{
// printf("ukrkontr_r_key_press\n");
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}
/*********************/
/*Распечатка реквизитов поиска*/
/*******************************/
void ukrras_rekp(const char *kontrp,
const char *vkomp,
const char *kodras,
const char *kodgrpod,
const char *shetu,
FILE *ff)
{
if(kontrp[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код контрагента"),kontrp);
if(vkomp[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код вида командировки"),vkomp);
if(kodras[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код расхода"),kodras);
if(kodgrpod[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код группы подразделения"),kodgrpod);
if(shetu[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт"),shetu);

}

/***************************************/
/*Распечатка массивов*/
/*****************************/
void ukrras_rm(int metka, //0-распечатка сумм 1-распечатка количества
short dn,short mn,short gn,
short dk,short mk,short gk,
int kolkr,
int kolkont,
double *msum,
short *mkkr,
iceb_u_spisok *KONTR,
int *mkolkom,
const char *kontrp,
const char *vkomp,
const char *kodras,
const char *kodgrpod,
const char *shetu,
const char *imaf,
GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
SQLCURSOR curr;


FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

iceb_zagolov(gettext("Ведомость затрат по контрагентам"),dn,mn,gn,dk,mk,gk,ff,wpredok);

if(metka == 0)
 fprintf(ff,"%s\n",gettext("Распечатка сумм"));

if(metka == 1)
 fprintf(ff,"%s\n",gettext("Распечатка количества"));

ukrras_rekp(kontrp,vkomp,kodras,kodgrpod,shetu,ff);
 
 
class iceb_u_str naikont("");
class iceb_u_str naivkom("");
int	i1=0;
double  itogo=0.;
double  itogoo=0.;
double bb=0.;
class iceb_u_str strpod;
int     itkolkom=0;

strpod.plus("----------------------------------------");
for(int i=0; i < kolkr+2; i++)
 {
  if(i < kolkr)
   {
    bb=0.;
    for(i1=0; i1 < kolkont; i1++)
      bb+=msum[i1*kolkr+i];
    if(bb == 0.)
      continue;
   }
  strpod.plus("-----------");
 } 
fprintf(ff,"%s\n",strpod.ravno());
fprintf(ff,gettext("Код конт|  Фамилия, имя, отчество      |"));
fprintf(ff,"%-*.*s|",iceb_u_kolbait(10,gettext("Кол.коман.")),iceb_u_kolbait(10,gettext("Кол.коман.")),gettext("Кол.коман."));

for(int i=0; i < kolkr; i++)
 {
  if(i < kolkr)
   {
    bb=0.;
    for(i1=0; i1 < kolkont; i1++)
      bb+=msum[i1*kolkr+i];
    if(bb == 0.)
      continue;
   }
  sprintf(strsql,"select naim from Ukrkras where kod=%d",
  mkkr[i]);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   naivkom.new_plus(row[0]);
  else
   naivkom.new_plus("");
  fprintf(ff,"%-*.*s|",iceb_u_kolbait(10,naivkom.ravno()),iceb_u_kolbait(10,naivkom.ravno()),naivkom.ravno());
 }
fprintf(ff,"%-*.*s|\n",iceb_u_kolbait(10,gettext("Итого")),iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));
fprintf(ff,"%s\n",strpod.ravno());

for(int i=0; i < kolkont; i++)
 {
  //если по строке все равно нулю то строку не печатаем
  itogo=0.;
  for(i1=0; i1 < kolkr; i1++)
    itogo+=msum[i*kolkr+i1];
  if(itogo == 0.)
   continue;
   


  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
  KONTR->ravno(i));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   naikont.new_plus(row[0]);
  else
   naikont.new_plus("");
     
  fprintf(ff,"%*s %-*.*s %10d",
  iceb_u_kolbait(8,KONTR->ravno(i)),KONTR->ravno(i),
  iceb_u_kolbait(30,naikont.ravno()),iceb_u_kolbait(30,naikont.ravno()),naikont.ravno(),
  mkolkom[i]);
  
  itkolkom+=mkolkom[i];

  itogo=0.;
  for(i1=0; i1 < kolkr; i1++)
   {
    bb=0.;
    for(int i2=0; i2 < kolkont; i2++)
      bb+=msum[i2*kolkr+i1];
    if(bb == 0.)
      continue;
    if(msum[i*kolkr+i1] != 0.)
     fprintf(ff," %10.2f",msum[i*kolkr+i1]);
    else
     fprintf(ff," %10s","");
    itogo+=msum[i*kolkr+i1];
   }
  fprintf(ff," %10.2f\n",itogo);

 }
fprintf(ff,"%s\n",strpod.ravno());
fprintf(ff,"%*s: %10d",iceb_u_kolbait(38,gettext("Итого")),gettext("Итого"),itkolkom);

for(int i=0; i < kolkr; i++)
 {
  bb=0.;
  for(i1=0; i1 < kolkont; i1++)
    bb+=msum[i1*kolkr+i];
  if(bb == 0.)
    continue;
  itogo=0.;
  for(i1=0; i1 < kolkont; i1++)
    itogo+=msum[i1*kolkr+i];
  fprintf(ff," %10.2f",itogo);
  itogoo+=itogo;
 }

fprintf(ff," %10.2f\n",itogoo);

iceb_podpis(ff,wpredok);
fclose(ff);

}
/***************************/
/*Распечатка количество+сумма*/
/*****************************/
void ukrras_rmks(short dn,short mn,short gn,
short dk,short mk,short gk,
int kolkr,
int kolkont,
double *msum,
double *mkol,
short *mkkr,
class iceb_u_spisok *KONTR,
int *mkolkom,
const char *kontrp,
const char *vkomp,
const char *kodras,
const char *kodgrpod,
const char *shetu,
const char *imaf,
GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
SQLCURSOR curr;


FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }


iceb_zagolov(gettext("Ведомость затрат по контрагентам"),dn,mn,gn,dk,mk,gk,ff,wpredok);


ukrras_rekp(kontrp,vkomp,kodras,kodgrpod,shetu,ff);
 

class iceb_u_str naikont("");
class iceb_u_str naivkom("");
int	i1=0;
double  itogo=0.;
double  itogoo=0.;
double bb=0.;
class iceb_u_str strpod;
int     itkolkom=0;

strpod.plus("----------------------------------------------");
for(int i=0; i < kolkr+1; i++)
 {
  if(i < kolkr)
   {
    bb=0.;
    for(i1=0; i1 < kolkont; i1++)
      bb+=mkol[i1*kolkr+i];
    if(bb == 0.)
      continue;
   }
  strpod.plus("-----------------");
 } 

fprintf(ff,"%s\n",strpod.ravno());
fprintf(ff,gettext("Код конт|  Фамилия, имя, отчество      |"));

fprintf(ff,"%-*.*s|",
iceb_u_kolbait(5,gettext("Кол.")),
iceb_u_kolbait(5,gettext("Кол.")),
gettext("Кол."));


for(int i=0; i < kolkr; i++)
 {
  if(i < kolkr)
   {
    bb=0.;
    for(i1=0; i1 < kolkont; i1++)
      bb+=mkol[i1*kolkr+i];
    if(bb == 0.)
      continue;
   }
  sprintf(strsql,"select naim from Ukrkras where kod=%d",
  mkkr[i]);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   naivkom.new_plus(row[0]);
  else
   naivkom.new_plus("");
  fprintf(ff,"%-*.*s|",iceb_u_kolbait(16,naivkom.ravno()),iceb_u_kolbait(16,naivkom.ravno()),naivkom.ravno());
 }
fprintf(ff,"%-*.*s|\n",iceb_u_kolbait(16,gettext("Итого")),iceb_u_kolbait(16,gettext("Итого")),gettext("Итого"));

fprintf(ff,"        |                              |%*.*s|",
iceb_u_kolbait(5,gettext("кома.")),
iceb_u_kolbait(5,gettext("кома.")),
gettext("кома."));

for(int i=0; i < kolkr+1; i++)
 {
  if(i < kolkr)
   {
    bb=0.;
    for(i1=0; i1 < kolkont; i1++)
      bb+=mkol[i1*kolkr+i];
    if(bb == 0.)
      continue;
   }
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_u_kolbait(5,gettext("Кол.")),iceb_u_kolbait(5,gettext("Кол.")),gettext("Кол."),
  iceb_u_kolbait(10,gettext("Сумма")),iceb_u_kolbait(10,gettext("Сумма")),gettext("Сумма"));

 } 
fprintf(ff,"\n");

fprintf(ff,"%s\n",strpod.ravno());
double itogo_k=0.;
for(int i=0; i < kolkont; i++)
 {
  //если по строке все равно нулю то строку не печатаем
  itogo_k=itogo=0.;
  
  for(i1=0; i1 < kolkr; i1++)
    itogo+=mkol[i*kolkr+i1];
  if(itogo == 0.)
   continue;
   


  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
  KONTR->ravno(i));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   naikont.new_plus(row[0]);
  else
   naikont.new_plus("");
     
  fprintf(ff,"%*s %-*.*s %5d",
  iceb_u_kolbait(8,KONTR->ravno(i)),KONTR->ravno(i),
  iceb_u_kolbait(30,naikont.ravno()),iceb_u_kolbait(30,naikont.ravno()),naikont.ravno(),
  mkolkom[i]);
  
  itkolkom+=mkolkom[i];

  itogo=0.;
  for(i1=0; i1 < kolkr; i1++)
   {
    bb=0.;
    for(int i2=0; i2 < kolkont; i2++)
      bb+=mkol[i2*kolkr+i1];
    if(bb == 0.)
      continue;
    if(mkol[i*kolkr+i1] != 0. || msum[i*kolkr+i1] != 0.)
     fprintf(ff," %5.5g %10.2f",mkol[i*kolkr+i1],msum[i*kolkr+i1]);
    else
     fprintf(ff," %5s %10s","","");
    itogo+=msum[i*kolkr+i1];
    itogo_k+=mkol[i*kolkr+i1];
   }
  fprintf(ff," %5.5g %10.2f\n",itogo_k,itogo);

 }
fprintf(ff,"%s\n",strpod.ravno());
fprintf(ff,"%*s: %5d",iceb_u_kolbait(38,gettext("Итого")),gettext("Итого"),itkolkom);

double itogoo_k=0.;
for(int i=0; i < kolkr; i++)
 {
  bb=0.;
  for(i1=0; i1 < kolkont; i1++)
    bb+=mkol[i1*kolkr+i];
  if(bb == 0.)
    continue;
  itogo=0.;
  itogo_k=0.;
  for(i1=0; i1 < kolkont; i1++)
   {
    itogo+=msum[i1*kolkr+i];
    itogo_k+=mkol[i1*kolkr+i];
   }
  fprintf(ff," %5.5g %10.2f",itogo_k,itogo);
  itogoo+=itogo;
  itogoo_k+=itogo_k;
 }

fprintf(ff," %5.5g %10.2f\n",itogoo_k,itogoo);

iceb_podpis(ff,wpredok);
fclose(ff);
}


/******************************************/
/******************************************/

gint ukrkontr_r1(class ukrkontr_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;


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



//Определяем количество командировочных расходов
sprintf(strsql,"select kod from Ukrkras");

int kolkr=0;
if((kolkr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolkr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    
int i=0;
short mkkr[kolkr]; //Массив кодов командировочных расходов
while(cur.read_cursor(&row) != 0)
  mkkr[i++]=atoi(row[0]);  

//Определяем количество контрагентов
sprintf(strsql,"select distinct kont from Ukrdok where datao >= '%04d-%d-%d' and \
datao <= '%04d-%d-%d'",gn,mn,dn,gk,mk,dk);
int kolkont=0;
if((kolkont=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolkont == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    
int  mkolkom[kolkont]; //Массив количества командировок
memset(mkolkom,'\0',sizeof(mkolkom));

i=0;
iceb_u_spisok KONTR; //Массив кодов контрагентов
while(cur.read_cursor(&row) != 0)
  KONTR.plus(row[0]);  

double msum[kolkont*kolkr]; //Массив сум
memset(&msum,'\0',sizeof(msum));

double mkol[kolkont*kolkr]; //Массив количества
memset(&mkol,'\0',sizeof(mkol));

sprintf(strsql,"select datd,nomd,kont,vkom from Ukrdok where datao >= '%04d-%d-%d' and \
datao <= '%04d-%d-%d'",gn,mn,dn,gk,mk,dk);
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

SQLCURSOR cur1;
int pozkr=0;
int pozkont=0;
double suma=0.;
int kodkr=0;

FILE *ffprot;
char imafprot[30];
sprintf(imafprot,"ukrkp%d.lst",getpid());

if((ffprot = fopen(imafprot,"w")) == NULL)
 {
  iceb_er_op_fil(imafprot,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

short kolsimv=iceb_u_strlen(ICEB_ZAR_PKTN);


iceb_zagolov(gettext("Протокол хода расчёта"),dn,mn,gn,dk,mk,gk,ffprot,data->window);
double kolih=0.;
float kolstr1=0.;
int kolstr2=0;
SQL_str row1;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->kontr.ravno(),row[2],2,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->vidkom.ravno(),row[3],0,0) != 0)
     continue;

  if((pozkont=KONTR.find_r(row[2])) == -1)
    continue;

  if(data->rk->kod_gr_pod.ravno()[0] != '\0')
   {
    SQL_str row1;
    class SQLCURSOR cur1;
    class iceb_u_str podr("");
    short d,m,g;
    
    iceb_u_rsdat(&d,&m,&g,row[0],2);
    
    class iceb_u_str tabnom;
    tabnom.plus(iceb_u_adrsimv(kolsimv,row[2]));    
//    sprintf(tabnom,"%s",&row[2][2]);
    
    sprintf(strsql,"select podr from Zarn where tabn=%s and god=%d and mes=%d",tabnom.ravno(),g,m);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
      podr.new_plus(row1[0]);
    else
     {
      sprintf(strsql,"select podr from Kartb where tabn=%s",tabnom.ravno());
      if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
        podr.new_plus(row1[0]);
     }    

    sprintf(strsql,"select grup from Podr where kod=%d",podr.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
      if(iceb_u_proverka(data->rk->kod_gr_pod.ravno(),row1[0],0,0) != 0)
       continue;
   
   }

  
  fprintf(ffprot,"\n%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
  mkolkom[pozkont]++;
      
  sprintf(strsql,"select kodr,kolih,cena,snds,shet from Ukrdok1 where datd='%s' and nomd='%s'",
  row[0],row[1]);   
  
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    if(iceb_u_proverka(data->rk->kod_zat.ravno(),row1[0],0,0) != 0)
      continue;
    if(iceb_u_proverka(data->rk->shetu.ravno(),row1[4],1,0) != 0)
      continue;
    fprintf(ffprot," %s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
      
    kodkr=atoi(row1[0]);
    suma=atof(row1[2])+atof(row1[3]);
    kolih=atof(row1[1]);
        
    for(pozkr=0 ; pozkr < kolkr; pozkr++)
     if(mkkr[pozkr] == kodkr)
      break;
    msum[pozkont*kolkr+pozkr]+=suma;
    mkol[pozkont*kolkr+pozkr]+=kolih;
   }
 }

char imaf[64];
sprintf(imaf,"ukrk%d.lst",getpid());

ukrras_rm(0,dn,mn,gn,dk,mk,gk,kolkr,kolkont,msum,mkkr,&KONTR,mkolkom,data->rk->kontr.ravno(),data->rk->vidkom.ravno(),data->rk->kod_zat.ravno(),data->rk->kod_gr_pod.ravno(),data->rk->shetu.ravno(),imaf,data->window);

char imafkol[64];
sprintf(imafkol,"ukrkkol%d.lst",getpid());

ukrras_rm(1,dn,mn,gn,dk,mk,gk,kolkr,kolkont,mkol,mkkr,&KONTR,mkolkom,data->rk->kontr.ravno(),data->rk->vidkom.ravno(),data->rk->kod_zat.ravno(),data->rk->kod_gr_pod.ravno(),data->rk->shetu.ravno(),imafkol,data->window);

char imaf_ks[64];
sprintf(imaf_ks,"ukrks%d.lst",getpid());
ukrras_rmks(dn,mn,gn,dk,mk,gk,kolkr,kolkont,msum,mkol,mkkr,&KONTR,mkolkom,data->rk->kontr.ravno(),data->rk->vidkom.ravno(),data->rk->kod_zat.ravno(),data->rk->kod_gr_pod.ravno(),data->rk->shetu.ravno(),imaf_ks,data->window);

iceb_podpis(ffprot,data->window);
fclose(ffprot);


data->rk->imaf.plus(imaf_ks);

sprintf(strsql,"%s (%s+%s)",
gettext("Ведомость затрат по контрагентам"),
gettext("Количество"),
gettext("Сумма"));

data->rk->naim.plus(strsql);

data->rk->imaf.plus(imaf);

sprintf(strsql,"%s (%s)",
gettext("Ведомость затрат по контрагентам"),
gettext("Сумма"));

data->rk->naim.plus(strsql);

data->rk->imaf.plus(imafkol);

sprintf(strsql,"%s (%s)",
gettext("Ведомость затрат по контрагентам"),
gettext("Количество"));
data->rk->naim.plus(strsql);

data->rk->imaf.plus(imafprot);
data->rk->naim.plus(gettext("Протокол хода расчёта"));

for(int nomer=0; nomer < data->rk->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rk->imaf.ravno(nomer),3,data->window);



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
