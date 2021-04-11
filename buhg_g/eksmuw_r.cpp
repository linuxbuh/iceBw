/*$Id: eksmuw_r.c,v 1.6 2014/07/31 07:08:24 sasa Exp $*/
/*23.05.2016	18.06.2013	Белых А.И.	eksmuw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "eksmuw.h"
class eksmuw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class eksmuw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  eksmuw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   eksmuw_r_key_press(GtkWidget *widget,GdkEventKey *event,class eksmuw_r_data *data);
gint eksmuw_r1(class eksmuw_r_data *data);
void  eksmuw_r_v_knopka(GtkWidget *widget,class eksmuw_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;

int eksmuw_r(class eksmuw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
int gor=0;
int ver=0;
class eksmuw_r_data data;

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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(eksmuw_r_key_press),&data);

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(eksmuw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)eksmuw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  eksmuw_r_v_knopka(GtkWidget *widget,class eksmuw_r_data *data)
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

gboolean   eksmuw_r_key_press(GtkWidget *widget,GdkEventKey *event,class eksmuw_r_data *data)
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

gint eksmuw_r1(class eksmuw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
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
SQL_str row,row1;
class SQLCURSOR cur,cur1;

sprintf(strsql,"select * from Kart");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_fopen filost;
char imafost[64];
sprintf(imafost,"eks%d.csv",getpid());
if(filost.start(imafost,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_fopen filras;
char imafras[64];
sprintf(imafras,"eksr%d.lst",getpid());
if(filras.start(imafras,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_zageks(gettext("Экспорт остатков из материального учёта"),filost.ff,data->window);

fprintf(filost.ff,"#%s:%s\n",
gettext("Дата остатка"),
data->rk->dataost.ravno());


iceb_zagolov(gettext("Экспорт остатков материалов"),filras.ff,data->window);
fprintf(filras.ff,"%s:%s\n",gettext("Организация"),iceb_get_pnk("00",1,data->window));
fprintf(filras.ff,"%s:%s\n",gettext("Дата остатка"),data->rk->dataost.ravno());

if(data->rk->kodskl.getdlinna() > 1)
 {
  fprintf(filost.ff,"#%s:%s\n",gettext("Код склада"),data->rk->kodskl.ravno());
  fprintf(filras.ff,"%s:%s\n",gettext("Код склада"),data->rk->kodskl.ravno());
 }
if(data->rk->shetuh.getdlinna() > 1)
 {
  fprintf(filost.ff,"#%s:%s\n",gettext("Счёт учёта"),data->rk->shetuh.ravno());
  fprintf(filras.ff,"%s:%s\n",gettext("Счёт учёта"),data->rk->shetuh.ravno());
 }


fprintf(filost.ff,"#Расшифровка полей записи:\n\
#1  Код материалла\n\
#2  Наименование материала\n\
#3  0-цена c НДС 1-цена без НДС\n\
#4  Единица измерения\n\
#5  Счет учета\n\
#6  Цена учета\n\
#7  Цена продажи для розничной торговли\n\
#8  Кратность (количество в упаковке)\n\
#9  Н.Д.С\n\
#10 Фасовка (вес одной упаковки)\n\
#11 Код тары\n\
#12 Дата ввода в эксплуатацию (для малоценки) / Дата регистрации препарата для медикаментов\n\
#13 Инвентарный номер (для малоценки) / Серия для медикаментов\n\
#14 Регистрационный номер документа для мед. препарата/дата ввода в эксплуатацию для инвентарного номера\n\
#15 Номер заказа\n\
#16 Конечная дата годности товара\n\
#17 Количество\n");
  



fprintf(filras.ff,"\
--------------------------------------------------------------------------------\n");
fprintf(filras.ff,gettext("\
Склад|Карточка|Код мат.|Наименование материала|Счёт учёта|Цена учёта|Количество|\n"));

/*****  
12345 12345678 12345678 1234567890123456789012 1234567890 1234567890 1234567890
*********/
fprintf(filras.ff,"\
--------------------------------------------------------------------------------\n");
double ostatok=0.;

float kolstr1=0.;
class ostatok ost;
class iceb_u_str naim("");
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->kodskl.ravno(),row[0],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->shetuh.ravno(),row[5],0,0) != 0)
    continue;

  ostkarw(1,1,gost,dost,most,gost,atoi(row[0]),atoi(row[1]),&ost);
  if(ost.ostg[3] <= 0.00000009)
   continue;   
  ostatok=ost.ostg[3];
  sprintf(strsql,"select naimat from Material where kodm=%s",row[2]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim.new_plus(row1[0]);
  else
   naim.new_plus("");

/*
0  sklad    Код склада
1  nomk     Номер карточки

2  kodm     Код материалла
3  mnds     0-цена c НДС 1-цена без НДС
4  ei       Единица измерения
5  shetu    Счет учета
6  cena     Цена учета
7  cenap    Цена продажи для розничной торговли
8  krat     Кратность
9  nds      Н.Д.С
10 fas      Фасовка
11 kodt     Код тары
12 ktoi     Кто записал
13 vrem     Время записи
14 datv     Дата ввода в эксплуатацию (для малоценки) / Дата регистрации препарата для медикаментов
15 innom    Инвентарный номер (для малоценки) / Серия для медикаментов
16 rnd      Регистрационный номер документа для мед. препарата/дата ввода в эксплуатацию для инвентарного номера
17 nomz     Номер заказа
18 dat_god  Конечная дата годности товара

*/
  sprintf(strsql,"%6s %6s %6s %s\n",row[0],row[1],row[2],naim.ravno());
  iceb_printw(strsql,data->buffer,data->view);
  
  fprintf(filost.ff,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%f|\n",
  row[2],naim.ravno(),row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[14],row[15],row[16],row[17],row[18],ostatok);
  
  fprintf(filras.ff,"%-5s|%-8s|%-8s|%-*.*s|%-10s|%10s|%10.10g|\n",
  row[0],
  row[1],
  row[2],
  iceb_u_kolbait(22,naim.ravno()),
  iceb_u_kolbait(22,naim.ravno()),
  naim.ravno(),
  row[5],
  row[6],
  ostatok);
 }
 
fprintf(filras.ff,"\
--------------------------------------------------------------------------------\n");

iceb_podpis(filras.ff,data->window);
filost.end();
filras.end();


data->rk->imaf.plus(imafras);
data->rk->naimf.plus(gettext("Экспорт остатков материалов (распечатка)"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
  iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);


data->rk->imaf.plus(imafost);
data->rk->naimf.plus(gettext("Экспорт остатков материалов (файл для загрузки)"));

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
