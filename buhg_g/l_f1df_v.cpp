/*$Id: l_f1df_v.c,v 1.17 2013/08/25 08:26:35 sasa Exp $*/
/*28.02.2016	21.12.2006	Белых А.И.	l_f1df_v.c
Меню для ввода шапки формы 1ДФ
*/
#include "buhg_g.h"
#include "l_f1df.h"
enum
 {
  E_GOD,
  E_KVRT,
  E_NOMDOK,
  E_DATD,
  E_NOMDOK_DPA,
  E_DATD_DPA,
  E_KOLR,
  E_KOLS,
  KOLENTER  
 };

enum
 {
  FK2,
  FK3,
  FK4,
  FK10,
  KOL_FK
 };

class l_f1df_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *radiobutton2[3];
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_f1df_rek rk;
  class iceb_u_str nomdok;
    
  l_f1df_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.god.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GOD])));
    rk.kvrt.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KVRT])));
    rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK])));
    rk.datd.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATD])));
    rk.nomdok_dpa.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK_DPA])));
    rk.datd_dpa.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATD_DPA])));
    rk.kolr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOLR])));
    rk.kols.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOLS])));
    
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[0])) == TRUE)
     rk.metka_vd=0;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[1])) == TRUE)
     rk.metka_vd=1;
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2[2])) == TRUE)
     rk.metka_vd=2;
   
   }
  
  void clear_rek()
   {

    rk.clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton2[0]),TRUE); //Устанавливем активной кнопку

   }
 };


gboolean   l_f1df_m_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1df_m_data *data);
void  l_f1df_m_knopka(GtkWidget *widget,class l_f1df_m_data *data);
void    l_f1df_m_vvod(GtkWidget *widget,class l_f1df_m_data *data);


void  l_f1df_m_e_knopka(GtkWidget *widget,class l_f1df_m_data *data);
int nomdf8dr(GtkWidget *wpredok);
int l_f1df_v_zap(class l_f1df_m_data *data);

extern SQL_baza  bd;

int l_f1df_v(class iceb_u_str *nomdok,GtkWidget *wpredok)
{
int gor=0,ver=0;
class l_f1df_m_data data;
char strsql[512];
class iceb_u_str kikz;
data.nomdok.new_plus(nomdok->ravno());
data.name_window.plus(__FUNCTION__);
 

if(data.nomdok.getdlinna() > 1)
 {
  class SQLCURSOR cur;
  SQL_str row;
  sprintf(strsql,"select * from F8dr where nomd='%s'",data.nomdok.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_menu_soob(gettext("Не нашли запись для корректировки !"),wpredok);
    return(1);
   }
  data.rk.god.new_plus(row[0]);
  data.rk.kvrt.new_plus(row[1]);
  data.rk.nomdok.new_plus(row[2]);
  data.rk.metka_vd=atoi(row[3]);
  data.rk.nomdok_dpa.new_plus(row[4]);
  if(row[5][0] != '0')
   data.rk.datd.new_plus(iceb_u_datzap(row[5]));
  if(row[6][0] != '0')
    data.rk.datd_dpa.new_plus(iceb_u_datzap(row[6]));
  kikz.plus(iceb_kikz(row[8],row[9],wpredok));
  data.rk.kolr.new_plus(row[10]);
  data.rk.kols.new_plus(row[11]);
 }
else
 {
  data.rk.nomdok.new_plus(nomdf8dr(wpredok));
 }




data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;
if(data.nomdok.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой формы 1ДФ"));
  label=gtk_label_new(gettext("Ввод новой формы 1ДФ"));
 }
else
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Просмотр шапки формы 1ДФ"));
  label=gtk_label_new(gettext("Просмотр шапки формы 1ДФ"));
 }
gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_f1df_m_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

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
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 1);
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], TRUE, TRUE, 1);


GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
GtkWidget *separator3=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_box_pack_start (GTK_BOX (vbox),separator1, TRUE, TRUE, 2);

//Вставляем радиокнопки

data.radiobutton2[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Отчётный"));
data.radiobutton2[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[0]),gettext("Новый отчётный"));
data.radiobutton2[2]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton2[1]),gettext("Уточняющий"));

gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[0], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[1], TRUE, TRUE, 1);
gtk_box_pack_start (GTK_BOX (vbox), data.radiobutton2[2], TRUE, TRUE, 1);

gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data.radiobutton2[data.rk.metka_vd]),TRUE); //Устанавливем активной кнопку

gtk_box_pack_start (GTK_BOX (vbox),separator3, TRUE, TRUE, 2);

gtk_box_pack_start (GTK_BOX (vbox), hboxknop, TRUE, TRUE, 1);




