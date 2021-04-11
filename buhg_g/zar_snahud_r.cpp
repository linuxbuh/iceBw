/*$Id: zar_snahud_r.c,v 1.18 2013/09/26 09:47:00 sasa Exp $*/
/*16.08.2016	30.01.2007	Белых А.И.	zar_snahud1_r.c
Расчёт свода начислений и удержаиний
*/
#include <errno.h>
#include "buhg_g.h"
#include "zar_snahud.h"

class zar_snahud1_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class zar_snahud_rek *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_snahud1_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_snahud1_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_snahud1_r_data *data);
gint zar_snahud1_r1(class zar_snahud1_r_data *data);
void  zar_snahud1_r_v_knopka(GtkWidget *widget,class zar_snahud1_r_data *data);

void snu_ras(int metka,class iceb_u_int *kod,double *suma,FILE *ff,GtkWidget *wpredok);

extern SQL_baza bd;


int zar_snahud_r(class zar_snahud_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zar_snahud1_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать свод начислений и удержаний"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_snahud1_r_key_press),&data);

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

repl.plus(gettext("Распечатать свод начислений и удержаний"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_snahud1_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zar_snahud1_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_snahud1_r_v_knopka(GtkWidget *widget,class zar_snahud1_r_data *data)
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

gboolean   zar_snahud1_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_snahud1_r_data *data)
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
/***************************/
/*печать тивой формы N6,7*/
/*****************************/
int snu_tf(short mn,short gn,
short mk,short gk,
const char *tabnom,
class iceb_u_int *kodn,
double *suma_n,
class iceb_u_int *kodu,
double *suma_u,
const char *imaf,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str naim("");

class iceb_fopen fil;
if(fil.start(imaf,"w",wpredok) != 0)
 return(1);
if(tabnom[0] != '\0')
 fprintf(fil.ff,"%60s Типова форма N П-6\n","");
else
 fprintf(fil.ff,"%60s Типова форма N П-7\n","");

fprintf(fil.ff,"%60s ЗАТВЕРДЖЕНО\n","");
fprintf(fil.ff,"%60s наказом Деркомстату України\n","");
fprintf(fil.ff,"%60s від 5 грудня 2008 р. N 489\n\n","");

if(tabnom[0] == '\0')
 fprintf(fil.ff,"%30sРОЗРАХУНКОВО-ПЛАТІЖНА ВІДОМІСТЬ (зведена)\n","");
else
 {
  class iceb_u_str fio;
  class iceb_u_str tn;
  
  fprintf(fil.ff,"%30sРОЗРАХУНКОВО-ПЛАТІЖНА ВІДОМІСТЬ ПРАЦІВНИКА\n","");
  int kolr=iceb_u_pole2(tabnom,',');
  if(kolr == 0)
   kolr=1;

  for(int nom=1; nom <= kolr; nom++)
   {
    if(iceb_u_polen(tabnom,&tn,nom,',') != 0)
     tn.new_plus(tabnom);

    sprintf(strsql,"select fio from Kartb where tabn=%d",tn.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     fio.new_plus(row[0]);
    else
     fio.new_plus("");
    fprintf(fil.ff,"%30s%s %s\n","",tn.ravno(),fio.ravno());
   }       
 } 
short d;
iceb_u_dpm(&d,&mk,&gk,5);

fprintf(fil.ff,"%30sза період з 01.%02d.%d р. по %02d.%02d.%d р.\n","",mn,gn,d,mk,gk);

fprintf(fil.ff,"\
---------------------------------------------------------------------------------------------------------\n\
| N |    Нараховано за видами оплат                 | N |    Утримано                                   |\n\
|п/п|-----------------------------------------------|п/п|-----------------------------------------------|\n\
|   |Код|вид оплати                    | нараховано |   |Код|вид утримання                 |  утримано  |\n\
|   |   |                              |    грн.    |   |   |                              |    грн.    |\n\
---------------------------------------------------------------------------------------------------------\n");
/*
 123 123 123456789012345678901234567890 1234567890 123 123 123456789012345678901234567890 1234567890
*/
class iceb_u_int kod_nah;
class iceb_u_int kod_uder;
class iceb_u_double suma_nah;
class iceb_u_double suma_uder;

int kolih_nah=kodn->kolih();
int kolih_uder=kodu->kolih();


for(int nom=0; nom < kolih_nah; nom++)
 {
  if(suma_n[nom] == 0.)
   continue;
  kod_nah.plus(kodn->ravno(nom));     
  suma_nah.plus(suma_n[nom]);
 }
for(int nom=0; nom < kolih_uder; nom++)
 {
  if(suma_u[nom] == 0.)
   continue;
  kod_uder.plus(kodu->ravno(nom));     
  suma_uder.plus(suma_u[nom]*-1);
 }

kolih_nah=kod_nah.kolih();
kolih_uder=kod_uder.kolih();

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

//  fprintf(fil.ff,"\n");
  int kolstrn=sn.kolih();
  int kolstru=su.kolih();
  int kolstr=kolstrn;
  if(kolstrn < kolstru)
   kolstr=kolstru;
  for(int nom1=0; nom1 < kolstr; nom1++)
   {
    if(nom1 < kolstrn)
     fprintf(fil.ff,"%s",sn.ravno(nom1));
    else
     fprintf(fil.ff,"|%3s|%3s|%30s|%12s|","","","","");
    if(nom1 < kolstru)
     fprintf(fil.ff,"%s\n",su.ravno(nom1));
    else
     fprintf(fil.ff,"%3s|%3s|%30s|%12s|\n","","","","");
   }   

  fprintf(fil.ff,"\
---------------------------------------------------------------------------------------------------------\n");

 }
fprintf(fil.ff,"|%*s|%12.2f|%*s|%12.2f|\n",
iceb_u_kolbait(38,"Разом"),
"Разом",
itogo_n,
iceb_u_kolbait(38,"Разом"),
"Разом",
itogo_u);

class iceb_fioruk_rk gb;

iceb_fioruk(2,&gb,wpredok);


fprintf(fil.ff,"\
---------------------------------------------------------------------------------------------------------\n");
fprintf(fil.ff,"\n%20sГоловний бухгалтер __________________________%s\n","",gb.fio.ravno());
fprintf(fil.ff,"\n%20s         Бухгалтер __________________________%s\n","",iceb_getfioop(wpredok));

iceb_podpis(fil.ff,wpredok);

return(0);

}

/******************************************/
/******************************************/

gint zar_snahud1_r1(class zar_snahud1_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short mn,gn;
short mk,gk;

iceb_u_rsdat1(&mn,&gn,data->rk->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->rk->datan.ravno());
if(mk == 0)
 {
  mk=mn;
  gk=gn;
 }
sprintf(strsql,"%s %d.%d %s %d.%d\n",gettext("Период с"),mn,gn,
gettext("по"),mk,gk);
iceb_printw(strsql,data->buffer,data->view);









int kolstr;
SQL_str row;
SQLCURSOR cur;
class iceb_u_int nahis;
class iceb_u_int uder;

//формируем масив начислений
sprintf(strsql,"select kod from Nash");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не ввели начисления !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
 
while(cur.read_cursor(&row) != 0)
  nahis.plus(atoi(row[0]),-1);

double suma_nahis[nahis.kolih()];
memset(&suma_nahis,'\0',sizeof(suma_nahis));
//формируем масив удержаний
sprintf(strsql,"select kod from Uder");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не введены удержания !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
  uder.plus(atoi(row[0]),-1);

double suma_uder[uder.kolih()];
memset(&suma_uder,'\0',sizeof(suma_uder));


sprintf(strsql,"select tabn,prn,knah,suma,shet,podr from Zarp where \
datz >= '%04d-%02d-%02d' and datz <= '%04d-%02d-%02d' and suma <> 0. \
order by tabn asc",gn,mn,1,gk,mk,31);

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

float kolstr1=0;
int prn=0;
double suma=0.;
int knah=0;
int nomer=0;
short dz,mz,gz;
SQL_str row1;
class SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {

//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->podr.ravno(),row[5],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->shet.ravno(),row[4],0,0) != 0)
    continue;

  if(data->rk->kateg.getdlinna() > 1)
   {
    iceb_u_rsdat(&dz,&mz,&gz,row[6],2);    
    sprintf(strsql,"select kateg from Zarn where tabn=%s and god=%d and mes=%d",row[0],gz,mz);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      if(iceb_u_proverka(data->rk->kateg.ravno(),row1[0],0,0) != 0)
       continue;
     }
    else
     {
      sprintf(strsql,"Не найдена категория для %s !",row[0]);
      iceb_menu_soob(strsql,data->window);
     }
   }

  prn=atoi(row[1]);
  knah=atoi(row[2]);
  suma=atof(row[3]);  
  if(prn == 1)//начисления
   {
    if((nomer=nahis.find(knah)) < 0)
     continue;
    suma_nahis[nomer]+=suma;
   }

  if(prn == 2) //удержания
   {
    if((nomer=uder.find(knah)) < 0)
     continue;
    suma_uder[nomer]+=suma;
   }
  
 }

char imaf[64];
FILE *ff;
sprintf(imaf,"snu%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_zagolov(gettext("Свод начислений и удержаний"),1,mn,gn,31,mk,gk,ff,data->window);

if(data->rk->shet.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());

if(data->rk->podr.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Подразделение"),data->rk->podr.ravno());

if(data->rk->tabnom.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),data->rk->tabnom.ravno());

fprintf(ff,"\n");

snu_ras(1,&nahis,suma_nahis,ff,data->window);

fprintf(ff,"\n");

snu_ras(2,&uder,suma_uder,ff,data->window);

iceb_podpis(ff,data->window);

fclose(ff);

char imaf_tf[64];
sprintf(imaf_tf,"snutf%d.lst",getpid());

snu_tf(mn,gn,mk,gk,data->rk->tabnom.ravno(),&nahis,suma_nahis,&uder,suma_uder,imaf_tf,data->window);

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Свод начислений и удержаний"));

data->rk->imaf.plus(imaf_tf);
if(data->rk->tabnom.getdlinna() > 1)
 data->rk->naimf.plus(gettext("Типовая форма N6"));
else
 data->rk->naimf.plus(gettext("Типовая форма N7"));


for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);




data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}


/*******************************/
/*Распечатка*/
/*******************************/

void snu_ras(int metka,//1-Начисления 2-Удержания
class iceb_u_int *kod,
double *suma,
FILE *ff,
GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str naim("");
SQL_str row;
SQLCURSOR cur;

if(metka == 1)
 fprintf(ff,"%s\n",gettext("Начисления")); 
if(metka == 2)
 fprintf(ff,"%s\n",gettext("Удержания")); 

fprintf(ff,"\
----------------------------------------------\n");

fprintf(ff,gettext("\
Код|     Наименование             |  Сумма   |\n"));

fprintf(ff,"\
----------------------------------------------\n");
double itogo=0.;
for(int i=0; i < kod->kolih() ; i++)
 {
  if(suma[i] == 0.)
   continue;
  if(metka == 1)
   sprintf(strsql,"select naik from Nash where kod=%d",kod->ravno(i));
  if(metka == 2)
   sprintf(strsql,"select naik from Uder where kod=%d",kod->ravno(i));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
    naim.new_plus(row[0]);
  else
    naim.new_plus("");    
  fprintf(ff,"%3d %-*.*s %10.2f\n",kod->ravno(i),iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),suma[i]);
  itogo+=suma[i];
 }
fprintf(ff,"\
----------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(34,gettext("Итого")),gettext("Итого"),itogo);

}
