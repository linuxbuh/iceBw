/*$Id: zar_soc_nah_r.c,v 1.23 2013/09/26 09:47:00 sasa Exp $*/
/*19.06.2019	16.01.2007	Белых А.И.	zar_soc_nah_r.c
Расчёт свода отчислений в соц-фонды на фонд оплаты труда
*/
#include <math.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_soc_nah.h"

class zar_soc_nah_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class zar_soc_nah_rek *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_soc_nah_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_soc_nah_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_soc_nah_r_data *data);
gint zar_soc_nah_r1(class zar_soc_nah_r_data *data);
void  zar_soc_nah_r_v_knopka(GtkWidget *widget,class zar_soc_nah_r_data *data);

extern SQL_baza bd;
extern short	kodfsons;   /*Код фонда страхования от несчасного случая*/


extern double   okrg;

extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/
extern int kod_esv_vs;

extern int kodf_esv; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_bol; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_inv; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_dog; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_vs; /*Код фонда удержания единого социального взноса*/

int zar_soc_nah_r(class zar_soc_nah_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zar_soc_nah_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт начисления ЕСВ"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_soc_nah_r_key_press),&data);

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

repl.plus(gettext("Расчёт начисления ЕСВ"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_soc_nah_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zar_soc_nah_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_soc_nah_r_v_knopka(GtkWidget *widget,class zar_soc_nah_r_data *data)
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

gboolean   zar_soc_nah_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_soc_nah_r_data *data)
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
/**********************/
/*Поиск суммы отчисления в фонд с работника*/
/********************************************/

double socstr_rud(short mn,short gn,short mk,short gk,int tabnom,int kodud,GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;

char strsql[512];
int kolstr=0;

sprintf(strsql,"select suma from Zarp where datz >= '%04d-%d-%d' and datz <= '%04d-%d-%d' and tabn=%d and prn='2' and knah=%d and suma <> 0.",
gn,mn,1,gk,mk,31,tabnom,kodud);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }

double suma=0.;
while(cur.read_cursor(&row) != 0)
 {
  suma+=atof(row[0]);  
 }

return(suma);
}
/*************************************************************************/
/*Получение списка имеющих начисления и не имеющих отчислений в соц фонды*/
/*************************************************************************/

double	socnul(short mn,short gn,short mk,short gk,
short	kodsf,
FILE *ff,GtkWidget *wpredok)
{
char	strsql[512];
int	kolstr=0;
SQL_str	row,row1;
class iceb_u_str fio("");
double  suma=0.;
double  isuma=0.;
SQLCURSOR curr;
sprintf(strsql,"select distinct tabn from Zarp where \
datz >= '%04d-%d-%d' and datz <= '%04d-%d-%d' and prn='1'",
gn,mn,1,gk,mk,31);

SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }
fprintf(ff,"\n%s:\n",
gettext("Список работников не имеющих отчислений в соц. фонд"));

while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);

  sprintf(strsql,"select distinct kodz from Zarsocz where \
datz >= '%04d-%d-01' and datz <= '%04d-%d-31' and tabn=%s and kodz=%d",
  gn,mn,gk,mk,row[0],kodsf);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 0)
   {
    sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     fio.new_plus(row1[0]);
    else
     fio.new_plus("");

    suma=sosnsumw(row[0],1,mn,gn,31,mk,gk,wpredok);
    isuma+=suma;
    
    if(suma != 0.)
     fprintf(ff,"%5s %-*.*s %2s %11.2f\n",
     row[0],iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno()," ",suma);

   }

 }
if(isuma != 0. && isuma > suma)
 fprintf(ff,"%*s:%11.2f\n",iceb_u_kolbait(39,gettext("Итого по не имеющим удержания")),gettext("Итого по не имеющим удержания"),isuma);
