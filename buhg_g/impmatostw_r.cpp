/*$Id: impmatostw_r.c,v 1.6 2014/07/31 07:08:25 sasa Exp $*/
/*23.05.2016	17.06.2013	Белых А.И.	impmatost_r.c
импорт остатков материалов
*/
#include  <errno.h>
#include  <sys/stat.h>
#include "buhg_g.h"

class impmatost_r_data
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
  int sklad;
  short dd,md,gd;
  class iceb_u_str nomdok;
  
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчёт завершен 1-нет
  impmatost_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   impmatost_r_key_press(GtkWidget *widget,GdkEventKey *event,class impmatost_r_data *data);
gint impmatost_r1(class impmatost_r_data *data);
void  impmatost_r_v_knopka(GtkWidget *widget,class impmatost_r_data *data);

extern SQL_baza bd;
extern double okrcn;

int impmatost_r(const char *imaf,int tipz,int sklad,short dd,short md,short gd,const char *nomdok,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_spisok repl_s;
class impmatost_r_data data;
int gor=0;
int ver=0;

data.imafz.new_plus(imaf);
data.tipz=tipz;
data.sklad=sklad;
data.dd=dd;
data.md=md;
data.gd=gd;
data.nomdok.new_plus(nomdok);
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(impmatost_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Импорт документов"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(impmatost_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)impmatost_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  impmatost_r_v_knopka(GtkWidget *widget,class impmatost_r_data *data)
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

gboolean   impmatost_r_key_press(GtkWidget *widget,GdkEventKey *event,class impmatost_r_data *data)
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

gint impmatost_r1(class impmatost_r_data *data)
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
int razmer=0;
#define KOLKOL 17
class iceb_u_str rek[KOLKOL];
OPSHET reksh;
SQL_str row;
class SQLCURSOR cur;
int kodmat=0;
int ktoi=iceb_getuid(data->window);
int nomer_kart=0;
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
  if(rek[4].getdlinna() > 1)
   {
    if(iceb_prsh1(rek[4].ravno(),&reksh,data->window) != 0)
     {
      fprintf(filtmp.ff,"%s",strok.ravno());
      fprintf(filtmp.ff,"#%s %s\n",gettext("Не подходит счёт учёта"),rek[4].ravno());
      kolihoh++;
      continue;  
     }
   }
  else
   {
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s\n",gettext("Не введён счёт учёта"));
    kolihoh++;
    continue;  
   }
  /*проверяем единицу измерения*/
  if(rek[4].getdlinna() > 1)
   {  
    sprintf(strsql,"select kod from Edizmer where kod='%s'",rek[3].ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Не найдена единица измерения"),rek[3].ravno());
      iceb_menu_soob(strsql,data->window);
      fprintf(filtmp.ff,"#%s\n",strsql);
      kolihoh++;
      continue;
     }
   }
  else
   {
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s\n",gettext("Не введена единица измерения"));
    kolihoh++;
    continue;  
   }
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
  
//  fprintf(filost.ff,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%f|\n",
//  row[2],naim.ravno(),row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[14],row[15],row[16],row[17],row[18],ostatok);

  /*проверяем есть ли материал в списке материалов*/
  sprintf(strsql,"select kodm from Material where naimat='%s'",rek[1].ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   {
    kodmat=atoi(row[0]);
   }
  else
   {
    class iceb_lock_tables kkk("LOCK TABLES Material WRITE,Uslugi READ");
    /*проверяем есть ли в списке материалов код материала если есть нужно загрузить с другим кодом*/
    if(rek[0].ravno_atoi() != 0)
     {
      kodmat=rek[0].ravno_atoi();
      sprintf(strsql,"select kodm from Material where kodm=%d",rek[0].ravno_atoi());
      if(iceb_sql_readkey(strsql,data->window) == 0)
       {
        kodmat=nomkmw(data->window);
       }
      else
       {
        sprintf(strsql,"select kodus from Uslugi where kodus=%d",kodmat);
        if(iceb_sql_readkey(strsql,data->window) == 1)
         {
          kodmat=nomkmw(data->window);
         }
        
       }
     }
    else
      kodmat=nomkmw(data->window);

    sprintf(strsql,"insert into Material (kodm,naimat,ei,ktoz,vrem) \
values (%d,'%s','%s',%d,%ld)",
    kodmat,rek[1].ravno_filtr(),rek[3].ravno_filtr(),ktoi,time(NULL));
    if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
     {
      fprintf(filtmp.ff,"%s",strok.ravno());
      fprintf(filtmp.ff,"#%s Material\n",gettext("Ошибка записи в таблицу"));
      kolihoh++;
      continue;
     }
   }  
  nomer_kart=0;
  if(tipz == 1) /*для приходного документа определяем в какую карточку запись. Для расходного без привязки к карточке*/
   {
    /*проверяем есть ли нужная карточка*/
    //  index (kodm,sklad,shetu,cena,ei,nds,cenap,krat,mnds),
    sprintf(strsql,"select nomk from Kart where kodm=%d and sklad=%d and shetu='%s' and cena=%s and ei='%s' and nds=%s and cenap=%s and krat=%s and mnds=%s",
    kodmat,data->sklad,rek[4].ravno(),rek[5].ravno(),rek[3].ravno_filtr(),rek[8].ravno(),rek[6].ravno(),rek[7].ravno(),rek[2].ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
     {
      nomer_kart=atoi(row[0]);
      /*Если катрочка есть то может быт и запись в документе проверяем и увеличиваем количество если есть*/
      sprintf(strsql,"select kolih from Dokummat1 where sklad=%d and nomkar=%d and datd='%04d-%02d-%02d' and nomd='%s' and kodm=%d and tipz=%d",
      data->sklad,nomer_kart,data->gd,data->md,data->dd,data->nomdok.ravno(),kodmat,tipz);
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        double kolih=atof(row[0])+rek[16].ravno_atof();
        sprintf(strsql,"update Dokummat1 set kolih=%.10g where sklad=%d and nomkar=%d and datd='%04d-%02d-%02d' and nomd='%s' and kodm=%d and tipz=%d",
        kolih,data->sklad,nomer_kart,data->gd,data->md,data->dd,data->nomdok.ravno(),kodmat,tipz);
        iceb_sql_zapis(strsql,1,0,data->window);
        continue;
       }      
     }
    else
     {
      class iceb_lock_tables kkk("LOCK TABLES Kart WRITE");
      nomer_kart=nomkrw(data->sklad,data->window);
      
      sprintf(strsql,"insert into Kart (sklad,nomk,kodm,mnds,ei,shetu,cena,cenap,\
krat,nds,fas,kodt,\
ktoi,vrem,\
datv,innom,rnd,nomz,dat_god)\
 values (%d,%d,%d,%s,'%s','%s',%s,%s,\
%s,%s,%s,%s,\
%d,%ld,\
'%s','%s','%s','%s','%s')",
      data->sklad,nomer_kart,kodmat,rek[2].ravno(),rek[3].ravno_filtr(),rek[4].ravno(),rek[5].ravno(),rek[6].ravno(),
      rek[7].ravno(),rek[8].ravno(),rek[9].ravno(),rek[10].ravno(),
      ktoi,time(NULL),
      rek[11].ravno(),rek[12].ravno(),rek[13].ravno(),rek[14].ravno(),rek[15].ravno());

      if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
       {
        fprintf(filtmp.ff,"%s",strok.ravno());
        fprintf(filtmp.ff,"#%s Kart\n",gettext("Ошибка записи в таблицу"));
        kolihoh++;
        continue;
       }
     }
   }
/**************
  strcpy(strsql,"CREATE TABLE Dokummat1 (\
datd DATE not null default '0000-00-00',\
sklad  smallint not null  default 0 references Sklad (kod),\
nomd   char(16) not null default '',\
nomkar int not null default 0,\
kodm   int not null  default 0 references Material (kodm),\
kolih  double(16,6) not null default 0,\
cena   double(16,6) not null default 0,\
ei     varchar(8) not null default '' references Edizmer (kod),\
voztar smallint not null default 0,\
nds    float(2) not null default 0,\
mnds   smallint not null default 0,\
ktoi   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
tipz   smallint not null default 0,\
nomkarp int not null default 0,\
shet   varchar(12) not null default '',\
dnaim  varchar(70) not null default '',\
nomz   varchar(20) not null default '',\
index (sklad,nomd,kodm,nomkar),\
index (sklad,nomkar),\
index (kodm,sklad),\
index (datd,sklad,nomd))");

0  datd   Дата документа
1  sklad  Склад
2  nomd   Номер документа
3  nomkar Номер карточки
4  kodm   Код материалла
5  kolih  Количество
6  cena   Цена
7  ei     Единица измерения
8  voztar 1-Метка возвратной тары
9  nds    НДС
10 mnds   0- цена без НДС 1-цена с НДС в том числе
11 ktoi   Кто записал
12 vrem   Время записи
13 tipz   1-приход 2-расход
14 nomkarp номер карточки парного документа
15 shet   счёт списания/приобретения
16 dnaim  Дополнительное наименование материалла
17 nomz   Номер заказа
*/
  /*записываем в документ*/
  sprintf(strsql,"insert into Dokummat1 (datd,sklad,nomd,nomkar,kodm,kolih,cena,ei,nds,mnds,ktoi,vrem,tipz,shet) \
values('%04d-%02d-%02d',%d,'%s',%d,%d,%s,%s,'%s',%s,%s,%d,%ld,%d,'%s')",
  data->gd,data->md,data->dd,data->sklad,data->nomdok.ravno(),nomer_kart,kodmat,rek[16].ravno(),rek[5].ravno(),rek[3].ravno_filtr(),rek[8].ravno(),rek[2].ravno(),
  ktoi,time(NULL),tipz,rek[4].ravno());

  if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
   {
    fprintf(filtmp.ff,"%s",strok.ravno());
    fprintf(filtmp.ff,"#%s Dokummat1\n",gettext("Ошибка записи в таблицу"));
    kolihoh++;
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

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);


repl.new_plus(gettext("Загрузка завершена"));

iceb_menu_soob(&repl,data->window);

data->voz=0;

return(FALSE);
}
