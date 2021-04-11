/*$Id: l_zarmzpm_v.c,v 1.4 2013/08/25 08:26:46 sasa Exp $*/
/*12.07.2015	19.10.2011	Белых А.И.	l_zarmzpm_v.c
Ввод и корректировка в справочник минимальной зарплаты и прожиточного минимума
*/
#include "buhg_g.h"

enum
 {
  E_MES_GOD,
  E_MIN_ZAR,
  E_PROG_MIN,
  E_MAX_SUM,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_zarmzpm_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str mes_godz;  

  class iceb_u_str mes_godv;
  class iceb_u_str min_zar;
  class iceb_u_str prog_min;  
  class iceb_u_str max_sum;
  
  l_zarmzpm_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    mes_godz.new_plus("");
    clear();
   }

  void read_rek()
   {
    mes_godv.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MES_GOD])));
    min_zar.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MIN_ZAR])));
    prog_min.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PROG_MIN])));
    max_sum.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MAX_SUM])));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    clear();    
   }

  void clear()
   {
    mes_godv.new_plus("");
    min_zar.new_plus("");
    prog_min.new_plus("");    
    max_sum.new_plus("");
   }
 };


gboolean   l_zarmzpm_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarmzpm_v_data *data);
void  l_zarmzpm_v_knopka(GtkWidget *widget,class l_zarmzpm_v_data *data);
void    l_zarmzpm_v_vvod(GtkWidget *widget,class l_zarmzpm_v_data *data);
int l_zarmzpm_pk(const char *data,GtkWidget*);
int l_zarmzpm_zap(class l_zarmzpm_v_data *data);


extern SQL_baza  bd;

int l_zarmzpm_v(class iceb_u_str *mes_god,GtkWidget *wpredok)
{

class l_zarmzpm_v_data data;
char strsql[512];
iceb_u_str kikz;
data.mes_godz.new_plus(mes_god->ravno());
data.mes_godv.new_plus(mes_god->ravno());

if(mes_god->getdlinna() >  1)
 {
  short m,g;
  iceb_u_rsdat1(&m,&g,data.mes_godv.ravno());
  sprintf(strsql,"select * from Zarmzpm where data='%04d-%02d-01'",g,m);
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.min_zar.new_plus(row[1]);
  data.prog_min.new_plus(row[2]);
  data.max_sum.new_plus(row[3]);  
  kikz.plus(iceb_kikz(row[4],row[5],wpredok));
  
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

if(data.mes_godv.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zarmzpm_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 0);
//gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


label=gtk_label_new(gettext("Дата (м.г)"));
gtk_box_pack_start (GTK_BOX (hbox[E_MES_GOD]), label, FALSE, FALSE, 0);

data.entry[E_MES_GOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MES_GOD]),8);
gtk_box_pack_start (GTK_BOX (hbox[E_MES_GOD]), data.entry[E_MES_GOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_MES_GOD], "activate",G_CALLBACK(l_zarmzpm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MES_GOD]),data.mes_godv.ravno());
gtk_widget_set_name(data.entry[E_MES_GOD],iceb_u_inttochar(E_MES_GOD));

label=gtk_label_new(gettext("Минимальная зарплата"));
gtk_box_pack_start (GTK_BOX (hbox[E_MIN_ZAR]), label, FALSE, FALSE, 0);

data.entry[E_MIN_ZAR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MIN_ZAR]),4);
gtk_box_pack_start (GTK_BOX (hbox[E_MIN_ZAR]), data.entry[E_MIN_ZAR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_MIN_ZAR], "activate",G_CALLBACK(l_zarmzpm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MIN_ZAR]),data.min_zar.ravno());
gtk_widget_set_name(data.entry[E_MIN_ZAR],iceb_u_inttochar(E_MIN_ZAR));

label=gtk_label_new(gettext("Прожиточный минимум"));
gtk_box_pack_start (GTK_BOX (hbox[E_PROG_MIN]), label, FALSE, FALSE, 0);

data.entry[E_PROG_MIN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PROG_MIN]),6);
gtk_box_pack_start (GTK_BOX (hbox[E_PROG_MIN]), data.entry[E_PROG_MIN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PROG_MIN], "activate",G_CALLBACK(l_zarmzpm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PROG_MIN]),data.prog_min.ravno());
gtk_widget_set_name(data.entry[E_PROG_MIN],iceb_u_inttochar(E_PROG_MIN));

label=gtk_label_new(gettext("Максимальная сумма на которую начисляются соц. отчисления"));
gtk_box_pack_start (GTK_BOX (hbox[E_MAX_SUM]), label, FALSE, FALSE, 0);

data.entry[E_MAX_SUM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MAX_SUM]),6);
gtk_box_pack_start (GTK_BOX (hbox[E_MAX_SUM]), data.entry[E_MAX_SUM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_MAX_SUM], "activate",G_CALLBACK(l_zarmzpm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MAX_SUM]),data.max_sum.ravno());
gtk_widget_set_name(data.entry[E_MAX_SUM],iceb_u_inttochar(E_MAX_SUM));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_zarmzpm_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введённой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_zarmzpm_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_zarmzpm_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.voz == 0)
 mes_god->new_plus(data.mes_godv.ravno());
return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zarmzpm_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarmzpm_v_data *data)
{
//char  bros[512];

//printf("l_zarmzpm_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");

    return(TRUE);
  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");

    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");

    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
  //  printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_zarmzpm_v_knopka(GtkWidget *widget,class l_zarmzpm_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_zarmzpm_v_knopka knop=%d\n",knop);

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_zarmzpm_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_zarmzpm_v_vvod(GtkWidget *widget,class l_zarmzpm_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("l_zarmzpm_v_vvod enter=%d\n",enter);

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_zarmzpm_zap(class l_zarmzpm_v_data *data)
{
char strsql[1024];
short m,g;

if(iceb_u_rsdat1(&m,&g,data->mes_godv.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
  return(1);
 }

short mz,gz;
iceb_u_rsdat1(&mz,&gz,data->mes_godz.ravno());


if(gz != g || mz != m)
  if(l_zarmzpm_pk(data->mes_godv.ravno(),data->window) != 0)
     return(1);


if(data->mes_godz.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into Zarmzpm \
values ('%04d-%02d-01',%.10g,%.10g,%.10g,%d,%ld)",
   g,m,
   data->min_zar.ravno_atof(),
   data->prog_min.ravno_atof(),
   data->max_sum.ravno_atof(),
   iceb_getuid(data->window),time(NULL));
 }
else
 {
  
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Zarmzpm \
set \
data='%04d-%02d-01',\
mz=%.10g,\
pm=%.10g,\
ms=%.10g,\
ktoz=%d,\
vrem=%ld \
where data='%04d-%02d-01'",
   g,m,
   data->min_zar.ravno_atof(),
   data->prog_min.ravno_atof(),
   data->max_sum.ravno_atof(),
   iceb_getuid(data->window),time(NULL),
   gz,mz);
   
 }

//printf("kontr_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_zarmzpm_pk(const char *data,GtkWidget *wpredok)
{
char strsql[512];
short m,g;
iceb_u_rsdat1(&m,&g,data);


sprintf(strsql,"select kod from Zarmzpm where data='%04d-%02d-01'",g,m);
//printf("l_zarmzpm_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_menu_soob(gettext("Такая запись уже есть !"),wpredok);
  return(1);
 }

return(0);

}
