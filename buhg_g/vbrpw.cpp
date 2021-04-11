/*$Id:$*/
/*22.03.2017	09.07.2004	Белых А.И.	vbrpw.c
Ввод материалла без привязки к карточке
*/
#include "buhg_g.h"

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
  E_KOLIH,
  E_CENA,
  E_EI,
  E_NDS,
  E_SHET_POL,
  E_NOMZ,
//обязательные для чтения реквизиты
  E_CENASNDS,
  E_KOLTAR,
  E_SUMABNDS,
  KOLENTER  
 };

class vbrpw_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-записано 1 нет
  
  short dd,md,gd;
  iceb_u_str nomdok;
  int tipz;
  int skl;
  iceb_u_str kodm;
  
  iceb_u_str kolih;
  iceb_u_str cenasnds;
  iceb_u_str cena;
  iceb_u_str ei;
  iceb_u_str nds;
  iceb_u_str koltar;
  iceb_u_str sumabnds;
  iceb_u_str nomz;
  class iceb_u_str shet_pol;
  double     krat;  
  short      metkanz;
  float pnds;
  vbrpw_data() //Конструктор
   {
    metkanz=0;
    krat=0.;
    kl_shift=0;
    voz=1;
    kodm.new_plus("");
    kolih.new_plus("");
    cenasnds.new_plus("");
    cena.new_plus("");
    ei.new_plus("");
    nds.new_plus("");
    koltar.new_plus("");
    sumabnds.new_plus("");
    nomz.new_plus("");
    shet_pol.new_plus("");
   }

  void read_rek()
   {
//    for(int i=0; i < 4+metkanz; i++)
//      g_signal_emit_by_name(entry[i],"activate");
    kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOLIH])));
    cena.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_CENA])));
    ei.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_EI])));
    nds.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NDS])));
    shet_pol.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET_POL])));
    if(metkanz == 1)
     nomz.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMZ])));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
     {
      if(metkanz == 0 && i == E_NOMZ)
        continue;
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
     }
   }
 };

gboolean   vbrpw_key_press(GtkWidget *widget,GdkEventKey *event,class vbrpw_data *data);
void    vbrpw_vvod(GtkWidget *widget,class vbrpw_data *data);
void  vbrpw_knopka(GtkWidget *widget,class vbrpw_data *data);
int vbrpw_zap(class vbrpw_data *data);
void  vbrpw_e_knopka(GtkWidget *widget,class vbrpw_data *data);

extern SQL_baza bd;
extern short	cnsnds; /*0-Цена материалла без ндс 1- с ндс*/
extern double	okrcn;  /*Округление цены*/
extern double   okrg1;  /*Округление суммы*/


int  vbrpw(short dd,short md,short gd,int skl,const char *nomdok,int kodm,int tipz,GtkWidget *wpredok)
{
vbrpw_data data;
char strsql[512];
iceb_u_str naim;
SQL_str    row;
SQLCURSOR  cur;

data.dd=dd;
data.md=md;
data.gd=gd;
data.skl=skl;
data.tipz=tipz;
data.nomdok.new_plus(nomdok);

iceb_u_str kodop;
kodop.plus("");
data.pnds=0.;
//Узнаём код операции
sprintf(strsql,"select kodop,pn from Dokummat where datd='%d-%d-%d' and sklad=%d and nomd='%s' \
and tip=%d",gd,md,dd,skl,nomdok,tipz);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  kodop.new_plus(row[0]);
  data.pnds=atof(row[1]);
 }  

if(iceb_poldan("Коды операций прихода из производства",strsql,"matnast.alx",wpredok) == 0)
 if(iceb_u_proverka(strsql,kodop.ravno(),0,1) == 0)
   data.metkanz=1;



naim.new_plus("");
//Читаем наименование материалла
sprintf(strsql,"select kodm,naimat,ei,cenapr,krat,nds from Material where kodm=%d",kodm);
printf("strsql=%s\n",strsql);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден код материалла"));
  repl.plus(" ");
  repl.plus(kodm);
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);
  return(1); 
 }

