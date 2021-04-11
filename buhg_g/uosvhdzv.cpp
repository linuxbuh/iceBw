/*$Id: uosvhdzv.c,v 1.17 2013/09/26 09:46:57 sasa Exp $*/
/*13.07.2015	19.10.2007	Белых А.И.	uosvhdzv.c
Ввод шапки документа для операций смены стоимости
Если вернули 0-шапка записана
             1-шапка не записана
*/
#include "buhg_g.h"
#include "uosvhd.h"
enum
{
  FK2,
  FK3,
  FK10,
  KOL_F_KL
};

enum
 {
  /*Обязательные реквизиты для заполнения*/
  E_DATADOK,
  E_NOMDOK,
  E_PODR,
  E_MAT_OT,
  E_OSNOV,
  KOLENTER  
 };

class uosvhdzv_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *opt;
  GtkWidget *window;
  GtkWidget *label_podr;
  GtkWidget *label_kod_op;
  GtkWidget *label_mat_ot;
  short kl_shift;
  short voz;      //0-шапка записана расчёт 1 нет

  /*Реквизиты шапки документа*/
  class uosvhd_data rk;
  uosvhdzv_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    rk.datadok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATADOK])));
    rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK])));
    rk.podr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PODR])));
    rk.mat_ot.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MAT_OT])));
    rk.osnov.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_OSNOV])));
   }

 };



gboolean   uosvhdzv_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class uosvhdzv_m_data *data);
void    uosvhdzv_m_v_vvod(GtkWidget *widget,class uosvhdzv_m_data *data);
void  uosvhdzv_m_v_knopka(GtkWidget *widget,class uosvhdzv_m_data *data);
void   uosvhdzv_m_rekviz(class uosvhdzv_m_data *data);
void  uosvhdzv_v_e_knopka(GtkWidget *widget,class uosvhdzv_m_data *data);


extern SQL_baza bd;

int uosvhdzv(class iceb_u_str *datadok,
class iceb_u_str *nomdok,
int tipz,
const char *kod_op, /*Код операции для нового документа*/
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
class uosvhdzv_m_data data;
char strsql[512];
class iceb_u_str naim_podr("");
class iceb_u_str naim_mat_ot("");
class iceb_u_str naim_kod_op("");

data.rk.datadok_z.new_plus(datadok->ravno());
data.rk.datadok.new_plus(datadok->ravno());
data.rk.nomdok.new_plus(nomdok->ravno());
data.rk.nomdok_z.new_plus(nomdok->ravno());
data.rk.kod_op.new_plus(kod_op);
data.rk.tipz=tipz;
data.rk.kontr.new_plus("00");
data.rk.kontr_z.new_plus("00");

if(data.rk.datadok.getdlinna() > 1)
 {
  sprintf(strsql,"select tipz,kodop,podr,kodol,osnov,nds,pn  from Uosdok where datd='%s' and nomd='%s'",data.rk.datadok.ravno_sqldata(),
  data.rk.nomdok.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найден документ"));
    repl.plus(" !!!");

    sprintf(strsql,"%s:%s %s:%s",
    gettext("Дата"),
    data.rk.datadok.ravno(),
    gettext("Номер документа"),
    data.rk.nomdok.ravno());
    
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);  
    
    return(1);
   } 

  if(atoi(row[0]) != tipz)
   {
    iceb_menu_soob("Не соовпадает тип документа !",wpredok);
    return(1);
   }
  data.rk.kod_op.new_plus(row[1]);

  data.rk.podr.new_plus(row[2]);
  data.rk.podr_z.new_plus(row[2]);
  
  data.rk.mat_ot.new_plus(row[3]);
  data.rk.mat_ot_z.new_plus(row[3]);
  
  data.rk.osnov.new_plus(row[4]);
  data.rk.lnds=atoi(row[5]);
  data.rk.pnds=atof(row[6]);  


  sprintf(strsql,"select naik from Uospod where kod=%d",data.rk.podr.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_podr.new_plus(row[0]);

  sprintf(strsql,"select naik from Uosol where kod=%d",data.rk.mat_ot.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
    naim_mat_ot.new_plus(row[0]);

 }
if(data.rk.tipz == 1)
 sprintf(strsql,"select naik from Uospri where kod='%s'",data.rk.kod_op.ravno());
if(data.rk.tipz == 2)
 sprintf(strsql,"select naik from Uosras where kod='%s'",data.rk.kod_op.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  naim_kod_op.new_plus(row[0]);


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Работа с шапкой документа"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(uosvhdzv_m_v_key_press),&data);


if(wpredok != NULL)
 {
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
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }



gtk_container_add (GTK_CONTAINER (data.window), vbox);
//gtk_container_add (GTK_CONTAINER (vbox), label);


GtkWidget *label=NULL;
if(data.rk.tipz == 1)
 label=gtk_label_new(gettext("Приход"));
if(data.rk.tipz == 2)
 label=gtk_label_new(gettext("Расход"));
gtk_box_pack_start (GTK_BOX (vbox),label, FALSE, FALSE,1);

sprintf(strsql,"%s:%s %s (%s)",gettext("Операция"),
data.rk.kod_op.ravno(),naim_kod_op.ravno(),gettext("Смена стоимости"));

data.label_kod_op=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (vbox),data.label_kod_op, FALSE, FALSE,1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], FALSE, FALSE,1);
//  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

//gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, FALSE, FALSE,1);


sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("д.м.г"));
data.knopka_enter[E_DATADOK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATADOK]), data.knopka_enter[E_DATADOK], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATADOK],"clicked",G_CALLBACK(uosvhdzv_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATADOK],iceb_u_inttochar(E_DATADOK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATADOK],gettext("Выбор даты"));

