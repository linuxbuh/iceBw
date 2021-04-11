/*$Id:$*/
/*28.02.2016	22.04.2015	Белых А.И.	l_f1dfvs_v.c
Ввод и корректировка военного сбора
*/
#include "buhg_g.h"

enum
 {
  E_INN,
  E_FIO,
  E_NAH_DOHOD,
  E_NAH_VS,
  E_VIP_DOHOD,
  E_VIP_VS,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_f1dfvs_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str nomd;  
  class iceb_u_str inns;
    
  class iceb_u_str inn;
  class iceb_u_str fio;
  class iceb_u_str nah_dohod;
  class iceb_u_str nah_vs;
  class iceb_u_str vip_dohod;
  class iceb_u_str vip_vs;
  
  l_f1dfvs_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_data();
   }

  void read_rek()
   {
    inn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_INN])));
    fio.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_FIO])));
    nah_dohod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAH_DOHOD])));
    nah_vs.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAH_VS])));
    vip_dohod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_VIP_DOHOD])));
    vip_vs.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_VIP_VS])));
   }
  
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    
   }
  void clear_data()
   {
    inn.new_plus("");
    fio.new_plus("");
    nah_dohod.new_plus("");
    nah_vs.new_plus("");
    vip_dohod.new_plus("");
    vip_vs.new_plus("");
   }
 };


gboolean   l_f1dfvs_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1dfvs_v_data *data);
void  l_f1dfvs_v_knopka(GtkWidget *widget,class l_f1dfvs_v_data *data);
void    l_f1dfvs_v_vvod(GtkWidget *widget,class l_f1dfvs_v_data *data);
int l_f1dfvs_zap(class l_f1dfvs_v_data *data);


extern SQL_baza  bd;

int l_f1dfvs_v(const char *nomd,class iceb_u_str *inn,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class l_f1dfvs_v_data data;
char strsql[1024];
class iceb_u_str kikz;

data.nomd.new_plus(nomd);
data.inn.new_plus(inn->ravno());
data.inns.new_plus(inn->ravno());
data.name_window.plus(__FUNCTION__);


if(data.inn.getdlinna() >  1)
 {
  sprintf(strsql,"select * from F1dfvs where nomd='%s' and inn='%s'",data.nomd.ravno(),data.inn.ravno());
  printf("%s-%s\n",__FUNCTION__,strsql);
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }

  data.fio.new_plus(row[2]);
  data.nah_dohod.new_plus(row[3]);
  data.nah_vs.new_plus(row[4]);
  data.vip_dohod.new_plus(row[5]);
  data.vip_vs.new_plus(row[6]);
  
  kikz.plus(iceb_kikz(row[7],row[8],wpredok));

  if(row[2][0] == '\0')
   {
    SQL_str row1;
    class SQLCURSOR cur1;
    /*Читаем фамилию*/

    sprintf(strsql,"select fio from Kartb where inn='%s'",data.inn.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) < 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден идентификационный номер"),data.inn.ravno());
      iceb_menu_soob(strsql,data.window);
     }
    else
      data.fio.new_plus(row1[0]);
   }
  
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

if(data.nomd.getdlinna() <= 1)
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_f1dfvs_v_key_press),&data);

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


label=gtk_label_new(gettext("Идентификационный номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), label, FALSE, FALSE, 0);

data.entry[E_INN] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_INN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), data.entry[E_INN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_INN], "activate",G_CALLBACK(l_f1dfvs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INN]),data.inn.ravno());
gtk_widget_set_name(data.entry[E_INN],iceb_u_inttochar(E_INN));/******************/

label=gtk_label_new(gettext("Фамилия"));
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), label, FALSE, FALSE, 0);

data.entry[E_FIO] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_FIO]),60);
gtk_box_pack_start (GTK_BOX (hbox[E_FIO]), data.entry[E_FIO], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FIO], "activate",G_CALLBACK(l_f1dfvs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FIO]),data.fio.ravno());
gtk_widget_set_name(data.entry[E_FIO],iceb_u_inttochar(E_FIO));/******************/

label=gtk_label_new(gettext("Начисленный доход"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAH_DOHOD]), label, FALSE, FALSE, 0);

data.entry[E_NAH_DOHOD] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAH_DOHOD]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NAH_DOHOD]), data.entry[E_NAH_DOHOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAH_DOHOD], "activate",G_CALLBACK(l_f1dfvs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAH_DOHOD]),data.nah_dohod.ravno());
gtk_widget_set_name(data.entry[E_NAH_DOHOD],iceb_u_inttochar(E_NAH_DOHOD));/******************/

label=gtk_label_new(gettext("Начисленный сбор"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAH_VS]), label, FALSE, FALSE, 0);

