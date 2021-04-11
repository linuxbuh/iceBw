/*$Id: iceb_l_kontrp.c,v 1.23 2013/08/25 08:27:06 sasa Exp $*/
/*26.02.2016	03.12.2003	Белых А.И.	iceb_l_kontrp.c
Ввод и корректировка реквизитов поиска записей
*/
#include	"iceb_libbuh.h"
#include        "iceb_l_kontr.h"

enum
 {
  E_KONTR,
  E_NAIMK,
  E_ADRES,
  E_KOD,
  E_NOMSH,
  E_MFO,
  E_NAIMB,
  E_ADRESB,
  E_NSPNDS,
  E_GRUP,
  E_TELEF,
  E_NA,
  E_REGNOM,
  E_INNN,
  E_NAIMP,
  E_GK,
  KOLENTER  
 };

enum
 {
  FK1,
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class iceb_l_kontrp_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *window;
  GtkWidget *opt1;
  GtkWidget *label_link;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class kontr_rek *rk;
  
  
  iceb_l_kontrp_data() //Конструктор
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
    rk->metka_nal=gtk_combo_box_get_active (GTK_COMBO_BOX(opt1));

   }
 };

void    kontrp_vvod(GtkWidget *widget,class iceb_l_kontrp_data *data);
void  kontrp_knopka(GtkWidget *widget,class iceb_l_kontrp_data *data);
gboolean   kontrp_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_kontrp_data *data);
void kontrp_clear(class iceb_l_kontrp_data *data);


int      iceb_l_kontrp(class kontr_rek *rek_poi,GtkWidget *wpredok)
{
int gor=0;
int ver=0;

char    strsql[512];

class iceb_l_kontrp_data data;
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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(kontrp_key_press),&data);

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
gtk_container_add (GTK_CONTAINER (data.window), vbox);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox3),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox4),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox5),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox6 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox6),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox7 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox7),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox8 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox8),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox9),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox10 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox10),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox_en = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox_en),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (vbox), hbox);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hbox4);
gtk_container_add (GTK_CONTAINER (vbox), hbox5);
gtk_container_add (GTK_CONTAINER (vbox), hbox6);
gtk_container_add (GTK_CONTAINER (vbox), hbox7);
gtk_container_add (GTK_CONTAINER (vbox), hbox8);
gtk_container_add (GTK_CONTAINER (vbox), hbox9);
gtk_container_add (GTK_CONTAINER (vbox), hbox10);

gtk_container_add (GTK_CONTAINER (vbox), hbox_en);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkWidget *label=gtk_label_new(gettext("Код контрагента"));
data.entry[E_KONTR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KONTR]),19);
gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), data.entry[E_KONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk->kodkontr.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));

label=gtk_label_new(gettext("Наименование"));
data.entry[E_NAIMK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIMK]),79);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_NAIMK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIMK], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIMK]),data.rk->naimkon.ravno());
gtk_widget_set_name(data.entry[E_NAIMK],iceb_u_inttochar(E_NAIMK));


label=gtk_label_new(gettext("Адрес"));
data.entry[E_ADRES] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_ADRES]),99);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_ADRES], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ADRES], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRES]),data.rk->adres.ravno());
gtk_widget_set_name(data.entry[E_ADRES],iceb_u_inttochar(E_ADRES));

label=gtk_label_new(gettext("Код ЕГРПОУ"));
data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),19);
gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox3), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.rk->kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));

label=gtk_label_new(gettext("Счет"));
data.entry[E_NOMSH] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMSH]),29);
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4), data.entry[E_NOMSH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMSH], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMSH]),data.rk->nomsh.ravno());
gtk_widget_set_name(data.entry[E_NOMSH],iceb_u_inttochar(E_NOMSH));

label=gtk_label_new(gettext("МФО"));
data.entry[E_MFO] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MFO]),19);
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4), data.entry[E_MFO], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_MFO], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MFO]),data.rk->mfo.ravno());
gtk_widget_set_name(data.entry[E_MFO],iceb_u_inttochar(E_MFO));

label=gtk_label_new(gettext("Наименование банка"));
data.entry[E_NAIMB] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIMB]),79);
gtk_box_pack_start (GTK_BOX (hbox5), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox5), data.entry[E_NAIMB], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIMB], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIMB]),data.rk->naimban.ravno());
gtk_widget_set_name(data.entry[E_NAIMB],iceb_u_inttochar(E_NAIMB));

label=gtk_label_new(gettext("Город банка"));
data.entry[E_ADRESB] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_ADRESB]),99);
gtk_box_pack_start (GTK_BOX (hbox6), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox6), data.entry[E_ADRESB], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ADRESB], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ADRESB]),data.rk->adresb.ravno());
gtk_widget_set_name(data.entry[E_ADRESB],iceb_u_inttochar(E_ADRESB));

label=gtk_label_new(gettext("Номер св. НДС"));
data.entry[E_NSPNDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NSPNDS]),19);
gtk_box_pack_start (GTK_BOX (hbox6), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox6), data.entry[E_NSPNDS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NSPNDS], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NSPNDS]),data.rk->nspnds.ravno());
gtk_widget_set_name(data.entry[E_NSPNDS],iceb_u_inttochar(E_NSPNDS));

label=gtk_label_new(gettext("Группа"));
data.entry[E_GRUP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GRUP]),9);
gtk_box_pack_start (GTK_BOX (hbox7), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox7), data.entry[E_GRUP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GRUP], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUP]),data.rk->grup.ravno());
gtk_widget_set_name(data.entry[E_GRUP],iceb_u_inttochar(E_GRUP));

