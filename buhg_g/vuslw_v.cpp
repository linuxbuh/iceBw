/*$Id: vuslw_v.c,v 1.23 2013/09/26 09:46:59 sasa Exp $*/
/*06.10.2018	19.10.2005	Белых А.И.	vuslw_v.c
Меню ввода и корректировки услуги/материалла
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
  E_KOLIH,
  E_CENA_SNDS,
  E_CENA_BNDS,
  E_EI,
  E_SHETU,
  E_SUMA_BNDS,
  E_SUMA_SNDS,
  E_SHET_PS,
  KOLENTER  
 };

class vuslw_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  //Реквизиты документа в который вводим запись
  short dd,md,gd;
  int tipz;
  int podr;
  class iceb_u_str nomdok;
  iceb_u_str kodzap;
  int metka_zap;
  int nom_zap;
  float pnds;

  //Реквизиты введённые в меню
  class iceb_u_str kolih;
  class iceb_u_str cena_snds;
  class iceb_u_str cena_bnds;
  class iceb_u_str ei;
  class iceb_u_str shetu;
  class iceb_u_str suma_bnds;
  class iceb_u_str suma_snds;
  class iceb_u_str shet_ps;
  unsigned int ktoz;
  
  vuslw_v_data() //Конструктор
   {
    ktoz=0;
    kl_shift=0;
    voz=1;
    kolih.plus("");
    cena_snds.plus("");
    cena_bnds.plus("");
    ei.plus("");
    shetu.plus("");
    suma_bnds.plus("");
    suma_snds.plus("");
    shet_ps.plus("");
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      if(i == E_CENA_SNDS)
       continue;
      if(i == E_SUMA_SNDS)
       continue;
      if(i == E_SUMA_BNDS)
       continue;
      g_signal_emit_by_name(entry[i],"activate");
     }
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    void read_rek();
   }
 };

void  vuslw_v_e_knopka(GtkWidget *widget,class vuslw_v_data *data);
void    vuslw_v_vvod(GtkWidget *widget,class vuslw_v_data *data);
gboolean   vuslw_v_key_press(GtkWidget *widget,GdkEventKey *event,class vuslw_v_data *data);
void  vuslw_v_knopka(GtkWidget *widget,class vuslw_v_data *data);

int vuslw_v_zap(class vuslw_v_data *data);

extern SQL_baza	bd;
extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление1 */


int vuslw_v(short dd,short md,short gd, //Дата документа
int tipz,
int podr,
const char *nomdok,
const char *kod,
int metka, //0-материал 1-услуга
int nom_zap, //Номер записи в документе. Если равно -1 то это новая запись.
float pnds, //Процент НДС на момент создания документа
GtkWidget *wpredok)
{
char strsql[512];
class vuslw_v_data data;

data.dd=dd;
data.md=md;
data.gd=gd;
data.tipz=tipz;
data.podr=podr;
data.nomdok.new_plus(nomdok);
data.kodzap.new_plus(kod);
data.metka_zap=metka;
data.nom_zap=nom_zap;
data.pnds=pnds;
SQL_str row;
SQLCURSOR cur;


//читаем код записи и узнаем нужные реквизиты
double  ndscen=0.;
if(metka == 1)
 {
  sprintf(strsql,"select cena,ei,naius,nds,shetu from Uslugi \
  where kodus=%s",kod);

  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код услуги"),kod);
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }
   data.shetu.new_plus(row[4]);
 }

if(metka == 0)
 {
  sprintf(strsql,"select cenapr,ei,naimat,nds from Material \
  where kodm=%s",kod);

  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),kod);
    iceb_menu_soob(strsql,wpredok);
    return(1);
   }
 }
 
double cenabn,cena;

cenabn=cena=atof(row[0]);
data.ei.new_plus(row[1]);
iceb_u_str naim_zap;
naim_zap.plus(row[2]);

ndscen=atof(row[3]);


if(cena != 0.  && ndscen == 0.)
 {
  cenabn=cena-cena*pnds/(100.+pnds);
  cenabn=iceb_u_okrug(cenabn,okrcn);
 }

