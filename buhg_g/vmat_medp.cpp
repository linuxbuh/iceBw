/*$Id: vmat_medp.c,v 1.15 2013/09/26 09:46:58 sasa Exp $*/
/*18.07.2004	18.07.2004	Белых А.И.	vmat_medp.c
Вод инвентарного номера и даты ввода в эксплуатацию для малоценки
*/
#include "buhg_g.h"

enum
{
  FK2,
//  FK3,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAR,
  E_SERIQ,
  E_REGNOM,
  KOLENTER  
 };

class vmat_medp_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  iceb_u_str datar;
  iceb_u_str seriq;
  iceb_u_str regnom;

  vmat_medp_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear();
    
   }
  void clear()
   {
    datar.new_plus("");
    seriq.new_plus("");
    regnom.new_plus("");
   }
  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    clear();
   }
 };

void    vmat_medp_v_vvod(GtkWidget *widget,class vmat_medp_data *data);
void  vmat_medp_v_knopka(GtkWidget *widget,class vmat_medp_data *data);
void   vmat_medp_rekviz(class vmat_medp_data *data);
gboolean   vmat_medp_v_key_press(GtkWidget *widget,GdkEventKey *event,class vmat_medp_data *data);

extern SQL_baza bd;

int vmat_medp(iceb_u_str *innom,short *dn,short *mn,short *gn,iceb_u_str *regnom,GtkWidget *wpredok)
{
vmat_medp_data data;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод карточки мед. препората"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vmat_medp_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label=gtk_label_new(gettext("Ввод карточки мед. препората"));

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

sprintf(strsql,"%s",gettext("Дата регистрации медицинского препарата"));
label=gtk_label_new(strsql);
data.entry[E_DATAR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAR]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAR]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAR]), data.entry[E_DATAR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAR], "activate",G_CALLBACK(vmat_medp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAR]),data.datar.ravno());
gtk_widget_set_name(data.entry[E_DATAR],iceb_u_inttochar(E_DATAR));

sprintf(strsql,"%s",gettext("Серия медицинского препарата"));
label=gtk_label_new(strsql);
data.entry[E_SERIQ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SERIQ]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SERIQ]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SERIQ]), data.entry[E_SERIQ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SERIQ], "activate",G_CALLBACK(vmat_medp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SERIQ]),data.seriq.ravno());
gtk_widget_set_name(data.entry[E_SERIQ],iceb_u_inttochar(E_SERIQ));

sprintf(strsql,"%s",gettext("Регистрационный номер документа мед.прерарата"));
label=gtk_label_new(strsql);
data.entry[E_REGNOM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_REGNOM]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_REGNOM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_REGNOM]), data.entry[E_REGNOM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_REGNOM], "activate",G_CALLBACK(vmat_medp_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_REGNOM]),data.regnom.ravno());
gtk_widget_set_name(data.entry[E_REGNOM],iceb_u_inttochar(E_REGNOM));



sprintf(strsql,"F2 %s",gettext("Записать"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Записать введенную в меню информацию."));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(vmat_medp_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

/*******************
sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Просмотр нужных реквизитов."));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(vmat_medp_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);
******************/

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации."));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(vmat_medp_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне."));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(vmat_medp_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 {
  innom->new_plus(data.seriq.ravno());
  regnom->new_plus(data.regnom.ravno());
  iceb_u_rsdat(dn,mn,gn,data.datar.ravno(),1);
 }
if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);


}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vmat_medp_v_key_press(GtkWidget *widget,GdkEventKey *event,class vmat_medp_data *data)
{

//printf("vmat_medp_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);
/************
  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
    return(TRUE);
***************/
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
void  vmat_medp_v_knopka(GtkWidget *widget,class vmat_medp_data *data)
{
short dn,mn,gn;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(iceb_u_rsdat(&dn,&mn,&gn,data->datar.ravno(),1) != 0)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не верно введена дата !"));
      iceb_menu_soob(&repl,data->window);
      return;
     }
    if(data->regnom.getdlinna() <= 1)
     data->regnom.new_plus("N");
     
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

//  case FK3:
//    return;

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    vmat_medp_v_vvod(GtkWidget *widget,class vmat_medp_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_DATAR:
    data->datar.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SERIQ:
    data->seriq.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_REGNOM:
    data->regnom.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
