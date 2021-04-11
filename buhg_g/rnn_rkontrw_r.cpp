/*$Id: rnn_rkontrw_r.c,v 1.6 2013/09/26 09:46:54 sasa Exp $*/
/*23.05.2016	20.01.2012	Белых А.И.	rnn_rkontrw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "rnn_rkontrw.h"
#define		KOLSTRHAP 9 //Количество строк в шапке

class rnn_rkontrw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class rnn_rkontrw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  rnn_rkontrw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rnn_rkontrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rnn_rkontrw_r_data *data);
gint rnn_rkontrw_r1(class rnn_rkontrw_r_data *data);
void  rnn_rkontrw_r_v_knopka(GtkWidget *widget,class rnn_rkontrw_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;

int rnn_rkontrw_r(class rnn_rkontrw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
int gor=0;
int ver=0;
class rnn_rkontrw_r_data data;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка записей по контрагенту"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rnn_rkontrw_r_key_press),&data);

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

repl.plus(gettext("Распечатка записей по контрагенту"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(rnn_rkontrw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)rnn_rkontrw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rnn_rkontrw_r_v_knopka(GtkWidget *widget,class rnn_rkontrw_r_data *data)
{
//printf("rnn_rkontrw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rnn_rkontrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rnn_rkontrw_r_data *data)
{
// printf("rnn_rkontrw_r_key_press\n");
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
/*распечатка шапки документа*/
/*******************************/
void rnn_rkontr_rsd(FILE *ff)
{

fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
       Реквизиты документа                   |         Реквизиты налоговой накладной       |\n\
  Номер   |  Дата    |Сумма по док|Сумма НДС |  Номер   |  Дата    |Сумма по док|Сумма НДС |\n"));

/***********
1234567890|1234567890|123456789012|1234567890|
*************/
fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
}
/**********************************/
/*распечатка шапки документа развёрнутого отчёта для приходных документов*/
/*******************************/
void rnn_rkontr_rsd_ro(FILE *ff)
{

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
       Реквизиты документа                   |         Реквизиты налоговой накладной       |Внутренние документы |\n\
  Номер   |  Дата    |Сумма по док|Сумма НДС |  Номер   |  Дата    |Сумма по док|Сумма НДС |  Номер   |  Дата    |\n"));

/***********
1234567890|1234567890|123456789012|1234567890|
*************/
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");
}


