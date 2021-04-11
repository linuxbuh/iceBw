/*$Id: impuosostw_r.c,v 1.6 2014/07/31 07:08:26 sasa Exp $*/
/*23.05.2016	18.06.2013	Белых А.И.	impuosost_r.c
импорт материалов
*/
#include  <errno.h>
#include  <sys/stat.h>
#include "buhg_g.h"

class impuosost_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
    
  class iceb_u_str imafz;

  int tipz;
  short dd,md,gd;
  class iceb_u_str kodop;
  class iceb_u_str nomdok;
  int podr;
  int kodol;
    
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчёт завершен 1-нет
  impuosost_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   impuosost_r_key_press(GtkWidget *widget,GdkEventKey *event,class impuosost_r_data *data);
gint impuosost_r1(class impuosost_r_data *data);
void  impuosost_r_v_knopka(GtkWidget *widget,class impuosost_r_data *data);

extern SQL_baza bd;
extern double okrcn;

int impuosost_r(const char *imaf,int tipz,short dd,short md,short gd,const char *nomdok,const char *kodop,int podr,int kodol,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_spisok repl_s;
int gor=0;
int ver=0;
class impuosost_r_data data;

data.imafz.new_plus(imaf);
data.tipz=tipz;
data.dd=dd;
data.md=md;
data.gd=gd;
data.nomdok.new_plus(nomdok);
data.kodop.new_plus(kodop);
data.podr=podr;
data.kodol=kodol;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт основных сердств"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(impuosost_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Импорт основных средств"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(impuosost_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)impuosost_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  impuosost_r_v_knopka(GtkWidget *widget,class impuosost_r_data *data)
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

gboolean   impuosost_r_key_press(GtkWidget *widget,GdkEventKey *event,class impuosost_r_data *data)
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

gint impuosost_r1(class impuosost_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
struct stat work;
int tipz=1;

if(stat(data->imafz.ravno(),&work) != 0)
 {
  repl.new_plus(gettext("Нет такого файла !"));
  repl.ps_plus(data->imafz.ravno());
  iceb_menu_soob(&repl,data->window);
  return(FALSE);
 }


class iceb_u_str strok("");

class iceb_fopen filimp;
if(filimp.start(data->imafz.ravno(),"r",data->window) != 0)
  return(FALSE);

char imaftmp[64];
sprintf(imaftmp,"imp%d.tmp",getpid());

class iceb_fopen filtmp;
if(filtmp.start(imaftmp,"w",data->window) != 0)
  return(FALSE);

sprintf(strsql,"%s\n\n",gettext("Загружаем записи из файла"));
iceb_printw(strsql,data->buffer,data->view);
float razmer=0;
#define KOLKOL 21
class iceb_u_str rek[KOLKOL];
OPSHET reksh;
SQL_str row;
class SQLCURSOR cur;
int ktoi=iceb_getuid(data->window);
int kolihoh=0;
int nom=0;
while(iceb_u_fgets(&strok,filimp.ff) == 0)
 {
  
  razmer+=strlen(strok.ravno());

  if(strok.ravno()[0] == '#')
   continue;

  iceb_printw(strok.ravno(),data->buffer,data->view);
  iceb_pbar(data->bar,work.st_size,razmer);    

  for(nom=0; nom < KOLKOL; nom++)
   if(iceb_u_polen(strok.ravno(),&rek[nom],nom+1,'|') != 0)
    {
     fprintf(filtmp.ff,"%s",strok.ravno());
     fprintf(filtmp.ff,"#%s %d\n",gettext("Не найдено поле"),nom+1);
     kolihoh++;
     break;     
    }

  if(nom < KOLKOL)
   continue;

  /*проверяем счёт*/
  if(rek[8].getdlinna() > 1)
  if(iceb_prsh1(rek[8].ravno(),&reksh,data->window) != 0)
   {
    rek[8].new_plus("");
   }
  /*проверяем шифр затрат*/
  sprintf(strsql,"select kod from Uoshz where kod='%s'",rek[9].ravno_filtr());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    rek[9].new_plus("");
   }

  /*проверяем шифр аналитического учёта*/
  sprintf(strsql,"select kod from Uoshau where kod='%s'",rek[10].ravno_filtr());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    rek[10].new_plus("");
   }

  /*проверяем группу налогового учёта (шифр нормы амортотчислений)*/
  sprintf(strsql,"select kod from Uosgrup where kod='%s'",rek[11].ravno_filtr());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    rek[11].new_plus("");
   }

  /*проверяем группу бухгалтерского учёта (шифр нормы амортотчислений)*/
  sprintf(strsql,"select kod from Uosgrup1 where kod='%s'",rek[15].ravno_filtr());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    rek[15].new_plus("");
   }
  
