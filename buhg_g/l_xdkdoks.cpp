/*$Id: l_xdkdoks.c,v 1.34 2013/12/31 11:49:15 sasa Exp $*/
/*19.06.2019	06.04.2006	Белых А.И.	l_xdkdoks.c
Работа со списком платёжных документов
*/
#include <math.h>
#include "buhg_g.h"
#include "l_xdkdoks.h"
enum
{
 FK2,
// SFK2,
 FK3,
// SFK3,
 FK4,
 FK5,
// FK8,
 FK9,
 FK10,
 KOL_F_KL
};

enum
{
 COL_METKAZ,
 COL_DATAD,
 COL_KODOP,
 COL_NOMDOK,
 COL_KONTR,
 COL_NAIM_KONTR,
 COL_SUMA,
 COL_DATA_VREM,
 COL_KTO,  
 NUM_COLUMNS
};

class  xdkdoks_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *radiobutton[4];
  GtkWidget *radiobutton1[2];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей

  static class xdkdoks_rek poi;
  class iceb_u_str zapros;

  short ds,ms,gs; //Стартовая дата просмотра
  char  tablica[64];
    
  short dn,mn,gn;
  short dk,mk,gk;
  
  class iceb_u_str datav;
  class iceb_u_str nomdokv;
  short metka_sort_dat; 
  //Конструктор
  xdkdoks_data()
   {
    metka_sort_dat=0;
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    dn=mn=gn=dk=mk=gk=0;
   }      
 };
class xdkdoks_rek xdkdoks_data::poi;

gboolean   xdkdoks_key_press(GtkWidget *widget,GdkEventKey *event,class xdkdoks_data *data);
void xdkdoks_vibor(GtkTreeSelection *selection,class xdkdoks_data *data);
void xdkdoks_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class xdkdoks_data *data);
void  xdkdoks_knopka(GtkWidget *widget,class xdkdoks_data *data);
void xdkdoks_add_columns (GtkTreeView *treeview);
void xdkdoks_create_list(class xdkdoks_data *data);
int  xdkdoks_prov_row(SQL_str row,class xdkdoks_data *data);

void  l_xdkdoks_radio0(GtkWidget *widget,class xdkdoks_data *data);
void  l_xdkdoks_radio1(GtkWidget *widget,class xdkdoks_data *data);

int l_xdkdoks_p(class xdkdoks_rek *datap,char *tablica,GtkWidget *wpredok);

extern SQL_baza	bd;
extern short startgodd;
 
void l_xdkdoks(short ds,short ms,short gs, //Дата начала просмотра
const char *tablica,
GtkWidget *wpredok)
{
class xdkdoks_data data;
char bros[1024];
data.poi.metka_poi=0;  //0-нет поиска 1-есть поск
data.poi.metka_pros=0; //0-все 1-не подтверждённые 2-без проводок    
sprintf(bros,"01.01.%d",startgodd);
data.poi.datan.new_plus(bros);
data.ds=data.dn=ds;
data.ms=data.mn=ms;
if(gs != 0)
 data.gs=data.gn=gs;
else
 {
    
  data.gs=data.gn=startgodd;
  if(startgodd == 0)
   {
    short dt,mt,gt;
    iceb_u_poltekdat(&dt,&mt,&gt);
    data.gs=data.gn=gt;
    
   }
  data.mn=data.ms=1;
  data.dn=data.ds=1;
 }
strcpy(data.tablica,tablica);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

//gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

if(iceb_u_SRAV(data.tablica,"Pltp",0) == 0)
 sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список платёжных поручений"));
else
 sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Список платёжных требований"));

gtk_window_set_title (GTK_WINDOW (data.window),bros);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(xdkdoks_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_widget_show(hbox);


if(iceb_u_SRAV(data.tablica,"Pltp",0) == 0)
  data.label_kolstr=gtk_label_new (gettext("Список платёжных поручений"));
else
  data.label_kolstr=gtk_label_new (gettext("Список платёжных требований"));

gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.label_poisk=gtk_label_new ("");

gtk_box_pack_start (GTK_BOX (vbox2),data.label_poisk,FALSE, FALSE, 0);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

GtkWidget *hboxradio = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous(GTK_BOX(hboxradio),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

//Вставляем радиокнопки

data.radiobutton[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Все"));
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[0], TRUE, TRUE, 0);

g_signal_connect(data.radiobutton[0], "clicked",G_CALLBACK(l_xdkdoks_radio0),&data);

sprintf(bros,"\"?\"");
data.radiobutton[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton[0]),bros);
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[1], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton[1], "clicked",G_CALLBACK(l_xdkdoks_radio0),&data);

sprintf(bros,"\"*\"");
data.radiobutton[2]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton[1]),bros);
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[2], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton[2], "clicked",G_CALLBACK(l_xdkdoks_radio0),&data);