/*****************************************/
/*приходные документы*/
/**********************************/
int rnn_rkontr_plus(class rnn_rkontrw_r_data *data,FILE *ff,FILE *ff_ro)
{
char strsql[512];
int kolstr=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;

short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 return(1);

sprintf(strsql,"select datd,nnn,sumd1,snds1,sumd2,snds2,sumd3,snds3,sumd4,snds4,nkontr,mi,datdi,nomdi,dvd \
from Reenn1 where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and inn='%s'",gn,mn,dn,gk,mk,dk,data->rk->inn.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного полученного документа!"),data->window);
  return(1);
 }

fprintf(ff,"\n%s\n",gettext("Полученные документы"));

rnn_rkontr_rsd(ff);
rnn_rkontr_rsd_ro(ff_ro);

int metka_ps=0;
float kolstr1=0;
double sumadok=0.;
double isumadok[2];
double sumands=0.;
double isumands[2];
class iceb_u_str nd_pos("");

memset(isumadok,'\0',sizeof(isumadok));
memset(isumands,'\0',sizeof(isumands));

while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  metka_ps=atoi(row[11]);

  sumadok=atof(row[2])+atof(row[4])+atof(row[6])+atof(row[8]);
  sumands=atof(row[3])+atof(row[5])+atof(row[7])+atof(row[9]);

  isumadok[1]+=sumadok;
  isumands[1]+=sumands;

  if(metka_ps == 0 || metka_ps == 5) /*запись сделана в ручную или из главной книги*/
   {
    fprintf(ff,"%10s|%10s|%12s|%10s|%-*s|%10s|%12.2f|%10.2f|\n",
    "","","","",
    iceb_u_kolbait(10,row[1]),
    row[1],
    iceb_u_datzap(row[0]),sumadok,sumands);

    class iceb_u_str datdok(iceb_u_datzap(row[12]));

    fprintf(ff_ro,"%10s|%10s|%12s|%10s|%-*s|%10s|%12.2f|%10.2f|%-10s|%10s|\n",
    "","","","",
    iceb_u_kolbait(10,row[1]),
    row[1],
    iceb_u_datzap(row[0]),sumadok,sumands,row[13],datdok.ravno());
    continue;
   }

  nd_pos.new_plus("");
  if(metka_ps == 1) /*материальный учёт*/
   {
    sprintf(strsql,"select sklad,nomon from Dokummat where datd='%s' and nomd='%s' and tip=1",row[12],row[13]);    
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
    
      nd_pos.new_plus(row1[1]);
/*************
      rsdat(&d,&m,&g,row[12],2);

      sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%s and nomd='%s' and nomerz=14",g,row1[0],row[13]);
      if(iceb_sql_readkey(strsql,&row1,&cur1) == 1)
       datavnn.new_plus(row1[0]);
***************/             
     }
   }

  if(metka_ps == 2) /*учёт услуг*/
   {
    sprintf(strsql,"select nomdp,datdp from Usldokum where datd='%s' and nomd='%s' and tp=1",row[12],row[13]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      nd_pos.new_plus(row1[0]);
      
     }
   }

  if(metka_ps == 3) /*учёт основных средств*/
   {
    sprintf(strsql,"select nomdv from Uosdok where datd='%s' and nomd='%s' and tipz=1",row[12],row[13]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      nd_pos.new_plus(row1[0]);
     }
   }
  

  if(metka_ps == 4) /*учёт командировочных расходов*/
   {
/**********
    sprintf(strsql,"select dvnn from Ukrdok where datd='%s' and nomd='%s' and ndrnn='%s'",row[12],row[13],row[1]);
    if(iceb_sql_readkey(strsql,&row1,&cur1) == 1)
     datavnn.new_plus(row1[0]);
**************/
    nd_pos.new_plus(row[1]);
    
    
   }

  if(metka_ps >0 && metka_ps < 5) /*запись импортирована*/
   {
    isumadok[0]+=sumadok;
    isumands[0]+=sumands;

    class iceb_u_str datnak(iceb_u_datzap(row[0]));    
    class iceb_u_str datnn(iceb_u_datzap(row[14]));

    fprintf(ff,"%-*s|%10s|%12.2f|%10.2f|%-*s|%10s|%12.2f|%10.2f|\n",
    iceb_u_kolbait(10,nd_pos.ravno()),
    nd_pos.ravno(),
    datnak.ravno(),
    sumadok,sumands,
    iceb_u_kolbait(10,row[1]),
    row[1],datnn.ravno(),sumadok,sumands);

    fprintf(ff_ro,"%-*s|%10s|%12.2f|%10.2f|%-*s|%10s|%12.2f|%10.2f|%-10s|%10s|\n",
    iceb_u_kolbait(10,nd_pos.ravno()),
    nd_pos.ravno(),
    datnak.ravno(),
    sumadok,sumands,
    iceb_u_kolbait(10,row[1]),
    row[1],datnn.ravno(),sumadok,sumands,row[13],iceb_u_datzap(row[12]));
   }
 }

fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
fprintf(ff_ro,"\
------------------------------------------------------------------------------------------------------------------\n");
 
fprintf(ff,"%*s|%12.2f|%10.2f|%21s|%12.2f|%10.2f|\n",
iceb_u_kolbait(21,gettext("Итого")),
gettext("Итого"),
isumadok[0],isumands[0],
"",isumadok[1],isumands[1]);

fprintf(ff_ro,"%*s|%12.2f|%10.2f|%21s|%12.2f|%10.2f|\n",
iceb_u_kolbait(21,gettext("Итого")),
gettext("Итого"),
isumadok[0],isumands[0],
"",isumadok[1],isumands[1]);




return(0);
}
/*****************************************/
/*расходные документы*/
/**********************************/
int rnn_rkontr_minus(class rnn_rkontrw_r_data *data,FILE *ff)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 return(1);

sprintf(strsql,"select datd,nnn,sumd,snds,nkontr,mi,datdi,nomdi \
from Reenn where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and inn='%s'",gn,mn,dn,gk,mk,dk,data->rk->inn.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного выданного документа !"),data->window);
  return(1);
 }

fprintf(ff,"\n%s\n",gettext("Выданные документы"));