sprintf(strsql,"%s",gettext("Год"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), label, FALSE, FALSE, 0);

data.entry[E_GOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GOD]),4);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD]), data.entry[E_GOD], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_GOD], "activate",G_CALLBACK(l_f1df_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD]),data.rk.god.ravno());
gtk_widget_set_name(data.entry[E_GOD],iceb_u_inttochar(E_GOD));

sprintf(strsql,"%s (1,2,3,4)",gettext("Квартал"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KVRT]), label, FALSE, FALSE, 0);

data.entry[E_KVRT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KVRT]),1);
gtk_box_pack_start (GTK_BOX (hbox[E_KVRT]), data.entry[E_KVRT], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_KVRT], "activate",G_CALLBACK(l_f1df_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KVRT]),data.rk.kvrt.ravno());
gtk_widget_set_name(data.entry[E_KVRT],iceb_u_inttochar(E_KVRT));

sprintf(strsql,"%s",gettext("Номер документа"));
data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_NOMDOK],"clicked",G_CALLBACK(l_f1df_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_NOMDOK],gettext("Выбор даты"));

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(l_f1df_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk.nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));

sprintf(strsql,"%s (%s)",gettext("Дата д-та"),gettext("д.м.г"));
data.knopka_enter[E_DATD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATD]), data.knopka_enter[E_DATD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATD],"clicked",G_CALLBACK(l_f1df_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATD],iceb_u_inttochar(E_DATD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATD],gettext("Выбор даты"));

data.entry[E_DATD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATD]), data.entry[E_DATD], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_DATD], "activate",G_CALLBACK(l_f1df_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATD]),data.rk.datd.ravno());
gtk_widget_set_name(data.entry[E_DATD],iceb_u_inttochar(E_DATD));

sprintf(strsql,"%s (%s)",gettext("Дата д-та ГНИ"),gettext("д.м.г"));
data.knopka_enter[E_DATD_DPA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATD_DPA]), data.knopka_enter[E_DATD_DPA], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATD_DPA],"clicked",G_CALLBACK(l_f1df_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATD_DPA],iceb_u_inttochar(E_DATD_DPA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATD_DPA],gettext("Выбор даты"));

data.entry[E_DATD_DPA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATD_DPA]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATD_DPA]), data.entry[E_DATD_DPA], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_DATD_DPA], "activate",G_CALLBACK(l_f1df_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATD_DPA]),data.rk.datd_dpa.ravno());
gtk_widget_set_name(data.entry[E_DATD_DPA],iceb_u_inttochar(E_DATD_DPA));


sprintf(strsql,"%s",gettext("Номер д-та ГНИ"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK_DPA]), label, FALSE, FALSE, 0);

data.entry[E_NOMDOK_DPA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK_DPA]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK_DPA]), data.entry[E_NOMDOK_DPA], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_NOMDOK_DPA], "activate",G_CALLBACK(l_f1df_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK_DPA]),data.rk.nomdok_dpa.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK_DPA],iceb_u_inttochar(E_NOMDOK_DPA));


label=gtk_label_new(gettext("Количество работников"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOLR]), label, FALSE, FALSE, 0);

data.entry[E_KOLR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOLR]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLR]), data.entry[E_KOLR], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_KOLR], "activate",G_CALLBACK(l_f1df_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLR]),data.rk.kolr.ravno());
gtk_widget_set_name(data.entry[E_KOLR],iceb_u_inttochar(E_KOLR));

label=gtk_label_new(gettext("Количество совместителей"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOLS]), label, FALSE, FALSE, 0);

data.entry[E_KOLS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOLS]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLS]), data.entry[E_KOLS], FALSE, FALSE, 0);
g_signal_connect(data.entry[E_KOLS], "activate",G_CALLBACK(l_f1df_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLS]),data.rk.kols.ravno());
gtk_widget_set_name(data.entry[E_KOLS],iceb_u_inttochar(E_KOLS));



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_f1df_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Ном.док."));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Получение номера документа"));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(l_f1df_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_f1df_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_f1df_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 nomdok->new_plus(data.rk.nomdok.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_f1df_m_e_knopka(GtkWidget *widget,class l_f1df_m_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   

  case E_DATD:
    iceb_calendar(&data->rk.datd,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATD]),data->rk.datd.ravno());
    return;

  case E_DATD_DPA:
    iceb_calendar(&data->rk.datd_dpa,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATD_DPA]),data->rk.datd_dpa.ravno());
    return;

    
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_f1df_m_key_press(GtkWidget *widget,GdkEventKey *event,class l_f1df_m_data *data)
{
//char  bros[512];

//printf("l_f1df_m_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");

    return(TRUE);


  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");

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
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_f1df_m_knopka(GtkWidget *widget,class l_f1df_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);

    data->read_rek();
    if(l_f1df_v_zap(data) != 0)
     return;
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK3:
    data->rk.nomdok.new_plus(nomdf8dr(data->window));
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk.nomdok.ravno());
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

