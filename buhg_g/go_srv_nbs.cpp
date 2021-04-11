/*$Id: go_srv_nbs.c,v 1.30 2014/07/31 07:08:25 sasa Exp $*/
/*23.05.2016	26.03.2004	Белых А.И.	go_srv_nbs.c
Журнал ордер для неболансовых счетов с развернутым сальдо
*/
#include <errno.h>
#include "buhg_g.h"
#include "go.h"

class go_srv_nbs_r_data
 {
  public:
  
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  iceb_u_spisok imaf;
  iceb_u_spisok naim;

  go_rr *rek_r;
  short dn,mn,gn;
  short dk,mk,gk;
  short  godn;
  time_t vremn;
  

  iceb_u_spisok KONTR;

  iceb_u_double MDO;
  iceb_u_double MKO;
  iceb_u_double MDO1;
  iceb_u_double MKO1;
  iceb_u_double MDO2;
  iceb_u_double MKO2;
  iceb_u_int PROV;

  double deb,kre;
  double deb1,kre1;
  double deb2,kre2;
  double ideb,ikre;
  double ideb1,ikre1;
  double ideb2,ikre2;
  short  d,m,g;
  FILE   *ff;
    
  //Конструктор  
  go_srv_nbs_r_data()
   {

   deb=kre=0.;
   deb1=kre1=0.;
   deb2=kre2=0.;
   ideb=kre=0.;
   ideb1=ikre1=0.;
   ideb2=ikre2=0.;
   }

 };

gboolean   go_srv_nbs_r_key_press(GtkWidget *widget,GdkEventKey *event,class go_srv_nbs_r_data *data);
gint go_srv_nbs_r1(class go_srv_nbs_r_data *data);
void  go_srv_nbs_r_v_knopka(GtkWidget *widget,class go_srv_nbs_r_data *data);

void	sapnbs(short,short,short,short,short,short,int,FILE*);
void	shetnbs(short,short,short,short,short,short,int*,int*,FILE*);

extern SQL_baza bd;
extern short vplsh; /*0-двух порядковый план счетов 1-многпорядковый*/
extern short	startgodb; /*Стартовый год*/
extern short    koolk; /*Корректор отступа от левого края*/
extern int kol_strok_na_liste;

