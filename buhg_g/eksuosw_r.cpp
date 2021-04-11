/*$Id: eksuosw_r.c,v 1.6 2014/07/31 07:08:24 sasa Exp $*/
/*23.05.2016	18.06.2013	Белых А.И.	eksuosw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "eksuosw.h"
class eksuosw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class eksuosw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  eksuosw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   eksuosw_r_key_press(GtkWidget *widget,GdkEventKey *event,class eksuosw_r_data *data);
gint eksuosw_r1(class eksuosw_r_data *data);
void  eksuosw_r_v_knopka(GtkWidget *widget,class eksuosw_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;

int eksuosw_r(class eksuosw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class eksuosw_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка списка документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(eksuosw_r_key_press),&data);

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

repl.plus(gettext("Распечатка списка документов"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(eksuosw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)eksuosw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  eksuosw_r_v_knopka(GtkWidget *widget,class eksuosw_r_data *data)
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

gboolean   eksuosw_r_key_press(GtkWidget *widget,GdkEventKey *event,class eksuosw_r_data *data)
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

gint eksuosw_r1(class eksuosw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[2048];
iceb_clock sss(data->window);

short dost=0,most=0,gost=0;
if(iceb_u_rsdat(&dost,&most,&gost,data->rk->dataost.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select * from Uosin");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  return(FALSE);
 }
class iceb_fopen fil;
char imafil[64];

sprintf(imafil,"eksuos%d.csv",getpid());

if(fil.start(imafil,"w",data->window) != 0)
  return(FALSE);

class iceb_fopen filrs;
char imafilrs[64];

sprintf(imafilrs,"eksuosr%d.lst",getpid());

if(filrs.start(imafilrs,"w",data->window) != 0)
  return(FALSE);

iceb_zageks(gettext("Экспорт основных средств"),filrs.ff,data->window);
fprintf(filrs.ff,"%s:%s\n",gettext("Дата остатка"),data->rk->dataost.ravno());


fprintf(fil.ff,"#Расшифровка полей записи:\n\
#1  инвентарный номер\n\
#2  год изготовления\n\
#3  наименование\n\
#4  завод изготовитель\n\
#5  паспорт\n\
#6  модель\n\
#7  заводской номер\n\
#8  дата ввода в эксплуатацию\n\
#9  счёт учета\n\
#10 шифр затрат\n\
#11 шифр аналитического учета\n\
#12 шифр нормы амотротчислений (группа) для налогового учёта\n\
#13 поправочный коэффициент для налогового учёта\n\
#14 состояние объекта 0-амортизация считается для бухучета и налогового учёта\n\
#                     1-амортизация не считается для бух.учета и налогового учёта\n\
#                     2-считается для бух.учета и не считается для налогового\n\
#                     3-не считается для бух учета и считается для налогового\n\
#15 номерной знак (для автомобилей)\n\
#16 шифр нормы амотротчислений (группа) для бух. учета\n\
#17 поправочный коэффициент для бух. учета\n\
#18 Балансовая стоимость для налогового учёта\n\
#19 Износ для налогового учёта\n\
#20 Балансовая стоимость для бухгалтерского учёта\n\
#21 Износ для бухгалтерского учёта\n");


if(data->rk->kodpdr.getdlinna() > 1)
 {
  fprintf(fil.ff,"#%s:%s\n",gettext("Код подразделения"),data->rk->kodpdr.ravno());
  fprintf(filrs.ff,"%s:%s\n",gettext("Код подразделения"),data->rk->kodpdr.ravno());
 } 

if(data->rk->kodmot.getdlinna() > 1)
 {
  fprintf(fil.ff,"#%s:%s\n",gettext("Код материально ответственного"),data->rk->kodmot.ravno());
  fprintf(filrs.ff,"%s:%s\n",gettext("Код материально ответственного"),data->rk->kodmot.ravno());
 } 
if(data->rk->shetuh.getdlinna() > 1)
 {
  fprintf(fil.ff,"#%s:%s\n",gettext("Счёт учёта"),data->rk->shetuh.ravno());
  fprintf(filrs.ff,"%s:%s\n",gettext("Счёт учёта"),data->rk->shetuh.ravno());
 } 
fprintf(filrs.ff,"\
---------------------------------------------------------------------------------------------------\n");
fprintf(filrs.ff,gettext("\
Инв-й ном.|Подр.|М.о.л|Наименование основного средства|Нал.стоим.|Нал.износ |Бух.стоим.|Бух.износ |\n"));
/***********
1234567890 12345 12345 1234567890123456789012345678901 1234567890 1234567890 1234567890 1234567890
***********/
fprintf(filrs.ff,"\
---------------------------------------------------------------------------------------------------\n");
/*****************************
strcpy(strsql,"CREATE TABLE Uosin (\
innom   int unsigned not null default 0 primary key,\
god     smallint not null default 0,\
naim	varchar(80) not null default '',\
zaviz   varchar(60) not null default '',\
pasp	varchar(60) not null default '',\
model   varchar(60) not null default '',\
zavnom  varchar(60) not null default '',\
datvv   DATE not null default '0000-00-00',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0)");

0 innom   инвентарный номер
1 god     год изготовления
2 naim	наименование
3 zaviz   завод изготовитель
4 pasp	паспорт
5 model   модель
6 zavnom  заводской номер
7 datvv   дата ввода в эксплуатацию
8 ktoz  кто записал
9 vrem  время записи
*********************************/
int kolstr1=0;
int innom=0;
int podr=0;
int kodotl=0;
class poiinpdw_data rekin;
class bsizw_data bal_st;