label=gtk_label_new(gettext("Телефон"));
data.entry[E_TELEF] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_TELEF]),79);
gtk_box_pack_start (GTK_BOX (hbox7), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox7), data.entry[E_TELEF], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_TELEF], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_TELEF]),data.rk->telef.ravno());
gtk_widget_set_name(data.entry[E_TELEF],iceb_u_inttochar(E_TELEF));

label=gtk_label_new(gettext("Налоговый адрес"));
data.entry[E_NA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NA]),79);
gtk_box_pack_start (GTK_BOX (hbox8), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox8), data.entry[E_NA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NA], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NA]),data.rk->na.ravno());
gtk_widget_set_name(data.entry[E_NA],iceb_u_inttochar(E_NA));

label=gtk_label_new(gettext("Реги.ном.ч/п"));
data.entry[E_REGNOM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_REGNOM]),29);
gtk_box_pack_start (GTK_BOX (hbox9), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox9), data.entry[E_REGNOM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_REGNOM], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_REGNOM]),data.rk->regnom.ravno());
gtk_widget_set_name(data.entry[E_REGNOM],iceb_u_inttochar(E_REGNOM));

label=gtk_label_new(gettext("Инд. налог.номер"));
data.entry[E_INNN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_INNN]),19);
gtk_box_pack_start (GTK_BOX (hbox9), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox9), data.entry[E_INNN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_INNN], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INNN]),data.rk->innn.ravno());
gtk_widget_set_name(data.entry[E_INNN],iceb_u_inttochar(E_INNN));

label=gtk_label_new(gettext("Полное наименование"));
data.entry[E_NAIMP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIMP]),99);
gtk_box_pack_start (GTK_BOX (hbox10), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox10), data.entry[E_NAIMP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIMP], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIMP]),data.rk->naimp.ravno());
gtk_widget_set_name(data.entry[E_NAIMP],iceb_u_inttochar(E_NAIMP));

label=gtk_label_new(gettext("Населённый пункт"));
data.entry[E_GK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GK]),255);
gtk_box_pack_start (GTK_BOX (hbox10), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox10), data.entry[E_GK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GK], "activate",G_CALLBACK(kontrp_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GK]),data.rk->gk.ravno());
gtk_widget_set_name(data.entry[E_GK],iceb_u_inttochar(E_GK));

label=gtk_label_new(gettext("Система налога"));
gtk_box_pack_start (GTK_BOX (hbox_en), label, FALSE, FALSE, 1);

class iceb_u_spisok bal_st;
bal_st.plus(gettext("Все"));
bal_st.plus(gettext("Общая система"));
bal_st.plus(gettext("Единый налог"));

iceb_pm_vibor(&bal_st,&data.opt1,data.rk->metka_nal);
gtk_box_pack_start (GTK_BOX (hbox_en), data.opt1, TRUE, TRUE,1);





sprintf(strsql,"<a href=\"%s/%s.html\" title=\"%s\">F1 %s</a>",ICEB_PUT_NA_HTML,__FUNCTION__,gettext("Просмотр документации"),gettext("Помощь"));
data.label_link=gtk_label_new(strsql);
gtk_label_set_use_markup (GTK_LABEL (data.label_link), TRUE);
g_signal_connect(data.label_link, "activate-current-link", G_CALLBACK (iceb_activate_link), NULL);

data.knopka[FK1]=gtk_button_new();
gtk_container_add (GTK_CONTAINER (data.knopka[FK1]), data.label_link);
g_signal_connect(data.knopka[FK1],"clicked",G_CALLBACK(kontrp_knopka),&data);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK1], TRUE, TRUE, 0);


sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(kontrp_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(kontrp_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Поиск не выполнять"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(kontrp_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);
         
gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}


/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   kontrp_key_press(GtkWidget *widget,GdkEventKey *event,class iceb_l_kontrp_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F1:
    g_signal_emit_by_name(data->knopka[FK1],"clicked");
    return(TRUE);

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
void  kontrp_knopka(GtkWidget *widget,class iceb_l_kontrp_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
data->kl_shift=0;

switch (knop)
 {
  case FK1:
    g_signal_emit_by_name(data->label_link,"activate-current-link");
    return;

  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    data->voz=0;
    
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    kontrp_clear(data);
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

void    kontrp_vvod(GtkWidget *widget,class iceb_l_kontrp_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_KONTR:
    data->rk->kodkontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_NAIMK:
    data->rk->naimkon.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_ADRES:
    data->rk->adres.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_ADRESB:
    data->rk->adresb.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOD:
    data->rk->kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMSH:
    data->rk->nomsh.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_MFO:
    data->rk->mfo.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NAIMB:
    data->rk->naimban.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NSPNDS:
    data->rk->nspnds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_GRUP:
    data->rk->grup.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


  case E_TELEF:
    data->rk->telef.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NA:
    data->rk->na.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_REGNOM:
    data->rk->regnom.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_INNN:
    data->rk->innn.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NAIMP:
    data->rk->naimp.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_GK:
    data->rk->gk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
 }
enter++;
if(enter >= KOLENTER)
 enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Очистка меню от введенной информации*/
/**************************************/
void kontrp_clear(class iceb_l_kontrp_data *data)
{

data->rk->clear_zero();
for(int i=0; i< KOLENTER; i++)
 gtk_entry_set_text(GTK_ENTRY(data->entry[i]),"");

gtk_widget_grab_focus(data->entry[0]);

}
