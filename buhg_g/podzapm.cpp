/*$Id: podzapm.c,v 1.21 2013/09/26 09:46:52 sasa Exp $*/
/*11.11.2008	20.09.2004	Белых А.И.	podzapm.c
Подтверждение записи в материальном учёте.
*/
#include "buhg_g.h"

enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_DATAP,
  E_KOLIH,
  KOLENTER  
 };

class podzapm_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  iceb_u_str datap;
  iceb_u_str kolih;
  
  //полученные реквизиты
  short dd,md,gd; //Дата документа
  int tipz;  //1-приход 2-расход
  int skl;  //Склад
  iceb_u_str nomdok; //Номер документа
  int kodmat;
  int n_kart;
  double kolih_dok;
  double cena_dok; 

  podzapm_data() //Конструктор
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
   }
 };

gboolean   podzapm_v_key_press(GtkWidget *widget,GdkEventKey *event,class podzapm_data *data);
void    podzapm_v_vvod(GtkWidget *widget,class podzapm_data *data);
void  podzapm_v_knopka(GtkWidget *widget,class podzapm_data *data);

int podzapm_zapis(class podzapm_data *data);

extern SQL_baza bd;

int podzapm(short dd,short md,short gd, //Дата документа
int tipz,  //1-приход 2-расход
int skl,  //Склад
const char *nomdok, //Номер документа
int kodmat,
int n_kart,
double kolih_dok,
double cena_dok,
GtkWidget *wpredok)
{
podzapm_data data;

char strsql[512];
double kol=readkolkw(skl,n_kart,dd,md,gd,nomdok,wpredok);  
data.kolih.plus(kolih_dok-kol);

if(kol == 0.)
 {
  sprintf(strsql,"%d.%d.%d",dd,md,gd);
  data.datap.plus(strsql);
 }
else
  data.datap.plus("");
data.dd=dd;
data.md=md;
data.gd=gd;
data.tipz=tipz;
data.skl=skl;
data.nomdok.plus(nomdok);
data.kodmat=kodmat;
data.n_kart=n_kart;
data.kolih_dok=kolih_dok;
data.cena_dok=cena_dok;

 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Подтверждение записи"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(podzapm_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Подтверждение записи"));

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxradio = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxradio),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxradio);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

sprintf(strsql,"%s",gettext("Дата подтверждения"));
label=gtk_label_new(strsql);
data.entry[E_DATAP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAP]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAP]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAP]), data.entry[E_DATAP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAP], "activate",G_CALLBACK(podzapm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAP]),data.datap.ravno());
gtk_widget_set_name(data.entry[E_DATAP],iceb_u_inttochar(E_DATAP));

label=gtk_label_new(gettext("Количество"));
data.entry[E_KOLIH] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOLIH]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOLIH], "activate",G_CALLBACK(podzapm_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno());
gtk_widget_set_name(data.entry[E_KOLIH],iceb_u_inttochar(E_KOLIH));



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введенной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(podzapm_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(podzapm_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(podzapm_v_knopka),&data);
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

gboolean   podzapm_v_key_press(GtkWidget *widget,GdkEventKey *event,class podzapm_data *data)
{

//printf("podzapm_v_key_press\n");
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
void  podzapm_v_knopka(GtkWidget *widget,class podzapm_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if( podzapm_zapis(data) != 0)
      return;
        
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

void    podzapm_v_vvod(GtkWidget *widget,class podzapm_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("podzapm_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAP:
    data->datap.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_KOLIH:
    data->kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**********************************/
/*Запись введенной информации*/
/**********************************/
int podzapm_zapis(class podzapm_data *data)
{
short d,m,g;
iceb_u_str repl;
if(iceb_u_rsdat(&d,&m,&g,data->datap.ravno(),0) != 0)
 {
  repl.new_plus(gettext("Не верно введена дата !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

if(iceb_pbpds(m,g,data->window) != 0)
  return(1);


//if(SRAV1(g,m,d,gd,md,dd) > 0)
if(iceb_u_sravmydat(d,m,g,data->dd,data->md,data->gd) < 0)
 {
  repl.new_plus(gettext("Дата меньше чем дата документа ! Подтверждение невозможно !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

double kol=readkolkw(data->skl,data->n_kart,data->dd,data->md,data->gd,data->nomdok.ravno(),data->window);  

//if(kol1-kol2 - ATOFM(VV.VVOD_SPISOK_return_data(1)) < -0.00001)
if(data->kolih_dok-kol - data->kolih.ravno_atof() < -0.00001)
 {
  repl.new_plus(gettext("Не верно введено количество !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }
short ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);

class ostatok ost;

ostkarw(1,1,ggg,31,mmm,ggg,data->skl,data->n_kart,&ost);


if(data->tipz == 2 && data->kolih.ravno_atof() - ost.ostg[3] > 0.000000001)
 {
  repl.new_plus(gettext("В карточке нет такого количества на остатке !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

/*Проверяем дату первого прихода, расход не может быть раньше прихода*/


if(data->tipz == 2)
 {
  short dp,mp,gp;
  
  prdppw(data->skl,data->n_kart,&dp,&mp,&gp,data->window);
  
//  if(SRAV1(g,m,d,gp,mp,dp) > 0)
  if(iceb_u_sravmydat(d,m,g,dp,mp,gp) < 0)
   {
    char strsql[512];
    sprintf(strsql,gettext("Дата расхода (%d.%d.%dг.) меньше чем дата первого прихода (%d.%d.%dг."),
    d,m,g,dp,mp,gp);
    repl.new_plus(strsql);
    
    repl.ps_plus(gettext("Такой расход не может быть !"));
    iceb_menu_soob(&repl,data->window);
    return(1);
   }
 }

zapvkrw(d,m,g,data->nomdok.ravno(),data->skl,data->n_kart,data->dd,data->md,data->gd,data->tipz,
data->kolih.ravno_atof(),data->cena_dok,data->window);

return(0);

}
