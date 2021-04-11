/*$Id: usl_spismat_m.c,v 1.18 2013/09/26 09:46:58 sasa Exp $*/
/*13.07.2015	23.11.2005	Белых А.И.	usl_spismat_m.c
Меню для создания документа на расход в материальном учёте.
*/
#include "buhg_g.h"
#include "usl_spismat.h"
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
  E_DATAS,
  E_SKLAD,
  E_NOMDOK,
  E_KODOP,
  KOLENTER  
 };

class usl_spismat_m_data
 {
  public:
  usl_spismat_rek  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  usl_spismat_m_data() //Конструктор
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


gboolean   usl_spismat_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class usl_spismat_m_data *data);
void    usl_spismat_m_v_vvod(GtkWidget *widget,class usl_spismat_m_data *data);
void  usl_spismat_m_v_knopka(GtkWidget *widget,class usl_spismat_m_data *data);
void   usl_spismat_m_rekviz(class usl_spismat_m_data *data);
void usl_spismat_vibor_rek(class usl_spismat_m_data *data);
void  usl_spismat_m_v_e_knopka(GtkWidget *widget,class usl_spismat_m_data *data);

int usl_spismat_prov_rek(class usl_spismat_m_data *data);

extern SQL_baza bd;

int   usl_spismat_m(class usl_spismat_rek *rek_ras,GtkWidget *wpredok)
{

class usl_spismat_m_data data;

data.rk=rek_ras;

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Списание материаллов в подсистеме \"Материальный учёт\"."));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(usl_spismat_m_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }


GtkWidget *label=gtk_label_new(gettext("Списание материаллов в подсистеме \"Материальный учёт\"."));

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



sprintf(strsql,"%s (%s)",gettext("Дата документа"),gettext("д.м.г"));
data.knopka_enter[E_DATAS]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAS]), data.knopka_enter[E_DATAS], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAS],"clicked",G_CALLBACK(usl_spismat_m_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAS],iceb_u_inttochar(E_DATAS));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAS],gettext("Выбор даты."));

data.entry[E_DATAS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAS]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAS]), data.entry[E_DATAS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAS], "activate",G_CALLBACK(usl_spismat_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAS]),data.rk->datas.ravno());
gtk_widget_set_name(data.entry[E_DATAS],iceb_u_inttochar(E_DATAS));

sprintf(strsql,"%s",gettext("Склад"));
data.knopka_enter[E_SKLAD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.knopka_enter[E_SKLAD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SKLAD],"clicked",G_CALLBACK(usl_spismat_m_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SKLAD],iceb_u_inttochar(E_SKLAD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SKLAD],gettext("Выбор склада."));

data.entry[E_SKLAD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SKLAD]),100);
gtk_box_pack_start (GTK_BOX (hbox[E_SKLAD]), data.entry[E_SKLAD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SKLAD], "activate",G_CALLBACK(usl_spismat_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SKLAD]),data.rk->sklad.ravno());
gtk_widget_set_name(data.entry[E_SKLAD],iceb_u_inttochar(E_SKLAD));

sprintf(strsql,"%s",gettext("Номер документа"));
data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_NOMDOK],"clicked",G_CALLBACK(usl_spismat_m_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_NOMDOK],gettext("Получить номер документа."));

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),100);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(usl_spismat_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk->nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));

sprintf(strsql,"%s",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODOP],"clicked",G_CALLBACK(usl_spismat_m_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODOP],iceb_u_inttochar(E_KODOP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODOP],gettext("Выбор операции."));

data.entry[E_KODOP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODOP]),100);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODOP], "activate",G_CALLBACK(usl_spismat_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk->kodop.ravno());
gtk_widget_set_name(data.entry[E_KODOP],iceb_u_inttochar(E_KODOP));








sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт."));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(usl_spismat_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Просмотр нужных реквизитов."));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(usl_spismat_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации."));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(usl_spismat_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне."));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(usl_spismat_m_v_knopka),&data);
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
void  usl_spismat_m_v_e_knopka(GtkWidget *widget,class usl_spismat_m_data *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");
short d,m,g;
char strsql[512];

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATAS:

    if(iceb_calendar(&data->rk->datas,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAS]),data->rk->datas.ravno());
      
    return;  

  case E_KODOP:
     if(data->rk->tipz == 1)
      iceb_vibrek(1,"Prihod",&data->rk->kodop,data->window);
     if(data->rk->tipz == 2)
      iceb_vibrek(1,"Rashod",&data->rk->kodop,data->window);
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rk->kodop.ravno());

    
    return;  

  case E_SKLAD:

    iceb_vibrek(1,"Sklad",&data->rk->sklad,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno());

    return;  

  case E_NOMDOK:
    data->read_rek(); //Читаем реквизиты меню

    if(iceb_u_rsdat(&d,&m,&g,data->rk->datas.ravno(),1) != 0)
     {
      iceb_menu_soob(gettext("Не правильно введена дата !"),data->window);
      return;
     }
    if(data->rk->sklad.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введено склад !"),data->window);
      return;
     }

    //Проверяем склад
    sprintf(strsql,"select kod from Sklad where kod=%d",data->rk->sklad.ravno_atoi());
    if(iceb_sql_readkey(strsql,data->window) == 0)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден код склада"),data->rk->sklad.ravno_atoi());
      iceb_menu_soob(strsql,data->window);
      return;
     }         
    iceb_nomnak(g,data->rk->sklad.ravno(),&data->rk->nomdok,data->rk->tipz,0,0,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk->nomdok.ravno());

    return;  


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   usl_spismat_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class usl_spismat_m_data *data)
{

//printf("usl_spismat_m_v_key_press\n");
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
void  usl_spismat_m_v_knopka(GtkWidget *widget,class usl_spismat_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню

    if(usl_spismat_prov_rek(data) != 0)
     return;
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK3:
    usl_spismat_vibor_rek(data);
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

void    usl_spismat_m_v_vvod(GtkWidget *widget,class usl_spismat_m_data *data)
{
//short d,m,g;

int enter=atoi(gtk_widget_get_name(widget));
//g_print("usl_spismat_m_v_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_DATAS:
    data->rk->datas.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


  case E_KODOP:
    data->rk->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;



  case E_NOMDOK:
    data->rk->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SKLAD:
    data->rk->sklad.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/*******************/
/*выбрать реквизиты*/
/*********************/

void usl_spismat_vibor_rek(class usl_spismat_m_data *data)
{

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Выбор"));


zagolovok.plus(gettext("Выберите нужное."));


punkt_m.plus(gettext("Просмотр списка складов."));//0
punkt_m.plus(gettext("Просмотр списка операций расходов."));//2


int nomer=0;
nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);

switch(nomer)
 {
  case 0:
   iceb_vibrek(1,"Sklad",&data->rk->sklad,data->window);
   gtk_entry_set_text(GTK_ENTRY(data->entry[E_SKLAD]),data->rk->sklad.ravno());
   break;

  case 1:
     if(data->rk->tipz == 1)
      iceb_vibrek(1,"Prihod",&data->rk->kodop,data->window);
     if(data->rk->tipz == 2)
      iceb_vibrek(1,"Rashod",&data->rk->kodop,data->window);
   gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),data->rk->kodop.ravno());
   break;

 }
}
/*********************/
/*Проверка дат*/
/**********************/

int usl_spismat_prov_rek(class usl_spismat_m_data *data)
{

if(data->rk->datas.prov_dat() != 0)
 return(1);
char strsql[512];
//Проверяем склад
sprintf(strsql,"select kod from Sklad where kod=%d",data->rk->sklad.ravno_atoi());
if(iceb_sql_readkey(strsql,data->window) <= 0)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код склада"),data->rk->sklad.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
//Проверяем код операции
if(data->rk->tipz == 1)
  sprintf(strsql,"select kod from Prihod where kod='%s'",data->rk->kodop.ravno());
if(data->rk->tipz == 2)
  sprintf(strsql,"select kod from Rashod where kod='%s'",data->rk->kodop.ravno());
if(iceb_sql_readkey(strsql,data->window) <= 0)
 {
  sprintf(strsql,"%s %d !",gettext("Не найден код операции"),data->rk->sklad.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }
 
return(0);

}

