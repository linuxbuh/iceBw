/*$Id: l_uosin_p.c,v 1.11 2013/08/25 08:26:40 sasa Exp $*/
/*01.03.2016	20.12.2007	Белых А.И.	l_uosin_p.c
Меню для ввода реквизитов поиска 
*/
#include "buhg_g.h"
#include "l_uosin.h"
enum
 {
  E_NAIM,
  E_ZAVOD_IZ,
  E_PASPORT,
  E_MODEL,
  E_GOD_IZ,
  E_ZAV_NOMER,
  E_DATA_VVODA,
  E_SHETU,
  E_HZT,
  E_HAU,
  E_HNA,
  E_HNABU,
  E_POPKF_NU,
  E_POPKF_BU,
  E_NOM_ZNAK,
  E_MAT_OT,
  E_PODR,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_uosin_p_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class l_uosin_poi *rk;
  
  
  l_uosin_p_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      g_signal_emit_by_name(entry[i],"activate");
     }
   }

  void clear_data()
   {
    rk->clear();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void l_uosin_p_clear(class l_uosin_p_data *data);
void    l_uosin_p_vvod(GtkWidget *widget,class l_uosin_p_data *data);
void  l_uosin_p_knopka(GtkWidget *widget,class l_uosin_p_data *data);
gboolean   l_uosin_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosin_p_data *data);
void  l_uosin_p_e_knopka(GtkWidget *widget,class l_uosin_p_data *data);


int l_uosin_p(class l_uosin_poi *rek_poi,GtkWidget *wpredok)
{
class l_uosin_p_data data;
char strsql[512];
int gor=0,ver=0;
data.rk=rek_poi;
data.name_window.plus(__FUNCTION__);


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Поиск"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_uosin_p_key_press),&data);

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
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,1);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s (,,)",gettext("Наименование"));
GtkWidget *label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE,1);



data.entry[E_NAIM] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.rk->naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));

label=gtk_label_new(gettext("Завод изготовитель"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZAVOD_IZ]), label, FALSE, FALSE,1);

data.entry[E_ZAVOD_IZ] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_ZAVOD_IZ]), data.entry[E_ZAVOD_IZ], TRUE, TRUE,1);
g_signal_connect(data.entry[E_ZAVOD_IZ], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZAVOD_IZ]),data.rk->zavod_iz.ravno());
gtk_widget_set_name(data.entry[E_ZAVOD_IZ],iceb_u_inttochar(E_ZAVOD_IZ));


label=gtk_label_new(gettext("Паспорт"));
gtk_box_pack_start (GTK_BOX (hbox[E_PASPORT]), label, FALSE, FALSE,1);

data.entry[E_PASPORT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PASPORT]), data.entry[E_PASPORT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_PASPORT], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PASPORT]),data.rk->pasport.ravno());
gtk_widget_set_name(data.entry[E_PASPORT],iceb_u_inttochar(E_PASPORT));

label=gtk_label_new(gettext("Модель"));
gtk_box_pack_start (GTK_BOX (hbox[E_MODEL]), label, FALSE, FALSE,1);

data.entry[E_MODEL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_MODEL]), data.entry[E_MODEL], TRUE, TRUE,1);
g_signal_connect(data.entry[E_MODEL], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MODEL]),data.rk->model.ravno());
gtk_widget_set_name(data.entry[E_MODEL],iceb_u_inttochar(E_MODEL));

label=gtk_label_new(gettext("Год изготовления"));
gtk_box_pack_start (GTK_BOX (hbox[E_GOD_IZ]), label, FALSE, FALSE,1);

data.entry[E_GOD_IZ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GOD_IZ]),4);
gtk_box_pack_start (GTK_BOX (hbox[E_GOD_IZ]), data.entry[E_GOD_IZ], TRUE, TRUE,1);
g_signal_connect(data.entry[E_GOD_IZ], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GOD_IZ]),data.rk->god_iz.ravno());
gtk_widget_set_name(data.entry[E_GOD_IZ],iceb_u_inttochar(E_GOD_IZ));

label=gtk_label_new(gettext("Заводской номер"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZAV_NOMER]), label, FALSE, FALSE,1);

data.entry[E_ZAV_NOMER] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_ZAV_NOMER]), data.entry[E_ZAV_NOMER], TRUE, TRUE,1);
g_signal_connect(data.entry[E_ZAV_NOMER], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZAV_NOMER]),data.rk->zav_nomer.ravno());
gtk_widget_set_name(data.entry[E_ZAV_NOMER],iceb_u_inttochar(E_ZAV_NOMER));


