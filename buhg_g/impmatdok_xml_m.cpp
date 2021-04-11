/*$Id: impmatdok_xml_m.c,v 1.27 2014/05/07 10:58:06 sasa Exp $*/
/*11.07.2015	30.01.2015	Белых А.И.	impmatdok_xml_m.c
*/
#include "buhg_g.h"

enum
 {
  E_DATA,
  E_SKLAD,
  E_NOMDOK,
  E_KOD_OP,
  E_SHETU,
  KOLENTER  
 };

enum
 {
  FK2,
  FK4,
  FK10,
  KOL_FK
 };

class impmatdok_xml_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_FK];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  struct iceb_imp_xml_nn_r *rk; 
  impmatdok_xml_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk->datdok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA])));
    rk->sklad.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SKLAD])));
    rk->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK])));
    rk->kod_op.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_OP])));
    rk->shetu.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHETU])));
    rk->nds=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    
   }
 };


gboolean   impmatdok_xml_m_key_press(GtkWidget *widget,GdkEventKey *event,class impmatdok_xml_m_data *data);
void  impmatdok_xml_m_knopka(GtkWidget *widget,class impmatdok_xml_m_data *data);
void    impmatdok_xml_m_vvod(GtkWidget *widget,class impmatdok_xml_m_data *data);
int impmatdok_xml_zap(class impmatdok_xml_m_data *data);
void  impmatdok_xml_v_e_knopka(GtkWidget *widget,class impmatdok_xml_m_data *data);


extern SQL_baza  bd;
extern class iceb_u_str spmatshets; /*Список материальных счетов*/


int impmatdok_xml_m(class iceb_imp_xml_nn_r *rkv,GtkWidget *wpredok)
{
class iceb_u_str repl("");
class impmatdok_xml_m_data data;
char strsql[512];
data.rk=rkv;
data.rk->datdok.new_plus(iceb_u_poltekdat());

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

GtkWidget *label=NULL;

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт налоговой накладной"));

repl.new_plus(gettext("Импорт налоговой накладной"));
repl.ps_plus(gettext("Контагент"));
repl.plus(":");
repl.plus(data.rk->kontr_prod.ravno(),40);
repl.plus(" ",data.rk->naim_prod.ravno());
repl.ps_plus(gettext("Код ЕДРПОУ"));
repl.plus(":",data.rk->edrpuo_prod.ravno());
repl.ps_plus(gettext("Индивидуальный налоговый номер"));
repl.plus(":",data.rk->inn_prod.ravno());
repl.ps_plus(gettext("Номер налоговой накладной"));
repl.plus(":",data.rk->nom_nn.ravno());
repl.ps_plus(gettext("Дата налоговой накладной"));
repl.plus(":",data.rk->data_vnn.ravno());

label=gtk_label_new(repl.ravno());

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(impmatdok_xml_m_key_press),&data);

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

GtkWidget *hboxnds = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxnds),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_box_pack_start (GTK_BOX (vbox),label, TRUE, TRUE, 0);
for(int i=0; i < KOLENTER; i++)
 gtk_box_pack_start (GTK_BOX (vbox),hbox[i], TRUE, TRUE, 0);

gtk_box_pack_start (GTK_BOX (vbox),hboxnds, TRUE, TRUE, 0);

label=gtk_label_new(gettext("НДС"));
gtk_box_pack_start (GTK_BOX (hboxnds), label, FALSE, FALSE, 0);
//создание пункта меню
iceb_pm_nds(data.rk->datdok.ravno(),&data.opt,data.rk->nds);

gtk_box_pack_start (GTK_BOX (hboxnds), data.opt, FALSE, FALSE, 0);

gtk_box_pack_start (GTK_BOX (vbox),hboxknop, TRUE, TRUE, 0);




sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("д.м.г"));
data.knopka_enter[E_DATA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.knopka_enter[E_DATA], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATA],"clicked",G_CALLBACK(impmatdok_xml_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA],iceb_u_inttochar(E_DATA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA],gettext("Выбор даты"));

