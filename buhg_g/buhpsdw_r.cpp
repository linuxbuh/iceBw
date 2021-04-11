/*$Id: buhpsdw_r.c,v 1.22 2014/07/31 07:08:24 sasa Exp $*/
/*16.05.2018	11.09.2008	Белых А.И.	buhpsdw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "buhpsdw.h"

class buhpsdw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class buhpsdw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  buhpsdw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   buhpsdw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhpsdw_r_data *data);
gint buhpsdw_r1(class buhpsdw_r_data *data);
void  buhpsdw_r_v_knopka(GtkWidget *widget,class buhpsdw_r_data *data);


extern SQL_baza bd;
extern short	startgodb; /*Стартовый год*/
extern int      kol_strok_na_liste;

int buhpsdw_r(class buhpsdw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class buhpsdw_r_data data;
int gor=0;
int ver=0;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Проверка согласованности данных в подсистемах"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(buhpsdw_r_key_press),&data);

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

repl.plus(gettext("Проверка согласованности данных в подсистемах"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(buhpsdw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);
gtk_text_buffer_create_tag(data.buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)buhpsdw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  buhpsdw_r_v_knopka(GtkWidget *widget,class buhpsdw_r_data *data)
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

gboolean   buhpsdw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhpsdw_r_data *data)
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
/************/
/*Шапка*/
/********/
void buhpsd_sap(short dn,short mn,short gn,
short dk,short mk,short gk,FILE *ff)
{
fprintf(ff,"\
---------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
    Подсиcтема      |  Cальдо на %02d.%02d.%04d   |   Оборот за период      |  Cальдо на %02d.%02d.%04d   |\n\
                    |   Дебет    |   Кредит   |   Дебет    |   Кредит   |   Дебет    |   Кредит   |\n"),
dn,mn,gn,dk,mk,gk);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------\n");
}
/*****************************/
/*Получение данных по проводкам*/
/*******************************/

void buhpsd_pr(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *shet_uh,class iceb_u_str *naim_shet,double *deb_kre,
GtkWidget *wpredok)
{
naim_shet->new_plus("");
int  saldo=0;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
sprintf(strsql,"select nais,saldo from Plansh where ns='%s'",shet_uh);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  naim_shet->new_plus(row[0]);
  saldo=atoi(row[1]);
 }

if(saldo != 3)
   rpshet_ssw(shet_uh,dn,mn,gn,dk,mk,gk,deb_kre,wpredok);
else
   rpshet_rsw(shet_uh,dn,mn,gn,dk,mk,gk,deb_kre,wpredok);

}
/***************************/
/*Учёт основных средств*/
/**************************/

int buhpsd_uos(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *sheta,
FILE *ff,
GtkWidget *bar,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"\n%s:%s\n",gettext("Подсистема"),
gettext("Учёт основных средств"));

iceb_printw(strsql,buffer,view);

sprintf(strsql,"select innom from Uosin");

int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 return(0);




float kolstr1=0;
/*Переменные данные по инвентарному номеру*/
//class iceb_u_str shetu(""); /*Счет учета*/

int podr=0;
int kodotl=0;
long innom=0;
class iceb_u_spisok shet_uh;

const int razmer_mas=4;

class iceb_u_double suma;
int nomer_sheta=0;
class poiinpdw_data rekos;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(bar,kolstr,++kolstr1);    
  innom=atol(row[0]);
  /*Смотрим на начало и на конец периода, так как может быть приход в периоде или расход*/
  if(poiinw(innom,dn,mn,gn,&podr,&kodotl,wpredok) != 0)
   if(poiinw(innom,dk,mk,gk,&podr,&kodotl,wpredok) != 0)
    continue;

  
//  poiinpdw(innom,mk,gk,shetu,hzt,hau,hna,&popkf,&soso,nomz,&m1,&g1,hnaby,&popkfby,wpredok);
  poiinpdw(innom,mk,gk,&rekos,wpredok);


  if(iceb_u_proverka(sheta,rekos.shetu.ravno(),1,0) != 0)
   continue;


  if((nomer_sheta=shet_uh.find(rekos.shetu.ravno())) < 0)
   {
    shet_uh.plus(rekos.shetu.ravno());
    suma.plus_str(razmer_mas);
    
    nomer_sheta=shet_uh.find(rekos.shetu.ravno()); /*определяем под каким номером записан*/

   }  

  class ostuosw_data uos_ost;

  ostuosw(innom,dn,mn,gn,dk,mk,gk,0,0,&uos_ost,wpredok);

  for(int ii=0; ii < razmer_mas; ii++)
   {
    suma.plus(uos_ost.ostmcby[ii],nomer_sheta*razmer_mas+ii);
   }  
//  OSTANOV();
 }



