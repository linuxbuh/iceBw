/*$Id: zar_mg_r.c,v 1.21 2013/09/26 09:47:00 sasa Exp $*/
/*11.04.2018	06.12.2006	Белых А.И.	zar_mg_r.c
Расчёт распределения зарплаты мужчины/женщины
*/
#include <errno.h>
#include "buhg_g.h"

class zar_mg_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  const char *data_r;
  class iceb_u_str kod_pod;
  class iceb_u_spisok *imaf;
  class iceb_u_spisok *naimf;
      
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_mg_r_data()
   {
    voz=1;
    kon_ras=1;
    kod_pod.new_plus("");
   }
 };
gboolean   zar_mg_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_mg_r_data *data);
gint zar_mg_r1(class zar_mg_r_data *data);
void  zar_mg_r_v_knopka(GtkWidget *widget,class zar_mg_r_data *data);

extern SQL_baza bd;

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_mg_r(const char *data_r,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zar_mg_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);

data.data_r=data_r;
data.imaf=imaf;
data.naimf=naimf;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать распределение зарплаты мужчины/женщины"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_mg_r_key_press),&data);

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

repl.plus(gettext("Распечатать распределение зарплаты мужчины/женщины"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_mg_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);



g_idle_add((GSourceFunc)zar_mg_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_mg_r_v_knopka(GtkWidget *widget,class zar_mg_r_data *data)
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

gboolean   zar_mg_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_mg_r_data *data)
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



/*******************************************************/
/****************************************************/
void raspol_rm(short metka,/*0-мужчины 2-мужчины совместители 1-женщины 3-женщины совместители*/
double tablic[4][3],
int kolmug,
int koltabel,
int kolnah,
class iceb_u_int *NASH,
class iceb_u_int *TABEL,
class iceb_u_int *DNIMUG,
class iceb_u_int *HASMUG,
class iceb_u_int *KDNMUG,
class iceb_u_double *ZNAHMUG,
double *itogo,
FILE *ff,FILE *ffi,
class zar_mg_r_data *data)
{
SQL_str row;
class SQLCURSOR curr;
char strsql[1024];

if(metka == 0)
 {
  fprintf(ff,"\n%s\n",gettext("Пол:мужчина"));
  sprintf(strsql,"\n%s\n",gettext("Пол:мужчина"));
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
 }
if(metka == 2)
 {
  fprintf(ff,"\n%s\n",gettext("В том числе мужчины совместители"));
  sprintf(strsql,"\n%s\n",gettext("В том числе мужчины совместители"));
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
 }
if(metka == 1)
 {
  fprintf(ff,"\n%s\n",gettext("Пол:женщина"));
  sprintf(strsql,"\n%s\n",gettext("Пол:женщина"));
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
 }
if(metka == 3)
 {
  fprintf(ff,"\n%s\n",gettext("В том числе женщины совместители"));
  sprintf(strsql,"\n%s\n",gettext("В том числе женщины совместители"));
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
 }

fprintf(ff,"----------------------------------------------------------\n");

fprintf(ff,"\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%10d\n\n",
iceb_u_kolbait(15,gettext("Основная")),gettext("Основная"),
tablic[metka][0],
iceb_u_kolbait(15,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[metka][1],
iceb_u_kolbait(15,gettext("Прочая")),gettext("Прочая"),
tablic[metka][2],
iceb_u_kolbait(15,gettext("Итого")),gettext("Итого"),
tablic[metka][0]+tablic[metka][1]+tablic[metka][2],
iceb_u_kolbait(15,gettext("Количество")),gettext("Количество"),kolmug);

sprintf(strsql,"\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%10d\n\n",
iceb_u_kolbait(15,gettext("Основная")),gettext("Основная"),
tablic[metka][0],
iceb_u_kolbait(15,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[metka][1],
iceb_u_kolbait(15,gettext("Прочая")),gettext("Прочая"),
tablic[metka][2],
iceb_u_kolbait(15,gettext("Итого")),gettext("Итого"),
tablic[metka][0]+tablic[metka][1]+tablic[metka][2],
iceb_u_kolbait(15,gettext("Количество")),gettext("Количество"),kolmug);

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

int s1=0,s2=0,s3=0;
int kodt=0;
class iceb_u_str naimtab("");
for(int i=0; i < koltabel; i++)
 {
  kodt=TABEL->ravno(i);
  sprintf(strsql,"select naik from Tabel where kod=%d",kodt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimtab.new_plus(row[0]);
  else
    naimtab.new_plus("");

  fprintf(ff,"%3d %-*.*s %10d %10d %10d\n",
  kodt,
  iceb_u_kolbait(30,naimtab.ravno()),iceb_u_kolbait(30,naimtab.ravno()),naimtab.ravno(),
  DNIMUG->ravno(i),HASMUG->ravno(i),
  KDNMUG->ravno(i));
  s1+=DNIMUG->ravno(i);
  s2+=HASMUG->ravno(i);
  s3+=KDNMUG->ravno(i);

 }
fprintf(ff,"%3s %*s %10d %10d %10d\n\n",
" ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),s1,s2,s3);
class iceb_u_str vidn("");
double sum=0.;
for(int i=0; i < kolnah; i++)
 {
  if(ZNAHMUG->ravno(i) == 0.)
    continue;
  kodt=NASH->ravno(i);
  sprintf(strsql,"select naik,vidn from Nash where kod=%d",kodt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   {
    naimtab.new_plus(row[0]);
    if(atoi(row[1]) == 0)
     vidn.new_plus("основная");
    if(atoi(row[1]) == 1)
     vidn.new_plus("дополнительная");
    if(atoi(row[1]) == 2)
     vidn.new_plus("прочая");
    }
  else
   {
    naimtab.new_plus("");
    vidn.new_plus("");
   }
  fprintf(ff,"%3d %-*.*s %12.2f %s\n",
  kodt,
  iceb_u_kolbait(30,naimtab.ravno()),iceb_u_kolbait(30,naimtab.ravno()),naimtab.ravno(),
  ZNAHMUG->ravno(i),vidn.ravno());
  sum+=ZNAHMUG->ravno(i);
 }

fprintf(ff,"%3s %*s %12.2f\n"," ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),sum);
*itogo=sum;

if(metka == 0)
 fprintf(ffi,"%*s:%12.2f\n",
iceb_u_kolbait(40,gettext("Мужчины")),
gettext("Мужчины"),
sum);

if(metka == 2)
  fprintf(ffi,"%*s:%12s %12.2f\n",
iceb_u_kolbait(40,gettext("В том числе мужчины совместители")),
gettext("В том числе мужчины совместители"),
"",
sum);

if(metka == 1)
 fprintf(ffi,"%*s:%12.2f\n",
iceb_u_kolbait(40,gettext("Женщины")),
gettext("Мужчины"),
sum);

if(metka == 3)
  fprintf(ffi,"%*s:%12s %12.2f\n",
iceb_u_kolbait(40,gettext("В том числе женщины совместители")),
gettext("В том числе женщины совместители"),
"",
sum);

}




/******************************************/
/******************************************/

gint zar_mg_r1(class zar_mg_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[2048];
class iceb_clock sss(data->window);


short mn,gn;
iceb_u_rsdat1(&mn,&gn,data->data_r);


//Создаем массивы для видов табеля

class iceb_u_int TABEL;

sprintf(strsql,"select kod from Tabel");


class SQLCURSOR cur,curr;
SQL_str row,row1,row2;
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
  iceb_menu_soob(gettext("Не введены виды табеля !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int koltabel=0;
while(cur.read_cursor(&row) != 0)
 {
  koltabel++;
  TABEL.plus(atoi(row[0]),-1);
 }

class iceb_u_int DNIMUG;
class iceb_u_int HASMUG;
class iceb_u_int KDNMUG;
class iceb_u_int DNIMUG_S;
class iceb_u_int HASMUG_S;
class iceb_u_int KDNMUG_S;

class iceb_u_int DNIGEN;
class iceb_u_int HASGEN;
class iceb_u_int KDNGEN;
class iceb_u_int DNIGEN_S;
class iceb_u_int HASGEN_S;
class iceb_u_int KDNGEN_S;

DNIMUG.make_class(koltabel);
HASMUG.make_class(koltabel);
KDNMUG.make_class(koltabel);
DNIMUG_S.make_class(koltabel);
HASMUG_S.make_class(koltabel);
KDNMUG_S.make_class(koltabel);

DNIGEN.make_class(koltabel);
HASGEN.make_class(koltabel);
KDNGEN.make_class(koltabel);

DNIGEN_S.make_class(koltabel);
HASGEN_S.make_class(koltabel);
KDNGEN_S.make_class(koltabel);


//Создаем массивы для видов начислений

class iceb_u_int NASH;

sprintf(strsql,"select kod from Nash");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не введены начисления !"),data->window);
  return(1);  
 }
int kolnah=0;
while(cur.read_cursor(&row) != 0)
 {
  kolnah++;
  NASH.plus(atoi(row[0]),-1);
 }

class iceb_u_double ZNAHMUG;
class iceb_u_double ZNAHMUG_S;
class iceb_u_double ZNAHGEN;
class iceb_u_double ZNAHGEN_S;
ZNAHMUG.make_class(kolnah);
ZNAHMUG_S.make_class(kolnah);
ZNAHGEN.make_class(kolnah);
ZNAHGEN_S.make_class(kolnah);

double tablic[4][3]; 
memset(&tablic,'\0',sizeof(tablic));
sprintf(strsql,"select tabn,datk,sovm,podr from Zarn where god=%d and mes=%d",gn,mn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_prot[64];
sprintf(imaf_prot,"pol%d.lst",getpid());

class iceb_fopen ff_prot;
if(ff_prot.start(imaf_prot,"w",data->window) != 0)
 return(1);
 
short metka_sovm=0;
float kolstr1=0;
class SQLCURSOR cur1;  
double suma_r=0.; /*сумма начисленная одному работнику*/
int kolgen=0,kolgen_s=0;
int kolmug=0,kolmug_s=0;
int kolstr2=0;
int kodt=0;
double sum=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
//  strzag(LINES-1,0,kolstr,++kolstr1);
  if(iceb_u_proverka(data->kod_pod.ravno(),row[3],0,0) != 0)
   continue;
   
  metka_sovm=atoi(row[2]);
  
  sprintf(strsql,"select pl,fio from Kartb where tabn=%s",row[0]);
  int pol=0;
  
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
    pol=atoi(row1[0]);
  else
   {
    sprintf(strsql,"%s %s\n",gettext("Не найден табельный номер"),row[0]);
    iceb_beep();
    iceb_printw(strsql,data->buffer,data->view);
    continue;
   }
  if(pol == 0)
   {
    kolmug++;  
    if(metka_sovm == 1)
     kolmug_s++;
   }
  if(pol == 1)
   {
    kolgen++;  
    if(metka_sovm == 1)
     kolgen_s++;
   }
  if(pol == 0)
   {
    fprintf(ff_prot.ff,"\n%-5s %-5s %-*.*s мужчина",row[0],row[3],iceb_u_kolbait(30,row1[1]),iceb_u_kolbait(30,row1[1]),row1[1]);
   }
 
  if(pol == 1)
   {
    fprintf(ff_prot.ff,"\n%-5s %-5s %-*.*s женщина",row[0],row[3],iceb_u_kolbait(30,row1[1]),iceb_u_kolbait(30,row1[1]),row1[1]);
   }   

  sprintf(strsql,"%6s %s\n",row[0],row1[1]);
  iceb_printw(strsql,data->buffer,data->view);

  if(metka_sovm == 1)
   fprintf(ff_prot.ff," %s\n",gettext("совместитель"));
  else
   fprintf(ff_prot.ff,"\n");

  //Читаем отработанное время
  sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where god=%d and \
mes=%d and tabn=%s",gn,mn,row[0]);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  int i=0;
  int it=0;
  while(cur1.read_cursor(&row1) != 0)
   {
    kodt=atoi(row1[0]);
    i=TABEL.find(kodt);
    it+=atoi(row1[1])+atoi(row1[2])+atoi(row1[3]);
    if(pol == 0)
     {
      DNIMUG.plus(atoi(row1[1]),i);
      HASMUG.plus(atoi(row1[2]),i);
      KDNMUG.plus(atoi(row1[3]),i);
      if(metka_sovm == 1)
       {
        DNIMUG_S.plus(atoi(row1[1]),i);
        HASMUG_S.plus(atoi(row1[2]),i);
        KDNMUG_S.plus(atoi(row1[3]),i);
       }
     }
    if(pol == 1)
     {
      DNIGEN.plus(atoi(row1[1]),i);
      HASGEN.plus(atoi(row1[2]),i);
      KDNGEN.plus(atoi(row1[3]),i);
      if(metka_sovm == 1)
       {
        DNIGEN_S.plus(atoi(row1[1]),i);
        HASGEN_S.plus(atoi(row1[2]),i);
        KDNGEN_S.plus(atoi(row1[3]),i);
       }
     }
   }

  if(it != 0) /*если введён табель то человек не уволен даже если ему ничего не начислено*/
   {
    fprintf(ff_prot.ff,"Введён табель\n");
    if(pol == 0)
     {
      kolmug++;  
      if(metka_sovm == 1)
       kolmug_s++;
     }
    if(pol == 1)
     {
      kolgen++;  
      if(metka_sovm == 1)
       kolgen_s++;
     }
   }
  else
   fprintf(ff_prot.ff,"Не введён табель\n");
   
  sprintf(strsql,"select knah,suma from Zarp where datz >= '%d-%d-1' \
and datz <= '%d-%d-31' and tabn=%s and prn='1' and suma <> 0.",
  gn,mn,gn,mn,row[0]);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  suma_r=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
//    printw("%s %s\n",row1[0],row1[1]);
//    refresh();
    suma_r+=sum=atof(row1[1]);
    sprintf(strsql,"select vidn from Nash where kod=%s",row1[0]);
    if(iceb_sql_readkey(strsql,&row2,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код начисления"),row1[0]);
      iceb_printw(strsql,data->buffer,data->view);
      iceb_beep();
     }
    int kom1=atoi(row2[0]);
    tablic[pol][kom1]+=sum;
    if(metka_sovm == 1)
     tablic[pol+2][kom1]+=sum;
    
    i=NASH.find(atoi(row1[0]));
    if(pol == 0)
     { 
      ZNAHMUG.plus(sum,i);
      if(metka_sovm == 1)
       ZNAHMUG_S.plus(sum,i);
     }
    if(pol == 1)
     { 
      ZNAHGEN.plus(sum,i);
      if(metka_sovm == 1)
        ZNAHGEN_S.plus(sum,i);
      
     }

   }    
  fprintf(ff_prot.ff,"Начислено %.2f\n",suma_r);
 }

ff_prot.end();


class iceb_fopen fil;
char imaf[64];
sprintf(imaf,"m-g%d.lst",getpid());

if(fil.start(imaf,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
short dkm=1;
iceb_u_dpm(&dkm,&mn,&gn,5);

iceb_zagolov(gettext("Расчёт распределения начислений мужчина/женщина"),1,mn,gn,dkm,mn,gn,fil.ff,data->window);


class iceb_fopen oi;
char imafi[64];
sprintf(imafi,"poli%d.lst",getpid());
if(oi.start(imafi,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_zagolov(gettext("Расчёт распределения начислений мужчина/женщина"),1,mn,gn,dkm,mn,gn,oi.ff,data->window);

if(data->kod_pod.getdlinna() > 1)
 {
  fprintf(fil.ff,"%s:%s\n",gettext("Код подразделения"),data->kod_pod.ravno());
  fprintf(oi.ff,"%s:%s\n",gettext("Код подразделения"),data->kod_pod.ravno());
 }
gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);

double itogo_mug=0.;
double itogo_mug_sov=0.;
double itogo_gen=0.;
double itogo_gen_sov=0.;

raspol_rm(0,tablic,kolmug,koltabel,kolnah,&NASH,&TABEL,&DNIMUG,&HASMUG,&KDNMUG,&ZNAHMUG,&itogo_mug,fil.ff,oi.ff,data);
raspol_rm(2,tablic,kolmug_s,koltabel,kolnah,&NASH,&TABEL,&DNIMUG_S,&HASMUG_S,&KDNMUG_S,&ZNAHMUG_S,&itogo_mug_sov,fil.ff,oi.ff,data);

raspol_rm(1,tablic,kolgen,koltabel,kolnah,&NASH,&TABEL,&DNIGEN,&HASGEN,&KDNGEN,&ZNAHGEN,&itogo_gen,fil.ff,oi.ff,data);
raspol_rm(3,tablic,kolgen_s,koltabel,kolnah,&NASH,&TABEL,&DNIGEN_S,&HASGEN_S,&KDNGEN_S,&ZNAHGEN_S,&itogo_gen_sov,fil.ff,oi.ff,data);

#if 0
############################################################33
fprintf(fil.ff,"\n%s\n",gettext("Пол:мужчина"));
fprintf(fil.ff,"----------------------------------------------------------\n");

fprintf(fil.ff,"\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%10d\n\n",
iceb_u_kolbait(15,gettext("Основная")),gettext("Основная"),
tablic[0][0],
iceb_u_kolbait(15,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[0][1],
iceb_u_kolbait(15,gettext("Прочая")),gettext("Прочая"),
tablic[0][2],
iceb_u_kolbait(15,gettext("Итого")),gettext("Итого"),
tablic[0][0]+tablic[0][1]+tablic[0][2],
iceb_u_kolbait(15,gettext("Количество")),gettext("Количество"),kolmug);

int s1=0,s2=0,s3=0;
class iceb_u_str naimtab("");
for(int i=0; i < koltabel; i++)
 {
  kodt=TABEL.ravno(i);
  sprintf(strsql,"select naik from Tabel where kod=%d",kodt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimtab.new_plus(row[0]);
  else
    naimtab.new_plus("");

  fprintf(fil.ff,"%3d %-*.*s %10d %10d %10d\n",
  kodt,
  iceb_u_kolbait(30,naimtab.ravno()),iceb_u_kolbait(30,naimtab.ravno()),naimtab.ravno(),
  DNIMUG.ravno(i),HASMUG.ravno(i),
  KDNMUG.ravno(i));
  s1+=DNIMUG.ravno(i);
  s2+=HASMUG.ravno(i);
  s3+=KDNMUG.ravno(i);

 }
fprintf(fil.ff,"%3s %*s %10d %10d %10d\n\n",
" ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),s1,s2,s3);

sum=0.;
for(int i=0; i < kolnah; i++)
 {
  if(ZNAHMUG.ravno(i) == 0.)
    continue;
  kodt=NASH.ravno(i);
  sprintf(strsql,"select naik from Nash where kod=%d",kodt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimtab.new_plus(row[0]);
  else
    naimtab.new_plus("");

  fprintf(fil.ff,"%3d %-*.*s %12.2f\n",
  kodt,
  iceb_u_kolbait(30,naimtab.ravno()),iceb_u_kolbait(30,naimtab.ravno()),naimtab.ravno(),
  ZNAHMUG.ravno(i));
  sum+=ZNAHMUG.ravno(i);
 }

fprintf(fil.ff,"%3s %*s %12.2f\n"," ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),sum);
double itogo_mug=sum;

fprintf(oi.ff,"%*s:%12.2f\n",
iceb_u_kolbait(40,gettext("Мужчины")),
gettext("Мужчины"),
sum);


fprintf(fil.ff,"\n%s\n",gettext("В том числе мужчины совместители"));
fprintf(fil.ff,"----------------------------------------------------------\n");

fprintf(fil.ff,"\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%10d\n\n",
iceb_u_kolbait(15,gettext("Основная")),gettext("Основная"),
tablic[2][0],
iceb_u_kolbait(15,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[2][1],
iceb_u_kolbait(15,gettext("Прочая")),gettext("Прочая"),
tablic[2][2],
iceb_u_kolbait(15,gettext("Итого")),gettext("Итого"),
tablic[2][0]+tablic[2][1]+tablic[2][2],
iceb_u_kolbait(15,gettext("Количество")),gettext("Количество"),kolmug_s);
s1=s2=s3=0;
for(int i=0; i < koltabel; i++)
 {
  kodt=TABEL.ravno(i);
  sprintf(strsql,"select naik from Tabel where kod=%d",kodt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimtab.new_plus(row[0]);
  else
    naimtab.new_plus("");

  fprintf(fil.ff,"%3d %-*.*s %10d %10d %10d\n",
  kodt,
  iceb_u_kolbait(30,naimtab.ravno()),iceb_u_kolbait(30,naimtab.ravno()),naimtab.ravno(),
  DNIMUG_S.ravno(i),
  HASMUG_S.ravno(i),
  KDNMUG_S.ravno(i));
  s1+=DNIMUG_S.ravno(i);
  s2+=HASMUG_S.ravno(i);
  s3+=KDNMUG_S.ravno(i);

 }
fprintf(fil.ff,"%3s %*s %10d %10d %10d\n\n"," ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),s1,s2,s3);

sum=0.;
for(int i=0; i < kolnah; i++)
 {
  if(ZNAHMUG_S.ravno(i) == 0.)
    continue;
  kodt=NASH.ravno(i);
  sprintf(strsql,"select naik from Nash where kod=%d",kodt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimtab.new_plus(row[0]);
  else
    naimtab.new_plus("");

  fprintf(fil.ff,"%3d %-*.*s %12.2f\n",
  kodt,
  iceb_u_kolbait(30,naimtab.ravno()),iceb_u_kolbait(30,naimtab.ravno()),naimtab.ravno(),
  ZNAHMUG_S.ravno(i));
  sum+=ZNAHMUG_S.ravno(i);
 }
fprintf(fil.ff,"%3s %*s %12.2f\n"," ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),sum);

fprintf(oi.ff,"%*s:%12s %12.2f\n",
iceb_u_kolbait(40,gettext("В том числе мужчины совместители")),
gettext("В том числе мужчины совместители"),
"",
sum);


sprintf(strsql,"\n%s\n",gettext("Пол:мужчина"));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"---------------------------------\n");
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%10d\n",
iceb_u_kolbait(15,gettext("Основная")),gettext("Основная"),
tablic[0][0],
iceb_u_kolbait(15,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[0][1],
iceb_u_kolbait(15,gettext("Прочая")),gettext("Прочая"),
tablic[0][2],
iceb_u_kolbait(15,gettext("Итого")),gettext("Итого"),
tablic[0][0]+tablic[0][1]+tablic[0][2],
iceb_u_kolbait(15,gettext("Количество")),gettext("Количество"),kolmug);

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

fprintf(fil.ff,"\n%s\n",gettext("Пол:женщина"));
fprintf(fil.ff,"----------------------------------------------------------\n");

fprintf(fil.ff,"\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%10d\n",
iceb_u_kolbait(15,gettext("Основная")),gettext("Основная"),
tablic[1][0],
iceb_u_kolbait(15,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[1][1],
iceb_u_kolbait(15,gettext("Прочая")),gettext("Прочая"),
tablic[1][2],
iceb_u_kolbait(15,gettext("Итого")),gettext("Итого"),
tablic[1][0]+tablic[1][1]+tablic[1][2],
iceb_u_kolbait(15,gettext("Количество")),gettext("Количество"),kolgen);

s1=s2=s3=0;
for(int i=0; i < koltabel; i++)
 {
  kodt=TABEL.ravno(i);
  sprintf(strsql,"select naik from Tabel where kod=%d",kodt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimtab.new_plus(row[0]);

  fprintf(fil.ff,"%3d %-*.*s %10d %10d %10d\n",
  kodt,
  iceb_u_kolbait(30,naimtab.ravno()),iceb_u_kolbait(30,naimtab.ravno()),naimtab.ravno(),
  DNIGEN.ravno(i),HASGEN.ravno(i),
  KDNGEN.ravno(i));
  s1+=DNIGEN.ravno(i);
  s2+=HASGEN.ravno(i);
  s3+=KDNGEN.ravno(i);
 }

fprintf(fil.ff,"%3s %*s %10d %10d %10d\n\n"," ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),s1,s2,s3);

sum=0.;
for(int i=0; i < kolnah; i++)
 {
  if(ZNAHGEN.ravno(i) == 0.)
    continue;
  kodt=NASH.ravno(i);
  sprintf(strsql,"select naik from Nash where kod=%d",kodt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimtab.new_plus(row[0]);
  else
    naimtab.new_plus("");

  fprintf(fil.ff,"%3d %-*.*s %12.2f\n",
  kodt,
  iceb_u_kolbait(30,naimtab.ravno()),iceb_u_kolbait(30,naimtab.ravno()),naimtab.ravno(),
  ZNAHGEN.ravno(i));
  sum+=ZNAHGEN.ravno(i);
 }

fprintf(fil.ff,"%3s %*s %12.2f\n"," ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),sum);

fprintf(oi.ff,"\n%*s:%12.2f\n",
iceb_u_kolbait(40,gettext("Женщины")),
gettext("Женщины"),
sum);

double itogo_gen=sum;

fprintf(fil.ff,"\n%s\n",gettext("В том числе женщины совместители"));
fprintf(fil.ff,"----------------------------------------------------------\n");

fprintf(fil.ff,"\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%10d\n",
iceb_u_kolbait(15,gettext("Основная")),gettext("Основная"),
tablic[3][0],
iceb_u_kolbait(15,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[3][1],
iceb_u_kolbait(15,gettext("Прочая")),gettext("Прочая"),
tablic[3][2],
iceb_u_kolbait(15,gettext("Итого")),gettext("Итого"),
tablic[3][0]+tablic[3][1]+tablic[3][2],
iceb_u_kolbait(15,gettext("Количество")),gettext("Количество"),kolgen_s);
s1=s2=s3=0;
for(int i=0; i < koltabel; i++)
 {
  kodt=TABEL.ravno(i);
  sprintf(strsql,"select naik from Tabel where kod=%d",kodt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimtab.new_plus(row[0]);
  else
    naimtab.new_plus("");

  fprintf(fil.ff,"%2d %-*.*s %10d %10d %10d\n",
  kodt,
  iceb_u_kolbait(30,naimtab.ravno()),iceb_u_kolbait(30,naimtab.ravno()),naimtab.ravno(),
  DNIGEN_S.ravno(i),HASGEN_S.ravno(i),
  KDNGEN_S.ravno(i));
  s1+=DNIGEN_S.ravno(i);
  s2+=HASGEN_S.ravno(i);
  s3+=KDNGEN_S.ravno(i);
 }

fprintf(fil.ff,"%2s %*s %10d %10d %10d\n\n",
" ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),s1,s2,s3);

sum=0.;
for(int i=0; i < kolnah; i++)
 {
  if(ZNAHGEN_S.ravno(i) == 0.)
    continue;
  kodt=NASH.ravno(i);
  sprintf(strsql,"select naik from Nash where kod=%d",kodt);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    naimtab.new_plus(row[0]);
  else
    naimtab.new_plus("");

  fprintf(fil.ff,"%3d %-*.*s %12.2f\n",
  kodt,
  iceb_u_kolbait(30,naimtab.ravno()),iceb_u_kolbait(30,naimtab.ravno()),naimtab.ravno(),
  ZNAHGEN_S.ravno(i));
  sum+=ZNAHGEN_S.ravno(i);
 }

fprintf(fil.ff,"%3s %*s %12.2f\n",
" ",iceb_u_kolbait(30,gettext("Итого")),gettext("Итого"),sum);

fprintf(oi.ff,"%*s:%12s %12.2f\n",
iceb_u_kolbait(40,gettext("В том числе женщины совместители")),
gettext("В том числе женщины совместители"),
"",
sum);





sprintf(strsql,"\n%s\n",gettext("Пол:женщина"));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"---------------------------------\n");
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%12.2f\n\
%*s:%10d\n",
iceb_u_kolbait(15,gettext("Основная")),gettext("Основная"),
tablic[1][0],
iceb_u_kolbait(15,gettext("Дополнительная")),gettext("Дополнительная"),
tablic[1][1],
iceb_u_kolbait(15,gettext("Прочая")),gettext("Прочая"),
tablic[1][2],
iceb_u_kolbait(15,gettext("Итого")),gettext("Итого"),
tablic[1][0]+tablic[1][1]+tablic[1][2],
iceb_u_kolbait(15,gettext("Количество")),gettext("Количество"),kolgen);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
##################################################################3
#endif
sprintf(strsql,"%s:%.2f\n",gettext("Ощая сумма всех начислений"),itogo_gen+itogo_mug);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

fprintf(oi.ff,"%*s:%12.2f\n",
iceb_u_kolbait(40,gettext("Ощая сумма всех начислений")),
gettext("Ощая сумма всех начислений"),itogo_gen+itogo_mug);

fprintf(fil.ff,"\n%*s:%12.2f\n",
iceb_u_kolbait(34,gettext("Ощая сумма всех начислений")),
gettext("Ощая сумма всех начислений"),itogo_gen+itogo_mug);

iceb_podpis(fil.ff,data->window);
fil.end();

iceb_podpis(oi.ff,data->window);
oi.end();




















data->imaf->plus(imaf);
data->naimf->plus(gettext("Расчёт распределения зарплат мужчины/женщины"));
data->imaf->plus(imafi);
data->naimf->plus(gettext("Ощие итоги"));

data->imaf->plus(imaf_prot);
data->naimf->plus(gettext("Протокол хода расчёта"));


for(int nom=0; nom < data->imaf->kolih(); nom++)
 iceb_ustpeh(data->imaf->ravno(nom),0,data->window);

data->kon_ras=0;
data->voz=0;
gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

return(FALSE);

}
