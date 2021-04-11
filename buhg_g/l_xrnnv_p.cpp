/*$Id: l_xrnnv_p.c,v 1.14 2014/02/28 05:20:59 sasa Exp $*/
/*02.03.2016	07.04.2008	Белых А.И.	l_xrnnv_p.c
Поиск
*/
#include "buhg_g.h"
#include "xrnn_poiw.h"
enum
 {
  E_DATAN,
  E_DATAK,
  E_NNN,
  E_NOMDOK,
  E_INN,
  E_NAIM_KONTR,
  E_KOMENT,
  E_KOD_GR,
  E_PODS,
  E_KOD_OP,
  E_VIDDOK,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_xrnnv_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  class iceb_u_str name_window;
  short kl_shift;
  GtkWidget *opt;
  short voz;      //0-ввели 1 нет

  class xrnn_poi *rk;
  
  l_xrnnv_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN])));
    rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK])));
    rk->nnn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NNN])));
    rk->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK])));
    rk->inn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_INN])));
    rk->naim_kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM_KONTR])));
    rk->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
    rk->kodgr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_GR])));
    rk->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_OP])));
    rk->podsistema.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PODS])));
    rk->viddok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_VIDDOK])));

    rk->metka_ins=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));
   }
  void clear()
   {
    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   
   }
 };


gboolean   l_xrnnv_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_xrnnv_p_data *data);
void  l_xrnnv_p_knopka(GtkWidget *widget,class l_xrnnv_p_data *data);
void    l_xrnnv_p_vvod(GtkWidget *widget,class l_xrnnv_p_data *data);

void  l_xrnnv_p_e_knopka(GtkWidget *widget,class l_xrnnv_p_data *data);
int l_xrnnv_vpds(class iceb_u_str *pods,GtkWidget *wpredok);

extern SQL_baza  bd;

int l_xrnnv_p(class xrnn_poi *rek_poi,GtkWidget *wpredok)
{

class l_xrnnv_p_data data;
char strsql[512];
int gor=0,ver=0;
data.name_window.plus(__FUNCTION__);

data.rk=rek_poi;

 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Поиск"));
label=gtk_label_new(gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_xrnnv_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL,1);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
  gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxopt = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxopt),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start(GTK_BOX (vbox), label, TRUE, TRUE,1);
for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start(GTK_BOX (vbox), hbox[i], TRUE, TRUE,1);

gtk_box_pack_start(GTK_BOX (vbox), hboxopt, TRUE, TRUE,1);

gtk_box_pack_start(GTK_BOX (vbox), hboxknop, TRUE, TRUE,1);



sprintf(strsql,"%s (%s)",gettext("Дата начала"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(l_xrnnv_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(l_xrnnv_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));

sprintf(strsql,"%s (%s)",gettext("Дата конца"),gettext("д.м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(l_xrnnv_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(l_xrnnv_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));

label=gtk_label_new(gettext("Номер налоговой накладной"));
gtk_box_pack_start (GTK_BOX (hbox[E_NNN]), label, FALSE, FALSE,1);

data.entry[E_NNN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NNN]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_NNN]), data.entry[E_NNN], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NNN], "activate",G_CALLBACK(l_xrnnv_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NNN]),data.rk->nnn.ravno());
gtk_widget_set_name(data.entry[E_NNN],iceb_u_inttochar(E_NNN));

label=gtk_label_new(gettext("Номер документа"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE,1);

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(l_xrnnv_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk->nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));


label=gtk_label_new(gettext("Индивидуальный налоговый номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), label, FALSE, FALSE,1);

data.entry[E_INN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_INN]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_INN]), data.entry[E_INN], TRUE, TRUE,1);
g_signal_connect(data.entry[E_INN], "activate",G_CALLBACK(l_xrnnv_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INN]),data.rk->inn.ravno());
gtk_widget_set_name(data.entry[E_INN],iceb_u_inttochar(E_INN));

sprintf(strsql,"%s",gettext("Наименование контрагента"));
data.knopka_enter[E_NAIM_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM_KONTR]), data.knopka_enter[E_NAIM_KONTR], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_NAIM_KONTR],"clicked",G_CALLBACK(l_xrnnv_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NAIM_KONTR],iceb_u_inttochar(E_NAIM_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_NAIM_KONTR],gettext("Выбор контрагента"));

data.entry[E_NAIM_KONTR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM_KONTR]),99);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM_KONTR]), data.entry[E_NAIM_KONTR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NAIM_KONTR], "activate",G_CALLBACK(l_xrnnv_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM_KONTR]),data.rk->naim_kontr.ravno());
gtk_widget_set_name(data.entry[E_NAIM_KONTR],iceb_u_inttochar(E_NAIM_KONTR));


label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE,1);

data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),99);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(l_xrnnv_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.rk->koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));


sprintf(strsql,"%s (,,)",gettext("Код группы"));
data.knopka_enter[E_KOD_GR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GR]), data.knopka_enter[E_KOD_GR], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_GR],"clicked",G_CALLBACK(l_xrnnv_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_GR],iceb_u_inttochar(E_KOD_GR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_GR],gettext("Выбор группы"));

data.entry[E_KOD_GR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_GR]), data.entry[E_KOD_GR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_GR], "activate",G_CALLBACK(l_xrnnv_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_GR]),data.rk->kodgr.ravno());
gtk_widget_set_name(data.entry[E_KOD_GR],iceb_u_inttochar(E_KOD_GR));


sprintf(strsql,"%s (,,)",gettext("Код операции"));
data.knopka_enter[E_KOD_OP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OP]), data.knopka_enter[E_KOD_OP], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_OP],"clicked",G_CALLBACK(l_xrnnv_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_OP],iceb_u_inttochar(E_KOD_OP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_OP],gettext("Выбор операции"));