/**************
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
*/

  /*проверяем есть ли такой инвентарный номер*/
  sprintf(strsql,"select naim from Uosin where innom=%d",rek[0].ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   {
    rek[0].new_plus(iceb_invnomer(data->window));
   }

  sprintf(strsql,"insert into Uosin (innom,god,naim,zaviz,pasp,model,zavnom,datvv,ktoz,vrem) \
values (%d,%d,'%s','%s','%s','%s','%s','%s',%d,%ld)",
  rek[0].ravno_atoi(),rek[1].ravno_atoi(),rek[2].ravno_filtr(),rek[3].ravno_filtr(),
  rek[4].ravno_filtr(),rek[5].ravno_filtr(),rek[6].ravno_filtr(),rek[7].ravno_filtr(),ktoi,time(NULL));
  if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
   {
    kolihoh++;
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s Uosin\n",gettext("Ошибка записи в таблицу"));
    continue;  
   }
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
    
  fprintf(fil.ff,"%s|%s|%s|%s|%s|%s|%s|%s|\
%s|%s|%s|%s|%f|%d|%s|%s|%f|\
%.2f|%.2f|%.2f|%.2f|\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],
  rekin.shetu.ravno(),rekin.hzt.ravno(),rekin.hna.ravno(),rekin.hna.ravno(),rekin.popkf,rekin.soso,rekin.nomz.ravno(),rekin.hnaby.ravno(),rekin.popkfby,
  bs_nu,iznos_nu,bs_bu,iznos_bu);  

*********************************/
  
  sprintf(strsql,"insert into Uosinp (innom,mes,god,shetu,hzt,hau,hna,popkof,soso,nomz,ktoz,vrem,hnaby,popkofby) \
values (%d,%d,%d,'%s','%s','%s','%s',%f,%d,'%s',%d,%ld,'%s',%f)",
  rek[0].ravno_atoi(),data->md,data->gd,rek[8].ravno_filtr(),rek[9].ravno_filtr(),rek[10].ravno_filtr(),rek[11].ravno_filtr(),
  rek[12].ravno_atof(),rek[13].ravno_atoi(),rek[14].ravno_filtr(),ktoi,time(NULL),rek[15].ravno_filtr(),rek[16].ravno_atof());

  if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
   {
    kolihoh++;
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s Uosinp\n",gettext("Ошибка записи в таблицу"));
    continue;  
   }

/*********************
strcpy(strsql,"CREATE TABLE Uosdok1 (\
datd	DATE not null default '0000-00-00',\
tipz	smallint not null default 0,\
podt	smallint not null default 0,\
innom   int unsigned not null default 0,\
nomd	varchar(20) not null default '',\
podr	smallint not null default 0,\
kodol   int not null default 0,\
kol     smallint not null default 0,\
bs	double(12,2) not null default 0,\
iz	double(12,2) not null default 0,\
kind	float not null default 0,\
kodop   varchar(20) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
bsby	double(12,2) not null default 0,\
izby    double(12,2) not null default 0,\
cena	double(14,2) not null default 0,\
shs     varchar(20) not null default '',\
unique(datd,nomd,innom),\
index(innom,datd))");

0  datd	  дата документа
1  tipz	  1-приход 2-расход
2  podt	  0-не подтверждена 1-подтверждена
3  innom  инвентарный номер
4  nomd	  номер документа
5  podr   подразделение
6  kodol  код ответственного лица
7  kol    количество : +1 приход -1 расход 0 изменение стоимости
8  bs	  балансовая стоимость для налогового учёта
9  iz	  износ для налогового учёта
10 kind	  коэффициент индексации
11 kodop  код операции
12 ktoz   кто записал
13 vrem   время записи
14 bsby	  балансовая стоимость для бух-учета
15 izby	  износ для бух-учета
16 cena   цена продажи
17 shs    счёт получения/списания
*/
  sprintf(strsql,"insert into Uosdok1 (datd,tipz,podt,innom,nomd,podr,kodol,kol,bs,iz,kind,kodop,ktoz,vrem,bsby,izby,cena,shs) \
values('%04d-%02d-%02d',%d,%d,%d,'%s',%d,%d,%d,%.2f,%.2f,%f,'%s',%d,%ld,%.2f,%.2f,%.2f,'%s')",
  data->gd,data->md,data->dd,tipz,0,rek[0].ravno_atoi(),
  data->nomdok.ravno(),data->podr,data->kodol,1,rek[17].ravno_atof(),rek[18].ravno_atof(),0.,data->kodop.ravno(),ktoi,time(NULL),
  rek[19].ravno_atof(),rek[20].ravno_atof(),0.,rek[8].ravno_filtr());
  if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
   {
    kolihoh++;
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s Uosdok1\n",gettext("Ошибка записи в таблицу"));
    continue;  
   }


 }
iceb_pbar(data->bar,work.st_size,work.st_size);

filtmp.end();
filimp.end();

unlink(data->imafz.ravno());

rename(imaftmp,data->imafz.ravno());

if(kolihoh == 0)
 iceb_menu_soob(gettext("Все записи загружены"),data->window);
else
 {
  sprintf(strsql,"%s %d",gettext("Количество не загруженых записей"),kolihoh);
  iceb_menu_soob(strsql,data->window);
 }

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Импорт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);


repl.new_plus(gettext("Загрузка завершена"));

iceb_menu_soob(&repl,data->window);

data->voz=0;

return(FALSE);
}