naim.new_plus(row[1]);
data.kodm.new_plus(row[0]);
data.ei.new_plus(row[2]);

double cenamat=atof(row[3]);
int ndscen=atoi(row[5]);
double cenabnds=cenamat;
data.krat=atof(row[4]);
if(cnsnds == 1 && cenamat != 0. && ndscen == 0.)
 {
  cenabnds=cenamat-cenamat*data.pnds/(100.+data.pnds);
  cenabnds=iceb_u_okrug(cenabnds,okrcn);
 }

if(cenabnds != 0)
 { 
  sprintf(strsql,"%.10g",cenabnds);
  data.cena.new_plus(strsql);
 }

double cenasnds=cenabnds+cenabnds*data.pnds/100.;
cenasnds=iceb_u_okrug(cenasnds,okrg1);

if(cenasnds != 0.)
 {
  sprintf(strsql,"%.10g",cenasnds);
  data.cenasnds.new_plus(strsql);
 }
sprintf(strsql,"%.5g",data.pnds);
data.nds.new_plus(strsql);


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод материалла"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vbrpw_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
iceb_u_str repl;
repl.new_plus(gettext("Ввод материалла без привязки к карточке"));
repl.plus("\n");
repl.plus(kodm);
repl.plus(" ");
repl.plus(naim.ravno());

if(cnsnds == 1)
 {
  sprintf(strsql,"%s: %.10g",gettext("Цена с НДС"),cenasnds);
  repl.ps_plus(strsql);
 }

GtkWidget *label=gtk_label_new(repl.ravno());

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


sprintf(strsql,"%s",gettext("Количество"));
label=gtk_label_new(strsql);
data.entry[E_KOLIH] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOLIH]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOLIH], "activate",G_CALLBACK(vbrpw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih.ravno());
gtk_widget_set_name(data.entry[E_KOLIH],iceb_u_inttochar(E_KOLIH));

sprintf(strsql,"%s",gettext("Цена с НДС"));
label=gtk_label_new(strsql);
data.entry[E_CENASNDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_CENASNDS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENASNDS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENASNDS]), data.entry[E_CENASNDS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_CENASNDS], "activate",G_CALLBACK(vbrpw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENASNDS]),data.cenasnds.ravno());
gtk_widget_set_name(data.entry[E_CENASNDS],iceb_u_inttochar(E_CENASNDS));

sprintf(strsql,"%s",gettext("Цена без НДС"));
label=gtk_label_new(strsql);
data.entry[E_CENA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_CENA]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), data.entry[E_CENA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_CENA], "activate",G_CALLBACK(vbrpw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA]),data.cena.ravno());
gtk_widget_set_name(data.entry[E_CENA],iceb_u_inttochar(E_CENA));

sprintf(strsql,"%s",gettext("Единица измерения"));
label=gtk_label_new(strsql);
data.entry[E_EI] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_EI]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_EI], "activate",G_CALLBACK(vbrpw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.ei.ravno());
gtk_widget_set_name(data.entry[E_EI],iceb_u_inttochar(E_EI));

sprintf(strsql,"%s",gettext("НДС"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), label, FALSE, FALSE, 0);

data.entry[E_NDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NDS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), data.entry[E_NDS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NDS], "activate",G_CALLBACK(vbrpw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NDS]),data.nds.ravno());
gtk_widget_set_name(data.entry[E_NDS],iceb_u_inttochar(E_NDS));


sprintf(strsql,"%s",gettext("Счёт получения"));
data.knopka_enter[E_SHET_POL]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_POL]), data.knopka_enter[E_SHET_POL], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_SHET_POL],"clicked",G_CALLBACK(vbrpw_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET_POL],iceb_u_inttochar(E_SHET_POL));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET_POL],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET_POL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHET_POL]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET_POL]), data.entry[E_SHET_POL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET_POL], "activate",G_CALLBACK(vbrpw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET_POL]),data.shet_pol.ravno());
gtk_widget_set_name(data.entry[E_SHET_POL],iceb_u_inttochar(E_SHET_POL));