sprintf(bros,"\"#\"");
data.radiobutton[3]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton[2]),bros);
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[3], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton[3], "clicked",G_CALLBACK(l_xdkdoks_radio0),&data);

GtkWidget *separator1=gtk_separator_new(GTK_ORIENTATION_VERTICAL);
gtk_box_pack_start (GTK_BOX (hboxradio),separator1, TRUE, TRUE, 0);

/*вставляем вторую группу радиокнопок*/
data.radiobutton1[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("По возростанию дат"));
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton1[0], TRUE, TRUE, 0);

g_signal_connect(data.radiobutton1[0], "clicked",G_CALLBACK(l_xdkdoks_radio1),&data);

data.radiobutton1[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton1[0]),gettext("По убыванию дат"));
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton1[1], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton1[1], "clicked",G_CALLBACK(l_xdkdoks_radio1),&data);


gtk_box_pack_start(GTK_BOX (vbox2), hboxradio, FALSE,FALSE, 0);

gtk_widget_show_all(hboxradio);


//Кнопки


sprintf(bros,"F2 ?");
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(xdkdoks_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Снятие метки неподтверждённого документа"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);
/*************************
sprintf(bros,"SF2 %s",gettext("Метка"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(xdkdoks_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Снятие метки неподтверждённого документа"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);
****************************/
sprintf(bros,"F3 *");
data.knopka[FK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(xdkdoks_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Снятие метки документа без проводок"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);
/***************************
sprintf(bros,"SF3 %s",gettext("Метка"));
data.knopka[SFK3]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(xdkdoks_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Снятие метки документа без проводок"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));
gtk_widget_show(data.knopka[SFK3]);
***********************/
sprintf(bros,"F4 %s",gettext("Поиск"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(xdkdoks_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Поиск нужных записей"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F5 %s",gettext("Просмотр"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(xdkdoks_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Просмотр выбранного документа"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);
/****************************
sprintf(bros,"F8 #");
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK8], "clicked",G_CALLBACK(xdkdoks_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK8],gettext("Включить/выключить показ только документов помеченных для передчи в банк"));
gtk_widget_set_name(data.knopka[FK8],iceb_u_inttochar(FK8));
gtk_widget_show(data.knopka[FK8]);
********************/
sprintf(bros,"F9 %s",gettext("В банк"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK9], "clicked",G_CALLBACK(xdkdoks_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK9],gettext("Уcтановить/снять метку для передчи в банк"));
gtk_widget_set_name(data.knopka[FK9],iceb_u_inttochar(FK9));
gtk_widget_show(data.knopka[FK9]);


sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(xdkdoks_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

xdkdoks_create_list(&data);
gtk_widget_show(data.window);

//if(metka == 0)
  gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));


gtk_main();



if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  l_xdkdoks_radio0(GtkWidget *widget,class xdkdoks_data *data)
{
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton[0])) == TRUE)
 {
  if(data->poi.metka_pros == 0)
   return;
  data->poi.metka_pros=0;
 }
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton[1])) == TRUE)
 {
  if(data->poi.metka_pros == 1)
   return;
  data->poi.metka_pros=1;
 }
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton[2])) == TRUE)
 {
  if(data->poi.metka_pros == 2)
   return;
  data->poi.metka_pros=2;
 }
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton[3])) == TRUE)
 {
  if(data->poi.metka_pros == 3)
   return;
  data->poi.metka_pros=3;
 }

