/*$Id: buhpsdw_m.c,v 1.13 2013/09/26 09:46:46 sasa Exp $*/
/*11.09.2008	11.09.2008	Белых А.И.	buhpsdw_m.c
Проверка согласованности данных в подсистемах
*/
#include "buhg_g.h"
#include "buhpsdw.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAN,
  E_DATAK,
  E_SHET,
  E_PODSYS,
  KOLENTER  
 };

class buhpsdw_data
 {
  public:
  buhpsdw_rr  *rek_r;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  buhpsdw_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rek_r->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN])));
    rek_r->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAK])));
    rek_r->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    rek_r->podsys.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PODSYS])));
   }

  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    rek_r->clear_data();
   }
 };

gboolean   buhpsdw_v_key_press(GtkWidget *widget,GdkEventKey *event,class buhpsdw_data *data);
void    buhpsdw_v_vvod(GtkWidget *widget,class buhpsdw_data *data);
void  buhpsdw_v_knopka(GtkWidget *widget,class buhpsdw_data *data);
void buhpsdw_r(class buhpsdw_rr *datap);
void  buhpsdw_v_e_knopka(GtkWidget *widget,class buhpsdw_data *data);

int buhpsdw_vp(class iceb_u_str *podsys,GtkWidget *wpredok);


int buhpsdw_m(class buhpsdw_rr *rek)
{

char strsql[512];

if(rek->datan.getdlinna() == 0)
 {
  rek->clear_data();
  time_t vrem;
  time(&vrem);
  struct tm *bf;
  bf=localtime(&vrem);

  sprintf(strsql,"%d.%d.%d",
  1,1,bf->tm_year+1900);
  rek->datan.new_plus(strsql);

  sprintf(strsql,"%d.%d.%d",
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
  rek->datak.new_plus(strsql);
 }


buhpsdw_data data;

data.rek_r=rek;

 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Проверка согласованности данных в подсистемах"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(buhpsdw_v_key_press),&data);

GtkWidget *label=gtk_label_new(gettext("Проверка согласованности данных в подсистемах"));

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



sprintf(strsql,"%s",gettext("Дата начала"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(buhpsdw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты начала отчёта"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(buhpsdw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rek_r->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));

sprintf(strsql,"%s",gettext("Дата конца"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(buhpsdw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты конца отчёта"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(buhpsdw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rek_r->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));

sprintf(strsql,"%s (,,)",gettext("Счет"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(buhpsdw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(buhpsdw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rek_r->shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

sprintf(strsql,"%s",gettext("Подсистема"));
data.knopka_enter[E_PODSYS]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODSYS]), data.knopka_enter[E_PODSYS], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_PODSYS],"clicked",G_CALLBACK(buhpsdw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODSYS],iceb_u_inttochar(E_PODSYS));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PODSYS],gettext("Выбор группы контрагента"));

data.entry[E_PODSYS] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_PODSYS]), data.entry[E_PODSYS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PODSYS], "activate",G_CALLBACK(buhpsdw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODSYS]),data.rek_r->podsys.ravno());
gtk_widget_set_name(data.entry[E_PODSYS],iceb_u_inttochar(E_PODSYS));




sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(buhpsdw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(buhpsdw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(buhpsdw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  buhpsdw_v_e_knopka(GtkWidget *widget,class buhpsdw_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rek_r->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rek_r->datan.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rek_r->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rek_r->datak.ravno());
      
    return;  
   
  case E_SHET:

    iceb_vibrek(0,"Plansh",&data->rek_r->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rek_r->shet.ravno());

    return;  

  case E_PODSYS:

    buhpsdw_vp(&data->rek_r->podsys,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODSYS]),data->rek_r->podsys.ravno());
    return;
 }
}


/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   buhpsdw_v_key_press(GtkWidget *widget,GdkEventKey *event,class buhpsdw_data *data)
{

//printf("buhpsdw_v_key_press\n");
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
void  buhpsdw_v_knopka(GtkWidget *widget,class buhpsdw_data *data)
{
short dn,mn,gn;
short dk,mk,gk;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(iceb_rsdatp(&dn,&mn,&gn,data->rek_r->datan.ravno(),&dk,&mk,&gk,data->rek_r->datak.ravno(),data->window) != 0)
      return;

    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    buhpsdw_v_vvod(GtkWidget *widget,class buhpsdw_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//g_print("buhpsdw_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAN:
    data->rek_r->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAK:
    data->rek_r->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET:
    data->rek_r->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PODSYS:
    data->rek_r->podsys.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**********************************/
/*Выбор подсистем*/
/***********************************/
int buhpsdw_vp(class iceb_u_str *podsys,GtkWidget *wpredok)
{

class iceb_u_str titl;
class iceb_u_str zagolovok;
class iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(iceb_get_pnk("00",0,wpredok),40);
zagolovok.ps_plus(gettext("Выберите нужное"));

char bros[500];

sprintf(bros,"%s %s",ICEB_MP_MATU,gettext("Материальный учёт"));
punkt_m.plus(bros);//0


sprintf(bros,"%s %s",ICEB_MP_UOS,gettext("Учёт основных средств"));
punkt_m.plus(bros);//1

int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,wpredok);
switch(nomer)
 {
  case 0:
   podsys->new_plus(ICEB_MP_MATU);
   return(0);  
  case 1:
   podsys->new_plus(ICEB_MP_UOS);
   return(0);  
 }
return(1);
}