if(data.metkanz == 1)
 {
  sprintf(strsql,"%s",gettext("Номер заказа"));
  label=gtk_label_new(strsql);
  data.entry[E_NOMZ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(  data.entry[E_NOMZ]),20);
  gtk_box_pack_start (GTK_BOX (hbox[E_NOMZ]), label, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox[E_NOMZ]), data.entry[E_NOMZ], TRUE, TRUE, 0);
  g_signal_connect(data.entry[E_NOMZ], "activate",G_CALLBACK(vbrpw_vvod),&data);
  gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMZ]),data.nomz.ravno());
  gtk_widget_set_name(data.entry[E_NOMZ],iceb_u_inttochar(E_NOMZ));
 }
sprintf(strsql,"%s",gettext("Количество тары"));
label=gtk_label_new(strsql);
data.entry[E_KOLTAR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOLTAR]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLTAR]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLTAR]), data.entry[E_KOLTAR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOLTAR], "activate",G_CALLBACK(vbrpw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLTAR]),data.koltar.ravno());
gtk_widget_set_name(data.entry[E_KOLTAR],iceb_u_inttochar(E_KOLTAR));

sprintf(strsql,"%s",gettext("Сумма без НДС"));
label=gtk_label_new(strsql);
data.entry[E_SUMABNDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMABNDS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMABNDS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMABNDS]), data.entry[E_SUMABNDS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMABNDS], "activate",G_CALLBACK(vbrpw_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMABNDS]),data.sumabnds.ravno());
gtk_widget_set_name(data.entry[E_SUMABNDS],iceb_u_inttochar(E_SUMABNDS));



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(vbrpw_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Просмотр нужных реквизитов"));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(vbrpw_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(vbrpw_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(vbrpw_knopka),&data);
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
void  vbrpw_e_knopka(GtkWidget *widget,class vbrpw_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   
  case E_SHET_POL:

    iceb_vibrek(1,"Plansh",&data->shet_pol,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET_POL]),data->shet_pol.ravno());

    return;  


 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vbrpw_key_press(GtkWidget *widget,GdkEventKey *event,class vbrpw_data *data)
{

//printf("vbrpw_key_press\n");
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
void  vbrpw_knopka(GtkWidget *widget,class vbrpw_data *data)
{
SQLCURSOR cur;
iceb_u_str kod;
iceb_u_str naikod;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(vbrpw_zap(data) != 0)
      return;
    data->voz=0;        
    gtk_widget_destroy(data->window);
    data->window=NULL;
    return;  

  case FK3:
    if(iceb_vibrek(1,"Edizmer",&data->ei,data->window) == 0)
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->ei.ravno());
    return;

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

void    vbrpw_vvod(GtkWidget *widget,class vbrpw_data *data)
{
double bb=0.;
double cena=0.;
char strsql[512];

int enter=atoi(gtk_widget_get_name(widget));
switch (enter)
 {
  case E_KOLIH:
    data->kolih.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(data->cena.getdlinna() > 1)
     {
      bb=data->cena.ravno_atof()*data->kolih.ravno_atof();
      bb=iceb_u_okrug(bb,okrg1);
      sprintf(strsql,"%.10g",bb);
      data->sumabnds.new_plus(strsql);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMABNDS]),data->sumabnds.ravno());
     }

    if(data->krat != 0.)
     {
      bb=data->kolih.ravno_atof()/data->krat;
      sprintf(strsql,"%.f",bb);
      data->koltar.new_plus(strsql);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOLTAR]),data->koltar.ravno());
     }       


    break;

  case E_CENA:
    data->cena.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    bb=data->cena.ravno_atof();
    cena=bb=iceb_u_okrug(bb,okrcn);
    sprintf(strsql,"%.10g",bb);

    bb=bb+bb*data->pnds/100.;
    bb=iceb_u_okrug(bb,okrg1);

    sprintf(strsql,"%.10g",bb);
    data->cenasnds.new_plus(strsql);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENASNDS]),data->cenasnds.ravno());


    bb=cena*data->kolih.ravno_atof();
    bb=iceb_u_okrug(bb,okrg1);
    sprintf(strsql,"%.10g",bb);
    data->sumabnds.new_plus(strsql);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMABNDS]),data->sumabnds.ravno());
    
    break;

  case E_CENASNDS:
    data->cenasnds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    bb=data->cenasnds.ravno_atof();

    bb=bb-bb*data->pnds/(100.+data->pnds);
    bb=iceb_u_okrug(bb,okrcn);
    sprintf(strsql,"%.10g",bb);
    data->cena.new_plus(strsql);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA]),data->cena.ravno());

    bb=bb*data->kolih.ravno_atof();
    bb=iceb_u_okrug(bb,okrg1);
    sprintf(strsql,"%.10g",bb);
    data->sumabnds.new_plus(strsql);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMABNDS]),data->sumabnds.ravno());

    break;


  case E_EI:
    data->ei.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NDS:
    data->nds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET_POL:
    data->shet_pol.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMZ:
    data->nomz.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOLTAR:
    data->koltar.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    if(data->krat == 0.)
     break;

    bb=data->koltar.ravno_atof()*data->krat;
    sprintf(strsql,"%.10g",bb);
    data->kolih.new_plus(strsql);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOLIH]),data->kolih.ravno());

    bb=data->cena.ravno_atof()*data->kolih.ravno_atof();
    bb=iceb_u_okrug(bb,okrg1);
    sprintf(strsql,"%.10g",bb);
    data->sumabnds.new_plus(strsql);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SUMABNDS]),data->sumabnds.ravno());
          
    break;

  case E_SUMABNDS:
    data->sumabnds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    
    if(data->sumabnds.ravno_atof() != 0. && data->kolih.ravno_atof() != 0.)
     {
      bb=data->sumabnds.ravno_atof()/data->kolih.ravno_atof();
      bb=iceb_u_okrug(bb,okrcn);
      sprintf(strsql,"%.10g",bb);
      data->cena.new_plus(strsql);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENA]),data->cena.ravno());

      bb=bb+bb*data->pnds/100.;
      bb=iceb_u_okrug(bb,okrg1);

      sprintf(strsql,"%.10g",bb);
      data->cenasnds.new_plus(strsql);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_CENASNDS]),data->cenasnds.ravno());
     }
    break;


 }

