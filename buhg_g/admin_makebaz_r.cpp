/*$Id: admin_makebaz_r.c,v 1.52 2014/08/31 06:19:18 sasa Exp $*/
/*20.04.2018	02.09.2004	Белых А.И.	admin_makebaz_r.c
Создание таблиц базы данных
*/
#include <errno.h>
#include <iceb_libbuh.h>

class admin_makebaz_r_data
 {
  public:
  

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  GtkWidget *view_error;
  GtkTextBuffer *buffer_error;
  GtkWidget *vert_panel;
  class iceb_u_str name_window;
  class iceb_u_str name_paned;
  time_t    vremn;
  int       kolstr;
  gfloat    kolstr1;  
  class iceb_u_str imabaz;
  class iceb_u_str host; 
  class iceb_u_str parol;
  class iceb_u_str kodirovka;
  class iceb_u_str fromnsi;
  class iceb_u_str fromdoc;
  //Конструктор  
  admin_makebaz_r_data()
   {
    kolstr=139+2;/*плюс загрузка файлов alx и doc*/
    kolstr1=0.;
   }

 };

gboolean   admin_makebaz_r_key_press(GtkWidget *widget,GdkEventKey *event,class admin_makebaz_r_data *data);
gint admin_makebaz_r1(class admin_makebaz_r_data *data);
void  admin_makebaz_r_v_knopka(GtkWidget *widget,class admin_makebaz_r_data *data);

int admin_cp_alx(const char *from_alx,const char *to_alx,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);
int alxinw(const char *put_alx,int metka,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);
void docinw(const char *put_na_doc,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);

extern SQL_baza bd;

void admin_makebaz_r(const char *imabaz,const char *host,const char *parol,const char *kodirovka,
const char *fromnsi,
const char *fromdoc,
GtkWidget *wpredok)
{
char strsql[1024];
int gor=0;
int ver=0;

admin_makebaz_r_data data;
data.imabaz.plus(imabaz);
data.host.plus(host);
data.parol.plus(parol);
data.kodirovka.plus(kodirovka);
data.fromnsi.new_plus(fromnsi);
data.fromdoc.new_plus(fromdoc);
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Разметка таблиц базы данных"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(admin_makebaz_r_key_press),&data);

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

sprintf(strsql,"%s:%s",gettext("Разметка таблиц базы данных"),data.imabaz.ravno());

GtkWidget *label=gtk_label_new(strsql);

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

 
data.vert_panel=gtk_paned_new(GTK_ORIENTATION_VERTICAL);
gtk_box_pack_start(GTK_BOX(vbox),data.vert_panel,TRUE,TRUE,0);

sprintf(strsql,"%s-PANEL",data.name_window.ravno());
data.name_paned.plus(strsql);
if(iceb_sizwr(data.name_paned.ravno(),&gor,&ver) == 0)
 gtk_paned_set_position(GTK_PANED(data.vert_panel),gor);
else
 gtk_paned_set_position(GTK_PANED(data.vert_panel),100);

/*первое окно*/
data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
//gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_paned_add1(GTK_PANED(data.vert_panel),sw);
//gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


//gtk_box_pack_start (GTK_BOX (vbox),separator1, FALSE, FALSE, 0);

/*второе окно*/

data.view_error=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view_error),FALSE); //Запрет на редактирование текста

data.buffer_error=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view_error));

GtkWidget *sw_error=gtk_scrolled_window_new(NULL,NULL);
//gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw_error),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_paned_add2(GTK_PANED(data.vert_panel),sw_error);
//gtk_box_pack_start (GTK_BOX (vbox), sw_error, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw_error),data.view_error);

data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(admin_makebaz_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);
gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна


