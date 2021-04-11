/*$Id: zar_krz_r.c,v 1.26 2013/09/26 09:47:00 sasa Exp $*/
/*25.05.2016	23.10.2006	Белых А.И.	zar_krz_r.c
Контроль расчёта зарплаты
*/
#include <math.h>
#include <errno.h>
#include "buhg_g.h"
#include "kasothkcn.h"

class zar_krz_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
  
  short mr,gr;
  class iceb_u_spisok *imafr;
  class iceb_u_spisok *naimfr;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_krz_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_krz_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_krz_r_data *data);
gint zar_krz_r1(class zar_krz_r_data *data);
void  zar_krz_r_v_knopka(GtkWidget *widget,class zar_krz_r_data *data);


extern SQL_baza bd;
extern double   okrg1;
extern double	okrcn;
extern class iceb_u_str	shrpz; /*Счет расчётов по зарплате*/
extern double   okrg; /*Округление*/
extern short kodpn;   /*Код подоходного налога*/
extern short kodpn_sbol;/*Код подоходного налога с больничного*/



int zar_krz_r(short mr,short gr,class iceb_u_spisok *imafr,class iceb_u_spisok *naimfr,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zar_krz_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);
data.mr=mr;
data.gr=gr;
data.imafr=imafr;
data.naimfr=naimfr;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Контроль расчёта зарплаты"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_krz_r_key_press),&data);

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

