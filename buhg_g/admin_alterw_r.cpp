/*$Id:$*/
/*15.05.2020	19.02.2012	Белых А.И.	admin_alter_r.c
Преобразование баз для новой версии программ
*/
#include <errno.h>
#include "iceb_libbuh.h"

class admin_alter_r_data
 {
  public:
  

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkWidget *vert_panel;
  GtkTextBuffer *buffer;
  GtkWidget *view_error;
  GtkTextBuffer *buffer_error;
  class iceb_u_str name_window;
  class iceb_u_str name_panel;
  time_t    vremn;
  class iceb_u_str imabaz;
  class iceb_u_str fromnsi;
  class iceb_u_str fromdoc;
  int kolvsehbaz;
  int kol_oh;
  int nombaz;
  //Конструктор  
  admin_alter_r_data()
   {
    kol_oh=0;
    nombaz=0;
   }

 };

gboolean   admin_alter_r_key_press(GtkWidget *widget,GdkEventKey *event,class admin_alter_r_data *data);
gint admin_alter_r1(class admin_alter_r_data *data);
void  admin_alter_r_v_knopka(GtkWidget *widget,class admin_alter_r_data *data);

int admin_cp_alx(const char *from_alx,const char *to_alx,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);
int alxinw(const char *put_alx,int metka,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);
void docinw(const char *put_na_doc,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);

extern SQL_baza bd;
extern char *imabaz;

void admin_alter_r(const char *imabaz,
const char *fromnsi,
const char *fromdoc,
GtkWidget *wpredok)
{
char strsql[1024];
class iceb_u_str soob;
int gor=0;
int ver=0;

admin_alter_r_data data;
data.imabaz.plus(imabaz);
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Преобразование баз для новой версии системы"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(admin_alter_r_key_press),&data);

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

sprintf(strsql,"%s",gettext("Преобразование баз для новой версии системы"));

soob.new_plus(strsql);

GtkWidget *label=gtk_label_new(soob.ravno());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.vert_panel=gtk_paned_new(GTK_ORIENTATION_VERTICAL);
gtk_box_pack_start(GTK_BOX(vbox),data.vert_panel,TRUE,TRUE,0);

sprintf(strsql,"%s-PANEL",data.name_window.ravno());
data.name_panel.plus(strsql);
if(iceb_sizwr(data.name_panel.ravno(),&gor,&ver) == 0)
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


gtk_container_add(GTK_CONTAINER(sw),data.view);


/*второе окно*/

data.view_error=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view_error),FALSE); //Запрет на редактирование текста

data.buffer_error=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view_error));

GtkWidget *sw_error=gtk_scrolled_window_new(NULL,NULL);
//gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw_error),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_paned_add2(GTK_PANED(data.vert_panel),sw_error);

gtk_container_add(GTK_CONTAINER(sw_error),data.view_error);

data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(admin_alter_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);
gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна


g_idle_add((GSourceFunc)admin_alter_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  admin_alter_r_v_knopka(GtkWidget *widget,class admin_alter_r_data *data)
{
iceb_sizww(data->name_window.ravno(),data->window);
iceb_sizw(data->name_panel.ravno(),gtk_paned_get_position(GTK_PANED(data->vert_panel)) ,0,data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   admin_alter_r_key_press(GtkWidget *widget,GdkEventKey *event,class admin_alter_r_data *data)
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

/**********************************/
/*установка даты*/
/*************************************/
void admin_alter_r_ustdat(class admin_alter_r_data *data)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
short d=0,m=0,g=0;
sprintf(strsql,"select god,datd,nomd from Zardok");




if(cur.make_cursor(&bd,strsql) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s %s %s\n",row[0],row[1],row[2]);
  iceb_printw(strsql,data->buffer,data->view);

  iceb_u_rsdat(&d,&m,&g,row[1],2);
  sprintf(strsql,"update Zardok set datsd='%s',god=%d where god='%s' and nomd='%s'",row[1],g,row[0],row[2]);
  
  iceb_sql_zapis(strsql,1,0,data->window);
 }
}


/***************************************/
/*вставка новой колонки в таблицу*/
/***************************************/

int admin_alter_r_vnk(const char *strsql,class admin_alter_r_data *data)
{
char bros[1024];
if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_FIELDNAME)
   {
    sprintf(bros,"%s\n",gettext("Колонкa в таблице уже существуeт"));
    iceb_printw(bros,data->buffer,data->view);
    return(1);
   }
  else
   {
    sprintf(bros,"%s ! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
    iceb_printw(bros,data->buffer_error,data->view_error);
    return(2);
   }
 }

return(0);
}

/******************************/
/*создание таблицы*/
/***************************/
int admin_alter_r_crtb(const char *strsql,const char *table,class admin_alter_r_data *data)
{

char bros[512];

if(sql_zap(&bd,strsql) != 0)
 { 
  if(sql_nerror(&bd) == ER_TABLE_EXISTS_ERROR )
   {
    sprintf(bros,"%s\n\n",gettext("Таблица уже существует"));
    iceb_printw(bros,data->buffer,data->view);
    return(1);
   }  
 else
  {
   sprintf(bros,"%s %s\n",gettext("Таблица"),table);
   iceb_printw(bros,data->buffer_error,data->view_error);

   sprintf(bros,"%d %s\n",sql_nerror(&bd),sql_error(&bd));
   iceb_printw(bros,data->buffer_error,data->view_error);
   data->kol_oh+=1;
   return(1);
  }
 } 
else
 {
  sprintf(bros,"%s\n\n",gettext("Таблица создана"));
  iceb_printw(bros,data->buffer,data->view);
 }
return(0);
}

/*********************************/
/*преобразование таблиц амортизации*/
/**************************************/
int poiinw(int in,short d,short m,short g, int *podr,int *kodotl,GtkWidget *wpedok);

int admin_alter_r_amort(const char *tabl,class admin_alter_r_data *data)
{

char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

class iceb_tmptab tabtmp;
const char *imatmptab={"amort"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
innom   int not null default 0,\
da DATE NOT NULL DEFAULT '0000-00-00',\
podr	smallint not null default 0,\
hzt	varchar(20) not null default '',\
hna	varchar(20) not null default '',\
suma	double (14,2) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kmo INT NOT NULL DEFAULT 0,\
unique(innom,da)) ENGINE = MYISAM",imatmptab);

/****************
sprintf(zaprostmp,"CREATE TABLE %s (\
innom   int not null default 0,\
da DATE NOT NULL DEFAULT '0000-00-00',\
podr	smallint not null default 0,\
hzt	varchar(20) not null default '',\
hna	varchar(20) not null default '',\
suma	double (14,2) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kmo INT NOT NULL DEFAULT 0,\
unique(innom,da)) ENGINE = MYISAM",imatmptab);
sql_zapis(zaprostmp,1,0);
*******************/

if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  return(1);
 }  
sprintf(strsql,"select * from %s order by innom asc",tabl);

if(cur.make_cursor(&bd,strsql) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
 
int innom=0;
short d=1,m=0,g=0;
int podr=0;
int kodmo=0;
while(cur.read_cursor(&row) != 0)
 {
  innom=atoi(row[0]);    

  m=atoi(row[1]);
  g=atoi(row[2]);
    
  poiinw(innom,d,m,g,&podr,&kodmo,data->window);
  podr=atoi(row[3]); /*на всякий случай оставляем подразделение как было*/

  sprintf(strsql,"insert into %s values (%d,'%04d-%02d-%02d',%d,'%s','%s',%s,%s,%s,%d)",imatmptab,innom,g,m,d,podr,row[4],row[5],row[6],row[7],row[8],kodmo);
//  wprintw(win1,"%s\n",strsql);
  iceb_sql_zapis(strsql,1,0,data->window);
//  OSTANOVW(win1);
 }

sprintf(strsql,"drop table %s",tabl);
iceb_sql_zapis(strsql,1,0,data->window);



sprintf(zaprostmp,"CREATE TABLE %s (\
innom   int not null default 0,\
da DATE NOT NULL DEFAULT '0000-00-00',\
podr	smallint not null default 0,\
hzt	varchar(20) not null default '',\
hna	varchar(20) not null default '',\
suma	double (14,2) not null default 0,\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
kmo INT NOT NULL DEFAULT 0,\
unique(innom,da)) ENGINE = MYISAM",tabl);

iceb_sql_zapis(zaprostmp,1,0,data->window);

sprintf(strsql,"INSERT INTO %s SELECT * FROM %s",tabl,imatmptab);

iceb_sql_zapis(strsql,1,0,data->window);

return(0);
}


/*************************************/
/*Вставить новую строку настройки в файл в таблице Alx*/
/********************************************************/

void admin_alter_r_ins_str_alx(const char *imaf,const char *naststr,const char *znahnast,class admin_alter_r_data *data)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
/*Проверяем может такая настройка уже есть*/

sprintf(strsql,"select ns from Alx where fil='%s' and str like '%s|%%'",imaf,naststr);
if(iceb_sql_readkey(strsql,data->window) > 0)
 return;

/*Узнаём номер последней строки*/
sprintf(strsql,"select ns from Alx where fil='%s' order by ns desc limit 1",imaf);
if(iceb_sql_readkey(strsql,&row,&cur,data->window) <= 0)
 return;

sprintf(strsql,"%s %s:\n%s|%s\n",gettext("Вставляем настройку в файл"),imaf,naststr,znahnast);
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"insert into Alx values('%s',%d,%d,%ld,'\n')",
imaf,
atoi(row[0])+1,
getuid(),
time(NULL));

iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"insert into Alx values('%s',%d,%d,%ld,'%s|%s\n')",
imaf,
atoi(row[0])+2,
getuid(),
time(NULL),
naststr,
znahnast);

iceb_sql_zapis(strsql,1,0,data->window);

}
/********************/
/*выполнить запрос*/
/************************/
int admin_alter_sql_zap(const char *strsql,class admin_alter_r_data *data)
{

if(sql_zap(&bd,strsql) != 0)
 {
  data->kol_oh++;
  char bros[1024];
  sprintf(bros,"%s ! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
  iceb_printw(bros,data->buffer_error,data->view_error);
  return(1);
 }
return(0);
}
/**********************************************************/
/***********************************************************/
void admin_alter_r_vnst0(class admin_alter_r_data *data)
{
char strsql[1024];

  sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("пг"),gettext("Норма по городу"),iceb_getuid(data->window),time(NULL));
  admin_alter_sql_zap(strsql,data);

  sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("зг"),gettext("Норма за городом"),iceb_getuid(data->window),time(NULL));
  admin_alter_sql_zap(strsql,data);

  sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("лпг"),gettext("Летняя норма по городу"),iceb_getuid(data->window),time(NULL));
  admin_alter_sql_zap(strsql,data);

  sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("лзг"),gettext("Летняя норма за городом"),iceb_getuid(data->window),time(NULL));
  admin_alter_sql_zap(strsql,data);

  sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("зпг"),gettext("Зимняя норма по городу"),iceb_getuid(data->window),time(NULL));
  admin_alter_sql_zap(strsql,data);

  sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("ззг"),gettext("Зимняя норма за городом"),iceb_getuid(data->window),time(NULL));
  admin_alter_sql_zap(strsql,data);

  sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("пегр"),gettext("Перевозка груза"),iceb_getuid(data->window),time(NULL));
  admin_alter_sql_zap(strsql,data);

  sprintf(strsql,"insert into Uplnst values('%s','%s',%d,%ld)",gettext("рд"),gettext("Работа двигателя"),iceb_getuid(data->window),time(NULL));
  admin_alter_sql_zap(strsql,data);


}





/*********************************************/
/*вставка норм списания топлива в новую таблицу*/
/***********************************************/
void admin_alter_r_vnst(class admin_alter_r_data *data)
{

char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;


/*вставляем нужные единицы измерения*/
sprintf(strsql,"select kod from Edizmer where kod='%s'",gettext("Км"));
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"insert into Edizmer values('%s','%s',%d,%ld)",gettext("Км"),gettext("Киллометры"),iceb_getuid(data->window),time(NULL));
  admin_alter_sql_zap(strsql,data);
 }

sprintf(strsql,"select kod from Edizmer where kod='%s'",gettext("Ткм"));
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"insert into Edizmer values('%s','%s',%d,%ld)",gettext("Ткм"),gettext("Тонно-киллометры"),iceb_getuid(data->window),time(NULL));
  admin_alter_sql_zap(strsql,data);
 }

