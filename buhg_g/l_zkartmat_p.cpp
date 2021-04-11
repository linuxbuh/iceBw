/*$Id: l_zkartmat_p.c,v 1.20 2013/09/26 09:46:51 sasa Exp $*/
/*03.03.2016	02.06.2004	Белых А.И.	l_zkartmat_p.c
Меню ввода реквизитов поиска
*/
#include "buhg_g.h"
#include "l_zkartmat.h"

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
  E_DATAN,
  E_DATAK,
  E_KONTR,
  E_KODOP,
  E_NOMDOK,
  E_PR,
  KOLENTER  
 };

class zkartmat_p_data
 {
  public:
  zkartmat_poi  *rpoi;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *opt;
  class iceb_u_str name_window;
  short kl_shift;
  
  
  zkartmat_p_data() //Конструктор
   {
    kl_shift=0;
  
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      
      if(i == E_PR)
        continue;
      
      g_signal_emit_by_name(entry[i],"activate");
     }
    rpoi->pri_ras=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));

   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      if(i == E_PR)
       {
        rpoi->pri_ras=0;
        continue;
       }
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
     }
   }
 };

gboolean   zkartmat_p_key_press(GtkWidget *widget,GdkEventKey *event,class zkartmat_p_data *data);
void    zkartmat_p_vvod(GtkWidget *widget,class zkartmat_p_data *data);
void  zkartmat_p_knopka(GtkWidget *widget,class zkartmat_p_data *data);
void   zkartmat_p_rekviz(class zkartmat_p_data *data);

void            zkartmat_get_pm0(GtkWidget*,int*);
void            zkartmat_get_pm1(GtkWidget*,int*);
void            zkartmat_get_pm2(GtkWidget*,int*);

extern SQL_baza bd;


int l_zkartmat_p(class zkartmat_poi *datap,GtkWidget *wpredok)
{
class zkartmat_p_data data;
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

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zkartmat_p_key_press),&data);

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

sprintf(strsql,"%s (,,)",gettext("Номер документа"));
label=gtk_label_new(strsql);
data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),100);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(zkartmat_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rpoi->nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));

sprintf(strsql,"%s",gettext("Дата начала"));
label=gtk_label_new(strsql);
data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], FALSE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(zkartmat_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rpoi->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));

sprintf(strsql,"%s",gettext("Дата конца"));
label=gtk_label_new(strsql);
data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], FALSE, TRUE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(zkartmat_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rpoi->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));

sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
label=gtk_label_new(strsql);
data.entry[E_KONTR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KONTR]),100);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(zkartmat_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rpoi->kontr.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));

sprintf(strsql,"%s (,,)",gettext("Код операции"));
label=gtk_label_new(strsql);
data.entry[E_KODOP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODOP]),100);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODOP], "activate",G_CALLBACK(zkartmat_p_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rpoi->kodop.ravno());
gtk_widget_set_name(data.entry[E_KODOP],iceb_u_inttochar(E_KODOP));



label=gtk_label_new(gettext("Приход/расход"));
gtk_box_pack_start (GTK_BOX (hbox[E_PR]), label, FALSE, FALSE, 0);
/*************************
data.opt = gtk_option_menu_new();
GtkWidget *menu = gtk_menu_new();

GtkWidget *item;
item = gtk_menu_item_new_with_label (gettext("Все записи"));
g_signal_connect(item, "activate",G_CALLBACK(zkartmat_get_pm0),&data.rpoi->pri_ras);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("Только приходы"));
g_signal_connect(item, "activate",G_CALLBACK(zkartmat_get_pm1),&data.rpoi->pri_ras);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);

item = gtk_menu_item_new_with_label (gettext("Только расходы"));
g_signal_connect(item, "activate",G_CALLBACK(zkartmat_get_pm2),&data.rpoi->pri_ras);
gtk_widget_show (item);
gtk_menu_append (GTK_MENU (menu), item);
gtk_option_menu_set_menu (GTK_OPTION_MENU (data.opt), menu);
gtk_option_menu_set_history(GTK_OPTION_MENU(data.opt),data.rpoi->pri_ras);
******************************/

iceb_pm_pr(&data.opt,data.rpoi->pri_ras);



gtk_box_pack_start (GTK_BOX (hbox[E_PR]), data.opt, FALSE, FALSE, 0);




sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(zkartmat_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Просмотр нужных реквизитов"));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(zkartmat_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(zkartmat_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(zkartmat_p_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(0);
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zkartmat_p_key_press(GtkWidget *widget,GdkEventKey *event,class zkartmat_p_data *data)
{

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
void  zkartmat_p_knopka(GtkWidget *widget,class zkartmat_p_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek(); //Читаем реквизиты меню

    if(iceb_rsdatp_str(&data->rpoi->datan,&data->rpoi->datak,data->window) != 0)
      return;
    
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->rpoi->metka_poi=1;
    return;  

  case FK3:
    zkartmat_p_rekviz(data);
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

void    zkartmat_p_vvod(GtkWidget *widget,class zkartmat_p_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
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

/************
  case E_PR:
    data->rpoi->pri_ras.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
*****************/

  case E_NOMDOK:
    data->rpoi->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;

if(enter == E_PR)
  enter+=1;
  
if(enter >= KOLENTER)
enter=0;


gtk_widget_grab_focus(data->entry[enter]);
 
}
/********************/
/*Работа с реквизитами*/
/***********************/
void   zkartmat_p_rekviz(class zkartmat_p_data *data)
{
short nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Реквизиты"));

zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Просмотр списка контрагентов"));
punkt_m.plus(gettext("Просмотр списка операций приходов"));
punkt_m.plus(gettext("Просмотр списка операций расходов"));

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);



iceb_u_str kod;
iceb_u_str naikod;

switch (nomer)
 {
  case -1:
    return;
    
  case 0:
    iceb_vibrek(0,"Kontragent",&data->rpoi->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rpoi->kontr.ravno());
    break;

  case 1:
    iceb_vibrek(0,"Prihod",&data->rpoi->kodop,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rpoi->kodop.ravno());
    break;

  case 2:
    iceb_vibrek(0,"Rashod",&data->rpoi->kodop,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rpoi->kodop.ravno());
    break;

 }

}
/**********************/
/*Читаем              */
/**********************/

void   zkartmat_get_pm0(GtkWidget *widget,int *data)
{
*data=0;
printf("zkartmat_get_pm0-%d\n",*data);
}
void   zkartmat_get_pm1(GtkWidget *widget,int *data)
{
*data=1;
printf("zkartmat_get_pm1-%d\n",*data);
}
void   zkartmat_get_pm2(GtkWidget *widget,int *data)
{
*data=2;
printf("zkartmat_get_pm2-%d\n",*data);
}