g_idle_add((GSourceFunc)admin_makebaz_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  admin_makebaz_r_v_knopka(GtkWidget *widget,class admin_makebaz_r_data *data)
{
 iceb_sizww(data->name_window.ravno(),data->window);
 iceb_sizw(data->name_paned.ravno(),gtk_paned_get_position(GTK_PANED(data->vert_panel)) ,0,data->window);

 gtk_widget_destroy(data->window);
 return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   admin_makebaz_r_key_press(GtkWidget *widget,GdkEventKey *event,class admin_makebaz_r_data *data)
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
/******************************/
/*создание таблицы*/
/***************************/
int admint_makebaz_r_crtb(const char *strsql,int *kol_oh,class admin_makebaz_r_data *data)
{

char bros[512];

if(sql_zap(&bd,strsql) != 0)
 { 
  sprintf(bros,"%s %s\n",gettext("Таблица"),"Plansh");
  iceb_printw(bros,data->buffer_error,data->view_error);

  sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  iceb_printw(bros,data->buffer_error,data->view_error);
  *kol_oh+=1;
  return(1);
 } 
else
 {
  sprintf(bros,"%s\n\n",gettext("Таблица создана"));
  iceb_printw(bros,data->buffer,data->view);
 }
return(0);
}



/******************************************/
/******************************************/

gint admin_makebaz_r1(class admin_makebaz_r_data *data)
{
time(&data->vremn);
char strsql[4096];
char bros[2048];
iceb_u_str repl;
SQL_str row;
class SQLCURSOR cur;
int metka_ver=0;
int kol_oh=0;


iceb_sql_readkey("select VERSION()",&row,&cur,data->window);
if(atof(row[0]) > 4)
  metka_ver=1;  

sprintf(strsql,"%s: %s\n",gettext("Версия базы данных"),row[0]);
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s\n",gettext("Создаём базу данных"));
iceb_printw(strsql,data->buffer,data->view);

if(metka_ver == 0)
  sprintf(strsql,"create database %s",data->imabaz.ravno());
else
 sprintf(strsql,"create database %s DEFAULT CHARACTER SET '%s'",data->imabaz.ravno(),data->kodirovka.ravno());


if(sql_zap(&bd,strsql) != 0)
 {
  //База уже может быть создана
  sprintf(strsql,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
  iceb_printw(strsql,data->buffer,data->view);
 }

sprintf(bros,"%s\n",gettext("Окрываем базу"));
iceb_printw(bros,data->buffer,data->view);


if(sql_openbaz(&bd,data->imabaz.ravno(),data->host.ravno(),"root",data->parol.ravno()) != 0)
 {
  iceb_eropbaz(data->imabaz.ravno(),0,"",1);

  gtk_label_set_text(GTK_LABEL(data->label),gettext("Создание базы аварийно завершено"));

  gtk_widget_grab_focus(data->knopka);
  gtk_widget_show_all(data->window);
  return(FALSE);

 }
iceb_start_rf();
sprintf(bros,"%s\n",gettext("Окно ошибок"));
iceb_printw(bros,data->buffer_error,data->view_error);

/*1*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Plansh\n",gettext("Создание таблицы плана счетов"));
iceb_printw(strsql,data->buffer,data->view);


memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Plansh (\
ns	char(20) not null default '' primary key,\
nais    varchar(255) not null default '',\
tips    smallint not null default 0,\
vids	smallint not null default 0,\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
saldo   smallint not null default 0,\
stat    smallint not null default 0,\
val     smallint not null default 0,\
ksb     smallint not null default 0,\
bs tinyint not null default 0) ENGINE = MYISAM");

/*
 0 ns	Номер счета
 1 nais   Наименование счета
 2 tips  признак синтетического счёта 0-активный 1-пассивный 2-активно-пассивный 3-контрактивный 4-контрпасивный
 3 vids   0-счет 1-субсчет
 4 ktoi  Кто записал
 5 vrem  Время записи
 6 saldo 0-свернутое 3-развернутое
 7 stat  0-балансовый счёт 1-вне балансовый счет
 8 val   0-национальная валюта или номер валюты из справочника валют (пока не задействовано)
 9 ksb   код суббаланса
10 bs    0-счёт можно применять 1-счёт заблокирован
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Glksval\n",gettext("Создание таблицы справочника валют"));
iceb_printw(strsql,data->buffer,data->view);


memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Glksval (\
kod  char(16) not null primary key default '',\
naik varchar(128) not null default '',\
kom  varchar(255) NOT NULL DEFAULT '',\
ktoz smallint unsigned not null default 0,\
vrem int unsigned not null default 0) ENGINE = MYISAM");

/*
0 kod   код валюты
1 naik  наименование валюты
2 kom   коментарий
3 ktoz  кто записал
4 vrem  время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Glkkv\n",gettext("Создание таблицы справочника курсов валют"));
iceb_printw(strsql,data->buffer,data->view);

strcpy(strsql,"CREATE TABLE Glkkv (\
kod  char(16) not null default '',\
datk DATE not null default '0000-00-00',\
kurs double(14,6) NOT NULL DEFAULT 0.,\
kom  varchar(255) NOT NULL DEFAULT '',\
ktoz smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
unique(kod,datk)) ENGINE = MYISAM");

/*
0 kod   код валюты
1 datk  дата 
2 kurs  курс валюты
3 kom   коментарий
4 ktoz  кто записал
5 vrem  время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/



iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Glksubbal\n",gettext("Создание таблицы суббалансов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Glksubbal (\
kod    smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");

/*
0 kod   код cуббаланса
1 naik  наименование суббаланса
2 ktoz  кто записал
3 vrem  время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/




iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Gkont\n",gettext("Создание таблицы групп конрагентов"));
iceb_printw(strsql,data->buffer,data->view);


memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Gkont (\
kod    smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");

/*
0 kod   код группы
1 naik  наименование группы контрагентов
2 ktoz  кто записал
3 vrem  время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Kontragent\n",gettext("Создание таблицы контрагентов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kontragent (\
kodkon char(20) not NULL default '' primary key,\
naikon varchar(255) not null default '',\
naiban varchar(80) not null default '',\
adres  varchar(100) not null default '',\
adresb varchar(100) not null default '',\
kod    varchar(20) not null default '',\
mfo    varchar(20) not null default '',\
nomsh  varchar(30) not null default '',\
innn   varchar(20) not null default '',\
nspnds varchar(20) not null default '',\
telef  varchar(80) not null default '',\
grup   varchar(10) not null default '',\
ktoz   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
na  varchar(255) not null default '',\
regnom varchar(30) not null default '',\
pnaim  varchar(255) not null default '',\
gk varchar(255) not null default '',\
en TINYINT NOT NULL DEFAULT 0,\
unique(naikon),\
index(pnaim),\
index(kod)) ENGINE = MYISAM");

/*
0  kodkon Код контрагента
1  naikon Наименование контрагента
2  naiban Наименование банка
3  adres  Адрес контрагента
4  adresb Адрес банка
5  kod    Код ЕГРПОУ
6  mfo    МФО
7  nomsh  Номер счета
8  innn   Индивидуальный налоговый номер
9  nspnds Номер свидетельства плательщика НДС
10 telef  Телефон
11 grup   Группа
12 ktoz   Кто записал
13 vrem   Время записи
14 na     Налоговый адрес
15 regnom регистрационный номер частного предпринимателя
16 pnaim  полное наименование контрагента
17 gk     город контрагента
18 en     0-общая система 1-единый налог
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

sprintf(strsql,"%s Kontragent1\n",gettext("Создание таблицы дополнительной информации по контрагенту"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kontragent1 (\
kodkon varchar(20) not null default '',\
nomz   smallint unsigned not null default 0,\
zapis  varchar(255) not null default '',\
unique(kodkon,nomz)) ENGINE = MYISAM");

/*
0 kodkon  код контрагента
1 nomz    номер записи
3 zapis   содержимое записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Kontragent2\n",gettext("Создание таблицы списка договоров по контрагенту"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kontragent2 (\
nz int unsigned PRIMARY KEY AUTO_INCREMENT NOT NULL ,\
kodkon varchar(32) not null default '',\
datd DATE NOT NULL DEFAULT '0000-00-00',\
nomd VARCHAR (64) NOT NULL DEFAULT '',\
vidd VARCHAR (255) NOT NULL DEFAULT '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
vnn VARCHAR(16) NOT NULL DEFAULT '',\
pu TINYINT NOT NULL DEFAULT 0,\
index(kodkon)) ENGINE = MYISAM");

/*
0 nz     - уникальный номер записи
1 kodkon - код контрагента
2 datd   - дата документа
3 nomd   - номер документа
4 vidd   - вид документа
5 ktoz   - кто записал
6 vrem   - время записи
7 vnn    - вид налоговой накладной
8 pu      Если равно 1-то это договор по умолчанию
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Skontr\n",gettext("Создание таблицы списка контрагентов для счетов"));
iceb_printw(strsql,data->buffer,data->view);


memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Skontr (\
ns	char(20) not null default '' references Plansh,\
kodkon  char(20) not null default '' references Kontragent,\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
unique(ns,kodkon)) ENGINE = MYISAM");

/*
0 ns	номер счета
1 kodkon  Код контрагента
2 ktoi    кто записал
3 vrem    время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Saldo\n",gettext("Создание таблицы сальдо для счетов"));
iceb_printw(strsql,data->buffer,data->view);


memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Saldo \(\
kkk 	char(2) not null default '',\
gs      smallint not null default 0,\
ns	char(20) not null default '' references Plansh,\
kodkon  char(20) not null default '' references Kontragent,\
deb     double(14,2) not null default 0,\
kre     double(14,2) not null default 0,\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
index(kkk,gs,ns)) ENGINE = MYISAM");
/*
0 kkk     0- счета обычные 1- с развернутым сальдо
1 gs      Год сальдо
2 ns	Номер счета
3 kodkon  Код контрагента
4 deb     Дебет
5 kre     Кредит
6 ktoi    Кто записал
7 vrem    Время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Prov\n",gettext("Создание таблицы проводок"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Prov (\
val smallint not null default 0,\
datp DATE not null default '0000-00-00',\
sh char(20) not null default '',\
shk char(20) not null default '',\
kodkon char(20) not null default '',\
kto char(8) not null default '',\
nomd char(20) not null default '',\
pod int not null default 0,\
oper char(8) not null default '',\
deb double(14,2) not null default 0,\
kre double(14,2) not null default 0,\
ktoi smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
komen varchar(100) not null default '',\
datd DATE not null default '0000-00-00',\
tz tinyint not null default 0,\
kekv smallint not null default 0,\
index(datp,sh,shk),\
index(kto,pod,nomd,datd),\
index(vrem)) ENGINE = MYISAM");

/*
0  val    0-балансовые проводки -1 не балансовые
1  datp   дата проводки
2  sh     счет
3  shk    счёт корреспондент
4  kodkon код контрагента для счетов с развернутым сальдо
5  kto    из какой подсистемы сделана проводка
6  nomd   Номер документа к которому относится проводка
7  pod    подразделение
8  oper   код операции
9  deb    сумма по дебету
10 kre    сумма по кредиту
11 ktoi   номер логина того кто записал проводку
12 vrem   время записи проводки
13 komen  комментарий
14 datd   дата документа к которому относится проводка
15 tz     1-приходный документ 2-расходеый 0-не определено
16 kekv   код экономической классификации расходов
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/




/******************************************/
/*Материальный учёт*/
/****************************************/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Musnrm\n",gettext("Создание таблицы норм расхода материалов"));
iceb_printw(strsql,data->buffer,data->view);


memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Musnrm \
(nz INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,\
kd   INT NOT NULL DEFAULT 0,\
eid  VARCHAR(10) NOT NULL DEFAULT '',\
km    INT NOT NULL DEFAULT 0,\
kol   DOUBLE(20,10) NOT NULL DEFAULT 0,\
eim   VARCHAR(10) NOT NULL DEFAULT '',\
vs    TINYINT NOT NULL DEFAULT 0,\
vz    TINYINT NOT NULL DEFAULT 0,\
kom   VARCHAR(60) NOT NULL DEFAULT '',\
ktoz  SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem  INT UNSIGNED NOT NULL DEFAULT 0,\
INDEX(kd,eid,km)) ENGINE = MYISAM");

/*
 0 nz    уникальный номер записи
 1 kd    код детали
 2 eid   единица измерения детали
 3 km    код материала заготовки
 4 kol   количество материала заготовки на единицу детали
 5 eim   единица измерения материала заготовки
 6 vs    вид списания 0-основной 1-альтернативный
 7 vz    вид заготовки 0-материал 1-покупная заготовка 
 8 kom   комментарий
 9 ktoz  кто записал
10 vrem	 время записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/











iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Grup\n",gettext("Создание таблицы перечня групп материалов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Grup \
(kod    smallint not null default 0 primary key,\
naik    varchar(80) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod   код группы
1 naik  наименование группы
2 ktoz  кто записал
3 vrem  время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Edizmer\n",gettext("Создание таблицы перечня единиц измерения"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Edizmer \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kei char(16) not null default '') ENGINE = MYISAM");
/*
0 kod    Короткое наименоваие единицы измерения
1 naik   Полное наименование единицы измерения
2 ktoz   Кто записал
3 vrem   Время записи
4 kei    Код единиці измерения
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Material\n",gettext("Создание таблицы перечня материалов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Material (\
kodm int not NULL default 0 primary key,\
kodgr smallint not null default 0 references Grup,\
naimat varchar(100) not null default '',\
strihkod varchar(50) not null default '',\
ei char(8) not null default '' references Edizmer,\
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

/*

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
15 ku        код товара согласно УКТ ЗБД
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Prihod\n",gettext("Создание таблицы перечня операций приходов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Prihod \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
vido    smallint not null default 0,\
prov    smallint not null default 0) ENGINE = MYISAM");

/*
0 kod  код операции
1 naik наименование операции
2 ktoz кто записал
3 vrem время записи
4 vido вид операции 0 - внешняя 1-внутренняя 2-изменения учётных данных
5 prov 0 проводки нужно делать 1 - не нужно
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Rashod\n",gettext("Создание таблицы перечня операций расходов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Rashod \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
vido    smallint not null default 0,\
sheta   varchar(100) not null default '',\
prov    smallint not null default 0,\
kk CHAR(16) NOT NULL DEFAULT '00') ENGINE = MYISAM");
/*
0 kod  код операции
1 naik наименование операции
2 ktoz кто записал
3 vrem время записи
4 vido  вид операции 0-внешняя 1-внутренняя 2-изменения учётных данных
5 sheta возможные счета учета для операции
6 prov 0 проводки нужно делать 1 - не нужно
7 kk   код контрагента реквизиты которого нужно брать при распечатке документов
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

/*14*/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Foroplat\n",gettext("Создание таблицы перечня форм оплаты"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Foroplat \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");

/*
0 kod    Код формы оплаты
1 naik   Наименование формы оплаты
2 ktoz   Кто записал
3 vrem   Время записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Sklad\n",gettext("Создание таблицы перечня складов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Sklad \
(kod    smallint not null default 0 primary key,\
naik    varchar(124) not null default '',\
fmol    varchar(255) not null default '',\
dolg    varchar(124) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
mi tinyint not null DEFAULT 0,\
np varchar(255) NOT NULL DEFAULT '') ENGINE = MYISAM");

/*
0 kod     Код склада
1 naik    Наименование склада
2 fmol    Фамилия материальноответственного лица
3 dolg    Должность
4 ktoz 	  кто записал
5 vrem    время записи
6 mi      метка использования склада 0-используется 1-нет
7 np      населённый пункт
*/



admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Roznica\n",gettext("Создание таблицы розничной торговли"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Roznica (\
datrp  DATE not null default '0000-00-00',\
kassa  smallint not null default 0,\
nomer  smallint not null default 0,\
metka  smallint not null default 0,\
kodm   int not null default 0 references Material (kodm),\
ei char(8) not null default '' references Edizmer (kod),\
kolih  double (14,4) not null default 0,\
cena   double (9,2) not null default 0,\
nds    float(2) not null default 0,\
sklad  smallint not null default 0,\
ktoi   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
unique (datrp,kassa,nomer,metka,kodm),\
index (metka,kodm,sklad)\
) ENGINE = MYISAM");
/*
0 datrp  дата реализации
1 kassa  номер рабочего места реализации
2 nomer  Номер продажи
3 metka  1-товар 2- услуга
4 kodm   код товара или материалла
5 ei     единица измерения
6 kolih  количество
7 cena   цена
8 nds    1-цена введена без НДС 0-цена с НДС
9 sklad  отдел
10 ktoi   кто продал
11 vrem   время продажи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Dokummat\n",gettext("Создание таблицы заголовков документов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Dokummat (\
tip    smallint not null default 0,\
datd   DATE not null default '0000-00-00',\
sklad  smallint not null default 0 references Sklad (kod),\
kontr  char(20) not null default '' references Kontragent (kontr),\
nomd   char(16) not null default '',\
nomnn  char(16) not null default '',\
kodop  char(8) not null default '',\
pod    smallint not null default 0,\
pro    smallint not null default 0,\
ktoi   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
nomon  char(16) not null default '',\
datpnn DATE not null default '0000-00-00',\
pn float(5,2) not null default 0,\
mo tinyint NOT NULL default 0,\
k00 char(16) NOT NULL DEFAULT '00',\
unique (datd,sklad,nomd),\
index (nomnn)\
) ENGINE = MYISAM");

/*
0 tip    1-приход 2-расход
1 datd   дата
2 sklad  код склада
3 kontr  код контрагента
4 nomd   номер документа
5 nomnn  номер налоговой накладной
6 kodop  код операции
7 pod    0-не подтверждена 1-подтверждена
8 pro    0-не выполнены проводки 1-выполнены
9 ktoi   кто записал
10 vrem   время записи
11 nomon  Номер парной накладной при внутреннем перемещении или номер документа поставщика для приходного документа
12 datpnn дата получения налоговой накладной
13 pn     процент НДС действовавший на момент создания документа
14 mo     метка оплаты 0-неоплачено 1-оплачено
15 k00    код контрагента реквизиты которого берутся в распечатку
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Dokummat1\n",gettext("Создание таблицы записей к документу"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Dokummat1 (\
datd DATE not null,\
sklad  smallint not null default 0 references Sklad (kod),\
nomd   char(16) not null default '',\
nomkar int not null default 0,\
kodm   int not null default 0 references Material (kodm),\
kolih  double(17,7) not null default 0,\
cena   double(17,7) not null default 0,\
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
mi TINYINT NOT NULL DEFAULT 0,\
kdstv VARCHAR(32) NOT NULL DEFAULT '',\
index (sklad,nomd,kodm,nomkar),\
index (sklad,nomkar),\
index (kodm,sklad),\
index (datd,sklad,nomd)) ENGINE = MYISAM");

/*
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
15 shet   счёт списания для расходного документа
16 dnaim  Дополнительное наименование материалла
17 nomz   Номер заказа
18 mi     метка импортного товара 0-не импорт 1-импорт
19 kdstv  код діяльності сільськогосподарського товаровиробника
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Dokummat2\n",gettext("Создание таблицы дополнительных записей к документу"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Dokummat2 (\
god    smallint not null default 0,\
sklad  smallint not null default 0 references Sklad (kod),\
nomd   char(16) not null default '',\
nomerz smallint not null default 0,\
sodz   varchar(255) not null default '',\
unique (god,nomd,sklad,nomerz)\
) ENGINE = MYISAM");
/*
0 god    
1 sklad
2 nomd
3 nomerz=1 - доверенность#дата выдачи
         2 - через кого
         3 - основание для расходного документа / комментарий для приходного документа
         4 - документ заблокирован для программы xpro если есть эта запись
         5 - login распечатавшего чек
         6 - Сумма НДС для приходного документа введенная вручную
         7 - условие продажи
         8 - код формы оплаты
         9 - дата оплаты или отсрочки платежа
        10 - государственный номер машины
        11 - НДС
             0 20% НДС.
             1 0% НДС реализация на територии Украины.
             2 0% НДС экспорт.
             3 0% Освобождение от НДС статья 5.
        12 - Кто заблокировал документ
        13 - Сумма корректировки по документа +/-
        14 - дата накладной поставщика
        15 - Номер путевого листа
        16 - Марка автомобиля
        17 - Марка и номер прицепа
        18 - Перевозчик груза
        19 - Заказчик (плательщик)
        20 - Фамилия водителя
        21 - Пункт загрузки
        22 - Пункт разгрузки
        23 - Акцизный сбор        
        24 - Лицензия на алкоголь|период действия лиценции для контрагента 00 в формате дд.мм.гг-дд.мм.гг
        25 - Лицензия на алкоголь|период действия лиценции для контрагента получателя в формате дд.мм.гг-дд.мм.гг
        26 - Вид договора|Дата договора|номер договора
4 sodz  Содержимое записи
 */


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/






iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Dokummat3\n",gettext("Создание таблицы услуг для документа"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Dokummat3 (\
datd  DATE not null,\
sklad  smallint not null default 0 references Sklad (kod),\
nomd   char(16) not null default '',\
ei     char(12) not null default '',\
kolih double not null default 0,\
cena double not null default 0,\
nds double not null default 0,\
naimu varchar(100) not null default '',\
ktoi smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
ku int not null,\
nz int unsigned PRIMARY KEY AUTO_INCREMENT NOT NULL,\
index(datd,sklad,nomd)) ENGINE = MYISAM");

/*
 0 datd  дата документа
 1 sklad код склада
 2 nomd  номер документа
 3 ei    единица измерения
 4 kolih количество
 5 cena  цена/сумма
 6 nds   НДС (процент)
 7 naimu наименование услуги
 8 ktoi  кто записал
 9 vrem  время записи
10 ku    код услуги
11 nz    уникальный номер записи устанавливаемый базой данных автоматически
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

sprintf(strsql,"%s Dokummat4\n",gettext("Создание таблицы дополнительных записей к документу"));
iceb_printw(strsql,data->buffer,data->view);


strcpy(strsql,"CREATE TABLE Dokummat4 \
(datd  DATE NOT NULL default '0000-00-00',\
skl    smallint NOT NULL default 0,\
nomd   char(16) NOT NULL default '',\
km     int NOT NULL default 0,\
nk     int NOT NULL default 0,\
kd     int NOT NULL default 0,\
kol    double(16,6) NOT NULL default 0,\
kom    varchar(60) NOT NULL default '',\
ktoz   smallint unsigned NOT NULL default 0,\
vrem   int unsigned NOT NULL default 0,\
nz int unsigned PRIMARY KEY AUTO_INCREMENT NOT NULL,\
index(datd,skl,nomd)) ENGINE = MYISAM");

/*
 0 datd дата документа
 1 skl  код склада
 2 nomd номер документа
 3 km   код материала в документе
 4 nk   номер карточки
 5 kd   код детали на изготовление которой будет использоваться списанный материал
 6 kol  количество материала на указанный код детали (может быть списание на несколько деталей)
 7 kom  коментарий 
 8 ktoz	кто записал
 9 vrem	время записи
10 nz   уникальный номер записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/






iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Opldok\n",gettext("Создание таблицы оплат по документам"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Opldok (\
ps    tinyint not null default 0,\
datd  DATE not null default '0000-00-00',\
pd    int not null default 0,\
nomd  char(20) not null default '',\
dato  DATE NOT NULL default '0000-00-00',\
shet  char(20) not null default '',\
suma  double(10,2) not null default 0,\
kekv  smallint not null default 0,\
nomp  char(20) not null default '',\
kom   varchar(100) not null default '',\
ktoi smallint unsigned not null default 0,\
vrem int unsigned not null,\
index(ps,datd,pd,nomd)) ENGINE = MYISAM");

/*
0 ps    подсистема 1-материальный учёт 2- учёт услуг 3-учёт основных средств
1 datd  дата документа
2 pd    подразделение (для Учёта основных средств pd=0 так как нумерация документов не зависит от подразделения)
3 nomd  номер документа
4 dato  дата оплаты
5 shet  счёт 
6 suma  сумма закрытая на документ
7 kekv  код экономической классификации затрат
8 nomp  номер платёжного поручения
9 kom   комментарий
10 ktoi  кто записал
11 vrem  время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Kart\n",gettext("Создание таблицы карточек материалов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kart (\
sklad  smallint not null default 0 references Sklad (kod),\
nomk    int not null default 0,\
kodm   int not null default 0 references Material (kodm),\
mnds    smallint not null default 0,\
ei char(8) not null default '' references Edizmer (kod),\
shetu   char(12) not null default '' references Plansh (ns),\
cena    double(17,7) not null default 0,\
cenap   double(17,7) not null default 0,\
krat    decimal(10,2) not null default 0,\
nds	float(2) not null default 0,\
fas     float(2) not null default 0,\
kodt    int not null default 0,\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
datv    DATE not null default '0000-00-00',\
innom   varchar(16) not null default '',\
rnd     varchar(20) not null default '',\
nomz    varchar(20) not null default '',\
dat_god DATE NOT NULL default '0000-00-00',\
unique (sklad,nomk),\
index (kodm,sklad,shetu,cena,ei,nds,cenap,krat,mnds),\
index(innom),\
index(nomz)\
) ENGINE = MYISAM");

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
16 rnd      Регистрационный номер документа для мед. препарата
17 nomz     Номер заказа
18 dat_god  Конечная дата годности товара
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zkart\n",gettext("Создание таблицы записей в карточках"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zkart (\
sklad  smallint not null default 0 references Sklad (kod),\
nomk    int not null default 0,\
nomd    char(16) not null default '',\
datdp	DATE not null default '0000-00-00',\
datd    DATE not null default '0000-00-00',\
tipz	smallint not null default 0,\
kolih   double(16,6) not null default 0,\
cena    double(17,7) not null default 0,\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
unique (sklad,nomk,nomd,datdp), \
index (datd,nomd,sklad,nomk),\
index (datdp)\
) ENGINE = MYISAM");

/*
0 sklad	Склад
1 nomk 	Номер карточки
2 nomd 	Номер документа
3 datdp	дата получения-отпуска (дата подтверждения докумена)
4 datd  Дата выписки документа
5 tipz 	1 - приход 2 - расход
6 kolih	Количество
7 cena 	Цена
8 ktoi 	Кто записал
9 vrem 	Время записи
*/



admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

/********************************/
/* Таблицы для зароботной платы */
/*********************************/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Kartb\n",gettext("Создание таблицы карточек работников"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kartb (\
tabn int not null primary key,\
fio  varchar(60) not null default '',\
dolg varchar(60) not null default '',\
zvan smallint not null default 0,\
podr int not null default 0,\
kateg smallint not null default 0,\
datn DATE not null default '0000-00-00',\
datk DATE not null default '0000-00-00',\
shet char(12) not null default '' references Plansh (ns),\
sovm smallint not null default 0,\
inn  char(15) not null default '',\
adres varchar(100) not null default '',\
nomp varchar(30) not null default '',\
vidan varchar(100) not null default '',\
telef varchar(50) not null default '',\
kodg smallint not null default 0,\
harac varchar(50) not null default '',\
lgoti varchar(30) not null default '',\
bankshet varchar(30) not null default '',\
datvd DATE not null default '0000-00-00',\
tipkk varchar(30) not null default '',\
denrog DATE not null default '0000-00-00',\
pl tinyint not null default 0,\
dppz DATE not null default '0000-00-00',\
kb int not null default 0,\
kss varchar(16) not null default '',\
index(fio),\
index(inn)) ENGINE = MYISAM");