sprintf(strsql,"select kod from Edizmer where kod='%s'",gettext("Мч"));
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"insert into Edizmer values('%s','%s',%d,%ld)",gettext("Мч"),gettext("Моточасы"),iceb_getuid(data->window),time(NULL));
  admin_alter_sql_zap(strsql,data);
 }

sprintf(strsql,"select kod,lnr,znr,lnrzg,znrzg,nrtk,nrmh from Uplavt");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

if(kolstr == 0)
 {
  return;
 }

/***********
лпг
зпг
лзг
ззг
пeгр
рд
*************/

while(cur.read_cursor(&row) != 0)
 {
  if(atof(row[1]) != 0.)
   {
    sprintf(strsql,"insert into Uplavt1 values(%s,'%s',%.3f,'%s',%d,%ld)",row[0],gettext("лпг"),atof(row[1]),gettext("Км"),iceb_getuid(data->window),time(NULL));
    admin_alter_sql_zap(strsql,data);
   }
  if(atof(row[2]) != 0.)
   {
    sprintf(strsql,"insert into Uplavt1 values(%s,'%s',%.3f,'%s',%d,%ld)",row[0],gettext("зпг"),atof(row[2]),gettext("Км"),iceb_getuid(data->window),time(NULL));
    admin_alter_sql_zap(strsql,data);
   }
  if(atof(row[3]) != 0.)
   {
    sprintf(strsql,"insert into Uplavt1 values(%s,'%s',%.3f,'%s',%d,%ld)",row[0],gettext("лзг"),atof(row[3]),gettext("Км"),iceb_getuid(data->window),time(NULL));
    admin_alter_sql_zap(strsql,data);
   }
  if(atof(row[4]) != 0.)
   {
    sprintf(strsql,"insert into Uplavt1 values(%s,'%s',%.3f,'%s',%d,%ld)",row[0],gettext("ззг"),atof(row[4]),gettext("Км"),iceb_getuid(data->window),time(NULL));
    admin_alter_sql_zap(strsql,data);
   }
  if(atof(row[5]) != 0.)
   {
    sprintf(strsql,"insert into Uplavt1 values(%s,'%s',%.3f,'%s',%d,%ld)",row[0],gettext("пегр"),atof(row[5]),gettext("Ткм"),iceb_getuid(data->window),time(NULL));
    admin_alter_sql_zap(strsql,data);
   }
  if(atof(row[6]) != 0.)
   {
    sprintf(strsql,"insert into Uplavt1 values(%s,'%s',%.3f,'%s',%d,%ld)",row[0],gettext("рд"),atof(row[6])*100.,gettext("Мч"),iceb_getuid(data->window),time(NULL));
    admin_alter_sql_zap(strsql,data);
   }


 }


}
/***************************************************/
/*переписать в таблицу нормы которые были в шапке путевого листа*/
/**************************************************/

void admin_alter_r_vnstpl(class admin_alter_r_data *data)
{

char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;

sprintf(strsql,"select datd,kp,nomd,ka,ztpn,ztfa,prob,nst,ztpnzg,ztfazg,probzg,nstzg,vesg,ztvsn,ztvsf,vrr,ztvrn,ztvrf,tk from Upldok");
/*                      0   1   2   3   4    5    6    7    8      9     10     11    12   13    14   15   16    17   18 */
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
if(kolstr == 0)
  return;
while(cur.read_cursor(&row) != 0)
 {
  /*так как в шапке не указано летняя или зимняя норма то всё записываем на летнюю*/
  
  if(atof(row[4]) != 0. || atof(row[5]) != 0. || atof(row[6]) != 0.)
   {
    sprintf(strsql,"insert into Upldok3 values('%s',%s,'%s','%s',%.3f,%.3f,%.3f,%.3f,'%s','',%d,%ld)",
    row[0],row[1],row[2],
    gettext("пг"),
    atof(row[6]),
    atof(row[7]),
    atof(row[4]),
    atof(row[5]),
    gettext("Км"),
    iceb_getuid(data->window),
    time(NULL));    

    admin_alter_sql_zap(strsql,data);
   }

  if(atof(row[8]) != 0. || atof(row[9]) != 0. || atof(row[10]) != 0.)
   {
    sprintf(strsql,"insert into Upldok3 values('%s',%s,'%s','%s',%.3f,%.3f,%.3f,%.3f,'%s','',%d,%ld)",
    row[0],row[1],row[2],
    gettext("зг"),
    atof(row[10]),
    atof(row[11]),
    atof(row[8]),
    atof(row[9]),
    gettext("Км"),
    iceb_getuid(data->window),
    time(NULL));    

    admin_alter_sql_zap(strsql,data);
   }

  if(atof(row[18]) != 0. || atof(row[13]) != 0. || atof(row[14]) != 0.)
   {
    sprintf(strsql,"insert into Upldok3 values('%s',%s,'%s','%s',%.3f,%.3f,%.3f,%.3f,'%s','',%d,%ld)",
    row[0],row[1],row[2],
    gettext("пегр"),
    atof(row[18]),
    atof(row[13]),
    atof(row[14]),
    atof(row[14]),
    gettext("Ткм"),
    iceb_getuid(data->window),
    time(NULL));    

    admin_alter_sql_zap(strsql,data);
   }

  if(atof(row[15]) != 0. || atof(row[16]) != 0. || atof(row[17]) != 0.)
   {
    char ves[512];
    memset(ves,'\0',sizeof(ves));
    if(atof(row[12]) != 0.)
      sprintf(ves,"%s:%s",gettext("Вес"),row[12]);

    sprintf(strsql,"insert into Upldok3 values('%s',%s,'%s','%s',%.3f,%.3f,%.3f,%.3f,'%s','%s',%d,%ld)",
    row[0],row[1],row[2],
    gettext("рд"),
    atof(row[15]),
    atof(row[16]),
    atof(row[17]),
    atof(row[17]),
    gettext("Мч"),
    ves,        
    iceb_getuid(data->window),
    time(NULL));    

    admin_alter_sql_zap(strsql,data);
   }
 }

}
/********************************/
/*заполнение новой таблицы списания топлива по счетам*/
/******************************************************/
void admin_alter_r_upldok2a(class admin_alter_r_data *data)
{
char strsql[1024];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;
int kolstr1=0;
int nomer=0;
class iceb_u_str kod_top;
class iceb_u_str shet;
double zatr_top=0.;

//sprintf(strsql,"select datd,kp,nomd from Upldok");
sprintf(strsql,"select * from Upldok");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
if(kolstr == 0)
  return;

while(cur.read_cursor(&row) != 0)
 {
  class iceb_u_spisok kodtop_shet;
  class iceb_u_double kolih_top;
  class iceb_u_double zatpn; //*Затраты по норме
//  sprintf(strsql,"select kt,shet,zt,ztzg,ztgr,ztmh from Upldok2 where datd='%s' and kp=%s and nomd='%s'",row[0],row[1],row[2]);
  sprintf(strsql,"select * from Upldok2 where datd='%s' and kp=%s and nomd='%s'",row[0],row[1],row[2]);

  if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return;
   }
  if(kolstr1 == 0)
    continue;
  double zatr_pn=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    sprintf(strsql,"%s|%s",row1[3],row1[4]);
    if((nomer=kodtop_shet.find(strsql)) < 0)
     kodtop_shet.plus(strsql);

    zatr_top=atof(row1[7])+atof(row1[14])+atof(row1[15])+atof(row1[16]);
    zatr_pn=atof(row1[6])*atof(row[19])/100.+atof(row1[13])*atof(row[25])/100.+atof(row1[17])*atof(row[27])/100.+atof(row1[18])*atof(row[30]);

    kolih_top.plus(zatr_top,nomer);
    zatpn.plus(zatr_pn,nomer);
   }  
  for(nomer=0; nomer < kodtop_shet.kolih(); nomer++)
   {  
    iceb_u_polen(kodtop_shet.ravno(nomer),&kod_top,1,'|');
    iceb_u_polen(kodtop_shet.ravno(nomer),&shet,2,'|');
     
    sprintf(strsql,"insert into Upldok2a values('%s',%s,'%s','%s','%s',%.3f,%.3f,'',%d,%ld)",
    row[0],row[1],row[2],
    kod_top.ravno_filtr(),
    shet.ravno_filtr(),
    kolih_top.ravno(nomer),
    zatpn.ravno(nomer),
    iceb_getuid(data->window),
    time(NULL));

    iceb_sql_zapis(strsql,1,0,data->window);   

   }  
 }
}



/***************************************/
/*замена приставки к табельному номеру*/
/*************************************/
void admin_alter_r_zpktn(class admin_alter_r_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str pris_k_tn("");
int kolstr=0;

if(iceb_poldan("Приставка к табельному номеру",&pris_k_tn,"zarnast.alx",data->window) != 0)
  return;

sprintf(strsql,"Приставка к табельному номеру=%s\n",pris_k_tn.ravno());


iceb_printw(strsql,data->buffer,data->view);

if(iceb_u_SRAV(ICEB_ZAR_PKTN,pris_k_tn.ravno(),0) == 0)
 return;

sprintf(strsql,"%s %s на %s\n",gettext("Меняем приставку к табельному номеру"),pris_k_tn.ravno(),ICEB_ZAR_PKTN);

iceb_printw(strsql,data->buffer,data->view);
 
sprintf(strsql,"select kodkon from Kontragent where kodkon like '%s%%'",pris_k_tn.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  return;
 }

class iceb_u_str kod_kontr_new("");

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s -> %s%s\n",row[0],ICEB_ZAR_PKTN,iceb_u_adrsimv(iceb_u_strlen(pris_k_tn.ravno()),row[0]));
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"%s%s",ICEB_ZAR_PKTN,iceb_u_adrsimv(iceb_u_strlen(pris_k_tn.ravno()),row[0]));
  kod_kontr_new.new_plus(strsql);
  
  sprintf(strsql,"update Kontragent \
set \
kodkon='%s',\
ktoz=%d,\
vrem=%ld \
where kodkon='%s'",
  kod_kontr_new.ravno_filtr(),
  iceb_getuid(data->window),time(NULL),
  row[0]);

  iceb_sql_zapis(strsql,1,0,data->window);

  sprintf(strsql,"update Skontr \
set \
kodkon='%s',\
ktoi=%d,\
vrem=%ld \
where kodkon='%s'",
  kod_kontr_new.ravno_filtr(),
  iceb_getuid(data->window),time(NULL),
  row[0]);

  iceb_sql_zapis(strsql,1,0,data->window);

  iceb_kor_kod_kontr(row[0],kod_kontr_new.ravno(),data->window);
  
 }

/*есть проводки в которых есть код контрагента и его нет в списке контрагентов*/

sprintf(strsql,"select distinct kodkon from Prov where kodkon like '%s%%'",pris_k_tn.ravno());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {

  if(iceb_u_SRAV("тн",row[0],1) == 0)
   continue;
   
  sprintf(strsql,"%s -> ",row[0]);
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"%s%s",ICEB_ZAR_PKTN,iceb_u_adrsimv(iceb_u_strlen(pris_k_tn.ravno()),row[0]));
 
  iceb_printw(strsql,data->buffer,data->view);
  iceb_printw("\n",data->buffer,data->view);
  
  kod_kontr_new.new_plus(strsql);
  
  sprintf(strsql,"update Prov set kodkon='%s' where kodkon='%s'",kod_kontr_new.ravno(),row[0]);
  iceb_sql_zapis(strsql,1,0,data->window);
 }

}
/*****************************************/
/*смена кода контрагента в проводках может быть в проводке и не быть в общем списке*/
/****************************************/
void admin_alter_r_skkvp(class admin_alter_r_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str pris_k_tn("ТН");
int kolstr=0;

sprintf(strsql,"select distinct kodkon from Prov where kodkon like 'ТН%%'");
class iceb_u_str kod_kontr_new("");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {

  if(iceb_u_SRAV("тн",row[0],1) == 0)
   continue;
   
  sprintf(strsql,"%s -> ",row[0]);
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"%s%s",ICEB_ZAR_PKTN,iceb_u_adrsimv(iceb_u_strlen(pris_k_tn.ravno()),row[0]));
 
  iceb_printw(strsql,data->buffer,data->view);

  iceb_printw("\n",data->buffer,data->view);
  
  kod_kontr_new.new_plus(strsql);
  
  sprintf(strsql,"update Prov set kodkon='%s' where kodkon='%s'",kod_kontr_new.ravno(),row[0]);
  iceb_sql_zapis(strsql,1,0,data->window);
 }

}

