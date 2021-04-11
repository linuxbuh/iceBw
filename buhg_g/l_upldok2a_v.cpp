/*$Id: l_upldok2a_v.c,v 1.3 2013/08/25 08:26:41 sasa Exp $*/
/*01.03.2016	23.04.2012	Белых А.И.	l_upldok2a_v.c
Ввод и корректировка
*/
#include "buhg_g.h"

enum
 {
  E_SHET,
  E_KODTOP,
  E_ZATR,
  E_ZATRPN,
  E_KOMENT,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class l_upldok2a_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_naim_shet;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  class iceb_u_str shet;
  class iceb_u_str kodtop;
  class iceb_u_str zatr;
  class iceb_u_str zatrpn;
  class iceb_u_str koment;    

  class iceb_u_str sheti;
  class iceb_u_str kodtopi;
  class iceb_u_str data_dok;
  class iceb_u_str nom_dok;
  int kod_podr;
  int kod_avt;
      

  l_upldok2a_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_data();
    
   }

  void clear_data()
   {
    shet.new_plus("");
    kodtop.new_plus("");
    zatr.new_plus("");
    koment.new_plus("");
   }

  void read_rek()
   {
    shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    kodtop.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODTOP])));
    zatr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_ZATR])));
    zatrpn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_ZATRPN])));
    koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
   }

  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");

    gtk_label_set_text(GTK_LABEL(label_naim_shet),"");
    
   }
 };


gboolean   l_upldok2a_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_upldok2a_v_data *data);
void  l_upldok2a_v_knopka(GtkWidget *widget,class l_upldok2a_v_data *data);
void    l_upldok2a_v_vvod(GtkWidget *widget,class l_upldok2a_v_data *data);
int l_upldok2a_pk(class l_upldok2a_v_data *data);
int l_upldok2a_zap(class l_upldok2a_v_data *data);
void  l_upldok2a_v_e_knopka(GtkWidget *widget,class l_upldok2a_v_data *data);
void l_upldok2a_v_provpst(class l_upldok2a_v_data *data);


extern SQL_baza  bd;

int l_upldok2a_v(const char *data_dok,
const char *nom_dok,
int kod_podr,
class iceb_u_str *shet,
class iceb_u_str *kodtop,
GtkWidget *wpredok)
{
class iceb_u_str naim_ei("");
class iceb_u_str naim_shet("");
SQL_str row;
SQLCURSOR cur;

class l_upldok2a_v_data data;
char strsql[1024];
class iceb_u_str kikz;
int gor=0,ver=0;
data.name_window.plus(__FUNCTION__);
data.shet.new_plus(shet->ravno());
data.sheti.new_plus(shet->ravno());
data.kodtop.new_plus(kodtop->ravno());
data.kodtopi.new_plus(kodtop->ravno());
data.data_dok.new_plus(data_dok);
data.nom_dok.new_plus(nom_dok);
data.kod_podr=kod_podr;


sprintf(strsql,"select ka from Upldok where god=%d and kp=%d and nomd='%s'",data.data_dok.ravno_god(),data.kod_podr,data.nom_dok.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.kod_avt=atoi(row[0]);

if(data.shet.getdlinna() >  1)
 {
  sprintf(strsql,"select zt,zpn,koment,ktoz,vrem from Upldok2a where datd='%s' and kp=%d and nomd='%s' and kt='%s' and shet='%s'",
  data.data_dok.ravno_sqldata(),data.kod_podr,data.nom_dok.ravno(),data.kodtop.ravno(),data.shet.ravno());
  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись для корректировки !"),wpredok);
    return(1);
   }


  data.zatr.new_plus(row[0]);
  data.zatrpn.new_plus(row[1]);
  data.koment.new_plus(row[2]);
  kikz.plus(iceb_kikz(row[3],row[4],wpredok));
  

  sprintf(strsql,"select nais from Plansh where ns='%s'",data.shet.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_shet.new_plus(row[0]);
   

 }

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

if(data.shet.getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));
  label=gtk_label_new(gettext("Ввод новой записи"));
 }
else
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
  iceb_u_str repl;
  repl.plus(gettext("Корректировка записи"));
  repl.ps_plus(kikz.ravno());
  label=gtk_label_new(repl.ravno());
 }

gtk_window_set_title(GTK_WINDOW(data.window),strsql);
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_upldok2a_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
 }
//Закрыть окно если окно предок удалено
gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
 hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX( hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 0);
//gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);



data.knopka_enter[E_SHET]=gtk_button_new_with_label(gettext("Счёт"));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(l_upldok2a_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор нормы списания топлива"));

data.entry[E_SHET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET]),16);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], FALSE,FALSE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(l_upldok2a_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));


data.label_naim_shet=gtk_label_new(naim_shet.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.label_naim_shet, TRUE, TRUE, 0);



data.knopka_enter[E_KODTOP]=gtk_button_new_with_label(gettext("Код топлива"));
gtk_box_pack_start (GTK_BOX (hbox[E_KODTOP]), data.knopka_enter[E_KODTOP], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KODTOP],"clicked",G_CALLBACK(l_upldok2a_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODTOP],iceb_u_inttochar(E_KODTOP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODTOP],gettext("Выбор кода топлива"));

data.entry[E_KODTOP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODTOP]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KODTOP]), data.entry[E_KODTOP], FALSE,FALSE, 0);
g_signal_connect(data.entry[E_KODTOP], "activate",G_CALLBACK(l_upldok2a_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODTOP]),data.kodtop.ravno());
gtk_widget_set_name(data.entry[E_KODTOP],iceb_u_inttochar(E_KODTOP));


