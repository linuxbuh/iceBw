/*$Id: l_ukrspdok_p.c,v 1.12 2013/08/25 08:26:39 sasa Exp $*/
/*01.03.2016	21.02.2008	Белых А.И.	l_ukrspdok_p.c
Ввод реквизитов поиска документов
*/
#include "buhg_g.h"
#include "l_ukrspdok.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_NOMDOK,
  E_DATAN,
  E_DATAK,
  E_KONTR,
  E_VIDKOM,
  E_NOMER_PRIK,
  E_DATA_PRIK,
  E_NOMER_AO,
  E_DATA_AO,
  E_PUNKTNAZ,
  E_ORGAN,
  KOLENTER  
 };

class ukrspdok_p_data
 {
  public:
  class ukrspdok_rek  *rpoi;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt;
  class iceb_u_str name_window;
  short kl_shift;
  
  
  ukrspdok_p_data() //Конструктор
   {
    kl_shift=0;
  
   }

  void read_rek()
   {
    rpoi->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK])));
    rpoi->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN])));
    rpoi->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK])));
    rpoi->kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR])));
    rpoi->vidkom.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_VIDKOM])));
    rpoi->nomer_prik.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMER_PRIK])));
    rpoi->data_prik.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_PRIK])));
    rpoi->nomer_ao.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMER_AO])));
    rpoi->data_ao.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_AO])));
    rpoi->punktnaz.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PUNKTNAZ])));
    rpoi->organ.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_ORGAN])));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    rpoi->clear();
   }
 };

gboolean   ukrspdok_p_key_press(GtkWidget *widget,GdkEventKey *event,class ukrspdok_p_data *data);
void    ukrspdok_p_vvod(GtkWidget *widget,class ukrspdok_p_data *data);
void  ukrspdok_p_knopka(GtkWidget *widget,class ukrspdok_p_data *data);

void  ukrspdok_v_e_knopka(GtkWidget *widget,class ukrspdok_p_data *data);

extern SQL_baza bd;

int l_ukrspdok_p(class ukrspdok_rek *datap,GtkWidget *wpredok)
{
class ukrspdok_p_data data;
int gor=0,ver=0;
data.rpoi=datap;
data.name_window.plus(__FUNCTION__);

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Поиск"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(ukrspdok_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Поиск"));

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL,1);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s",gettext("Номер документа"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE,1);

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rpoi->nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));

sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(ukrspdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rpoi->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));


sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(ukrspdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rpoi->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));

sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KONTR],"clicked",G_CALLBACK(ukrspdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KONTR],iceb_u_inttochar(E_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KONTR],gettext("Выбор контрагента"));

data.entry[E_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rpoi->kontr.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));

sprintf(strsql,"%s (,,)",gettext("Вид командировки"));
data.knopka_enter[E_VIDKOM]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_VIDKOM]), data.knopka_enter[E_VIDKOM], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_VIDKOM],"clicked",G_CALLBACK(ukrspdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_VIDKOM],iceb_u_inttochar(E_VIDKOM));
gtk_widget_set_tooltip_text(data.knopka_enter[E_VIDKOM],gettext("Выбор вида командировки"));

data.entry[E_VIDKOM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_VIDKOM]), data.entry[E_VIDKOM], TRUE, TRUE,1);
g_signal_connect(data.entry[E_VIDKOM], "activate",G_CALLBACK(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VIDKOM]),data.rpoi->vidkom.ravno());
gtk_widget_set_name(data.entry[E_VIDKOM],iceb_u_inttochar(E_VIDKOM));


sprintf(strsql,"%s (,,)",gettext("Номер приказа"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_PRIK]), label, FALSE, FALSE,1);

data.entry[E_NOMER_PRIK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMER_PRIK]),100);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_PRIK]), data.entry[E_NOMER_PRIK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NOMER_PRIK], "activate",G_CALLBACK(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER_PRIK]),data.rpoi->nomer_prik.ravno());
gtk_widget_set_name(data.entry[E_NOMER_PRIK],iceb_u_inttochar(E_NOMER_PRIK));


sprintf(strsql,"%s",gettext("Дата приказа"));
data.knopka_enter[E_DATA_PRIK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_PRIK]), data.knopka_enter[E_DATA_PRIK], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATA_PRIK],"clicked",G_CALLBACK(ukrspdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA_PRIK],iceb_u_inttochar(E_DATA_PRIK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA_PRIK],gettext("Выбор даты"));

data.entry[E_DATA_PRIK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA_PRIK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_PRIK]), data.entry[E_DATA_PRIK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATA_PRIK], "activate",G_CALLBACK(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_PRIK]),data.rpoi->data_prik.ravno());
gtk_widget_set_name(data.entry[E_DATA_PRIK],iceb_u_inttochar(E_DATA_PRIK));


sprintf(strsql,"%s (,,)",gettext("Номер авансового отчёта"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_AO]), label, FALSE, FALSE,1);

data.entry[E_NOMER_AO] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMER_AO]),100);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMER_AO]), data.entry[E_NOMER_AO], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NOMER_AO], "activate",G_CALLBACK(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMER_AO]),data.rpoi->nomer_ao.ravno());
gtk_widget_set_name(data.entry[E_NOMER_AO],iceb_u_inttochar(E_NOMER_AO));