/********************************/
/*загрузка таблицы должностных окладов*/
/*********************************/
void admin_alter_r_zdo(class admin_alter_r_data *data)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str kod_do(""); /*код дожностного оклада*/
class iceb_u_str tabnom("");
class iceb_u_str oklad("");
class iceb_u_str hasi("");
class iceb_u_str koef("");
class iceb_u_str shet("");
class iceb_u_str bros("");
int metka_sp=0;

short mt=1,gt=2016;



sprintf(strsql,"select str from Alx where fil='zardolhn.alx' order by ns asc");




if(cur.make_cursor(&bd,strsql) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  metka_sp=1; /*месячный расчёт по часам*/
  
  if(iceb_u_SRAV("Код должносного оклада|",row[0],1) == 0)
   {
    iceb_u_polen(row[0],&kod_do,2,'|');
   }

  if(iceb_u_polen(row[0],&tabnom,1,'|') != 0)
   continue;

  if(tabnom.ravno_atoi() == 0)
   continue;  

  iceb_u_polen(row[0],&oklad,2,'|');
  
  if(oklad.ravno_atof() == 0.)
   continue;

 if(iceb_u_strstrm(oklad.ravno(),"Ч") == 1)
   metka_sp=3; /*почасовая ставка*/
 if(iceb_u_strstrm(oklad.ravno(),"Ч") == 1)
   metka_sp=4; /*дневная ставка*/
 if(iceb_u_strstrm(oklad.ravno(),"r") == 1)
   metka_sp=5; /*тариф*/

  iceb_u_polen(row[0],&bros,4,'|');
  if(iceb_u_strstrm(bros.ravno(),"Д") == 1)
   metka_sp=0; /*месячный по дням*/
   
  iceb_u_polen(row[0],&shet,5,'|');
  
  iceb_u_polen(row[0],&bros,3,'|');
  
  if(bros.ravno()[0] == '*')
   metka_sp=2;  /*месячный без расчёта*/
  else  
   if(iceb_u_strstrm(bros.ravno(),"*") == 1)
    {
     iceb_u_polen(bros.ravno(),&hasi,1,'*');
     iceb_u_polen(bros.ravno(),&koef,2,'*');
    } 
   else
    {
     hasi.new_plus(bros.ravno());
     koef.new_plus("1.");
    }

  sprintf(strsql,"insert into Zarsdo values(%d,'%04d-%02d-01',%d,%.2f,%d,%.2f,%.2f,'%s','%s',%d,%ld)",
  tabnom.ravno_atoi(),
  gt,mt,
  kod_do.ravno_atoi(),
  oklad.ravno_atof(),
  metka_sp,
  hasi.ravno_atof(),
  koef.ravno_atof(),
  shet.ravno_filtr(),
  "",
  iceb_getuid(data->window),
  time(NULL));

  iceb_sql_zapis(strsql,1,0,data->window);
 }  




}



/************************************************/
/*Преобразованяе конкретной базы*/
/*****************************************/
int admin_alter_r_preobr(class admin_alter_r_data *data)
{
char strsql[4096];
class iceb_u_str bros("");
/*имя базы используется в подпрограммах глобально*/
if(imabaz != NULL)
 delete [] imabaz;

imabaz=new char[strlen(data->imabaz.ravno())+1];
strcpy(imabaz,data->imabaz.ravno());

sprintf(strsql,"USE %s",data->imabaz.ravno());
if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
 return(1);

iceb_start_rf(); //чтение и выполнение запросов к базе

sprintf(strsql,"\n%s N%d %s %s:%d\n",
gettext("Преобразование базы"),
++data->nombaz,data->imabaz.ravno(),
gettext("Количество всех баз"),
data->kolvsehbaz);

iceb_printw(strsql,data->buffer_error,data->view_error);
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"-----------------------------------------------------------------------\n");

iceb_printw(strsql,data->buffer_error,data->view_error);
iceb_printw(strsql,data->buffer,data->view);

SQL_str row;
class SQLCURSOR cur;

/*проверяем дату последненего обновления базы и перепрыгиваем через ненужный код*/
sprintf(strsql,"select str from Alx where fil='iceb_data_baz' and ns=1");
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
//  printf("%s-%s\n",__FUNCTION__,row[0]);
  if(iceb_u_SRAV("13.8.2012",row[0],0) == 0)
   goto jump13082012;
  if(iceb_u_SRAV("05.09.2012",row[0],0) == 0)
   goto jump05092012;
  if(iceb_u_SRAV("06.11.2012",row[0],0) == 0)
   goto jump06112012;
  if(iceb_u_SRAV("28.11.2012",row[0],0) == 0)
   goto jump28112012;
  if(iceb_u_SRAV("30.11.2012",row[0],0) == 0)
   goto jump30112012;
  if(iceb_u_SRAV("05.12.2012",row[0],0) == 0)
   goto jump05122012;
  if(iceb_u_SRAV("02.01.2013",row[0],0) == 0)
   goto jump02012013;
  if(iceb_u_SRAV("11.01.2013",row[0],0) == 0)
   goto jump11012013;
  if(iceb_u_SRAV("11.02.2013",row[0],0) == 0)
   goto jump11022013;
  if(iceb_u_SRAV("06.03.2013",row[0],0) == 0)
   goto jump06032013;
  if(iceb_u_SRAV("05.04.2013",row[0],0) == 0)
   goto jump05042013;
  if(iceb_u_SRAV("21.06.2013",row[0],0) == 0)
   goto jump21062013;
  if(iceb_u_SRAV("04.09.2013",row[0],0) == 0)
   goto jump04092013;
  if(iceb_u_SRAV("18.09.2013",row[0],0) == 0)
   goto jump18092013;
  if(iceb_u_SRAV("21.09.2013",row[0],0) == 0)
   goto jump21092013;
  if(iceb_u_SRAV("28.09.2013",row[0],0) == 0)
   goto jump28092013;
  if(iceb_u_SRAV("31.10.2013",row[0],0) == 0)
   goto jump31102013;
  if(iceb_u_SRAV("24.11.2013",row[0],0) == 0)
   goto jump24112013;
  if(iceb_u_SRAV("27.12.2013",row[0],0) == 0)
   goto jump27122013;
  if(iceb_u_SRAV("10.01.2014",row[0],0) == 0)
   goto jump10012014;
  if(iceb_u_SRAV("29.01.2014",row[0],0) == 0)
   goto jump29012014;
  if(iceb_u_SRAV("20.02.2014",row[0],0) == 0)
   goto jump20022014;
  if(iceb_u_SRAV("25.02.2014",row[0],0) == 0)
   goto jump25022014;
  if(iceb_u_SRAV("12.03.2014",row[0],0) == 0)
   goto jump12032014;
  if(iceb_u_SRAV("08.04.2014",row[0],0) == 0)
   goto jump08042014;
  if(iceb_u_SRAV("01.05.2014",row[0],0) == 0)
   goto jump01052014;
  if(iceb_u_SRAV("14.05.2014",row[0],0) == 0)
   goto jump14052014;
  if(iceb_u_SRAV("30.05.2014",row[0],0) == 0)
   goto jump30052014;
  if(iceb_u_SRAV("03.06.2014",row[0],0) == 0)
   goto jump03062014;
  if(iceb_u_SRAV("03.07.2014",row[0],0) == 0)
   goto jump03072014;
  if(iceb_u_SRAV("19.07.2014",row[0],0) == 0)
   goto jump19072014;
  if(iceb_u_SRAV("31.07.2014",row[0],0) == 0)
   goto jump31072014;
  if(iceb_u_SRAV("12.08.2014",row[0],0) == 0)
   goto jump12082014;
  if(iceb_u_SRAV("28.08.2014",row[0],0) == 0)
   goto jump28082014;
  if(iceb_u_SRAV("27.11.2014",row[0],0) == 0)
   goto jump27112014;
  if(iceb_u_SRAV("26.01.2015",row[0],0) == 0)
   goto jump26012015;
  if(iceb_u_SRAV("01.02.2015",row[0],0) == 0)
   goto jump01022015;
  if(iceb_u_SRAV("09.02.2015",row[0],0) == 0)
   goto jump09022015;
  if(iceb_u_SRAV("02.03.2015",row[0],0) == 0)
   goto jump02032015;
  if(iceb_u_SRAV("21.04.2015",row[0],0) == 0)
   goto jump21042015;
  if(iceb_u_SRAV("09.05.2015",row[0],0) == 0)
   goto jump09052015;
  if(iceb_u_SRAV("03.07.2015",row[0],0) == 0)
   goto jump03072015;
  if(iceb_u_SRAV("28.07.2015",row[0],0) == 0)
   goto jump28072015;
  if(iceb_u_SRAV("01.09.2015",row[0],0) == 0)
   goto jump01092015;
  if(iceb_u_SRAV("07.09.2015",row[0],0) == 0)
   goto jump07092015;
  if(iceb_u_SRAV("22.09.2015",row[0],0) == 0)
   goto jump22092015;
  if(iceb_u_SRAV("02.10.2015",row[0],0) == 0)
   goto jump02102015;
  if(iceb_u_SRAV("03.12.2015",row[0],0) == 0)
   goto jump03122015;
  if(iceb_u_SRAV("12.01.2016",row[0],0) == 0)
   goto jump12012016;
  if(iceb_u_SRAV("18.01.2016",row[0],0) == 0)
   goto jump18012016;
  if(iceb_u_SRAV("04.02.2016",row[0],0) == 0)
   goto jump04022016;
  if(iceb_u_SRAV("08.02.2016",row[0],0) == 0)
   goto jump08022016;
  if(iceb_u_SRAV("24.02.2016",row[0],0) == 0)
   goto jump24022016;
  if(iceb_u_SRAV("09.03.2016",row[0],0) == 0)
   goto jump09032016;
  if(iceb_u_SRAV("16.03.2016",row[0],0) == 0)
   goto jump16032016;
  if(iceb_u_SRAV("30.03.2016",row[0],0) == 0)
   goto jump30032016;
  if(iceb_u_SRAV("01.04.2016",row[0],0) == 0)
   goto jump01042016;
  if(iceb_u_SRAV("25.05.2016",row[0],0) == 0)
   goto jump25052016;
  if(iceb_u_SRAV("31.05.2016",row[0],0) == 0)
   goto jump31052016;
  if(iceb_u_SRAV("24.06.2016",row[0],0) == 0)
   goto jump24062016;
  if(iceb_u_SRAV("16.11.2016",row[0],0) == 0)
   goto jump16112016;
  if(iceb_u_SRAV("22.11.2016",row[0],0) == 0)
   goto jump22112016;
  if(iceb_u_SRAV("24.11.2016",row[0],0) == 0)
   goto jump24112016;
  if(iceb_u_SRAV("13.02.2017",row[0],0) == 0)
   goto jump13022017;
  if(iceb_u_SRAV("24.02.2017",row[0],0) == 0)
   goto jump24022017;
  if(iceb_u_SRAV("06.03.2017",row[0],0) == 0)
   goto jump06032017;
  if(iceb_u_SRAV("22.03.2017",row[0],0) == 0)
   goto jump22032017;
  if(iceb_u_SRAV("18.05.2017",row[0],0) == 0)
   goto jump18052017;
  if(iceb_u_SRAV("04.09.2017",row[0],0) == 0)
   goto jump04092017;
  if(iceb_u_SRAV("26.09.2017",row[0],0) == 0)
   goto jump26092017;
  if(iceb_u_SRAV("09.01.2018",row[0],0) == 0)
   goto jump09012018;
  if(iceb_u_SRAV("10.01.2018",row[0],0) == 0)
   goto jump10012018;
  if(iceb_u_SRAV("27.04.2018",row[0],0) == 0)
   goto jump27042018;
  if(iceb_u_SRAV("22.07.2019",row[0],0) == 0)
   goto jump22072019;
  if(iceb_u_SRAV("06.08.2019",row[0],0) == 0)
   goto jump06082019;
  if(iceb_u_SRAV("30.08.2019",row[0],0) == 0)
   goto jump30082019;
  if(iceb_u_SRAV("10.01.2020",row[0],0) == 0)
   goto jump10012020;
  if(iceb_u_SRAV("07.03.2020",row[0],0) == 0)
   goto jump07032020;
  if(iceb_u_SRAV("15.05.2020",row[0],0) == 0)
   goto jump15052020;
 }







