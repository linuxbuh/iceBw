/*$Id: go_ss_nbs.c,v 1.28 2014/07/31 07:08:25 sasa Exp $*/
/*23.05.2016	26.03.2004	Белых А.И.	go_ss_nbs.c
Журнал-ордер для небалансовых счетов со свернутым сальдо
*/
#include <errno.h>
#include "buhg_g.h"
#include "go.h"

class go_ss_nbs_r_data
 {
  public:
  
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  gfloat kolstr1; //Количество выполненных проходов
  int    kolstr;  //Количество проходов при расчёте
  short     prohod;
  iceb_u_spisok imaf;
  iceb_u_spisok naim;

  go_rr *rek_r;
  short dn,mn,gn;
  short dk,mk,gk;
  short  godn;
  time_t vremn;
  
  short d2,m2,g2;
  double sdeb,skre;
  double odeb,okre;
  double sndeb,snkre;
  short metkasaldo;
  FILE  *ff;
  //Конструктор  
  go_ss_nbs_r_data()
   {
    kolstr1=0.;
    kolstr=9;
    prohod=0;
    sdeb=skre=0.;
    odeb=okre=0.;
    sndeb=snkre=0.;
    metkasaldo=0;
   }

 };

gboolean   go_ss_nbs_r_key_press(GtkWidget *widget,GdkEventKey *event,class go_ss_nbs_r_data *data);
gint go_ss_nbs_r1(class go_ss_nbs_r_data *data);
void  go_ss_nbs_r_v_knopka(GtkWidget *widget,class go_ss_nbs_r_data *data);

 
extern SQL_baza bd;
extern short vplsh; /*0-двух порядковый план счетов 1-многпорядковый*/
extern short	startgodb; /*Стартовый год*/
extern short    koolk; /*Корректор отступа от левого края*/

void go_ss_nbs(class go_rr *data_rr)
{
go_ss_nbs_r_data data;

data.rek_r=data_rr;

char strsql[512];
iceb_u_str soob;



if(iceb_rsdatp(&data.dn,&data.mn,&data.gn,data.rek_r->datan.ravno(),
&data.dk,&data.mk,&data.gk,data.rek_r->datak.ravno(),NULL) != 0)
  return;
  
data.godn=startgodb;
if(startgodb == 0 || startgodb > data.gn)
 data.godn=data.gn;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Журнал ордер"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(go_ss_nbs_r_key_press),&data);

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

sprintf(strsql,"%s %s",gettext("Расчёт журнал-ордера по счёту"),data.rek_r->shet.ravno());
soob.new_plus(strsql);

sprintf(strsql,"%s %d.%d.%d => %d.%d.%d",gettext("Расчёт за период"),
data.dn,data.mn,data.gn,
data.dk,data.mk,data.gk);
soob.ps_plus(strsql);

GtkWidget *label=gtk_label_new(soob.ravno());

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
gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(go_ss_nbs_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)go_ss_nbs_r1,&data);

gtk_main();