if(data.nom_zap != -1) //Корректировать запись
 {
  //Читаем количество и цену в документе
  sprintf(strsql,"select kolih,cena,ei,shetu,shsp,ktoi from Usldokum1 where \
datd='%04d-%02d-%02d' and podr=%d and tp=%d and nomd='%s' and metka=%d \
and kodzap=%s and nz=%d",
  gd,md,dd,podr,tipz,nomdok,metka,kod,nom_zap);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_menu_soob(gettext("Не найдена запись в документе !"),wpredok);
    return(1);
   }
  data.kolih.new_plus(row[0]);

  cenabn=atof(row[1]);
  cenabn=iceb_u_okrug(cenabn,okrcn);

  data.ei.new_plus(row[2]);
  data.shetu.new_plus(row[3]);
  data.shet_ps.new_plus(row[4]);
  data.ktoz=atoi(row[5]);
 }


double bb=0.;

if(cenabn != 0.)
 {
  sprintf(strsql,"%.10g",cenabn);
  data.cena_bnds.new_plus(strsql);
  bb=cenabn+cenabn*pnds/100.;
  bb=iceb_u_okrug(bb,okrg1);
 }
if(bb != 0.)
 {
  sprintf(strsql,"%.10g",bb);
  data.cena_snds.new_plus(strsql);  
 }




data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(data.metka_zap == 0)
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод материалла"));
if(data.metka_zap == 1)
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод услуги"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vuslw_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

iceb_u_str nadpis;

if(data.metka_zap == 0)
 nadpis.plus(gettext("Ввод материалла"));
if(data.metka_zap == 1)
 nadpis.plus(gettext("Ввод услуги"));

nadpis.ps_plus(kod);
nadpis.plus(" ");
nadpis.plus(naim_zap.ravno());

GtkWidget *label=gtk_label_new(nadpis.ravno());

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
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


label=gtk_label_new(gettext("Количество"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE, 0);

data.entry[E_KOLIH] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOLIH]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOLIH], "activate",G_CALLBACK(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno());
gtk_widget_set_name(data.entry[E_KOLIH],iceb_u_inttochar(E_KOLIH));

label=gtk_label_new(gettext("Цена с НДС"));
gtk_box_pack_start (GTK_BOX (hbox[E_CENA_SNDS]), label, FALSE, FALSE, 0);

data.entry[E_CENA_SNDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_CENA_SNDS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA_SNDS]), data.entry[E_CENA_SNDS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_CENA_SNDS], "activate",G_CALLBACK(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA_SNDS]),data.cena_snds.ravno());
gtk_widget_set_name(data.entry[E_CENA_SNDS],iceb_u_inttochar(E_CENA_SNDS));

label=gtk_label_new(gettext("Цена без НДС"));
gtk_box_pack_start (GTK_BOX (hbox[E_CENA_BNDS]), label, FALSE, FALSE, 0);

data.entry[E_CENA_BNDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_CENA_BNDS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA_BNDS]), data.entry[E_CENA_BNDS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_CENA_BNDS], "activate",G_CALLBACK(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA_BNDS]),data.cena_bnds.ravno());
gtk_widget_set_name(data.entry[E_CENA_BNDS],iceb_u_inttochar(E_CENA_BNDS));

sprintf(strsql,"%s",gettext("Единица измерения"));
data.knopka_enter[E_EI]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.knopka_enter[E_EI], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_EI],"clicked",G_CALLBACK(vuslw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_EI],iceb_u_inttochar(E_EI));
gtk_widget_set_tooltip_text(data.knopka_enter[E_EI],gettext("Выбор единицы измерения."));

data.entry[E_EI] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_EI]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_EI], "activate",G_CALLBACK(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.ei.ravno());
gtk_widget_set_name(data.entry[E_EI],iceb_u_inttochar(E_EI));



label=gtk_label_new(gettext("Сумма без НДС"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_BNDS]), label, FALSE, FALSE, 0);

