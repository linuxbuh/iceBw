/*$Id: l_zarlgot_v.c,v 1.3 2013/08/25 08:26:46 sasa Exp $*/
/*03.03.2016	10.11.2011	Белых А.И.	l_zarlgot_v.c
Ввод и корректировка в справочник льгот
*/
#include "buhg_g.h"

enum
 {
  E_TABNOM,
  E_DNDL,
  E_PROC,
  E_KOL_LGOT,
  E_KOD_LGOT,
  E_KOMENT,
  KOLENTER
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_zarlgot_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str tabnomz;  
  class iceb_u_str dndlz;
  
  class iceb_u_str tabnomv;
  class iceb_u_str dndl;
  class iceb_u_str proc;  
  class iceb_u_str kol_lgot;
  class iceb_u_str kod_lgot;
  class iceb_u_str koment;
    
  l_zarlgot_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    tabnomz.new_plus("");
    clear();
  
   }

  void read_rek()
   {
    tabnomv.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM])));
    dndl.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DNDL])));
    proc.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PROC])));
    kol_lgot.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_LGOT])));
    koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
    kod_lgot.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_LGOT])));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    clear();    
   }

  void clear()
   {
    tabnomv.new_plus("");
    dndl.new_plus("");
    kol_lgot.new_plus("");
    kod_lgot.new_plus("");
    proc.new_plus("");
    koment.new_plus("");
   }
 };


gboolean   l_zarlgot_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarlgot_v_data *data);
void  l_zarlgot_v_knopka(GtkWidget *widget,class l_zarlgot_v_data *data);
void    l_zarlgot_v_vvod(GtkWidget *widget,class l_zarlgot_v_data *data);
int l_zarlgot_zap(class l_zarlgot_v_data *data);
void  l_zarlgot_v_e_knopka(GtkWidget *widget,class l_zarlgot_v_data *data);


extern SQL_baza  bd;

int l_zarlgot_v(class iceb_u_str *tabnom,class iceb_u_str *dndl,GtkWidget *wpredok)
{

class l_zarlgot_v_data data;
char strsql[512];
class iceb_u_str kikz;
int gor=0,ver=0;
data.tabnomz.new_plus(tabnom->ravno());
data.tabnomv.new_plus(tabnom->ravno());
data.dndlz.new_plus(dndl->ravno());
data.name_window.plus(__FUNCTION__);

if(tabnom->getdlinna() >  1)
 {
  sprintf(strsql,"select * from Zarlgot where tn=%d and dndl='%s'",tabnom->ravno_atoi(),dndl->ravno_sqldata());
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  
  data.dndl.new_plus(iceb_u_datzap_mg(row[1]));
  data.proc.new_plus(row[2]);

  data.kol_lgot.new_plus(row[3]);
  data.kod_lgot.new_plus(row[4]);
  data.koment.new_plus(row[5]);
       
  kikz.plus(iceb_kikz(row[6],row[7],wpredok));
  
 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

if(data.tabnomv.getdlinna() <= 1)
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zarlgot_v_key_press),&data);

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
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);


data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(gettext("Табельный номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_TABNOM],"clicked",G_CALLBACK(l_zarlgot_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_TABNOM],iceb_u_inttochar(E_TABNOM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_TABNOM],gettext("Выбор табельного номера"));

data.entry[E_TABNOM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_TABNOM]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TABNOM], "activate",G_CALLBACK(l_zarlgot_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.tabnomv.ravno());
gtk_widget_set_name(data.entry[E_TABNOM],iceb_u_inttochar(E_TABNOM));

//label=gtk_label_new(gettext("Код удержания"));
//gtk_box_pack_start (GTK_BOX (hbox[E_DNDL]), label, FALSE, FALSE, 0);

data.knopka_enter[E_DNDL]=gtk_button_new_with_label(gettext("Дата начала действия льготы (м.г)"));
gtk_box_pack_start (GTK_BOX (hbox[E_DNDL]), data.knopka_enter[E_DNDL], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DNDL],"clicked",G_CALLBACK(l_zarlgot_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DNDL],iceb_u_inttochar(E_DNDL));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DNDL],gettext("Выбор даты"));

data.entry[E_DNDL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DNDL]),7);
gtk_box_pack_start (GTK_BOX (hbox[E_DNDL]), data.entry[E_DNDL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DNDL], "activate",G_CALLBACK(l_zarlgot_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DNDL]),data.dndl.ravno());
gtk_widget_set_name(data.entry[E_DNDL],iceb_u_inttochar(E_DNDL));

label=gtk_label_new(gettext("Процент"));
gtk_box_pack_start (GTK_BOX (hbox[E_PROC]), label, FALSE, FALSE, 0);