/****************************************/
/*********** 7.18 05.01.2012 ************/
/****************************************/

sprintf(strsql,"%s md %s Uosin1\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Uosin1 add md tinyint not null default 0");

if(admin_alter_r_vnk(strsql,data) == 0)
 {
  sprintf(strsql,"alter table Uosin1 change zapis zapis TEXT not null default ''");
  iceb_sql_zapis(strsql,1,0,data->window);

  sprintf(strsql,"alter table Uosin1 drop index innom");
  iceb_sql_zapis(strsql,1,0,data->window);

  sprintf(strsql,"alter table Uosin1 add unique (innom,md,nomz)");
  iceb_sql_zapis(strsql,1,0,data->window);
  
  sprintf(strsql,"select * from Uosin1");
  
  SQL_str row;
  class SQLCURSOR cur;
  class iceb_u_str zapis("");  
  if(cur.make_cursor(&bd,strsql) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  else
   while(cur.read_cursor(&row) != 0)
    {
     zapis.new_plus(row[2]);
     zapis.plus("\n");
     sprintf(strsql,"update Uosin1 set zapis='%s' where innom=%s and md=%s and nomz=%s",zapis.ravno_filtr(),row[0],row[3],row[1]);
     iceb_sql_zapis(strsql,1,0,data->window);
    }  

  sprintf(strsql,"%s/uoskar.alx",data->fromnsi.ravno());
  unlink(strsql);

  iceb_sql_zapis("delete from Alx where fil='uoskar.alx'",1,0,data->window);

  iceb_sql_zapis("delete from Alx where fil='nn4_hap.alx'",1,0,data->window); /*обновляем внесены исправления*/
  iceb_sql_zapis("delete from Alx where fil='zarsvfz_end.alx'",1,0,data->window); /*обновляем внесены исправления*/

  admin_alter_r_ins_str_alx("matnast.alx","Дата отгрузки в налоговой накладной без разделительных точек","Выключено",data);

  /****************************************вложили сюда чтобы отработало только один раз*/
  /*********** 12.19 13.01.2012 ************/
  /****************************************/
  int razmer_otstupa=0;
  if(iceb_poldan("Отступ от левого края",&razmer_otstupa,"matnast.alx",data->window) == 0)
   {
    if(razmer_otstupa > 5)
     {
      sprintf(strsql,"update Alx set str='Отступ от левого края|5\n' where fil='matnast.alx' and str like 'Отступ от левого края|%%'");
      admin_alter_sql_zap(strsql,data);
     }
   } 

  if(iceb_poldan("Отступ налоговой накладной",&razmer_otstupa,"matnast.alx",data->window) == 0)
   {
    if(razmer_otstupa > 5)
     {
      sprintf(strsql,"update Alx set str='Отступ налоговой накладной|5\n' where fil='matnast.alx' and str like 'Отступ налоговой накладной|%%'");
      admin_alter_sql_zap(strsql,data);
     }
   } 

  if(iceb_poldan("Отступ от левого края",&razmer_otstupa,"uslnast.alx",data->window) == 0)
   {
    if(razmer_otstupa > 2)
     {
      sprintf(strsql,"update Alx set str='Отступ от левого края|2\n' where fil='uslnast.alx' and str like 'Отступ от левого края|%%'");
      admin_alter_sql_zap(strsql,data);
     }
   } 

  if(iceb_poldan("Отступ от левого края",&razmer_otstupa,"nastdok.alx",data->window) == 0)
   {
    if(razmer_otstupa > 4)
     {
      sprintf(strsql,"update Alx set str='Отступ от левого края|4\n' where fil='nastdok.alx' and str like 'Отступ от левого края|%%'");
      admin_alter_sql_zap(strsql,data);
     }
   } 

  admin_alter_r_ins_str_alx("nastsys.alx","Количество строк на А4 с уменьшенным межстрочным расстоянием для системы с графическим интерфейсом","76",data);
  admin_alter_r_ins_str_alx("nastsys.alx","Количество строк на А4 с уменьшенным межстрочным расстоянием в ориентации ландшафт для системы с графическим интерфейсом","56",data);

 }





/****************************************/
/*********** 7.22 20.02.2012 ************/
/****************************************/

sprintf(strsql,"%s en %s Kontragent\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"alter table Kontragent add en tinyint not null default 0");

if(admin_alter_r_vnk(strsql,data) == 0)
 {


  sprintf(strsql,"%s nais %s Plansh\n",gettext("Увеличиваем длинну колонки"),gettext("в таблице"));
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"alter table Plansh change nais nais varchar(255) not null default ''");

  admin_alter_sql_zap(strsql,data);

 }







/****************************************/
/*********** 7.18 05.01.2012 ************/
/****************************************/

sprintf(strsql,"%s md %s Uosin1\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Uosin1 add md tinyint not null default 0");

if(admin_alter_r_vnk(strsql,data) == 0)
 {
  sprintf(strsql,"alter table Uosin1 change zapis zapis TEXT not null default ''");
  iceb_sql_zapis(strsql,1,0,data->window);

  sprintf(strsql,"alter table Uosin1 drop index innom");
  iceb_sql_zapis(strsql,1,0,data->window);

  sprintf(strsql,"alter table Uosin1 add unique (innom,md,nomz)");
  iceb_sql_zapis(strsql,1,0,data->window);
  
  sprintf(strsql,"select * from Uosin1");
  
  SQL_str row;
  class SQLCURSOR cur;
  class iceb_u_str zapis("");  
  if(cur.make_cursor(&bd,strsql) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  else
   while(cur.read_cursor(&row) != 0)
    {
     zapis.new_plus(row[2]);
     zapis.plus("\n");
     sprintf(strsql,"update Uosin1 set zapis='%s' where innom=%s and md=%s and nomz=%s",zapis.ravno_filtr(),row[0],row[3],row[1]);
     iceb_sql_zapis(strsql,1,0,data->window);
    }  

  sprintf(strsql,"%s/uoskar.alx",data->fromnsi.ravno());
  unlink(strsql);

  iceb_sql_zapis("delete from Alx where fil='uoskar.alx'",1,0,data->window);

  iceb_sql_zapis("delete from Alx where fil='nn4_hap.alx'",1,0,data->window); /*обновляем внесены исправления*/
  iceb_sql_zapis("delete from Alx where fil='zarsvfz_end.alx'",1,0,data->window); /*обновляем внесены исправления*/

  admin_alter_r_ins_str_alx("matnast.alx","Дата отгрузки в налоговой накладной без разделительных точек","Выключено",data);

  /****************************************вложили сюда чтобы отработало только один раз*/
  /*********** 12.19 13.01.2012 ************/
  /****************************************/
  int razmer_otstupa=0;
  if(iceb_poldan("Отступ от левого края",&razmer_otstupa,"matnast.alx",data->window) == 0)
   {
    if(razmer_otstupa > 5)
     {
      sprintf(strsql,"update Alx set str='Отступ от левого края|5\n' where fil='matnast.alx' and str like 'Отступ от левого края|%%'");
      admin_alter_sql_zap(strsql,data);
     }
   } 

  if(iceb_poldan("Отступ налоговой накладной",&razmer_otstupa,"matnast.alx",data->window) == 0)
   {
    if(razmer_otstupa > 5)
     {
      sprintf(strsql,"update Alx set str='Отступ налоговой накладной|5\n' where fil='matnast.alx' and str like 'Отступ налоговой накладной|%%'");
      admin_alter_sql_zap(strsql,data);
     }
   } 

  if(iceb_poldan("Отступ от левого края",&razmer_otstupa,"uslnast.alx",data->window) == 0)
   {
    if(razmer_otstupa > 2)
     {
      sprintf(strsql,"update Alx set str='Отступ от левого края|2\n' where fil='uslnast.alx' and str like 'Отступ от левого края|%%'");
      admin_alter_sql_zap(strsql,data);
     }
   } 

  if(iceb_poldan("Отступ от левого края",&razmer_otstupa,"nastdok.alx",data->window) == 0)
   {
    if(razmer_otstupa > 4)
     {
      sprintf(strsql,"update Alx set str='Отступ от левого края|4\n' where fil='nastdok.alx' and str like 'Отступ от левого края|%%'");
      admin_alter_sql_zap(strsql,data);
     }
   } 

  admin_alter_r_ins_str_alx("nastsys.alx","Количество строк на А4 с уменьшенным межстрочным расстоянием для системы с графическим интерфейсом","76",data);
  admin_alter_r_ins_str_alx("nastsys.alx","Количество строк на А4 с уменьшенным межстрочным расстоянием в ориентации ландшафт для системы с графическим интерфейсом","56",data);

 }





/****************************************/
/*********** 7.22 20.02.2012 ************/
/****************************************/

sprintf(strsql,"%s en %s Kontragent\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"alter table Kontragent add en tinyint not null default 0");

if(admin_alter_r_vnk(strsql,data) == 0)
 {


  sprintf(strsql,"%s nais %s Plansh\n",gettext("Увеличиваем длинну колонки"),gettext("в таблице"));
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"alter table Plansh change nais nais varchar(255) not null default ''");
  admin_alter_sql_zap(strsql,data);


  sprintf(strsql,"%s dokum %s Kasord\n",gettext("Увеличиваем длинну колонки"),gettext("в таблице"));
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"alter table Kasord change dokum dokum varchar(255) not null default ''");
  admin_alter_sql_zap(strsql,data);

  admin_alter_sql_zap("delete from Alx where fil='nn4_start.alx'",data); /*обновляем внесены исправления*/

  sprintf(strsql,"update Kontragent set en=1 where regnom <> ''");
  admin_alter_sql_zap(strsql,data);

  sprintf(strsql,"%s naik %s Sklad\n",gettext("Увеличиваем длинну колонки"),gettext("в таблице"));
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"alter table Sklad change naik naik varchar(124) not null default ''");
  admin_alter_sql_zap(strsql,data);

  sprintf(strsql,"%s fmol %s Sklad\n",gettext("Увеличиваем длинну колонки"),gettext("в таблице"));
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"alter table Sklad change fmol fmol varchar(255) not null default ''");
  admin_alter_sql_zap(strsql,data);


  sprintf(strsql,"%s dolg %s Sklad\n",gettext("Увеличиваем длинну колонки"),gettext("в таблице"));
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"alter table Sklad change dolg dolg varchar(124) not null default ''");
  admin_alter_sql_zap(strsql,data);
 }


/****************************************/
/*********** 7.23 06.05.2012 ************/
/****************************************/


sprintf(strsql,"%s Uplnst\n",gettext("Создание таблицы норм списания топлива"));
iceb_printw(strsql,data->buffer,data->view);


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

