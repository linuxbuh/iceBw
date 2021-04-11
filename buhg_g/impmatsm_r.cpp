/*$Id: impmatsm_r.c,v 1.6 2013/11/24 08:25:06 sasa Exp $*/
/*23.05.2016	22.06.2013	Белых А.И.	impmatsm_r.c
импорт списка материалов
*/
#include  <errno.h>
#include  <sys/stat.h>
#include "buhg_g.h"

class impmatsm_r_data
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

  
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчёт завершен 1-нет
  impmatsm_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   impmatsm_r_key_press(GtkWidget *widget,GdkEventKey *event,class impmatsm_r_data *data);
gint impmatsm_r1(class impmatsm_r_data *data);
void  impmatsm_r_v_knopka(GtkWidget *widget,class impmatsm_r_data *data);

extern SQL_baza bd;
extern double okrcn;

int impmatsm_r(const char *imaf,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_spisok repl_s;
class impmatsm_r_data data;

data.imafz.new_plus(imaf);
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт списка материалов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(impmatsm_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Импорт списка материалов"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(impmatsm_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)impmatsm_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  impmatsm_r_v_knopka(GtkWidget *widget,class impmatsm_r_data *data)
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

gboolean   impmatsm_r_key_press(GtkWidget *widget,GdkEventKey *event,class impmatsm_r_data *data)
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

gint impmatsm_r1(class impmatsm_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
struct stat work;

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

float razmer=0;
#define KOLKOL 14
class iceb_u_str rek[KOLKOL];
SQL_str row;
class SQLCURSOR cur;
int ktoi=iceb_getuid(data->window);
int kolihoh=0;
int nom=0;
while(iceb_u_fgets(&strok,filimp.ff) == 0)
 {
  
  razmer+=strlen(strok.ravno());
  iceb_printw(strok.ravno(),data->buffer,data->view);

  if(strok.ravno()[0] == '#')
   continue;

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

  /*Проверяем наименование материала*/
  sprintf(strsql,"select naimat from Material where naimat='%s'",rek[2].ravno_filtr());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 0)
   {
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s\n",gettext("Такое наименование уже есть"));

    sprintf(strsql,"%s\n",gettext("Такое наименование уже есть"));
    iceb_printw(strsql,data->buffer,data->view);

    continue;
   }

  /*проверяем единицу измерения*/
  if(rek[4].getdlinna() > 1)
   {
    sprintf(strsql,"select naik from Edizmer where kod='%s'",rek[4].ravno_filtr());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     rek[4].new_plus("");
   }

  /*проверяем код группы*/
  if(rek[1].getdlinna() > 1)
   {
    sprintf(strsql,"select naik from Grup where kod=%d",rek[1].ravno_atoi());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     rek[1].new_plus("");
   }

  class iceb_lock_tables kkk("LOCK TABLES Material WRITE,Uslugi READ");
  /*проверяем код материала*/
  if(rek[0].ravno_atoi() > 0)
   {
    sprintf(strsql,"select naimat from Material where kodm='%d'",rek[0].ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) >= 1)
     {
      rek[0].new_plus(nomkmw(data->window));
     }
    else
     {
      sprintf(strsql,"select kodus from Uslugi where kodus=%d",rek[0].ravno_atoi());
      if(iceb_sql_readkey(strsql,data->window) >= 1)
       {
        rek[0].new_plus(nomkmw(data->window));
       }

     }
   }
  else
   rek[0].new_plus(nomkmw(data->window));  



/****
strcpy(strsql,"CREATE TABLE Material (\
kodm int not NULL primary key default 0,\
kodgr smallint not null default 0,\
naimat varchar(100) not null default '',\
strihkod varchar(50) not null default '',\
ei char(8) not null default '',\
kriost double not null default 0,\
cenapr double (15,6) not null default 0,\
krat double not null default 0,\
fasv double not null default 0,\
kodt int not null default 0,\
ktoz smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
nds  real not null default 0,\
msp smallint not null default 0,\
artikul varchar(20) not null default '',\
ku CHAR(32) NOT NULL DEFAULT '',\
unique (naimat),\
index (strihkod)) ENGINE = MYISAM");

0  kodm      Код материалла
1  kodgr     Код группы материалла
2  naimat    Наименование материалла
3  strihkod  Штрих-код
4  ei        Единица измерения
5  kriost    Критический остаток
6  cenapr    Цена реализации
7  krat      Кратность
8  fasv      Фасовка
9  kodt      Код тары
10 ktoz      Кто записал
11 vrem      Время записи
12 nds       1-цена введена без НДС 0-цена с НДС
13 msp       Метка скоропортящегося материалла
14 artikul   артикул
15 ku        код товара согласно УКТ ЗЕД
*/
  sprintf(strsql,"insert into Material (kodm,kodgr,naimat,strihkod,ei,kriost,cenapr,krat,fasv,kodt,ktoz,vrem,nds,msp,artikul,ku) \
values (%d,%d,'%s','%s','%s',%.10g,%.10g,%.10g,%.10g,%d,%d,%ld,%d,%d,'%s','%s')",
  rek[0].ravno_atoi(),
  rek[1].ravno_atoi(),
  rek[2].ravno_filtr(),
  rek[3].ravno_filtr(),
  rek[4].ravno_filtr(),
  rek[5].ravno_atof(),
  rek[6].ravno_atof(),
  rek[7].ravno_atof(),
  rek[8].ravno_atof(),
  rek[9].ravno_atoi(),
  ktoi,
  time(NULL),
  rek[10].ravno_atoi(),
  rek[11].ravno_atoi(),
  rek[12].ravno_filtr(),
  rek[13].ravno_filtr());
  
  if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
   {
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s\n",gettext("Ошибка записи в базу"));
    kolihoh++;
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

gtk_label_set_text(GTK_LABEL(data->label),gettext("Загрузка закончена"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);


repl.new_plus(gettext("Загрузка завершена"));

iceb_menu_soob(&repl,data->window);

data->voz=0;

return(FALSE);
}