sprintf(strsql,"%s",gettext("Дата ввода в эксплуатацию"));
data.knopka_enter[E_DATA_VVODA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VVODA]), data.knopka_enter[E_DATA_VVODA], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATA_VVODA],"clicked",G_CALLBACK(l_uosin_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA_VVODA],iceb_u_inttochar(E_DATA_VVODA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA_VVODA],gettext("Выбор даты"));

data.entry[E_DATA_VVODA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VVODA]), data.entry[E_DATA_VVODA], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATA_VVODA], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_VVODA]),data.rk->data_vvoda.ravno());
gtk_widget_set_name(data.entry[E_DATA_VVODA],iceb_u_inttochar(E_DATA_VVODA));


sprintf(strsql,"%s (,,)",gettext("Счёт учёта"));
data.knopka_enter[E_SHETU]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.knopka_enter[E_SHETU], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_SHETU],"clicked",G_CALLBACK(l_uosin_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHETU],iceb_u_inttochar(E_SHETU));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHETU],gettext("Выбор шифра затрат"));

data.entry[E_SHETU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.entry[E_SHETU], TRUE, TRUE,1);
g_signal_connect(data.entry[E_SHETU], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETU]),data.rk->shetu.ravno());
gtk_widget_set_name(data.entry[E_SHETU],iceb_u_inttochar(E_SHETU));


sprintf(strsql,"%s (,,)",gettext("Шифр затрат аморт-отчислений"));
data.knopka_enter[E_HZT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_HZT]), data.knopka_enter[E_HZT], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_HZT],"clicked",G_CALLBACK(l_uosin_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_HZT],iceb_u_inttochar(E_HZT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_HZT],gettext("Выбор шифра затрат аморт-отчислений"));

data.entry[E_HZT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HZT]), data.entry[E_HZT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_HZT], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HZT]),data.rk->hzt.ravno());
gtk_widget_set_name(data.entry[E_HZT],iceb_u_inttochar(E_HZT));



sprintf(strsql,"%s (,,)",gettext("Шифр аналитического учёта"));
data.knopka_enter[E_HAU]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_HAU]), data.knopka_enter[E_HAU], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_HAU],"clicked",G_CALLBACK(l_uosin_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_HAU],iceb_u_inttochar(E_HAU));
gtk_widget_set_tooltip_text(data.knopka_enter[E_HAU],gettext("Выбор шифра аналитического учёта"));

data.entry[E_HAU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HAU]), data.entry[E_HAU], TRUE, TRUE,1);
g_signal_connect(data.entry[E_HAU], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HAU]),data.rk->hau.ravno());
gtk_widget_set_name(data.entry[E_HAU],iceb_u_inttochar(E_HAU));




sprintf(strsql,"%s (,,)",gettext("Группа налогового учёта"));
data.knopka_enter[E_HNA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_HNA]), data.knopka_enter[E_HNA], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_HNA],"clicked",G_CALLBACK(l_uosin_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_HNA],iceb_u_inttochar(E_HNA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_HNA],gettext("Выбор группы"));

data.entry[E_HNA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HNA]), data.entry[E_HNA], TRUE, TRUE,1);
g_signal_connect(data.entry[E_HNA], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HNA]),data.rk->hna.ravno());
gtk_widget_set_name(data.entry[E_HNA],iceb_u_inttochar(E_HNA));



sprintf(strsql,"%s (,,)",gettext("Группа бух. учёта"));
data.knopka_enter[E_HNABU]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_HNABU]), data.knopka_enter[E_HNABU], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_HNABU],"clicked",G_CALLBACK(l_uosin_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_HNABU],iceb_u_inttochar(E_HNABU));
gtk_widget_set_tooltip_text(data.knopka_enter[E_HNABU],gettext("Выбор группы"));

data.entry[E_HNABU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_HNABU]), data.entry[E_HNABU], TRUE, TRUE,1);
g_signal_connect(data.entry[E_HNABU], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HNABU]),data.rk->hnabu.ravno());
gtk_widget_set_name(data.entry[E_HNABU],iceb_u_inttochar(E_HNABU));

label=gtk_label_new(gettext("Поправочный коэффициент для налогового учёта"));
gtk_box_pack_start (GTK_BOX (hbox[E_POPKF_NU]), label, FALSE, FALSE,1);

data.entry[E_POPKF_NU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_POPKF_NU]), data.entry[E_POPKF_NU], TRUE, TRUE,1);
g_signal_connect(data.entry[E_POPKF_NU], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_POPKF_NU]),data.rk->popkf_nu.ravno());
gtk_widget_set_name(data.entry[E_POPKF_NU],iceb_u_inttochar(E_POPKF_NU));

label=gtk_label_new(gettext("Поправочный коэффициент для бух. учёта"));
gtk_box_pack_start (GTK_BOX (hbox[E_POPKF_BU]), label, FALSE, FALSE,1);