void    l_f1df_m_vvod(GtkWidget *widget,class l_f1df_m_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_GOD:
    data->rk.god.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KVRT:
    data->rk.kvrt.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NOMDOK:
    data->rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NOMDOK_DPA:
    data->rk.nomdok_dpa.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DATD:
    data->rk.datd.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DATD_DPA:
    data->rk.datd_dpa.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/******************************/
/*Определение номера документа*/
/******************************/

int nomdf8dr(GtkWidget *wpredok)
{
int		kolstr;
SQL_str         row;
char		strsql[512];
int             mpn;  /*Максимальный порядковый номер*/
int             pn;   /*Порядковый номер*/


sprintf(strsql,"select nomd from F8dr");

//printw("\n%s\n",strsql);
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
mpn=pn=0;
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  pn=atoi(row[0]);
  if(pn > mpn)
   mpn=pn;

 }

mpn++;

return(mpn);


}
/****************************************/
/*Запись*/
/*************************************/
int l_f1df_v_zap(class l_f1df_m_data *data)
{
char		strsql[512];
time_t		vrem;
short		d1,m1,g1;
short		d2,m2,g2;

if(data->rk.datd.getdlinna() > 1)
if(iceb_u_rsdat(&d1,&m1,&g1,data->rk.datd.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата документа !"),data->window);
  return(1);
 }

if(data->rk.datd_dpa.getdlinna() > 1)
if(iceb_u_rsdat(&d2,&m2,&g2,data->rk.datd_dpa.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата документа ГНИ !"),data->window);
  return(1);
 }
if(data->rk.kvrt.ravno_atoi() < 1 || data->rk.kvrt.ravno_atoi() > 4)
 {
  iceb_menu_soob(gettext("Неправильно введён квартал !"),data->window);
  return(1);
 }

class iceb_lock_tables kkk("LOCK TABLE F8dr WRITE,icebuser READ");
/*Проверяем есть ли такой номер документа*/
if(iceb_u_SRAV(data->rk.nomdok.ravno(),data->nomdok.ravno(),0) != 0)
 {
  sprintf(strsql,"select nomd from F8dr where nomd='%s'",data->rk.nomdok.ravno());
  if(iceb_sql_readkey(strsql,data->window) >= 1)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть !"),data->rk.nomdok.ravno());
    iceb_menu_soob(strsql,data->window);
    return(1);

   }
 }

 
time(&vrem);

/*Ввод новой записи*/
if(data->nomdok.getdlinna() <= 1)
 {
  sprintf(strsql,"insert into F8dr \
values (%d,%d,'%s',%d,'%s','%04d-%d-%d','%04d-%d-%d',%d,%d,%ld,%d,%d)",
  data->rk.god.ravno_atoi(),
  data->rk.kvrt.ravno_atoi(),
  data->rk.nomdok.ravno(),
  data->rk.metka_vd,
  data->rk.nomdok_dpa.ravno(),
  g1,m1,d1,
  g2,m2,d2,
  0,
  iceb_getuid(data->window),
  vrem,
  data->rk.kolr.ravno_atoi(),
  data->rk.kols.ravno_atoi());
  
  if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
    return(1);
 }
else
 {
/*Корректировка записи*/
  sprintf(strsql,"update F8dr set \
god=%d,\
kvrt=%d,\
nomd='%s',\
vidd=%d,\
nomdn='%s',\
datad='%04d-%d-%d',\
datan='%04d-%d-%d',\
metbl=%d,\
ktoz=%d,\
vrem=%ld,\
kr=%d,\
ks=%d \
where nomd='%s'",
data->rk.god.ravno_atoi(),
data->rk.kvrt.ravno_atoi(),
data->rk.nomdok.ravno(),
data->rk.metka_vd,
data->rk.nomdok_dpa.ravno(),
g1,m1,d1,
g2,m2,d2,0,
iceb_getuid(data->window),
vrem,
data->rk.kolr.ravno_atoi(),
data->rk.kols.ravno_atoi(),
data->nomdok.ravno());

  if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
    return(1);

  if(iceb_u_SRAV(data->rk.nomdok.ravno(),data->nomdok.ravno(),0) != 0)
   {
    sprintf(strsql,"update F8dr1 set nomd='%s' where nomd='%s'",
    data->rk.nomdok.ravno(),data->nomdok.ravno());
    iceb_sql_zapis(strsql,1,0,data->window);

   }

 }

return(0);

}