fprintf(ff,"\n%s:%s\n",gettext("Подсистема"),
gettext("Учёт основных средств"));


buhpsd_sap(dn,mn,gn,dk,mk,gk,ff); /*шапка*/
double deb_kre[6];
int kolih_shet=shet_uh.kolih();
class iceb_u_str naim_shet;
kolstr1=0;
for(nomer_sheta=0 ; nomer_sheta < kolih_shet; nomer_sheta++)
 {
//  strzag(LINES-1,0,kolih_shet,++kolstr1);
  iceb_pbar(bar,kolih_shet,++kolstr1);    

  buhpsd_pr(dn,mn,gn,dk,mk,gk,shet_uh.ravno(nomer_sheta),&naim_shet,deb_kre,wpredok);

  fprintf(ff,"\n%s %s\n",shet_uh.ravno(nomer_sheta),naim_shet.ravno());
  
  sprintf(strsql,"\n%s %s\n",shet_uh.ravno(nomer_sheta),naim_shet.ravno());
  iceb_printw(strsql,buffer,view,"naim_shrift");

  fprintf(ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

  fprintf(ff,"%-*.*s|%12.2f|%12s|%12.2f|%12.2f|%12.2f|%12s|\n",
  iceb_u_kolbait(20,gettext("Учёт основных средств")),iceb_u_kolbait(20,gettext("Учёт основных средств")),gettext("Учёт основных средств"),
  suma.ravno(nomer_sheta*razmer_mas+0),
  "",
  suma.ravno(nomer_sheta*razmer_mas+1),
  suma.ravno(nomer_sheta*razmer_mas+2),
  suma.ravno(nomer_sheta*razmer_mas+3),
  "");

  sprintf(strsql,"%-*.*s|%12.2f|%12s|%12.2f|%12.2f|%12.2f|%12s|\n",
  iceb_u_kolbait(10,gettext("Учёт основных средств")),iceb_u_kolbait(10,gettext("Учёт основных средств")),gettext("Учёт основных средств"),
  suma.ravno(nomer_sheta*razmer_mas+0),
  "",
  suma.ravno(nomer_sheta*razmer_mas+1),
  suma.ravno(nomer_sheta*razmer_mas+2),
  suma.ravno(nomer_sheta*razmer_mas+3),
  "");
  iceb_printw(strsql,buffer,view,"naim_shrift");

  fprintf(ff,"%-*.*s|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|\n",
  iceb_u_kolbait(20,gettext("Главная книга")),iceb_u_kolbait(20,gettext("Главная книга")),gettext("Главная книга"),
  deb_kre[0],
  deb_kre[1],
  deb_kre[2],
  deb_kre[3],
  deb_kre[4],
  deb_kre[5]);

  sprintf(strsql,"%-*.*s|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|\n",
  iceb_u_kolbait(10,gettext("Главная книга")),iceb_u_kolbait(10,gettext("Главная книга")),gettext("Главная книга"),
  deb_kre[0],
  deb_kre[1],
  deb_kre[2],
  deb_kre[3],
  deb_kre[4],
  deb_kre[5]);
  iceb_printw(strsql,buffer,view,"naim_shrift");

  fprintf(ff,"%-*.*s|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|\n",
  iceb_u_kolbait(20,gettext("Разница")),iceb_u_kolbait(20,gettext("Разница")),gettext("Разница"),
  suma.ravno(nomer_sheta*razmer_mas+0)-deb_kre[0],
  deb_kre[1],
  suma.ravno(nomer_sheta*razmer_mas+1)-deb_kre[2],
  suma.ravno(nomer_sheta*razmer_mas+2)-deb_kre[3],
  suma.ravno(nomer_sheta*razmer_mas+3)-deb_kre[4],
  deb_kre[5]);

  sprintf(strsql,"%-*.*s|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|\n",
  iceb_u_kolbait(10,gettext("Разница")),iceb_u_kolbait(10,gettext("Разница")),gettext("Разница"),
  suma.ravno(nomer_sheta*razmer_mas+0)-deb_kre[0],
  deb_kre[1],
  suma.ravno(nomer_sheta*razmer_mas+1)-deb_kre[2],
  suma.ravno(nomer_sheta*razmer_mas+2)-deb_kre[3],
  suma.ravno(nomer_sheta*razmer_mas+3)-deb_kre[4],
  deb_kre[5]);
  
  iceb_printw(strsql,buffer,view,"naim_shrift");
  
 
 
 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------------\n");

return(0);
}

/************************/
/*Материальный учёт*/
/**********************/
int buhpsd_mu(short dn,short mn,short gn,
short dk,short mk,short gk,
const char *sheta,
FILE *ff,
GtkWidget *bar,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_spisok shet_uh; /*Список счетов учёта в материальном учёте*/

sprintf(strsql,"\n%s:%s\n",gettext("Подсистема"),
gettext("Материальный учёт"));

iceb_printw(strsql,buffer,view);


sprintf(strsql,"select distinct shetu from Kart");

SQLCURSOR cur;
SQL_str row;
int kolstr=0;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не нашли ни одной записи!"),wpredok);
  return(1);
 }  
float kolstr1=0;
 
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(bar,kolstr,++kolstr1);    
  shet_uh.plus(row[0]);
 }
