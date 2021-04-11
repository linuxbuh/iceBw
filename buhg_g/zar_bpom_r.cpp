/*$Id: zar_bpom_r.c,v 1.22 2013/09/26 09:46:59 sasa Exp $*/
/*23.05.2016	08.12.2006	Белых А.И.	zar_bpom_r.c
Распечатка списка работников имеющих благотворительную помощь
*/
#include <errno.h>
#include "buhg_g.h"
#include "zar_bpom.h"

class zar_bpom_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class zar_bpom_rek *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_bpom_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_bpom_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_bpom_r_data *data);
gint zar_bpom_r1(class zar_bpom_r_data *data);
void  zar_bpom_r_v_knopka(GtkWidget *widget,class zar_bpom_r_data *data);


extern SQL_baza bd;
extern short    *kodmp;   /*Коды материальной помощи*/


int zar_bpom_r(class zar_bpom_rek *datark,GtkWidget *wpredok)
{
char strsql[524];
class iceb_u_str repl;
class zar_bpom_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать список работников, которым начислялась благотворительная помощь"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_bpom_r_key_press),&data);

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

repl.plus(gettext("Распечатать список работников, которым начислялась благотворительная помощь"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_bpom_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zar_bpom_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_bpom_r_v_knopka(GtkWidget *widget,class zar_bpom_r_data *data)
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

gboolean   zar_bpom_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_bpom_r_data *data)
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

/********/
/*Шапка*/
/********/
void	sprmatpoms(FILE *ff)
{

fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код|        Наименование          |  Дата    | Счёт|  Сумма   |Не обл.сум| Коментарий\n"));
fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");

}

/******************************************/
/******************************************/

gint zar_bpom_r1(class zar_bpom_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);





char		bros[512];
int		kolstr=0,kolstr1=0;
SQL_str		row,row1;
char		imaf[64];
FILE		*ff=NULL;
short		d,m,g;
class iceb_u_str tabnomz("");
class iceb_u_str fio("");
double		suma=0.;
double		sumatab=0.;
double		itogo=0.;
class iceb_u_str naimn("");
double		sumano=0.;
double		sumanoi=0.;
SQLCURSOR curr;

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

if(kodmp == NULL)
 {
  iceb_menu_soob(gettext("Не введены коды благотворительной помощи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kodmp[0] == 0)
 {
  iceb_menu_soob(gettext("Не введены коды благотворительной помощи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(strsql,"select datz,tabn,knah,suma,shet,kom,podr from Zarp where \
datz >= '%d-%d-1' and datz <= '%d-%d-31' and prn='1' and suma <> 0. and (",gn,mn,gk,mk);

for(int i1=1; i1 <= kodmp[0] ; i1++)
 {
  if(i1 == 1)
   sprintf(bros,"knah=%d",kodmp[i1]);
  else
   sprintf(bros," or  knah=%d",kodmp[i1]);
  strcat(strsql,bros);
 }

strcat(strsql,") order by tabn,datz asc");
//printw("%s\n",strsql);

class SQLCURSOR cur;



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
 

sprintf(imaf,"matp%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

d=1; m=mk; g=gk;
iceb_u_dpm(&d,&m,&g,5);

iceb_u_zagolov(gettext("Список работников, которые имеют благотворительную помощь"),1,mn,gn,d,mk,gk,iceb_get_pnk("00",0,data->window),ff);

short	knah=0;
kolstr1=0;
short mesz=0;
int metkapropuska=0;
class iceb_u_str kateg("");
double  matpom[kodmp[0]]; //Массив сумм мат-помощи по кодам на работника за месяц
double  imatpom[kodmp[0]]; //Массив сумм мат-помощи по кодам на работника
double  inmp[kodmp[0]]; //Массив сумм необлагаемой мат-помощи по кодам всего
memset(&matpom,'\0',sizeof(matpom));
memset(&imatpom,'\0',sizeof(imatpom));
memset(&inmp,'\0',sizeof(inmp));
double bb=0;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[1],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->podr.ravno(),row[6],0,0) != 0)
    continue;

  if(iceb_u_SRAV(tabnomz.ravno(),row[1],0) != 0)
   {

    //Читаем карточку табельного номера
    sprintf(strsql,"select fio,kateg from Kartb where tabn=%s",row[1]);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     {
      fio.new_plus(row1[0]);
      kateg.new_plus(row1[1]);
     }
    else
     {
      fio.new_plus("");
      kateg.new_plus("");
     }
    metkapropuska=0;
    if(iceb_u_proverka(data->rk->kod_kat.ravno(),kateg.ravno(),0,0) != 0)
     {
      metkapropuska=1;
      continue;
     }

    if(tabnomz.ravno()[0] != '\0')
     {
      bb=0.;    
      
      fprintf(ff,"%*s:%10.2f %10.2f\n",iceb_u_kolbait(51,gettext("Итого")),gettext("Итого"),sumatab,sumano);
      sumanoi+=sumano;
      if(bb != 0.)
       for(int i1=0; i1 < kodmp[0] ; i1++)
        {
         if(matpom[i1] == 0.)
          continue;
         if(matpom[i1] > bb)
          {
           inmp[i1]+=bb;
           bb=0.;
           break;
          }
         else
          {
           inmp[i1]+=matpom[i1]; 
           bb-=matpom[i1];
          }
        }
     }

    tabnomz.new_plus(row[1]);
    mesz=0;
    sumano=0.;
    sumatab=0.;


    fprintf(ff,"\n%5s %s\n",row[1],fio.ravno());

    sprmatpoms(ff);

    memset(&matpom,'\0',sizeof(matpom));

   }

  if(metkapropuska == 1)
   continue;
   
  //Читаем наименование начисления
  sprintf(strsql,"select naik from Nash where kod=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    naimn.new_plus(row1[0]);
  else
    naimn.new_plus("");    
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(mesz != m)
   {
    if(mesz != 0)
     {
      bb=0.;

      sumanoi+=sumano;
      if(bb != 0.)
       for(int i1=0; i1 < kodmp[0] ; i1++)
        {
         if(matpom[i1] == 0.)
          continue;
         if(matpom[i1] > bb)
          {
           inmp[i1]+=bb;
           bb=0.;
           break;
          }
         else
          {
           inmp[i1]+=matpom[i1]; 
           bb-=matpom[i1];
          }
        }
      memset(&matpom,'\0',sizeof(matpom));
     }
    mesz=m;
   }

  suma=atof(row[3]);
  itogo+=suma;
  sumatab+=suma;
  knah=atoi(row[2]);

  for(int i1=1; i1 <= kodmp[0] ; i1++)
   if(knah == kodmp[i1])
    {
     matpom[i1-1]+=suma;
     imatpom[i1-1]+=suma;
    }                 

  fprintf(ff,"%3s %-*.*s %02d.%02d.%d %-5s %10.2f %10s %s\n",
  row[2],
  iceb_u_kolbait(30,naimn.ravno()),iceb_u_kolbait(30,naimn.ravno()),naimn.ravno(),
  d,m,g,row[4],suma," ",row[5]);  
 }
bb=0.;
if(bb != 0.)
 for(int i1=0; i1 < kodmp[0] ; i1++)
  {
   if(matpom[i1] == 0.)
    continue;
   if(matpom[i1] > bb)
    {
     inmp[i1]+=bb;
     bb=0.;
     break;
    }
   else
    {
     inmp[i1]+=matpom[i1]; 
     bb-=matpom[i1];
    }
  }

fprintf(ff,"%*s:%10.2f %10.2f\n",iceb_u_kolbait(51,gettext("Итого")),gettext("Итого"),sumatab,sumano);
sumanoi+=sumano;

fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %10.2f\n",iceb_u_kolbait(51,gettext("Общий итог")),gettext("Общий итог"),itogo,sumanoi);
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(51,gettext("Разница")),gettext("Разница"),itogo-sumanoi);

if(kodmp[0] > 1)
 {
  fprintf(ff,"\n");
  for(int i1=1; i1 <= kodmp[0] ; i1++)
   {
    sprintf(strsql,"select naik from Nash where kod=%d",kodmp[i1]);
    if(iceb_sql_readkey(strsql,&row,&curr,data->window) == 1)
     naimn.new_plus(row[0]);    
    else
     naimn.new_plus("");
    fprintf(ff,"%d %-*.*s %10.2f %10.2f %10.2f\n",
    kodmp[i1],
    iceb_u_kolbait(30,naimn.ravno()),iceb_u_kolbait(30,naimn.ravno()),naimn.ravno(),
    imatpom[i1-1],inmp[i1-1],imatpom[i1-1]-inmp[i1-1]);
   }                 
 }

iceb_podpis(ff,data->window);
fclose(ff);




data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Список работников, которые имеют благотворительную помощь"));

iceb_ustpeh(imaf,0,data->window);  


data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