data.entry[E_DATA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA]), data.entry[E_DATA], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DATA], "activate",G_CALLBACK(impmatdok_xml_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA]),data.rk->datdok.ravno());
gtk_widget_set_name(data.entry[E_DATA],iceb_u_inttochar(E_DATA));



data.knopka_enter[E_SKLAD]=gtk_button_new_with_label(gettext("Склад"));
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.knopka_enter[E_SKLAD], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_SKLAD],"clicked",G_CALLBACK(impmatdok_xml_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SKLAD],iceb_u_inttochar(E_SKLAD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SKLAD],gettext("Выбор склада"));

data.entry[E_SKLAD] = gtk_entry_new ();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SKLAD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.entry[E_SKLAD], TRUE, TRUE,1);
g_signal_connect(data.entry[E_SKLAD], "activate",G_CALLBACK(impmatdok_xml_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKLAD]),data.rk->sklad.ravno());
gtk_widget_set_name(data.entry[E_SKLAD],iceb_u_inttochar(E_SKLAD));



data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(gettext("Номер документа"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_NOMDOK],"clicked",G_CALLBACK(impmatdok_xml_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_NOMDOK],gettext("Получить номер документа"));

data.entry[E_NOMDOK] = gtk_entry_new ();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(impmatdok_xml_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk->nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));



data.knopka_enter[E_KOD_OP]=gtk_button_new_with_label(gettext("Код операции"));
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OP]), data.knopka_enter[E_KOD_OP], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KOD_OP],"clicked",G_CALLBACK(impmatdok_xml_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_OP],iceb_u_inttochar(E_KOD_OP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_OP],gettext("Выбор кода операции в справочнике операций"));

data.entry[E_KOD_OP] = gtk_entry_new ();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_OP]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_OP]), data.entry[E_KOD_OP], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KOD_OP], "activate",G_CALLBACK(impmatdok_xml_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_OP]),data.rk->kod_op.ravno());
gtk_widget_set_name(data.entry[E_KOD_OP],iceb_u_inttochar(E_KOD_OP));


data.knopka_enter[E_SHETU]=gtk_button_new_with_label(gettext("Счёт учёта материалов"));
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.knopka_enter[E_SHETU], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_SHETU],"clicked",G_CALLBACK(impmatdok_xml_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHETU],iceb_u_inttochar(E_SHETU));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHETU],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHETU] = gtk_entry_new ();
//gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHETU]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.entry[E_SHETU], TRUE, TRUE,1);
g_signal_connect(data.entry[E_SHETU], "activate",G_CALLBACK(impmatdok_xml_m_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETU]),data.rk->shetu.ravno());
gtk_widget_set_name(data.entry[E_SHETU],iceb_u_inttochar(E_SHETU));






