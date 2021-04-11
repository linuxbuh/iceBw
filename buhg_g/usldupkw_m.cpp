/*$Id: usldupkw_m.c,v 1.14 2013/08/25 08:26:56 sasa Exp $*/
/*08.02.2015	08.02.2015	Белых А.И.	usldupkw_m.c
Меню для ввода реквизитов поиска для списания/получения услуг по контрагентам
*/
#include "buhg_g.h"
#include "usldupkw.h"
enum
 {
  E_DATAN,
  E_DATAK,
  E_PODR,
  E_KONTR,
  E_GRUP_USL,
  E_KODOP,
  E_SHETU,
  E_KODUSL,
  E_NOMDOK,
  KOLENTER
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 }; 

class usldupkw_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class usldupkw_rr *rk;
      
  usldupkw_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN])));
    rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK])));
    rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PODR])));
    rk->kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR])));
    rk->grup_usl.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GRUP_USL])));
    rk->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODOP])));
    rk->kodusl.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODUSL])));
    rk->shetu.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHETU])));
    rk->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK])));
    rk->prih_rash=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));

   }

  void clear_rek()
   {
    rk->clear_data();
    for(int i=0; i< KOLENTER; i++)
     gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_widget_grab_focus(entry[0]);
   }
 };

void    usldupkw_m_vvod(GtkWidget *widget,class usldupkw_m_data *data);
void  usldupkw_m_knopka(GtkWidget *widget,class usldupkw_m_data *data);
gboolean   usldupkw_m_key_press(GtkWidget *widget,GdkEventKey *event,class usldupkw_m_data *data);
void  usldupkw_m_e_knopka(GtkWidget *widget,class usldupkw_m_data *data);


int usldupkw_m(class usldupkw_rr *rek_poi,
GtkWidget *wpredok)
{

class usldupkw_m_data data;
char strsql[512];
data.rk=rek_poi;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт списания/получения материаллов по контрагентам"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(usldupkw_m_key_press),&data);

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

GtkWidget *label=gtk_label_new (gettext("Расчёт списания/получения материаллов по контрагентам"));
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);


GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox_opt = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox_opt),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox), hbox[i], FALSE, FALSE, 0);

gtk_box_pack_start (GTK_BOX (vbox), hbox_opt, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE, 0);



sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(usldupkw_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(usldupkw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));


sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(usldupkw_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(usldupkw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));


sprintf(strsql,"%s (,,)",gettext("Подразделение"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(usldupkw_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PODR],gettext("Выбор подразделения"));

data.entry[E_PODR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(usldupkw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk->podr.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));


sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KONTR],"clicked",G_CALLBACK(usldupkw_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KONTR],iceb_u_inttochar(E_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KONTR],gettext("Выбор контрагента"));

data.entry[E_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(usldupkw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk->kontr.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));


sprintf(strsql,"%s (,,)",gettext("Группа услуг"));
data.knopka_enter[E_GRUP_USL]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_USL]), data.knopka_enter[E_GRUP_USL], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_GRUP_USL],"clicked",G_CALLBACK(usldupkw_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_GRUP_USL],iceb_u_inttochar(E_GRUP_USL));
gtk_widget_set_tooltip_text(data.knopka_enter[E_GRUP_USL],gettext("Выбор группы"));

data.entry[E_GRUP_USL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP_USL]), data.entry[E_GRUP_USL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GRUP_USL], "activate",G_CALLBACK(usldupkw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUP_USL]),data.rk->grup_usl.ravno());
gtk_widget_set_name(data.entry[E_GRUP_USL],iceb_u_inttochar(E_GRUP_USL));

sprintf(strsql,"%s (,,)",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODOP],"clicked",G_CALLBACK(usldupkw_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODOP],iceb_u_inttochar(E_KODOP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODOP],gettext("Выбор операции"));

data.entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODOP], "activate",G_CALLBACK(usldupkw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk->kodop.ravno());
gtk_widget_set_name(data.entry[E_KODOP],iceb_u_inttochar(E_KODOP));