/*
0  tabn  табельный номер
1  fio   фамилия имя отчество
2  dolg  должность
3  zvan  звание
4  podr  подразделение
5  kateg категория
6  datn  дата начала работы
7  datk  дата конца работы
8  shet  счёт учета
9  sovm  метка совместителя 0-не совм. 1-совместитель
10 inn   индивидуальный налоговый номер
11 adres адрес
12 nomp  номер паспорта
13 vidan кем выдан
14 telef телефон
15 kodg  код города налоговой инспекции
16 harac характер работы
17 lgoti код льгот для формы 8ДР
18 bankshet банковский счёт на который перечисляется зарплата
19 datvd дата выдачи документа
20 tipkk тип кредитной карты
21 denrog день рождения
22 pl     0-мужчина 1-женщина
23 dppz   дата последнего повышения зарплаты
24 kb    код банка для перечисления зарплаты на карт счёт
25 kss   Код основания для учёта спец.стажа
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Kartb1\n",gettext("Создание таблицы дополнительной информации по работнику"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kartb1 (\
tabn int not null default 0,\
nomz   smallint unsigned not null default 0,\
zapis  varchar(255) not null default '',\
unique(tabn,nomz)) ENGINE = MYISAM");

/*
0 tabn табельный номер
1 nomz    номер записи
2 zapis   содержимое записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zarsdo\n",gettext("Создание таблицы должностных окладов почасовых ставок и тарифных разрядов"));
iceb_printw(strsql,data->buffer,data->view);


memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarsdo \
(tn INT NOT NULL DEFAULT 0,\
dt DATE NOT NULL DEFAULT '0000-00-00',\
kn INT NOT NULL DEFAULT 0,\
sm DOUBLE(12,2) NOT NULL DEFAULT 0.,\
mt SMALLINT NOT NULL DEFAULT 0,\
kh FLOAT(6,2) NOT NULL DEFAULT 0.,\
ks FLOAT(6,2) NOT NULL DEFAULT 1.,\
sh CHAR(32) NOT NULL DEFAULT '',\
km CHAR(100) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
unique(tn,dt,kn)) ENGINE = MYISAM");

/*
0  tn   табельный номер
1  dt   дата - день всегда 1
2  kn   код начисления
3  sm   сумма 
4  mt   метка суммы
        0 - оплата за месяц расчётная по отработанным дням
        1 - оплата за месяц расчётная по отработанным часам
        2 - оплата за месяц постоянная
        3 - оплата за час
        4 - оплата за день
        5 - оплата по тарифу (минимальная зарплата умноженная на тарифный коэффициент)
5  kh   количество часов в рабочем дне
6  ks   коэффициент ставки по умолчанию 1.
7  sh   бухгалтерский счёт 
8  km   коментарий
9  ktoz	Кто записал
10 vrem	Время записи

*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zarpr\n",gettext("Создание таблицы протоколов хода расчёта зарплаты"));
iceb_printw(strsql,data->buffer,data->view);

strcpy(strsql,"CREATE TABLE Zarpr \
(tn INT NOT NULL DEFAULT 0,\
tz TINYINT NOT NULL DEFAULT 0,\
dr DATE NOT NULL DEFAULT '0000-00-00',\
nz INT NOT NULL DEFAULT 0,\
sz TEXT NOT NULL DEFAULT '',\
unique(tn,tz,dr,nz)) ENGINE = MYISAM");
/*
0 tn   табельный номер
1 tz  1-протокол расчёта начислений 2-удержаний 3-начислений на фонд зарплаты
2 dr  Дата расчёта год и месяц день всегда 1
3 nz   номер записи
4 sd  содержимое записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/



iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zarlgot\n",gettext("Создание таблицы льгот"));
iceb_printw(strsql,data->buffer,data->view);

strcpy(strsql,"CREATE TABLE Zarlgot \
(tn INT NOT NULL DEFAULT 0,\
dndl DATE NOT NULL DEFAULT '0000-00-00',\
pl FLOAT(7,2) NOT NULL DEFAULT 0.,\
koll INT NOT NULL DEFAULT 0,\
kodl CHAR(16) NOT NULL DEFAULT '',\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
unique(tn,dndl)) ENGINE = MYISAM");
/*
0 tn   табельный номер
1 dndl Дата начала действия льготы
2 pl   процент льготы
3 koll количество льгот
4 kodl код льготы
5 kom  коментарий
6 ktoz Кто записал
7 vrem Время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zarkh\n",gettext("Создание таблицы списка картсчетов работников"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarkh (\
tn int not null default 0,\
kb  int not null default 0,\
nks varchar(40) not null default '',\
ktoz smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
unique(tn,kb)) ENGINE = MYISAM");
/****
0 tn - табельный номер
1 kb - код банка
2 nks - номер карт-счёта
3 ktoz кто записал
4 vrem время записи
****/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zarrud\n",gettext("Создание таблицы справочника регулярных удержаний с работников"));
iceb_printw(strsql,data->buffer,data->view);