if(admin_alter_r_crtb(strsql,"Uplnst",data) == 0)
 {
  admin_alter_r_vnst0(data);

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
  3  ktoz  кто записал
  4  vrem  время записи
  */
  if(admin_alter_r_crtb(strsql,"Uplavt1",data) == 0)
    admin_alter_r_vnst(data);


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
  9 kom  коментарий
  10  ktoz  кто записал
  11 vrem  время записи
  */
  if(admin_alter_r_crtb(strsql,"Upldok3",data) == 0)
    admin_alter_r_vnstpl(data);

  sprintf(strsql,"%s Upldok2a\n",gettext("Создание таблицы записей списания топлива по счетам"));
  iceb_printw(strsql,data->buffer,data->view);

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
  5 zt    затраты топлива
  6 zpn  затраты по норме
  7 koment коментарий
  8 ktoz  кто записал
  9 vrem  время записи
  */

  if(admin_alter_r_crtb(strsql,"Upldok2a",data) == 0)
    admin_alter_r_upldok2a(data);

  sprintf(strsql,"delete from Alx where fil='uplnast.alx' and str like 'Летняя норма расхода топлива|%%'");
  admin_alter_sql_zap(strsql,data);

  sprintf(strsql,"%s Uplost\n",gettext("Удаляем таблицу"));
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"drop table Uplost");
  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) != ER_BAD_TABLE_ERROR)
     {
      sprintf(strsql,"%s ! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
      iceb_printw(strsql,data->buffer_error,data->view);
     }
    else
     {
      sprintf(strsql,"%s\n",gettext("Таблица не обнаружена"));
      iceb_printw(strsql,data->buffer,data->view);
     }

   }

  sprintf(strsql,"%s Upldok2\n",gettext("Удаляем таблицу"));
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"drop table Upldok2");
  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) != ER_BAD_TABLE_ERROR)
     {
      sprintf(strsql,"%s ! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
      iceb_printw(strsql,data->buffer_error,data->view);
     }
    else
     {
      sprintf(strsql,"%s\n",gettext("Таблица не обнаружена"));
      iceb_printw(strsql,data->buffer,data->view);
     }

   }

  sprintf(strsql,"%s Uplgost\n",gettext("Удаляем таблицу"));
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"drop table Uplgost");
  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) != ER_BAD_TABLE_ERROR)
     {
      sprintf(strsql,"%s ! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
      iceb_printw(strsql,data->buffer_error,data->view);
     }
    else
     {
      sprintf(strsql,"%s\n",gettext("Таблица не обнаружена"));
      iceb_printw(strsql,data->buffer,data->view);
     }

   }


  sprintf(strsql,"%s Uplavt\n",gettext("Удаляем колонки из таблицы"));
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"alter table Uplavt drop nrmh,drop nrtk,drop znrzg,drop lnrzg,drop lnr,drop znr");

  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) != ER_CANT_DROP_FIELD_OR_KEY)
     {
      sprintf(strsql,"%s ! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
      iceb_printw(strsql,data->buffer_error,data->view);
     }
    else
     {
      sprintf(strsql,"%s\n",gettext("Колонки уже удалены"));
      iceb_printw(strsql,data->buffer,data->view);
     }
   }

  sprintf(strsql,"%s Upldok\n",gettext("Удаляем колонки из таблицы"));
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"alter table Upldok drop tk,drop ztvrf,drop ztvrn,drop vrr,drop ztvsf,drop ztvsn,drop vesg,drop nstzg,drop probzg,drop ztfazg,drop ztpnzg,drop nst,drop ztpn,drop ztfa,drop prob");

  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) != ER_CANT_DROP_FIELD_OR_KEY)
     {
      sprintf(strsql,"%s ! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
      iceb_printw(strsql,data->buffer_error,data->view);
     }
    else
     {
      sprintf(strsql,"%s\n",gettext("Колонки уже удалены"));
      iceb_printw(strsql,data->buffer,data->view);
     }
   }

 }
/****************************************/
/*********** 7.26 31.05.2012 ************/
/****************************************/