data.entry[E_SUMA_BNDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMA_BNDS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_BNDS]), data.entry[E_SUMA_BNDS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMA_BNDS], "activate",G_CALLBACK(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA_BNDS]),data.suma_bnds.ravno());
gtk_widget_set_name(data.entry[E_SUMA_BNDS],iceb_u_inttochar(E_SUMA_BNDS));

label=gtk_label_new(gettext("Сумма з НДС"));
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_SNDS]), label, FALSE, FALSE, 0);

data.entry[E_SUMA_SNDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMA_SNDS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMA_SNDS]), data.entry[E_SUMA_SNDS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMA_SNDS], "activate",G_CALLBACK(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMA_SNDS]),data.suma_snds.ravno());
gtk_widget_set_name(data.entry[E_SUMA_SNDS],iceb_u_inttochar(E_SUMA_SNDS));



sprintf(strsql,"%s",gettext("Счёт учёта"));
data.knopka_enter[E_SHETU]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.knopka_enter[E_SHETU], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHETU],"clicked",G_CALLBACK(vuslw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHETU],iceb_u_inttochar(E_SHETU));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHETU],gettext("Выбор счёта в плане счетов."));

data.entry[E_SHETU] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHETU]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.entry[E_SHETU], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHETU], "activate",G_CALLBACK(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETU]),data.shetu.ravno());
gtk_widget_set_name(data.entry[E_SHETU],iceb_u_inttochar(E_SHETU));

if(data.tipz == 2)
 sprintf(strsql,"%s",gettext("Счёт списания"));
if(data.tipz == 1)
 sprintf(strsql,"%s",gettext("Счёт приобретения"));

data.knopka_enter[E_SHET_PS]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_PS]), data.knopka_enter[E_SHET_PS], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET_PS],"clicked",G_CALLBACK(vuslw_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET_PS],iceb_u_inttochar(E_SHET_PS));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET_PS],gettext("Выбор счёта в плане счетов."));

data.entry[E_SHET_PS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET_PS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_PS]), data.entry[E_SHET_PS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET_PS], "activate",G_CALLBACK(vuslw_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_PS]),data.shet_ps.ravno());
gtk_widget_set_name(data.entry[E_SHET_PS],iceb_u_inttochar(E_SHET_PS));





