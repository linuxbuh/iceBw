/*$Id: vvko_kontr.c,v 1.24 2013/10/04 07:27:17 sasa Exp $*/
/*13.07.2015	13.02.2006	Белых А.И.	vvko_kontr.c
Ввод в кассовый ордер контрагента
*/
#include "buhg_g.h"
enum
{
  FK2,
  FK10,
  KOL_F_KL
};

enum
 {
  E_KONTR,
  E_SUMA,
  E_KOMENT,
  KOLENTER  
 };

class vvko_kontr_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *label_naim_kontr;
  GtkWidget *label_suma_den;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  //Ключевые реквизиты докумета
  iceb_u_str datad_k;
  iceb_u_str nomdok_k;
  iceb_u_str kassa_k;
  int tipz; //не корректируемый реквизит  
  iceb_u_str kontr_k;
  class iceb_u_str shetd;    
  //Реквизиты записи
  iceb_u_str kontr;
  iceb_u_str suma;
  class iceb_u_str koment;
  
  double max_sum_den;
  vvko_kontr_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_data();
    max_sum_den=0.;
   }

  void read_rek()
   {
    koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOMENT])));
    kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR])));
    suma.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SUMA])));

   }
  void clear_data()
   {
    shetd.new_plus("");
    kontr.new_plus("");
    suma.new_plus("");
    koment.new_plus("");
   }
 };

gboolean   vvko_kontr_v_key_press(GtkWidget *widget,GdkEventKey *event,class vvko_kontr_data *data);
void    vvko_kontr_v_vvod(GtkWidget *widget,class vvko_kontr_data *data);
void  vvko_kontr_v_knopka(GtkWidget *widget,class vvko_kontr_data *data);
void vvko_kontr_kkvt(char *skod,char *nkod,GtkWidget *wpredok);
int vvko_kontr_pk(char *kod,GtkWidget *wpredok);

void  vvko_kontr_v_e_knopka(GtkWidget *widget,class vvko_kontr_data *data);
int vvko_kontr_zap(class vvko_kontr_data *data);
double vvko_kontr_suma_den(const char *datdok,const char *kodkontr,GtkWidget *wpredok);

extern SQL_baza bd;


int vvko_kontr(const char *datad, //дата документа
const char *nomdok,
const char *kassa,
int tipz,
iceb_u_str *kontr, //приходит корректируемый - уходит записанный
const char *shetd,
GtkWidget *wpredok)
{
class vvko_kontr_data data;
char strsql[1024];
class iceb_u_str kikz;
class iceb_u_str naim_kontr("");
double suma_den=0.;
class iceb_u_str str_suma_den("");

class iceb_u_str kod_val("");
class iceb_u_str data_ord(iceb_u_tosqldat(datad));
class iceb_u_str bros("");

iceb_poldan("Максимальная сумма за день от контрагента",&bros,"kasnast.alx",wpredok);

class iceb_getkue_dat getval(kod_val.ravno(),wpredok);

data.max_sum_den=getval.toue(bros.ravno(),data_ord.ravno());


data.datad_k.plus(datad);
data.nomdok_k.plus(nomdok);
data.kassa_k.plus(kassa);
data.tipz=tipz; //меню для работы только с приходными документами
data.kontr_k.new_plus(kontr->ravno());
data.shetd.new_plus(shetd);

if(data.kontr_k.getdlinna() > 1) //корректировка шапки документа
 {
  SQL_str row;
  SQLCURSOR cur;
  sprintf(strsql,"select suma,ktoi,vrem,koment from Kasord1 where nomd='%s' and kassa=%d and \
tp=%d and god=%d and kontr='%s'",
  data.nomdok_k.ravno(),data.kassa_k.ravno_atoi(),data.tipz,data.datad_k.ravno_god(),
  data.kontr_k.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);
    return(1);
   }

  data.suma.new_plus(row[0]);
  kikz.plus(iceb_kikz(row[1],row[2],wpredok));
  data.koment.new_plus(row[3]);
  //Узнаём наименование контрагента
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data.kontr_k.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_kontr.new_plus(row[0]);

  if(tipz == 1)
   {
    suma_den=vvko_kontr_suma_den(datad,data.kontr_k.ravno(),wpredok);
    if(suma_den != 0.)
     {
      str_suma_den.new_plus(gettext("Сумма уже полученная от контрагента"));
      str_suma_den.plus(":");
      str_suma_den.plus(suma_den);          
     }
   }
}

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.kontr_k.getdlinna() > 1)
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка записи"));
else
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод новой записи"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vvko_kontr_v_key_press),&data);