int kolih_shet=shet_uh.kolih();
/*Создаём массив для числовых данных*/

double suma_mat[kolih_shet][4];
memset(suma_mat,'\0',sizeof(suma_mat));

sprintf(strsql,"select sklad,nomk,shetu from Kart");


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }


kolstr1=0;
int nomer_sheta=0; 
class ostatok ostk;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(sheta,row[2],1,0) != 0)
   continue;

  if((nomer_sheta=shet_uh.find(row[2])) < 0)
   {
    sprintf(strsql,"Не найден счёт %s в списке счетов !",row[2]);
    iceb_menu_soob(strsql,wpredok);
    continue;
   }
 
  ostkarw(dn,mn,gn,dk,mk,gk,atoi(row[0]),atoi(row[1]),&ostk);

  suma_mat[nomer_sheta][0]+=ostk.ostgc[0];
  suma_mat[nomer_sheta][1]+=ostk.ostgc[1];
  suma_mat[nomer_sheta][2]+=ostk.ostgc[2];
  suma_mat[nomer_sheta][3]+=ostk.ostgc[3];

 }



/*Сравниваем данные с проводками в Главной книге*/

fprintf(ff,"\n%s:%s\n",gettext("Подсистема"),
gettext("Материальный учёт"));


buhpsd_sap(dn,mn,gn,dk,mk,gk,ff); /*шапка*/

