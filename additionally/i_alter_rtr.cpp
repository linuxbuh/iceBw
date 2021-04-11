/*$Id: i_alter_rtr.c,v 1.28 2013/08/13 06:10:23 sasa Exp $*/
/*19.07.2016	29.04.2004	Белых А.И.	i_alter_rtr.c
Преобразование таблиц
*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <iceb_libbuh.h>

class alter_rtr_data
 {
  public:
  

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  time_t    vremn;
  int       kolstr;
  gfloat    kolstr1;  
  iceb_u_str imabaz;
  iceb_u_str host; 
  iceb_u_str parol;

  //Конструктор  
  alter_rtr_data()
   {
    kolstr=20;
    kolstr1=0.;
   }

 };

gboolean   alter_rtr_key_press(GtkWidget *widget,GdkEventKey *event,class alter_rtr_data *data);
gint alter_rtr1(class alter_rtr_data *data);
void  alter_rtr_v_knopka(GtkWidget *widget,class alter_rtr_data *data);

extern SQL_baza bd;
extern char *name_system;

void i_alter_rtr(const char *imabaz,const char *host,const char *parol,GtkWidget *wpredok)
{
char strsql[300];
iceb_u_str soob;

alter_rtr_data data;
data.imabaz.plus(imabaz);
data.host.plus(host);
data.parol.plus(parol);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",name_system,"Преобразование таблиц");
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(alter_rtr_key_press),&data);

if(wpredok != NULL)
 {
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанваливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

sprintf(strsql,"%s:%s","Преобразование таблиц базы данных",data.imabaz.ravno());

soob.new_plus(strsql);

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



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);
gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(alter_rtr_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)alter_rtr1,&data);

gtk_main();


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  alter_rtr_v_knopka(GtkWidget *widget,class alter_rtr_data *data)
{
 printf("alter_rtr_v_knopka\n");
 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   alter_rtr_key_press(GtkWidget *widget,GdkEventKey *event,class alter_rtr_data *data)
{
 printf("alter_rtr_key_press\n");

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

gint alter_rtr1(class alter_rtr_data *data)
{
char strsql[1000];

//1
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

time(&data->vremn);

iceb_printw("Открываем базу\n",data->buffer,data->view);

if(sql_openbaz(&bd,data->imabaz.ravno(),data->host.ravno(),"root",data->parol.ravno()) != 0)
 {
  iceb_eropbaz(data->imabaz.ravno(),0,"",1);

  gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

  gtk_widget_grab_focus(data->knopka);
  gtk_widget_show_all(data->window);
  return(FALSE);

 }
iceb_start_rf();

//2
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw("Вставляем колонку podr в таблицу Restdok\n",data->buffer,data->view);

sprintf(strsql,"alter table Restdok add podr int not null");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонка в таблице уже существует.\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }

 }
else
 {
  iceb_printw("Удаляем индексы mo и datd в таблице Restdok\n",data->buffer,data->view);

  sprintf(strsql,"alter table Restdok drop index mo, drop index datd");

  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
  iceb_printw("Добавляем индексы mo,podr и datd,podr в таблицу Restdok.\n",data->buffer,data->view);
  sprintf(strsql,"alter table Restdok add index mo (mo,podr), add index datd (datd,podr)");
  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }
 
iceb_printw("Вставляем колонку podr в таблицу Restdok1\n",data->buffer,data->view);

sprintf(strsql,"alter table Restdok1 add podr int not null");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонка в таблице уже существует\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }

//3  
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw("Вставляем колонку nz и mr в таблицу Restdok1\n",data->buffer,data->view);

sprintf(strsql,"alter table Restdok1 add nz smallint not null, add mr smallint not null");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонки в таблице уже существуют\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }

 }
else
 {
  iceb_printw("Удаляем индекс datd в таблице Restdok1\n",data->buffer,data->view);

  sprintf(strsql,"alter table Restdok1 drop index datd");

  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }

  iceb_printw("Добавляем индекс  datd в таблицу Restdok1\n",data->buffer,data->view);
  sprintf(strsql,"alter table Restdok1 add unique datd (datd,nomd,skl,mz,kodz,nz)");
  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }
//4
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw("Вставляем колонки vremz,vremo в таблицу Restdok\n",data->buffer,data->view);

sprintf(strsql,"alter table Restdok add vremz int unsigned not null, add vremo int unsigned not null");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонки в таблице уже существуют\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }

//5
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw("Вставляем колонки ps (процент скидки и grup (группа) в таблицу Taxiklient\n",data->buffer,data->view);

sprintf(strsql,"alter table Taxiklient add ps float not null,add grup char(10) not null");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонкa в таблице уже существует\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }

//6
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw("Создаём таблицу групп клиентов Grupklient\n",data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Grupklient \
(kod    char(10) not null primary key,\
naik    varchar(100) not null,\
ktoz	smallint unsigned not null,\
vrem	int unsigned not null)");
/*
0 kod   код группы
1 naik  наименование группы
2 ktoz  кто записал
3 vrem  время записи
*/
if(sql_zap(&bd,strsql) < 0)
 { 
  sprintf(strsql,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  iceb_printw(strsql,data->buffer,data->view);
 } 
else
  iceb_printw("Таблица создана !\n",data->buffer,data->view);

//7
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

iceb_printw("Вставляем колонку ps (процент скидки таблицу Restdok\n",data->buffer,data->view);

sprintf(strsql,"alter table Restdok add ps float not null");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонкa в таблице уже существует\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }


//8
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw("Вставляем колонку nds в таблицу Restdok1\n",data->buffer,data->view);

sprintf(strsql,"alter table Restdok1 add nds tinyint not null");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонка в таблице уже существует\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }

//9
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw("Вставляем колонку sp в таблицу Taxiklient\n",data->buffer,data->view);

sprintf(strsql,"alter table Taxiklient add sp varchar(100) not null");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонка в таблице уже существует\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }

//10
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw("Вставляем колонку ps в таблицу Restdok1\n",data->buffer,data->view);

sprintf(strsql,"alter table Restdok1 add ps float not null");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонка в таблице уже существует\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }

//11
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw("Вставляем колонку mk в таблицу Taxiklient\n",data->buffer,data->view);

sprintf(strsql,"alter table Taxiklient add mk tinyint not null");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонка в таблице уже существует\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }

//12
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw("Вставляем колонку dvk в таблицу Taxiklient\n",data->buffer,data->view);

sprintf(strsql,"alter table Taxiklient add dvk DATETIME not null");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонка в таблице уже существует\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }

//13
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw("Вставляем колонку pl в таблицу Taxiklient\n",data->buffer,data->view);

sprintf(strsql,"alter table Taxiklient add pl tinyint not null");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонка в таблице уже существует\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }

//14
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw("Вставляем колонку cdg в таблицу Restvi\n",data->buffer,data->view);

sprintf(strsql,"alter table Restvi add cdg float(7,2) not null");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонка в таблице уже существует\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }

//15
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw("Вставляем колонку denrog в таблицу Taxiklient\n",data->buffer,data->view);

sprintf(strsql,"alter table Taxiklient add denrog DATE not null");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонка в таблице уже существует\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }

//16
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw("Вставляем колонку kom в таблицу Taxiklient\n",data->buffer,data->view);

sprintf(strsql,"alter table Taxiklient add kom VARCHAR(100) NOT NULL");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонка в таблице уже существует\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }

//17
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw("Добавляем индекс nk,datz,kodz в таблицу Restkas\n",data->buffer,data->view);

sprintf(strsql,"alter table Restkas add index nk1 (nk,kodz,datz)");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_KEYNAME)
   {
    iceb_printw("Индексы в таблице уже существуют\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }

//18
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw("Вставляем колонку mb в таблицу Taxiklient\n",data->buffer,data->view);

sprintf(strsql,"alter table Taxiklient add mb tinyint not null");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    iceb_printw("Колонка в таблице уже существует\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }

//19
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw("Добавляем индекс в таблицу Restdok1\n",data->buffer,data->view);

sprintf(strsql,"alter table Restdok1 add index kodz (kodz,skl,mz,ms)");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_KEYNAME)
   {
    iceb_printw("Индекс уже существует\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }

//20
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw("Добавляем индекс в таблицу Restdok1\n",data->buffer,data->view);

sprintf(strsql,"alter table Restdok1 add index skl (skl,ms,mz)");
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_KEYNAME)
   {
    iceb_printw("Индекс уже существует\n",data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"Ошибка ! %d\n%s\n",sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }



/*******************************/

sql_closebaz(&bd);

iceb_printw_vr(data->vremn,data->buffer,data->view);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчет закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);
return(FALSE);

}