strcpy(strsql,"CREATE TABLE Zarrud \
(tn INT NOT NULL DEFAULT 0 primary key,\
ku int  NOT NULL DEFAULT 0,\
sp DOUBLE(10,2) NOT NULL DEFAULT 0.,\
mu TINYINT NOT NULL DEFAULT 0,\
dd DATE NOT NULL DEFAULT '0000-00-00',\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 tn  табельный номер
1 ku  код удержания
2 sp  сумма/процент
3 mu  метка 0-сумма 1-процент
4 sd  дата действия
5 kom коментарий
6 ktoz   Кто записал
7 vrem   Время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
iceb_printw("Создание таблицы справочника количества рабочих дней и часов Zarskrdh !",data->buffer,data->view);


strcpy(strsql,"CREATE TABLE Zarskrdh \
(data DATE NOT NULL DEFAULT '0000-00-00' primary key,\
krd FLOAT NOT NULL DEFAULT 0.,\
krh FLOAT NOT NULL DEFAULT 0.,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kpd int unsigned not null default 0) ENGINE = MYISAM");
/*
0 data   месяц и god день всегда равен 1
1 krd    количество рабочих дней
2 krh    количество рабочих часов
3 ktoz   Кто записал
4 vrem   Время записи
5 kpd    Количество праздничных дней (применяется для расчёта отпускных
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zarsvd\n",gettext("Создание таблицы справочника выходных дней"));
iceb_printw(strsql,data->buffer,data->view);


strcpy(strsql,"CREATE TABLE Zarsvd \
(data DATE NOT NULL DEFAULT '0000-00-00' primary key,\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 data   дата выходного дня
1 kom    коментарий
2 ktoz   Кто записал
3 vrem   Время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s %s\n",gettext("Создание таблицы справочника минимальной зарплаты и прожиточного минимума"),"Zarmzpm");
iceb_printw(strsql,data->buffer,data->view);

strcpy(strsql,"CREATE TABLE Zarmzpm \
(data DATE NOT NULL DEFAULT '0000-00-00' primary key,\
mz FLOAT NOT NULL DEFAULT 0.,\
pm FLOAT NOT NULL DEFAULT 0.,\
ms FLOAT NOT NULL DEFAULT 0.,\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0) ENGINE = MYISAM");
/*
0 data   дата
1 mz     минимальная зарплата
2 pm     прожиточный минимум
3 ms     максимальная сумма с которой берутся соц-отчисления
4 ktoz   Кто записал
5 vrem   Время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Nash\n",gettext("Создание таблицы начислений"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Nash (\
kod	smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
shet    char(12)  not null default '' references Plansh (ns),\
kodt    smallint not null default 0,\
vidn    char(2) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
prov	smallint not null default 0,\
fz      smallint not null default 0) ENGINE = MYISAM");

/*
0 kod	код начисления
1 naik    наименование начисления
2 shet    счёт на который ложится начисление
3 kodt    код табеля к которому привязано начисление
4 vidn    0-основная 1-дополнительная 2-прочая    
5 ktoz    кто записал
6 vrem    время записи
7 prov    0-делать проводки 1-не делать
8 fz      0-входит в фонд зарплаты 1-не входит
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

sprintf(strsql,"%s Uder\n",gettext("Создание таблицы удержаний"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uder (\
kod	smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
shet    varchar(12) not null default '' references Plansh (ns),\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
prov	smallint not null default 0,\
shetb   varchar(32) not null default '') ENGINE = MYISAM");
/*
0 kod	код удержание
1 naik  наименование удержания
2 shet  хозрасчётный
3 ktoz  кто записал
4 vrem  время записи
5 prov    0-делать проводки 1-не делать
6 shetb бюджетный счет
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zargrupp\n",gettext("Создание таблицы групп подразделений"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zargrupp \
(kod   char(10) not null default '' primary key,\
naik   varchar(100) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");

/*
0 kod    код группы
1 naik   наименование группы
2 ktoz	кто записал
3 vrem	время записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

sprintf(strsql,"%s Zargr\n",gettext("Создание таблиц групп работников"));
iceb_printw(strsql,data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zargr \
(kg INT NOT NULL DEFAULT 0,\
tn INT NOT NULL DEFAULT 0,\
dnd DATE NOT NULL DEFAULT '0000-00-00',\
dkd DATE NOT NULL DEFAULT '0000-00-00',\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz SMALLINT UNSIGNED NOT NULL DEFAULT 0,\
vrem INT UNSIGNED NOT NULL DEFAULT 0,\
unique(kg,tn,dnd)) ENGINE = MYISAM");
/*
0 kg   код группы работкика 1-пенсионер 2-инвалид 3-работающий по договору подряда
1 tn   табельный номер
2 dnd  Дата начала действия
3 dkd  дата конца действия
4 kom  коментарий
5 ktoz Кто записал
6 vrem Время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zarsb\n",gettext("Создание таблицы списка банков"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarsb \
(kod    int not null default 0 primary key,\
naik    varchar(80) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod   код банка
1 naik  наименование банка
2 ktoz  кто записал
3 vrem  время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Podr\n",gettext("Создание таблицы подразделений"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Podr (\
kod	smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
grup    varchar(10) not null default '') ENGINE = MYISAM");
/*
0 kod	код подразделения
1 naik    наименование подразделения
2 ktoz  Кто записал
3 vrem  Время записи
4 grup  код группы
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Kateg\n",gettext("Создание таблицы категорий работников"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kateg (\
kod	smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod	код категории
1 naik    наименование категории
2 ktoz  Кто записал
3 vrem  Время записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zvan\n",gettext("Создание таблицы званий работников"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zvan (\
kod	smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
sdzz DOUBLE(12,2) NOT NULL DEFAULT 0.) ENGINE = MYISAM");
/*
0 kod	код звания
1 naik    наименование звания
2 ktoz  Кто записал
3 vrem  Время записи
4 sdzz  сумма доплаты за звание
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Gnali\n",gettext("Создание таблицы городов налоговых инспекций"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Gnali (\
kod	smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
kontr   char(20) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod	код города
1 naik    наименование города
2 kontr   код контрагента налоговой инспекции
3 ktoz  кто записал
4 vrem  время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Tabel\n",gettext("Создание таблицы видов табелей"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Tabel (\
kod	smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod	код табеля
1 naik    наименование табеля
2 ktoz  Кто записал
3 vrem  Время записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zarsoc\n",gettext("Создание таблицы видов социальных отчислений"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarsoc (\
kod    smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
shet	varchar(30) not null default '',\
proc    float(6,2) not null default 0,\
kodn    varchar(255) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod   код фонда
1 naik  наименование фонда
2 shet  счет
3 proc  процент отчисления
4 kodn  коды начислений не входящие в расчёт этого отчисления
5 ktoz  кто записал
6 vrem  время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zarsocz\n",gettext("Создание таблицы социальных отчислений по работникам"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarsocz (\
datz    DATE not null default '0000-00-00',\
tabn    int unsigned not null default 0,\
kodz    smallint not null default 0,\
shet	varchar(30) not null default '',\
sumas   double(8,2) not null default 0,\
sumap   double(8,2) not null default 0,\
proc    float(6,2) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
sumapb  double(8,2) not null default 0,\
sumasb  double(8,2) not null default 0, \
nz int unsigned PRIMARY KEY AUTO_INCREMENT NOT NULL,\
index(datz,tabn,kodz)) ENGINE = MYISAM");

/*
0  datz дата записи, день всегда равен 1
1  tabn табельный номер
2  kodz код социального отчисления
3  shet счёт социального отчисления хозрасчётный через запятую бюджетный (если он есть)
4  sumas сумма с которой расчитано отчисление (общая вместе с бюжетой)
5  sumap сумма отчисления (общая вместе с бюджетной)
6  proc  процент отчисления
7  ktoz  кто записал
8  vrem  время записи
9  sumapb сумма только с бюджетных счетов
10 sumasb бюджетная сумма с которой расчитано отчисление
11 nz  уникальный номер записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zsal\n",gettext("Создание таблицы сальдо по зарплате"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zsal (\
god	smallint not null default 0,\
tabn    int not null default 0,\
saldo   double(12,2) not null default 0,\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
saldob  double(12,2) not null default 0,\
unique(god,tabn)) ENGINE = MYISAM");
/*
0 god	год
1 tabn	табельный номер
2 saldo   значение сальдо
3 ktoz  кто записал
4 vrem  время записи
5 saldob  сальдо по бюджетным счетам
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Ztab\n",gettext("Создание таблицы записей отработанного времени"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ztab (\
god	smallint not null default 0,\
mes     smallint not null default 0,\
tabn    int not null default 0,\
kodt    smallint not null default 0,\
dnei    float not null default 0,\
has     float not null default 0,\
kdnei   float not null default 0,\
datn    DATE not null default '0000-00-00',\
datk    DATE not null default '0000-00-00',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
kolrd float(5,2) not null default 0,\
kolrh float(5,2) not null default 0,\
kom  varchar(40) not null default '',\
nomz smallint not null default 0,\
unique(tabn,god,mes,kodt,nomz),\
index(god,mes,tabn)) ENGINE = MYISAM");

/*
0  god	год
1  mes     месяц
2  tabn	табельный номер
3  kodt    код табеля
4  dnei    отработано дней
5  has     отработано часов
6  kdnei   отработано календарных дней
7  datn    дата начала
8  datk    дата конца
9  ktoz    кто записал
10 vrem    время модификации
11 kolrd   количество рабочих дней в месяце по данному табелю для конкретного работника
12 kolrh   скольки часовой рабочий день
13 kom     комментарий
14 nomz    номер записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zarp\n",gettext("Создание таблицы записей начилений/удержаний по работникам"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarp (\
datz	DATE not null default '0000-00-00',\
tabn    int not null default 0,\
prn     char(2) not null default '',\
knah    smallint not null default 0,\
suma    double (14,2) not null default 0,\
shet    varchar(12) not null default '' references Plansh (ns),\
godn    smallint not null default 0,\
mesn    smallint not null default 0,\
kdn     smallint not null default 0,\
nomz    smallint not null default 0,\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
kom	varchar(40) not null default '',\
podr	int not null default 0,\
nd      VARCHAR(20) NOT NULL default '',\
unique (datz,tabn,prn,knah,godn,mesn,podr,shet,nomz),\
index (tabn,datz),\
index tabn1 (tabn,godn,mesn),\
index (nd,datz)) ENGINE = MYISAM");

/*
0  datz    дата начисления/удержания
1  tabn	табельный номер
2  prn	1- начисление 2 удержание
3  knah	код начисления
4  suma	сума начисления/удержания
5  shet    счет
6  godn    год в счёт которого начисления/удержания
7  mesn    месяц в счёт которого начисления/удержания
8  kdn     количество дней для больничного
9  nomz    номер записи
10 ktoz    кто записал
11 vrem    дата записи
12 kom     комментарий
13 podr    код подразделения
14 nd      номер документа
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zarn\n",gettext("Создание таблицы записей текущей настройки карточки"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Zarn (\
god  smallint not null default 0,\
mes  smallint not null default 0,\
podr int not null default 0,\
tabn int not null default 0,\
kateg smallint not null default 0,\
sovm smallint not null default 0,\
zvan smallint not null default 0,\
shet varchar(20) not null default '' references Plansh (ns),\
lgot varchar(60) not null default '',\
prov smallint not null default 0,\
datn DATE not null default '0000-00-00',\
datk DATE not null default '0000-00-00',\
dolg varchar(60) not null default '',\
blok smallint not null default 0,\
unique(tabn,god,mes),\
index(god,mes,podr)) ENGINE = MYISAM");

/*
0  god	   год
1  mes	   месяц
2  podr    подразделение
3  kateg   категория
4  tabn	   табельный номер
5  sovm	   0-нет 1-совместитель
6  zvan	   код звания
7  shet	   счёт 
8  lgot	   льготы
9  prov    0-проводки не сделаны 1-сделаны
10 datn    Дата приема на работу
11 datk    Дата увольнения с работы
12 dolg    Должность
13 blok    0- не заблокирована или логин заблокировавшего карточку
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zarn1\n",gettext("Создание таблицы записей текущей настройки начислений/удержаний"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zarn1 (\
tabn	int not null default 0,\
prn	smallint not null default 0,\
knah    smallint not null default 0,\
datd    DATE not null default '0000-00-00',\
shet	varchar(20) not null default '' references Plansh (ns),\
unique(tabn,prn,knah)) ENGINE = MYISAM");
/*
0 tabn	табельный номер
1 prn	1-начисление 2 удержание
2 knah    код начисления/удержания
3 datd    дата до которой действует начисление/удержание
4 shet	счет
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


/*Создаем таблицы для формы 1дф*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s F8dr\n",gettext("Создание таблицы заголовка документа формы 1дф"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE F8dr (\
god    smallint not null default 0,\
kvrt   smallint not null default 0,\
nomd   varchar(20) not null default '' primary key,\
vidd   smallint not null default 0,\
nomdn  varchar(20) not null default '',\
datad  DATE not null default '0000-00-00',\
datan  DATE not null default '0000-00-00',\
metbl  smallint not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kr INT NOT NULL DEFAULT 0,\
ks INT NOT NULL DEFAULT 0) ENGINE = MYISAM");

/*
0 god    год
1 kvrt   квартал
2 nomd   номер документа
3 vidd   вид документа 0-отчётный 1-новый отчётный 2-уточняющий
4 nomdn  номер под которым документ зарегистрирован в налоговой
5 datad  дата подачи документа
6 datan  дата когда документ зарегистрирован в налоговой
7 metbl  0-документ не заблокирован или логин того кто заблокировал
8 ktoz  кто записал
9 vrem  время записи
10 kr   количество работающих
11 ks   количество совместителей
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s F8dr1\n",gettext("Создание таблицы записей в документе формы 1дф"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE F8dr1 (\
nomd   varchar(20) not null default '' references F8dr,\
inn    char(10)  not null default '',\
sumad  double(10,2) not null default 0,\
suman  double(8,2) not null default 0,\
priz   char(4) not null default '',\
datap  DATE not null default '0000-00-00',\
datau  DATE not null default '0000-00-00',\
lgota  char(4) not null default '',\
ktoz   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
fio    varchar(60) not null default '',\
sumadn  double(10,2) not null default 0,\
sumann  double(8,2) not null default 0,\
pr     smallint not null default 0,\
unique(nomd,inn,priz,lgota,pr)) ENGINE = MYISAM");

/*
0 nomd   номер документа
1 inn    индивидуальный налоговый номер
2 sumad  сума выплаченного дохода
3 suman  суммы выплаченного подоходного налога
4 priz   признак дохода
5 datap  дата приема на работу
6 datau  дата увольнения
7 lgota  признак льготы по подоходному налогу
8 ktoz  кто записал
9 vrem  время записи
10 fio фамилия для произвольного ввода записи
11 sumadn  сума начисленного дохода
12 sumann  суммы начисленного подоходного налога
13 pr    0-новая запись 1-удалить запись
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

sprintf(strsql,"%s F1dfvs\n",gettext("Создание таблицы записей военного сбора формы 1дф"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE F1dfvs (\
nomd   varchar(20) not null default '',\
inn    char(10)  not null default '',\
fio    varchar(60) not null default '',\
sfr  double(10,2) not null default 0,\
svs  double(8,2) not null default 0,\
sfrv  double(10,2) not null default 0,\
svsv  double(8,2) not null default 0,\
ktoz   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
unique(nomd,inn)) ENGINE = MYISAM");

/*
0 nomd   номер документа
1 inn    индивидуальный налоговый номер
2 fio    фамилия для произвольного ввода записи
3 sfr    сумма с которой был сделан расчёт
4 svs    сумма военного сбора
5 sfrv   сумма с коророй был сделан расчёт выплаченная
6 svsv   сумма военного сбора выплаченная
7 ktoz  кто записал
8 vrem  время записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/







iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zardok\n",gettext("Создание таблицы заголовков нарядов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zardok (\
god smallint NOT NULL default 0,\
datd DATE not null default '0000-00-00',\
nomd varchar(20) not null default '',\
pd int not null default 0,\
koment varchar(100) not null default '',\
ktoi smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
td tinyint NOT NULL default 0,\
datsd DATE not null default '0000-00-00',\
unique(nomd,god),\
index(datd,nomd)) ENGINE = MYISAM");

/************
0  god     год документа
1  datd    дата начисления в карточку
2  nomd      номер документа
3  pd      код подразделения
4  koment  комментарий
5  ktoi    Кто записал
6  vrem    Время записи
7  td      1-приходы 2-расходы
8  datsd   дата дата создания документа
***************/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/



/***********************************************************/
/*Таблицы для программы работы с бухгалтерскими документами*/
/************************************************************/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Pltp\n",gettext("Создание таблицы заголовков платежных поручений"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Pltp (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
oper    varchar(12) not null default '',\
suma	double (14,2) not null default 0,\
nds     double not null default 0,\
uslb    double not null default 0,\
shet    varchar(30) not null default '',\
plat	varchar(255) not null default '',\
polu    varchar(255) not null default '',\
podt    char(2) not null default '',\
prov    char(2) not null default '',\
vidpl   char(2) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
datv    DATE not null default '0000-00-00',\
kodnr   varchar(10) not null default '',\
unique(datd,nomd)) ENGINE = MYISAM");
/*
0  datd	дата документа
1  nomd    номер документа
2  oper    код операции
3  suma	сумма платежа
4  nds     НДС
5  uslb    сумма за услуги банка
6  shet    счёт на который платят за услуги банка
7  plat	реквизиты плательщика
8  polu    реквизиты получателя
9  podt    0-не подтверждена 1- подтверждена
10 prov    0-выполнены проводки 1-нет
11 vidpl   вид платежа О,В,К  - в настоящий момент не используется
12 ktoz   кто записал
13 vrem   время записи
14 datv   Дата валютирования
15 kodnr  Код не резидента
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Pltpz\n",gettext("Создание таблицы записей к платежному поручению"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Pltpz (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
mz      smallint not null default 0,\
nz      smallint not null default 0,\
zapis   varchar(255) not null default '',\
index(datd,nomd)) ENGINE = MYISAM");

/*
0 datd	дата документа
1 nomd    номер документа
2 mz      метка записи
          0-текст назначения платежа
3 nz      номер записи
4 zapis   содержимое записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Pltt\n",gettext("Создание таблицы заголовков платежных требований"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Pltt (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
oper    varchar(12) not null default '',\
suma	double (14,2) not null default 0,\
nds     double not null default 0,\
uslb    double not null default 0,\
shet    varchar(30) not null default '',\
plat	varchar(255) not null default '',\
polu    varchar(255) not null default '',\
podt    char(2) not null default '',\
prov    char(2) not null default '',\
vidpl   char(2) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
datv    DATE not null default '0000-00-00',\
kodnr   varchar(10) not null default '',\
index(datd,nomd)) ENGINE = MYISAM");
/*
0  datd	дата документа
1  nomd    номер документа
2  oper    код операции
3  suma	сумма платежа
4  nds     НДС
5  uslb    сумма за услуги банка
6  shet    счёт на который платят за услуги банка
7  plat	реквизиты плательщика
8  polu    реквизиты получателя
9  podt    0-не подтверждена 1- подтверждена
10 prov    0-выполнены проводки 1-нет
11 vidpl   вид платежа (здесь пока не применяется)
12 ktoz   кто записал
13 vrem   время записи
14 datv   Дата валютирования
15 kodnr  Код не резидента
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Plttz\n",gettext("Создание таблицы записей к платежному требованию"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Plttz (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
mz      smallint not null default 0,\
nz      smallint not null default 0,\
zapis   varchar(255) not null default '',\
index(datd,nomd)) ENGINE = MYISAM");

/*
0 datd	дата документа
1 nomd    номер документа
2 mz      метка записи
          0-текст назначения платежа
3 nz      номер записи
4 zapis   содержимое записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Tpltt\n",gettext("Создание таблицы заголовков платёжного требования"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Tpltt (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
oper    varchar(12) not null default '',\
suma	double (14,2) not null,\
nds     double not null,\
uslb    double not null,\
shet    varchar(30) not null default '',\
plat	varchar(255) not null default '',\
polu    varchar(255) not null default '',\
podt    char(2) not null default '',\
prov    char(2) not null default '',\
vidpl   char(2) not null default '',\
ktoz    smallint unsigned not null,\
vrem    int unsigned not null,\
datv    DATE not null default '0000-00-00',\
kodnr   varchar(10) not null default '',\
unique(datd,nomd)) ENGINE = MYISAM");
/*
0  datd	дата документа
1  nomd    номер документа
2  oper    код операции
3  suma	сумма платежа
4  nds     НДС
5  uslb    сумма за услуги банка
6  shet    счёт на который платят за услуги банка
7  plat	реквизиты плательщика
8  polu    реквизиты получателя
9  podt    0-не подтверждена 1- подтверждена
10 prov    0-выполнены проводки 1-нет
11 vidpl   здесть не используется
12 ktoz   кто записал
13 vrem   время записи
14 datv   Дата валютирования
15 kodnr  Код не резидента
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Tplttz\n",gettext("Создание таблицы записей к типовому платежному требованию"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Tplttz (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
mz      smallint not null default 0,\
nz      smallint not null default 0,\
zapis   varchar(255) not null default '',\
index(datd,nomd)) ENGINE = MYISAM");

/*
0 datd	дата документа
1 nomd    номер документа
3 mz      метка записи
        0-текст назначения платежа
4 nz      номер записи
5 zapis   содержимое записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Tpltp\n",gettext("Создание таблицы заголовков типовых платежных поручений"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Tpltp (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
oper    varchar(12) not null default '',\
suma	double (14,2) not null default 0,\
nds     double not null default 0,\
uslb    double not null default 0,\
shet    varchar(30) not null default '',\
plat	varchar(255) not null default '',\
polu    varchar(255) not null default '',\
podt    char(2) not null default '',\
prov    char(2) not null default '',\
vidpl   char(2) not null default '',\
ktoz    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
datv    DATE not null default '0000-00-00',\
kodnr   varchar(10) not null default '',\
unique(datd,nomd)) ENGINE = MYISAM");
/*
0  datd	дата документа
1  nomd    номер документа
2  oper    код операции
3  suma	сумма платежа
4  nds     НДС
5  uslb    сумма за услуги банка
6  shet    счёт на который платят за услуги банка
7  plat	реквизиты плательщика
8  polu    реквизиты получателя
9  podt    0-не подтверждена 1- подтверждена
10 prov    0-выполнены проводки 1-нет
11 vidpl   вид платежа О,В,К - в настоящий момент не используется
12 ktoz   кто записал
13 vrem   время записи
14 datv   Дата валютирования
15 kodnr  Код не резидента
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Tpltpz\n",gettext("Создание таблицы записей к типовому платежному поручению"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Tpltpz (\
datd	DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
mz      smallint not null default 0,\
nz      smallint not null default 0,\
zapis   varchar(255) not null default '',\
index(datd,nomd)) ENGINE = MYISAM");

/*
0 datd	дата документа
1 nomd    номер документа
3 mz      метка записи
        0-текст назначения платежа
4 nz      номер записи
5 zapis   содержимое записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Opltp\n",gettext("Создание таблицы перечня операций платежных поручений"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Opltp \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod    Код операции платежного поручения
1 naik   Наименование операции платежного поручения
2 ktoz   Кто записал
3 vrem   Время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Opltt\n",gettext("Создание таблицы перечня операций платежных требований"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Opltt \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod    Код операции платежного требования
1 naik   Наименование операции платежного требования
2 ktoz   Кто записал
3 vrem   Время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Obqvka\n",gettext("Создание таблицы объявок cдачи налички в банк"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Obqvka (\
datd	DATE not null default '0000-00-00',\
nomd	varchar(20) not null default '',\
suma    double (14,2) not null default 0,\
kodop   varchar(8) not null default '',\
fio	varchar(40) not null default '',\
prov    char(2) not null default '',\
podt    char(2) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(datd,nomd)) ENGINE = MYISAM");

/*
0 datd	дата документа
1 nomd	номер документа
2 suma    сумма документа
3 kodop   код операции
4 fio	фамилия сдающего деньги
5 prov    0-проводки не выполнены 1-выполнены
6 podt    0-не подтвержден 1-подтвержден
7 ktoz  
8 vrem
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Oznvb\n",gettext("Создание таблицы перечня операций cдачи налички в банк"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Oznvb \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod    Код операции
1 naik   Наименование операции
2 ktoz   Кто записал
3 vrem   Время записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Ktnp\n",gettext("Создание таблицы перечня типовых платежек"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Ktnp \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod    Код типового платежного поручения
1 naik   Наименование типового платежного поручения
2 ktoz   Кто записал
3 vrem   Время записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Ktnt\n",gettext("Создание таблицы перечня типовых требований"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Ktnt \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod    Код типового платежного требования
1 naik   Наименование типового платежного требования
2 ktoz   Кто записал
3 vrem   Время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Ksn\n",gettext("Создание таблицы перечня сдающих в банк наличку"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Ksn \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod    Код сдающего наличку в банк
1 naik   Фамилия сдающего наличку в банк
2 ktoz   Кто записал
3 vrem   Время записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Pzpd\n",gettext("Создание таблицы подтверждающих записей документов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Pzpd (\
tp	smallint not null default 0,\
datd    DATE not null default '0000-00-00',\
datp	DATE not null default '0000-00-00',\
nomd	varchar(20) not null default '',\
suma	double (14,2) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
index(tp,nomd)) ENGINE = MYISAM");

/*
0 tp	0-платежкка 1-требование
1 datd    дата документа
2 datp	дата подтверждения
3 nomd	номер документа
4 suma	сумма подтверждения
5 ktoz  кто записал
6 vrem  время записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


/************************************************/
/***********Учет основных средств****************/
/************************************************/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uospri\n",gettext("Создание таблицы перечня операций приходов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uospri \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
vido    smallint not null default 0,\
prov    smallint not null default 0) ENGINE = MYISAM");

