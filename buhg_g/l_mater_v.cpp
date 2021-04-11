/*$Id: l_mater_v.c,v 1.34 2013/09/26 09:46:50 sasa Exp $*/
/*26.02.2017	05.05.2004	Белых А.И.	iceb_l_mater_v.c
Ввод и корректировка материалов
*/
#include "buhg_g.h"
#include "l_mater.h"

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };
enum
 {
  E_KOD,
  E_GRUP,
  E_NAIM,
  E_CENA,
  E_MCENA,
  E_EI,
  E_KRAT,
  E_KODTAR,
  E_FAS,
  E_HTRIX,
  E_ARTIK,
  E_KRIOST,
  E_KODTOV,
  KOLENTER  
 };

class  mater_data_vvod
 {
  public:
  GtkWidget *window;
  GtkWidget *knopka_kod;
  GtkWidget *opt_grup;
  GtkWidget *opt_ei;
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka_pv[KOL_FK];
  GtkWidget *opt1;
  class iceb_u_str name_window;
  int voz;
  class iceb_u_str kodmat;
  class iceb_u_str naimkodmat;
  
  short     kl_shift; //0-отжата 1-нажата  
  static int kod_mat_pred;  

  mater_rek vvod;

  mater_data_vvod()
   {
    voz=1;
    kl_shift=0;
    kodmat.plus("");
    naimkodmat.plus("");  

   }

  void read_rek()
   {
    vvod.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD])));
    vvod.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NAIM])));
    vvod.cena.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_CENA])));
    vvod.krat.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KRAT])));
    vvod.kodtar.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODTAR])));
    vvod.fas.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_FAS])));

    vvod.htrix.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_HTRIX])));
    vvod.artik.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_ARTIK])));
    vvod.kriost.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KRIOST])));
    vvod.kodtov.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODTOV])));


    vvod.mcena=gtk_combo_box_get_active (GTK_COMBO_BOX(opt1));
    vvod.grupa.new_plus(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(opt_grup)));
    vvod.ei.new_plus(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(opt_ei)));
   }

 };
 
gboolean   mater_v_key_press(GtkWidget *widget,GdkEventKey *event,class mater_data_vvod *data);
void  mater_v_knopka(GtkWidget *widget,class mater_data_vvod *data);
void    mater_v_vvod(GtkWidget *widget,class mater_data_vvod *data);
int mater_pk(const char *god,GtkWidget*);
int mater_zap(class mater_data_vvod *data);

void  grupa_get(GtkWidget *widget,class mater_data_vvod *data);
void  ediz_get(GtkWidget *widget,class mater_data_vvod *data);
int mater_pnaim(const char *naim,GtkWidget *wpredok);
void  l_mater_v_e_knopka(GtkWidget *widget,class  mater_data_vvod *data);

int mater_data_vvod::kod_mat_pred;

