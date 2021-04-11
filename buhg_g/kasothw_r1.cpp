/*$Id: kasothw_r1.c,v 1.20 2013/09/26 09:46:49 sasa Exp $*/
/*23.05.2016	10.03.2006	Белых А.И.	kasothw_r1.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "kasothw.h"

class kasothw_r1_data
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

  kasothw_r1_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   kasothw_r1_key_press(GtkWidget *widget,GdkEventKey *event,class kasothw_r1_data *data);
gint kasothw_r11(class kasothw_r1_data *data);
void  kasothw_r1_v_knopka(GtkWidget *widget,class kasothw_r1_data *data);

void 	rasoth(short dn,short mn,short gn,class iceb_u_spisok *OPP,int kolopp,int kolden,double *prih,int metka,FILE *ff);
void 	rasothdn(short dn,short mn,short gn,class iceb_u_spisok *OPP,int kolopp,int kolden,double *prih,int metka,FILE *ff);

extern SQL_baza bd;
extern double   okrg1;
extern double	okrcn;

int kasothw_r1(class kasothw_rr *datark,class spis_oth *oth,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class kasothw_r1_data data;

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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка свода по операциям"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(kasothw_r1_key_press),&data);

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

repl.plus(gettext("Распечатка свода по операциям"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(kasothw_r1_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)kasothw_r11,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  kasothw_r1_v_knopka(GtkWidget *widget,class kasothw_r1_data *data)
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

gboolean   kasothw_r1_key_press(GtkWidget *widget,GdkEventKey *event,class kasothw_r1_data *data)
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
/*печать реквизитов поиска*/
/******************************/
void kasothw_r1_rp(class kasothw_r1_data *data,FILE *ff)
{
if(data->rk->kassa.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Касса"),data->rk->kassa.ravno());
if(data->rk->kodop.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
if(data->rk->shet.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());
if(data->rk->metka_pr != 0)
  fprintf(ff,"%s:%d\n",gettext("Вид операции"),data->rk->metka_pr);
if(data->rk->kod_kontr.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Код контрагента"),data->rk->kod_kontr.ravno());
}
/******************************************/
/* Вычислить сумму по вертикальной колонке*/
/******************************************/

double  sumavk(int nomden,int kolopp,int kolden,double *prih)
{
double  itog=0.;

for(int nomop=0; nomop < kolopp; nomop++)
 {
  itog+=prih[nomop*kolden+nomden];
 }

return(itog);

}
/************************************/
/*распечатываем общий итог*/
/***********************************/
void rasoth_it(short dn,short mn,short gn,
class iceb_u_spisok *OPP, //Список операций
int kolopp, //Количество операций
int kolden, //Количество дней в периоде
double *prih, //Данные
int metka, //1-приходные операции 2-расходные
FILE *ff)
{
int nomop=0;
int nomden=0;
char strsql[512];
SQL_str row;
double itog=0.,itogo=0.;
SQLCURSOR curr;

if(metka == 1)
 fprintf(ff,"\n%s\n",gettext("Приходы"));
if(metka == 2)
 fprintf(ff,"\n%s\n",gettext("Расходы"));


fprintf(ff,"\
-----------------------------------------------\n");
fprintf(ff,"\
К.о.|   Наименование операции      |  Итого   |\n");
fprintf(ff,"\
-----------------------------------------------\n");


class iceb_u_str naimop("");

for(nomop=0; nomop < kolopp; nomop++)
 {
  /*Определяем нужно ли печатать горизонтальную строку*/
  itog=0;
  for(nomden=0; nomden < kolden; nomden++)
   {
    int nom=nomop*kolden+nomden;
    itog+=prih[nom];
    if(itog != 0.)
     break;
   }
  if(itog == 0)
   continue;

  
  //Читаем наименование операции
  if(metka == 1)
   sprintf(strsql,"select naik from Kasop1 where kod='%s'",OPP->ravno(nomop));
  if(metka == 2)
   sprintf(strsql,"select naik from Kasop2 where kod='%s'",OPP->ravno(nomop));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimop.new_plus(row[0]);
  else
    naimop.new_plus("");
  fprintf(ff,"%-*s %-*.*s",iceb_u_kolbait(4,OPP->ravno(nomop)),OPP->ravno(nomop),
  iceb_u_kolbait(30,naimop.ravno()),iceb_u_kolbait(30,naimop.ravno()),naimop.ravno());
  itog=0.;
  for(nomden=0; nomden < kolden; nomden++)
   {
    if(sumavk(nomden,kolopp,kolden,prih) == 0.)
     continue;    
    int nom=nomop*kolden+nomden;
//    fprintf(ff," %10.2f",prih[nom]);
    itog+=prih[nom];
   }
  fprintf(ff," %10.2f\n",itog);
 }
fprintf(ff,"\
-----------------------------------------------\n");

fprintf(ff,"%*s",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"));
for(nomden=0; nomden < kolden; nomden++)
 {
  if(sumavk(nomden,kolopp,kolden,prih) == 0.)
   continue;    
  itog=0.;
  for(nomop=0; nomop < kolopp; nomop++)
   {
    itog+=prih[nomop*kolden+nomden];
   }
//fprintf(ff," %10.2f",itog);
  itogo+=itog;
 }
fprintf(ff," %10.2f\n",itogo);
 
}

/******************************************/
/******************************************/

gint kasothw_r11(class kasothw_r1_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);

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


//Вычисляем количество дней в периоде
int kolden=iceb_u_period(dn,mn,gn,dk,mk,gk,0);

SQLCURSOR cur;
SQLCURSOR cur1;

//Читаем и запоминаем список операций на приход
sprintf(strsql,"select kod from Kasop1");
int kolopp=0;
if((kolopp=cur.make_cursor(&bd,strsql)) < 0)
 {
  sss.clear_data();
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
SQL_str row;

iceb_u_spisok OPP;
while(cur.read_cursor(&row) != 0)
  OPP.plus(row[0]);

//Читаем и запоминаем список операций на расход
sprintf(strsql,"select kod from Kasop2");
int kolopr=0;
if((kolopr=cur.make_cursor(&bd,strsql)) < 0)
 {
  sss.clear_data();
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_spisok OPR;
while(cur.read_cursor(&row) != 0)
  OPR.plus(row[0]);

double  prih[kolden*kolopp]; //Массив данных приходов
double  rash[kolden*kolopr]; //Массив данных расходов

memset(&prih,'\0',sizeof(prih));
memset(&rash,'\0',sizeof(rash));

sprintf(strsql,"select kassa,tp,datd,nomd,shetk,kodop \
from Kasord where datd >= '%04d-%d-%d' and datd <= '%04d-%d-%d' \
order by datd,tp,nomd asc",gn,mn,dn,gk,mk,dk);

int kolstr=0;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  sss.clear_data();
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  sss.clear_data();
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int tipz=0;
short den,mes,god;
int kolstr2=0;
double suma=0.;
SQL_str row1;
int nomden=0;
int nomop=0;

float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->kassa.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->kodop.ravno(),row[5],0,0) != 0)
    continue;
  tipz=atoi(row[1]);

  if(data->rk->metka_pr != 0)
   {
    if(data->rk->metka_pr == 1 && tipz == 2)
      continue;
    if(data->rk->metka_pr == 2 && tipz == 1)
      continue;
   }
  if(iceb_u_proverka(data->rk->shet.ravno(),row[4],0,0) != 0)
    continue;
  
  iceb_u_rsdat(&den,&mes,&god,row[2],2);
  
  //Читаем подтвержденные суммы в документе
  sprintf(strsql,"select suma from Kasord1 where kassa=%s and god=%d and \
tp=%d and nomd='%s' and datp <> '0000-00-00'",row[0],god,tipz,row[3]);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  suma=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    suma+=atof(row1[0]);    
   }

  if(suma == 0.)
   continue;
  
  nomden=iceb_u_period(dn,mn,gn,den,mes,god,0)-1;
  
  if(tipz == 1)
    if((nomop=OPP.find(row[5])) >= 0)
     {
      prih[nomop*kolden+nomden]+=suma;
     }

  if(tipz == 2)
    if((nomop=OPR.find(row[5])) >= 0)
     {
      rash[nomop*kolden+nomden]+=suma;
     }
 }


//Распечатываем результаты
char imaf[64];
FILE *ff;
sprintf(imaf,"kasvop%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  sss.clear_data();
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_zagolov(gettext("Распечатка свода по видам операций"),dn,mn,gn,dk,mk,gk,ff,data->window);
kasothw_r1_rp(data,ff);

//Распечатываем результаты
char imafdn[64];
FILE *ffdn;

sprintf(imafdn,"kasvopd%d.lst",getpid());
if((ffdn = fopen(imafdn,"w")) == NULL)
 {
  sss.clear_data();
  iceb_er_op_fil(imafdn,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_zagolov(gettext("Распечатка свода по видам операций"),dn,mn,gn,dk,mk,gk,ffdn,data->window);
kasothw_r1_rp(data,ffdn);

char imaf_it[64];
sprintf(imaf_it,"kasopi%d.lst",getpid());
class iceb_fopen ff_it;
if(ff_it.start(imaf_it,"w",data->window) != 0)
 {
  sss.clear_data();
  iceb_er_op_fil(imafdn,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_zagolov(gettext("Общие итоги по операциям"),dn,mn,gn,dk,mk,gk,ff_it.ff,data->window);
kasothw_r1_rp(data,ffdn);

//Распечатываем приходы по операциям
rasoth(dn,mn,gn,&OPP,kolopp,kolden,prih,1,ff);

//Распечатываем расходы по операциям
rasoth(dn,mn,gn,&OPR,kolopr,kolden,rash,2,ff);

//Распечатываем приходы по дням
rasothdn(dn,mn,gn,&OPP,kolopp,kolden,prih,1,ffdn);

//Распечатываем расходы по дням
rasothdn(dn,mn,gn,&OPR,kolopr,kolden,rash,2,ffdn);

//Распечатываем общий итог приходы по операциям
rasoth_it(dn,mn,gn,&OPP,kolopp,kolden,prih,1,ff_it.ff);

//Распечатываем общий итог расходы по операциям
rasoth_it(dn,mn,gn,&OPR,kolopr,kolden,rash,2,ff_it.ff);

iceb_podpis(ff,data->window);
fclose(ff);

iceb_podpis(ffdn,data->window);
fclose(ffdn);

iceb_podpis(ff_it.ff,data->window);
ff_it.end();

data->oth->spis_imaf.plus(imaf);
data->oth->spis_naim.plus(gettext("Свода по видам операций"));
data->oth->spis_imaf.plus(imafdn);
data->oth->spis_naim.plus(gettext("Свода по видам операций по дням"));
data->oth->spis_imaf.plus(imaf_it);
data->oth->spis_naim.plus(gettext("Общий итог по операциям"));

for(int nomer=0; nomer < data->oth->spis_imaf.kolih(); nomer++)
 iceb_ustpeh(data->oth->spis_imaf.ravno(nomer),0,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/****************************************************/
/*Вычислить сумму по вертикали для распечатки по дням*/
/*****************************************************/
double kasoth1_spvd(int nomop,int kolopp,int kolden,double *prih)
{
double itog=0.;
for(int nomden=0; nomden < kolden; nomden++)
  itog+=prih[nomop*kolden+nomden];
return(itog);
}

/**************************************************/
/*Распечатка результатов расчёта по операциям    */
/*************************************************/

void 	rasoth(short dn,short mn,short gn,
class iceb_u_spisok *OPP, //Список операций
int kolopp, //Количество операций
int kolden, //Количество дней в периоде
double *prih, //Данные
int metka, //1-приходные операции 2-расходные
FILE *ff)
{
int nomop=0;
int nomden=0;
class iceb_u_str naimop("");
char strsql[1024];
SQL_str row;
double itog=0.,itogo=0.;
short   d,m,g;
int	kolnpk=0; //Количество не пустых колонок
SQLCURSOR curr;
//Определяем количество не пустых колонок 
for(nomden=0; nomden < kolden; nomden++)
 if(sumavk(nomden,kolopp,kolden,prih) != 0.)
   kolnpk++;

char	stroka[36+11*(kolnpk+1)+1];
memset(stroka,'-',sizeof(stroka)-1);
stroka[sizeof(stroka)-1]='\0';

if(metka == 1)
 fprintf(ff,"\n%s\n",gettext("Приходы"));
if(metka == 2)
 fprintf(ff,"\n%s\n",gettext("Расходы"));


fprintf(ff,"%s\n",stroka);

fprintf(ff,"К.о.|   Наименование операции      |");

d=dn; m=mn; g=gn;

for(nomden=0; nomden < kolden; nomden++)
 {
  if(sumavk(nomden,kolopp,kolden,prih) == 0.)
   {
    iceb_u_dpm(&d,&m,&g,1);
    continue;    
   }

  fprintf(ff,"%02d.%02d.%04d|",d,m,g);
  iceb_u_dpm(&d,&m,&g,1);
 }

fprintf(ff,"%*s|\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",stroka);

for(nomop=0; nomop < kolopp; nomop++)
 {
  /*Определяем нужно ли печатать горизонтальную строку*/
  itog=0;
  for(nomden=0; nomden < kolden; nomden++)
   {
    int nom=nomop*kolden+nomden;
    itog+=prih[nom];
    if(itog != 0.)
     break;
   }
  if(itog == 0)
   continue;
  //Читаем наименование операции
  if(metka == 1)
   sprintf(strsql,"select naik from Kasop1 where kod='%s'",OPP->ravno(nomop));
  if(metka == 2)
   sprintf(strsql,"select naik from Kasop2 where kod='%s'",OPP->ravno(nomop));
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimop.new_plus(row[0]);  
  else
    naimop.new_plus("");
  fprintf(ff,"%*s %-*.*s",
  iceb_u_kolbait(4,OPP->ravno(nomop)),OPP->ravno(nomop),
  iceb_u_kolbait(30,naimop.ravno()),iceb_u_kolbait(30,naimop.ravno()),naimop.ravno());

  itog=0.;
  for(nomden=0; nomden < kolden; nomden++)
   {
    if(sumavk(nomden,kolopp,kolden,prih) == 0.)
     continue;    
    int nom=nomop*kolden+nomden;
    fprintf(ff," %10.2f",prih[nom]);
    itog+=prih[nom];
   }
  fprintf(ff," %10.2f\n",itog);
 }
fprintf(ff,"%s\n",stroka);

fprintf(ff,"%*s",iceb_u_kolbait(35,gettext("Итого")),gettext("Итого"));
for(nomden=0; nomden < kolden; nomden++)
 {
  if(sumavk(nomden,kolopp,kolden,prih) == 0.)
   continue;    
  itog=0.;
  for(nomop=0; nomop < kolopp; nomop++)
   {
    itog+=prih[nomop*kolden+nomden];
   }
  fprintf(ff," %10.2f",itog);
  itogo+=itog;
 }
fprintf(ff," %10.2f\n",itogo);
 
}


/*****************************************/
/*Распечатка результата отчёта по дням   */
/*****************************************/
void 	rasothdn(short dn,short mn,short gn,
class iceb_u_spisok *OPP, //Список операций
int kolopp, //Количество операций
int kolden, //Количество дней в периоде
double *prih, //Данные
int metka, //1-приходные операции 2-расходные
FILE *ff)
{

int nomop=0;
int nomden=0;
double itog=0.,itogo=0.;
short   d,m,g;
int     nom=0;
char strsql[1024];
class iceb_u_str stroka;
SQL_str row;
class SQLCURSOR cur;
stroka.plus("-----------");
for(nomop=0; nomop < kolopp; nomop++) 
 {
  if(kasoth1_spvd(nomop,kolopp,kolden,prih) == 0)
   continue;
  stroka.plus("-----------");
 }
stroka.plus("-----------"); /*Итого*/


if(metka == 1)
 fprintf(ff,"\n%s\n",gettext("Приходы"));
if(metka == 2)
 fprintf(ff,"\n%s\n",gettext("Расходы"));


fprintf(ff,"%s\n",stroka.ravno());

fprintf(ff,"   Дата   |");

class iceb_u_str naim_op("");
for(nomop=0; nomop < kolopp; nomop++)
 {
  if(kasoth1_spvd(nomop,kolopp,kolden,prih) == 0)
   continue;
  /*Читаем наименование операции*/
  sprintf(strsql,"select naik from Kasop%d where kod='%s'",metka,OPP->ravno(nomop));
  if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
    naim_op.new_plus(row[0]);  
  else
    naim_op.new_plus("");
  sprintf(strsql,"%s %s",OPP->ravno(nomop),naim_op.ravno());

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(10,strsql),iceb_u_kolbait(10,strsql),strsql);
 }
fprintf(ff,"%*s|\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));
fprintf(ff,"%s\n",stroka.ravno());

d=dn; m=mn; g=gn;
for(nomden=0; nomden < kolden; nomden++)
 {
  itog=0.;
  for(nomop=0; nomop < kolopp; nomop++)
    itog+=prih[nomop*kolden+nomden];

  if(itog == 0.)
   {
    iceb_u_dpm(&d,&m,&g,1);
    continue;
   }

  fprintf(ff,"%02d.%02d.%04d|",d,m,g);
     
  for(nomop=0; nomop < kolopp; nomop++)
   {
    if(kasoth1_spvd(nomop,kolopp,kolden,prih) == 0)
     continue;
    nom= nomop*kolden+nomden;
//    printw("nomden=%d kolopp=%d nomop=%d nom=%d suma=%f\n",
//    nomden,kolopp,nomop,nom,prih[nom]);
    
    fprintf(ff," %10.2f",prih[nom]);
   
   }  
  fprintf(ff," %10.2f\n",itog);
  iceb_u_dpm(&d,&m,&g,1);
 }
fprintf(ff,"%s\n",stroka.ravno());

fprintf(ff,"%*s|",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));

itogo=0.;
for(nomop=0; nomop < kolopp; nomop++)
 {
    if(kasoth1_spvd(nomop,kolopp,kolden,prih) == 0)
     continue;
  itog=0.;
  for(nomden=0; nomden < kolden; nomden++)
    itog+=prih[nomop*kolden+nomden];
  fprintf(ff," %10.2f",itog);
  itogo+=itog;   
 }
  fprintf(ff," %10.2f\n",itogo);
}