data.entry[E_NAH_VS] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAH_VS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NAH_VS]), data.entry[E_NAH_VS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAH_VS], "activate",G_CALLBACK(l_f1dfvs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAH_VS]),data.nah_vs.ravno());
gtk_widget_set_name(data.entry[E_NAH_VS],iceb_u_inttochar(E_NAH_VS));/******************/


label=gtk_label_new(gettext("Выплаченный доход"));
gtk_box_pack_start (GTK_BOX (hbox[E_VIP_DOHOD]), label, FALSE, FALSE, 0);

data.entry[E_VIP_DOHOD] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_VIP_DOHOD]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_VIP_DOHOD]), data.entry[E_VIP_DOHOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_VIP_DOHOD], "activate",G_CALLBACK(l_f1dfvs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VIP_DOHOD]),data.vip_dohod.ravno());
gtk_widget_set_name(data.entry[E_VIP_DOHOD],iceb_u_inttochar(E_VIP_DOHOD));/******************/

label=gtk_label_new(gettext("Выплаченный сбор"));
gtk_box_pack_start (GTK_BOX (hbox[E_VIP_VS]), label, FALSE, FALSE, 0);

data.entry[E_VIP_VS] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_VIP_VS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_VIP_VS]), data.entry[E_VIP_VS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_VIP_VS], "activate",G_CALLBACK(l_f1dfvs_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VIP_VS]),data.vip_vs.ravno());
gtk_widget_set_name(data.entry[E_VIP_VS],iceb_u_inttochar(E_VIP_VS));/******************/

sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_f1dfvs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введённой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_f1dfvs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_f1dfvs_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 inn->new_plus(data.inn.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


return(data.voz);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_f1dfvs_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1dfvs_v_data *data)
{
//char  bros[512];

//printf("l_f1dfvs_v_key_press\n");
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
void  l_f1dfvs_v_knopka(GtkWidget *widget,class l_f1dfvs_v_data *data)
{

//int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_f1dfvs_v_knopka knop=%d\n",knop);
int knop=atoi(gtk_widget_get_name(widget));
switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_f1dfvs_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_f1dfvs_v_vvod(GtkWidget *widget,class l_f1dfvs_v_data *data)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_INN:
    data->inn.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    if(strlen(data->inn.ravno()) < 8)
     {
      sprintf(strsql,"select fio,inn from Kartb where tabn=%d",data->inn.ravno_atoi());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       {
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_INN]),row[1]);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_FIO]),row[0]);
       }
      break;
     }

    sprintf(strsql,"select fio from Kartb where inn='%s'",data->inn.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) >= 1)
     {
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_FIO]),row[0]);
     }
    break;



  case E_FIO:
    data->fio.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_f1dfvs_zap(class l_f1dfvs_v_data *data)
{
char strsql[1024];
class iceb_u_str fio("");
SQL_str row;
class SQLCURSOR cur;

if(data->inn.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён идентификационной номер !"),data->window);
  return(1);
 }


if(iceb_u_SRAV(data->inns.ravno(),data->inn.ravno(),0) != 0)
 {
  sprintf(strsql,"select inn from F1dfvs where nomd='%s' and inn='%s'",data->nomd.ravno(),data->inn.ravno());
  if(iceb_sql_readkey(strsql,data->window) >= 1)
   {
    iceb_menu_soob(gettext("Такая запись уже есть!"),data->window);
    return(1);
   }
 }

sprintf(strsql,"select fio from Kartb where inn='%s'",data->inn.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) < 1)
   fio.new_plus(data->fio.ravno());
else
   fio.new_plus("");
        

if(data->inns.getdlinna() <= 1)
 {

  sprintf(strsql,"insert into F1dfvs \
values ('%s','%s','%s',%.2f,%.2f,%.2f,%.2f,%d,%ld)",
   data->nomd.ravno(),
   data->inn.ravno(),
   fio.ravno_filtr(),
   data->nah_dohod.ravno_atof(),
   data->nah_vs.ravno_atof(),
   data->vip_dohod.ravno_atof(),
   data->vip_dohod.ravno_atof(),
   iceb_getuid(data->window),
   time(NULL));
  }
 else            
  sprintf(strsql,"update F1dfvs set \
inn='%s',\
fio='%s',\
sfr=%.2f,\
svs=%.2f,\
sfrv=%.2f,\
svsv=%.2f,\
ktoz=%d,\
vrem=%ld \
where nomd='%s' and inn='%s'",
  data->inn.ravno(),
  fio.ravno_filtr(),
  data->nah_dohod.ravno_atof(),
  data->nah_vs.ravno_atof(),
  data->vip_dohod.ravno_atof(),
  data->vip_dohod.ravno_atof(),
  iceb_getuid(data->window),
  time(NULL),
  data->nomd.ravno(),
  data->inns.ravno());
   

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);

}