void go_srv_nbs(class go_rr *data_rr)
{
go_srv_nbs_r_data data;

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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(go_srv_nbs_r_key_press),&data);

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

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(go_srv_nbs_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна


g_idle_add((GSourceFunc)go_srv_nbs_r1,&data);

gtk_main();

iceb_rabfil(&data.imaf,&data.naim,NULL);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  go_srv_nbs_r_v_knopka(GtkWidget *widget,class go_srv_nbs_r_data *data)
{
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   go_srv_nbs_r_key_press(GtkWidget *widget,GdkEventKey *event,class go_srv_nbs_r_data *data)
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

gint go_srv_nbs_r1(class go_srv_nbs_r_data *data)
{
class iceb_getkue_dat kue(data->rek_r->kod_val.ravno(),data->window);
char strsql[512];
iceb_u_str soob;
class iceb_u_str data_saldo(data->godn);
data_saldo.plus("-01-01");
double deb=0.,kre=0.;
//printf("go_srv_nbs_r1 data->prohod=%d\n",data->prohod);

  time(&data->vremn);

  /*Определяем все организации и открываем массивы дебетов и
  кредитов организаций
  */

  if(data->rek_r->vds == 0 || vplsh == 1)
   sprintf(strsql,"select distinct kodkon from Skontr where ns like '%s%%'",data->rek_r->shet.ravno());

  if(data->rek_r->vds == 1 && vplsh == 0)
   sprintf(strsql,"select kodkon from Skontr where ns='%s'",data->rek_r->shet.ravno());

  SQLCURSOR cur;
  SQLCURSOR cur1;
  int kolstr;
  SQL_str   row;
  SQL_str   row1;
  
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(FALSE);
   }

  if(kolstr == 0)
   {
    iceb_u_str repl;
    sprintf(strsql,"%s %s !",gettext("Не введён список контрагентов для счёта"),
    data->rek_r->shet.ravno());
    repl.plus(strsql);
    iceb_menu_soob(&repl,data->window);
    
    return(FALSE);
    
   }
  float kolstr1=0.;
  while(cur.read_cursor(&row) != 0)
   {
    iceb_pbar(data->bar,kolstr,++kolstr1);    
    if(data->rek_r->kodgr.getdlinna() > 1)
     {
      sprintf(strsql,"select grup from Kontragent where kodkon='%s'",
      row[0]);
      if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
       {
        iceb_u_str repl;
        sprintf(strsql,"%s %s ",gettext("Не найден код контрагента в общем списке !"),
        row[0]);
        repl.plus(strsql);
        iceb_menu_soob(&repl,data->window);
        continue;
       }
      if(iceb_u_proverka(data->rek_r->kodgr.ravno(),row1[0],0,0) != 0)
        continue;  
     }
      
    data->KONTR.plus(row[0]);

   }

  sprintf(strsql,"%s: %d\n",gettext("Количество контрагентов"),data->KONTR.kolih());
  soob.new_plus(strsql);
    
  if(data->KONTR.kolih() == 0)
   {
    iceb_u_str repl;
    sprintf(strsql,"%s %s !",gettext("Не введено список контрагентов для счёта"),
    data->rek_r->shet.ravno());
    repl.plus(strsql);
    iceb_menu_soob(&repl,data->window);
    return(FALSE);
   }


  int koo=data->KONTR.kolih();

  data->MKO.make_class(koo);
  data->MDO.make_class(koo);
  data->MKO1.make_class(koo);
  data->MDO1.make_class(koo);
  data->MKO2.make_class(koo);
  data->MDO2.make_class(koo);

  data->PROV.make_class(koo);

  /*Узнаем начальное сальдо по каждой организации*/
  //printw(gettext("Вычисляем стартовое сальдо по всем контрагентам.\n"));

  if(data->rek_r->vds == 0 || vplsh == 1)
   sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='1' and gs=%d \
  and ns like '%s%%'",data->godn,data->rek_r->shet.ravno());

  if(data->rek_r->vds == 1 && vplsh == 0)
   sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='1' and gs=%d \
  and ns='%s'",data->godn,data->rek_r->shet.ravno());

  //printw("%s\n",strsql);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(FALSE);
   }
  kolstr1=0.;
  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
    iceb_pbar(data->bar,kolstr,++kolstr1);    
    if(data->rek_r->kodgr.getdlinna() > 1)
     {
      sprintf(strsql,"select grup from Kontragent where kodkon='%s'",
      row[0]);
      if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
       {
        iceb_u_str repl;
        repl.plus(gettext("Не найден код контрагента в общем списке !"));
        repl.ps_plus(row[0]);
        iceb_menu_soob(&repl,data->window);
        continue;
       }
      if(iceb_u_proverka(data->rek_r->kodgr.ravno(),row1[0],0,0) != 0)
        continue;  
     }
    int i1;
    if((i1=data->KONTR.find_r(row[0])) >= 0)
     {
      deb=kue.toue(row[1],data_saldo.ravno());
      kre=kue.toue(row[2],data_saldo.ravno());
      data->MDO.plus(deb,i1);
      data->MKO.plus(kre,i1);
     }      
    else
     {
      iceb_u_str repl;
      repl.plus(gettext("Не найден код контрагента в массиве кодов !"));
      repl.ps_plus(row[0]);
      iceb_menu_soob(&repl,data->window);
     }

   }
  char imaf[64];

  sprintf(strsql,"%s %s %s",
  gettext("Журнал ордер"),
  data->rek_r->shet.ravno(),data->rek_r->naimshet.ravno());

  data->naim.new_plus(strsql);
  
  sprintf(imaf,"nbs%d.lst",getpid());
  data->imaf.new_plus(imaf);

  if((data->ff = fopen(imaf,"w")) == NULL)
   {
    iceb_er_op_fil(imaf,"",errno,data->window);
    return(FALSE);
   }
//  iceb_u_startfil(data->ff);
  data->d=1; data->m=1; data->g=data->godn;
  kolstr=iceb_u_period(1,1,data->godn,data->dk,data->mk,data->gk,0);