repl.plus(gettext("Контроль расчёта зарплаты"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_krz_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zar_krz_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_krz_r_v_knopka(GtkWidget *widget,class zar_krz_r_data *data)
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

gboolean   zar_krz_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_krz_r_data *data)
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

/******************************************/
/******************************************/

gint zar_krz_r1(class zar_krz_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[2048];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,row1;
SQLCURSOR cur,cur1;



int kolstr=0;

sprintf(strsql,"select tabn,fio,datk,sovm,datn,podr,kateg,shet,zvan,\
lgoti,datn,dolg from Kartb");
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

char imaf[64];
char imaf1[64];

sprintf(imaf,"nnz%d.lst",getpid());
sprintf(imaf1,"szr%d.lst",getpid());
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,NULL);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
FILE *f;

if((f = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,NULL);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_p[64];
sprintf(imaf_p,"konrz%d.lst",getpid());

if(zar_pr_startw(imaf_p,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
/*Чтение настроек для расчёта единого социального взноса*/
zarrnesvw(data->window);

short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;
iceb_u_dpm(&dk,&data->mr,&data->gr,5);
iceb_u_zagolov(gettext("Зароботная плата"),1,data->mr,data->gr,dk,data->mr,data->gr,iceb_get_pnk("00",0,data->window),f);
                                                                                    

fprintf(f,"\
------------------------------------------------------------------------------------------\n");
fprintf(f,gettext("Т.Н.|    Фамилия  Имя Отчество               | Сальдо   |Начислено |  Удержано |К выдаче\n"));

fprintf(f,"\
------------------------------------------------------------------------------------------\n");

iceb_u_zagolov(gettext("Контроль расчёта зарплаты"),1,data->mr,data->gr,dk,data->mr,data->gr,iceb_get_pnk("00",0,data->window),ff);


SQLCURSOR curr;
int kol=0;
float kolstr1=0.;
double itdl=0.,itsl=0.,itsn=0.,itsu=0.;
int sovm=0;
int tabb=0;
class iceb_u_str fio("");
double sal=0.;
double sum=0.;
double sumn=0.;
double sumu=0.;
double saldb=0.;
class iceb_u_str pr(""); /*Дата приема и увольнения с работы*/
int podr=0;
int kateg=0;    
short		zvan;
class iceb_u_str lgoti("");
class iceb_u_str shet("");
short d,m,g;
int kolkd1=0;
int kolstr2=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
  tabb=atoi(row[0]);
  fio.new_plus(row[1]);


//  sprintf(pr,"%s/%s",row[4],row[2]);
  pr.new_plus(row[4]);
  pr.plus("/",row[2]);
  
  sovm=atoi(row[3]);
  podr=atoi(row[5]);
  kateg=atoi(row[6]);
  shet.new_plus(row[7]);
  zvan=atoi(row[8]);
  lgoti.new_plus(row[9]);

  sum=sumn=sumu=0.;

  /*Читаем сальдо*/
  sal=0.;
  sum=sal=zarsaldw(1,data->mr,data->gr,tabb,&saldb,data->window);
  
  if(fabs(sal) > 0.009)
   {
    /*Проверяем был ли вход в карточку*/
    sprintf(strsql,"select tabn from Zarn where tabn=%d and god=%d and \
mes=%d",tabb,data->gr,data->mr);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
     {
      iceb_u_rsdat(&dn,&mn,&gn,row[10],2);
      iceb_u_rsdat(&dk,&mk,&gk,row[2],2);
      zapzarnw(data->mr,data->gr,podr,tabb,kateg,sovm,zvan,shet.ravno(),lgoti.ravno(),dn,mn,gn,dk,mk,gk,0,row[11],data->window);
     }

    if(iceb_u_rsdat(&d,&m,&g,row[2],2) == 0)
     {
      sprintf(strsql,"%d %*.*s %s %d.%d.%d - %s %.2f\n",
      tabb,iceb_u_kolbait(20,fio.ravno()),iceb_u_kolbait(20,fio.ravno()),fio.ravno(),gettext("Уволен"),d,m,g,
      gettext("не нулевое сальдо"),sal);
      
      iceb_printw(strsql,data->buffer,data->view);

      fprintf(ff,"%d %s %s %d.%d.%d - %s %.2f\n",
      tabb,fio.ravno(),gettext("Уволен"),d,m,g,
      gettext("не нулевое сальдо"),sal);
      continue;
     }
   }
  else
   {
    //Проверяем нет ли начислений или удержаний 
    sprintf(strsql,"select prn from Zarp where tabn=%d and datz >= '%04d-%2d-%2d' \
and datz <= '%04d-%2d-%2d' and suma <> 0.",tabb,data->gr,data->mr,1,data->gr,data->mr,31);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 0)
     {
      /*Проверяем был ли вход в карточку*/
      sprintf(strsql,"select tabn from Zarn where tabn=%d and god=%d and \
mes=%d",tabb,data->gr,data->mr);
      if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
       {
        iceb_u_rsdat(&dn,&mn,&gn,row[10],2);
        iceb_u_rsdat(&dk,&mk,&gk,row[2],2);
        zapzarnw(data->mr,data->gr,podr,tabb,kateg,sovm,zvan,shet.ravno(),lgoti.ravno(),dn,mn,gn,dk,mk,gk,0,row[11],data->window);
       }
     }    
   }
   
  itsl+=sal;

  if(iceb_u_rsdat(&d,&m,&g,row[2],2) == 0)
    continue;

  double sumnprov=0.,sumuprov=0.;
  short metk=0;

  sprintf(strsql,"select prn,knah,suma from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and \
  suma <> 0. order by prn,knah asc",data->gr,data->mr,data->gr,data->mr,tabb);

  class SQLCURSOR cur;
  if((kolstr2=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  metk=0;
  if(kolstr2 == 0)
    metk=1;

  double sumanah=0;
  while(cur.read_cursor(&row) != 0)
   {
    sumanah=atof(row[2]);
    sum+=sumanah;
    if(atoi(row[0]) == 1)
     {
      sumn+=sumanah;
      //Определяем нужно ли делать проводки
      sprintf(strsql,"select prov from Nash where kod=%d",atoi(row[1]));
      if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
       if(atoi(row1[0]) == 0)
         sumnprov+=sumanah;
     }
    else
     {
      sumu+=sumanah;
       //Определяем нужно ли делать проводки
       
       sprintf(strsql,"select prov from Uder where kod=%d",atoi(row[1]));
//       printw("%s\n",strsql);
       if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
        if(atoi(row1[0]) == 0)
          sumuprov+=sumanah;
     }
   }

    itsn+=sumn;
    itsu+=sumu;
    if(sal+sumn+sumu < 0.)
      itdl+=sal+sumn+sumu;


    /*расчитываем подоходный налог*/
    double suma_podoh_ras=ras_podoh1w(tabb,data->mr,data->gr,podr,0.,0.,1,0.,"",data->window);
    double suma_podoh_nah=0.;
    /*смотрим сколько его посчитано*/

    sprintf(strsql,"select suma from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' \
 and tabn=%d and prn='2' and (knah=%d or knah=%d)",data->gr,data->mr,data->gr,data->mr,tabb,
    kodpn,
    kodpn_sbol);

    if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
      continue;
     }
    while(cur1.read_cursor(&row1) != 0)
     {
      suma_podoh_nah+=atof(row1[0]);
     }     

    if(fabs(suma_podoh_nah - suma_podoh_ras) > 0.009)
     {
      sprintf(strsql,"%4d %-*s %s\n",tabb,iceb_u_kolbait(30,fio.ravno()),fio.ravno(),pr.ravno());
      iceb_printw(strsql,data->buffer,data->view);

      sprintf(strsql,"%s %.2f != %.2f\n",gettext("Не правильно посчитан единый социальный взнос"),suma_podoh_nah,suma_podoh_ras);
      iceb_printw(strsql,data->buffer,data->view);

      fprintf(ff,"%4d %-*s %s\n",tabb,iceb_u_kolbait(30,fio.ravno()),fio.ravno(),pr.ravno());
      fprintf(ff,"%s %.2f != %.2f\n",gettext("Не правильно посчитан единый социальный взнос"),suma_podoh_nah,suma_podoh_ras);


      kol++;
     }




  if(metk == 1 || sum <=0. || sumn == 0. || sumu == 0.)
   {

    if(metk == 1)
     {
      sprintf(strsql,"%s ",gettext("Нет записей"));
      iceb_printw(strsql,data->buffer,data->view);
      fprintf(ff,"%s ",gettext("Нет записей"));
     }
    else
     {
      if(sumn == 0.)
       {
        sprintf(strsql,"%s ",gettext("Нет начислений"));
        iceb_printw(strsql,data->buffer,data->view);
        fprintf(ff,"%s ",gettext("Нет начислений"));
       }
      if(sumu == 0.)
       {
        sprintf(strsql,"%s ",gettext("Нет удержаний"));
        fprintf(ff,"%s ",gettext("Нет удержаний"));
       }

      if(sumn != 0. && sumu != 0.)
       {
        if(fabs(sum) <= 0.0001 )
         {
          sprintf(strsql,"%s ",gettext("Ноль"));
          iceb_printw(strsql,data->buffer,data->view);
          fprintf(ff,"%s ",gettext("Ноль"));
         }
        else
         {
          sprintf(strsql,"%s ",gettext("Долг"));
          iceb_printw(strsql,data->buffer,data->view);
          fprintf(ff,"%s ",gettext("Долг"));
         }
       }
     }
    sprintf(strsql,"%4d %-*s %s\n",tabb,iceb_u_kolbait(30,fio.ravno()),fio.ravno(),pr.ravno());
    iceb_printw(strsql,data->buffer,data->view);

    fprintf(ff,"%4d %-*s %s\n",tabb,iceb_u_kolbait(30,fio.ravno()),fio.ravno(),pr.ravno());
    kol++;
   }

  /*Смотрим сумму выполненых проводок*/ 
  if(shrpz.ravno()[0] != '\0')
   {
    double sumd=0.,sumk=0.;
    class iceb_u_str nomdok;
    nomdok.plus(data->mr);
    nomdok.plus("-",tabb);
  //  sprintf(sss1,"%d-%d",data->mr,tabb);
    
    sumprzw(data->mr,data->gr,nomdok.ravno(),0,&sumd,&sumk,data->window);
    
    if(fabs(sumd-sumnprov) > 0.009 || fabs(sumuprov*(-1)-sumk) > 0.009)
     {
      sprintf(strsql,"%4d %-*s  %s (%f %f/%f %f)\n",
      tabb,iceb_u_kolbait(30,fio.ravno()),fio.ravno(),gettext("Не выполнены все проводки !"),
      sumd,sumnprov,sumuprov*-1,sumk);

      iceb_printw(strsql,data->buffer,data->view);

      fprintf(ff,"%4d %-*s  %s (%f %f/%f %f)\n",
      tabb,iceb_u_kolbait(30,fio.ravno()),fio.ravno(),gettext("Не выполнены все проводки !"),
      sumd,sumnprov,sumuprov*-1,sumk);
     }      
   }

  /*Смотрим введён ли табель и проверяем его*/
  int kolrd=0,kolkd=0;

  sprintf(strsql,"select dnei,kdnei from Ztab where god=%d and \
mes=%d and tabn=%d",data->gr,data->mr,tabb);
  SQLCURSOR cur2;
  int kolstr2=0;
  if((kolstr2=cur2.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr2 == 0)
   {        
    sprintf(strsql,"%s - %d %s %s %d.%d%s\n",
    gettext("Табель не введен"),tabb,fio.ravno(),
    gettext("за"),data->mr,data->gr,
    gettext("г."));
    
    iceb_printw(strsql,data->buffer,data->view);

    fprintf(ff,"%s - %d %s %s %d.%d%s\n",
    gettext("Табель не введен"),tabb,fio.ravno(),
    gettext("за"),data->mr,data->gr,
    gettext("г."));
    goto vp;
   }

  while(cur2.read_cursor(&row1) != 0)
   {
    kolrd+=atoi(row1[0]);
    kolkd+=atoi(row1[1]);
   }  

  if(data->mr == 1 || data->mr == 3 || data->mr == 5 || data->mr == 7 || data->mr == 8 || data->mr == 10 || data->mr == 12)
    kolkd1=31;
  else
    kolkd1=30;

  if(data->mr == 2 )
   {
    if(kolkd > 29 || kolkd < 28)
     {
      sprintf(strsql,"%s %d %s\n",
      gettext("Не верно введено количество календарных дней"),
      tabb,fio.ravno());

      iceb_printw(strsql,data->buffer,data->view);

      fprintf(ff,"%s %d %s\n",
      gettext("Не верно введено количество календарных дней"),
      tabb,fio.ravno());
     }
   }
  else
   if(kolkd != kolkd1)
    {
     sprintf(strsql,"%s %d %s\n",
     gettext("Не верно введено количество календарных дней"),
     tabb,fio.ravno());


     iceb_printw(strsql,data->buffer,data->view);

     fprintf(ff,"%s %d %s\n",
     gettext("Не верно введено количество календарных дней"),
     tabb,fio.ravno());
    }

  if(kolkd != 0 && kolrd > kolkd)
   {
     sprintf(strsql,"%s %d %s\n",
     gettext("Количество рабочих дней больше количества календарных дней"),
     tabb,fio.ravno());

     iceb_printw(strsql,data->buffer,data->view);
     
     fprintf(ff,"%s %d %s\n",
     gettext("Количество рабочих дней больше количества календарных дней"),
     tabb,fio.ravno());
   }

 vp:;

   /*Проверяем был ли вход в карточку*/
   sprintf(strsql,"select tabn from Zarn where tabn=%d and god=%d and \
mes=%d",tabb,data->gr,data->mr);
   if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
    {
     sprintf(strsql,"%s %d %s\n",
     gettext("Не было входа в карточку для"),tabb,fio.ravno());

     iceb_printw(strsql,data->buffer,data->view);

     fprintf(ff,"%s %d %s\n",
     gettext("Не было входа в карточку для"),tabb,fio.ravno());
    }
  fprintf(f,"%-4d %-*.*s %10.2f %10.2f %10.2f %10.2f\n",
  tabb,
  iceb_u_kolbait(40,fio.ravno()),
  iceb_u_kolbait(40,fio.ravno()),
  fio.ravno(),
  sal,sumn,
  sumu,sal+sumn+sumu);

 }

fprintf(f,"\
------------------------------------------------------------------------------------------\n");
fprintf(f,"%-*s %10.2f %10.2f %10.2f %10.2f\n",
iceb_u_kolbait(45,gettext("Итого")),gettext("Итого"),itsl,itsn,itsu,itsl+itsn+itsu-itdl);

fprintf(ff,"\
---------------------------------------------------------------\n\
%s %d\n",gettext("Количество работников"),kol);

sprintf(strsql,"%s %d\n",gettext("Количество работников"),kol);

iceb_printw(strsql,data->buffer,data->view);

fclose(f);
fclose(ff);
zar_pr_endw(data->window);

gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace",NULL);

sprintf(strsql,"\n\
%-*s:%15.2f\n\
%-*s:%15.2f\n\
%-*s:%15.2f\n\
%-*s:%15.2f\n\
%-*s:%15.2f\n",
iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsl,
iceb_u_kolbait(20,gettext("Итого начислено")),gettext("Итого начислено"),itsn,
iceb_u_kolbait(20,gettext("Итого удержано")),gettext("Итого удержано"),itsu,
iceb_u_kolbait(20,gettext("Итого долгов")),gettext("Итого долгов"),itdl,
iceb_u_kolbait(20,gettext("Сальдо")),gettext("Сальдо"),itsl+itsn+itsu-itdl);

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");


data->imafr->plus(imaf);
data->imafr->plus(imaf1);
data->imafr->plus(imaf_p);
data->naimfr->plus(gettext("Контроль расчёта зарплаты"));
data->naimfr->plus(gettext("Зароботная плата"));
data->naimfr->plus(gettext("Протокол хода расчёта"));


for(int nom=0; nom < data->imafr->kolih(); nom++)
 iceb_ustpeh(data->imafr->ravno(nom),3,data->window);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