/*
0 kod  код операции
1 naik наименование операции
2 ktoz Кто записал
3 vrem время записи
4 vido 0 - внешняя 1-внутренняя 2-изменения стоимости
5 prov 0 проводки нужно делать 1 - не нужно
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uosras\n",gettext("Создание таблицы перечня операций расходов"));
iceb_printw(strsql,data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosras \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
vido    smallint not null default 0,\
prov    smallint not null default 0,\
kk CHAR(16) NOT NULL DEFAULT '00') ENGINE = MYISAM");
/*
0 kod  код операции
1 naik наименование операции
2 ktoz Кто записал
3 vrem время записи
4 vido 0 - внешняя 1-внутренняя 2-изменения стоимости
5 prov 0 проводки нужно делать 1 - не нужно
6 kk   код контрагента реквизиты которого нужно брать в расходные документы
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uosls\n",gettext("Создание таблицы ликвидационной стоимости"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosls (\
inn INT NOT NULL DEFAULT 0,\
data DATE NOT NULL DEFAULT '0000-00-00',\
lsnu DOUBLE NOT NULL DEFAULT 0.,\
lsbu DOUBLE NOT NULL DEFAULT 0.,\
kom VARCHAR(255) NOT NULL DEFAULT '',\
ktoz smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
index(inn,data)) ENGINE = MYISAM");

/*
0 inn    инвентарный номер
1 data  дата записи
2 lsnu  ликвидационная цена для налогового учёта
3 lsbu  ликвидационная цена для бух. учёта
4 kom   коментарий
5 ktoz  кто записал
6 vrem  время записи
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uospod\n",gettext("Создание таблицы перечня подразделений"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uospod \
(kod    smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
mi tinyint not null) ENGINE = MYISAM");
/*
0 kod    Код подразделения
1 naik   Наименование подразделения
2 ktoz   Кто записал
3 vrem   Время записи
4 mi     0-используется 1-неиспользуется (уволен и остатки по нему нулевые)
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uosdm\n",gettext("Создание таблицы перечня драг-металлов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosdm \
(kod    smallint not null default 0 primary key,\
naik    varchar(100) not null default '',\
ei      varchar(20) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod    Код драгоценного металла
1 naik   Наименование металла
2 ei     Единица измерения
3 ktoz   Кто записал
4 vrem   Время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uosindm\n",gettext("Создание таблицы содержимого драг-металлов в инвентарном номере"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosindm \
(innom int not null,\
kod    smallint not null default 0,\
ei     varchar(20) not null default '',\
ves    double(13,8) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(innom,kod)) ENGINE = MYISAM");
/*
0 innom    инвентарный номер
1 kod    Код драгоценного металла
2 ei     Единица измерения
3 ves    вес материалла
4 ktoz   Кто записал
5 vrem   Время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Uosgrup\n",gettext("Создание таблицы перечня груп для налогового учёта"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosgrup \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
kof     float not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
ta tinyint not null,\
ar tinyint not null) ENGINE = MYISAM");
/*
0 kod    код
1 naik   наименование
2 kof    годовой процент амортизационного отчисления
3 ktoz   Кто записал
4 vrem   Время записи
5 ta     Тип амортизации 0-по объектная 1-в целом по группе
6 ar     алготитм расчёта 0-от начальной балансовой стоимости 1-от остаточной балансовой стоимости
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Uosgrup1\n",gettext("Создание таблицы перечня груп для бухгалтерского учёта"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosgrup1 \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
kof     float not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
ar tinyint not null) ENGINE = MYISAM");

/*
0 kod    код
1 naik   наименование
2 kof    коэффициент нормы амортотчисления
3 ktoz
4 vrem
5 ar     алготитм расчёта 0-от начальной балансовой стоимости 1-от остаточной балансовой стоимости
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Uoshau\n",gettext("Создание таблицы шифров аналитического учета"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uoshau \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod    Код шифра аналитического учета
1 naik   Наименование шифра аналитического учета
2 ktoz   Кто записал
3 vrem   Время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Uoshz\n",gettext("Создание таблицы шифров затрат"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uoshz \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod    Код шифра затрат
1 naik   Наименование шифра затрат
2 ktoz   Кто записал
3 vrem   Время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Uosol\n",gettext("Создание таблицы ответственных"));
iceb_printw(strsql,data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosol \
(kod    int not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
mi tinyint not null) ENGINE = MYISAM");
/*
0 kod    Код ответственного лица
1 naik   Фамилия ответственного лица
2 ktoz   Кто записал
3 vrem   Время записи
4 mi     0-используется 1-неиспользуется (уволен и остатки по нему нулевые)
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Uosin\n",gettext("Создание таблицы инвентарных номеров"));
iceb_printw(strsql,data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

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
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
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

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Uosin1\n",gettext("Создание таблицы дополнительной информации к инвентарному номеру"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosin1 (\
innom int unsigned not null default 0,\
nomz   smallint unsigned not null default 0,\
zapis  TEXT not null default '',\
md TINYINT NOT NULL DEFAULT 0,\
unique(innom,nomz)) ENGINE = MYISAM");

/*
0 innom инвентарный номер
1 nomz    номер записи
2 zapis   содержимое записи
3 md    метка данных 
        0-дополнительная иныформация к инвентарному номеру 
        1-обратная сторона карточки форма ОЗ-6 
        2-обратная сторона карточки приказ министерства финансов 818 13.09.2016 г.
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Uosinp\n",gettext("Создание таблицы переменных данных инвентарных номеров"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

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
unique(innom,god,mes)) ENGINE = MYISAM");
/*
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
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Uosdok\n",gettext("Создание таблицы заголовков документов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosdok (\
datd	DATE not null default '0000-00-00',\
tipz	smallint not null default 0,\
kodop   varchar(20) not null default '',\
kontr   varchar(20) not null default '',\
nomd	varchar(20) not null default '',\
podr	smallint not null default 0,\
kodol   int not null default 0,\
podrv	smallint not null default 0,\
nomdv	varchar(20) not null default '',\
kodolv  int not null default 0,\
prov	smallint not null default 0,\
podt	smallint not null default 0,\
osnov   varchar(60) not null default '',\
ktoz	smallint unsigned not null,\
vrem	int unsigned not null,\
dover varchar(30) not null default '',\
datvd DATE not null default '0000-00-00',\
forop varchar(10) not null default '',\
datpnn DATE not null default '0000-00-00',\
nomnn varchar(16) not null default '',\
sherez varchar(40) not null default '',\
datop DATE not null default '0000-00-00',\
nds smallint not null default 0,\
pn float(5,2) not null default 0,\
dvnn DATE NOT NULL default '0000-00-00',\
mo tinyint NOT NULL default 0,\
vd varchar(255) NOT NULL DEFAULT '',\
unique(datd,nomd),\
index(nomnn)) ENGINE = MYISAM");
/*
0  datd	дата документа
1  tipz	1-приход 2-расход
2  kodop   код операции
3  kontr   код контрагента
4  nomd	номер документа
5  podr	подразделение
6  kodol  код ответственного лица
7  podrv  подразделение встречное
8  nomdv  номер парного документа для внутреннего перемещения или номер документа поставщика для приходного документа
9  kodolv  код встречного ответственного лица
10 prov	  0-провоки выполнены 1-нет
11 podt	  0-неподтвержден 1-подтвержден
12 osnov  основание
13 ktoz	 кто записал
14 vrem	 время записи
15 dover доверенность
16 datvd дата выдачи доверенности
17 forop код формы оплаты
18 datpnn дата выдачи налоговой накладной
19 nomnn номер налоговой накладной
20 sherez через кого
21 datop дата оплаты
22 nds    НДС
       0 20% НДС.
       1 0% НДС реализация на територии Украины.
       2 0% НДС экспорт.
       3 Освобождение от НДС статья 5.
23 pn - процент НДС дейстованший на момент создания документа
24 dvnn - дата выписки налоговой накладной
25 mo - метка оплаты 0- неоплачено 1-оплачено
26 vd - вида договора (для налоговой накладной)
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Uosdok1\n",gettext("Создание таблицы записей в документе"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

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
iz	double(12,2) not null,\
kind	float not null default 0,\
kodop   varchar(20) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
bsby	double(12,2) not null default 0,\
izby    double(12,2) not null default 0,\
cena	double(14,2) not null default 0,\
shs     varchar(20) not null default '',\
unique(datd,nomd,innom),\
index(innom,datd)) ENGINE = MYISAM");

/*
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

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Uosamor\n",gettext("Создание таблицы записей амортизации для налогового учёта"));
iceb_printw(strsql,data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosamor (\
innom   int not null default 0,\
da DATE NOT NULL DEFAULT '0000-00-00',\
podr	smallint not null default 0,\
hzt	varchar(20) not null default '',\
hna	varchar(20) not null default '',\
suma	double (14,2) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kmo INT NOT NULL DEFAULT 0,\
unique(innom,da)) ENGINE = MYISAM");

/*
0 innom   инвентарный номер
1 da    дата амортизации
2 podr	подразделение
3 hzt	шифр затрат
4 hna	шифр нормы аморт отчислений (группа)
5 suma	сумма
6 ktoz	кто записал
7 vrem	дата записи
8 kmo   код материально-ответсвенного
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Uosamor1\n",gettext("Создание таблицы записей амортизации для бух. учёта"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uosamor1 (\
innom   int not null default 0,\
da DATE NOT NULL DEFAULT '0000-00-00',\
podr	smallint not null default 0,\
hzt	varchar(20) not null default '',\
hna	varchar(20) not null default '',\
suma	double (14,2) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kmo INT NOT NULL DEFAULT 0,\
unique(innom,da)) ENGINE = MYISAM");

/*
0 innom   инвентарный номер
1 da    дата амортизации
2 podr	подразделение
3 hzt	шифр затрат
4 hna	шифр нормы аморт отчислений (группа)
5 suma	сумма
6 ktoz	кто записал
7 vrem	дата записи
8 kmo   код материально-ответсвенного
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

/*********Учет услуг*************************/
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Uslgrup\n",gettext("Создание таблицы перечня групп услуг"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uslgrup \
(kod    smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod    Код группы услуг
1 naik   Наименование группы услуг
2 ktoz   Кто записал
3 vrem   Время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Usloper1\n",gettext("Создание таблицы перечня операций услуг на приход"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usloper1 \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
vido    smallint not null default 0,\
prov    smallint not null default 0,\
shet	varchar(20) not null default '') ENGINE = MYISAM");

