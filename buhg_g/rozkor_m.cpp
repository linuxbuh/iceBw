/*$Id: rozkor_m.c,v 1.16 2013/09/26 09:46:54 sasa Exp $*/
/*12.07.2015	20.10.2004	Белых А.И.	rozkor_m.c
Меню для расчёта возврата
*/
#include "buhg_g.h"
#include "rozkor.h"
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_NOMRAS,
  E_NOMNALNAK,
  E_KRAS,
  E_PRIHINA,
  E_NOMDOG,
  E_DATDOG,
  E_GODNAK,
  KOLENTER  
 };

class rozkor_m_data
 {
  public:
  rozkor_data  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  rozkor_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
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
    rk->clear_data();
   }
 };



gboolean   rozkor_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class rozkor_m_data *data);
void    rozkor_m_v_vvod(GtkWidget *widget,class rozkor_m_data *data);
void  rozkor_m_v_knopka(GtkWidget *widget,class rozkor_m_data *data);
void   rozkor_m_rekviz(class rozkor_m_data *data);

extern SQL_baza bd;

int   rozkor_m(class rozkor_data *rek_ras,GtkWidget *wpredok)
{
//rozkor_m_rr  rek_ras;
rozkor_m_data data;

data.rk=rek_ras;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Оформить возврат"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rozkor_m_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Оформить возврат"));

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

sprintf(strsql,"%s",gettext("Номер расчёта"));
label=gtk_label_new(strsql);
data.entry[E_NOMRAS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMRAS]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMRAS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMRAS]), data.entry[E_NOMRAS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMRAS], "activate",G_CALLBACK(rozkor_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMRAS]),data.rk->nomer_ras.ravno());
gtk_widget_set_name(data.entry[E_NOMRAS],iceb_u_inttochar(E_NOMRAS));

label=gtk_label_new(gettext("Номер налоговой накладной"));
data.entry[E_NOMNALNAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMNALNAK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMNALNAK]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMNALNAK]), data.entry[E_NOMNALNAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMNALNAK], "activate",G_CALLBACK(rozkor_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMNALNAK]),data.rk->nomer_nal_nak.ravno());
gtk_widget_set_name(data.entry[E_NOMNALNAK],iceb_u_inttochar(E_NOMNALNAK));

sprintf(strsql,"%s",gettext("Кто выполняет расчёт"));
label=gtk_label_new(strsql);
data.entry[E_KRAS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KRAS]),100);
gtk_box_pack_start (GTK_BOX (hbox[E_KRAS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KRAS]), data.entry[E_KRAS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KRAS], "activate",G_CALLBACK(rozkor_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KRAS]),data.rk->kto_v_ras.ravno());
gtk_widget_set_name(data.entry[E_KRAS],iceb_u_inttochar(E_KRAS));

sprintf(strsql,"%s",gettext("Причина расчёта"));
label=gtk_label_new(strsql);
data.entry[E_PRIHINA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PRIHINA]),100);
gtk_box_pack_start (GTK_BOX (hbox[E_PRIHINA]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_PRIHINA]), data.entry[E_PRIHINA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PRIHINA], "activate",G_CALLBACK(rozkor_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PRIHINA]),data.rk->prihina.ravno());
gtk_widget_set_name(data.entry[E_PRIHINA],iceb_u_inttochar(E_PRIHINA));

sprintf(strsql,"%s",gettext("Номер договора"));
label=gtk_label_new(strsql);
data.entry[E_NOMDOG] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOG]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOG]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOG]), data.entry[E_NOMDOG], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMDOG], "activate",G_CALLBACK(rozkor_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOG]),data.rk->nomer_dogovora.ravno());
gtk_widget_set_name(data.entry[E_NOMDOG],iceb_u_inttochar(E_NOMDOG));

sprintf(strsql,"%s",gettext("Дата договора"));
label=gtk_label_new(strsql);
data.entry[E_DATDOG] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATDOG]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATDOG]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DATDOG]), data.entry[E_DATDOG], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATDOG], "activate",G_CALLBACK(rozkor_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATDOG]),data.rk->data_dogovora.ravno());
gtk_widget_set_name(data.entry[E_DATDOG],iceb_u_inttochar(E_DATDOG));

sprintf(strsql,"%s",gettext("Год налоговой накладной"));
label=gtk_label_new(strsql);
data.entry[E_GODNAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_GODNAK]),4);
gtk_box_pack_start (GTK_BOX (hbox[E_GODNAK]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_GODNAK]), data.entry[E_GODNAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GODNAK], "activate",G_CALLBACK(rozkor_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GODNAK]),data.rk->god_nal_nak.ravno());
gtk_widget_set_name(data.entry[E_GODNAK],iceb_u_inttochar(E_GODNAK));




sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(rozkor_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(rozkor_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(rozkor_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);

}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rozkor_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class rozkor_m_data *data)
{

//printf("rozkor_m_v_key_press\n");
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
void  rozkor_m_v_knopka(GtkWidget *widget,class rozkor_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    rozkor_m_v_vvod(GtkWidget *widget,class rozkor_m_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("rozkor_m_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_NOMRAS:
    data->rk->nomer_ras.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMNALNAK:
    data->rk->nomer_nal_nak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KRAS:
    data->rk->kto_v_ras.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PRIHINA:
    data->rk->prihina.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMDOG:
    data->rk->nomer_dogovora.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATDOG:
    data->rk->data_dogovora.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_GODNAK:
    data->rk->god_nal_nak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