xdkdoks_create_list(data);

}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  l_xdkdoks_radio1(GtkWidget *widget,class xdkdoks_data *data)
{
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton1[0])) == TRUE)
 {
  if(data->metka_sort_dat == 0)
   return;
  data->metka_sort_dat=0;
 }
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton1[1])) == TRUE)
 {
  if(data->metka_sort_dat == 1)
   return;
  data->metka_sort_dat=1;
 }

xdkdoks_create_list(data);

}




/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  xdkdoks_knopka(GtkWidget *widget,class xdkdoks_data *data)
{
short dd,md,gd;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

int knop=atoi(gtk_widget_get_name(widget));
data->kl_shift=0; //Сбрасываем нажатый сшифт

switch (knop)
 {
/**********************
  case FK2:
    if(data->poi.metka_pros == 1)
      data->poi.metka_pros=0;
    else
      data->poi.metka_pros=1;  
     
    xdkdoks_create_list(data);
    return;  
********************/
  case FK2:
    if(iceb_menu_danet(gettext("Снять отметку неподтвеждённого документа ? Вы уверены ?"),2,data->window) == 2)
     return;     

    sprintf(strsql,"update %s set \
podt=1 \
where datd='%s' and nomd='%s'",
    data->tablica,data->datav.ravno_sqldata(),data->nomdokv.ravno());

    iceb_sql_zapis(strsql,0,0,data->window);

    xdkdoks_create_list(data);
    return;  
/*********************
  case FK3:
    if(data->poi.metka_pros == 2)
      data->poi.metka_pros=0;
    else
      data->poi.metka_pros=2;
    xdkdoks_create_list(data);
    return;  
**********************/
  case FK3:

    if(iceb_menu_danet(gettext("Снять отметку документа без проводок ? Вы уверены ?"),2,data->window) == 2)
     return;     

    sprintf(strsql,"update %s \
set \
prov=0 \
where datd='%s' and nomd='%s'",
    data->tablica,data->datav.ravno_sqldata(),data->nomdokv.ravno());

    iceb_sql_zapis(strsql,0,0,data->window);
    xdkdoks_create_list(data);
    return;  

  case FK4:

    l_xdkdoks_p(&data->poi,data->tablica,data->window);
    
    if(data->poi.metka_poi == 1)
     {
      if(data->poi.datan.getdlinna() > 1)
       iceb_rsdatp(&data->dn,&data->mn,&data->gn,data->poi.datan.ravno(),\
       &data->dk,&data->mk,&data->gk,data->poi.datak.ravno(),data->window);
     }
    else
     {
      data->dn=data->ds;
      data->mn=data->ms;
      data->gn=data->gs;
      
     }
    xdkdoks_create_list(data);
    return;  

  case FK5:
    iceb_u_rsdat(&dd,&md,&gd,data->datav.ravno(),1);

    readpdokw(data->tablica,gd,data->nomdokv.ravno(),data->window);

    xdkdok(data->tablica,data->window);
    /*При выходе из документа устанавливаются метки на документ*/
    xdkdoks_create_list(data);
    return;  
/**********************
  case FK8:
    data->poi.metka_pp++;

    if(data->poi.metka_pp > 1)
     data->poi.metka_pp=0;    

    xdkdoks_create_list(data);
    return;  
*****************/
   case FK9: /*Установить/снять метку для передачи в банк*/
      sprintf(strsql,"select vidpl from %s where datd='%s' and nomd='%s'",
      data->tablica,data->datav.ravno_sqldata(),data->nomdokv.ravno());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
       {
        iceb_menu_soob(gettext("Ненашли запись!"),data->window);
        return;
       }
      if(atoi(row[0]) == 1)
       sprintf(strsql,"update %s set vidpl='' where datd='%s' and nomd='%s'",
       data->tablica,data->datav.ravno_sqldata(),data->nomdokv.ravno());
      else
       sprintf(strsql,"update %s set vidpl='1' where datd='%s' and nomd='%s'",
       data->tablica,data->datav.ravno_sqldata(),data->nomdokv.ravno());

      iceb_sql_zapis(strsql,1,0,data->window);

      xdkdoks_create_list(data);

      return;  
  
  case FK10:
//    printf("xdkdoks_knopka F10\n");
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   xdkdoks_key_press(GtkWidget *widget,GdkEventKey *event,class xdkdoks_data *data)
{
iceb_u_str repl;
//printf("xdkdoks_key_press keyval=%d state=%d\n",event->keyval,event->state);


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

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");
    return(TRUE);
/*******************
  case GDK_KEY_F8:
    g_signal_emit_by_name(data->knopka[FK8],"clicked");
    return(TRUE);
*********************/
  case GDK_KEY_F9:
    g_signal_emit_by_name(data->knopka[FK9],"clicked");
    return(TRUE);

  case GDK_KEY_F11:
//    g_signal_emit_by_name(data->knopka[FK11],"clicked");
    return(TRUE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("xdkdoks_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
//    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void xdkdoks_create_list (class xdkdoks_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQL_str row;
char strsql[2048];
char bros[1024];
int  kolstr=0;
iceb_u_str zagolov;

//printf("xdkdoks_create_list %d\n",data->snanomer);
data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(xdkdoks_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(xdkdoks_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);


model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING,
G_TYPE_STRING, 
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_INT);

sprintf(bros,"datd,nomd,oper,suma,polu,podt,prov,ktoz,vrem,vidpl");

sprintf(strsql,"select %s from %s where datd >= '%d-01-01'",bros,data->tablica,data->gn);

if(data->dn != 0)
  sprintf(strsql,"select %s from %s where datd >= '%d-%02d-%02d'",bros,data->tablica,data->gn,data->mn,data->dn);

if(data->dn != 0 && data->dk != 0)
  sprintf(strsql,"select %s from %s where datd >= '%d-%02d-%02d' and datd <= '%d-%02d-%02d'",bros,data->tablica,data->gn,data->mn,data->dn,data->gk,data->mk,data->dk);

if(data->metka_sort_dat == 0)
 strcat(strsql," order by datd asc,nomd asc");
else
 strcat(strsql," order by datd desc,nomd asc");

data->zapros.new_plus(strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

iceb_u_str ss[NUM_COLUMNS];

data->kolzap=0;

float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
   if(xdkdoks_prov_row(row,data) != 0)
    continue;

  if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0)
   ss[COL_METKAZ].new_plus("-");
  else
   ss[COL_METKAZ].new_plus("+");

  if(row[5][0] == '0')
   ss[COL_METKAZ].plus("?");

  if(row[6][0] == '1')
   ss[COL_METKAZ].plus("*");

  if(row[9][0] == '1')
   ss[COL_METKAZ].plus("#");
  //Контрагент
  iceb_u_polen(row[4],strsql,sizeof(strsql),1,'#');
  ss[COL_KONTR].new_plus(strsql);
  
  //Наименование контрагента
  iceb_u_polen(row[4],strsql,sizeof(strsql),2,'#');
  ss[COL_NAIM_KONTR].new_plus(strsql);


  //Сумма по документа
  sprintf(strsql,"%10.2f",atof(row[3]));
  ss[COL_SUMA].new_plus(strsql);
  
  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_METKAZ,ss[COL_METKAZ].ravno(),
  COL_DATAD,iceb_u_datzap(row[0]),
  COL_NOMDOK,row[1],
  COL_KONTR,ss[COL_KONTR].ravno(),
  COL_NAIM_KONTR,ss[COL_NAIM_KONTR].ravno(),
  COL_SUMA,ss[COL_SUMA].ravno(),
  COL_KODOP,row[2],
  COL_DATA_VREM,iceb_u_vremzap(row[8]),
  COL_KTO,iceb_kszap(row[7],data->window),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

xdkdoks_add_columns (GTK_TREE_VIEW (data->treeview));


if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK5]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK9]),TRUE);//Доступна
 }

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