iceb_u_str zagolov;
if(data.kontr_k.getdlinna() <= 1)
 zagolov.new_plus(gettext("Ввод новой записи"));
else
 {
  zagolov.new_plus(gettext("Корректировка записи"));
  zagolov.ps_plus(kikz.ravno());
 }

GtkWidget *label=gtk_label_new(zagolov.ravno());

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

data.label_suma_den=gtk_label_new(str_suma_den.ravno());
gtk_container_add (GTK_CONTAINER (vbox), data.label_suma_den);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
gtk_container_add (GTK_CONTAINER (vbox), separator1);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);



sprintf(strsql,"%s",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KONTR],"clicked",G_CALLBACK(vvko_kontr_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KONTR],iceb_u_inttochar(E_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KONTR],gettext("Выбор контрагента"));

data.entry[E_KONTR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KONTR]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(vvko_kontr_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.kontr_k.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));

data.label_naim_kontr=gtk_label_new(naim_kontr.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.label_naim_kontr,TRUE, TRUE, 0);

sprintf(strsql,"%s",gettext("Сумма"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), label, FALSE, FALSE, 0);

data.entry[E_SUMA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMA]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA]), data.entry[E_SUMA],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMA], "activate",G_CALLBACK(vvko_kontr_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA]),data.suma.ravno());
gtk_widget_set_name(data.entry[E_SUMA],iceb_u_inttochar(E_SUMA));

sprintf(strsql,"%s",gettext("Коментарий"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);

data.entry[E_KOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOMENT]),99);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT],TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(vvko_kontr_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(vvko_kontr_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(vvko_kontr_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();
if(data.voz == 0)
 kontr->new_plus(data.kontr.ravno());
return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vvko_kontr_v_e_knopka(GtkWidget *widget,class vvko_kontr_data *data)
{
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_KONTR:

    if(iceb_poi_kontr(&data->kontr,&naim,data->window) != 0)
       return;
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->kontr.ravno());
    gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),naim.ravno());

    if(data->tipz == 1)
     {
      class iceb_u_str str_suma_den("");
      double suma_den=vvko_kontr_suma_den(data->datad_k.ravno(),data->kontr_k.ravno(),data->window);
      if(suma_den != 0.)
       {
        str_suma_den.new_plus(gettext("Сумма уже полученная от контрагента"));
        str_suma_den.plus(":");
        str_suma_den.plus(suma_den);          
        gtk_label_set_text(GTK_LABEL(data->label_suma_den),str_suma_den.ravno());
       }
     }
    
    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vvko_kontr_v_key_press(GtkWidget *widget,GdkEventKey *event,class vvko_kontr_data *data)
{

//printf("vvko_kontr_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
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
void  vvko_kontr_v_knopka(GtkWidget *widget,class vvko_kontr_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(vvko_kontr_zap(data) != 0)
      return;

    gtk_widget_destroy(data->window);
    data->window=NULL;

    data->voz=0;
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

void    vvko_kontr_v_vvod(GtkWidget *widget,class vvko_kontr_data *data)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
int enter=atoi(gtk_widget_get_name(widget));
//g_print("vvko_kontr_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_SUMA:
    data->suma.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KONTR:

    data->kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->kontr.ravno());


    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      sprintf(strsql,"%.*s",iceb_u_kolbait(20,row[0]),row[0]);
      gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),strsql);
     }    
    else
      gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),"");

    if(data->tipz == 1)
     {

      class iceb_u_str str_suma_den("");
      double suma_den=vvko_kontr_suma_den(data->datad_k.ravno(),data->kontr.ravno(),data->window);
      if(suma_den != 0.)
       {
        str_suma_den.new_plus(gettext("Сумма уже полученная от контрагента"));
        str_suma_den.plus(":");
        str_suma_den.plus(suma_den);          
        gtk_label_set_text(GTK_LABEL(data->label_suma_den),str_suma_den.ravno());
       }
     }

    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 

}
/*********/
/*Запись*/
/*********/
int vvko_kontr_zap(class vvko_kontr_data *data)
{
if(data->kontr.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён код контрагента !"),data->window);
  return(1);
 }
if(data->suma.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Не введена сумма !"),data->window);
  return(1);
 }

char strsql[1024];

if(data->tipz == 1)
 {
  double suma_den=vvko_kontr_suma_den(data->datad_k.ravno(),data->kontr.ravno(),data->window);
  if(suma_den != 0. && data->max_sum_den != 0.)
   {
    if(suma_den+data->suma.ravno_atof() > data->max_sum_den)
     {
      sprintf(strsql,"%s %.2f %s %.2f",gettext("Сумма полученная от контрагента за день"),suma_den+data->suma.ravno_atof(),gettext("превышает максимально допустимую"),data->max_sum_den);
      iceb_menu_soob(strsql,data->window);
     }
   }
 }


//Проверяем есть ли в списке контрагентов
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data->kontr.ravno());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  iceb_menu_soob(gettext("Не найден код контрагента в общем списке !"),data->window);
  return(1);
 }