data.entry[E_POPKF_BU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_POPKF_BU]), data.entry[E_POPKF_BU], TRUE, TRUE,1);
g_signal_connect(data.entry[E_POPKF_BU], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_POPKF_BU]),data.rk->popkf_bu.ravno());
gtk_widget_set_name(data.entry[E_POPKF_BU],iceb_u_inttochar(E_POPKF_BU));


label=gtk_label_new(gettext("Гос. номерной знак автотранспорта"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_ZNAK]), label, FALSE, FALSE,1);

data.entry[E_NOM_ZNAK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_NOM_ZNAK]), data.entry[E_NOM_ZNAK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NOM_ZNAK], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOM_ZNAK]),data.rk->nom_znak.ravno());
gtk_widget_set_name(data.entry[E_NOM_ZNAK],iceb_u_inttochar(E_NOM_ZNAK));


sprintf(strsql,"%s (,,)",gettext("Код материально-ответственного"));
data.knopka_enter[E_MAT_OT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_MAT_OT]), data.knopka_enter[E_MAT_OT], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_MAT_OT],"clicked",G_CALLBACK(l_uosin_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_MAT_OT],iceb_u_inttochar(E_MAT_OT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_MAT_OT],gettext("Выбор материально-ответственного"));

data.entry[E_MAT_OT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_MAT_OT]), data.entry[E_MAT_OT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_MAT_OT], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MAT_OT]),data.rk->mat_ot.ravno());
gtk_widget_set_name(data.entry[E_MAT_OT],iceb_u_inttochar(E_MAT_OT));



sprintf(strsql,"%s (,,)",gettext("Подразделение"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(l_uosin_p_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PODR],gettext("Выбор подразделения"));

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(l_uosin_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->podr.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));




sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_uosin_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_uosin_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Поиск не выполнять"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_uosin_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE,1);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();

if(data.voz == 0)
 rek_poi->metka_poi=1;
else
 rek_poi->metka_poi=0;
  
return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_uosin_p_e_knopka(GtkWidget *widget,class l_uosin_p_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATA_VVODA:

    if(iceb_calendar(&data->rk->data_vvoda,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_VVODA]),data->rk->data_vvoda.ravno());
      
    return;  

   
  case E_SHETU:

    iceb_vibrek(0,"Plansh",&data->rk->shetu,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),data->rk->shetu.ravno());

    return;

  case E_HZT:

    if(l_uoshz(1,&kod,&naim,data->window) == 0)
     data->rk->hzt.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_HZT]),data->rk->hzt.ravno());

    return;
  
  case E_HAU:

    if(l_uoshau(1,&kod,&naim,data->window) == 0)
     data->rk->hau.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_HAU]),data->rk->hau.ravno());

    return;

  case E_HNA:

    if(l_uosgrnu(1,&kod,&naim,data->window) == 0)
     data->rk->hna.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_HNA]),data->rk->hna.ravno());

    return;

  case E_HNABU:

    if(l_uosgrbu(1,&kod,&naim,data->window) == 0)
     data->rk->hnabu.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_HNABU]),data->rk->hnabu.ravno());

    return;

  case E_MAT_OT:

    if(l_uosmo(1,&kod,&naim,data->window) == 0)
     data->rk->mat_ot.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_MAT_OT]),data->rk->mat_ot.ravno());

    return;
  case E_PODR:

    if(l_uospodr(1,&kod,&naim,data->window) == 0)
     data->rk->podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno());

    return;
 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_uosin_p_key_press(GtkWidget *widget,GdkEventKey *event,class l_uosin_p_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(FALSE);

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
void  l_uosin_p_knopka(GtkWidget *widget,class l_uosin_p_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_data();
    return;  


  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=1;
    gtk_widget_destroy(data->window);

    return;
 }
}
/********************************/
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_uosin_p_vvod(GtkWidget *widget,class l_uosin_p_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_ZAVOD_IZ:
    data->rk->zavod_iz.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_PASPORT:
    data->rk->pasport.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_MODEL:
    data->rk->model.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_ZAV_NOMER:
    data->rk->zav_nomer.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DATA_VVODA:
    data->rk->data_vvoda.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHETU:
    data->rk->shetu.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_HZT:
    data->rk->hzt.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_HAU:
    data->rk->hau.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_HNA:
    data->rk->hna.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
  case E_HNABU:
    data->rk->hnabu.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_POPKF_NU:
    data->rk->popkf_nu.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_POPKF_BU:
    data->rk->popkf_bu.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NOM_ZNAK:
    data->rk->nom_znak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_MAT_OT:
    data->rk->mat_ot.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_PODR:
    data->rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_GOD_IZ:
    data->rk->god_iz.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NAIM:
    data->rk->naim.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
 }
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
