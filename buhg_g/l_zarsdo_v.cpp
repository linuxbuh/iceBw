/*$Id: l_zarsdo_v.c,v 1.27 2014/05/07 10:58:06 sasa Exp $*/
/*03.06.2016	30.05.2016	Белых А.И.	l_zarsdo_v.c
Ввод и корректировка должностных окладов
*/
#include "buhg_g.h"
#include "l_zarsdo.h"
enum
 {
  E_TABNOM,
  E_DATAZ,
  E_KOD_NAH,
  E_SUMA,
  E_METKA_SP,
  E_KOL_HAS,
  E_KOEF,
  E_SHET,
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

class l_zarsdo_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_tabnom;
  GtkWidget *label_kod_nah;
  GtkWidget *label_shet;
  GtkWidget *opt;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str tabnomk;
  class iceb_u_str datazk;
  class iceb_u_str kod_nahk;    

  class iceb_u_str tabnom;
  class iceb_u_str dataz;
  class iceb_u_str kod_nah;
  class iceb_u_str suma;
  int metka_sp;
  class iceb_u_str kol_has;
  class iceb_u_str koef;
  class iceb_u_str shet;
  class iceb_u_str koment;
  
  l_zarsdo_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear(); 
   }
  void clear()
   {
    tabnom.new_plus("");
    dataz.new_plus("");
    kod_nah.new_plus("");
    suma.new_plus("");
    metka_sp=0;
    kol_has.new_plus("");
    koef.new_plus("");
    shet.new_plus("");
    koment.new_plus("");
   }

  void read_rek()
   {
    tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_TABNOM])));
    dataz.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAZ])));
    suma.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA])));
    kod_nah.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_NAH])));

    metka_sp=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));

    kol_has.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOL_HAS])));
    koef.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOEF])));
    shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
    
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    
   }
 };


gboolean   l_zarsdo_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsdo_v_data *data);
void  l_zarsdo_v_knopka(GtkWidget *widget,class l_zarsdo_v_data *data);
void    l_zarsdo_v_vvod(GtkWidget *widget,class l_zarsdo_v_data *data);
int l_zarsdo_zap(class l_zarsdo_v_data *data);
void  l_zarsdo_v_e_knopka(GtkWidget *widget,class l_zarsdo_v_data *data);


extern SQL_baza  bd;

int l_zarsdo_v(const char *tabnom,const char *dataz,const char *kod_nah,GtkWidget *wpredok)
{
int gor=0;
int ver=0;

class l_zarsdo_v_data data;
char strsql[512];
class iceb_u_str kikz;

data.name_window.plus(__FUNCTION__);
data.tabnomk.new_plus(tabnom);
data.datazk.new_plus(dataz);
data.kod_nahk.new_plus(kod_nah);

if(tabnom[0] != '\0')
 {
  sprintf(strsql,"select * from Zarsdo where tn=%d and dt='%s' and kn=%d",atoi(tabnom),iceb_u_tosqldat(dataz),atoi(kod_nah));
  SQL_str row;
  SQLCURSOR cur;
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }
  data.tabnom.new_plus(row[0]);
  data.dataz.new_plus(iceb_u_datzap_mg(row[1]));
  data.kod_nah.new_plus(row[2]);
  data.suma.new_plus(row[3]);

  data.metka_sp=atoi(row[4]);
  data.kol_has.new_plus(row[5]);
  
  data.koef.new_plus(row[6]);
  data.shet.new_plus(row[7]);
  data.koment.new_plus(row[8]);
  


  kikz.plus(iceb_kikz(row[9],row[10],wpredok));
  
 }
else
 {
  data.koef.new_plus("1.");
 } 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

if(tabnom[0] == '\0')
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zarsdo_v_key_press),&data);

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

data.label_tabnom=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (vbox), data.label_tabnom, FALSE, FALSE, 0);
data.label_kod_nah=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (vbox), data.label_kod_nah, FALSE, FALSE, 0);
data.label_shet=gtk_label_new("");
gtk_box_pack_start (GTK_BOX (vbox), data.label_shet, FALSE, FALSE, 0);


for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);



data.knopka_enter[E_TABNOM]=gtk_button_new_with_label(gettext("Табельный номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.knopka_enter[E_TABNOM], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_TABNOM],"clicked",G_CALLBACK(l_zarsdo_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_TABNOM],iceb_u_inttochar(E_TABNOM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_TABNOM],gettext("Поиск в списке работников"));

data.entry[E_TABNOM] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_TABNOM]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_TABNOM]), data.entry[E_TABNOM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TABNOM], "activate",G_CALLBACK(l_zarsdo_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TABNOM]),data.tabnom.ravno());
gtk_widget_set_name(data.entry[E_TABNOM],iceb_u_inttochar(E_TABNOM));/******************/



sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("м.г"));
data.knopka_enter[E_DATAZ]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAZ]), data.knopka_enter[E_DATAZ], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAZ],"clicked",G_CALLBACK(l_zarsdo_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAZ],iceb_u_inttochar(E_DATAZ));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAZ],gettext("Выбор даты"));

data.entry[E_DATAZ] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAZ]),7);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAZ]), data.entry[E_DATAZ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAZ], "activate",G_CALLBACK(l_zarsdo_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAZ]),data.dataz.ravno());
gtk_widget_set_name(data.entry[E_DATAZ],iceb_u_inttochar(E_DATAZ));/******************/


data.knopka_enter[E_KOD_NAH]=gtk_button_new_with_label(gettext("Код начисления"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_NAH]), data.knopka_enter[E_KOD_NAH], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_NAH],"clicked",G_CALLBACK(l_zarsdo_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_NAH],iceb_u_inttochar(E_KOD_NAH));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_NAH],gettext("Поиск в справочнике начислений"));

data.entry[E_KOD_NAH] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_NAH]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_NAH]), data.entry[E_KOD_NAH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_NAH], "activate",G_CALLBACK(l_zarsdo_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_NAH]),data.kod_nah.ravno());
gtk_widget_set_name(data.entry[E_KOD_NAH],iceb_u_inttochar(E_KOD_NAH));/******************/

label=gtk_label_new(gettext("Сумма"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE, 0);

data.entry[E_SUMA] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMA]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMA], "activate",G_CALLBACK(l_zarsdo_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma.ravno());
gtk_widget_set_name(data.entry[E_SUMA],iceb_u_inttochar(E_SUMA));/******************/


label=gtk_label_new(gettext("Способ расчёта"));
gtk_box_pack_start (GTK_BOX (hbox[E_METKA_SP]), label, FALSE, FALSE, 0);

class l_zarsdo_menu_mr spmenu;
/***********************
class iceb_u_spisok spmenu;
spmenu.plus(gettext("Оплата за месяц, расчёт по дням"));
spmenu.plus(gettext("Оплата за месяц, расчёт по часам"));
spmenu.plus(gettext("Оплата за месяц, без расчёта"));
spmenu.plus(gettext("Оплата за час"));
spmenu.plus(gettext("Оплата за день"));
spmenu.plus(gettext("Оплата по тарифу, расчёт по дням"));
spmenu.plus(gettext("Оплата по тарифу, расчёт по часам"));
******************/
iceb_pm_vibor(&spmenu.strsr,&data.opt,data.metka_sp);
gtk_box_pack_start (GTK_BOX (hbox[E_METKA_SP]),data.opt, TRUE, TRUE,1);



label=gtk_label_new(gettext("Количество часов в рабочем дне"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_HAS]), label, FALSE, FALSE, 0);

data.entry[E_KOL_HAS] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOL_HAS]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOL_HAS]), data.entry[E_KOL_HAS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOL_HAS], "activate",G_CALLBACK(l_zarsdo_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOL_HAS]),data.kol_has.ravno());
gtk_widget_set_name(data.entry[E_KOL_HAS],iceb_u_inttochar(E_KOL_HAS));/******************/

label=gtk_label_new(gettext("Коэффициент ставки"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOEF]), label, FALSE, FALSE, 0);

data.entry[E_KOEF] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOEF]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOEF]), data.entry[E_KOEF], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOEF], "activate",G_CALLBACK(l_zarsdo_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOEF]),data.koef.ravno());
gtk_widget_set_name(data.entry[E_KOEF],iceb_u_inttochar(E_KOEF));/******************/



data.knopka_enter[E_SHET]=gtk_button_new_with_label(gettext("Счёт"));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(l_zarsdo_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"));


data.entry[E_SHET] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(l_zarsdo_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));/******************/