sprintf(strsql,"F2 %s",gettext("Записать"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации."));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(vuslw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации."));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(vuslw_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне."));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(vuslw_v_knopka),&data);
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
void  vuslw_v_e_knopka(GtkWidget *widget,class vuslw_v_data *data)
{
struct OPSHET	shetv;
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
   
  case E_EI:

    iceb_vibrek(1,"Edizmer",&data->shetu,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->ei.ravno());
    return;
    
  case E_SHETU:

    iceb_vibrek(1,"Plansh",&data->shetu,data->window);
    if(iceb_prsh1(data->shetu.ravno(),&shetv,data->window) != 0)
      data->shetu.new_plus("");
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),data->shetu.ravno());

    return;  

  case E_SHET_PS:

    iceb_vibrek(1,"Plansh",&data->shet_ps,data->window);
    if(iceb_prsh1(data->shet_ps.ravno(),&shetv,data->window) != 0)
      data->shet_ps.new_plus("");
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_PS]),data->shet_ps.ravno());

    return;  


 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vuslw_v_key_press(GtkWidget *widget,GdkEventKey *event,class vuslw_v_data *data)
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
void  vuslw_v_knopka(GtkWidget *widget,class vuslw_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(vuslw_v_zap(data) != 0)
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

void    vuslw_v_vvod(GtkWidget *widget,class vuslw_v_data *data)
{
char strsql[512];
double suma_snds;
double suma_bnds;
double cena_snds;
double cena_bnds;
double kolih;

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_KOLIH:
    data->kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    if(data->kolih.ravno_atof() != 0.)
    if(data->cena_bnds.ravno_atof() != 0.)
     {
      //Сумма без НДС
      sprintf(strsql,"%.10g",data->kolih.ravno_atof()*data->cena_bnds.ravno_atof());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA_BNDS]),strsql);

      //Сумма с НДС      
      cena_snds=data->cena_bnds.ravno_atof()+data->cena_bnds.ravno_atof()*data->pnds/100.;
      cena_snds=iceb_u_okrug(cena_snds,okrg1);
      sprintf(strsql,"%.10g",data->kolih.ravno_atof()*cena_snds);

      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA_SNDS]),strsql);
     }
    break;

  case E_CENA_SNDS:
    data->cena_snds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(data->cena_snds.ravno_atof() > 0.)
     {
      cena_bnds=data->cena_snds.ravno_atof()-data->cena_snds.ravno_atof()*data->pnds/(100.+data->pnds);
      cena_bnds=iceb_u_okrug(cena_bnds,okrcn);
      sprintf(strsql,"%.10g",cena_bnds);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA_BNDS]),strsql);

      if(data->kolih.ravno_atof() != 0.)
       {
        //Сумма без НДС
        sprintf(strsql,"%.10g",data->kolih.ravno_atof()*cena_bnds);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA_BNDS]),strsql);

        //Сумма с НДС      
        cena_snds=cena_bnds+cena_bnds*data->pnds/100.;
        cena_snds=iceb_u_okrug(cena_snds,okrg1);
        sprintf(strsql,"%.10g",data->kolih.ravno_atof()*cena_snds);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA_SNDS]),strsql);
       }      
     }
    break;

  case E_CENA_BNDS:
    data->cena_bnds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    if(data->cena_bnds.ravno_atof() != 0.)
     {
      cena_snds=data->cena_bnds.ravno_atof() + data->cena_bnds.ravno_atof()*data->pnds/100.;
      cena_snds=iceb_u_okrug(cena_snds,okrg1);
      sprintf(strsql,"%.10g",data->kolih.ravno_atof()*cena_snds);

      gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA_SNDS]),strsql);

      if(data->kolih.ravno_atof() != 0.)
       {
        //Сумма без НДС
        cena_bnds=data->cena_bnds.ravno_atof();
        cena_bnds=iceb_u_okrug(cena_bnds,okrcn);
        sprintf(strsql,"%.10g",data->kolih.ravno_atof()*cena_bnds);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA_BNDS]),strsql);

        //Сумма с НДС      
        cena_snds=cena_bnds+cena_bnds*data->pnds/100.;
        cena_snds=iceb_u_okrug(cena_snds,okrg1);
        sprintf(strsql,"%.10g",data->kolih.ravno_atof()*cena_snds);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA_SNDS]),strsql);
       }      
      
     }
    break;

  case E_SUMA_BNDS:
    data->suma_bnds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(data->suma_bnds.ravno_atof() != 0.)
     {
      kolih=data->kolih.ravno_atof();
      if(kolih == 0.)
       {
        if(data->cena_bnds.ravno_atof() != 0.)
         {
          kolih=data->suma_bnds.ravno_atof()/data->cena_bnds.ravno_atof();
          sprintf(strsql,"%.10g",kolih);
          gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOLIH]),strsql);
         }
       }
      if(kolih == 0.)
       break;

      //Пересчитываем цену без НДС
      cena_bnds=data->suma_bnds.ravno_atof()/kolih;
      cena_bnds=iceb_u_okrug(cena_bnds,okrcn);

      sprintf(strsql,"%.10g",cena_bnds);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA_BNDS]),strsql);

      //Пересчитываем цену с НДС
      cena_snds=cena_bnds+cena_bnds*data->pnds/100.;
      cena_snds=iceb_u_okrug(cena_snds,okrg1);

      sprintf(strsql,"%.10g",cena_snds);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA_SNDS]),strsql);

      //Пересчитываем сумму с НДС
      suma_snds=cena_snds*kolih;
      suma_snds=iceb_u_okrug(suma_snds,okrg1);
      sprintf(strsql,"%.10g",suma_snds);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMA_SNDS]),strsql);
      
     }
    break;

  case E_SUMA_SNDS:
    data->suma_snds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(data->suma_snds.ravno_atof() == 0.)
     break;

    suma_bnds=data->suma_snds.ravno_atof()-data->suma_snds.ravno_atof()*data->pnds/(100.-data->pnds);
    suma_bnds=iceb_u_okrug(suma_bnds,okrg1);
    
    kolih=data->kolih.ravno_atof();
    if(kolih == 0.)
     {
      if(data->cena_snds.ravno_atof() != 0.)
       {
        kolih=data->suma_snds.ravno_atof()/data->cena_snds.ravno_atof();
        sprintf(strsql,"%.10g",kolih);
        gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOLIH]),strsql);
       }
     }
    if(kolih == 0.)
     break;

    //Пересчитываем цену без НДС
    cena_bnds=suma_bnds/kolih;
    cena_bnds=iceb_u_okrug(cena_bnds,okrcn);

    sprintf(strsql,"%.10g",cena_bnds);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA_BNDS]),strsql);
    //Пересчитываем цену с НДС
    cena_snds=cena_bnds+cena_bnds*data->pnds/100.;
    cena_snds=iceb_u_okrug(cena_snds,okrg1);

    sprintf(strsql,"%.10g",cena_snds);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA_SNDS]),strsql);
    break;

  case E_SHETU:
    data->shetu.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_EI:
    data->ei.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET_PS:
    data->shet_ps.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/****************************/
