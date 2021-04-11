/*$Id: ost_skl_shetw_m.c,v 1.14 2013/09/26 09:46:52 sasa Exp $*/
/*12.07.2015	17.11.2008	Белых А.И. ost_skl_shetw_m.c
Меню для ввода данных 
*/
#include "buhg_g.h"
#include "ost_skl_shetw.h"
enum
{
  FK2,
  FK3,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SKLAD,
  E_GRUPA,
  E_SHET,
  E_KODMAT,
  E_DATA_OST,
  KOLENTER  
 };

class ost_skl_shetw_m_data
 {
  public:
  ost_skl_shetw_data  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет
  
  ost_skl_shetw_m_data() //Конструктор
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



gboolean   ost_skl_shetw_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class ost_skl_shetw_m_data *data);
void    ost_skl_shetw_m_v_vvod(GtkWidget *widget,class ost_skl_shetw_m_data *data);
void  ost_skl_shetw_m_v_knopka(GtkWidget *widget,class ost_skl_shetw_m_data *data);
void   ost_skl_shetw_m_rekviz(class ost_skl_shetw_m_data *data);
void  ost_skl_shetw_v_e_knopka(GtkWidget *widget,class ost_skl_shetw_m_data *data);


void ost_skl_shetw_sr(class ost_skl_shetw_m_data *data);

extern SQL_baza bd;

int   ost_skl_shetw_m(class ost_skl_shetw_data *rek_ras,GtkWidget *wpredok)
{
//ost_skl_shetw_m_rr  rek_ras;
ost_skl_shetw_m_data data;
char strsql[512];

data.rk=rek_ras;
if(data.rk->data_ost.getdlinna() <= 1)
 {
  short dt,mt,gt;
  iceb_u_poltekdat(&dt,&mt,&gt);
  sprintf(strsql,"%d.%d.%d",dt,mt,gt);
  data.rk->data_ost.new_plus(strsql);  
 }


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт остатков по складам и счетам учёта"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(ost_skl_shetw_m_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Расчёт остатков по складам и счетам учёта"));

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
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


sprintf(strsql,"%s (,,)",gettext("Слад"));
data.knopka_enter[E_SKLAD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.knopka_enter[E_SKLAD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SKLAD],"clicked",G_CALLBACK(ost_skl_shetw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SKLAD],iceb_u_inttochar(E_SKLAD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SKLAD],gettext("Выбор склада"));

data.entry[E_SKLAD] = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.entry[E_SKLAD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SKLAD], "activate",G_CALLBACK(ost_skl_shetw_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKLAD]),data.rk->sklad.ravno());
gtk_widget_set_name(data.entry[E_SKLAD],iceb_u_inttochar(E_SKLAD));

sprintf(strsql,"%s (,,)",gettext("Группа"));
data.knopka_enter[E_GRUPA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.knopka_enter[E_GRUPA], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_GRUPA],"clicked",G_CALLBACK(ost_skl_shetw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_GRUPA],iceb_u_inttochar(E_GRUPA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_GRUPA],gettext("Выбор группы"));

data.entry[E_GRUPA] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPA]), data.entry[E_GRUPA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GRUPA], "activate",G_CALLBACK(ost_skl_shetw_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPA]),data.rk->grupa.ravno());
gtk_widget_set_name(data.entry[E_GRUPA],iceb_u_inttochar(E_GRUPA));

sprintf(strsql,"%s (,,)",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(ost_skl_shetw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(ost_skl_shetw_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

sprintf(strsql,"%s (,,)",gettext("Код материалла"));
data.knopka_enter[E_KODMAT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.knopka_enter[E_KODMAT], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODMAT],"clicked",G_CALLBACK(ost_skl_shetw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODMAT],iceb_u_inttochar(E_KODMAT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODMAT],gettext("Выбор материалла"));

data.entry[E_KODMAT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODMAT]), data.entry[E_KODMAT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODMAT], "activate",G_CALLBACK(ost_skl_shetw_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODMAT]),data.rk->kodmat.ravno());
gtk_widget_set_name(data.entry[E_KODMAT],iceb_u_inttochar(E_KODMAT));

sprintf(strsql,"%s",gettext("Дата остатка"));
data.knopka_enter[E_DATA_OST]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_OST]), data.knopka_enter[E_DATA_OST], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATA_OST],"clicked",G_CALLBACK(ost_skl_shetw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA_OST],iceb_u_inttochar(E_DATA_OST));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA_OST],gettext("Выбор даты"));

data.entry[E_DATA_OST] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA_OST]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_OST]), data.entry[E_DATA_OST], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATA_OST], "activate",G_CALLBACK(ost_skl_shetw_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_OST]),data.rk->data_ost.ravno());
gtk_widget_set_name(data.entry[E_DATA_OST],iceb_u_inttochar(E_DATA_OST));




sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(ost_skl_shetw_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Просмотр нужных списков, для заполнения нужными кодами реквизитов меню"));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(ost_skl_shetw_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(ost_skl_shetw_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(ost_skl_shetw_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
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
void  ost_skl_shetw_v_e_knopka(GtkWidget *widget,class ost_skl_shetw_m_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATA_OST:

    if(iceb_calendar(&data->rk->data_ost,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_OST]),data->rk->data_ost.ravno());
      
    return;  

   
  case E_SHET:

    iceb_vibrek(0,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno());

    return;  

  case E_SKLAD:

    iceb_vibrek(0,"Sklad",&data->rk->sklad,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno());

    return;  

  case E_GRUPA:

    iceb_vibrek(0,"Grup",&data->rk->grupa,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA]),data->rk->grupa.ravno());

    return;  

  case E_KODMAT:

    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
     {
      data->rk->kodmat.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODMAT]),data->rk->kodmat.ravno());
     }

    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   ost_skl_shetw_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class ost_skl_shetw_m_data *data)
{

//printf("ost_skl_shetw_m_v_key_press\n");
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
void  ost_skl_shetw_m_v_knopka(GtkWidget *widget,class ost_skl_shetw_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(data->rk->data_ost.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Не верно введена дата !"),data->window);
      return;       
     }
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK3:

    ost_skl_shetw_sr(data);
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

void    ost_skl_shetw_m_v_vvod(GtkWidget *widget,class ost_skl_shetw_m_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("ost_skl_shetw_m_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SKLAD:
    data->rk->sklad.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_GRUPA:
    data->rk->grupa.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET:
    data->rk->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODMAT:
    data->rk->kodmat.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATA_OST:
    data->rk->data_ost.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/*******************/
/*выбрать вид списка*/
/*********************/

void ost_skl_shetw_sr(class ost_skl_shetw_m_data *data)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное"));


punkt_m.plus(gettext("Просмотр списка складов"));//0
punkt_m.plus(gettext("Просмотр списка групп"));//1
punkt_m.plus(gettext("Просмотр списка счетов"));//2
punkt_m.plus(gettext("Просмотр списка материалов"));//3


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");

switch(nomer)
 {
  case 0:
    if(iceb_vibrek(0,"Sklad",&data->rk->sklad,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno());
    break;

  case 1:
    if(iceb_vibrek(0,"Grup",&data->rk->grupa,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPA]),data->rk->grupa.ravno());
    break;

  case 2:
    if(iceb_vibrek(0,"Plansh",&data->rk->shet,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno());
    break;

  case 3:
    if(l_mater(1,&kod,&naim,0,0,data->window) == 0)
     {
      data->rk->kodmat.z_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODMAT]),data->rk->kodmat.ravno());
     }
    break;
 
 }
 


}