/*
0 kod  код операции
1 naik наименование операции
2 ktoz
3 vrem
4 vido 0 - внешняя 1-внутренняя
5 prov 0 проводки нужно делать 1 - не нужно
6 shet счёт учета операции
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Usloper2\n",gettext("Создание таблицы перечня операций услуг на расход"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usloper2 \
(kod    char(10) not null default '' primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
vido    smallint not null default 0,\
prov    smallint not null default 0,\
shet	varchar(20) not null default '',\
kk CHAR(16) NOT NULL DEFAULT '00') ENGINE = MYISAM");
/*
0 kod  код операции
1 naik наименование операции
2 ktoz
3 vrem
4 vido 0 - внешняя 1-внутренняя
5 prov 0 проводки нужно делать 1 - не нужно
6 shet счёт учета операции
7 kk   код контрагента реквизиты которого нужно брать в расходные документы
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Uslpodr\n",gettext("Создание таблицы перечня подразделений"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Uslpodr \
(kod    smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
np VARCHAR(255) NOT NULL DEFAULT '',\
fo VARCHAR(128) NOT NULL DEFAULT '',\
dol VARCHAR(255) NOT NULL DEFAULT '') ENGINE = MYISAM");

/*
0 kod  код подразделения
1 naik наименование подразделения
2 ktoz   Кто записал
3 vrem   Время записи
4 np    населённый пункт
5 fo   фамилия ответственного
6 dol  должность ответственного
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Uslugi\n",gettext("Создание таблицы перечня услуг"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));


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
art varchar(64) not null default '',\
ku CHAR(32) NOT NULL DEFAULT '') ENGINE = MYISAM");

/*
0 kodus   код услуги
1 kodgr   код группы
2 naius   наименование услуги
3 ei      единица измерения
4 shetu   счёт учета
5 cena    цена услуги
6 nds	0-НДС не применяется 1-применяется
7 ktoz	кто записал
8 vrem	время записи
9 art  артикул
10 ku   код "Держаного класифікатора продукції та послуг"
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Usldokum\n",gettext("Создание таблицы заголовков документов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usldokum (\
tp     smallint not null default 0,\
datd   DATE not null default '0000-00-00',\
podr   smallint not null default 0 references Uslpod (kod),\
kontr  varchar(20) not null default '' references Kontragent (kontr),\
nomd   varchar(20) not null default '',\
nomnn  varchar(20) not null default '',\
kodop  varchar(10) not null default '' references Usloper (kod),\
nds    smallint not null default 0,\
forop  varchar(20) not null default '',\
datop  DATE not null default '0000-00-00',\
uslpr  varchar(80) not null default '',\
sumkor double(10,2) not null default 0,\
pod    smallint not null default 0,\
pro    smallint not null default 0,\
oplata smallint not null default 0,\
blokir smallint unsigned not null default 0,\
nomdp  varchar(20) not null default '',\
datdp  DATE not null default '0000-00-00',\
dover  varchar(20) not null default '',\
datdov DATE not null default '0000-00-00',\
sherez varchar(40) not null default '',\
ktoi   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
osnov  varchar(80) not null default '',\
datpnn DATE not null default '0000-00-00',\
sumnds double(10,2) not null default 0,\
pn float(5,2) not null default 0,\
mo tinyint NOT NULL default 0,\
k00 char(16) NOT NULL DEFAULT '00',\
unique (datd,podr,nomd,tp),\
index (nomnn)\
) ENGINE = MYISAM");

/*
0  tp     1-приход 2-расход
1  datd   дата
2  podr   код подразделения
3  kontr  код контрагента
4  nomd   номер документа
5  nomnn  номер налоговой накладной
6  kodop  код операции
7  nds    НДС
       0 20% НДС.
       1 0% НДС реализация на територии Украины.
       2 0% НДС экспорт.
       3 0% Освобождение от НДС статья 5.
8  forop  код формы оплаты
9  datop  дата оплаты или отсрочки
10 uslpr  условие продажи
11 sumkor сумма корректировки к документа
12 pod    0-не подтверждена 1-подтверждена
13 pro    0-не выполнены проводки 1-выполнены
14 oplata 0-не оплачена 1-оплачена
15 blokir код того кто заблокировал документ
16 nomdp  номер документа поставщика
17 datdp  Дата виписки документа поставщика
18 dover  довереность
19 datdov дата доверенности
20 sherez через кого
21 ktoi   кто записал
22 vrem   время записи
23 osnov  основание
24 datpnn дата получения налоговой накладной
25 sumnds Сумма НДС для приходных документов введенная вручную
26 pn - процент НДС дейстованший на момент создания документа
27 mo - метка оплаты 0-неоплачено 1-оплачено
28 k00 - код контрагента реквизиты которого берутся в распечатку
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Usldokum1\n",gettext("Создание таблицы записей в документе"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usldokum1 (\
tp     smallint not null default 0,\
datd   DATE not null default '0000-00-00',\
nomd   varchar(20) not null default '',\
metka  smallint not null default 0,\
kodzap int not null default 0,\
kolih  double(16,6) not null default 0,\
cena   double(16,6) not null default 0,\
ei     varchar(20) not null default '' references Edizmer (kod),\
shetu  varchar(20) not null default '' references Plansh (ns),\
podr   smallint not null default 0 references Uslpod (kod),\
ktoi   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
dnaim  varchar(70) not null default '',\
nz     smallint not null default 0,\
shsp   varchar(20) not null default '',\
unique (datd,podr,tp,nomd,metka,kodzap,nz)) ENGINE = MYISAM");

/*
0  tp     1-приход 2-расход
1  datd   Дата документа
2  nomd   Номер документа
3  metka  0-материал 1-услуга
4  kodzap Код материалла/услуги
5  kolih  Количество
6  cena   Цена
7  ei     Единица измерения
8  shetu  Счет учета
9  podr   Код подразделения
10 ktoi   Кто записал
11 vrem   Время записи
12 dnaim  Дополнительное наименование услуги
13 nz     номер записи
14 shsp   счёт списания/приобретения
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Usldokum2\n",gettext("Создание таблицы подверждающих записей к документу"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usldokum2 (\
tp     smallint not null default 0,\
datd   DATE not null default '0000-00-00',\
datp   DATE not null default '0000-00-00',\
nomd   varchar(20) not null default '',\
metka  smallint not null default 0,\
kodzap int not null default 0,\
kolih  double(16,6) not null default 0,\
cena   double(16,6) not null default 0,\
ei     varchar(20) not null default '' references Edizmer (kod),\
shetu  varchar(20) not null default '' references Plansh (ns),\
podr   smallint not null default 0 references Uslpod (kod),\
ktoi   smallint unsigned not null default 0,\
vrem   int unsigned not null default 0,\
nz     smallint not null default 0,\
unique (datd,datp,podr,tp,nomd,metka,kodzap,nz),\
index (datp)) ENGINE = MYISAM");

/*
0  tp     1-приход 2-расход
1  datd   Дата документа
2  datp   Дата подтверждения документа
3  nomd   Номер документа
4  metka  0-материал 1-услуга
5  kodzap Код услуги
6  kolih  Количество
7  cena   Цена
8  ei     Единица измерения
9  shetu  Счет учета
10 podr   Код подразделения
11 ktoi   Кто записал
12 vrem   Время записи
13 nz     номер записи в документе
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

//Записывается произвольный текст
iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s  Usldokum3\n",gettext("Создание таблицы приложения к счёту"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Usldokum3 (\
podr    int not null default 0,\
god     smallint not null default 0,\
tp      smallint not null default 0,\
nomd    varchar(20) not null default '',\
nomz    smallint unsigned not null default 0,\
zapis   varchar(255) not null default '',\
unique(podr,god,tp,nomd,nomz)) ENGINE = MYISAM");

/*
0 podr    подразделение
1 god     год
2 tp      1-приход 2-расход
3 nomd    номер документа
4 nomz    номер записи
5 zapis   содержимое записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

/***********************/
/*Учет кассовых ордеров*/
/***********************/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Kas\n",gettext("Создание таблицы перечня кас"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kas \
(kod    smallint not null default 0 primary key,\
naik    char(60) not null default '',\
shet    char(20) not null default '',\
fio     char(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kv CHAR(16) NOT NULL DEFAULT '') ENGINE = MYISAM");

/*
0 kod     Код кассы
1 naik    Наименование кассы
2 shet   Счёт кассы
3 fio     Фамилия кассира
4 ktoz   Кто записал
5 vrem   Время записи
6 kv     Код валюты
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Kasop1\n",gettext("Создание таблицы приходных операций"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kasop1 \
(kod    char(10) not null default '' primary key,\
naik    char(250) not null default '',\
shetkas char(20) not null default '',\
shetkor char(20) not null default '',\
metkapr smallint not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kcn     smallint not null default 0) ENGINE = MYISAM");

/*
0 kod     Код операции
1 naik    Наименование операции
2 shetkas счёт кассы
3 shetkor счёт корреспондент
4 metkapr 0-проводки не выполняются 1-выполняются
5 ktoz    кто записал
6 vrem    время записи
7 kcn     код целевого назначения
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Kasop2\n",gettext("Создание таблицы расходных операций"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kasop2 \
(kod    char(10) not null default '' primary key,\
naik    char(250) not null default '',\
shetkas char(20) not null default '',\
shetkor char(20) not null default '',\
metkapr smallint not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kcn     smallint not null default 0) ENGINE = MYISAM");

/*
0 kod     Код операции
1 naik    Наименование операции
2 shetkas счёт кассы
3 shetkor счёт корреспондент
4 metkapr 0-проводки не выполняются 1-выполняются
5 ktoz    кто записал
6 vrem    время записи
7 kcn     код целевого назначения
*/


admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Kasord\n",gettext("Создание таблицы заголовков кассовых ордеров"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kasord (\
kassa   smallint not null default 0,\
tp	smallint not null default 0,\
datd    DATE NOT NULL default '0000-00-00',\
nomd    varchar(20) NOT NULL default '',\
shetk   varchar(20) not null default '',\
kodop   varchar(10) not null default '',\
prov    smallint not null default 0,\
podt    smallint not null default 0,\
osnov   varchar(250) not null default '',\
dopol   varchar(80) not null default '',\
fio     varchar(60) not null default '',\
dokum   varchar(255) not null default '',\
god     smallint not null default 0,\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
nb      varchar(20) not null default '',\
nomv    varchar(20) not null default '',\
unique(nomd,kassa,tp,god),\
index(datd,kassa),\
index(nb)) ENGINE = MYISAM");