double bs_nu=0.;
double iznos_nu=0.;

double bs_bu=0.;
double iznos_bu=0.;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  innom=atoi(row[0]);
  
  if(poiinw(innom,dost,most,gost,&podr,&kodotl,data->window) != 0)
   continue;

  if(iceb_u_proverka(data->rk->kodpdr.ravno(),podr,0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->kodmot.ravno(),kodotl,0,0) != 0)
   continue;

  poiinpdw(innom,most,gost,&rekin,data->window);

  if(iceb_u_proverka(data->rk->shetuh.ravno(),rekin.shetu.ravno(),0,0) != 0)
   continue;


  sprintf(strsql,"%10s %s\n",row[0],row[2]);
  iceb_printw(strsql,data->buffer,data->view);


  bsizw(innom,podr,dost,most,gost,&bal_st,NULL,data->window);
/**********
  bs_nu=bal_st.sbs+bal_st.bs;
  iznos_nu=bal_st.siz+bal_st.iz+bal_st.iz1;

  bs_bu=bal_st.sbsby+bal_st.bsby;
  iznos_bu=bal_st.sizby+bal_st.izby+bal_st.iz1by;
*************/  
  bs_nu=bal_st.kbsnu;
  iznos_nu=bal_st.kiznu;

  bs_bu=bal_st.kbsbu;
  iznos_bu=bal_st.kizbu;
/****************************************

    double sbs,siz; //Стартовая балансовая стоимость и износ для налогового учёта
    double bs,iz; //Изменения балансовой стоимости и износа для налогового учёта
    double iz1; //Амортизация для налогового учёта
    double sbsby,sizby; //Стартовая балансовая стоимость и износ для бух.учёта
    double bsby,izby; //Изменения балансовой стоимости и износа для бух.учёта
    double iz1by; //Амортизация для бух. учёта
strcpy(strsql,"CREATE TABLE Uosinp (\
innom   int unsigned not null default 0,\
mes	smallint not null default 0,\
god     smallint not null default 0,\
shetu   varchar(20) not null default '' references Plansh (ns),\
hzt     varchar(20) not null default '',\
hau     varchar(20) not null default '',\
hna     varchar(20) not null default '',\
popkof  double(12,6) not null default 0.,\
soso    smallint not null default 0,\
nomz	varchar(40) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
hnaby   varchar(20) not null default '',\
popkofby double(12,6) not null default 0.,\
unique(innom,god,mes))");

0  innom   инвентарный номер
1  mes	   месяц записи
2  god     год записи,\
3  shetu   счёт учета
4  hzt     шифр затрат
5  hau     шифр аналитического учета
6  hna     шифр нормы амотротчислений (группа) для налогового учёта
7  popkof  поправочный коэффициент для налогового учёта
8  soso    состояние объекта 0-амортизация считается для бухучета и налогового учёта
                             1-амортизация не считается для бух.учета и налогового учёта
                             2-считается для бух.учета и не считается для налогового
                             3-не считается для бух учета и считается для налогового
9  nomz	   номерной знак (для автомобилей)
10 ktoz    кто записал
11 vrem    время записи
12 hnaby   шифр нормы амотротчислений (группа) для бух. учета
13 popkofby  поправочный коэффициент для бух. учета

  class iceb_u_str shetu; //Счет учета
  class iceb_u_str hzt; //Шифр затрат
  class iceb_u_str hau; //Шифр аналитического учета
  class iceb_u_str hna; //Шифр нормы амортотчислений для налогового учёта
  float popkf; //Поправочный коэффициент для налогового учёта
  int soso; //Состояние объекта
  class iceb_u_str nomz;  //Номерной знак
  short mz; short gz;  //дата записи
  class iceb_u_str hnaby; //Шифр нормы амортотчислений для бух. учета
  float popkfby;  ///Поправочный коэффициент для бух. учета


***************************************/
  fprintf(filrs.ff,"%10d|%5d|%5d|%-*.*s|%10.2f|%10.2f|%10.2f|%10.2f|\n",
  innom,
  podr,
  kodotl,
  iceb_u_kolbait(31,row[2]),
  iceb_u_kolbait(31,row[2]),
  row[2],
  bs_nu,iznos_nu,bs_bu,iznos_bu);  
    
  fprintf(fil.ff,"%s|%s|%s|%s|%s|%s|%s|%s|\
%s|%s|%s|%s|%f|%d|%s|%s|%f|\
%.2f|%.2f|%.2f|%.2f|\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],
  rekin.shetu.ravno(),rekin.hzt.ravno(),rekin.hna.ravno(),rekin.hna.ravno(),rekin.popkf,rekin.soso,rekin.nomz.ravno(),rekin.hnaby.ravno(),rekin.popkfby,
  bs_nu,iznos_nu,bs_bu,iznos_bu);  
 }

fprintf(filrs.ff,"\
---------------------------------------------------------------------------------------------------\n");
iceb_podpis(filrs.ff,data->window);

fil.end();
filrs.end();


data->rk->imaf.plus(imafilrs);
data->rk->naimf.plus(gettext("Экспорт основных средств (распечатка)"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
  iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);


data->rk->imaf.plus(imafil);
data->rk->naimf.plus(gettext("Экспорт основных средств (файл для загрузки)"));

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