sprintf(strsql,"%s (,,)",gettext("Счёт учёта"));
data.knopka_enter[E_SHETU]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.knopka_enter[E_SHETU], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHETU],"clicked",G_CALLBACK(usldupkw_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHETU],iceb_u_inttochar(E_SHETU));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHETU],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHETU] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.entry[E_SHETU], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHETU], "activate",G_CALLBACK(usldupkw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETU]),data.rk->shetu.ravno());
gtk_widget_set_name(data.entry[E_SHETU],iceb_u_inttochar(E_SHETU));


sprintf(strsql,"%s (,,)",gettext("Код услуги"));
data.knopka_enter[E_KODUSL]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODUSL]), data.knopka_enter[E_KODUSL], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODUSL],"clicked",G_CALLBACK(usldupkw_m_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODUSL],iceb_u_inttochar(E_KODUSL));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODUSL],gettext("Выбор услуги"));

data.entry[E_KODUSL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODUSL]), data.entry[E_KODUSL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODUSL], "activate",G_CALLBACK(usldupkw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODUSL]),data.rk->kodusl.ravno());
gtk_widget_set_name(data.entry[E_KODUSL],iceb_u_inttochar(E_KODUSL));


sprintf(strsql,"%s (,,)",gettext("Номер документа"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE, 0);

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(usldupkw_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk->nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));


sprintf(strsql,"%s",gettext("Приход/расход"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox_opt), label, FALSE, FALSE, 0);

iceb_pm_pr(&data.opt,data.rk->prih_rash);
gtk_box_pack_start (GTK_BOX (hbox_opt), data.opt, TRUE, TRUE, 0);


sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(usldupkw_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введеноой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(usldupkw_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Поиск не выполнять"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(usldupkw_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);
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
void  usldupkw_m_e_knopka(GtkWidget *widget,class usldupkw_m_data *data)
{
class iceb_u_str kod;
class iceb_u_str naim;
naim.plus("");
kod.plus("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("go_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rk->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rk->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());
      
    return;  

  case E_PODR:

    if(l_uslpodr(1,&kod,&naim,data->window) == 0)
      data->rk->podr.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk->podr.ravno());
    return;  

  case E_KONTR:

    iceb_vibrek(0,"Kontragent",&data->rk->kontr,&naim,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk->kontr.ravno());
      
    return;  


  case E_GRUP_USL:

    if(l_uslgrup(1,&kod,&naim,data->window) == 0)
     data->rk->grup_usl.z_plus(kod.ravno());     
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUP_USL]),data->rk->grup_usl.ravno());
      
    return;  

  case E_KODOP:

    vib_kodop_usl(&data->rk->kodop,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rk->kodop.ravno());
      
    return;  

  case E_SHETU:

    iceb_vibrek(0,"Plansh",&data->rk->shetu,&naim,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),data->rk->shetu.ravno());
      
    return;  

  case E_KODUSL:
    if(l_uslugi(1,&kod,&naim,data->window) == 0)
     data->rk->kodusl.z_plus(kod.ravno());
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODUSL]),data->rk->kodusl.ravno());
      
    return;  
   
 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   usldupkw_m_key_press(GtkWidget *widget,GdkEventKey *event,class usldupkw_m_data *data)
{
//char  bros[512];

//printf("vei_key_press\n");
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
void  usldupkw_m_knopka(GtkWidget *widget,class usldupkw_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(iceb_rsdatp(data->rk->datan.ravno(),data->rk->datak.ravno(),data->window) != 0)
     return;

    data->voz=0;
    gtk_widget_destroy(data->window);
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
/*чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    usldupkw_m_vvod(GtkWidget *widget,class usldupkw_m_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DATAK:
    data->rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PODR:
    data->rk->podr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KONTR:
    data->rk->kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_GRUP_USL:
    data->rk->grup_usl.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODOP:
    data->rk->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHETU:
    data->rk->shetu.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODUSL:
    data->rk->kodusl.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMDOK:
    data->rk->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }
enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