/*
0  kassa   номер кассы
1  tp	   1-приход 2-расход
2  datd    дата документа
3  nomd    номер документа
4  shetk   счёт корреспондент
5  kodop   код операции
6  prov    0 провоки не сделаны 1-сделаны
7  podt    0 не подтвержден 1-подтвержден
8  osnov   Основание 
9  dopol   дополнение
10 fio     фамилия имя отчество кому выданы деньги если есть ведомомть к ордеру
11 dokum   наименование дата и номер документа
12 god     год
13 ktoi    Кто записал
14 vrem    Время записи
15 nb      номер бланка
16 nomv    номер ведомости на выплату средств кореспондируется с Kasnomved
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Kasord1\n",gettext("Создание таблицы записей кассовых ордеров"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kasord1 (\
kassa   smallint not null default 0,\
tp	smallint not null default 0,\
datd    DATE not null default '0000-00-00',\
datp    DATE not null default '0000-00-00',\
nomd    varchar(20) NOT NULL default '',\
kontr   varchar(20) not null default '',\
suma    double(14,2) not null default 0,\
god     smallint not null default 0,\
metka   smallint not null default 0,\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
koment  varchar(100) not null default '',\
unique(kassa,god,tp,nomd,kontr),\
index(datd),\
index(datp)) ENGINE = MYISAM");

/*
0  kassa   номер кассы
1  tp	  1-приход 2-расход
2  datd    дата документа
3  datp    дата подтверждения записи
4  nomd    номер документа
5  kontr   код контрагента
6  suma    сумма
7  god     год
8  metka   0 запись не перегружена 1-перегружена (В расчёт зарплаты)
9  ktoi    Кто записал
10 vrem    Время записи
11 koment  комментарий
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Kasnomved\n",gettext("Создание таблицы журнала ведомостей"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Kasnomved (\
datd    DATE not null default '0000-00-00',\
god     smallint not null default 0,\
nomv    varchar(10) not null default '',\
suma    double(14,2) not null default 0,\
koment  varchar(100) not null default '',\
ktoi    smallint unsigned not null default 0,\
vrem    int unsigned not null default 0,\
nomd varchar(20) not null default '',\
unique(god,nomv),\
index(datd,nomv)) ENGINE = MYISAM");

/*
0  datd    дата документа
1  god     год документа
2  nomv    номер ведомости
3  suma    сумма по документу
4  koment  комментарий
5  ktoi    Кто записал
6  vrem    Время записи
7  nomd    номер кассового ордера из таблицы Kasord
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Kascn\n",gettext("Создание таблицы целевых назначений"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kascn (\
kod    smallint not null default 0 primary key,\
naik    varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");

/*
kod   код целевого назначения
naik  наименование целевого назначения
ktoz  кто записал
vrem  время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Kascnsl\n",gettext("Создание таблицы сальдо по целевым назначениям"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Kascnsl (\
god   smallint not null default 0,\
kod   smallint not null default 0,\
ks    int not null default 0,\
saldo double(10,2) not null default 0,\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
unique(god,kod,ks)) ENGINE = MYISAM");

/*
0 god   год
1 kod   код целевого назначения
2 ks    код кассы
3 saldo сальдо
4 ktoz  кто записал
5 vrem  время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

//************************************************************************
//Подсистема "Учёт командировочных расходов"
//************************************************************************



iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Ukrvkr\n",gettext("Создание таблицы видов командировок"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ukrvkr (\
kod	char(10) not null default '' primary key,\
naik    varchar(80) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");

/*
0 kod   код вида командировок
1 naik  наименование вида командировок
2 ktoz  кто записал
3 vrem  время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Ukrgrup\n",gettext("Создание таблицы групп видов командировок"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ukrgrup (\
kod    smallint not null default 0 primary key,\
naik    varchar(80) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");

/*
0 kod   код группы вида командировок
1 naik  наименование группы
2 ktoz  кто записал
3 vrem  время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Ukrkras\n",gettext("Создание таблицы видов расходов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ukrkras (\
kod     int not null default 0 primary key,\
naim    varchar(80) not null default '',\
cena    double(10,2) not null default 0,\
shet    varchar(20) not null default '',\
kgr     int not null default 0,\
eiz     varchar(10) not null default '',\
mnds    smallint not null default 0,\
ktoz    smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(naim)) ENGINE = MYISAM");

/*
0 kod   код вида расходов
1 naim  наименование вида расхода
2 cena  цена
3 shet  счёт учета
4 kgr   код группы
5 eiz   единица измерения
6 mnds  0-цена введена с НДС 1-без НДС
7 ktoz  кто записал
8 vrem  время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Ukrdok\n",gettext("Создание таблицы заголовков документов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ukrdok (\
god     smallint not null default 0,\
nomd	varchar(20) not null default '',\
datd    DATE not null default '0000-00-00',\
kont	varchar(20) not null default '',\
pnaz    varchar(80) not null default '',\
organ   varchar(80) not null default '',\
celk    varchar(80) not null default '',\
datn    DATE not null default '0000-00-00',\
datk    DATE not null default '0000-00-00',\
nompr   varchar(20) not null default '',\
datp    DATE not null default '0000-00-00',\
mprov   smallint not null default 0,\
mpodt   smallint not null default 0,\
vkom    varchar(10) not null default '',\
nomao   varchar(10) not null default '',\
datao   DATE not null default '0000-00-00',\
ktoz    smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(god,nomd),\
index(datd,nomd),\
index(datao,nomao)) ENGINE = MYISAM");

/*
 0 god   год документа
 1 nomd  Номер документа
 2 datd  Дата документа
 3 kont  код контрагента
 4 pnaz  пунк назначения
 5 organ предприятия и организации
 6 celk  Цель командировки
 7 datn  Дата начала командировки
 8 datk  Дата конца командировки
 9 nompr Номер приказа
10 datp  Дата приказа
11 mprov Метка выполнения проводок 0-не выполнены 1-выполнены
12 mpodt Метка присутствия записей в документе 0-нет записей 1-есть
13 vkom  Вид командировки
14 nomao Номер авансового отчёта
15 datao Дата авансового отчёта
16 ktoz  кто записал
17 vrem  время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Ukrdok1\n",gettext("Создание таблицы записей в документе"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Ukrdok1 (\
datd	DATE not null default '0000-00-00',\
nomd	varchar(20) not null default '',\
kodr    int not null default 0,\
shet	varchar(20) not null default '',\
kolih   double(15,4) not null default 0,\
cena    double(15,2) not null default 0,\
ediz    varchar(10) not null default '',\
snds    double(10,2) not null default 0,\
ktoz    smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
nomz	smallint not null default 0,\
kontr   varchar(20) not null default '',\
kdrnn   varchar(20) not null default '',\
ndrnn   varchar(30) not null default '',\
sn      varchar(10) NOT NULL default '',\
dvnn DATE NOT NULL default '0000-00-00',\
ss double(10,2) not null default 0,\
unique(datd,nomd,kodr,shet,nomz))" );

/*
 0 datd  Дата документа
 1 nomd  Номер документа
 2 kodr  Код расхода
 3 shet  Счет
 4 kolih Количество
 5 cena  Сумма без НДС
 6 ediz  Единица измерения
 7 snds  Сумма НДС
 8 ktoz  Кто записал
 9 vrem  Время записи
10 nomz  Номер записи
11 kontr Код контрагента от кого получена услуга или товар за наличные(вводится чтобы была сделана дополнительная проводка)
12 kdrnn Код контрагента для реестра налоговых накладных
13 ndrnn Номер документа, который в реестре нал. накладных будет записан как номер налоговой накладной
14 sn    счёт НДС (Иногда нужно чтобы в одном документе разные записи шли на разные счета)
15 dvnn  дата выписки налоговой накладной
16 ss    Старховой сбор
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

//*************************************************************
//Подсистема "Учёт путевых листов"
//************************************************************

/************31.07.2003******************/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uplpodr\n",gettext("Создание таблицы подразделений"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplpodr (\
kod   smallint not null default 0 primary key,\
naik  varchar(80) not null default '',\
ktoz  smallint unsigned default 0,\
vrem  int unsigned default 0) ENGINE = MYISAM");

/*
0 kod   код подразделения
1 naik  наименование подразделения
2 ktoz  кто записал
3 vrem  время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uplmt\n",gettext("Создание таблицы марок топлива"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplmt (\
kodt  char(20) not null default '' primary key,\
kodm  int not null default 0,\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0) ENGINE = MYISAM");

/*
0 kodt  код марки топлива
1 kodm  код материалла в списке материалов
2 ktoz  кто записал
3 vrem  время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uplouot\n",gettext("Создание таблицы объектов учёта остатков топлива"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplouot (\
kod   smallint not null default 0 primary key,\
mt    smallint not null default 0,\
skl   smallint not null default 0,\
naik  varchar(80) not null default '',\
kontr varchar(20) not null default '',\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0) ENGINE = MYISAM");

/*
0 kod   код объекта
1 mt    0-заправка 1-водитель
2 skl   код склада в материальном учете
3 naik  наименование
4 kontr код контрагента
5 ktoz  кто записал
6 vrem  время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uplavt\n",gettext("Создание таблицы списка автомобилей"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplavt (\
kod   smallint not null default 0 primary key,\
nomz  varchar(20) not null default '',\
kzv   smallint not null default 0,\
naik  varchar(80) not null default '',\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
index(nomz)) ENGINE = MYISAM");

/*
0  kod   код автомобиля
1  nomz  номерной знак
2  kzv   код закрепленного водителя
3  naik  наименование
4  ktoz  кто записал
5  vrem  время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/



iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uplnst\n",gettext("Создание таблицы норм списания топлива"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplnst (\
kod   char(16) not null default '' primary key,\
naik  char(124) not null default '',\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0) ENGINE = MYISAM");

/*
0  kod   код нормы списания топлива
1  naik  наименование нормы списания топлива
2  ktoz  кто записал
3  vrem  время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uplavt1\n",gettext("Создание таблицы норм списания топлива к автомобилю"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplavt1 (\
kod   smallint not null default 0,\
knst  char(16) not null default '',\
nst   float(8,3) not null default 0.,\
ei    varchar(16) not null default '',\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
unique(kod,knst)) ENGINE = MYISAM");

/*
0  kod   код автомобиля
1  knst  код нормы списания топлива
2  nst   норма списания топлива
3  ei    единица изменения
4  ktoz  кто записал
5  vrem  время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Upldok3\n",gettext("Создание таблицы списания топлива к путевому листу"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Upldok3 (\
datd  DATE not null default '0000-00-00',\
kp smallint not null default 0,\
nomd varchar(16) not null default '',\
knst varchar(16) not null default '',\
kl   double(10,3) not null default 0.,\
nst  float(8,3) not null default 0.,\
zn   float(8,3) not null default 0.,\
zf   float(8,3) not null default 0.,\
ei    varchar(16) not null default '',\
kom varchar(255) not null default '',\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
unique(datd,kp,nomd,knst)) ENGINE = MYISAM");

/*
0  datd  дата документа
1  kp    код подразделения (если равно 0 - акт списания)
2  nomd  номер документа
3  knst  код нормы списания топлива
4  kl    количество
5  nst   норма списания топлива
6  zn    затраты по норме
7  zf    затраты по факту
8  ei    единица измерения
9 kom   коментарий
10  ktoz  кто записал
11 vrem  время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Upldok2a\n",gettext("Создание таблицы записей списания топлива по счетам"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Upldok2a (\
datd  DATE not null default '0000-00-00',\
kp smallint not null default 0,\
nomd varchar(32) not null default '',\
kt varchar(32) not null default '',\
shet varchar(10) not null default '',\
zt float(8,3) not null default 0,\
zpn float(8,3) not null default 0.,\
koment varchar(255) not null default '',\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
unique(datd,kp,nomd,kt,shet)) ENGINE = MYISAM");

/*
0 datd  дата документа
1 kp    код подразделения
2 nomd  номер документа
3 kt    код топлива
4 shet  счёт списания
5 zt    затраты топлива по факту
6 zpn  затраты по норме
7 koment коментарий
8 ktoz  кто записал
9 vrem  время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/





iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Upldok\n",gettext("Создание таблицы заголовков путевых листов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Upldok (\
datd DATE not null default '0000-00-00',\
kp smallint not null default 0,\
nomd varchar(20) not null default '',\
god smallint not null default 0,\
nomb varchar(20) not null default '',\
ka smallint not null default 0,\
kv smallint not null default 0,\
psv int unsigned not null default 0,\
psz int unsigned not null default 0,\
denn DATE not null default '0000-00-00',\
denk DATE not null default '0000-00-00',\
vremn TIME not null default '000:00:00',\
vremk TIME not null default '000:00:00',\
mt smallint not null default 0,\
mp smallint not null default 0,\
otmet varchar(250) not null default '',\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
nz smallint not null default 0,\
unique(god,kp,nomd),\
unique(ka,datd,nz),\
index(datd),\
index(nomd)) ENGINE = MYISAM");

/*
0  datd  дата документа
1  kp    код подразделения (если равно 0 - акт списания)
2  nomd  номер документа
3  god   год документа
4  nomb  номер бланка документа строгой отчётности
5  ka    код автомобиля
6  kv    код водителя
7  psv   показания спидометра при выезде
8  psz   показания спидометра при возвращении
9  denn  Дата выезда
10 denk  Дата возвращения
11 vremn время выезда
12 vremk время возвращения
13 mt    0-не списано (или не полностью списано) топливо пообъектно 1-списано
14 mp    0-проводки не сделаны 1-сделаны
15 otmet Особенные отметки
16 ktoz  кто записал
17 vrem  время записи
18 nz     номер по порядку документа выписанного в один и тотже день на один и тотже автомобиль
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Upldok1\n",gettext("Создание таблицы записей в путевой лист"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Upldok1 (\
datd  DATE not null default '0000-00-00',\
kp smallint not null default 0,\
nomd varchar(10) not null default '',\
nzap smallint not null default 0,\
ka smallint not null default 0,\
kv smallint not null default 0,\
kodsp smallint not null default 0,\
tz smallint not null default 0,\
kodtp varchar(20) not null default '',\
kolih double(10,3) not null default 0,\
kom varchar(80) not null default '',\
kap smallint not null default 0,\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
nz smallint not null default 0,\
unique(datd,kp,nomd,nzap),\
index(datd,ka)) ENGINE = MYISAM");

/*
 0 datd  дата документа
 1 kp    код подразделения (если равно 0 - акт списания)
 2 nomd  номер документа
 3 nzap  номер записи в документе
 4 ka    код автомобиля
 5 kv    код водителя
 6 kodsp для приходной записи код поставщика топлива(заправка или водитель) / для расходной записи 0
 7 tz    1-приход 2-расход
 8 kodtp код топлива
 9 kolih количество
10 kom   комментарий
11 kap   код автомобиля с которого передали топливо (Если kodsp это код водителя, иначе 0)
12 ktoz  кто записал
13 vrem  время записи
14 nz    номер по порядку документа выписанного в один и тотже день на один и тотже автомобиль
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uplsal\n",gettext("Создание таблицы сальдо по топливу"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplsal (\
god smallint not null default 0,\
ka smallint not null default 0,\
kv smallint not null default 0,\
kmt varchar(20) not null default '',\
sal double(10,2) not null default 0,\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
unique(god,ka,kv,kmt)) ENGINE = MYISAM");

/*
0 god   год
1 ka    код автомобиля
2 kv    код водителя
3 kmt   код марки топлива
4 sal   сальдо
5 ktoz  кто записал
6 vrem  время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uplmtsc\n",gettext("Создание таблицы средних цен на топлива"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uplmtsc (\
datsc DATE NOT NULL default '0000-00-00',\
kodt  char(20) not null default '',\
srcn  float(7,2) not null default 0,\
ktoz  smallint unsigned not null default 0,\
vrem  int unsigned not null default 0,\
kp    float(7,3) not null default 0,\
unique (datsc,kodt)) ENGINE = MYISAM");

/*
0 datsc дата начала действия настройки
1 kodt  код топлива
2 srcn  средняя цена топлива
3 ktoz  кто записал
4 vrem  время записи
5 kp    коэффициент перевода литры => киллограммы
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

/***********************************************/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Specif\n",gettext("Создание таблицы спецификаций на изделия"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Specif \
(kodi   int not null default 0,\
kodd    int not null default 0,\
kolih   double(20,10) not null default 0,\
koment  varchar(60) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kz      smallint not null default 0,\
ei      varchar(10) not null default '',\
unique(kodi,kodd)) ENGINE = MYISAM");