if(enter == E_NOMZ && data->metkanz == 0)
 enter+=1;

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/***************/
/*Запись       */
/**************/

int vbrpw_zap(class vbrpw_data *data)
{

if(data->kolih.getdlinna() <= 1 || data->cena.getdlinna() <= 1 || data->ei.getdlinna() <= 1 || \
data->nds.getdlinna() <= 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не введены все обязательные реквизиты !"));
  iceb_menu_soob(&repl,data->window);
  return(1);
 }
 
char strsql[512];
SQL_str row;
SQLCURSOR cur;

/*Проверяем единицу измерения*/
sprintf(strsql,"select kod from Edizmer where kod='%s'",data->ei.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найдено единицу измерения"));
  repl.plus(" ");
  repl.plus(data->ei.ravno());
  repl.plus(" !");
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

double cena=data->cena.ravno_atof();
cena=iceb_u_okrug(cena,okrcn);

double nds=data->nds.ravno_atof();
nds=iceb_u_okrug(nds,0.01);
if(zapvdokw(data->dd,data->md,data->gd,data->skl,0,data->kodm.ravno_atoi(),
data->nomdok.ravno(),data->kolih.ravno_atof(),cena,data->ei.ravno(),nds,0,0,
data->tipz,0,"",data->nomz.ravno(),0,"",data->window) != 0)
 return(1);




return(0);
}