label=gtk_label_new(gettext("Затраты по факту"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZATR]), label, FALSE, FALSE, 0);

data.entry[E_ZATR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_ZATR]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_ZATR]), data.entry[E_ZATR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ZATR], "activate",G_CALLBACK(l_upldok2a_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZATR]),data.zatr.ravno());
gtk_widget_set_name(data.entry[E_ZATR],iceb_u_inttochar(E_ZATR));

label=gtk_label_new(gettext("Затраты по норме"));
gtk_box_pack_start (GTK_BOX (hbox[E_ZATRPN]), label, FALSE, FALSE, 0);

data.entry[E_ZATRPN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_ZATRPN]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_ZATRPN]), data.entry[E_ZATRPN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ZATRPN], "activate",G_CALLBACK(l_upldok2a_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ZATRPN]),data.zatrpn.ravno());
gtk_widget_set_name(data.entry[E_ZATRPN],iceb_u_inttochar(E_ZATRPN));


label=gtk_label_new(gettext("Коментарий"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),255);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(l_upldok2a_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_upldok2a_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введённой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(l_upldok2a_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_upldok2a_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();
if(data.voz == 0)
 {
  shet->new_plus(data.shet.ravno());
  kodtop->new_plus(data.kodtop.ravno());
 }
if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_upldok2a_v_e_knopka(GtkWidget *widget,class l_upldok2a_v_data *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_SHET:

     if(iceb_l_plansh(1,&kod,&naim,data->window) == 0)
      {
       data->shet.new_plus(kod.ravno());

       gtk_label_set_text(GTK_LABEL(data->label_naim_shet),naim.ravno(20));
       
      }
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->shet.ravno());
      
    return;  

  case E_KODTOP:

     if(l_uplmt(1,&kod,&naim,data->window) == 0)
       data->kodtop.new_plus(kod.ravno());

     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODTOP]),data->kodtop.ravno());
      
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_upldok2a_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_upldok2a_v_data *data)
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
  //  printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_upldok2a_v_knopka(GtkWidget *widget,class l_upldok2a_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek();
    if(l_upldok2a_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_upldok2a_v_vvod(GtkWidget *widget,class l_upldok2a_v_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_SHET:
    data->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODTOP:
    data->kodtop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOMENT:
    data->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    break;

 }

enter+=1;
if(enter >= KOLENTER)
enter=0;
gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись                    */
/****************************/
int l_upldok2a_zap(class l_upldok2a_v_data *data)
{
char strsql[1024];


if(data->shet.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён счёт!"),data->window);
  return(1);
 }


if(data->kodtop.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён код топлива!"),data->window);
  return(1);
 }

if(data->zatr.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введены затраты!"),data->window);
  return(1);
 }

struct OPSHET reksh;
if(iceb_prsh1(data->shet.ravno(),&reksh,data->window) != 0)
 return(1);
sprintf(strsql,"select kodt from Uplmt where kodt='%s'",data->kodtop.ravno_filtr());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s!",gettext("Не найден код топлива!"),data->kodtop.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }


if(iceb_u_SRAV(data->shet.ravno(),data->sheti.ravno(),0) != 0 || iceb_u_SRAV(data->kodtop.ravno(),data->kodtopi.ravno(),0) != 0)
  if(l_upldok2a_pk(data) != 0)
     return(1);


if(data->sheti.getdlinna() <= 1)
 {
  sprintf(strsql,"insert into Upldok2a values('%s',%d,'%s','%s','%s',%.3f,%.3f,'%s',%d,%ld)",
  data->data_dok.ravno_sqldata(),
  data->kod_podr,
  data->nom_dok.ravno(),
  data->kodtop.ravno_filtr(),
  data->shet.ravno_filtr(),
  data->zatr.ravno_atof(),
  data->zatrpn.ravno_atof(),
  data->koment.ravno_filtr(),  
  iceb_getuid(data->window),time(NULL));
 }
else
 {
  
  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"update Upldok2a set \
shet='%s',\
kt='%s',\
zt=%.3f,\
zpn=%.3f,\
koment='%s',\
ktoz=%d,\
vrem=%ld \
where datd='%s' and kp=%d and nomd='%s' and kt='%s' and shet='%s'",
  data->shet.ravno_filtr(),
  data->kodtop.ravno_filtr(),
  data->zatr.ravno_atof(),
  data->zatrpn.ravno_atof(),
  data->koment.ravno_filtr(),
  iceb_getuid(data->window),time(NULL),
  data->data_dok.ravno_sqldata(),
  data->kod_podr,
  data->nom_dok.ravno(),
  data->kodtopi.ravno_filtr(),
  data->sheti.ravno_filtr());
    
 }

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);


return(0);

}
/*******************************/
/*Проверка на уже введенную запись*/
/**********************************/
int l_upldok2a_pk(class l_upldok2a_v_data *data) 
{
char strsql[512];

sprintf(strsql,"select shet from Upldok2a where datd='%s' and kp=%d and nomd='%s' and kt='%s' and shet='%s'",
data->data_dok.ravno_sqldata(),data->kod_podr,data->nom_dok.ravno(),data->kodtop.ravno(),data->shet.ravno());

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_menu_soob(gettext("Такая запись уже есть !"),data->window);
  return(1);
 }

return(0);

}