data.entry[E_KOD_OP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OP]), data.entry[E_KOD_OP], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_OP], "activate",G_CALLBACK(l_xrnnv_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_OP]),data.rk->kodop.ravno());
gtk_widget_set_name(data.entry[E_KOD_OP],iceb_u_inttochar(E_KOD_OP));

sprintf(strsql,"%s (,,)",gettext("Подсистема"));
data.knopka_enter[E_PODS]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODS]), data.knopka_enter[E_PODS], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_PODS],"clicked",G_CALLBACK(l_xrnnv_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODS],iceb_u_inttochar(E_PODS));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PODS],gettext("Выбор подсистемы"));

data.entry[E_PODS] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODS]), data.entry[E_PODS], TRUE, TRUE,1);
g_signal_connect(data.entry[E_PODS], "activate",G_CALLBACK(l_xrnnv_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODS]),data.rk->kodop.ravno());
gtk_widget_set_name(data.entry[E_PODS],iceb_u_inttochar(E_PODS));

sprintf(strsql,"%s",gettext("Вид документа"));
data.knopka_enter[E_VIDDOK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_VIDDOK]), data.knopka_enter[E_VIDDOK], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_VIDDOK],"clicked",G_CALLBACK(l_xrnnv_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_VIDDOK],iceb_u_inttochar(E_VIDDOK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_VIDDOK],gettext("Выбор вида документа"));

data.entry[E_VIDDOK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_VIDDOK]), data.entry[E_VIDDOK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_VIDDOK], "activate",G_CALLBACK(l_xrnnv_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VIDDOK]),data.rk->viddok.ravno());
gtk_widget_set_name(data.entry[E_VIDDOK],iceb_u_inttochar(E_VIDDOK));


label=gtk_label_new(gettext("Отмеченные записи"));
gtk_box_pack_start (GTK_BOX (hboxopt), label, FALSE, FALSE, 1);

class iceb_u_spisok bal_st;
bal_st.plus(gettext("Все записи"));
bal_st.plus(gettext("Только отмеченные записи"));
bal_st.plus(gettext("Только не отмеченные записи"));

iceb_pm_vibor(&bal_st,&data.opt,data.rk->metka_ins);
gtk_box_pack_start (GTK_BOX (hboxopt), data.opt, TRUE, TRUE,1);




sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_xrnnv_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_xrnnv_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без поиска нужных записей"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_xrnnv_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

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
void  l_xrnnv_p_e_knopka(GtkWidget *widget,class l_xrnnv_p_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rk->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rk->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datak.ravno());
      
    return;  



  case E_NAIM_KONTR:
    if(iceb_l_kontr(1,&kod,&naim,data->window) == 0)
     data->rk->naim_kontr.new_plus(iceb_get_pnk(kod.ravno(),1,data->window));
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NAIM_KONTR]),data->rk->naim_kontr.ravno());
    return;  

  case E_KOD_GR:
    if(l_xrnngpn(1,&kod,&naim,data->window) == 0)
     data->rk->kodgr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_GR]),data->rk->kodgr.ravno());
    return;  

  case E_KOD_OP:
    xrnn_kord(0,&data->rk->kodop,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_OP]),data->rk->kodop.ravno());
    return;  

  case E_PODS:
    l_xrnnv_vpds(&data->rk->podsistema,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODS]),data->rk->podsistema.ravno());
    return;  

  case E_VIDDOK:
    if(rnnvvd(&kod,data->window) == 0)
     data->rk->viddok.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_VIDDOK]),data->rk->viddok.ravno());
    return;  
     
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_xrnnv_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_xrnnv_p_data *data)
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
void  l_xrnnv_p_knopka(GtkWidget *widget,class l_xrnnv_p_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();

    if(data->rk->datan.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно ввели дату начала!"),data->window);
      return;
     }
    if(data->rk->datak.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно ввели дату конца!"),data->window);
      return;
     }


    data->rk->metka_poi=1;
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear();
    return;
    
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->rk->metka_poi=0;
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_xrnnv_p_vvod(GtkWidget *widget,class l_xrnnv_p_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/********************************/
/*Выбор подсистемы*/
/****************************/
int l_xrnnv_vpds(class iceb_u_str *pods,GtkWidget *wpredok)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));

char strsql[512];

sprintf(strsql,"%-3s %s",ICEB_MP_MATU,gettext("Материальный учет"));
punkt_m.plus(strsql);

sprintf(strsql,"%-3s %s",ICEB_MP_USLUGI,gettext("Учет услуг"));
punkt_m.plus(strsql);

sprintf(strsql,"%-3s %s",ICEB_MP_UOS,gettext("Учет основных средств"));
punkt_m.plus(strsql);

sprintf(strsql,"%-3s %s",ICEB_MP_UKR,gettext("Учет командировочных расходов"));
punkt_m.plus(strsql);

sprintf(strsql,"%-3s %s",gettext("ГК"),gettext("Главная книга"));
punkt_m.plus(strsql);


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);

switch(nomer)
 {
  case 0:
   pods->z_plus(ICEB_MP_MATU);
   return(0); 

  case 1:
   pods->z_plus(ICEB_MP_USLUGI);
   return(0); 

  case 2:
   pods->z_plus(ICEB_MP_UOS);
   return(0); 

  case 3:
   pods->z_plus(ICEB_MP_UKR);
   return(0); 

  case 4:
   pods->z_plus(gettext("ГК"));
   return(0); 

 }
return(1);
}