data.entry[E_DATADOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATADOK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATADOK]), data.entry[E_DATADOK], FALSE, FALSE,1);
g_signal_connect(data.entry[E_DATADOK], "activate",G_CALLBACK(uosvhdzv_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATADOK]),data.rk.datadok.ravno());
gtk_widget_set_name(data.entry[E_DATADOK],iceb_u_inttochar(E_DATADOK));

sprintf(strsql,"%s",gettext("Номер документа"));
data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_NOMDOK],"clicked",G_CALLBACK(uosvhdzv_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_NOMDOK],gettext("Получение номера документа"));

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], FALSE, FALSE,1);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(uosvhdzv_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk.nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));


sprintf(strsql,"%s",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(uosvhdzv_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PODR],gettext("Выбор подразделения"));

data.entry[E_PODR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PODR]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(uosvhdzv_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk.podr.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));

data.label_podr=gtk_label_new(naim_podr.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.label_podr, TRUE, TRUE,1);


sprintf(strsql,"%s",gettext("Код материально-ответственного"));
data.knopka_enter[E_MAT_OT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_MAT_OT]), data.knopka_enter[E_MAT_OT], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_MAT_OT],"clicked",G_CALLBACK(uosvhdzv_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_MAT_OT],iceb_u_inttochar(E_MAT_OT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_MAT_OT],gettext("Выбор материально-ответственного"));

data.entry[E_MAT_OT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MAT_OT]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_MAT_OT]), data.entry[E_MAT_OT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_MAT_OT], "activate",G_CALLBACK(uosvhdzv_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MAT_OT]),data.rk.mat_ot.ravno());
gtk_widget_set_name(data.entry[E_MAT_OT],iceb_u_inttochar(E_MAT_OT));

data.label_mat_ot=gtk_label_new(naim_mat_ot.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_MAT_OT]), data.label_mat_ot, TRUE, TRUE,1);



sprintf(strsql,"%s",gettext("Основание"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_OSNOV]), label, FALSE, FALSE, 0);

data.entry[E_OSNOV] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_OSNOV]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_OSNOV]), data.entry[E_OSNOV], TRUE, TRUE,1);
g_signal_connect(data.entry[E_OSNOV], "activate",G_CALLBACK(uosvhdzv_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_OSNOV]),data.rk.osnov.ravno());
gtk_widget_set_name(data.entry[E_OSNOV],iceb_u_inttochar(E_OSNOV));





sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(uosvhdzv_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F3 %s",gettext("Операция"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Выбор операции"));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(uosvhdzv_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(uosvhdzv_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 {
  datadok->new_plus(data.rk.datadok.ravno());
  nomdok->new_plus(data.rk.nomdok.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  uosvhdzv_v_e_knopka(GtkWidget *widget,class uosvhdzv_m_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
switch (knop)
 {
  case E_DATADOK:

    if(iceb_calendar(&data->rk.datadok,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATADOK]),data->rk.datadok.ravno());
      
    return;  

  case E_NOMDOK:

     data->rk.datadok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATADOK])));
     if(data->rk.datadok.ravno_god() == 0)
      {
       iceb_menu_soob(gettext("Не введена дата документа !"),data->window);
       return;
      }
    uosgetnd(data->rk.datadok.ravno_god(),data->rk.tipz,&data->rk.nomdok,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk.nomdok.ravno());
      
    return;  

  case E_PODR:
    if(l_uospodr(1,&kod,&naim,data->window) == 0)
     {
      data->rk.podr.new_plus(kod.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_podr),naim.ravno(20));
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk.podr.ravno());
     }
      
    return;  

  case E_MAT_OT:
    if(l_uosmo(1,&kod,&naim,data->window) == 0)
     {
      data->rk.mat_ot.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_MAT_OT]),data->rk.mat_ot.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_mat_ot),naim.ravno(20));
     }      
    return;  



 }
}


/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosvhdzv_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class uosvhdzv_m_data *data)
{

//printf("uosvhdzv_m_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
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
void  uosvhdzv_m_v_knopka(GtkWidget *widget,class uosvhdzv_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("uosvhdzv_m_v_knopka knop=%d\n",knop);
class iceb_u_str kod("");
class iceb_u_str naim("");

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(uosprhd(&data->rk,data->window) != 0)
     return;

    if(uoszaphd(&data->rk,data->window) != 0)
     return;
     
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK3:
    if(data->rk.tipz == 1)
     {
      char strsql[512];
      //Узнаём тип операции
      SQL_str row;
      class SQLCURSOR cur;
      int vido1=0;
      int vido2=0;
      sprintf(strsql,"select vido from Uospri where kod='%s'",data->rk.kod_op.ravno());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       vido1=atoi(row[0]);

      if(l_uosopp(1,&kod,&naim,data->window) == 0)
       {
        sprintf(strsql,"select vido from Uospri where kod='%s'",kod.ravno());
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
         vido2=atoi(row[0]);
       
        if(vido1 != vido2)
         {
          iceb_menu_soob(gettext("Такая замена невозможна !"),data->window);
          return;
         }
        sprintf(strsql,"%s:%s %s (%s)",
        gettext("Операция"),
        kod.ravno(),
        naim.ravno(),
        gettext("Смены стоимости"));
        data->rk.kod_op.new_plus(kod.ravno());
        gtk_label_set_text(GTK_LABEL(data->label_kod_op),strsql);
         
       }
     }
    return;
    
  case FK10:
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    uosvhdzv_m_v_vvod(GtkWidget *widget,class uosvhdzv_m_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//g_print("uosvhdzv_m_v_vvod enter=%d\n",enter);


switch (enter)
 {
  case E_DATADOK:
    data->rk.datadok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(data->rk.datadok.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата!"),data->window);
      return;
     }

    data->rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK])));
    if(data->rk.nomdok.getdlinna() <= 1)
     {
      uosgetnd(data->rk.datadok.ravno_god(),data->rk.tipz,&data->rk.nomdok,data->window);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk.nomdok.ravno());
     }
    break;

  case E_NOMDOK:
    data->rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PODR:
    data->rk.podr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_MAT_OT:
    data->rk.mat_ot.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_OSNOV:
    data->rk.osnov.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;



 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