return(isuma); 
}
/*******************************/
/*шапка*/
/*******************************/
void  socstrh(FILE *ff)
{
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 Т/н |Фамилия, имя, отчество        |1 |     2     |     3     |    4     |    5     |  6   |     7     |    8     |     9    |\n");
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------\n");
}
/*************************/
/*Реквизиты расчёта*/
/*************************/
void socstr_rek(class zar_soc_nah_rek *rk,FILE *ff1)
{

if(rk->podr.getdlinna() > 1)
 {
  fprintf(ff1,"%s:%s\n",gettext("Код подразделения"),rk->podr.ravno());
  int klst=0;
  iceb_printcod(ff1,"Podr","kod","naik",0,rk->podr.ravno(),&klst);
 }
else
 fprintf(ff1,gettext("По всем подразделениям.\n"));

if(rk->tabnom.getdlinna() > 1)
 fprintf(ff1,"%s:%s\n",gettext("Табельный номер"),rk->tabnom.ravno());
if(rk->kod_soc.getdlinna() > 1)
 fprintf(ff1,"%s:%s\n",gettext("Код соц. фонда"),rk->kod_soc.ravno());

}

/*************************/
/*Шапка итоговой таблицы*/
/*************************/
void zar_soz_nah_hi(FILE *ff)
{
fprintf(ff,"\n\
------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,
gettext("\
Код|       Наименование фонда     |Проц. |    Счёт   |Сумма в рас| Сумма ЕСВ |     Сумма бюджет      |     Сумма небюджет    |\n"));
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------\n");
}

/******************************************/
/******************************************/