int l_mater_v(class iceb_u_str *kodmat,GtkWidget *wpredok)
{
int gor=0,ver=0;
char strsql[512];
class iceb_u_str kikz;
class  mater_data_vvod data;
data.kodmat.new_plus(kodmat->ravno());
    
data.kl_shift=0;
data.vvod.clear_zero();
data.name_window.plus(__FUNCTION__);

SQL_str row;
SQLCURSOR cur;

if(data.kodmat.getdlinna() > 1)
 {
  data.vvod.kod.new_plus(data.kodmat.ravno());
  sprintf(strsql,"select * from Material where kodm=%s",data.kodmat.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдена запись для корректировки !"));
    iceb_menu_soob(&repl,data.window);
    return(1);
   }
  data.vvod.naim.new_plus(row[2]);
  data.naimkodmat.new_plus(row[2]);
  if(row[6][0] != '0')
    data.vvod.cena.new_plus(row[6]);
  if(row[7][0] != '0')
   data.vvod.krat.new_plus(row[7]);
  if(row[9][0] != '0')
    data.vvod.kodtar.new_plus(row[9]);
  if(row[8][0] != '0')
   data.vvod.fas.new_plus(row[8]);
  data.vvod.htrix.new_plus(row[3]);
  data.vvod.mcena=atoi(row[12]);
  data.vvod.artik.new_plus(row[14]);
  if(row[5][0] != '0')
   data.vvod.kriost.new_plus(row[5]);
  data.vvod.grupa.new_plus(row[1]);
  data.vvod.ei.new_plus(row[4]);
  data.vvod.kodtov.new_plus(row[15]);
  kikz.plus(iceb_kikz(row[10],row[11],wpredok));
  
  sprintf(strsql,"select naik from Edizmer where kod='%s'",data.vvod.ei.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    data.vvod.ei.plus(" ");
    data.vvod.ei.plus(row[0]);
   }

  sprintf(strsql,"select naik from Grup where kod='%s'",data.vvod.grupa.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    data.vvod.grupa.plus(" ");
    data.vvod.grupa.plus(row[0]);
   }
 }
else
 {
  sprintf(strsql,"%d",nomkmw(data.kod_mat_pred,wpredok));

  data.vvod.kod.new_plus(strsql);
  
 } 
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

GtkWidget *label=NULL;

if(data.kodmat.getdlinna() <= 1)
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
//g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_widget_destroyed),&data.window);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(mater_v_key_press),&data);


if(wpredok != NULL)
 {
//  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));


  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

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
gtk_container_add (GTK_CONTAINER (vbox), label);
for(int i=0; i < KOLENTER; i++)
 gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


//label=gtk_label_new(gettext("Код"));
//gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), label, FALSE, FALSE, 0);

data.knopka_kod=gtk_button_new_with_label(gettext("Код материала"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.knopka_kod, FALSE, FALSE,1);
g_signal_connect(data.knopka_kod,"clicked",G_CALLBACK(l_mater_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_kod,iceb_u_inttochar(E_KOD));
gtk_widget_set_tooltip_text(data.knopka_kod,gettext("Получить новый код материалла"));

data.entry[E_KOD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD]), data.entry[E_KOD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD], "activate",G_CALLBACK(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD]),data.vvod.kod.ravno());
gtk_widget_set_name(data.entry[E_KOD],iceb_u_inttochar(E_KOD));


label=gtk_label_new(gettext("Код группы"));
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP]), label, FALSE, FALSE, 0);

//Создаем выбор группы
/********************************************/


sprintf(strsql,"select kod,naik from Grup order by naik asc");
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);

class iceb_u_spisok VOD;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s %s",row[0],row[1]);/*для правильной работы должен быть только один пробел между кодом и названием*/
  VOD.plus(strsql);
 }

if(kolstr == 0)
 VOD.plus(gettext("Не введён список групп !!!"));

iceb_pm_vibor_enter(&VOD,&data.opt_grup,data.vvod.grupa.ravno());
gtk_box_pack_start (GTK_BOX (hbox[E_GRUP]), data.opt_grup, TRUE, TRUE, 0);


label=gtk_label_new(gettext("Наименование"));
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);

data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),99);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.vvod.naim.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));

label=gtk_label_new(gettext("Цена"));
data.entry[E_CENA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_CENA]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), data.entry[E_CENA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_CENA], "activate",G_CALLBACK(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA]),data.vvod.cena.ravno());
gtk_widget_set_name(data.entry[E_CENA],iceb_u_inttochar(E_CENA));



label=gtk_label_new(gettext("Цена введена"));
gtk_box_pack_start (GTK_BOX (hbox[E_MCENA]), label, FALSE, FALSE, 0);

class iceb_u_spisok spmenu;
spmenu.plus(gettext("С НДС"));
spmenu.plus(gettext("Без НДС"));

iceb_pm_vibor(&spmenu,&data.opt1,data.vvod.mcena);
gtk_box_pack_start (GTK_BOX (hbox[E_MCENA]),data.opt1, FALSE, FALSE, 0);


label=gtk_label_new(gettext("Единица измерения"));
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), label, FALSE, FALSE, 0);
//Создаем выбор единиц измерения
/********************************************/

sprintf(strsql,"select kod,naik from Edizmer order by naik asc");
kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);

class iceb_u_spisok VOD1;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s %s",row[0],row[1]); /*для правильной работы должен быть только один пробел между кодом и названием*/
  VOD1.plus(strsql);
 }

if(kolstr == 0)
 VOD1.plus(gettext("Не введён список единиц измерения !!!"));

iceb_pm_vibor_enter(&VOD1,&data.opt_ei,data.vvod.ei.ravno());
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.opt_ei, TRUE, TRUE, 0);

label=gtk_label_new(gettext("Кратность"));
data.entry[E_KRAT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KRAT]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KRAT]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KRAT]), data.entry[E_KRAT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KRAT], "activate",G_CALLBACK(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KRAT]),data.vvod.krat.ravno());
gtk_widget_set_name(data.entry[E_KRAT],iceb_u_inttochar(E_KRAT));