/*Запись в документ*/
/**********************/

int vuslw_v_zap(class vuslw_v_data *data)
{

if(data->kolih.getdlinna() <= 1 || data->cena_bnds.getdlinna() <= 1 \
 || data->shetu.getdlinna() <= 1 || data->ei.getdlinna() <= 1)
  {
   iceb_menu_soob(gettext("Не введены все обязательные реквизиты !"),data->window);
   return(1);
  }

struct OPSHET	shetv;
if(iceb_prsh1(data->shetu.ravno(),&shetv,data->window) != 0)
 return(1);

char strsql[512];

/*Проверяем единицу измерения*/
sprintf(strsql,"select kod from Edizmer where kod='%s'",data->ei.ravno());

if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не нашли единицу измерения"),data->ei.ravno());
   iceb_menu_soob(strsql,data->window);
   return(1);
 }

double cenabn=data->cena_bnds.ravno_atof();
cenabn=iceb_u_okrug(cenabn,okrcn);
if(cenabn < 0.01)
 {
   iceb_menu_soob(gettext("Не введена цена без НДС !"),data->window);
   return(1);
 }


if(data->shet_ps.getdlinna() > 1)
 if(iceb_prsh1(data->shet_ps.ravno(),&shetv,data->window) != 0)
  return(1);

if(data->nom_zap == -1) //Новая запись
 {
  if(zapuvdokw(data->tipz,data->dd,data->md,data->gd,data->nomdok.ravno(),data->metka_zap,
  data->kodzap.ravno_atoi(),data->kolih.ravno_atof(),cenabn,
  data->ei.ravno(),data->shetu.ravno(),data->podr,data->shet_ps.ravno(),"",data->window) != 0)
   return(1);
 }
else  //Корректировка старой записи
 {

  if(usl_pvkuw(data->ktoz,data->window) != 0)
   return(1);

  sprintf(strsql,"update Usldokum1 set \
kolih=%.10g,\
cena=%.10g,\
ei='%s',\
shsp='%s',\
shetu='%s' \
where datd='%04d-%d-%d' and podr=%d and nomd='%s' and metka=%d and \
kodzap=%d and tp=%d and nz=%d",
  data->kolih.ravno_atof(),
  cenabn,
  data->ei.ravno(),
  data->shet_ps.ravno(),
  data->shetu.ravno(),
  data->gd,data->md,data->dd,data->podr,data->nomdok.ravno(),data->metka_zap,data->kodzap.ravno_atoi(),
  data->tipz,data->nom_zap);
//  printf("vuslw_v-%s\n",strsql);
  if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
   return(1);
 }

return(0);
}