gint zar_soc_nah_r1(class zar_soc_nah_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short mn,gn;
short mk,gk;
iceb_u_rsdat1(&mn,&gn,data->rk->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->rk->datak.ravno());
if(gk == 0)
 {
  mk=mn;
  gk=gn;
 }

sprintf(strsql,"%d.%d > %d.%d\n",mn,gn,mk,gk);

iceb_printw(strsql,data->buffer,data->view);

int kolsoc=0;
/*Определяем количество начислений и удержаний*/
sprintf(strsql,"select kod from Zarsoc");
class SQLCURSOR cur;
SQL_str row;
if((kolsoc=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

if(kolsoc == 0)
 {
  iceb_menu_soob(gettext("Не введены коды соц. отчислений на фонд оплаты труда !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"select distinct kodz,proc,shet from Zarsocz where \
datz >= '%04d-%d-01' and datz <= '%04d-%d-31'",gn,mn,gk,mk);

if((kolsoc=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolsoc == 0)
 {
  iceb_menu_soob(gettext("Не нашли ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_u_spisok kodsocm;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s|%s",row[0],row[1],row[2]);
  kodsocm.plus(strsql);
 }

class iceb_u_double kodsum;
kodsum.make_class(kolsoc);
class iceb_u_double sumasn;
class iceb_u_double sumasnb;
sumasn.make_class(kolsoc);
sumasnb.make_class(kolsoc);


class iceb_u_double kodsumb; //Сумма перечисления в фонд с бюджетной части
kodsumb.make_class(kolsoc);


sprintf(strsql,"select tabn,kodz,sumap,proc,datz,shet,sumas,sumapb,sumasb from \
Zarsocz where datz >= '%04d-%d-01' and datz <= '%04d-%d-31' \
order by kodz,proc,tabn asc",gn,mn,gk,mk);
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не нашли ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
short dkm=1;
iceb_u_dpm(&dkm,&mk,&gk,5);

FILE *ff;
char imaf[64];
sprintf(imaf,"nnfz%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
 }
FILE *ff1;
char imaf1[64];
sprintf(imaf1,"nnfzs%d.lst",getpid());

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
 }
short	kodpenf; /*Код пенсионного фонда*/
short	kodsoc=0;   /*Код фонда социального страхования*/
short kodsocstr=0;  /*Код отчисления на соц-страх*/
short	kodpen;
short	kodbezf=0;  /*Код фонда занятости*/
short kodbzr=0;  /*Код отчисления на безработицу*/
iceb_poldan("Код пенсионного фонда",strsql,"zarnast.alx",data->window);
kodpenf=atoi(strsql);
iceb_poldan("Код пенсионного отчисления",strsql,"zarnast.alx",data->window);
kodpen=atoi(strsql);

iceb_poldan("Код фонда социального страхования",strsql,"zarnast.alx",data->window);
kodsoc=atoi(strsql);

iceb_poldan("Код отчисления в соц-страх",strsql,"zarnast.alx",data->window);
kodsocstr=atoi(strsql);

iceb_poldan("Код фонда занятости",strsql,"zarnast.alx",data->window);
kodbezf=atoi(strsql);

iceb_poldan("Код отчисления на безработицу",strsql,"zarnast.alx",data->window);
kodbzr=atoi(strsql);




iceb_u_zagolov(gettext("Расчёт начисления ЕСВ"),1,mn,gn,dkm,mk,gk,iceb_get_pnk("00",0,data->window),ff);

socstr_rek(data->rk,ff);


iceb_u_zagolov(gettext("Расчёт начисления ЕСВ"),1,mn,gn,dkm,mk,gk,iceb_get_pnk("00",0,data->window),ff1);
socstr_rek(data->rk,ff1);

fprintf(ff1,"\n\
Расшифровка колонок:\n\
1-код фонда\n\
2-общая сумма всех начислений\n\
3-сумма взятая в расчёт\n\
4-сумма налога с фонда оплаты\n\
5-сумма налога для бюджета\n\
6-процент налога\n\
7-счет\n\
8-дата записи\n\
9-сумма налога с работника\n\n");



double sum1=0.,sum2=0.,sum3=0.;
int nomerstr=0;
class iceb_u_spisok FONDSHET; //Список код фонда-счет
class iceb_u_double SUMA; //Массив сумм к списку код фонда-счет
class iceb_u_str tabn("");
double bb=0.;
double		sumkodz[5];
memset(&sumkodz,'\0',sizeof(sumkodz));
float kolstr1=0;
class SQLCURSOR curr;
class iceb_u_str naimf("");
double isumanah=0.;
double sumanah=0.;
short		kodzz=0;
short d,m,g;
SQL_str row1;
class iceb_u_str kod("");
int kodz=0;
int i=0;
double suma=0.;
class iceb_u_str shet("");
class iceb_u_str fio("");
char		metkasum[5];
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->kod_soc.ravno(),row[1],0,0) != 0)
    continue;

  if(data->rk->podr.getdlinna() > 1)
   {
    
    iceb_u_rsdat(&d,&m,&g,row[4],2);

    sprintf(strsql,"select podr from Zarn where god=%d and mes=%d \
and tabn=%s",g,m,row[0]);
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
      continue;    
    kod.new_plus(row1[0]);

    if(iceb_u_proverka(data->rk->podr.ravno(),kod.ravno(),0,0) != 0)
      continue;
    
   }
  kodz=atoi(row[1]);
  if(kodz != kodzz)
   {

    if(kodzz != 0)
     {
      fprintf(ff1,"%*.*s:%11.2f %11.2f %10.2f %10.2f %29s %10.2f\n",
      iceb_u_kolbait(39,naimf.ravno()),iceb_u_kolbait(39,naimf.ravno()),naimf.ravno(),sumkodz[0],sumkodz[1],sumkodz[2],sumkodz[3],"",sumkodz[4]);
      if((bb=socnul(mn,gn,mk,gk,kodzz,ff1,data->window)) != 0.)
       {
        fprintf(ff1,"%*s:%11.2f\n",
        iceb_u_kolbait(39,gettext("Общий итог по фонду")),gettext("Общий итог по фонду"),sumkodz[0]+bb);

       }
      
     }

    //Читаем наименование фонда
    sprintf(strsql,"select naik from Zarsoc where kod=%d",kodz);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
      naimf.new_plus(row1[0]);
    else
      naimf.new_plus("");
    fprintf(ff1,"%d %s\n",kodz,naimf.ravno());
    socstrh(ff1);
  //  isumanah=0.;
    memset(&sumkodz,'\0',sizeof(sumkodz));
    kodzz=kodz;
   }
  sprintf(strsql,"%s|%s|%s",row[1],row[3],row[5]);

  if((i=kodsocm.find(strsql)) < 0)
   {
    char soob[2048];
    sprintf(soob,"Не найдено %s в массиве кодов фондов/процент !\n",strsql);
    iceb_menu_soob(soob,data->window);
    continue;
   }       
  suma=atof(row[2]);
  suma=iceb_u_okrug(suma,okrg);
  
  kodsum.plus(suma,i);
  sum2+=suma;
  sumkodz[2]+=suma;

  suma=atof(row[6]);
  suma=iceb_u_okrug(suma,okrg);
  sumasn.plus(suma,i);
  sumasnb.plus(atof(row[8]),i);
  sum1+=suma;
  sumkodz[1]+=suma;

  suma=atof(row[7]);
  suma=iceb_u_okrug(suma,okrg);
  kodsumb.plus(suma,i);
  sum3+=suma;
  sumkodz[3]+=suma;

  if(iceb_u_SRAV(tabn.ravno(),row[0],0) != 0)
   {
    sprintf(strsql,"select fio,shet from Kartb where tabn=%s",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      
      sprintf(strsql,"%s %s !",gettext("Не найдено табельный номер"),row[0]);
      iceb_menu_soob(strsql,data->window);
      continue;      
     }
    fio.new_plus(row1[0]);
    shet.new_plus(row1[1]);
   }
  iceb_u_rsdat(&d,&m,&g,row[4],2);
  isumanah+=sumanah=sosnsumw(row[0],1,m,g,31,m,g,data->window);
  sumkodz[0]+=sumanah;
  
  memset(metkasum,'\0',sizeof(metkasum));
  if(fabs(sumanah-atof(row[6])) > 0.009)
     metkasum[0]='*';
  else
     metkasum[0]=' ';

  double suma_sr=0.; /*Сумма с работника*/
  
  if(kodpenf  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kodpen,data->window);
  
  if(kodsoc  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kodsocstr,data->window);
  
  if(kodbezf  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kodbzr,data->window);
  /**************/
  if(kodf_esv  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kod_esv,data->window);
  if(kodf_esv_bol  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kod_esv_bol,data->window);
  if(kodf_esv_inv  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kod_esv_inv,data->window);
  if(kodf_esv_dog  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kod_esv_dog,data->window);
  if(kodf_esv_vs  == kodz)
    suma_sr=socstr_rud(mn,gn,mk,gk,atoi(row[0]),kod_esv_vs,data->window);
  
  sumkodz[4]+=suma_sr;
    
  fprintf(ff1,"%5s %-*.*s %2s %11.2f %11s%s%10s %10s %5s%% %-11s %s %10.2f\n",
  row[0],iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno(),row[1],sumanah,row[6],metkasum,row[2],row[7],row[3],row[5],iceb_u_datzap(row[4]),suma_sr);
  tabn.new_plus(row[0]);

  suma=atof(row[2]);
  suma=iceb_u_okrug(suma,okrg);
  sprintf(strsql,"%d|%s",kodzz,shet.ravno());

  if((nomerstr=FONDSHET.find(strsql)) == -1)
    FONDSHET.plus(strsql);
  SUMA.plus(suma,nomerstr);
 }

fprintf(ff1,"%*.*s:%11.2f %11.2f %10.2f %10.2f %29s %10.2f\n",
iceb_u_kolbait(39,naimf.ravno()),iceb_u_kolbait(39,naimf.ravno()),naimf.ravno(),sumkodz[0],sumkodz[1],sumkodz[2],sumkodz[3],"",sumkodz[4]);

if((bb=socnul(mn,gn,mk,gk,kodzz,ff1,data->window)) != 0.)
 {
  fprintf(ff1,"%*s:%11.2f\n",
  iceb_u_kolbait(39,gettext("Общий итог по фонду")),gettext("Общий итог по фонду"),sumkodz[0]+bb);
 }

fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------\n");
fprintf(ff1,"%*s:%11.2f %11.2f %10.2f %10.2f\n",
iceb_u_kolbait(39,gettext("Общий итог по всем фондам")),gettext("Общий итог по всем фондам"),isumanah+bb,sum1,sum2,sum3);

double itogsn=0.,itogo=0.,itogob=0.;

zar_soz_nah_hi(ff); /*Шапка итоговой талицы*/
zar_soz_nah_hi(ff1); /*Шапка итоговой талицы*/

/*Распечатываем массивы*/
class iceb_u_str procent("");

gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace","foreground","red",NULL);

for(int i=0; i < kolsoc ; i++)
 {
  if(kodsum.ravno(i) == 0.)
    continue;
  strncpy(strsql,kodsocm.ravno(i),sizeof(strsql)-1);
  iceb_u_polen(strsql,&kod,1,'|');
  iceb_u_polen(strsql,&procent,2,'|');
  iceb_u_polen(strsql,&shet,3,'|');
  sprintf(strsql,"select naik,shet from Zarsoc where kod=%s",kod.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    naimf.new_plus(row[0]);
   }
  else
    naimf.new_plus("");


  sprintf(strsql,"%3s %-*.*s %5s%% %-*s %10.2f %10.2f %10.2f\n",
  kod.ravno(),
  iceb_u_kolbait(30,naimf.ravno()),iceb_u_kolbait(30,naimf.ravno()),naimf.ravno(),
  procent.ravno(),
  iceb_u_kolbait(10,shet.ravno()),shet.ravno(),
  sumasn.ravno(i),kodsum.ravno(i),kodsumb.ravno(i));

  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

  fprintf(ff,"%3s %-*.*s %5s%% %-*s %11.2f %11.2f %11.2f/%11.2f %11.2f/%11.2f\n",
  kod.ravno(),iceb_u_kolbait(30,naimf.ravno()),iceb_u_kolbait(30,naimf.ravno()),naimf.ravno(),
  procent.ravno(),
  iceb_u_kolbait(11,shet.ravno()),shet.ravno(),
  sumasn.ravno(i),kodsum.ravno(i),kodsumb.ravno(i),
  sumasnb.ravno(i),
  kodsum.ravno(i)-kodsumb.ravno(i),
  sumasn.ravno(i)-sumasnb.ravno(i));


  fprintf(ff1,"%3s %-*.*s %5s%% %-*s %11.2f %11.2f %11.2f/%11.2f %11.2f/%11.2f\n",
  kod.ravno(),
  iceb_u_kolbait(30,naimf.ravno()),iceb_u_kolbait(30,naimf.ravno()),naimf.ravno(),
  procent.ravno(),
  iceb_u_kolbait(11,shet.ravno()),shet.ravno(),
  sumasn.ravno(i),kodsum.ravno(i),kodsumb.ravno(i),
  
  sumasnb.ravno(i),
  kodsum.ravno(i)-kodsumb.ravno(i),
  sumasn.ravno(i)-sumasnb.ravno(i));

  itogo+=kodsum.ravno(i);
  itogob+=kodsumb.ravno(i);
  itogsn+=sumasn.ravno(i);
 }
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%11.2f %11.2f %11.2f %11s %11.2f\n",
iceb_u_kolbait(53,gettext("Итого")),gettext("Итого"),itogsn,itogo,itogob,"",itogo-itogob);


fprintf(ff1,"\
------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff1,"%*s:%11.2f %11.2f %11.2f %11s %11.2f\n",
iceb_u_kolbait(53,gettext("Итого")),gettext("Итого"),itogsn,itogo,itogob,"",itogo-itogob);

sprintf(strsql,"%*s:%10.2f %10.2f %10.2f\n",
iceb_u_kolbait(53,gettext("Итого")),gettext("Итого"),itogsn,itogo,itogob);

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

iceb_podpis(ff,data->window);
iceb_podpis(ff1,data->window);

fclose(ff);
fclose(ff1);

data->rk->imaf.plus(imaf1);
data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчёт начисления ЕСВ по работникам"));
data->rk->naimf.plus(gettext("Общие итоги"));


for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),1,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}