iceb_u_str strpoi;  
strpoi.plus("");

if(data->poi.metka_pros == 1)
 strpoi.new_plus(gettext("Просмотр только неподтверждённых документов"));
if(data->poi.metka_pros == 2)
 strpoi.new_plus(gettext("Просмотр только документов без проводок"));
if(data->poi.metka_pros == 3)
 strpoi.new_plus(gettext("Просмотр только помеченных для передачи в банк"));

if(data->poi.metka_poi == 1 )
 {

// printf("Формирование заголовка с реквизитами поиска.\n");

  if(strpoi.getdlinna() > 1)      
   strpoi.ps_plus(gettext("Поиск"));
  else
   strpoi.new_plus(gettext("Поиск"));
  strpoi.plus(" !!!");

  iceb_str_poisk(&strpoi,data->poi.nomdok.ravno(),gettext("Номер документа"));
  iceb_str_poisk(&strpoi,data->poi.datan.ravno(),gettext("Дата начала"));
  iceb_str_poisk(&strpoi,data->poi.datak.ravno(),gettext("Дата конца"));
  iceb_str_poisk(&strpoi,data->poi.kodop.ravno(),gettext("Код операции"));
  iceb_str_poisk(&strpoi,data->poi.suma.ravno(),gettext("Сумма"));
    
 }