label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(l_zarsdo_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));/******************/



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_zarsdo_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введённой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_zarsdo_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_zarsdo_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_zarsdo_v_e_knopka(GtkWidget *widget,class l_zarsdo_v_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_TABNOM:

     if(l_sptbn(1,&data->tabnom,&naim,0,data->window) == 0)
      gtk_label_set_text(GTK_LABEL(data->label_tabnom),naim.ravno());
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_TABNOM]),data->tabnom.ravno());
      
    return;  

  case E_DATAZ:

    if(iceb_calendar1(&data->dataz,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAZ]),data->dataz.ravno());
      
    return;  


   
  case E_SHET:

    if(iceb_vibrek(1,"Plansh",&data->shet,&naim,data->window) == 0)
      gtk_label_set_text(GTK_LABEL(data->label_shet),naim.ravno());
      
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno());

    return;  

  case E_KOD_NAH:

    if(l_zarnah(1,&data->kod_nah,&naim,data->window) == 0)
      gtk_label_set_text(GTK_LABEL(data->label_kod_nah),naim.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_NAH]),data->kod_nah.ravno());
    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zarsdo_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zarsdo_v_data *data)
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
void  l_zarsdo_v_knopka(GtkWidget *widget,class l_zarsdo_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
switch (knop)
 {
  case FK2:
    data->read_rek();
    if(l_zarsdo_zap(data) == 0)
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

void    l_zarsdo_v_vvod(GtkWidget *widget,class l_zarsdo_v_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;


int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_TABNOM:
    data->tabnom.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select fio from Kartb where tabn=%d",data->tabnom.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      iceb_menu_soob(gettext("Нет такого табельного номера!"),data->window);
      gtk_label_set_text(GTK_LABEL(data->label_tabnom),"");
      return;
     }
    gtk_label_set_text(GTK_LABEL(data->label_tabnom),row[0]);
    break;

  case E_DATAZ:
    data->dataz.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOD_NAH:
    data->kod_nah.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select naik from Nash where kod=%d",data->kod_nah.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      iceb_menu_soob(gettext("Нет такого кода начисления!"),data->window);
      gtk_label_set_text(GTK_LABEL(data->label_kod_nah),"");
      return;
     }
    gtk_label_set_text(GTK_LABEL(data->label_kod_nah),row[0]);
    break;

  case E_SHET:
    data->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select nais from Plansh where ns='%s'",data->shet.ravno_filtr());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
     {
      iceb_menu_soob(gettext("Нет такого счёта в плане счетов!"),data->window);
      gtk_label_set_text(GTK_LABEL(data->label_shet),"");
      return;
     }
    gtk_label_set_text(GTK_LABEL(data->label_shet),row[0]);
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
int l_zarsdo_zap(class l_zarsdo_v_data *data)
{
char strsql[1024];
short m=0,g=0;




if(data->tabnom.ravno()[0] == '\0' ||\
   data->dataz.ravno()[0] == '\0' ||\
   data->kod_nah.ravno()[0] == '\0' ||\
   data->suma.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введены все обязательные реквизиты !"),data->window);
  return(1);
 }       


sprintf(strsql,"select fio from Kartb where tabn=%d",data->tabnom.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d",gettext("Не найден табельный номер"),data->tabnom.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

sprintf(strsql,"select naik from Nash where kod=%d",data->kod_nah.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %d",gettext("Не найден код начисления в справочнике"),data->kod_nah.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

if(iceb_u_rsdat1(&m,&g,data->dataz.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
  return(1);
 }

if(data->shet.ravno()[0] != '\0' && data->shet.ravno()[0] != '*')
 {
  if(iceb_prsh1(data->shet.ravno(),data->window) != 0)
   return(1);
 }


if(iceb_u_SRAV(data->tabnomk.ravno(),data->tabnom.ravno(),0) != 0)
 {
  //Проверяем может такая запись уже есть
  sprintf(strsql,"select dt from Zarsdo where dt='%s' and tn=%d and kn=%d",iceb_u_tosqldat(data->dataz.ravno()),data->tabnom.ravno_atoi(),data->kod_nah.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) >= 1)
   {
    iceb_menu_soob(gettext("Такая запись уже есть!"),data->window);
    return(1);
   }

 }
 
if(data->tabnomk.ravno()[0] == '\0') //Ввод новой записи
  sprintf(strsql,"insert into Zarsdo values(%d,'%s',%d,%.2f,%d,%.2f,%.2f,'%s','%s',%d,%ld)",
  data->tabnom.ravno_atoi(),  
  iceb_u_tosqldat(data->dataz.ravno()),
  data->kod_nah.ravno_atoi(),
  data->suma.ravno_atof(),
  data->metka_sp,
  data->kol_has.ravno_atof(),
  data->koef.ravno_atof(),
  data->shet.ravno_filtr(),
  data->koment.ravno_filtr(),
  iceb_getuid(data->window),time(NULL));
else
 {
  class iceb_u_str datan(iceb_u_tosqldat(data->datazk.ravno())); /*только так*/
  sprintf(strsql,"update Zarsdo set \
dt='%s',\
tn=%d,\
kn=%d,\
sm=%.2f,\
mt=%d,\
kh=%.2f,\
ks=%.2f,\
sh='%s',\
km='%s',\
ktoz=%d,\
vrem=%ld \
where dt='%s' and tn=%d and kn=%d",  
  iceb_u_tosqldat(data->dataz.ravno()),
  data->tabnom.ravno_atoi(),  
  data->kod_nah.ravno_atoi(),
  data->suma.ravno_atof(),
  data->metka_sp,
  data->kol_has.ravno_atof(),
  data->koef.ravno_atof(),
  data->shet.ravno_filtr(),
  data->koment.ravno_filtr(),
  iceb_getuid(data->window),time(NULL),
  datan.ravno(),
  data->tabnomk.ravno_atoi(),
  data->kod_nah.ravno_atoi());
 }


if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);

}