data.entry[E_PROC] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PROC]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_PROC]), data.entry[E_PROC], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PROC], "activate",G_CALLBACK(l_zarlgot_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PROC]),data.proc.ravno());
gtk_widget_set_name(data.entry[E_PROC],iceb_u_inttochar(E_PROC));




label=gtk_label_new(gettext("Количество льгот"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_LGOT]), label, FALSE, FALSE, 0);

data.entry[E_KOL_LGOT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOL_LGOT]),2);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_LGOT]), data.entry[E_KOL_LGOT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOL_LGOT], "activate",G_CALLBACK(l_zarlgot_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_LGOT]),data.kol_lgot.ravno());
gtk_widget_set_name(data.entry[E_KOL_LGOT],iceb_u_inttochar(E_KOL_LGOT));

label=gtk_label_new(gettext("Код льготы"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_LGOT]), label, FALSE, FALSE, 0);

data.entry[E_KOD_LGOT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_LGOT]),2);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_LGOT]), data.entry[E_KOD_LGOT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_LGOT], "activate",G_CALLBACK(l_zarlgot_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_LGOT]),data.kol_lgot.ravno());
gtk_widget_set_name(data.entry[E_KOD_LGOT],iceb_u_inttochar(E_KOD_LGOT));

label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),255);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(l_zarlgot_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_zarlgot_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введённой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_zarlgot_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_zarlgot_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.voz == 0)
 tabnom->new_plus(data.tabnomv.ravno());
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_zarlgot_v_e_knopka(GtkWidget *widget,class l_zarlgot_v_data *data)
{
class iceb_u_str naim("");
class iceb_u_str kod("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_TABNOM:
    if(l_sptbn(1,&kod,&naim,0,data->window) == 0)
     data->tabnomv.new_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->tabnomv.ravno());
    return;  

  case E_DNDL:
    iceb_calendar1(&data->dndl,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DNDL]),data->dndl.ravno());
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zarlgot_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarlgot_v_data *data)
{

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
void  l_zarlgot_v_knopka(GtkWidget *widget,class l_zarlgot_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    if(l_zarlgot_zap(data) == 0)
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

void    l_zarlgot_v_vvod(GtkWidget *widget,class l_zarlgot_v_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_zarlgot_zap(class l_zarlgot_v_data *data)
{
char strsql[1024];
short d=0,m=0,g=0;

if(data->dndl.ravno()[0] != '\0')
 {
  if(iceb_u_rsdat1(&m,&g,data->dndl.ravno()) != 0)
   {
    iceb_menu_soob(gettext("Не правильно введена дата"),data->window);
    return(1);
   }
  if(m != 0)
   d=1;
 }
 
short dz=0,mz=0,gz=0;
iceb_u_rsdat1(&mz,&gz,data->dndlz.ravno());
if(mz != 0)
 dz=1;

/*проверяем табельный номер*/
sprintf(strsql,"select tabn from Kartb where tabn=%d",data->tabnomv.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d!",gettext("Не найден табельный номер"),data->tabnomv.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

/*проверяем может уже такая запись есть*/
if(data->tabnomv.ravno_atoi() != data->tabnomz.ravno_atoi() || (mz != m || gz != g))
 {
  sprintf(strsql,"select tn from Zarlgot where tn=%d and dndl='%04d-%02d-%02d'",data->tabnomv.ravno_atoi(),g,m,d);
  if(iceb_sql_readkey(strsql,data->window) > 0)
   {
    iceb_menu_soob(gettext("Такая запись уже есть!"),data->window);
    return(1);
   }
 }

if(data->tabnomz.ravno_atoi() == 0) /*новая запись*/
 {

  sprintf(strsql,"insert into Zarlgot values(%d,'%04d-%02d-%02d',%.2f,%d,'%s','%s',%d,%ld)",
  data->tabnomv.ravno_atoi(),
  g,m,d,
  data->proc.ravno_atof(),
  data->kol_lgot.ravno_atoi(),
  data->kod_lgot.ravno(),
  data->koment.ravno_filtr(),
  iceb_getuid(data->window),time(NULL));
  
  
 }
else /*корректировка записи*/
 {
  sprintf(strsql,"update Zarlgot set \
tn=%d,\
dndl='%04d-%02d-%02d',\
pl=%.2f,\
koll=%d,\
kodl='%s',\
kom='%s',\
ktoz=%d,\
vrem=%ld \
where tn=%d and dndl='%04d-%02d-%02d'",
  data->tabnomv.ravno_atoi(),
  g,m,d,
  data->proc.ravno_atof(),
  data->kol_lgot.ravno_atoi(),
  data->kod_lgot.ravno(),
  data->koment.ravno_filtr(),
  iceb_getuid(data->window),
  time(NULL),
  data->tabnomz.ravno_atoi(),gz,mz,dz);      
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);
return(0);

}
