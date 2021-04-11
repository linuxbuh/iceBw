/*$Id: impuslsu_r.c,v 1.6 2013/11/24 08:25:06 sasa Exp $*/
/*23.05.2016	22.06.2013	Белых А.И.	impuslsu_r.c
импорт списка услуг
*/
#include  <errno.h>
#include  <sys/stat.h>
#include "buhg_g.h"

class impuslsu_r_data
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
  impuslsu_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   impuslsu_r_key_press(GtkWidget *widget,GdkEventKey *event,class impuslsu_r_data *data);
gint impuslsu_r1(class impuslsu_r_data *data);
void  impuslsu_r_v_knopka(GtkWidget *widget,class impuslsu_r_data *data);

extern SQL_baza bd;
extern double okrcn;

int impuslsu_r(const char *imaf,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_spisok repl_s;
class impuslsu_r_data data;

data.imafz.new_plus(imaf);
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт справочника услуг"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(impuslsu_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Импорт справочника услуг"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(impuslsu_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)impuslsu_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  impuslsu_r_v_knopka(GtkWidget *widget,class impuslsu_r_data *data)
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

gboolean   impuslsu_r_key_press(GtkWidget *widget,GdkEventKey *event,class impuslsu_r_data *data)
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

gint impuslsu_r1(class impuslsu_r_data *data)
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



char strok[2048];


class iceb_fopen filimp;
if(filimp.start(data->imafz.ravno(),"r",data->window) != 0)
  return(FALSE);

char imaftmp[64];
sprintf(imaftmp,"imp%d.tmp",getpid());

class iceb_fopen filtmp;
if(filtmp.start(imaftmp,"w",data->window) != 0)
  return(FALSE);

int razmer=0;
int i=0;
#define KOLKOL 8
class iceb_u_str rek[KOLKOL];
SQL_str row;
class SQLCURSOR cur;
int ktoi=iceb_getuid(data->window);
int kolihoh=0;
int nom=0;
while(fgets(strok,sizeof(strok),filimp.ff) != NULL)
 {
  
  i=strlen(strok);
  razmer+=i;

  if(strok[0] == '#')
   continue;

  if(strok[i-2] == '\r') //Возврат каректки меняем на пробел
   strok[i-2]=' ';

//  printw("%s",strok);
  iceb_printw(strok,data->buffer,data->view);
  iceb_pbar(data->bar,work.st_size,razmer);    
//  strzag(LINES-1,0,work.st_size,razmer);

  for(nom=0; nom < KOLKOL; nom++)
   if(iceb_u_polen(strok,&rek[nom],nom+1,'|') != 0)
    {
     fprintf(filtmp.ff,"%s",strok);
     fprintf(filtmp.ff,"#%s %d\n",gettext("Не найдено поле"),nom+1);
     kolihoh++;
     break;     
    }

  if(nom < KOLKOL)
   continue;

  /*Проверяем наименование услуги*/
  sprintf(strsql,"select kodus from Uslugi where naius='%s'",rek[2].ravno_filtr());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 0)
   {
    fprintf(filtmp.ff,"%s",strok);
    fprintf(filtmp.ff,"#%s %s:%s\n",gettext("Такое наименование уже есть"),gettext("Код услуги"),row[0]);

    sprintf(strsql,"%s %s:%s\n",gettext("Такое наименование уже есть"),gettext("Код услуги"),row[0]);
    iceb_printw(strsql,data->buffer,data->view);

    continue;
   }

  /*проверяем единицу измерения*/
  if(rek[3].getdlinna() > 1)
   {
    sprintf(strsql,"select naik from Edizmer where kod='%s'",rek[3].ravno_filtr());
    if(iceb_sql_readkey(strsql,data->window ) != 1)
     rek[3].new_plus("");
   }

  /*проверяем код группы*/
  if(rek[1].getdlinna() > 1)
   {
    sprintf(strsql,"select kod from Uslgrup where kod=%d",rek[1].ravno_atoi());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     rek[1].new_plus("");

   }

  class iceb_lock_tables kkk("LOCK TABLES Material READ,Uslugi WRITE");
  /*проверяем код материала*/
  if(rek[0].ravno_atoi() > 0)
   {
    sprintf(strsql,"select kodus from Uslugi where kodus='%d'",rek[0].ravno_atoi());
    if(iceb_sql_readkey(strsql,data->window) >= 1)
     {
      rek[0].new_plus(nomkmw(data->window));
     }
    else
     {
      sprintf(strsql,"select naimat from Material where kodm='%d'",rek[0].ravno_atoi());
      if(iceb_sql_readkey(strsql,data->window) >= 1)
       {
        rek[0].new_plus(nomkmw(data->window));
       }

     }
   }
  else
   rek[0].new_plus(nomkmw(data->window));  
/******************
strcpy(strsql,"CREATE TABLE Uslugi (\
kodus   int not null default 0 primary key,\
kodgr   smallint not null default 0,\
naius   varchar(80) not null default '',\
ei      varchar(20) not null default '',\
shetu   varchar(20) not null default '' references Plansh (ns),\
cena    double(10,2) not null default 0,\
nds	real not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
art varchar(64) not null default '') ENGINE = MYISAM");


0 kodus   код услуги
1 kodgr   код группы
2 naius   наименование услуги
3 ei      единица измерения
4 shetu   счёт учета
5 cena    цена услуги
6 nds	0-НДС не применяется 1-применяется
7 ktoz	кто записал
8 vrem	время записи
9 art   артикул
*/
  sprintf(strsql,"insert into Uslugi (kodus,kodgr,naius,ei,shetu,cena,nds,ktoz,vrem,art) \
values (%d,%d,'%s','%s','%s',%.10g,%d,%d,%ld,'%s')",
  rek[0].ravno_atoi(),
  rek[1].ravno_atoi(),
  rek[2].ravno_filtr(),
  rek[3].ravno_filtr(),
  rek[4].ravno_filtr(),
  rek[5].ravno_atof(),
  rek[6].ravno_atoi(),  
  ktoi,
  time(NULL),
  rek[7].ravno_filtr());

  if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
   {
    fprintf(filtmp.ff,"%s",strok);
    fprintf(filtmp.ff,"#%s Uslugi\n",gettext("Ошибка записи в таблицу"));
    kolihoh++;
    continue;
   }
 }

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