//  printw("%s\n",gettext("Смотрим проводки, заполняем массивы"));
//  refresh();
  int kolstr2;
  kolstr1=0.;
  //Расчёт
  while(iceb_u_sravmydat(data->d,data->m,data->g,data->dk,data->mk,data->gk) <= 0)
   {
    iceb_pbar(data->bar,kolstr,++kolstr1);    
  //  printw("%d.%d.%d\n",d,m,g);
  //  refresh();

    if(data->rek_r->vds == 0 || vplsh == 1)
     sprintf(strsql,"select shk,kodkon,deb,kre,sh,datp from Prov \
  where val=-1 and datp='%04d-%02d-%02d' and sh like '%s%%'",
    data->g,data->m,data->d,data->rek_r->shet.ravno());

    if(data->rek_r->vds == 1 && vplsh == 0)
     sprintf(strsql,"select shk,kodkon,deb,kre,sh,datp from Prov \
  where val=-1 and datp='%04d-%02d-%02d' and sh='%s'",
     data->g,data->m,data->d,data->rek_r->shet.ravno());
         
    if((kolstr2=cur.make_cursor(&bd,strsql)) < 0)
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

    if(kolstr2 > 0)
    while(cur.read_cursor(&row) != 0)
     {
  //    printw("%s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[6]);
      if(data->rek_r->kodgr.getdlinna() > 1)
       {
        sprintf(strsql,"select grup from Kontragent where kodkon='%s'",
        row[0]);
        if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
         {
          iceb_u_str repl;
          repl.plus(gettext("Не найден код контрагента в общем списке !"));
          repl.ps_plus(row[0]);
          iceb_menu_soob(&repl,data->window);
          continue;
         }

        if(iceb_u_proverka(data->rek_r->kodgr.ravno(),row1[0],0,0) != 0)
          continue;
          
       }

      if(row[1][0] == '\0')
       {
        fprintf(data->ff,"%s %s %s %s %s %s\n",row[5],row[4],row[0],row[1],row[2],row[3]);
        fprintf(data->ff,gettext("В проводке нет кода контрагента !"));
        fprintf(data->ff,"\n");
        iceb_u_str repl;
        
        sprintf(strsql,"%d.%d.%d %s %s %s %s %s",data->d,data->m,data->g,row[4],row[0],row[1],row[2],row[3]);
        repl.plus(strsql);
        
        sprintf(strsql,gettext("В проводке нет кода контрагента !"));
        repl.ps_plus(strsql);
        iceb_menu_soob(&repl,data->window);
        continue;
       }
      int i1;     
      if((i1=data->KONTR.find_r(row[1])) == -1)
       {
        if(data->ff != NULL)
         {
          fprintf(data->ff,"%s %s %s %s %s %s\n",row[5],row[4],row[0],row[1],row[2],row[3]);
          fprintf(data->ff,"%s %s",gettext("Не найден код контрагента в массиве кодов !"),
          row[1]);
          fprintf(data->ff,"\n");
         }
        iceb_u_str repl;
        repl.plus(gettext("Не найден код контрагента в массиве кодов !"));
        repl.ps_plus(row1[1]);
        iceb_menu_soob(&repl,data->window);
        continue;

       }
      else
       {
        data->PROV.plus(1,i1);
        deb=kue.toue(row[2],row[5]);
        kre=kue.toue(row[3],row[5]);
         
        if(iceb_u_sravmydat(data->d,data->m,data->g,data->dn,data->mn,data->gn) < 0)
         {
          data->MDO1.plus(deb,i1);
          data->MKO1.plus(kre,i1);
         }
        else
         {
          data->MDO2.plus(deb,i1);
          data->MKO2.plus(kre,i1);
         }
       }


     }


    iceb_u_dpm(&data->d,&data->m,&data->g,1); //Увеличиваем дату на день
   }