if(iceb_u_SRAV(data->kontr_k.ravno(),data->kontr.ravno(),0) != 0)
 {
   /*Проверяем может он уже введён в ордер*/
   sprintf(strsql,"select kontr from Kasord1 where kassa=%d and \
god=%d and tp=%d and nomd='%s' and kontr='%s'",
   data->kassa_k.ravno_atoi(),data->datad_k.ravno_god(),data->tipz,data->nomdok_k.ravno(),data->kontr.ravno());
   if(iceb_sql_readkey(strsql,data->window) >= 1)
    {
     sprintf(strsql,gettext("Контрагент %s уже введён !"),data->kontr.ravno());
     iceb_menu_soob(strsql,data->window);
     return(1);
    }
 }
 
int kodv=0;

short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,data->datad_k.ravno(),1);
if((kodv=zapzkorw(data->kassa_k.ravno(),data->tipz,dd,md,gd,data->nomdok_k.ravno(),data->kontr.ravno(),
 data->suma.ravno_atof(),data->kontr_k.ravno(),data->koment.ravno_filtr(),data->window)) != 0)
  {
   if(kodv == 1)
    {
     sprintf(strsql,gettext("Контрагент %s уже введён !"),data->kontr.ravno());
     iceb_menu_soob(strsql,data->window);
    }
   return(1);
 
  }

struct OPSHET   shetv;
iceb_prsh(data->shetd.ravno(),&shetv,data->window);
if(shetv.saldo == 3)
 {
  sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",data->shetd.ravno(),data->kontr.ravno());
  if(iceb_sql_readkey(strsql,data->window) <= 0)
   {
    sprintf(strsql,"insert into Skontr (ns,kodkon,ktoi,vrem) values ('%s','%s',%d,%ld)",
    data->shetd.ravno(),
    data->kontr.ravno(),
    iceb_getuid(data->window),time(NULL));
    if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
       return(1);
   }
 }

return(0);

}
/********************************/
/*Получение суммы полученной от контрагента за день*/
/***********************************************/
double vvko_kontr_suma_den(const char *datdok,const char *kodkontr,GtkWidget *wpredok)
{
char strsql[512];
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select suma from Kasord1 where datp='%s' and kontr='%s' and tp=1",iceb_u_tosqldat(datdok),kodkontr);
//printf("%s-%s\n",__FUNCTION__,strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(0.);
 }
double suma=0.;
while(cur.read_cursor(&row) != 0)
  suma+=atof(row[0]);

return(suma);
}