sprintf(strsql,"%s k00 %s Dokummat\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"alter table Dokummat add k00 char(16) not null default '00'");
if(admin_alter_r_vnk(strsql,data) == 0)
 {
  sprintf(strsql,"%s k00 %s Usldokum\n",gettext("Вставляем колонку"),gettext("в таблицу"));
  iceb_printw(strsql,data->buffer,data->view);
  sprintf(strsql,"alter table Usldokum add k00 char(16) not null default '00'");
  admin_alter_r_vnk(strsql,data);

 }

/****************************************/
/*********** 7.30 27.07.2012 ************/
/****************************************/

admin_alter_r_ins_str_alx("matnast.alx","Системная подпись в налоговой накладной","Включено",data);

/****************************************/
/*********** 7.31 13.08.2012 ************/
/****************************************/

admin_alter_r_ins_str_alx("rnn_nast.alx","Экспортировать для регистрации в налоговой свою налоговую накладную","Выключено",data);
 
jump13082012:;
/****************************************/
/*********** 8.1 05.09.2012 ************/
/****************************************/
sprintf(strsql,"%s \"Код операции автоматической дооценки\" %s matnast.alx\n",gettext("Удаляем настройку"),gettext("в файле"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='matnast.alx' and str like 'Код операции автоматической дооценки|%%'");
iceb_sql_zapis(strsql,1,0,data->window);

jump05092012:;
/****************************************/
/*********** 8.7 06.11.2012 ************/
/****************************************/
sprintf(strsql,"%s\n",gettext("Добавляем код оператора для действий выполненных программой"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"insert into icebuser (login,fio,un,kom,logz,vrem) values('%s','%s',%d,'%s','%s',%ld)",
ICEB_LOGIN_OP_PROG,
gettext("Программа"),
ICEB_KOD_OP_PROG,
gettext("Для действий выполненных без участия оператора"),
iceb_u_getlogin(),time(NULL));

iceb_sql_zapis(strsql,1,1,data->window);


jump06112012:;

/****************************************/
/*********** 8.7 28.11.2012 ************/
/****************************************/
sprintf(strsql,"%s \"Копирование на ДОС дискету\" %s nastsys.alx\n",gettext("Удаляем настройку"),gettext("в файле"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='nastsys.alx' and str like 'Копирование на ДОС дискету|%%'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s \"Устройство ДОС для копирования\" %s nastsys.alx\n",gettext("Удаляем настройку"),gettext("в файле"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='nastsys.alx' and str like 'Устройство ДОС для копирования|%%'");
iceb_sql_zapis(strsql,1,0,data->window);

jump28112012:;

/****************************************/
/*********** 8.8 30.11.2012 ************/
/****************************************/
sprintf(strsql,"%s pp03072009.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='pp03072009.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s pt03072009.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='pt03072009.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

jump30112012:;

/****************************************/
/*********** 8.9 05.12.2012 ************/
/****************************************/
sprintf(strsql,"%s zarspv2.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='zarspv2.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s zarspv1.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='zarspv1.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

jump05122012:;

/****************************************/
/*********** 8.9 02.01.2013 ************/
/****************************************/
sprintf(strsql,"%s nn4_start.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='nn4_start.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s nn4_end.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='nn4_end.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

jump02012013:;

/****************************************/
/*********** 8.12 11.01.2013 ************/
/****************************************/

admin_alter_r_ins_str_alx("zarnast.alx","Табельный номер выплачивающего зарплату","",data);
admin_alter_r_ins_str_alx("zarnast.alx","Табельный номер проверяющего ведомости на зарплату","",data);


jump11012013:;

/****************************************/
/*********** 8.13 11.02.2013 ************/
/****************************************/

admin_alter_r_ins_str_alx("zarpensm.alx","Коды табеля прeбывания в трудовых отношениях","",data);


jump11022013:;

/****************************************/
/*********** 8.13 06.03.2013 ************/
/****************************************/

/*Загрузка новых настроечных файлов*/

jump06032013:;

/****************************************/
/*********** 8.13 05.04.2013 ************/
/****************************************/

sprintf(strsql,"%s vd %s Reenn\n",gettext("Увеличиваем длинну колонки"),gettext("в таблице"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Reenn change vd vd varchar(32) not null default ''");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"%s vd %s Reenn1\n",gettext("Увеличиваем длинну колонки"),gettext("в таблице"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Reenn1 change vd vd varchar(32) not null default ''");
admin_alter_sql_zap(strsql,data);

jump05042013:;

/****************************************/
/*********** 8.16 21.06.2013 ************/
/****************************************/

sprintf(strsql,"%s dogov %s Kontragent\n",gettext("Переименовываем колонку"),gettext("в таблице"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Kontragent change dogov na varchar(255) not null default ''");
if(admin_alter_sql_zap(strsql,data) == 0)
 {
  sprintf(strsql,"update Kontragent set na=''");
  admin_alter_sql_zap(strsql,data);
 }

jump21062013:;

/****************************************/
/*********** 9.3 03.09.2013 ************/
/****************************************/
sprintf(strsql,"%s Glksval\n",gettext("Создание таблицы справочника валют"));
iceb_printw(strsql,data->buffer,data->view);

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
if(admin_alter_r_crtb(strsql,"Uplnst",data) == 0)
 {
  sprintf(strsql,"insert into Glksval values ('%s','%s','',%d,%ld)",gettext("дол"),gettext("Доллар"),iceb_getuid(data->window),time(NULL));
  admin_alter_sql_zap(strsql,data);
  sprintf(strsql,"insert into Glksval values ('%s','%s','',%d,%ld)",gettext("евр"),gettext("Евро"),iceb_getuid(data->window),time(NULL));
  admin_alter_sql_zap(strsql,data);
  sprintf(strsql,"insert into Glksval values ('%s','%s','',%d,%ld)",gettext("руб"),gettext("Рубли"),iceb_getuid(data->window),time(NULL));
  admin_alter_sql_zap(strsql,data);
 }

sprintf(strsql,"%s Glkkv\n",gettext("Создание таблицы справочника курсов валют"));
iceb_printw(strsql,data->buffer,data->view);

strcpy(strsql,"CREATE TABLE Glkkv (\
kod  char(16) not null default '',\
datk DATE not null default '0000-00-00',\
kurs double(12,4) NOT NULL DEFAULT 0.,\
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
admin_alter_r_crtb(strsql,"Uplnst",data);


sprintf(strsql,"%s kv %s Kas\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"alter table Kas add kv char(16) not null default ''");

admin_alter_r_vnk(strsql,data);

jump04092013:;

sprintf(strsql,"update Alx set str='Печатать наименование счёта корреспондента в Главной книге|Включено\n' where fil='nastrb.alx' and str like 'Печатать наименование счёта кореспондента в Главной книге|%%'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"update Alx set str='Печатать наименование счёта корреспондента в журналах-ордерах для счетов с развёрнутым сальдо|Включено\n' where fil='nastrb.alx' and str like 'Печатать наименование счёта кореспондента в журналах-ордерах для счетов с развёрнутым сальдо|%%'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"update Alx set str='Печатать фамилию оператора в распечатках отчётов|Включено\n' where fil='nastsys.alx' and str like 'Печатать фамилию оператора в распечатках отчетов|%%'");
iceb_sql_zapis(strsql,1,0,data->window);

if(iceb_poldan("Код перерасчета подоходного налога",&bros,"zarnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Код перерасчёта подоходного налога|%s\n' where fil='zarnast.alx' and str like 'Код перерасчета подоходного налога|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
  
 }

if(iceb_poldan("Коды начислений не входящие в расчет подоходного налога",&bros,"zarnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Коды начислений не входящие в расчёт подоходного налога|%s\n' where fil='zarnast.alx' and str like 'Коды начислений не входящие в расчет подоходного налога|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
  
 }

if(iceb_poldan("Коды не входящие в расчет доплаты до минимальной зарплаты",&bros,"zarnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Коды не входящие в расчёт доплаты до минимальной зарплаты|%s\n' where fil='zarnast.alx' and str like 'Коды не входящие в расчет доплаты до минимальной зарплаты|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
  
 }

if(iceb_poldan("Коды отдельного расчета доплаты до минимальной зарплаты",&bros,"zarnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Коды отдельного расчёта доплаты до минимальной зарплаты|%s\n' where fil='zarnast.alx' and str like 'Коды отдельного расчета доплаты до минимальной зарплаты|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
  
 }

if(iceb_poldan("Коды удержаний входящие в расчет подоходного налога",&bros,"zarnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Коды удержаний входящие в расчёт подоходного налога|%s\n' where fil='zarnast.alx' and str like 'Коды удержаний входящие в расчет подоходного налога|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
  
 }

if(iceb_poldan("Дополнительные зарплатные счёта",&bros,"zarnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Дополнительные зарплатные счета|%s\n' where fil='zarnast.alx' and str like 'Дополнительные зарплатные счёта|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
  
 }

if(iceb_poldan("Коды удержаний входящие в расчет алиментов",&bros,"zaralim.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Коды удержаний входящие в расчёт алиментов|%s\n' where fil='zaralim.alx' and str like 'Коды удержаний входящие в расчет алиментов|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

jump18092013:; //*************************************************************

if(iceb_poldan("Код перерасчета подоходного налога",&bros,"zarnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Код перерасчёта подоходного налога|%s\n' where fil='zarnast.alx' and str like 'Код перерасчета подоходного налога|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
  
 }

if(iceb_poldan("Банковский счет",&bros,"nastdok.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Банковский счёт|%s\n' where fil='nastdok.alx' and str like 'Банковский счет|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
  
 }

if(iceb_poldan("Расчетный счет казначейства",&bros,"uslnastk.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Расчётный счёт казначейства|%s\n' where fil='uslnastk.alx' and str like 'Расчетный счет казначейства|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
  
 }
if(iceb_poldan("Регистрационный счет в казначействе",&bros,"uslnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Регистрационный счёт в казначействе|%s\n' where fil='uslnastk.alx' and str like 'Регистрационный счет в казначействе|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
  
 }
if(iceb_poldan("Коды начислений не входящие в расчет",&bros,"zaralim.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Коды начислений не входящие в расчёт|%s\n' where fil='zaralim.alx' and str like 'Коды начислений не входящие в расчет|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

if(iceb_poldan("Профсоюзный фонд-коды начислений не входящие в расчет",&bros,"zarnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Профсоюзный фонд-коды начислений не входящие в расчёт|%s\n' where fil='zarnast.alx' and str like 'Профсоюзный фонд-коды начислений не входящие в расчет|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

if(iceb_poldan("Коды не входящие в расчет индексации",&bros,"zarnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Коды не входящие в расчёт индексации|%s\n' where fil='zarnast.alx' and str like 'Коды не входящие в расчет индексации|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

if(iceb_poldan("Коды начислений не входящие в расчет командировочных",&bros,"zarnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Коды начислений не входящие в расчёт командировочных|%s\n' where fil='zarnast.alx' and str like 'Коды начислений не входящие в расчет командировочных|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

if(iceb_poldan("Коды табеля не входящие в расчет командировочных",&bros,"zarnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Коды табеля не входящие в расчёт командировочных|%s\n' where fil='zarnast.alx' and str like 'Коды табеля не входящие в расчет командировочных|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

if(iceb_poldan("Код перечисления зарплаты на счет",&bros,"zarnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Код перечисления зарплаты на счёт|%s\n' where fil='zarnast.alx' and str like 'Код перечисления зарплаты на счет|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

if(iceb_poldan("Счет расчетов по зарплате",&bros,"zarnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Счёт расчетов по зарплате|%s\n' where fil='zarnast.alx' and str like 'Счет расчетов по зарплате|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

if(iceb_poldan("Коды не входящие в расчет отпускных",&bros,"zarotp.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Коды не входящие в расчёт отпускных|%s\n' where fil='zarotp.alx' and str like 'Коды не входящие в расчет отпускных|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

if(iceb_poldan("Коды не входящие в расчет индексации отпускных",&bros,"zarotp.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Коды не входящие в расчёт индексации отпускных|%s\n' where fil='zarotp.alx' and str like 'Коды не входящие в расчет индексации отпускных|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

if(iceb_poldan("Коды не входящие в расчет средней зарплаты для начисления больничного",&bros,"zarsrbol.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Коды не входящие в расчёт средней зарплаты для начисления больничного|%s\n' where fil='zarsrbol.alx' and str like 'Коды не входящие в расчет средней зарплаты для начисления больничного|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

if(iceb_poldan("Печатать коды невходящие в расчет",&bros,"zarsrbol.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Печатать коды невходящие в расчёт|%s\n' where fil='zarsrbol.alx' and str like 'Печатать коды невходящие в расчет|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

if(iceb_poldan("Печатать коды невходящие в расчет",&bros,"zarsrbold.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Печатать коды невходящие в расчёт|%s\n' where fil='zarsrbold.alx' and str like 'Печатать коды невходящие в расчет|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

if(iceb_poldan("Округление 1",&bros,"matnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Округление суммы|%s\n' where fil='matnast.alx' and str like 'Округление 1|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

admin_alter_r_ins_str_alx("kasnast.alx","Максимальная сумма за день от контрагента","10000",data);

jump21092013:; /*************9.3************************************************/


if(iceb_poldan("Счёт расчетов по зарплате",&bros,"zarnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Счёт расчётов по зарплате|%s\n' where fil='zarnast.alx' and str like 'Счёт расчетов по зарплате|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

if(iceb_poldan("Счет подоходного налога",&bros,"zarnast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Счёт подоходного налога|%s\n' where fil='zarnast.alx' and str like 'Счет подоходного налога|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

sprintf(strsql,"alter table Dokummat2 change sodz sodz varchar(255) not null default ''");
admin_alter_sql_zap(strsql,data);

jump28092013:; /*************9.4************************************************/

jump31102013:; /*************9.7************************************************/
if(iceb_poldan("Экспортировать для регистрации в налоговой свою налоговую накладную",&bros,"rnn_nast.alx",data->window) == 0)
 {
  sprintf(strsql,"update Alx set str='Экспорт в ОПЗ|%s\n' where fil='rnn_nast.alx' and str like 'Экспортировать для регистрации в налоговой свою налоговую накладную|%%'",bros.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }

jump24112013:; /*************9.9************************************************/
sprintf(strsql,"delete from Alx where fil='nastdok.alx' and str like 'Разделитель копеек|%%'");
iceb_sql_zapis(strsql,1,0,data->window);

jump27122013:; /*************9.9************************************************/
sprintf(strsql,"delete from Alx where fil='nastsys.alx' and str like 'Кодировка в которой работает iceB|%%'");
iceb_sql_zapis(strsql,1,0,data->window);


sprintf(strsql,"%s vnn %s Kontragent2\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Kontragent2 add vnn VARCHAR(16) NOT NULL DEFAULT ''");

admin_alter_r_vnk(strsql,data); 

jump10012014:; /*************9.10************************************************/
admin_alter_r_ins_str_alx("zarnast.alx","Коды начислений дополнительно входящие в расчёт удержания ЕСВ для больничных","",data);

jump29012014:; /*************9.10************************************************/
/*вставка новых настроечных файлов*/

jump20022014:; /*************9.11************************************************/
admin_alter_r_ins_str_alx("rnn_nimp1.alx","Материальный учёт колонки 11-12","",data);
admin_alter_r_ins_str_alx("rnn_nimp1.alx","Учёт услуг колонки 11-12","",data);
admin_alter_r_ins_str_alx("rnn_nimp1.alx","Учёт основных средств колонки 11-12","",data);
admin_alter_r_ins_str_alx("rnn_nimp1.alx","Процент расчёта колонок 11-12","",data);

jump25022014:; /*************9.11************************************************/
/*вставка новых настроечных файлов*/

/*Удаляем файлы для того чтобы загрузились новые версии этих файлов*/
sprintf(strsql,"%s nn5_start.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='nn5_start.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s nn4_hap.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='nn4_hap.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

jump12032014:; /*************9.12************************************************/


/*следуующий прыжок сюда*/
/*Удаляем как не нужные*/

sprintf(strsql,"%s rnnd5_start.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"delete from Alx where fil='rnnd5_start.alx'");
iceb_sql_zapis(strsql,1,0,data->window);


sprintf(strsql,"%s rnnd2_start.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"delete from Alx where fil='rnnd2_start.alx'");
iceb_sql_zapis(strsql,1,0,data->window);


sprintf(strsql,"%s rnnd5_end.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"delete from Alx where fil='rnnd5_end.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s nn4_hap.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='nn4_hap.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s nn4_end.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='nn4_end.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

jump08042014:; /*************9.12************************************************/

sprintf(strsql,"%s nn4_end.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='nn4_end.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s nn5_start.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='nn5_start.alx'");
iceb_sql_zapis(strsql,1,0,data->window);
/*новые настроечные файлы*/
jump01052014:; /*************9.14************************************************/
sprintf(strsql,"%s nn5_start.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='nn5_start.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

jump14052014:; /*************9.15************************************************/
sprintf(strsql,"%s nn5_start.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='nn5_start.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

jump30052014:; /*************9.15************************************************/

admin_alter_r_ins_str_alx("zarnast.alx","Коды табеля входящие в расчёт индексации","",data);

jump03062014:; /*************9.15************************************************/

sprintf(strsql,"%s nn5_start.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='nn5_start.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s nn4_hap.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='nn4_hap.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s nn4_end.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='nn4_end.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

jump03072014:; /*************9.16************************************************/

sprintf(strsql,"%s rpvnn1_3.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='rpvnn1_3.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s rpvnn2_3.alx\n",gettext("Удаляем файл"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"delete from Alx where fil='rpvnn2_3.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

jump19072014:; /*************9.16************************************************/

sprintf(strsql,"%s vsb,vsn,vdb %s icebuser\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table icebuser add \
vsd tinyint not null default 0,\
add vsb tinyint not null default 0,\
add vsn tinyint not null default 0");

admin_alter_r_vnk(strsql,data);

sprintf(strsql,"%s vdb %s icebuser\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"alter table icebuser add vdb tinyint not null default 0");

admin_alter_r_vnk(strsql,data);

jump31072014:; /*************9.17************************************************/
admin_alter_r_ins_str_alx("zarnast.alx","Код военного налога","",data);
admin_alter_r_ins_str_alx("zarnast.alx","Коды не входящие в расчёт военного налога","",data);

jump12082014:; /*************9.17************************************************/

sprintf(strsql,"%s post %s Uddok\n",gettext("Увеличиваем длинну колонки"),gettext("в таблице"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Uddok change post post varchar(255) not null default ''");
admin_alter_sql_zap(strsql,data);

jump28082014:; /*************9.17************************************************/

/*Загрузка новых настроечных файлов*/

jump27112014:; /*************10.1************************************************/
/*Загрузка новых настроечных файлов*/
sprintf(strsql,"delete from Alx where fil='rpvnn0.alx'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='rpvnn0_1.alx'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='rpvnn0_2.alx'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='rpvnn0_3.alx'");
admin_alter_sql_zap(strsql,data);

jump26012015:; /*************10.4************************************************/

sprintf(strsql,"%s Dokummat\n",gettext("Преобразуем колонки в таблице"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Dokummat change tip tip smallint not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Dokummat change kodop kodop char(8) not null default ''");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Dokummat change pod pod smallint not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Dokummat change pro pro smallint not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Dokummat change ktoi ktoi smallint unsigned not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Dokummat change vrem vrem int unsigned not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Dokummat change nomon nomon char(16) not null default ''");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s Dokummat1\n",gettext("Преобразуем колонки в таблице"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Dokummat1 change kolih kolih double(16,6) not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Dokummat1 change cena cena double(16,6) not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Dokummat1 change voztar voztar smallint not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Dokummat1 change nds nds float(2) not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Dokummat1 change mnds mnds smallint not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Dokummat1 change ktoi ktoi smallint unsigned not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Dokummat1 change vrem vrem int unsigned not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Dokummat1 change tipz tipz smallint not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);


sprintf(strsql,"%s Material\n",gettext("Преобразуем колонки в таблице"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Material change krat krat double not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Material change fasv fasv double not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Material change kodt kodt int not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Material change ktoz ktoz smallint unsigned not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Material change vrem vrem int unsigned not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Material change nds nds  real not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Material change msp msp smallint not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);


sprintf(strsql,"%s Usldokum\n",gettext("Преобразуем колонки в таблице"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Usldokum change nds nds smallint not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum change forop forop  varchar(20) not null default ''");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum change datop datop  DATE not null default '0000-00-00'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum change uslpr uslpr  varchar(80) not null default ''");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum change sumkor sumkor double(10,2) not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum change pod pod    smallint not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum change pro pro    smallint not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum change oplata oplata smallint not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum change blokir blokir smallint unsigned not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum change nomdp nomdp  varchar(20) not null default ''");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum change datdp datdp  DATE not null default '0000-00-00'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum change dover dover  varchar(20) not null default ''");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum change datdov datdov DATE not null default '0000-00-00'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum change sherez sherez varchar(40) not null default ''");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum change ktoi ktoi   smallint unsigned not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum change vrem vrem   int unsigned not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s Usldokum1\n",gettext("Преобразуем колонки в таблице"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Usldokum1 change cena cena   double(16,6) not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum1 change ktoi ktoi   smallint unsigned not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Usldokum1 change vrem vrem   int unsigned not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s Uslugi\n",gettext("Преобразуем колонки в таблице"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Uslugi change kodgr kodgr   smallint not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Uslugi change naius naius   varchar(80) not null default ''");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Uslugi change ei ei varchar(20) not null default ''");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Uslugi change cena cena    double(10,2) not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Uslugi change nds nds real not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Uslugi change ktoz ktoz	smallint unsigned not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Uslugi change vrem vrem	int unsigned not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

jump01022015:; /*************10.5************************************************/

sprintf(strsql,"%s kei %s Edizmer\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Edizmer add kei char(16) not null default ''");
admin_alter_r_vnk(strsql,data);

sprintf(strsql,"alter table Kontragent change ktoz ktoz	smallint unsigned not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"alter table Kontragent change vrem vrem	int unsigned not null default 0");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='nn7_hap.alx'");
iceb_sql_zapis(strsql,1,0,data->window);
sprintf(strsql,"delete from Alx where fil='nn7_end.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

jump09022015:; /*************10.6************************************************/

/*новый настроечный файл*/

jump02032015:; /*************10.6************************************************/

sprintf(strsql,"%s F1dfvs\n",gettext("Создаём новую таблицу"));
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
admin_alter_r_crtb(strsql,"F1dfvs",data);

sprintf(strsql,"%s kr %s F8dr\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table F8dr add kr INT NOT NULL DEFAULT 0");
admin_alter_r_vnk(strsql,data);

sprintf(strsql,"%s ks %s F8dr\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table F8dr add ks INT NOT NULL DEFAULT 0");
admin_alter_r_vnk(strsql,data);

if(iceb_poldan("Код военного налога",&bros,"zarnast.alx",data->window) == 0)
 {

  sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Код военного налога|%%'");
  iceb_sql_zapis(strsql,1,0,data->window);

  admin_alter_r_ins_str_alx("zarnast.alx","Код военного сбора",bros.ravno(),data);
 }

if(iceb_poldan("Коды не входящие в расчёт военного налога",&bros,"zarnast.alx",data->window) == 0)
 {

  sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Коды не входящие в расчёт военного налога|%%'");
  iceb_sql_zapis(strsql,1,0,data->window);

  admin_alter_r_ins_str_alx("zarnast.alx","Коды не входящие в расчёт военного сбора",bros.ravno(),data);
 }

jump21042015:; /*************10.9************************************************/

admin_alter_r_ins_str_alx("nastsys.alx","Перекодировка XML файлов при копировании на Windows машину","Выключено",data);

sprintf(strsql,"update Alx set str='Кодовая таблица|%s\n' where fil='rnn_nast.alx' and str like 'Кодовая таблица|%%'","utf-8");
iceb_sql_zapis(strsql,1,0,data->window);

jump09052015:; /*************10.10************************************************/

sprintf(strsql,"alter table Uosamor add kmo INT NOT NULL DEFAULT 0");
if(admin_alter_r_vnk(strsql,data) == 0)
 {

  sprintf(strsql,"%s Uosamor\n",gettext("Преобразуем таблицу"));
  iceb_printw(strsql,data->buffer,data->view);

  admin_alter_r_amort("Uosamor",data);
 }

sprintf(strsql,"alter table Uosamor1 add kmo INT NOT NULL DEFAULT 0");
if(admin_alter_r_vnk(strsql,data) == 0)
 {
  sprintf(strsql,"%s Uosamor1\n",gettext("Преобразуем таблицу"));
  iceb_printw(strsql,data->buffer,data->view);

  admin_alter_r_amort("Uosamor1",data);
 }

jump03072015:; /*************10.14************************************************/

sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Максимальная сумма необлагаемой благотворительной помощи|%%'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Счёт подоходного налога|%%'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Автоматическая вставка кода контрагента в список счета|%%'");
iceb_sql_zapis(strsql,1,0,data->window);

admin_alter_r_ins_str_alx("zarotp.alx","Коды табеля не входящие в расчёт","",data);

jump28072015:; /*************10.16************************************************/
admin_alter_r_ins_str_alx("zarsdf1df.alx","Коды командировочных расходов для кода дохода NN","",data);
iceb_poldan("Код контрагента с нужными банковскими реквизитами",&bros,"nastud.alx",data->window);

admin_alter_r_ins_str_alx("nastud.alx","Код контрагента получателя",bros.ravno(),data);
admin_alter_r_ins_str_alx("nastud.alx","Код контрагента плательщика","",data);

sprintf(strsql,"delete from Alx where fil='nastud.alx' and str like 'Код контрагента с нужными банковскими реквизитами|%%'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Дополнительные зарплатные счёта|%%'");
iceb_sql_zapis(strsql,1,0,data->window);
sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Дополнительные зарплатные счета|%%'");
iceb_sql_zapis(strsql,1,0,data->window);
sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Начисления, для которых проводки выполняются по дополнительному счёту%%'");
iceb_sql_zapis(strsql,1,0,data->window);

jump01092015:; /*************11.0************************************************/
admin_alter_r_ins_str_alx("zarpensm.alx","Коды начисления помощи по беременности и родам","",data);
admin_alter_r_ins_str_alx("zarpensm.alx","Коды табеля по беременности и родам","",data);

jump07092015:; /*************11.0************************************************/
/*Новые настроечные файлы*/
jump22092015:; /*************11.0************************************************/

sprintf(strsql,"%s dolg %s Uosol\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Uosol add dolg varchar(100) NOT NULL DEFAULT ''");
admin_alter_r_vnk(strsql,data);

jump02102015:; /*************11.1************************************************/
/*Новые настроечные файлы*/
jump03122015:; /*************11.3************************************************/

sprintf(strsql,"delete from Alx where fil='invmu572z.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

if(iceb_poldan("Код фонда единого социального взноса",&bros,"zarnast.alx",data->window) == 0)
 {
  if(bros.ravno_atoi() > 0)
   {
    sprintf(strsql,"Код фонда единого социального взноса-%s Zaresv\n",gettext("Вставляем запись новых процентов"));
    iceb_printw(strsql,data->buffer,data->view);
    sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2016-01-01',22.,0.,%d,%ld)",bros.ravno_atoi(),iceb_getuid(data->window),time(NULL));

    iceb_sql_zapis(strsql,1,0,data->window);
   }
 }

if(iceb_poldan("Код фонда единого социального взноса для инвалидов",&bros,"zarnast.alx",data->window) == 0)
 {
  if(bros.ravno_atoi() > 0)
   {
    sprintf(strsql,"Код фонда единого социального взноса для инвалидов-%s Zaresv\n",gettext("Вставляем запись новых процентов"));
    iceb_printw(strsql,data->buffer,data->view);
    sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2016-01-01',8.41,0.,%d,%ld)",bros.ravno_atoi(),iceb_getuid(data->window),time(NULL));

    iceb_sql_zapis(strsql,1,0,data->window);
   }
 }

jump12012016:; /*************11.4************************************************/

if(iceb_poldan("Код фонда единого социального взноса для больничных",&bros,"zarnast.alx",data->window) == 0)
 {
  if(bros.ravno_atoi() > 0)
   {
    sprintf(strsql,"Код фонда единого социального взноса для больничных-%s Zaresv\n",gettext("Вставляем запись новых процентов"));
    iceb_printw(strsql,data->buffer,data->view);
    sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2016-01-01',22.,0.,%d,%ld)",bros.ravno_atoi(),iceb_getuid(data->window),time(NULL));

    iceb_sql_zapis(strsql,1,0,data->window);
   }
 }

if(iceb_poldan("Код фонда единого социального взноса с больничных для инвалидов",&bros,"zarnast.alx",data->window) == 0)
 {
  if(bros.ravno_atoi() > 0)
   {
    sprintf(strsql,"Код фонда единого социального взноса с больничных для инвалидов-%s Zaresv\n",gettext("Вставляем запись новых процентов"));
    iceb_printw(strsql,data->buffer,data->view);
    sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2016-01-01',8.41,0.,%d,%ld)",bros.ravno_atoi(),iceb_getuid(data->window),time(NULL));

    iceb_sql_zapis(strsql,1,0,data->window);
   }
 }
if(iceb_poldan("Код фонда единого социального взноса для договоров",&bros,"zarnast.alx",data->window) == 0)
 {
  if(bros.ravno_atoi() > 0)
   {
    sprintf(strsql,"Код фонда единого социального взноса для договоров-%s Zaresv\n",gettext("Вставляем запись новых процентов"));
    iceb_printw(strsql,data->buffer,data->view);
    sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2016-01-01',22.,0.,%d,%ld)",bros.ravno_atoi(),iceb_getuid(data->window),time(NULL));

    iceb_sql_zapis(strsql,1,0,data->window);
   }
 }
if(iceb_poldan("Код фонда единого социального взноса для военных",&bros,"zarnast.alx",data->window) == 0)
 {
  if(bros.ravno_atoi() > 0)
   {
    sprintf(strsql,"Код фонда единого социального взноса для военных-%s Zaresv\n",gettext("Вставляем запись новых процентов"));
    iceb_printw(strsql,data->buffer,data->view);
    sprintf(strsql,"insert into Zaresv (kf,datnd,pr,pr1,ktoz,vrem) values (%d,'2016-01-01',22.,0.,%d,%ld)",bros.ravno_atoi(),iceb_getuid(data->window),time(NULL));

    iceb_sql_zapis(strsql,1,0,data->window);
   }
 }

jump18012016:; /*************11.4************************************************/

sprintf(strsql,"%s naikon %s Kontragent\n",gettext("Увеличиваем длинну колонки"),gettext("в таблице"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Kontragent change naikon naikon varchar(255) not null default ''");

admin_alter_sql_zap(strsql,data);

jump04022016:; /*************11.4************************************************/

sprintf(strsql,"%s\n",gettext("Добавляем индекс pnaim в таблицу Kontragent"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Kontragent add index pnaim (pnaim)");

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DUP_KEYNAME)
   {
    sprintf(strsql,"%s\n",gettext("Индекс уже существует"));
    iceb_printw(strsql,data->buffer,data->view);
   }
  else
   {
    sprintf(strsql,"%s! %d\n%s\n",gettext("Ошибка"),sql_nerror(&bd),sql_error(&bd));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }
 
jump08022016:; /*************11.5************************************************/

sprintf(strsql,"delete from Alx where fil='zarsns_end.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='zarsns_start.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

jump24022016:; /*************11.5************************************************/

sprintf(strsql,"delete from Alx where fil='glk_balans2.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s bs %s Plansh\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Plansh add bs tinyint not null default 0");
admin_alter_r_vnk(strsql,data);

jump09032016:; /*************11.7************************************************/

sprintf(strsql,"%s tn %s icebuser\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table icebuser add tn int unsigned not null default 0");
admin_alter_r_vnk(strsql,data);
jump16032016:; /*************11.7************************************************/

admin_alter_r_zpktn(data); /*корректировка приставки*/

sprintf(strsql,"delete from Alx where fil='zarnast.alx' and str like 'Приставка к табельному номеру|%%'");
admin_alter_sql_zap(strsql,data);

jump30032016:; /*************11.7************************************************/

admin_alter_r_skkvp(data);

jump01042016:; /*************11.9************************************************/

sprintf(strsql,"delete from Alx where fil='rnn_nast.alx' and str like 'Экспорт в ОПЗ|%%'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='nakl.alx'");
admin_alter_sql_zap(strsql,data);
sprintf(strsql,"delete from Alx where fil='nakls.alx'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='nalnakst.alx'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='nalnaks1.alx'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='nalnake.alx'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='nn4_end.alx'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='nn4_hap.alx'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='nn4_start.alx'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='nn5_start.alx'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='nn6_end.alx'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='nn6_hap.alx'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='nn6_start.alx'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='nastsys.alx' and str like 'Шрифт с одинаковой шириной всех символов|%%'");
admin_alter_sql_zap(strsql,data);

sprintf(strsql,"delete from Alx where fil='nastsys.alx' and str like 'Шрифт реквизитов разработчика|%%'");
admin_alter_sql_zap(strsql,data);


admin_alter_r_ins_str_alx("zarnast.alx","Коды премии за квартал","",data);

sprintf(strsql,"%s kpd %s Zarskrdh\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Zarskrdh add kpd int unsigned not null default 0");
admin_alter_r_vnk(strsql,data);

jump25052016:; /*************11.13************************************************/

admin_alter_r_ins_str_alx("zarotp.alx","Дополнительные коды не входящие в расчёт","",data);
admin_alter_r_ins_str_alx("zarotp.alx","Коды начислений для дополнительных кодов","",data);



sprintf(strsql,"Создание таблицы справочника должностных окладов Zarsdo\n");
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

if(admin_alter_r_crtb(strsql,"Zarsdo",data) == 0)
 admin_alter_r_zdo(data);

jump31052016:; /*************11.13************************************************/

/*обновляем файл*/
sprintf(strsql,"delete from Alx where fil='zarsns_start.alx'");
iceb_sql_zapis(strsql,1,0,data->window);
jump24062016:; /*************11.15************************************************/


if(iceb_poldan("Код военного сбора с больничного",strsql,"zarnast.alx",data->window) != 0)
 {
  int nom=0;
  for(nom=1;; nom++)
   {
    sprintf(strsql,"select kod from Uder where kod=%d",nom);
    if(iceb_sql_readkey(strsql,data->window) == 0)
     break;
   }

  class iceb_u_str shet("");
  class iceb_u_str shetb("");
  int kodvs=0;
  if(iceb_poldan("Код военного сбора",&kodvs,"zarnast.alx",data->window) == 0)
   {
    sprintf(strsql,"select shet,shetb from Uder where kod=%d",kodvs);
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      shet.new_plus(row[0]);
      shetb.new_plus(row[1]);
     }
   }

  sprintf(strsql,"insert into Uder (kod,naik,shet,ktoz,vrem,prov,shetb) values (%d,'%s','%s',%d,%ld,0,'%s')",nom,gettext("Военный сбор c больничного"),shet.ravno(),iceb_getuid(data->window),time(NULL),shetb.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
  sprintf(strsql,"%d",nom);
  admin_alter_r_ins_str_alx("zarnast.alx","Код военного сбора с больничного",strsql,data);

  class iceb_u_str kod_ob_ud("");
  iceb_poldan("Обязательные удержания",&kod_ob_ud,"zarnast.alx",data->window);
  kod_ob_ud.z_plus(nom);    

  sprintf(strsql,"update Alx set str='Обязательные удержания|%s\n' where fil='zarnast.alx' and str like 'Обязательные удержания|%%'",kod_ob_ud.ravno());
  iceb_sql_zapis(strsql,1,0,data->window);
 }


jump16112016:; /*************12.3************************************************/

/*Загрузка новых настроечных файлов*/

jump22112016:; /*************12.3************************************************/

sprintf(strsql,"delete from Alx where fil='zarsns_start.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='zarsns_end.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s datsd %s Zardok\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Zardok add datsd DATE not null default '0000-00-00'");
admin_alter_r_vnk(strsql,data);

admin_alter_r_ustdat(data);

jump24112016:; /*************12.3************************************************/

sprintf(strsql,"%s ku %s Uslugi\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"alter table Uslugi add ku CHAR(32) NOT NULL DEFAULT ''");
admin_alter_r_vnk(strsql,data);

jump13022017:; /*************12.8************************************************/

sprintf(strsql,"%s Dokummat4\n",gettext("Создание таблицы списания материалов на детали"));
iceb_printw(strsql,data->buffer,data->view);
memset(strsql,'\0',sizeof(strsql));

strcpy(strsql,"CREATE TABLE Dokummat4 \
(datd  DATE NOT NULL default '0000-00-00',\
skl    smallint NOT NULL default 0,\
nomd   char(16) NOT NULL default '',\
km     int NOT NULL default 0,\
nk     int NOT NULL default 0,\
kd     int NOT NULL default 0,\
kol    double(20,10) NOT NULL default 0,\
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

admin_alter_r_crtb(strsql,"Dokummat4",data);

jump24022017:; /*************12.8************************************************/

admin_alter_r_ins_str_alx("zarotp.alx","Печать наименований кодов не входящих в расчёт","Включена",data);




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

admin_alter_r_crtb(strsql,"Musnrm",data);


jump06032017:; /*************12.9************************************************/

sprintf(strsql,"%s\n","Удаляем старые бланки налоговых накладных");
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"delete from Alx where fil like 'nn7_%%' ");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"%s\n","Добавляем колонки mi,kdstv таблицу Dokummat1");
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"alter table Dokummat1 add mi TINYINT NOT NULL DEFAULT 0,add kdstv VARCHAR(32) NOT NULL DEFAULT ''");

admin_alter_r_vnk(strsql,data);

jump22032017:; /*************12.9************************************************/

sprintf(strsql,"delete from Alx where fil='plansh_bd_ua.alx'"); /*грузим новый план счетов*/
iceb_sql_zapis(strsql,1,0,data->window);

jump18052017:; /*************12.11************************************************/

sprintf(strsql,"delete from Alx where fil='uosavp_end.alx'"); /*грузим исправленный файл*/
iceb_sql_zapis(strsql,1,0,data->window);

admin_alter_r_ins_str_alx("zarotp.alx","Коды начислений отпускных","",data);

jump04092017:; /*************12.14************************************************/

sprintf(strsql,"Увеличиваем количество знаков после запятой колонки cena в таблице Kart\n");
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"alter table Kart change cena cena double(17,7) not null default 0.");
admin_alter_r_vnk(strsql,data);

sprintf(strsql,"Увеличиваем количество знаков после запятой колонки cenap в таблице Kart\n");
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"alter table Kart change cenap cenap double(17,7) not null default 0.");
admin_alter_r_vnk(strsql,data);

sprintf(strsql,"Увеличиваем количество знаков после запятой колонки cena в таблице Zkart\n");
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"alter table Zkart change cena cena double(17,7) not null default 0.");
admin_alter_r_vnk(strsql,data);

sprintf(strsql,"Увеличиваем количество знаков после запятой колонки cena в таблице Dokummat1\n");
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"alter table Dokummat1 change cena cena double(17,7) not null default 0.");
admin_alter_r_vnk(strsql,data);

sprintf(strsql,"update Alx set str='Округление цены|0.0000001\n' where fil='matnast.alx' and str like 'Округление цены|%%'");
iceb_sql_zapis(strsql,1,0,data->window);



jump26092017:; /*************12.14************************************************/
/*загрузка новых настроечных файлов*/
jump09012018:; /*************13.1************************************************/

sprintf(strsql,"Увеличиваем количество знаков после запятой колонки kurs в таблице Glkkv\n");
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"alter table Glkkv change kurs kurs double(14,6) not null default 0.");
admin_alter_r_vnk(strsql,data);

jump10012018:; /*************13.1************************************************/

sprintf(strsql,"delete from Alx where fil='zarspv.alx'");
iceb_sql_zapis(strsql,1,0,data->window);
sprintf(strsql,"delete from Alx where fil='zarspv1.alx'");
iceb_sql_zapis(strsql,1,0,data->window);
sprintf(strsql,"delete from Alx where fil='zarspv2.alx'");
iceb_sql_zapis(strsql,1,0,data->window);
sprintf(strsql,"delete from Alx where fil='rnn_nimp1.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"Увеличиваем количество знаков после запятой колонки kolih в таблице Dokummat1\n");
iceb_printw(strsql,data->buffer,data->view);
sprintf(strsql,"alter table Dokummat1 change kolih kolih double(17,7) not null default 0.");
admin_alter_r_vnk(strsql,data);

sprintf(strsql,"%s pu %s Kontragent2\n",gettext("Вставляем колонку"),gettext("в таблицу"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"alter table Kontragent2 add pu tinyint not null default 0");
admin_alter_r_vnk(strsql,data);

jump27042018:; /*************13.4************************************************/

sprintf(strsql,"delete from Alx where fil like 'nn8_%%' ");
iceb_sql_zapis(strsql,1,0,data->window);

/*меняем "призвище" на "прізвище"*/

sprintf(strsql,"delete from Alx where fil='dok_dover_r.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='uosasoz818_1_end.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='uosasoz818_3_end.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='uosavp_end.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='uosoz6end.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='uosvamot_ra_end.alx'");
iceb_sql_zapis(strsql,1,0,data->window);


sprintf(strsql,"delete from Alx where fil='zarsf1df_1e.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='zarspv3.alx'");
iceb_sql_zapis(strsql,1,0,data->window);


sprintf(strsql,"delete from Alx where fil='zarsvfz_end.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='uosapp_h.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

jump22072019:; /*************14.6************************************************/

admin_alter_r_ins_str_alx("zarnast.alx","Коды премии за год","",data);

/*обновляем файл*/
sprintf(strsql,"delete from Alx where fil='pp.alx'"); /*грузим новый план счетов*/
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='pp03072009.alx'");
iceb_sql_zapis(strsql,1,0,data->window);


sprintf(strsql,"delete from Alx where fil='pt03072009.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

jump06082019:; /*************14.6************************************************/

sprintf(strsql,"delete from Alx where fil='pp.alx'"); /*грузим новый план счетов*/
iceb_sql_zapis(strsql,1,0,data->window);
sprintf(strsql,"delete from Alx where fil='pt.alx'"); /*грузим новый план счетов*/
iceb_sql_zapis(strsql,1,0,data->window);
sprintf(strsql,"delete from Alx where fil='pp03072009.alx'");
iceb_sql_zapis(strsql,1,0,data->window);


sprintf(strsql,"delete from Alx where fil='pt03072009.alx'");
iceb_sql_zapis(strsql,1,0,data->window);
jump30082019:; /*************14.7************************************************/

sprintf(strsql,"delete from Alx where fil='mord_start.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='mord_end.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"delete from Alx where fil='ppw.alx'");
iceb_sql_zapis(strsql,1,0,data->window);

jump10012020:; /*************14.10************************************************/

/* добавлены новые настроечные файлы*/

jump07032020:; /*************14.11************************************************/

/*Загрузка новых настроечных файлов*/
jump15052020:; /*************14.13************************************************/

/***************************************************************/
/***************************************************************/

/*Загружаем настроечные файлы в базу данных*/
iceb_alxzag(data->fromnsi.ravno(),0,data->view,data->buffer,data->window);

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






/*************************/
/*************************/


iceb_sql_zapis("USE mysql",1,0,data->window);





return(0);
}









/******************************************/
/******************************************/

gint admin_alter_r1(class admin_alter_r_data *data)
{
time(&data->vremn);
char strsql[4096];
iceb_u_str repl;
class SQLCURSOR cur;


sprintf(strsql,"%s\n",gettext("Окно ошибок"));
iceb_printw(strsql,data->buffer_error,data->view_error);


if(data->imabaz.getdlinna() <= 1)
 if(iceb_menu_danet(gettext("Преобразовать все базы данных?"),2,data->window) == 2)
  {
   gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
   gtk_widget_grab_focus(data->knopka);
   gtk_widget_show_all(data->window);
   return(FALSE);
  }

/*Проверяем возможность открытия настроечного файла*/   
class iceb_u_str imafilnast(data->fromnsi.ravno());
imafilnast.plus(G_DIR_SEPARATOR_S,"nastsys.alx");

FILE *ff;
if((ff = fopen(imafilnast.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(imafilnast.ravno(),gettext("Неправильно указан путь на настроечные файлы!"),errno,data->window);
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);
  gtk_widget_show_all(data->window);
  return(FALSE);
 }
fclose(ff);

/*Проверяем возможность открытия файла с документцией*/   
imafilnast.new_plus(data->fromdoc.ravno());
imafilnast.plus(G_DIR_SEPARATOR_S,"buhg.txt");

if((ff = fopen(imafilnast.ravno(),"r")) == NULL)
 {
  
  iceb_er_op_fil(imafilnast.ravno(),gettext("Неправильно указан путь на документцию!"),errno,data->window);
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);
  gtk_widget_show_all(data->window);
  return(FALSE);
 }
fclose(ff);


float kolstr1=0.;

if(data->imabaz.getdlinna() > 1)
 {
  data->kolvsehbaz=1;
  if(admin_alter_r_preobr(data) != 0)
   {
    gtk_widget_grab_focus(data->knopka);
    gtk_widget_show_all(data->window);
    return(FALSE);
   }
  iceb_pbar(data->bar,1,++kolstr1);    
 }
else
 {
  class SQLCURSOR cur,cur1;
  SQL_str row,row1;
  int kolstr=0;
  if((kolstr=cur.make_cursor(&bd,"SHOW DATABASES")) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),"SHOW DATABASES",data->window);
    gtk_widget_grab_focus(data->knopka);
    gtk_widget_show_all(data->window);
    return(FALSE);
   }
  data->kolvsehbaz=kolstr;
  data->nombaz=0;
  while(cur.read_cursor(&row) != 0)
   {
    iceb_pbar(data->bar,kolstr,++kolstr1);    

    sprintf(strsql,"select naikon from %s.Kontragent where kodkon='00'",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1) //именно так чтобы не показывала ненужных сообщений
      continue;
    data->imabaz.new_plus(row[0]);
    admin_alter_r_preobr(data);

   }    
 }



iceb_menu_soob(gettext("Преобразование завершено"),data->window);





sprintf(strsql,"%s:%d\n",gettext("Количество ошибок"),data->kol_oh);
iceb_printw(strsql,data->buffer_error,data->view_error);

iceb_printw_vr(data->vremn,data->buffer,data->view);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Преобразование завершено"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);







return(FALSE);

}