//  printw(gettext("Распечатываем оборот по счёту.\n"));




  sprintf(strsql,"%s %s %s (%s)",gettext("Журнала ордер"),
  data->rek_r->shet.ravno(),data->rek_r->naimshet.ravno(),gettext("Внебалансовый"));
  iceb_u_zagolov(strsql,data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,iceb_get_pnk("00",0,data->window),data->ff);

  int kolstras=11;

  if(data->rek_r->kod_val.getdlinna() > 1)
    {
     fprintf(data->ff,"%s:%s\n",gettext("Валюта"),data->rek_r->kod_val.ravno());
     kolstras++;
    }        
  int kollist=1;
  sapnbs(data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,kollist,data->ff);
  koo=data->KONTR.kolih();
  class iceb_u_str kontr("");
  class iceb_u_str naikontr("");
    
  for(int i=0 ;i<koo;i++)
   {
    /*Проверяем были ли сделаны проводки несмотряна общий ноль*/
    if(data->PROV.ravno(i) == 0) //Не было проводок
    if(data->MDO.ravno(i) == 0. && data->MKO.ravno(i) == 0.\
    && data->MDO1.ravno(i) == 0. && data->MKO1.ravno(i) == 0.\
    && data->MDO2.ravno(i) == 0. && data->MKO2.ravno(i) == 0.)
      continue;

    naikontr.new_plus("");

    kontr.new_plus(data->KONTR.ravno(i));

    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      fprintf(data->ff,"%s %s\n",gettext("Не найден код контрагента в общем списке !"),kontr.ravno());

      iceb_u_str repl;
      sprintf(strsql,"%s %s",gettext("Не найден код контрагента в общем списке !"),kontr.ravno());
      repl.plus(strsql);
      iceb_menu_soob(&repl,data->window);

      continue;
     }
    else
     {  
      naikontr.new_plus(row1[0]);
     }
    data->deb=data->MDO.ravno(i);
    data->kre=data->MKO.ravno(i);
    data->deb+=data->MDO1.ravno(i);
    data->kre+=data->MKO1.ravno(i);
    data->deb1=data->MDO2.ravno(i);
    data->kre1=data->MKO2.ravno(i);
    data->ideb1+=data->MDO2.ravno(i);
    data->ikre1+=data->MKO2.ravno(i);
    data->deb2=data->deb+data->deb1;    
    data->kre2=data->kre+data->kre1;    
    shetnbs(data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,&kolstras,&kollist,data->ff);
    if(data->deb > data->kre)
     {
      data->ideb+=data->deb-data->kre;
      fprintf(data->ff,"%*s %-*.*s %14.2f %14s %14.2f %14.2f ",
      iceb_u_kolbait(10,kontr.ravno()),kontr.ravno(),
      iceb_u_kolbait(20,naikontr.ravno()),iceb_u_kolbait(20,naikontr.ravno()),naikontr.ravno(),
      data->deb-data->kre," ",data->deb1,data->kre1);
     }
    else
     {
      data->ikre+=data->kre-data->deb;
      fprintf(data->ff,"%*s %-*.*s %14s %14.2f %14.2f %14.2f ",
      iceb_u_kolbait(10,kontr.ravno()),kontr.ravno(),
      iceb_u_kolbait(20,naikontr.ravno()),iceb_u_kolbait(20,naikontr.ravno()),naikontr.ravno(),
      " ",data->kre-data->deb,data->deb1,data->kre1);
     }

    shetnbs(data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,&kolstras,&kollist,data->ff);
    if(data->deb2 > data->kre2)
     {
      data->ideb2+=data->deb2-data->kre2;
      fprintf(data->ff,"%14.2f\n",data->deb2-data->kre2);
     }
    else
     {
      data->ikre2+=data->kre2-data->deb2;
      fprintf(data->ff,"%14s %14.2f\n"," ",data->kre2-data->deb2);
     }

    if(iceb_u_strlen(naikontr.ravno()) > 20)
     {
      fprintf(data->ff,"%-10s %s\n"," ",iceb_u_adrsimv(20,naikontr.ravno()));
      shetnbs(data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,&kolstras,&kollist,data->ff);
     }
   }
  fprintf(data->ff,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*.*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
  iceb_u_kolbait(31,gettext("Итого")),iceb_u_kolbait(31,gettext("Итого")),gettext("Итого"),
  data->ideb,data->ikre,data->ideb1,data->ikre1,data->ideb2,data->ikre2);


  iceb_podpis(data->ff,data->window);


  fclose(data->ff);

  sprintf(strsql,"\n%*s:%15s",iceb_u_kolbait(18,gettext("Сальдо начальное")),gettext("Сальдо начальное"),iceb_u_prnbr(data->ideb));
  soob.new_plus(strsql);
  
  sprintf(strsql," %15s\n",iceb_u_prnbr(data->ikre));
  soob.plus(strsql);

  sprintf(strsql,"%*s:%15s",iceb_u_kolbait(18,gettext("Оборот за период")),gettext("Оборот за период"),iceb_u_prnbr(data->ideb1));
  soob.plus(strsql);

  sprintf(strsql," %15s\n",iceb_u_prnbr(data->ikre1));
  soob.plus(strsql);

  sprintf(strsql,"%*s:%15s",iceb_u_kolbait(18,gettext("Сальдо конечное")),gettext("Сальдо конечное"),iceb_u_prnbr(data->ideb2));
  soob.plus(strsql);

  sprintf(strsql," %15s\n\n",iceb_u_prnbr(data->ikre2));
  soob.plus(strsql);

  iceb_printw(soob.ravno(),data->buffer,data->view);
  



  iceb_printw_vr(data->vremn,data->buffer,data->view);
for(int nom=0; nom < data->imaf.kolih(); nom++)
 iceb_ustpeh(data->imaf.ravno(nom),3,data->window);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_grab_focus(data->knopka);

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
return(FALSE);

}
/*****************/
/*Шапка документа*/
/*****************/
void	sapnbs(short dn,short mn,short gn,short dk,short mk,short gk,
int sl,FILE *ff)
{
  fprintf(ff,"%100s%s N%d\n","",gettext("Лист"),sl);
  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff,gettext("  Код     |    Наименование    | C а л ь д о  на  %02d.%02d.%d |    Оборот за перiод         | С а л ь д о  на  %02d.%02d.%d |\n"),
  dn,mn,gn,dk,mk,gk);
  fprintf(ff,gettext("контраген-|     контрагента    -------------------------------------------------------------------------------------------\n"));
  fprintf(ff,gettext("   та     |                    |   Дебет      |   Кредит     |   Дебет      |    Кредит    |   Дебет      |   Кредит     |\n"));

  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------\n");
}
/********************/
/*Счетчик стpок*/
/********************/
void	shetnbs(short dn,short mn,short gn,short dk,short mk,short gk,
int *kolstrok,int *kollist,FILE *ff)
{
*kolstrok+=1;
if(*kolstrok > kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kolstrok=6;
  sapnbs(dn,mn,gn,dk,mk,gk,*kollist,ff);
  *kollist+=1;  
 }
}