if(strpoi.getdlinna() > 1)
 {
  iceb_label_set_text_color(data->label_poisk,strpoi.ravno(),"red");
  gtk_widget_show(data->label_poisk);
 }
else
  gtk_widget_hide(data->label_poisk); 


if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0)
  zagolov.new_plus(gettext("Список платёжных поручений"));
else
  zagolov.new_plus(gettext("Список платёжных требований"));

sprintf(strsql," %s:%d",gettext("Количество записей"),data->kolzap);
zagolov.plus(strsql);
zagolov.plus(" ");
zagolov.plus(gettext("Год"));
zagolov.plus(":");
zagolov.plus(data->gs);

sprintf(strsql,"%s:\"?\" %s \"*\" %s \"#\" %s",
gettext("Метки"),
gettext("не подтверждён"),
gettext("не сделаны проводки"),
gettext("передать в банк"));

zagolov.ps_plus(strsql);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),zagolov.ravno());

gtk_widget_show(data->label_kolstr);

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));

}
/*****************/
/*Создаем колонки*/
/*****************/

void xdkdoks_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Метка"), renderer,"text", COL_METKAZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Дата док-та"), renderer,"text",COL_DATAD,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Ном.док."), renderer,"text", COL_NOMDOK,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Операция"), renderer,"text", COL_KODOP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Сумма"), renderer,"text", COL_SUMA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Контрагент"), renderer,"text", COL_KONTR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Наименование контрагента"), renderer,"text", COL_NAIM_KONTR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);

//printf("xdkdoks_add_columns end\n");

}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void xdkdoks_vibor(GtkTreeSelection *selection,class xdkdoks_data *data)
{

GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *datad;
gchar *nomdok;

gint  nomer;


gtk_tree_model_get(model,&iter,COL_DATAD,&datad,COL_NOMDOK,&nomdok,NUM_COLUMNS,&nomer,-1);

data->datav.new_plus(datad);
data->nomdokv.new_plus(nomdok);

data->snanomer=nomer;

g_free(datad);
g_free(nomdok);

//printf("xdkdoks_vibor-%s %d\n",data->kodv.ravno(),data->snanomer);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void xdkdoks_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class xdkdoks_data *data)
{
//просмотр выбранного документа
g_signal_emit_by_name(data->knopka[FK5],"clicked");


}
/****************************/
/*Проверка записей          */
/*****************************/

int  xdkdoks_prov_row(SQL_str row,class xdkdoks_data *data)
{
short pro=atoi(row[6]);

if(data->poi.metka_pros == 3 && atoi(row[9]) != 1)
 return(1);
 
if(data->poi.metka_pros == 2 && pro == 0)
   return(1);
  
int pod=atoi(row[5]);
if(data->poi.metka_pros == 1 && pod == 1)
   return(1);

if(data->poi.metka_poi == 0)
 return(0);

//sprintf(bros,"kassa,tp,datd,nomd,shetk,kodop,prov,podt,fio,vrem,ktoi,god");
  

if(iceb_u_proverka(data->poi.kodop.ravno(),row[2],0,0) != 0)
 return(1);




if(data->poi.nomdok.getdlinna() > 1)
 if(iceb_u_SRAV(data->poi.nomdok.ravno(),row[1],1) != 0)
   return(1);

if(data->poi.suma.getdlinna() > 1)
 if(fabs(data->poi.suma.ravno_atof()-atof(row[3])) > 0.009)
  return(1);


  
return(0);
}
