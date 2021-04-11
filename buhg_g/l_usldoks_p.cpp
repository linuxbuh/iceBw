/*$Id: l_usldoks_p.c,v 1.20 2013/09/26 09:46:51 sasa Exp $*/
/*02.03.2016	18.07.2005	Белых А.И.	l_usldoks_p.c
Ввод реквизитов поиска документов
*/
#include "buhg_g.h"
#include "l_usldoks.h"

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
  E_KODOP,
  E_KODPOD,
  E_PR_RAS,
  E_NAIM_KONTR,
  E_NALNAK,  
  E_DOVER,
  KOLENTER  
 };

class usldoks_p_data
 {
  public:
  class usldoks_rek  *rpoi;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt;
  class iceb_u_str name_window;
  short kl_shift;
  
  
  usldoks_p_data() //Конструктор
   {
    kl_shift=0;
  
   }

  void read_rek()
   {
    rpoi->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK])));
    rpoi->dover.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DOVER])));
    rpoi->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN])));
    rpoi->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK])));
    rpoi->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODOP])));
    rpoi->kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR])));
    rpoi->kodpod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODPOD])));
    rpoi->pr_ras.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PR_RAS])));
    rpoi->naim_kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM_KONTR])));
    rpoi->nalnak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NALNAK])));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    rpoi->clear();
   }
 };

gboolean   usldoks_p_key_press(GtkWidget *widget,GdkEventKey *event,class usldoks_p_data *data);
void    usldoks_p_vvod(GtkWidget *widget,class usldoks_p_data *data);
void  usldoks_p_knopka(GtkWidget *widget,class usldoks_p_data *data);

void  usldoks_v_e_knopka(GtkWidget *widget,class usldoks_p_data *data);

extern SQL_baza bd;

int l_usldoks_p(class usldoks_rek *datap,GtkWidget *wpredok)
{
class usldoks_p_data data;
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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(usldoks_p_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Поиск"));

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s",gettext("Номер документа"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE, 0);
data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(usldoks_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rpoi->nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));

sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(usldoks_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(usldoks_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rpoi->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));


sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(usldoks_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(usldoks_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rpoi->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));

sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KONTR],"clicked",G_CALLBACK(usldoks_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KONTR],iceb_u_inttochar(E_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KONTR],gettext("Выбор контрагента"));

data.entry[E_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(usldoks_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rpoi->kontr.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));

sprintf(strsql,"%s (,,)",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODOP],"clicked",G_CALLBACK(usldoks_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODOP],iceb_u_inttochar(E_KODOP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODOP],gettext("Выбор операции"));

data.entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODOP], "activate",G_CALLBACK(usldoks_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rpoi->kodop.ravno());
gtk_widget_set_name(data.entry[E_KODOP],iceb_u_inttochar(E_KODOP));

sprintf(strsql,"%s (,,)",gettext("Код подразделения"));
data.knopka_enter[E_KODPOD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODPOD]), data.knopka_enter[E_KODPOD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODPOD],"clicked",G_CALLBACK(usldoks_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODPOD],iceb_u_inttochar(E_KODPOD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODPOD],gettext("Выбор подразделения"));

data.entry[E_KODPOD] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODPOD]), data.entry[E_KODPOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODPOD], "activate",G_CALLBACK(usldoks_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODPOD]),data.rpoi->kodpod.ravno());
gtk_widget_set_name(data.entry[E_KODPOD],iceb_u_inttochar(E_KODPOD));

sprintf(strsql,"%s (+/-)",gettext("Приход/расход"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PR_RAS]), label, FALSE, FALSE, 0);

data.entry[E_PR_RAS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PR_RAS]),1);
gtk_box_pack_start (GTK_BOX (hbox[E_PR_RAS]), data.entry[E_PR_RAS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PR_RAS], "activate",G_CALLBACK(usldoks_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PR_RAS]),data.rpoi->pr_ras.ravno());
gtk_widget_set_name(data.entry[E_PR_RAS],iceb_u_inttochar(E_PR_RAS));

sprintf(strsql,"%s (,,)",gettext("Наименование контрагента"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM_KONTR]), label, FALSE, FALSE, 0);

data.entry[E_NAIM_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM_KONTR]), data.entry[E_NAIM_KONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM_KONTR], "activate",G_CALLBACK(usldoks_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM_KONTR]),data.rpoi->naim_kontr.ravno());
gtk_widget_set_name(data.entry[E_NAIM_KONTR],iceb_u_inttochar(E_NAIM_KONTR));

sprintf(strsql,"%s (,,)",gettext("Номер налоговой накладной"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NALNAK]), label, FALSE, FALSE, 0);

data.entry[E_NALNAK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_NALNAK]), data.entry[E_NALNAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NALNAK], "activate",G_CALLBACK(usldoks_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NALNAK]),data.rpoi->nalnak.ravno());
gtk_widget_set_name(data.entry[E_NALNAK],iceb_u_inttochar(E_NALNAK));

sprintf(strsql,"%s (,,)",gettext("Доверенность"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DOVER]), label, FALSE, FALSE, 0);

data.entry[E_DOVER] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_DOVER]), data.entry[E_DOVER], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DOVER], "activate",G_CALLBACK(usldoks_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DOVER]),data.rpoi->dover.ravno());
gtk_widget_set_name(data.entry[E_DOVER],iceb_u_inttochar(E_DOVER));




sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(usldoks_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(usldoks_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(usldoks_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(0);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  usldoks_v_e_knopka(GtkWidget *widget,class usldoks_p_data *data)
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

  case E_DATAK:

    if(iceb_calendar(&data->rpoi->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rpoi->datak.ravno());
      
    return;  

  case E_KONTR:

    iceb_vibrek(0,"Kontragent",&data->rpoi->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rpoi->kontr.ravno());

    return;  

  case E_KODPOD:
    if(l_uslpodr(1,&kod,&naim,data->window) == 0)
     data->rpoi->kodpod.z_plus(kod.ravno());    
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODPOD]),data->rpoi->kodpod.ravno());

    return;  

  case E_KODOP:
    vib_kodop_usl(&data->rpoi->kodop,data->window);

    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rpoi->kodop.ravno());
    
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   usldoks_p_key_press(GtkWidget *widget,GdkEventKey *event,class usldoks_p_data *data)
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
void  usldoks_p_knopka(GtkWidget *widget,class usldoks_p_data *data)
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

    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->rpoi->metka_poi=1;
    return;  


  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->rpoi->metka_poi=0;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    usldoks_p_vvod(GtkWidget *widget,class usldoks_p_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_NOMDOK:
    data->rpoi->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAN:
    data->rpoi->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAK:
    data->rpoi->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KONTR:
    data->rpoi->kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODOP:
    data->rpoi->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODPOD:
    data->rpoi->kodpod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PR_RAS:
    data->rpoi->pr_ras.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NAIM_KONTR:
    data->rpoi->naim_kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NALNAK:
    data->rpoi->nalnak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }
enter+=1;

if(enter >= KOLENTER)
 enter=0;


gtk_widget_grab_focus(data->entry[enter]);
 
}