/*
0 kodi    код изделия
1 kodd    код детали
2 kolih   количество
3 koment  комментарий
4 ktoz	кто записал
5 vrem	время записи
6 kz    код записи 0-материалы 1-услуги
7 ei    единица измерения
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Reenn\n",gettext("Создание таблицы реестра виданых налоговых накладных"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Reenn (\
nz int unsigned AUTO_INCREMENT NOT NULL,\
datd DATE not null default '0000-00-00',\
nnn varchar(20) not null default '',\
sumd double(10,2) not null default 0,\
snds double(10,2) not null default 0,\
mt tinyint not null default 0,\
inn varchar(30) not null default '',\
nkontr varchar(255) not null default '',\
mi tinyint not null default 0,\
datdi DATE not null default '0000-00-00',\
nomdi varchar(20) not null default '',\
koment varchar(100) not null default '',\
ktoi smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
gr varchar(10) not null default '',\
ko varchar(10) not null default '',\
vd char(32) not null default '',\
datv DATE not null default '0000-00-00',\
dnz int NOT NULL DEFAULT 0,\
nnni int NOT NULL DEFAULT 0,\
PRIMARY KEY (nz),\
index(datd,nnn)) ENGINE = MYISAM");

/************
0  nz      уникальный номер записи устанавливаемый базой данных автоматически
1  datd    дата документа
2  nnn     номер налоговой накладной
3  sumd    общая сумма по документу
4  snds    сумма НДС
5  mt      метка документа (каким НДС обкладывается)
6  inn     индивидуальный налоговый номер
7  nkontr  наименование контрагента
8  mi      метка импорта документа 1-материальный учёт 2-учёт услуг 3-учёт основных средств 4-учёт командировочных расходов 5-главная книга
9 datdi   дата документа импорта
10 nomdi   номер документа импорта
11 koment  комментарий
12 ktoi    Кто записал
13 vrem    Время записи
14 gr      код группы документа
15 ko      код операции в импортируемом документе
16 vd      вид документа
17 datv    дата выписки налоговой накладной
18 dnz     дневная нумерация записей
19 nnni    номер налоговой накладной в цифровой форме - для правильной сортрировки
***************/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Reenn1\n",gettext("Создание таблицы реестка полученных налоговых накладных"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Reenn1 (\
nz int unsigned AUTO_INCREMENT NOT NULL,\
datd DATE not null default '0000-00-00',\
nnn varchar(20) not null default '',\
sumd1 double(10,2) not null default 0,\
snds1 double(10,2) not null default 0,\
sumd2 double(10,2) not null default 0,\
snds2 double(10,2) not null default 0,\
sumd3 double(10,2) not null default 0,\
snds3 double(10,2) not null default 0,\
sumd4 double(10,2) not null default 0,\
snds4 double(10,2) not null default 0,\
inn varchar(30) not null default '',\
nkontr varchar(255) not null default '',\
mi tinyint not null default 0,\
datdi DATE not null default '0000-00-00',\
nomdi varchar(20) not null default '',\
koment varchar(100) not null default '',\
gr varchar(10) not null default '',\
ktoi smallint unsigned not null default 0,\
vrem int unsigned not null default 0,\
ko varchar(10) not null default '',\
dvd DATE NOT NULL default '0000-00-00',\
vd char(32) not null default '',\
PRIMARY KEY (nz),\
index(datd,nnn)) ENGINE = MYISAM");

/************
0  nz      уникальный номер записи устанавливаемый базой данных автоматически
1  datd    дата получения налоговой накладной
2  nnn     номер налоговой накладной
3  sumd1    общая сумма по документу по основной ставке
4  snds1    сумма НДС                по основной ставке
5  sumd2    общая сумма по документу по ставке 7%
6  snds2    сумма НДС                по ставке 7%
7  sumd3    общая сумма по документу по ставке 0%
8  snds3    сумма НДС                по ставке 0%
9  sumd4    общая сумма по документу освобождены от налогообложения
10 snds4    сумма НДС                освобождены от налогообложения
11 inn     индивидуальный налоговый номер
12 nkontr  наименование контрагента
13 mi      метка импорта документа 1-материальный учёт 2-учёт услуг 3-учёт основных средств 4-учёт командировочных расходов 5-главная книга
14 datdi   дата документа импорта
15 nomdi   номер документа импорта
16 koment  комментарий
17 gr      группа документа
18 ktoi    Кто записал
19 vrem    Время записи
20 ko      код операции в импортируемом документе
21 dvd     Дата выписки налоговой накладной
22 vd      вид документа
***************/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Reegrup1\n",gettext("Создание таблицы групп полученных налоговых накладных"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Reegrup1 \
(kod    varchar(10) not null default '' primary key,\
naik    varchar(80) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod   код группы
1 naik  наименование группы
2 ktoz  кто записал
3 vrem  время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Reegrup2\n",gettext("Создание таблицы групп выданных налоговых накладных"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Reegrup2 \
(kod    varchar(10) not null default '' primary key,\
naik    varchar(80) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0) ENGINE = MYISAM");
/*
0 kod   код группы
1 naik  наименование группы
2 ktoz  кто записал
3 vrem  время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uddok\n",gettext("Создание таблицы заголовков доверреностей"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uddok \
(datd   DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
datds   DATE NOT NULL default '0000-00-00',\
komu    varchar(80) NOT NULL default '',\
post    varchar(255) NOT NULL default '',\
podok   varchar(50) NOT NULL default '',\
otis    varchar(100) NOT NULL default '',\
datn    DATE NOT NULL default '0000-00-00',\
nomn    varchar(100) NOT NULL default '',\
seriq varchar(20) NOT NULL default '',\
nomerd varchar(20) NOT NULL default '',\
datavd DATE NOT NULL default '0000-00-00',\
vidan  varchar(200) NOT NULL default '',\
dolg   varchar(50) NOT NULL default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(datd,nomd),\
index(nomd)) ENGINE = MYISAM");
/*
0  datd Дата выдачи доверенности
1  nomd номер доверенности
2  datds действительна до
3  komu  кому выдана доверенность
4  post  поставщик
5  podok по документа
6  otis  отметка об использовании
7  datn  дата наряда
8  nomn  номер наряда
9 seriq серия
10 nomerd номер документа
11 datavd дата выдачи документа
12 vidan  кем выдан
13 dolg   должность
14  ktoz  кто записал
15 vrem  время записи
*/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Uddok1\n",gettext("Создание таблицы записей в доверрености"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Uddok1 \
(datd  DATE not null default '0000-00-00',\
nomd    varchar(20) not null default '',\
nz      smallint NOT NULL default 0,\
zapis   varchar(200) NOT NULL default '',\
ei      varchar(20) NOT NULL default '',\
kol     double(16,4) NOT NULL default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(datd,nomd,nz)) ENGINE = MYISAM");
/*
0  datd Дата выдачи доверенности
1  nomd номер доверенности
2  nz    уникальный номер записи
3  zapis содержимое записи
4  ei    единица измерения
5  kol   количество
6  ktoz  кто записал
7  vrem  время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s icebuser\n",gettext("Создание таблицы операторов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE icebuser \
(login char(32) not null default '' primary key,\
fio varchar(255) not null default '',\
un  int unsigned not null default 0,\
kom varchar(255) not null default '',\
logz   varchar(32) not null default '',\
vrem   int unsigned not null default 0,\
gkd tinyint not null default 0,\
gkb tinyint not null default 0,\
gkn tinyint not null default 0,\
mud tinyint not null default 0,\
mub tinyint not null default 0,\
mun tinyint not null default 0,\
pdd tinyint not null default 0,\
pdb tinyint not null default 0,\
pdn tinyint not null default 0,\
zpd tinyint not null default 0,\
zpb tinyint not null default 0,\
zpn tinyint not null default 0,\
osd tinyint not null default 0,\
osb tinyint not null default 0,\
osn tinyint not null default 0,\
uud tinyint not null default 0,\
uub tinyint not null default 0,\
uun tinyint not null default 0,\
kod tinyint not null default 0,\
kob tinyint not null default 0,\
kon tinyint not null default 0,\
krd tinyint not null default 0,\
krb tinyint not null default 0,\
krn tinyint not null default 0,\
pld tinyint not null default 0,\
plb tinyint not null default 0,\
pln tinyint not null default 0,\
nnd tinyint not null default 0,\
nnb tinyint not null default 0,\
nnn tinyint not null default 0,\
udd tinyint not null default 0,\
udb tinyint not null default 0,\
udn tinyint not null default 0,\
vsd tinyint not null default 0,\
vsb tinyint not null default 0,\
vsn tinyint not null default 0,\
vdb tinyint not null default 0,\
tn  INT UNSIGNED NOT NULL DEFAULT 0,\
unique(un)) ENGINE = MYISAM");

/*
 0 login  логин оператора
 1 fio    фамилия имя отчество
 2 un     уникальный номер оператора
 3 kom    коментарий
 4 logz   логин сделавшего запись
 5 vrem   Время записи

 6 gkd    если равно 0 то разрешена работа с подсистемой "Главная книга"
 7 gkb    если равно 0 то разрешена блокировка подсистемы "Главная книга"
 8 gkn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Главная книга"

 9 mud    если равно 0 то разрешена работа с подсистемой "Материальный учёт"
10 mub    если равно 0 то разрешена блокировка подсистемы "Материальный учёт"
11 mun    если равно 0 то разрешена работа с настроечными файлами подсистемы "Материальный учёт"

12 pdd    если равно 0 то разрешена работа с подсистемой "Платёжные документы"
13 pdb    если равно 0 то разрешена блокировка подсистемы "Платёжные документы"
14 pdn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Платёжные документы"

15 zpd    если равно 0 то разрешена работа с подсистемой "Заработная плата"
16 zpb    если равно 0 то разрешена блокировка подсистемы "Зароботная плата"
17 zpn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Зароботная плата"

18 osd    если равно 0 то разрешена работа с подсистемой "Учёт основных средств"
19 osb    если равно 0 то разрешена блокировка подсистемы "Учёт основных средств"
20 osn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт основных средств"

21 uud    если равно 0 то разрешена работа с подсистемой "Учёт услуг"
22 uub    если равно 0 то разрешена блокировка подсистемы "Учёт услуг"
23 uun    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт услуг"

24 kod    если равно 0 то разрешена работа с подсистемой "Учёт кассовых ордеров"
25 kob    если равно 0 то разрешена блокировка подсистемы "Учёт кассовых ордеров"
26 kon    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт кассовых ордеров"

27 krd    если равно 0 то разрешена работа с подсистемой "Учёт командировочных расходов"
28 krb    если равно 1 то разрешена блокировка подсистемы "Учёт командировочных расходов"
29 krn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт командироваочных расходов"

30 pld    если равно 0 то разрешена работа с подсистемой "Учёт путевых листов"
31 plb    если равно 0 то разрешена блокировка подсистемы "Учёт путувых листов"
32 pln    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт путевых листов"

33 nnd    если равно 0 то разрешена работа с подсистемой "Реестр налоговых накладных"
34 nnb    если равно 0 то разрешена блокировка подсистемы "Реестр налоговых накладных"
35 nnn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Реестр налоговых накладных"

36 udd    если равно 0 то разрешена работа с подсистемой "Учёт доверенностей"
37 udb    если равно 0 то разрешена блокировка подсистемы "Учёт доверенностей"
38 udn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Учёт доверенностей"

39 vsd    если равно 0 то разрешена работа с подсистемой "Выписка счетов"
40 vsb    если равно 0 то разрешена блокировка подсистемы "Выписка счетов"
41 vsn    если равно 0 то разрешена работа с настроечными файлами подсистемы "Выписка счетов"

42 vdb    если равно 0 то выгрузка дампа базы оператору разрешена
43 tn     табельный номер
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Blok\n",gettext("Создание таблицы блокировок подсистем"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Blok \
(kod int not null default 0,\
god smallint unsigned not null default 0,\
mes smallint unsigned not null default 0,\
log varchar(255) not null default '',\
shet varchar(255) not null default '',\
ktoz int not null default 0,\
vrem int unsigned not null default 0,\
unique(kod,god,mes)) ENGINE = MYISAM");

/*
0 kod  - код подсистемы
1 god  - год
2 mes  - месяц
3 log  - логины операторов которым отменена блокировка
4 shet - счёта которые заблокированы (только для подсистемы "Главная книга*)
5 ktoz - номер оператора сделавшего запись
6 vrem - время записи
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Alx\n",gettext("Создание таблицы настроек"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Alx \
(fil varchar(255) not null default '',\
ns int not null default 0,\
ktoz int not null default 0,\
vrem int unsigned not null default 0,\
str TEXT not null default '',\
unique(fil,ns)) ENGINE = MYISAM");

/*
0 fil  - имя файла
1 ns   - номер строки
2 ktoz - номер оператора сделавшего запись
3 vrem - время записи
4 str  - строка текста
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Doc\n",gettext("Создание таблицы документации"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Doc \
(fil varchar(255) not null default '',\
ns int not null default 0,\
str TEXT not null default '',\
unique(fil,ns)) ENGINE = MYISAM");


/*
0 fil  - имя файла
1 ns   - номер строки
2 str  - строка текста
*/

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Nalog\n",gettext("Создание таблицы налогов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Nalog \
(vn int not null default 0,\
dndn DATE not null default '0000-00-00',\
pr double not null default 0,\
kom varchar(255) not null default '',\
ktoz int not null default 0,\
vrem int unsigned not null default 0,\
unique(vn,dndn)) ENGINE = MYISAM");

admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

/*
0 vn   - вид налога 0-НДС
1 dndn - Дата начала действия налога
2 pr   - процент
3 kom  - коментарий
4 ktoz - номер оператора сделавшего запись
5 vrem - время записи
*/

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    
sprintf(strsql,"%s Zaresv\n",gettext("Создание таблицы процентов"));
iceb_printw(strsql,data->buffer,data->view);

memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"CREATE TABLE Zaresv \
(kf int not null default 0,\
datnd DATE not null default '0000-00-00',\
pr float not null default 0.,\
pr1 float not null default 0.,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
unique(kf,datnd)) ENGINE = MYISAM");

/******
0 kf    - код фонда
1 datnd - дата начала действия
2 pr    - процент начисления на фонд зарплаты
3 pr1   - процент удержания с работника
4 ktoz  - кто записал
5 vrem  - время записи
**/
admint_makebaz_r_crtb(strsql,&kol_oh,data); /*создаём таблицу*/

/*Загружаем настроечные файлы в базу данных*/
iceb_alxzag(data->fromnsi.ravno(),0,data->view,data->buffer,data->window);

iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    

/*Загружаем документацию для терминальной версии*/
if(data->fromdoc.getdlinna() > 1) /*Для виндовой версии этот параметр не указывается*/
 {
  sprintf(strsql,"%s\n",gettext("Загружаем документацию"));
  iceb_printw(strsql,data->buffer,data->view);
  docinw(data->fromdoc.ravno(),data->view,data->buffer,data->window);
 }

/*Записываем дату версии базы данных*/
sprintf(strsql,"replace into Alx (fil,ns,ktoz,vrem,str) values('iceb_data_baz',1,%d,%ld,'%s')",iceb_getuid(data->window),time(NULL),ICEB_DATA_BAZ);
iceb_sql_zapis(strsql,1,0,data->window);


iceb_pbar(data->bar,data->kolstr,++data->kolstr1);    /*Должна быть последней*/



sprintf(strsql,"%s:%.f\n",gettext("Количество таблиц"),data->kolstr1);
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s:%d\n",gettext("Количество ошибок"),kol_oh);
iceb_printw(strsql,data->buffer_error,data->view_error);


iceb_printw_vr(data->vremn,data->buffer,data->view);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Создание базы завершено"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);







return(FALSE);

}