sprintf(strsql,"F2 %s",gettext("Иморт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Импортировать налоговую накладную"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(impmatdok_xml_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введённой информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(impmatdok_xml_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Отказатся от импорта налоговой накладной"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(impmatdok_xml_m_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));/******************/
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[1]);

gtk_widget_show_all (data.window);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


return(data.voz);
}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  impmatdok_xml_v_e_knopka(GtkWidget *widget,class impmatdok_xml_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATA:

    if(iceb_calendar(&data->rk->datdok,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA]),data->rk->datdok.ravno());
      
    return;  

  case E_SKLAD:

    iceb_vibrek(1,"Sklad",&data->rk->sklad,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno());

    return;  

  case E_NOMDOK:
    data->rk->datdok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATA])));
    data->rk->sklad.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_SKLAD])));


    if(data->rk->datdok.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введена дата документа!"),data->window);
      return;
     }
    if(data->rk->sklad.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введён код склада!"),data->window);
      return;
     }
    if(data->rk->datdok.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Не правильно введена дата документа!"),data->window);
      return;
     }

    iceb_nomnak(data->rk->datdok.ravno(),data->rk->sklad.ravno(),&data->rk->nomdok,1,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk->nomdok.ravno());

    return;  

  case E_KOD_OP:
    iceb_vibrek(1,"Prihod",&data->rk->kod_op,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_OP]),data->rk->kod_op.ravno());

    return;

  case E_SHETU:
    iceb_vibrek(1,"Plansh",&data->rk->shetu,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),data->rk->shetu.ravno());

    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   impmatdok_xml_m_key_press(GtkWidget *widget,GdkEventKey *event,class impmatdok_xml_m_data *data)
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
void  impmatdok_xml_m_knopka(GtkWidget *widget,class impmatdok_xml_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek();
    if(impmatdok_xml_zap(data) == 0)
     {
      data->voz=0;
      gtk_widget_destroy(data->window);
     }
    return;  

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    impmatdok_xml_m_vvod(GtkWidget *widget,class impmatdok_xml_m_data *data)
{
char strsql[1024];

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_DATA:
    data->rk->datdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(data->rk->datdok.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
      return;
     }
    break;

  case E_SKLAD:
    data->rk->sklad.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));


    /*проверяем код склада*/
    sprintf(strsql,"select kod from Sklad where kod=%d",data->rk->sklad.ravno_atoi());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      iceb_menu_soob(gettext("Не найден код склада в справочнике!"),data->window);
      return;
     }    


    data->rk->datdok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATA])));
    if(data->rk->datdok.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
      return;
     }

    iceb_nomnak(data->rk->datdok.ravno(),data->rk->sklad.ravno(),&data->rk->nomdok,1,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk->nomdok.ravno());

    break;

  case E_KOD_OP:
    data->rk->kod_op.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
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
int impmatdok_xml_zap(class impmatdok_xml_m_data *data)
{
char strsql[2048];
SQL_str row;
class SQLCURSOR cur;

if(data->rk->datdok.prov_dat() != 0)
 {
  iceb_menu_soob(gettext("Не правильно введена дата!"),data->window);
  return(1);
 }
if(iceb_pbpds(data->rk->datdok.ravno(),data->window) != 0)
 return(1);
/*проверяем код склада*/
sprintf(strsql,"select kod from Sklad where kod=%d",data->rk->sklad.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  iceb_menu_soob(gettext("Не найден код склада в справочнике!"),data->window);
  return(1);
 }    

if(data->rk->nomdok.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён номер документа!"),data->window);
  return(1);
  
 }
if(data->rk->kod_op.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён код операции!"),data->window);
  return(1);
 }
/*проверяем код операции*/
sprintf(strsql,"select vido from Prihod where kod='%s'",data->rk->kod_op.ravno_filtr());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
 {
  iceb_menu_soob(gettext("Не найден код операции в справочнике!"),data->window);
  return(1);
 }

if(atoi(row[0]) != 0)
 {
  iceb_menu_soob(gettext("Не правильно выбрана операция! Тип операции должен быть \"Внешняя\""),data->window);
  return(1);
 }

if(data->rk->shetu.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён счёт учёта материалов!"),data->window);
  return(1);
 }

if(iceb_u_proverka(spmatshets.ravno(),data->rk->shetu.ravno(),1,1) != 0)
 {
  class iceb_u_str repl;
  repl.plus(gettext("Счёт не материальный !"));
  repl.ps_plus(data->rk->shetu.ravno());
  iceb_menu_soob(&repl,data->window);
  return(1);
 }

if(iceb_prsh1(data->rk->shetu.ravno(),data->window) != 0)
 return(1);

short dd=0,md=0,gd=0;
iceb_u_rsdat(&dd,&md,&gd,data->rk->datdok.ravno(),1);
short dk=dd;
iceb_u_dpm(&dk,&md,&gd,5);

/*проверяем может накладная с таким номером от этого поставщика в этом месяце уже была*/
sprintf(strsql,"select datd,sklad,nomd from Dokummat where datd >= '%04d-%02d-01' and datd <= '%04d-%02d-%02d' and nomnn='%s' and kontr='%s'",
gd,md,gd,md,dk,
data->rk->nom_nn.ravno(),
data->rk->kontr_prod.ravno());

if(iceb_sql_readkey(strsql,&row,&cur,data->window) >= 1)
 {
  sprintf(strsql,"%s %s %s %s %s %s %s %s\n%s",
  gettext("Номер налоговой накладной"),
  data->rk->nom_nn.ravno(),
  gettext("уже есть в документе"),
  row[2],
  gettext("склад"),
  row[1],
  gettext("дата"),
  iceb_u_datzap(row[0]),
  gettext("Подсистема \"Материальный учёт\""));

  iceb_menu_soob(strsql,data->window);

  return(1);
 }

sprintf(strsql,"select datd,podr,nomd from Usldokum where datd >= '%04d-%02d-01' and datd <= '%04d-%02d-%02d' and nomnn='%s' and kontr='%s'",
gd,md,gd,md,dk,
data->rk->nom_nn.ravno(),
data->rk->kontr_prod.ravno());

if(iceb_sql_readkey(strsql,&row,&cur,data->window) >= 1)
 {
  sprintf(strsql,"%s %s %s %s %s %s %s %s\n%s",
  gettext("Номер налоговой накладной"),
  data->rk->nom_nn.ravno(),
  gettext("уже есть в документе"),
  row[2],
  gettext("подразделение"),
  row[1],
  gettext("дата"),
  iceb_u_datzap(row[0]),
  gettext("Подсистема \"Учёт услуг\""));

  iceb_menu_soob(strsql,data->window);

  return(1);
 }


float pnds=iceb_pnds(data->rk->datdok.ravno(),data->window); /*основная ставка записывается в шапку документа*/

if(data->rk->nds == 4)
 {
  pnds=7.;
 }

if(data->rk->nds == 0 || data->rk->nds == 4) /*записывается в каждую запись документа*/
 data->rk->pnds=pnds;
else 
 data->rk->pnds=0.;

iceb_lock_tables lokt("LOCK TABLES Dokummat WRITE,icebuser READ");

/*Проверяем есть ли такой номер документа*/
sprintf(strsql,"select nomd from Dokummat where datd >= '%04d-01-01' and datd <= '%d-12-31' and sklad=%s and nomd='%s'",
data->rk->datdok.ravno_god(),data->rk->datdok.ravno_god(),data->rk->sklad.ravno(),data->rk->nomdok.ravno());
if(iceb_sql_readkey(strsql,data->window) >= 1)
  {
   sprintf(strsql,gettext("С номером %s документ уже есть !"),data->rk->nomdok.ravno());
   iceb_menu_soob(strsql,data->window);
   return(1);  
  }

sprintf(strsql,"insert into Dokummat (tip,datd,sklad,kontr,nomd,nomnn,kodop,ktoi,vrem,datpnn,pn,k00) values(%d,'%s',%d,'%s','%s','%s','%s',%d,%ld,'%s',%.2f,'%s')",
1,
data->rk->datdok.ravno_sqldata(),
data->rk->sklad.ravno_atoi(),
data->rk->kontr_prod.ravno_filtr(),
data->rk->nomdok.ravno_filtr(),
data->rk->nom_nn.ravno_filtr(),
data->rk->kod_op.ravno_filtr(),
iceb_getuid(data->window),
time(NULL),
data->rk->datdok.ravno_sqldata(),
pnds,
"00");

if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
 return(1);

lokt.unlock();/*разблокируем таблицы*/

if(data->rk->nds != 0 )
 {
  sprintf(strsql,"insert into Dokummat2 values (%d,%s,'%s',%d,%d)",
  data->rk->datdok.ravno_god(),data->rk->sklad.ravno(),data->rk->nomdok.ravno_filtr(),11,data->rk->nds);

  iceb_sql_zapis(strsql,1,0,data->window);

}

return(0);

}