label=gtk_label_new(gettext("Код тары"));
data.entry[E_KODTAR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODTAR]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KODTAR]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KODTAR]), data.entry[E_KODTAR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODTAR], "activate",G_CALLBACK(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODTAR]),data.vvod.kodtar.ravno());
gtk_widget_set_name(data.entry[E_KODTAR],iceb_u_inttochar(E_KODTAR));

label=gtk_label_new(gettext("Фасовка"));
data.entry[E_FAS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_FAS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_FAS]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_FAS]), data.entry[E_FAS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_FAS], "activate",G_CALLBACK(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FAS]),data.vvod.fas.ravno());
gtk_widget_set_name(data.entry[E_FAS],iceb_u_inttochar(E_FAS));

label=gtk_label_new(gettext("Штрих код"));
data.entry[E_HTRIX] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_HTRIX]),49);
gtk_box_pack_start (GTK_BOX (hbox[E_HTRIX]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_HTRIX]), data.entry[E_HTRIX], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_HTRIX], "activate",G_CALLBACK(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_HTRIX]),data.vvod.htrix.ravno());
gtk_widget_set_name(data.entry[E_HTRIX],iceb_u_inttochar(E_HTRIX));

label=gtk_label_new(gettext("Артикул"));
data.entry[E_ARTIK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_ARTIK]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_ARTIK]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_ARTIK]), data.entry[E_ARTIK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_ARTIK], "activate",G_CALLBACK(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_ARTIK]),data.vvod.artik.ravno());
gtk_widget_set_name(data.entry[E_ARTIK],iceb_u_inttochar(E_ARTIK));

label=gtk_label_new(gettext("Критичный остаток"));
data.entry[E_KRIOST] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KRIOST]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KRIOST]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KRIOST]), data.entry[E_KRIOST], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KRIOST], "activate",G_CALLBACK(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KRIOST]),data.vvod.kriost.ravno());
gtk_widget_set_name(data.entry[E_KRIOST],iceb_u_inttochar(E_KRIOST));

label=gtk_label_new(gettext("Код УКТ ВЭД"));  /*Украински класификато товата внешне экономической деятельности*/
data.entry[E_KODTOV] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODTOV]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KODTOV]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KODTOV]), data.entry[E_KODTOV], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODTOV], "activate",G_CALLBACK(mater_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODTOV]),data.vvod.kodtov.ravno());
gtk_widget_set_name(data.entry[E_KODTOV],iceb_u_inttochar(E_KODTOV));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka_pv[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka_pv[FK2],gettext("Запись введеной в меню информации"));
g_signal_connect(data.knopka_pv[FK2],"clicked",G_CALLBACK(mater_v_knopka),&data);
gtk_widget_set_name(data.knopka_pv[FK2],iceb_u_inttochar(FK2));

sprintf(strsql,"F4 %s",gettext("Код"));
data.knopka_pv[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka_pv[FK4],gettext("Получить новый код материалла"));
g_signal_connect(data.knopka_pv[FK4],"clicked",G_CALLBACK(mater_v_knopka),&data);
gtk_widget_set_name(data.knopka_pv[FK4],iceb_u_inttochar(FK4));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka_pv[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka_pv[FK10],gettext("Завершение работы с меню без записи введенной информации"));
g_signal_connect(data.knopka_pv[FK10],"clicked",G_CALLBACK(mater_v_knopka),&data);
gtk_widget_set_name(data.knopka_pv[FK10],iceb_u_inttochar(FK10));

gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka_pv[FK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka_pv[FK4], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka_pv[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gtk_main();


if(data.voz == 0)
 kodmat->new_plus(data.vvod.kod.ravno());

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
//  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_mater_v_e_knopka(GtkWidget *widget,class  mater_data_vvod *data)
{
iceb_u_str kod("");
iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("dvtmcf1_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
  case E_KOD:
   data->vvod.kod.new_plus(nomkmw(data->kod_mat_pred,data->window));
   gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->vvod.kod.ravno());
      
   return;  
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   mater_v_key_press(GtkWidget *widget,GdkEventKey *event,class mater_data_vvod *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka_pv[FK2],"clicked");

    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka_pv[FK4],"clicked");

    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka_pv[FK10],"clicked");

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
void  mater_v_knopka(GtkWidget *widget,class mater_data_vvod *data)
{
char bros[512];

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    if(mater_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
      data->window=NULL;
     }
    return;  


  case FK4:
    sprintf(bros,"%d",iceb_get_new_kod("Material","kodm",0,data->window));
    data->vvod.kod.new_plus(bros);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD]),data->vvod.kod.ravno());
    return;
    
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    mater_v_vvod(GtkWidget *widget,class mater_data_vvod *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("mater_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KOD:
    data->vvod.kod.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    gtk_widget_grab_focus(data->opt_grup);
    return;

  case E_NAIM:
    data->vvod.naim.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_CENA:
    data->vvod.cena.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    gtk_widget_grab_focus(data->opt_ei);
    return;

  case E_KRAT:
    data->vvod.krat.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODTAR:
    data->vvod.kodtar.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_FAS:
    data->vvod.fas.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_HTRIX:
    data->vvod.htrix.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_ARTIK:
    data->vvod.artik.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KRIOST:
    data->vvod.kriost.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int mater_zap(class mater_data_vvod *data)
{
char strsql[1024];
time_t   vrem;

data->read_rek();

if(data->vvod.kod.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён код !"),data->window);
  return(1);
 }

if(data->vvod.naim.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено наименование !"),data->window);
  return(1);
 }

if(data->vvod.grupa.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введено код группы !"),data->window);
  return(1);
 }

//Проверяем код группы
sprintf(strsql,"select kod from Grup where kod=%s",data->vvod.grupa.ravno_filtr_dpp());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",
  gettext("Не найдено код группы"),
  data->vvod.grupa.ravno_filtr_dpp());

  iceb_menu_soob(strsql,data->window);
  return(1);
 }

//Проверяем единицу измерения
if(data->vvod.ei.getdlinna() > 1)
 {
  sprintf(strsql,"select kod from Edizmer where kod='%s'",data->vvod.ei.ravno_filtr_dpp());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %s !",
    gettext("Не найденa единица измерения"),
    data->vvod.ei.ravno_filtr_dpp());

    iceb_menu_soob(strsql,data->window);
    return(1);
   }
  
 }
 


short metkakor=0;
time(&vrem);

if(data->kodmat.getdlinna() <= 1)
 {

  if(mater_pk(data->vvod.kod.ravno(),data->window) != 0)
   return(1);
  
  if(mater_pnaim(data->vvod.naim.ravno(),data->window) != 0)
   return(1);
  
  sprintf(strsql,"insert into Material \
values (%d,%d,'%s','%s','%s',%.10g,%.10g,%.10g,%.10g,%d,%d,%ld,%d,%d,'%s','%s')",
   data->vvod.kod.ravno_atoi(),
   data->vvod.grupa.ravno_atoi(),
   data->vvod.naim.ravno_filtr(),
   data->vvod.htrix.ravno_filtr(),
   data->vvod.ei.ravno_filtr_dpp(),
   data->vvod.kriost.ravno_atof(),
   data->vvod.cena.ravno_atof(),
   data->vvod.krat.ravno_atof(),
   data->vvod.fas.ravno_atof(),
   data->vvod.kodtar.ravno_atoi(),
   iceb_getuid(data->window),vrem,
   data->vvod.mcena,
   0,
   data->vvod.artik.ravno_filtr(),
   data->vvod.kodtov.ravno_filtr());   
 }