rnn_rkontr_rsd(ff);

int metka_ps=0;
float kolstr1=0;
double sumadok=0.;
double isumadok[2];
double sumands=0.;
double isumands[2];
class iceb_u_str nd_pos("");

memset(isumadok,'\0',sizeof(isumadok));
memset(isumands,'\0',sizeof(isumands));

while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  metka_ps=atoi(row[5]);

  sumadok=atof(row[2]);
  sumands=atof(row[3]);

  isumadok[1]+=sumadok;
  isumands[1]+=sumands;

  if(metka_ps == 0 || metka_ps == 5) /*запись сделана в ручную или импортирована из главной книги*/
   {
    fprintf(ff,"%10s|%10s|%12s|%10s|%-*s|%10s|%12.2f|%10.2f|\n",
    "","","","",
    iceb_u_kolbait(10,row[1]),
    row[1],
    iceb_u_datzap(row[0]),sumadok,sumands);
    continue;
   }

  if(metka_ps > 0 && metka_ps < 5) /*запись импортирована*/
   {
    isumadok[0]+=sumadok;
    isumands[0]+=sumands;

    class iceb_u_str datnak(iceb_u_datzap(row[6]));    
    
    fprintf(ff,"%-*s|%10s|%12.2f|%10.2f|%-*s|%10s|%12.2f|%10.2f|\n",
    iceb_u_kolbait(10,row[7]),
    row[7],
    datnak.ravno(),
    sumadok,sumands,
    iceb_u_kolbait(10,row[1]),
    row[1],iceb_u_datzap(row[0]),sumadok,sumands);
   }
 }
fprintf(ff,"\
--------------------------------------------------------------------------------------------\n");
 
fprintf(ff,"%*s|%12.2f|%10.2f|%21s|%12.2f|%10.2f|\n",
iceb_u_kolbait(21,gettext("Итого")),
gettext("Итого"),
isumadok[0],isumands[0],
"",isumadok[1],isumands[1]);




return(0);
}


/******************************************/
/******************************************/

gint rnn_rkontrw_r1(class rnn_rkontrw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;



short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"select kodkon,naikon,kod from Kontragent where innn='%s'",data->rk->inn.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) < 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден контрагент с ИНН"),data->rk->inn.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 } 


FILE *ff=NULL;
char imaf[64];
sprintf(imaf,"pkontr%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

FILE *ff_ro=NULL;
char imaf_ro[64];
sprintf(imaf_ro,"pkontrr%d.lst",getpid());


iceb_zagolov(gettext("Рапечатка записей по контрагенту"),dn,mn,gn,dk,mk,gk,ff,data->window);
fprintf(ff,"\n%s:%s\n",gettext("Индивидуальный налоговый номер"),data->rk->inn.ravno());
fprintf(ff,"%s:%s %s\n",gettext("Контрагент"),row[0],row[1]);
fprintf(ff,"%s:%s\n",gettext("Код"),row[2]);


if(data->rk->metka_pr == 0 || data->rk->metka_pr == 1)
 {

  if((ff_ro = fopen(imaf_ro,"w")) == NULL)
   {
    iceb_er_op_fil(imaf_ro,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }
  iceb_zagolov(gettext("Рапечатка записей по контрагенту"),dn,mn,gn,dk,mk,gk,ff_ro,data->window);
  fprintf(ff_ro,"\n%s:%s\n",gettext("Индивидуальный налоговый номер"),data->rk->inn.ravno());
  fprintf(ff_ro,"%s:%s %s\n",gettext("Контрагент"),row[0],row[1]);
  fprintf(ff_ro,"%s:%s\n",gettext("Код"),row[2]);

  rnn_rkontr_plus(data,ff,ff_ro);

  iceb_podpis(ff_ro,data->window);
  fclose(ff_ro);
 }


if(data->rk->metka_pr == 0 || data->rk->metka_pr == 2)
  rnn_rkontr_minus(data,ff);


iceb_podpis(ff,data->window);
fclose(ff);

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Распечатка записей по контрагенту"));
if(ff_ro != NULL)
 {
  data->rk->imaf.plus(imaf_ro);
  data->rk->naimf.plus(gettext("Распечатка записей по контрагенту (развёрнутый отчёт)"));
 }

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
  iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);




gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