sprintf(strsql,"%s",gettext("Дата авансового отчёта"));
data.knopka_enter[E_DATA_AO]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_AO]), data.knopka_enter[E_DATA_AO], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATA_AO],"clicked",G_CALLBACK(ukrspdok_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA_AO],iceb_u_inttochar(E_DATA_AO));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA_AO],gettext("Выбор даты"));

data.entry[E_DATA_AO] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA_AO]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_AO]), data.entry[E_DATA_AO], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATA_AO], "activate",G_CALLBACK(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_AO]),data.rpoi->data_ao.ravno());
gtk_widget_set_name(data.entry[E_DATA_AO],iceb_u_inttochar(E_DATA_AO));

sprintf(strsql,"%s (,,)",gettext("Пункт назначения"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PUNKTNAZ]), label, FALSE, FALSE,1);

data.entry[E_PUNKTNAZ] = gtk_entry_new ();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PUNKTNAZ]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_PUNKTNAZ]), data.entry[E_PUNKTNAZ], TRUE, TRUE,1);
g_signal_connect(data.entry[E_PUNKTNAZ], "activate",G_CALLBACK(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PUNKTNAZ]),data.rpoi->punktnaz.ravno());
gtk_widget_set_name(data.entry[E_PUNKTNAZ],iceb_u_inttochar(E_PUNKTNAZ));

sprintf(strsql,"%s (,,)",gettext("Организация"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_ORGAN]), label, FALSE, FALSE,1);

data.entry[E_ORGAN] = gtk_entry_new ();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_ORGAN]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_ORGAN]), data.entry[E_ORGAN], TRUE, TRUE,1);
g_signal_connect(data.entry[E_ORGAN], "activate",G_CALLBACK(ukrspdok_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ORGAN]),data.rpoi->organ.ravno());
gtk_widget_set_name(data.entry[E_ORGAN],iceb_u_inttochar(E_ORGAN));





sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(ukrspdok_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(ukrspdok_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(ukrspdok_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(0);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  ukrspdok_v_e_knopka(GtkWidget *widget,class ukrspdok_p_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rpoi->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rpoi->datan.ravno());
      
    return;  

  case E_DATA_PRIK:

    if(iceb_calendar(&data->rpoi->data_prik,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_PRIK]),data->rpoi->data_prik.ravno());
      
    return;  

  case E_DATA_AO:

    if(iceb_calendar(&data->rpoi->data_ao,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_AO]),data->rpoi->data_ao.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rpoi->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rpoi->datak.ravno());
      
    return;  

  case E_KONTR:

    iceb_vibrek(0,"Kontragent",&data->rpoi->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rpoi->kontr.ravno());

    return;  


  case E_VIDKOM:
    if(l_ukrvk(1,&kod,&naim,data->window) == 0)
      data->rpoi->vidkom.z_plus(kod.ravno());    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VIDKOM]),data->rpoi->vidkom.ravno());
    return;  
    
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   ukrspdok_p_key_press(GtkWidget *widget,GdkEventKey *event,class ukrspdok_p_data *data)
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
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  ukrspdok_p_knopka(GtkWidget *widget,class ukrspdok_p_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek(); //Читаем реквизиты меню

    if(data->rpoi->datan.getdlinna() > 1)
     if(data->rpoi->datan.prov_dat() != 0)
      {
       iceb_menu_soob(gettext("Не правильно введена дата начала !"),data->window);
       return;
      }     

    if(data->rpoi->datak.getdlinna() > 1)
     if(data->rpoi->datak.prov_dat() != 0)
      {
       iceb_menu_soob(gettext("Не правильно введена дата конца !"),data->window);
       return;
      }     

    if(data->rpoi->data_prik.getdlinna() > 1)
     if(data->rpoi->data_prik.prov_dat() != 0)
      {
       iceb_menu_soob(gettext("Не правильно введена дата приказа!"),data->window);
       return;
      }     

    if(data->rpoi->data_ao.getdlinna() > 1)
     if(data->rpoi->data_ao.prov_dat() != 0)
      {
       iceb_menu_soob(gettext("Не правильно введена дата авансового отчёта!"),data->window);
       return;
      }     

    data->rpoi->metka_poi=1;
    gtk_widget_destroy(data->window);
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek(); //Читаем реквизиты меню
    data->rpoi->metka_poi=0;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    ukrspdok_p_vvod(GtkWidget *widget,class ukrspdok_p_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_NOMDOK:
    data->rpoi->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMER_PRIK:
    data->rpoi->nomer_prik.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  
  case E_NOMER_AO:
    data->rpoi->nomer_ao.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAN:
    data->rpoi->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATA_PRIK:
    data->rpoi->data_prik.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATA_AO:
    data->rpoi->data_ao.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAK:
    data->rpoi->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KONTR:
    data->rpoi->kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_VIDKOM:
    data->rpoi->vidkom.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;

if(enter >= KOLENTER)
enter=0;


gtk_widget_grab_focus(data->entry[enter]);
 
}