class iceb_u_str naim_shet;
double deb_kre[6];
kolstr1=0;
for(nomer_sheta=0; nomer_sheta < kolih_shet; nomer_sheta++)
 {
//  strzag(LINES-1,0,kolih_shet,++kolstr1);
  iceb_pbar(bar,kolih_shet,++kolstr1);    
  if(iceb_u_proverka(sheta,shet_uh.ravno(nomer_sheta),1,0) != 0)
   continue;

  buhpsd_pr(dn,mn,gn,dk,mk,gk,shet_uh.ravno(nomer_sheta),&naim_shet,deb_kre,wpredok);

  fprintf(ff,"\n%s %s\n",shet_uh.ravno(nomer_sheta),naim_shet.ravno());

  sprintf(strsql,"\n%s %s\n",shet_uh.ravno(nomer_sheta),naim_shet.ravno());
  iceb_printw(strsql,buffer,view,"naim_shrift");

  fprintf(ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

  fprintf(ff,"%-*.*s|%12.2f|%12s|%12.2f|%12.2f|%12.2f|%12s|\n",
  iceb_u_kolbait(20,gettext("Материальный учёт")),iceb_u_kolbait(20,gettext("Материальный учёт")),gettext("Материальный учёт"),
  suma_mat[nomer_sheta][0],
  "",
  suma_mat[nomer_sheta][1],
  suma_mat[nomer_sheta][2],
  suma_mat[nomer_sheta][3],
  "");

  sprintf(strsql,"%-*.*s|%12.2f|%12s|%12.2f|%12.2f|%12.2f|%12s|\n",
  iceb_u_kolbait(10,gettext("Материальный учёт")),iceb_u_kolbait(10,gettext("Материальный учёт")),gettext("Материальный учёт"),
  suma_mat[nomer_sheta][0],
  "",
  suma_mat[nomer_sheta][1],
  suma_mat[nomer_sheta][2],
  suma_mat[nomer_sheta][3],
  "");
  iceb_printw(strsql,buffer,view,"naim_shrift");

  fprintf(ff,"%-*.*s|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|\n",
  iceb_u_kolbait(20,gettext("Главная книга")),iceb_u_kolbait(20,gettext("Главная книга")),gettext("Главная книга"),
  deb_kre[0],
  deb_kre[1],
  deb_kre[2],
  deb_kre[3],
  deb_kre[4],
  deb_kre[5]);

  sprintf(strsql,"%-*.*s|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|\n",
  iceb_u_kolbait(10,gettext("Главная книга")),iceb_u_kolbait(10,gettext("Главная книга")),gettext("Главная книга"),
  deb_kre[0],
  deb_kre[1],
  deb_kre[2],
  deb_kre[3],
  deb_kre[4],
  deb_kre[5]);
  iceb_printw(strsql,buffer,view,"naim_shrift");

  fprintf(ff,"%-*.*s|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|\n",
  iceb_u_kolbait(20,gettext("Разница")),iceb_u_kolbait(20,gettext("Разница")),gettext("Разница"),
  suma_mat[nomer_sheta][0]-deb_kre[0],
  deb_kre[1],
  suma_mat[nomer_sheta][1]-deb_kre[2],
  suma_mat[nomer_sheta][2]-deb_kre[3],
  suma_mat[nomer_sheta][3]-deb_kre[4],
  deb_kre[5]);

  sprintf(strsql,"%-*.*s|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|%12.2f|\n",
  iceb_u_kolbait(10,gettext("Разница")),iceb_u_kolbait(10,gettext("Разница")),gettext("Разница"),
  suma_mat[nomer_sheta][0]-deb_kre[0],
  deb_kre[1],
  suma_mat[nomer_sheta][1]-deb_kre[2],
  suma_mat[nomer_sheta][2]-deb_kre[3],
  suma_mat[nomer_sheta][3]-deb_kre[4],
  deb_kre[5]);
  
  iceb_printw(strsql,buffer,view,"naim_shrift");
  
 
 
 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------------\n");
return(0);
}


/******************************************/
/******************************************/

gint buhpsdw_r1(class buhpsdw_r_data *data)
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


FILE *ff;
char imaf[64];
sprintf(imaf,"psd%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_zagolov(gettext("Проверка согласованности данных в подсистемах"),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ff);

/*  Смотрим "Материальный учёт"  */
if(iceb_u_proverka(data->rk->podsys.ravno(),ICEB_MP_MATU,0,0) == 0)
  buhpsd_mu(dn,mn,gn,dk,mk,gk,data->rk->shet.ravno(),ff,data->bar,data->view,data->buffer,data->window);

/*Учёт основных средств*/
if(iceb_u_proverka(data->rk->podsys.ravno(),ICEB_MP_UOS,0,0) == 0)
  buhpsd_uos(dn,mn,gn,dk,mk,gk,data->rk->shet.ravno(),ff,data->bar,data->view,data->buffer,data->window);


iceb_podpis(ff,data->window);

fclose(ff);


data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Протокол проверки согласованности данных в подсистемах"));

iceb_ustpeh(imaf,3,data->window);






gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