else
 {
//  printf("metkazapisi=%d\n",data->metkazapisi);
  if(iceb_u_SRAV(data->naimkodmat.ravno(),data->vvod.naim.ravno(),0) != 0)
   {
    if(mater_pnaim(data->vvod.naim.ravno(),data->window) != 0)
     return(1);

   //проверяем разрешено ли оператору корректировать наименование
   class iceb_u_str spisok_log;
   if(iceb_poldan("Разрешено корректировать наименование материалла",&spisok_log,"matnast.alx",data->window) == 0)
    {
     if(spisok_log.getdlinna() > 1)
      {
        if(iceb_u_proverka(spisok_log.ravno(),iceb_u_getlogin(),0,0) != 0)
         {
          iceb_menu_soob(gettext("Вам запрещено корректировать наименование материалла !"),data->window);
          return(1);
         }
      }
    }

   }

  
  if(iceb_u_SRAV(data->kodmat.ravno(),data->vvod.kod.ravno(),0) != 0)
   {
    if(mater_pk(data->vvod.kod.ravno(),data->window) != 0)
     return(1);
    /*Проверка возможности корректировки кода*/
    if(l_mater_v_provkor(data->vvod.kod.ravno_atoi(),data->kodmat.ravno_atoi(),data->window) != 0)
     return(1);

    iceb_u_str repl;
    repl.plus(gettext("Корректировать код ? Вы уверены ?"));
    if(iceb_menu_danet(&repl,2,data->window) == 2)
      return(1);
    metkakor=1;
  
   }
  sprintf(strsql,"update Material \
set \
kodm = %d,\
kodgr= %d,\
naimat='%s',\
strihkod='%s',\
ei='%s',\
kriost=%.10g,\
cenapr=%.10g,\
krat=%.10g,\
fasv=%.10g,\
kodt=%d,\
ktoz=%d,\
vrem=%ld,\
nds=%d,\
msp=%d,\
artikul='%s',\
ku='%s' \
where kodm = %s",
   data->vvod.kod.ravno_atoi(),
   data->vvod.grupa.ravno_atoi(),
   data->vvod.naim.ravno_filtr(),
   data->vvod.htrix.ravno_filtr(),
   data->vvod.ei.ravno_filtr_dpp(),
   data->vvod.kriost.ravno_atof(),
   data->vvod.cena.ravno_atof(),
   data->vvod.krat.ravno_atof(),
   data->vvod.fas.ravno_atof(),
   data->vvod.kodtar.ravno_atoi(),
   iceb_getuid(data->window),vrem,
   data->vvod.mcena,
   0,
   data->vvod.artik.ravno_filtr(),
   data->vvod.kodtov.ravno_filtr(),
   data->kodmat.ravno());
   
 }

//printf("mater_zap-strsql=%s\n",strsql);
if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);

if(data->kodmat.getdlinna() <= 1)
  data->kod_mat_pred=data->vvod.kod.ravno_atoi();

gtk_widget_hide(data->window);

if(metkakor == 1)
 mater_kkvt(data->kodmat.ravno_atoi(),data->vvod.kod.ravno_atoi(),data->window);

return(0);

}
/*******************************/
/*Проверка кода материалла на уже введенную запись*/
/**********************************/
int mater_pk(const char *kod,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kodm from Material where kodm=%s",kod);
//printf("mater_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такой код уже введено !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}

/*******************************/
/*Проверка наименования материалла на уже введенную запись*/
/**********************************/
int mater_pnaim(const char *naim,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select naimat from Material where naimat='%s'",naim);
//printf("mater_pk-%s\n",strsql);

if(sql_readkey(&bd,strsql) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Такое наименование уже введено !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0);

}
/*****************************/
/*Корректировка кода в таблицах*/
/********************************/

void mater_kkvt(int skod,int nkod,GtkWidget *wpredok)
{

char strsql[512];

sprintf(strsql,"update Roznica set kodm=%d where kodm=%d and metka=1",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),wpredok);
   return;
  }
 else
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);
 }
sprintf(strsql,"update Dokummat1 set kodm=%d where kodm=%d",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Dokummat4 set km=%d where km=%d",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Dokummat4 set kd=%d where kd=%d",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Kart set kodm=%d where kodm=%d",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Uplmt set kodm=%d where kodm=%d",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Usldokum1 set kodzap=%d where kodzap=%d and metka=0",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Usldokum2 set kodzap=%d where kodzap=%d and metka=0",nkod,skod);
iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"update Specif set kodi=%d where kodi=%d and kz=0",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Specif set kodd=%d where kodd=%d and kz=0",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

sprintf(strsql,"update Restdok1 set kodz=%d where kodz=%d and mz=0",nkod,skod);
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
  iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);


}

/*******************************************/
/*проверка возможности корректировки кода*/
/*******************************************/
int l_mater_v_provkor(int nkod,int skod,GtkWidget *wpredok)
{

char strsql[512];

/*Проверяем есть ли изделие по старому коду и входит ли в него код на который хотим заменить*/
sprintf(strsql,"select kodi from Specif where kodi=%d and kodd=%d limit 1",skod,nkod);
if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  sprintf(strsql,"%s Specif\n%s\n%s",gettext("Таблица"),
  gettext("Коды взаимно связаны!"),
  gettext("Корректировка не возможна!"));
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
/*Проверяем есть ли издели по коду на который меняем и входит ли в него старый код*/
sprintf(strsql,"select kodi from Specif where kodi=%d and kodd=%d limit 1",nkod,skod);
if(iceb_sql_readkey(strsql,wpredok) > 0)
 {
  sprintf(strsql,"%s Specif\n%s\n%s",gettext("Таблица"),
  gettext("Коды взаимно связаны!"),
  gettext("Корректировка не возможна!"));
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
return(0);


}