iceb_rabfil(&data.imaf,&data.naim,NULL);





}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  go_ss_nbs_r_v_knopka(GtkWidget *widget,class go_ss_nbs_r_data *data)
{
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   go_ss_nbs_r_key_press(GtkWidget *widget,GdkEventKey *event,class go_ss_nbs_r_data *data)
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

gint go_ss_nbs_r1(class go_ss_nbs_r_data *data)
{
char strsql[512];
iceb_u_str soob;
class iceb_getkue_dat kue(data->rek_r->kod_val.ravno(),data->window);

//printf("go_ss_nbs_r1 data->prohod=%d\n",data->prohod);
class iceb_u_str data_saldo(data->godn);
data_saldo.plus("-01-01");

if(data->prohod == 0)
 {
  data->prohod++;
  time(&data->vremn);
  
  /*Узнаем начальное сальдо по счёту*/

  /*Отчет по для многопорядкового плана счетов*/
  if(vplsh == 1)
   sprintf(strsql,"select deb,kre from Saldo where kkk='0' and gs=%d and \
  ns like '%s%%'",data->godn,data->rek_r->shet.ravno());

  /*Отчет по субсчету для двух-порядкового плана счетов*/
  if(data->rek_r->vds == 1 && vplsh == 0)
   sprintf(strsql,"select deb,kre from Saldo where kkk='0' and gs=%d and \
ns='%s'",data->godn,data->rek_r->shet.ravno());

  /*Отчет по счёту для двух-порядкового плана счетов*/
  if(data->rek_r->vds == 0 && vplsh == 0)
   sprintf(strsql,"select deb,kre from Saldo where kkk='0' and gs=%d and \
ns='%s%%'",data->godn,data->rek_r->shet.ravno());

  //printw("%s\n",strsql);
  //refresh();
  SQLCURSOR cur;
  SQL_str   row;
  int       kolstr;  
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
    data->sdeb+=kue.toue(row[0],data_saldo.ravno());
    data->skre+=kue.toue(row[1],data_saldo.ravno());

  //  data->sdeb+=atof(row[0]);
//    data->skre+=atof(row[1]);
   }

  sprintf(strsql,"%s %.2f %.2f %d\n",
  gettext("Стартовое сальдо"),data->sdeb,data->skre,data->godn);

  soob.new_plus(strsql);
  iceb_printw(soob.ravno(),data->buffer,data->view);

  char imaf[64];  
  sprintf(imaf,"gnbs%d.lst",getpid());

  sprintf(strsql,"%s %s",gettext("Журнал ордер"),data->rek_r->shet.ravno());
  data->naim.new_plus(strsql);


  data->imaf.new_plus(imaf);

  if((data->ff = fopen(imaf,"w")) == NULL)
   {
    iceb_er_op_fil(imaf,"",errno,data->window);
    return(FALSE);
   }
//  iceb_u_startfil(data->ff);

//  fprintf(data->ff,"\x1b\x6C%c",10); /*Установка левого поля*/


  sprintf(strsql,"%s %s %s (%s)",gettext("Журнала ордер"),
  data->rek_r->shet.ravno(),data->rek_r->naimshet.ravno(),gettext("Внебалансовый"));

  if(data->rek_r->kod_val.getdlinna() > 1)
   fprintf(data->ff,"%s:%s\n",gettext("Валюта"),data->rek_r->kod_val.ravno());

  iceb_zagolov(strsql,data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,data->ff,data->window);

  fprintf(data->ff,"\
-------------------------------------------------------------\n");
  fprintf(data->ff,gettext("  Дата    |  Кто   |  Дебет   |  Кредит  | Коментарий\n"));

  fprintf(data->ff,"\
-------------------------------------------------------------\n");
  data->d2=1; data->m2=1; data->g2=data->godn;
  data->kolstr=iceb_u_period(1,1,data->godn,data->dk,data->mk,data->gk,0);
  
  return(TRUE);
 }

if(data->prohod == 1)
 {
/**************
  sprintf(strsql,"%d.%d.%d %d.%d.%d %d %f\n",
  data->d2,data->m2,data->g2,data->dk,data->mk,data->gk,data->kolstr,data->kolstr1);
  soob.new_plus(strsql);
  iceb_printw(soob.ravno(),data->buffer,data->view);
**************/


  while(iceb_u_sravmydat(data->d2,data->m2,data->g2,data->dk,data->mk,data->gk) <= 0)
   {
    iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
//    printf("%d.%d.%d\n",data->d2,data->m2,data->g2);
    if(data->metkasaldo == 0 && iceb_u_sravmydat(data->d2,data->m2,data->g2,data->dn,data->mn,data->gn) == 0)
     {
      
      data->metkasaldo=1;
  //    printw("%f %f %f %f\n",data->sdeb,data->sndeb,data->skre,data->snkre);
      
      
      if(data->sdeb+data->sndeb > data->skre+data->snkre)
       {
        sprintf(strsql,"%*s %10.2f\n",
        iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
        (data->sdeb+data->sndeb)-(data->skre+data->snkre));
       
        fprintf(data->ff,"%*.*s %10.2f\n",
        iceb_u_kolbait(19,gettext("Сальдо начальное")),iceb_u_kolbait(19,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
        (data->sdeb+data->sndeb)-(data->skre+data->snkre));
       }
      else
       {
        sprintf(strsql,"%*s %10s %10.2f\n",
        iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
        " ",(data->skre+data->snkre)-(data->sdeb+data->sndeb));

        fprintf(data->ff,"%*.*s %10s %10.2f\n",
        iceb_u_kolbait(19,gettext("Сальдо начальное")),iceb_u_kolbait(19,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
        " ",(data->skre+data->snkre)-(data->sdeb+data->sndeb));
       }

      soob.new_plus(strsql);
      iceb_printw(soob.ravno(),data->buffer,data->view);

     }

    //Для многопорядкового плана счетов
    if(vplsh == 1)
      sprintf(strsql,"select deb,kre,kto,ktoi,komen,datp from Prov \
  where val=-1 and datp = '%d-%02d-%02d' and sh like '%s%%'",
      data->g2,data->m2,data->d2,data->rek_r->shet.ravno());

    /*Отчет по субсчету для двух-порядкового плана счетов*/
    if(data->rek_r->vds == 1 && vplsh == 0)
      sprintf(strsql,"select deb,kre,kto,ktoi,komen,datp from Prov \
  where val=-1 and datp = '%d-%02d-%02d' and sh='%s'",
      data->g2,data->m2,data->d2,data->rek_r->shet.ravno());

    /*Отчет по счёту для двух-порядкового плана счетов*/
    if(data->rek_r->vds == 1 && vplsh == 0)
      sprintf(strsql,"select deb,kre,kto,ktoi,komen,datp from Prov \
  where val=-1 and datp = '%d-%02d-%02d' and sh like '%s%%'",
      data->g2,data->m2,data->d2,data->rek_r->shet.ravno());

    SQLCURSOR cur;
    SQL_str   row;
    int kolstr;
    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

    double deb,kre;
    while(cur.read_cursor(&row) != 0)
     {
      deb=kue.toue(row[0],row[5]);
      kre=kue.toue(row[1],row[5]);
//      deb=atof(row[0]);
//      kre=atof(row[1]);
      if(iceb_u_sravmydat(data->d2,data->m2,data->g2,data->dn,data->mn,data->gn) < 0)
       {
        data->sndeb+=deb;
        data->snkre+=kre;
       }
      if(iceb_u_sravmydat(data->d2,data->m2,data->g2,data->dn,data->mn,data->gn) >= 0)
       {
        data->odeb+=deb;
        data->okre+=kre;
        fprintf(data->ff,"%02d.%02d.%d %-4s%4s %10.2f %10.2f %s\n",
        data->d2,data->m2,data->g2,row[2],row[3],deb,kre,row[4]);
       }


     }

    iceb_u_dpm(&data->d2,&data->m2,&data->g2,1);
    return(TRUE);
   }
  data->prohod++;

  fprintf(data->ff,"\
-------------------------------------------------------------\n");

  sprintf(strsql,"%*s %10.2f %10.2f\n",
  iceb_u_kolbait(20,gettext("Оборот за период")),gettext("Оборот за период"),
  data->odeb,data->okre);

  soob.new_plus(strsql);

  fprintf(data->ff,"%*.*s %10.2f %10.2f\n",
  iceb_u_kolbait(19,gettext("Оборот за период")),iceb_u_kolbait(19,gettext("Оборот за период")),gettext("Оборот за период"),
  data->odeb,data->okre);

  if(data->sdeb+data->sndeb+data->odeb > data->skre+data->snkre+data->okre)
   {
    sprintf(strsql,"%*s %10.2f\n",
    iceb_u_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
    (data->sdeb+data->sndeb+data->odeb)-(data->skre+data->snkre+data->okre));

    fprintf(data->ff,"%*.*s %10.2f\n",
    iceb_u_kolbait(19,gettext("Сальдо конечное")),iceb_u_kolbait(19,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
    (data->sdeb+data->sndeb+data->odeb)-(data->skre+data->snkre+data->okre));
   }
  else
   {
    sprintf(strsql,"%*s %10s %10.2f\n",
    iceb_u_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
    " ",(data->skre+data->snkre+data->okre)-(data->sdeb+data->sndeb+data->odeb));

    fprintf(data->ff,"%*.*s %10s %10.2f\n",
    iceb_u_kolbait(19,gettext("Сальдо конечное")),iceb_u_kolbait(19,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
    " ",(data->skre+data->snkre+data->okre)-(data->sdeb+data->sndeb+data->odeb));
   }

  soob.plus(strsql);
  iceb_printw(soob.ravno(),data->buffer,data->view);

  iceb_podpis(data->ff,data->window);


  fclose(data->ff);

  return(TRUE);
 }

if(data->prohod == 2)
 {
  data->prohod++;

  iceb_printw_vr(data->vremn,data->buffer,data->view);
  return(TRUE);
 }

for(int nom=0; nom < data->imaf.kolih(); nom++)
 iceb_ustpeh(data->imaf.ravno(0),3,data->window);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);
return(FALSE);

}
