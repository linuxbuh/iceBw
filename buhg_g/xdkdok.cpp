/*$Id: xdkdok.c,v 1.54 2013/08/25 08:26:59 sasa Exp $*/
/*05.01.2020	10.04.2006	Белых А.И.	xdkdok.c
Просмотр и редактирование платёжного документа
Если вернули 0- вышли по F10
             1- удалили документ

*/

//#define GORIZONTAL_RIS 970
//#define VERTIKAL_RIS  470
#define GORIZONTAL_RIS 960
#define VERTIKAL_RIS  400
#include <errno.h>
#include "buhg_g.h"
#include "dok4w.h"

enum
{
 FK2,
 FK3,
 FK4,
 FK5,
 FK6,
 FK7,
 SFK7,
 FK8,
 FK9,
 FK10,
 KOL_F_KL
};

class  xdkdok_data
 {
  public:
  GtkWidget *label_nadpis;
  GtkWidget *label_prov;

  GtkWidget *window;
  GtkWidget *risunok;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int voz;
    
  int metka_proris;  
  double sump; //Сумма подтверждения документа
  char tablica[64];
  iceb_u_str naim_regim;  
  xdkdok_data()
   {
    voz=0;
   }
 };

gboolean   xdkdok_key_press(GtkWidget *widget,GdkEventKey *event,class xdkdok_data *data);
void  xdkdok_knopka(GtkWidget *widget,class xdkdok_data *data);
gboolean xdkdok_draw(GtkWidget *widget,cairo_t *cr,class xdkdok_data *data);
void xdkdok_zagolovok(class xdkdok_data *data);

int xdkdok_v(char *tablica,GtkWidget *wpredok);
int udpldw(const char *tabl,short d,short m,short g,const char *npp,int mu,const char *kodop,int metkasoob,int tipz,GtkWidget *wpredok);
int zappdokw(char tabl[],GtkWidget *wpredok);

void l_prov_xdk(char *tablica,GtkWidget *wpredok);
int   podzapxdk(char *tablica,GtkWidget *wpredok);
void l_xdkpz(char *tablica,GtkWidget *wpredok);
void xdkdok_menu_ras(class xdkdok_data *data);
int platpw(char *imaf1,short kp,short nomk,FILE *ff,int viddok,GtkWidget *wpredok);
int vibshbnw(char *kontr,char *tabl,GtkWidget *wpredok);
void xdkdok_smsh(class xdkdok_data *data);

extern SQL_baza	bd;
extern class REC rec;
extern char *imafkomtmp;
extern short kp;    /*Количество повторов*/

int xdkdok(const char *tablica,GtkWidget *wpredok)
{

class  xdkdok_data data;
data.metka_proris=0;
char bros[512];

strcpy(data.tablica,tablica);

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_resizable(GTK_WINDOW(data.window),FALSE); /*запрет на изменение размеров окна*/

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

data.label_nadpis=gtk_label_new ("");
data.label_prov=gtk_label_new("");

xdkdok_zagolovok(&data); //формируем заголовок меню

gtk_window_set_title (GTK_WINDOW (data.window),data.naim_regim.ravno());
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);



g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(xdkdok_key_press),&data);
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




gtk_box_pack_start (GTK_BOX (vbox2),data.label_nadpis,FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox2),data.label_prov,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

//Создаём зону рисования
data.risunok=gtk_drawing_area_new();
gtk_widget_set_size_request (data.risunok, GORIZONTAL_RIS, VERTIKAL_RIS);
gtk_box_pack_end (GTK_BOX (vbox2), data.risunok, TRUE, TRUE, 0);
/*если не будет этой команды то в зоне рисунка ничего отображаться не будет*/
g_signal_connect(G_OBJECT(data.risunok),"draw",G_CALLBACK(xdkdok_draw),&data); /*для gtk2.0*/ 
//g_signal_connect(data.risunok,"draw",G_CALLBACK(xdkdok_draw),&data);   /*для gtk3.0*/

//Кнопки

sprintf(bros,"F2 %s",gettext("Корректировать"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK2],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Корректировать реквизиты документа"));
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(xdkdok_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(xdkdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удалить документ"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Проводки"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(xdkdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Переход в режим работы с проводками для этого документа"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

if(iceb_u_SRAV(tablica,"Tpltp",0) == 0 || iceb_u_SRAV(tablica,"Tpltt",0) == 0)
  gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK4]),FALSE);//Недоступна

sprintf(bros,"F5 %s",gettext("Печать"));
data.knopka[FK5]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(xdkdok_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Распечатка документов"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(bros,"F6 %s",gettext("Смена счёта"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(xdkdok_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Сменить счёт дебета"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Подтверждение"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK7], "clicked",G_CALLBACK(xdkdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Подтверждение документа"));
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));
gtk_widget_show(data.knopka[FK7]);

if(iceb_u_SRAV(tablica,"Tpltp",0) == 0 || iceb_u_SRAV(tablica,"Tpltt",0) == 0)
  gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK7]),FALSE);//Недоступна

sprintf(bros,"%sF7 %s",RFK,gettext("Подтверждение"));
data.knopka[SFK7]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[SFK7], "clicked",G_CALLBACK(xdkdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK7],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[SFK7],gettext("Работа с подтверждающими записями"));
gtk_widget_set_name(data.knopka[SFK7],iceb_u_inttochar(SFK7));
gtk_widget_show(data.knopka[SFK7]);

if(iceb_u_SRAV(tablica,"Tpltp",0) == 0 || iceb_u_SRAV(tablica,"Tpltt",0) == 0)
  gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[SFK7]),FALSE);//Недоступна

sprintf(bros,"F8 %s",gettext("Нов.докум."));
data.knopka[FK8]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK8], "clicked",G_CALLBACK(xdkdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK8],gettext("Сделать новый документ с использованием реквизитов этого документа"));
gtk_widget_set_name(data.knopka[FK8],iceb_u_inttochar(FK8));
gtk_widget_show(data.knopka[FK8]);

if(iceb_u_SRAV(tablica,"Tpltp",0) == 0 || iceb_u_SRAV(tablica,"Tpltt",0) == 0)
  gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK8]),FALSE);//Недоступна

sprintf(bros,"F9 %s",gettext("В банк"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK9], "clicked",G_CALLBACK(xdkdok_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK9],gettext("Установить метку предачи в банк"));
gtk_widget_set_name(data.knopka[FK9],iceb_u_inttochar(FK9));
gtk_widget_show(data.knopka[FK9]);

if(iceb_u_SRAV(tablica,"Tpltp",0) == 0 || iceb_u_SRAV(tablica,"Tpltt",0) == 0)
  gtk_widget_set_sensitive(GTK_WIDGET(data.knopka[FK9]),FALSE);//Недоступна

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(xdkdok_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);


gtk_widget_show_all(data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  xdkdok_knopka(GtkWidget *widget,class xdkdok_data *data)
{
char strsql[512];
short dt,mt,gt;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    if(xdkdok_v(data->tablica,data->window) == 0)
      if(rec.nomdk_i.getdlinna() > 1) //Корректировка уже записаного документа
       {
        if(udpldw(data->tablica,rec.ddi,rec.mdi,rec.gdi,rec.nomdk_i.ravno(),1,rec.kodop_i.ravno(),0,rec.tipz,data->window) == 0)
          zappdokw(data->tablica,data->window);
       }

    xdkdok_zagolovok(data);
    //Для прорисовки всей области рисунка а не только той зоны, которая была накрыта предыдущим меню
    gtk_widget_hide(data->risunok);
    gtk_widget_show(data->risunok);
    return;  


  case FK3:

     
     if(iceb_pbpds(rec.md,rec.gd,data->window) != 0)
      {
       return;
      } 
     
     if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0 || iceb_u_SRAV(data->tablica,"Pltt",0) == 0 )
      {
       int metka_ps=0;
       if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0)
        metka_ps=4;
       if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0)
        metka_ps=5;
       /*Смотрим заблокированы ли проводки к этому документа*/
       if(iceb_pbp(metka_ps,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),0,rec.tipz,gettext("Удалить документ невозможно!"),data->window) != 0)
         return;
      }

    
     if(iceb_pblok(rec.md,rec.gd,ICEB_PS_GK,data->window) != 0)
       {
        sprintf(strsql,gettext("На дату %d.%dг. проводки заблокированы ! Удаление невозможно !"),rec.md,rec.gd);
        iceb_menu_soob(strsql,data->window);
        return;
       }
    
     if(iceb_menu_danet(gettext("Удалить документ ? Вы уверены ?"),2,data->window) == 2)
      return;

     if(udpldw(data->tablica,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),0,rec.kodop.ravno(),1,rec.tipz,data->window) == 0)
      {
       data->voz=1;
       gtk_widget_destroy(data->window);
      }    
    return;  


  case FK4:
     if(rec.nomdk_i.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Документ не записан !"),data->window);
      return;
     }

    if(data->sump == 0.)
     {
      iceb_menu_soob(gettext("Документ не подтверждён !"),data->window);
      return;
     }

    l_prov_xdk(data->tablica,data->window);

    xdkdok_zagolovok(data);
    return;  

  case FK5:
    xdkdok_menu_ras(data);
    xdkdok_zagolovok(data); //обязательно
    return;  

  case FK6:
    xdkdok_smsh(data);
    xdkdok_zagolovok(data);
    //Для прорисовки всей области рисунка а не только той зоны, которая была накрыта предыдущим меню
    gtk_widget_hide(data->risunok);
    gtk_widget_show(data->risunok);
    return;  

  case FK7:
    podzapxdk(data->tablica,data->window);
    xdkdok_zagolovok(data);
    return;  

  case SFK7:
    l_xdkpz(data->tablica,data->window);
    xdkdok_zagolovok(data);
    return;  

  case FK8:
    iceb_u_poltekdat(&dt,&mt,&gt);
    
    sprintf(strsql,"%d",iceb_nomdok(gt,data->tablica,data->window));
    rec.nomdk.new_plus(strsql);
    
    rec.dd=dt;
    rec.md=mt;
    rec.gd=gt;
    
    rec.ddi=0;
    rec.mdi=0;
    rec.gdi=0;
    rec.nomdk_i.new_plus("");
    
    sprintf(strsql,"%s N%s %s:%02d.%02d.%d",
    gettext("Делаем новый документ !"),
    rec.nomdk.ravno(),
    gettext("Дата"),
    rec.dd,rec.md,rec.gd);
    iceb_menu_soob(strsql,data->window);

    xdkdok_zagolovok(data);
    //Для прорисовки всей области рисунка а не только той зоны, которая была накрыта предыдущим меню
    gtk_widget_hide(data->risunok);
    gtk_widget_show(data->risunok);
    
    return;  
  
  case FK9:
    if(iceb_u_SRAV(data->tablica,"Tpltp",0) == 0 || iceb_u_SRAV(data->tablica,"Tpltt",0) == 0)
     {
      iceb_menu_soob(gettext("Типовые документы не передаются !"),data->window);
      return;
     }
    
    if(rec.nomdk_i.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Документ не записан !"),data->window);
      return;
     }
    sprintf(strsql,"update %s set vidpl='1' where datd='%04d-%02d-%02d' and nomd='%s'",
    data->tablica,rec.gd,rec.md,rec.dd,rec.nomdk.ravno());
    iceb_sql_zapis(strsql,1,0,data->window);
    iceb_menu_soob(gettext("Документ отмечен для передачи в банк"),data->window);
//    klient(rec.nomdk.ravno(),rec.dd,rec.md,rec.gd,data->tablica,data->window);
    return;  

    
  case FK10:
  
    if(rec.ddi != 0 && (iceb_u_SRAV(data->tablica,"Pltp",0) == 0 || iceb_u_SRAV(data->tablica,"Pltt",0) == 0))
       provpdw(data->tablica,data->window);
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   xdkdok_key_press(GtkWidget *widget,GdkEventKey *event,class xdkdok_data *data)
{
iceb_u_str repl;
//printf("xdkdoks_key_press keyval=%d state=%d\n",event->keyval,event->state);

switch(event->keyval)
 {
  case GDK_KEY_F2:
//    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
//    if(data->kl_shift == 1)
//      g_signal_emit_by_name(data->knopka[SFK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
  //  if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK3],"clicked");
  //  if(data->kl_shift == 1)
  //    g_signal_emit_by_name(data->knopka[SFK3],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_F5:
    g_signal_emit_by_name(data->knopka[FK5],"clicked");
    return(TRUE);

  case GDK_KEY_F6:
    g_signal_emit_by_name(data->knopka[FK6],"clicked");
    return(TRUE);

  case GDK_KEY_F7:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK7],"clicked");
    if(data->kl_shift == 1)
      g_signal_emit_by_name(data->knopka[SFK7],"clicked");
    return(TRUE);

  case GDK_KEY_F8:
    g_signal_emit_by_name(data->knopka[FK8],"clicked");
    return(TRUE);

  case GDK_KEY_F9:
    g_signal_emit_by_name(data->knopka[FK9],"clicked");
    return(TRUE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("xdkdoks_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}


/********************************/
/*прорисовка платёжного документа*/
/*********************************/
gboolean xdkdok_draw1(GtkWidget *widget,cairo_t *cr,class xdkdok_data *data)
{
char strsql[2048];
char bros[1024];

//printf("xdkdok_draw-рисуем платёжку %f\n",rec.sumd);

short metka_tip_dok=0; //0-платёжное поручение 1-требование
if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0)
  metka_tip_dok=1;



//cairo_t *cr=gdk_cairo_create(gtk_widget_get_window(data->risunok));

/********************.
до версии 3.22 это работало 
для новой библиотеки указали переменную cairo_t в вызове подпрограммы
удалили в конце программы cairo_destroy (cr) так как мы его не создавали
этого оказалось достаточно чтобы всё работало 
*******************/

cairo_set_line_width (cr, 5); /*устанвливаем толщину линии*/
//Рисуем рамку чёрного цвета вокруг области рисования
cairo_rectangle (cr, 0, 0, GORIZONTAL_RIS, VERTIKAL_RIS);//Рисует прямоугольник
                                      
cairo_set_source_rgb (cr, 1., 1., 1.); /*белый*/
cairo_fill_preserve (cr);
                                             
cairo_set_source_rgb (cr, 0, 0, 0); /*чёрный*/
cairo_stroke (cr);  /*выталкиваем на экран линии*/




/*********************************
//рисуем прямоуголиник по размерам области рисования белым цветом с заливкой его белым
gdk_draw_rectangle(gtk_widget_get_window(widget),
widget->style->white_gc,
TRUE, //фон прямоугольника закрашивается цветом линии
0,
0,
GORIZONTAL_RIS-1,
VERTIKAL_RIS-1);
**************************************/
//Создаём новый графический контекст

//GdkGC *gc=gdk_gc_new(gtk_widget_get_window(widget));


/*********************************************************/
//Для начала создаём контекст Pango - PangoContext, который нужен для создания PangoLayout
PangoLayout *layout = NULL;
GdkScreen *screen = gdk_screen_get_default();
PangoContext *context = (PangoContext*)gdk_pango_context_get_for_screen(screen);

//Узнаём  шрифт по умолчанию для созданного контекста.
// шрифт
PangoFontDescription *desc = pango_context_get_font_description(context);
//PangoFontDescription *desc=pango_font_description_from_string("Sans 16"); так тоже можно но нужно в конце освоботить память

//Потом задаём характеристики шрифта - имя шрифта, стиль, жирность и размер.

pango_font_description_set_family(desc,"mono");
pango_font_description_set_size(desc,18*PANGO_SCALE);

//Затем создаём PangoLayout из созданного контекста,
layout = pango_layout_new(context);


//Устанавливаем толщину линии
//gdk_gc_set_line_attributes(gc,4,GDK_LINE_SOLID,GDK_CAP_BUTT,GDK_JOIN_MITER);
cairo_set_line_width (cr, 4);
cairo_stroke (cr);  /*выталкиваем на экран линии*/

if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0)
  sprintf(strsql,"%s N %s",gettext("ПЛАТЁЖНОЕ ПОРУЧЕНИЕ"),rec.nomdk.ravno());
if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0)
  sprintf(strsql,"%s N %s",gettext("ПЛАТЁЖНОЕ ТРЕБОВАНИЕ"),rec.nomdk.ravno());
if(iceb_u_SRAV(data->tablica,"Tpltp",0) == 0)
  sprintf(strsql,"%s N %s",gettext("ТИПОВОЕ ПЛАТЁЖНОЕ ПОРУЧЕНИЕ"),rec.nomdk.ravno());
if(iceb_u_SRAV(data->tablica,"Tpltt",0) == 0)
  sprintf(strsql,"%s N %s",gettext("ТИПОВОЕ ПЛАТЁЖНОЕ ТРЕБОВАНИЕ"),rec.nomdk.ravno());


//добавляем в него текст.
pango_layout_set_text(layout,strsql,-1);
//Остаётся только отобразить текст на экране:
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,190.,5.,layout);

//cairo_rel_move_to (cr,100.,100.); /*Переходим в точку*/
cairo_move_to(cr,190.,5.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);


pango_font_description_set_size(desc,12*PANGO_SCALE);
pango_layout_set_text(layout,"0410001",-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,695.,8.,layout);
cairo_move_to(cr,695.,8.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);


//gdk_draw_rectangle(gtk_widget_get_window(widget),gc,FALSE,690,7,80,20);
cairo_rectangle (cr,690.,7.,80.,20); /*рисуем прямоугольник*/


pango_font_description_set_size(desc,14*PANGO_SCALE);

if(rec.gd != 0)
 {
  iceb_mesc(rec.md,1,bros);

  sprintf(strsql,"%s %02d %s %d%s",gettext("от"),rec.dd,bros,rec.gd,gettext("г."));


  pango_layout_set_text(layout,strsql,-1);
//  gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,215.,25.,layout);
  cairo_move_to(cr,215.,25.); /*Переходим в точку*/
  pango_cairo_show_layout (cr, layout);
 }

pango_layout_set_text(layout,gettext("Получено банком"),-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,590.,25.,layout);
cairo_move_to(cr,590.,25.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);

memset(strsql,'\0',sizeof(strsql));
if(metka_tip_dok == 0)
 sprintf(strsql,"%s %s",gettext("Плательщик"),rec.naior.ravno());
if(metka_tip_dok == 1)
 sprintf(strsql,"%s %s",gettext("Плательщик"),rec.naior1.ravno());
sprintf(bros,"%.*s",iceb_u_kolbait(50,strsql),strsql);

pango_layout_set_text(layout,bros,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,55.,layout);
cairo_move_to(cr,5.,55.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);

if(iceb_u_strlen(strsql) > 50)
 {
  sprintf(bros,"%s",iceb_u_adrsimv(50,strsql));
  pango_layout_set_text(layout,bros,-1);
//  gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,75.,layout);
  cairo_move_to(cr,5.,75.); /*Переходим в точку*/
  pango_cairo_show_layout (cr, layout);
 }
sprintf(strsql,"\"___\"________%d%s",rec.gd,gettext("г."));

pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,580.,55.,layout);
cairo_move_to(cr,580.,55.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/


//Прямоугольник для кода плательщика
//gdk_draw_rectangle(gtk_widget_get_window(widget),gc,FALSE, 50,100,150,20);
cairo_rectangle (cr,50.,100.,150.,20); /*рисуем прямоугольник*/

pango_layout_set_text(layout,gettext("Код"),-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,98.,layout);
cairo_move_to(cr,5.,98.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/
cairo_stroke (cr);

if(metka_tip_dok == 0)
 strcpy(strsql,rec.kod.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.kod1.ravno());
 
pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,55.,98.,layout);
cairo_move_to(cr,55.,99.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

pango_layout_set_text(layout,gettext("Банк плательщика"),-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,118.,layout);
cairo_move_to(cr,5.,118.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/


pango_layout_set_text(layout,gettext("Код банка"),-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,370.,118.,layout);
cairo_move_to(cr,370.,118.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

sprintf(strsql,"%s %s N",gettext("ДЕБЕТ"),gettext("счёт"));

pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,500.,118.,layout);
cairo_move_to(cr,500.,118.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

pango_layout_set_text(layout,gettext("СУММА"),-1);
//cairo_move_to(cr,700.,118.); /*Переходим в точку*/
cairo_move_to(cr,850.,118.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/


memset(strsql,'\0',sizeof(strsql));
if(metka_tip_dok == 0)
  sprintf(strsql,"%s в %s",rec.naiban.ravno(),rec.gorod.ravno());
if(metka_tip_dok == 1)
  sprintf(strsql,"%s в %s",rec.naiban1.ravno(),rec.gorod1.ravno());

sprintf(bros,"%.*s",iceb_u_kolbait(30,strsql),strsql);

pango_layout_set_text(layout,bros,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,138.,layout);
cairo_move_to(cr,5.,138.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/


//Устанавливаем толщину линии
//gdk_gc_set_line_attributes(gc,2,GDK_LINE_SOLID,GDK_CAP_BUTT,GDK_JOIN_MITER);
cairo_set_line_width (cr, 2); /*устанвливаем толщину линии*/

//Линия под наименованием банка плательщика
//gdk_draw_line(gtk_widget_get_window(widget),gc,0,161,370,161);
cairo_move_to(cr,0.,161.); /*Переходим в точку*/
cairo_line_to (cr,370,161); /*рисуем линию*/
cairo_stroke (cr);  /*выталкиваем на экран линии*/

//Устанавливаем толщину линии
//gdk_gc_set_line_attributes(gc,4,GDK_LINE_SOLID,GDK_CAP_BUTT,GDK_JOIN_MITER);
cairo_set_line_width (cr, 4); /*устанвливаем толщину линии*/

//Прямоугольник для МФО плательщика
//gdk_draw_rectangle(gtk_widget_get_window(widget),gc,FALSE,370,140,100,20);
cairo_rectangle (cr,370,140,100,20);//Рисует прямоугольник
cairo_stroke (cr);  /*выталкиваем на экран линии*/


if(metka_tip_dok == 0)
 strcpy(strsql,rec.mfo.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.mfo1.ravno());
 
pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,375.,138.,layout);
cairo_move_to(cr,375.,139.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

//Прямоугольник для счёта плательщика
//gdk_draw_rectangle(gtk_widget_get_window(widget),gc,FALSE,480,140,170,20);
//cairo_rectangle (cr,480,140,170,20);//Рисует прямоугольник
cairo_rectangle (cr,480,140,340,20);//Рисует прямоугольник
cairo_stroke (cr);  /*выталкиваем на экран линии*/


if(metka_tip_dok == 0)
 strcpy(strsql,rec.nsh.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.nsh1.ravno());


pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,485.,138.,layout);
cairo_move_to(cr,485.,139.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

//Прямоугольник для суммы платежа
//gdk_draw_rectangle(gtk_widget_get_window(widget),gc,FALSE,650,140,140,105);
cairo_rectangle (cr,820,140,140,105);//Рисует прямоугольник
cairo_stroke (cr); /*выталкиваем на экран линии*/

/*печатаем сумму платежа*/
sprintf(strsql,"%.2f",rec.sumd);
pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,655.,138.,layout);
//cairo_move_to(cr,655.,139.); /*Переходим в точку*/
cairo_move_to(cr,825.,139.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/
memset(strsql,'\0',sizeof(strsql));
if(metka_tip_dok == 0)
  sprintf(strsql,"%s %s",gettext("Получатель"),rec.naior1.ravno());
if(metka_tip_dok == 1)
  sprintf(strsql,"%s %s",gettext("Получатель"),rec.naior.ravno());

sprintf(bros,"%.*s",iceb_u_kolbait(55,strsql),strsql);

pango_layout_set_text(layout,bros,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,158.,layout);
cairo_move_to(cr,5.,158.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

//Прямоугольник для кода получателя
//gdk_draw_rectangle(gtk_widget_get_window(widget),gc,FALSE,50,185,150,20);
cairo_rectangle (cr,50,185,150,20);//Рисует прямоугольник
cairo_stroke (cr);  /*выталкиваем на экран линии*/

pango_layout_set_text(layout,gettext("Код"),-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,183.,layout);
cairo_move_to(cr,5.,183.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"%s %s N",gettext("КРЕДИТ"),gettext("счёт"));

pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,490.,183.,layout);
cairo_move_to(cr,490.,183.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

memset(strsql,'\0',sizeof(strsql));
if(metka_tip_dok == 0)
 strcpy(strsql,rec.kod1.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.kod.ravno());


pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,55.,183.,layout);
cairo_move_to(cr,55.,184.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

pango_layout_set_text(layout,gettext("Банк получателя"),-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,203.,layout);
cairo_move_to(cr,5.,203.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

pango_layout_set_text(layout,gettext("Код банка"),-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,370.,203.,layout);
cairo_move_to(cr,370.,203.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/


memset(strsql,'\0',sizeof(strsql));
if(metka_tip_dok == 0)
  sprintf(strsql,"%s в %s",rec.naiban1.ravno(),rec.gorod1.ravno());
if(metka_tip_dok == 1)
  sprintf(strsql,"%s в %s",rec.naiban.ravno(),rec.gorod.ravno());

sprintf(bros,"%.*s",iceb_u_kolbait(30,strsql),strsql);
pango_layout_set_text(layout,bros,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,223.,layout);
cairo_move_to(cr,5.,223.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/


//Устанавливаем толщину линии
//gdk_gc_set_line_attributes(gc,2,GDK_LINE_SOLID,GDK_CAP_BUTT,GDK_JOIN_MITER);
cairo_set_line_width (cr, 2); /*устанвливаем толщину линии*/

//Линия под наименованием банка плательщика
//gdk_draw_line(gtk_widget_get_window(widget),gc,0,246,370,246);
cairo_move_to(cr,0.,246.); /*Переходим в точку*/
cairo_line_to (cr,370,246);
cairo_stroke (cr); /*выталкиваем на экран линии*/


//Устанавливаем толщину линии
//gdk_gc_set_line_attributes(gc,4,GDK_LINE_SOLID,GDK_CAP_BUTT,GDK_JOIN_MITER);
cairo_set_line_width (cr, 4); /*устанвливаем толщину линии*/

//Прямоугольник для МФО плательщика
//gdk_draw_rectangle(gtk_widget_get_window(widget),gc,FALSE,370,225,100,20);
cairo_rectangle (cr,370,225,100,20);//Рисует прямоугольник
cairo_stroke (cr); /*выталкиваем на экран линии*/

if(metka_tip_dok == 0)
 strcpy(strsql,rec.mfo1.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.mfo.ravno());
 
pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,375.,223.,layout);
cairo_move_to(cr,375.,224.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

//Прямоугольник для счёта за услуги банка
//gdk_draw_rectangle(gtk_widget_get_window(widget),gc,FALSE,480,225,170,20);
cairo_rectangle (cr,480,225,340,20);//Рисует прямоугольник

//Прямоугольник для счёта плательщика
//gdk_draw_rectangle(gtk_widget_get_window(widget),gc,FALSE,480,205,170,20);

cairo_rectangle (cr,480,205,340,20);//Рисует прямоугольник
cairo_stroke (cr);  /*выталкиваем на экран линии*/

if(metka_tip_dok == 0)
 strcpy(strsql,rec.nsh1.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.nsh.ravno());

pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,485.,203.,layout);
cairo_move_to(cr,485.,204.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

//Устанавливаем толщину линии
//gdk_gc_set_line_attributes(gc,2,GDK_LINE_SOLID,GDK_CAP_BUTT,GDK_JOIN_MITER);
cairo_set_line_width (cr, 2); /*устанвливаем толщину линии*/

//Прямоугольник для кода нерезидента
if(metka_tip_dok == 0)
 {
//  gdk_draw_rectangle(gtk_widget_get_window(widget),gc,FALSE,650,255,140,20);
  cairo_rectangle (cr,650,255,140,20);//Рисует прямоугольник
  cairo_stroke (cr); /*выталкиваем на экран линии*/

  pango_layout_set_text(layout,rec.kodnr.ravno(),-1);
//  gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,655.,253.,layout);
  cairo_move_to(cr,825.,253.); /*Переходим в точку*/
  pango_cairo_show_layout (cr, layout);/*выводим текст*/

 }

sprintf(strsql,"%s (%s)",gettext("Сумма"),gettext("словами"));

pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,248.,layout);
cairo_move_to(cr,5.,248.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

//Сумма словами
memset(strsql,'\0',sizeof(strsql));
iceb_u_preobr(rec.sumd+rec.uslb,strsql,0);

pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,268.,layout);
cairo_move_to(cr,5.,268.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

//Линия под cуммой словами

//gdk_draw_line(gtk_widget_get_window(widget),gc,0,290,600,290);
cairo_move_to(cr,0.,290.); /*Переходим в точку*/
cairo_line_to (cr,600,290); /*рисуем линию*/
cairo_stroke (cr);  /*выталкиваем на экран линии*/

pango_layout_set_text(layout,gettext("Назначение платежа"),-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,293.,layout);
cairo_move_to(cr,5.,293.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/


pango_font_description_set_size(desc,14*PANGO_SCALE);
int YY=313;

int kol_ps=iceb_u_pole2(rec.naz_plat.ravno(),'\n');
if(kol_ps == 0)
 {
  pango_layout_set_text(layout,rec.naz_plat.ravno(),-1);
//  gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,YY,layout);
  cairo_move_to(cr,5.,YY); /*Переходим в точку*/
  pango_cairo_show_layout (cr, layout);/*выводим текст*/
  YY+=20;
 }
else
  for(int ii=1; ii <= kol_ps; ii++)
   {
    if(ii > 4)
     break;

    iceb_u_polen(rec.naz_plat.ravno(),strsql,sizeof(strsql),ii,'\n');  
    pango_layout_set_text(layout,strsql,-1);
//    gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,YY,layout);
    cairo_move_to(cr,5.,YY); /*Переходим в точку*/
    pango_cairo_show_layout (cr, layout);/*выводим текст*/
    YY+=20;

   }
#if 0
###################################################33
pango_layout_set_text(layout,"М.П.",-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,20.,393.,layout);
cairo_move_to(cr,20.,393.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

sprintf(strsql,"%s______________",gettext("Подпись"));

pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,100.,393.,layout);
cairo_move_to(cr,100.,393.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

pango_layout_set_text(layout,gettext("Проведено банком"),-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,600.,393.,layout);
cairo_move_to(cr,600.,393.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

pango_layout_set_text(layout,"______________",-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,170.,413.,layout);
cairo_move_to(cr,170.,413.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

sprintf(strsql,"\"___\"________%d%s",rec.gd,gettext("г."));

pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,600.,413.,layout);
cairo_move_to(cr,600.,413.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

pango_layout_set_text(layout,gettext("подпись банка"),-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,625.,433.,layout);
cairo_move_to(cr,625.,443.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/
################################################
#endif
//Напоследок почистим память:
g_object_unref(context); 
g_object_unref(layout);



//cairo_destroy (cr);

return(TRUE);
}

/*****************************************************************************/

gboolean xdkdok_draw127(GtkWidget *widget,cairo_t *cr,class xdkdok_data *data)
{
char strsql[2048];
char bros[1024];

//printf("xdkdok_draw-рисуем платёжку %f\n",rec.sumd);

short metka_tip_dok=0; //0-платёжное поручение 1-требование
if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0)
  metka_tip_dok=1;



//cairo_t *cr=gdk_cairo_create(gtk_widget_get_window(data->risunok));

/********************.
до версии 3.22 это работало 
для новой библиотеки указали переменную cairo_t в вызове подпрограммы
удалили в конце программы cairo_destroy (cr) так как мы его не создавали
этого оказалось достаточно чтобы всё работало 
*******************/

cairo_set_line_width (cr, 5); /*устанвливаем толщину линии*/
//Рисуем рамку чёрного цвета вокруг области рисования
cairo_rectangle (cr, 0, 0, GORIZONTAL_RIS, VERTIKAL_RIS);//Рисует прямоугольник
                                      
cairo_set_source_rgb (cr, 1., 1., 1.); /*белый*/
cairo_fill_preserve (cr);
                                             
cairo_set_source_rgb (cr, 0, 0, 0); /*чёрный*/
cairo_stroke (cr);  /*выталкиваем на экран линии*/




/*********************************
//рисуем прямоуголиник по размерам области рисования белым цветом с заливкой его белым
gdk_draw_rectangle(gtk_widget_get_window(widget),
widget->style->white_gc,
TRUE, //фон прямоугольника закрашивается цветом линии
0,
0,
GORIZONTAL_RIS-1,
VERTIKAL_RIS-1);
**************************************/
//Создаём новый графический контекст

//GdkGC *gc=gdk_gc_new(gtk_widget_get_window(widget));


/*********************************************************/
//Для начала создаём контекст Pango - PangoContext, который нужен для создания PangoLayout
PangoLayout *layout = NULL;
GdkScreen *screen = gdk_screen_get_default();
PangoContext *context = (PangoContext*)gdk_pango_context_get_for_screen(screen);

//Узнаём  шрифт по умолчанию для созданного контекста.
// шрифт
PangoFontDescription *desc = pango_context_get_font_description(context);
//PangoFontDescription *desc=pango_font_description_from_string("Sans 16"); так тоже можно но нужно в конце освоботить память

//Потом задаём характеристики шрифта - имя шрифта, стиль, жирность и размер.

pango_font_description_set_family(desc,"mono");
pango_font_description_set_size(desc,18*PANGO_SCALE);

//Затем создаём PangoLayout из созданного контекста,
layout = pango_layout_new(context);


//Устанавливаем толщину линии
//gdk_gc_set_line_attributes(gc,4,GDK_LINE_SOLID,GDK_CAP_BUTT,GDK_JOIN_MITER);
cairo_set_line_width (cr, 4);
cairo_stroke (cr);  /*выталкиваем на экран линии*/

if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0)
  sprintf(strsql,"%s N %s",gettext("ПЛАТЁЖНОЕ ПОРУЧЕНИЕ"),rec.nomdk.ravno());
if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0)
  sprintf(strsql,"%s N %s",gettext("ПЛАТЁЖНОЕ ТРЕБОВАНИЕ"),rec.nomdk.ravno());
if(iceb_u_SRAV(data->tablica,"Tpltp",0) == 0)
  sprintf(strsql,"%s N %s",gettext("ТИПОВОЕ ПЛАТЁЖНОЕ ПОРУЧЕНИЕ"),rec.nomdk.ravno());
if(iceb_u_SRAV(data->tablica,"Tpltt",0) == 0)
  sprintf(strsql,"%s N %s",gettext("ТИПОВОЕ ПЛАТЁЖНОЕ ТРЕБОВАНИЕ"),rec.nomdk.ravno());


//добавляем в него текст.
pango_layout_set_text(layout,strsql,-1);
//Остаётся только отобразить текст на экране:
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,190.,5.,layout);

//cairo_rel_move_to (cr,100.,100.); /*Переходим в точку*/
cairo_move_to(cr,190.,5.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);


pango_font_description_set_size(desc,12*PANGO_SCALE);
pango_layout_set_text(layout,"0410001",-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,695.,8.,layout);
cairo_move_to(cr,695.,8.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);


//gdk_draw_rectangle(gtk_widget_get_window(widget),gc,FALSE,690,7,80,20);
cairo_rectangle (cr,690.,7.,80.,20); /*рисуем прямоугольник*/


pango_font_description_set_size(desc,14*PANGO_SCALE);

if(rec.gd != 0)
 {
  iceb_mesc(rec.md,1,bros);

  sprintf(strsql,"%s %02d %s %d%s",gettext("от"),rec.dd,bros,rec.gd,gettext("г."));


  pango_layout_set_text(layout,strsql,-1);
//  gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,215.,25.,layout);
  cairo_move_to(cr,215.,25.); /*Переходим в точку*/
  pango_cairo_show_layout (cr, layout);
 }

pango_layout_set_text(layout,gettext("Получено банком"),-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,590.,25.,layout);
cairo_move_to(cr,590.,25.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);

memset(strsql,'\0',sizeof(strsql));
if(metka_tip_dok == 0)
 sprintf(strsql,"%s %s",gettext("Плательщик"),rec.naior.ravno());
if(metka_tip_dok == 1)
 sprintf(strsql,"%s %s",gettext("Плательщик"),rec.naior1.ravno());
sprintf(bros,"%.*s",iceb_u_kolbait(50,strsql),strsql);

pango_layout_set_text(layout,bros,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,55.,layout);
cairo_move_to(cr,5.,55.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);

if(iceb_u_strlen(strsql) > 50)
 {
  sprintf(bros,"%s",iceb_u_adrsimv(50,strsql));
  pango_layout_set_text(layout,bros,-1);
//  gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,75.,layout);
  cairo_move_to(cr,5.,75.); /*Переходим в точку*/
  pango_cairo_show_layout (cr, layout);
 }
sprintf(strsql,"\"___\"________%d%s",rec.gd,gettext("г."));

pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,580.,55.,layout);
cairo_move_to(cr,580.,55.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/


//Прямоугольник для кода плательщика
//gdk_draw_rectangle(gtk_widget_get_window(widget),gc,FALSE, 50,100,150,20);
cairo_rectangle (cr,50.,100.,150.,20); /*рисуем прямоугольник*/

pango_layout_set_text(layout,gettext("Код"),-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,98.,layout);
cairo_move_to(cr,5.,98.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/
cairo_stroke (cr);

if(metka_tip_dok == 0)
 strcpy(strsql,rec.kod.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.kod1.ravno());
 
pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,55.,98.,layout);
cairo_move_to(cr,55.,99.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

pango_layout_set_text(layout,gettext("Банк плательщика"),-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,118.,layout);
cairo_move_to(cr,5.,118.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/


sprintf(strsql,"%s %s N",gettext("ДЕБЕТ"),gettext("счёт"));

pango_layout_set_text(layout,strsql,-1);
//cairo_move_to(cr,500.,118.); /*Переходим в точку*/
cairo_move_to(cr,375.,118.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

pango_layout_set_text(layout,gettext("СУММА"),-1);
//cairo_move_to(cr,850.,118.); /*Переходим в точку*/
cairo_move_to(cr,720.,118.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/


memset(strsql,'\0',sizeof(strsql));
if(metka_tip_dok == 0)
  sprintf(strsql,"%s в %s",rec.naiban.ravno(),rec.gorod.ravno());
if(metka_tip_dok == 1)
  sprintf(strsql,"%s в %s",rec.naiban1.ravno(),rec.gorod1.ravno());

sprintf(bros,"%.*s",iceb_u_kolbait(30,strsql),strsql);

pango_layout_set_text(layout,bros,-1);
cairo_move_to(cr,5.,138.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/


//Устанавливаем толщину линии
cairo_set_line_width (cr, 2); /*устанвливаем толщину линии*/

//Линия под наименованием банка плательщика
cairo_move_to(cr,0.,161.); /*Переходим в точку*/
cairo_line_to (cr,370,161); /*рисуем линию*/
cairo_stroke (cr);  /*выталкиваем на экран линии*/

//Устанавливаем толщину линии
cairo_set_line_width (cr, 4); /*устанвливаем толщину линии*/

//Прямоугольник для счёта плательщика
//cairo_rectangle (cr,480,140,340,20);//Рисует прямоугольник
cairo_rectangle (cr,370,140,340,20);//Рисует прямоугольник
cairo_stroke (cr);  /*выталкиваем на экран линии*/


if(metka_tip_dok == 0)
 strcpy(strsql,rec.nsh.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.nsh1.ravno());


pango_layout_set_text(layout,strsql,-1);
//cairo_move_to(cr,485.,139.); /*Переходим в точку*/
cairo_move_to(cr,375.,139.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

//Прямоугольник для суммы платежа
//cairo_rectangle (cr,820,140,140,105);//Рисует прямоугольник
cairo_rectangle (cr,710,140,140,105);//Рисует прямоугольник
cairo_stroke (cr); /*выталкиваем на экран линии*/

/*печатаем сумму платежа*/
sprintf(strsql,"%.2f",rec.sumd);
pango_layout_set_text(layout,strsql,-1);
//cairo_move_to(cr,825.,139.); /*Переходим в точку*/
cairo_move_to(cr,715.,139.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

memset(strsql,'\0',sizeof(strsql));
if(metka_tip_dok == 0)
  sprintf(strsql,"%s %s",gettext("Получатель"),rec.naior1.ravno());
if(metka_tip_dok == 1)
  sprintf(strsql,"%s %s",gettext("Получатель"),rec.naior.ravno());

sprintf(bros,"%.*s",iceb_u_kolbait(55,strsql),strsql);

pango_layout_set_text(layout,bros,-1);
cairo_move_to(cr,5.,158.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

//Прямоугольник для кода получателя
cairo_rectangle (cr,50,185,150,20);//Рисует прямоугольник
cairo_stroke (cr);  /*выталкиваем на экран линии*/

pango_layout_set_text(layout,gettext("Код"),-1);
cairo_move_to(cr,5.,183.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"%s %s N",gettext("КРЕДИТ"),gettext("счёт"));

pango_layout_set_text(layout,strsql,-1);
//cairo_move_to(cr,490.,183.); /*Переходим в точку*/
cairo_move_to(cr,375.,203.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

memset(strsql,'\0',sizeof(strsql));
if(metka_tip_dok == 0)
 strcpy(strsql,rec.kod1.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.kod.ravno());


pango_layout_set_text(layout,strsql,-1);
cairo_move_to(cr,55.,184.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

pango_layout_set_text(layout,gettext("Банк получателя"),-1);
cairo_move_to(cr,5.,203.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

memset(strsql,'\0',sizeof(strsql));
if(metka_tip_dok == 0)
  sprintf(strsql,"%s в %s",rec.naiban1.ravno(),rec.gorod1.ravno());
if(metka_tip_dok == 1)
  sprintf(strsql,"%s в %s",rec.naiban.ravno(),rec.gorod.ravno());

sprintf(bros,"%.*s",iceb_u_kolbait(30,strsql),strsql);
pango_layout_set_text(layout,bros,-1);
cairo_move_to(cr,5.,223.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/


//Устанавливаем толщину линии
cairo_set_line_width (cr, 2); /*устанвливаем толщину линии*/

//Линия под наименованием банка плательщика
cairo_move_to(cr,0.,246.); /*Переходим в точку*/
cairo_line_to (cr,370,246);
cairo_stroke (cr); /*выталкиваем на экран линии*/


//Устанавливаем толщину линии
cairo_set_line_width (cr, 4); /*устанвливаем толщину линии*/

//Прямоугольник для счёта за услуги банка
//cairo_rectangle (cr,480,225,340,20);//Рисует прямоугольник
cairo_rectangle (cr,370,225,340,20);//Рисует прямоугольник
cairo_stroke (cr);  /*выталкиваем на экран линии*/

if(metka_tip_dok == 0)
 strcpy(strsql,rec.nsh1.ravno());
if(metka_tip_dok == 1)
 strcpy(strsql,rec.nsh.ravno());

pango_layout_set_text(layout,strsql,-1);
//cairo_move_to(cr,485.,204.); /*Переходим в точку*/
cairo_move_to(cr,375.,224.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

//Устанавливаем толщину линии
cairo_set_line_width (cr, 2); /*устанвливаем толщину линии*/

//Прямоугольник для кода нерезидента
if(metka_tip_dok == 0)
 {
//  gdk_draw_rectangle(gtk_widget_get_window(widget),gc,FALSE,650,255,140,20);
  cairo_rectangle (cr,650,255,140,20);//Рисует прямоугольник
  cairo_stroke (cr); /*выталкиваем на экран линии*/

  pango_layout_set_text(layout,rec.kodnr.ravno(),-1);
  cairo_move_to(cr,825.,253.); /*Переходим в точку*/
  pango_cairo_show_layout (cr, layout);/*выводим текст*/

 }

sprintf(strsql,"%s (%s)",gettext("Сумма"),gettext("словами"));

pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,248.,layout);
cairo_move_to(cr,5.,248.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

//Сумма словами
memset(strsql,'\0',sizeof(strsql));
iceb_u_preobr(rec.sumd+rec.uslb,strsql,0);

pango_layout_set_text(layout,strsql,-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,268.,layout);
cairo_move_to(cr,5.,268.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/

//Линия под cуммой словами

//gdk_draw_line(gtk_widget_get_window(widget),gc,0,290,600,290);
cairo_move_to(cr,0.,290.); /*Переходим в точку*/
cairo_line_to (cr,600,290); /*рисуем линию*/
cairo_stroke (cr);  /*выталкиваем на экран линии*/

pango_layout_set_text(layout,gettext("Назначение платежа"),-1);
//gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,293.,layout);
cairo_move_to(cr,5.,293.); /*Переходим в точку*/
pango_cairo_show_layout (cr, layout);/*выводим текст*/


pango_font_description_set_size(desc,14*PANGO_SCALE);
int YY=313;

int kol_ps=iceb_u_pole2(rec.naz_plat.ravno(),'\n');
if(kol_ps == 0)
 {
  pango_layout_set_text(layout,rec.naz_plat.ravno(),-1);
//  gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,YY,layout);
  cairo_move_to(cr,5.,YY); /*Переходим в точку*/
  pango_cairo_show_layout (cr, layout);/*выводим текст*/
  YY+=20;
 }
else
  for(int ii=1; ii <= kol_ps; ii++)
   {
    if(ii > 4)
     break;

    iceb_u_polen(rec.naz_plat.ravno(),strsql,sizeof(strsql),ii,'\n');  
    pango_layout_set_text(layout,strsql,-1);
//    gdk_draw_layout(GDK_DRAWABLE(data->risunok->window),gc,5.,YY,layout);
    cairo_move_to(cr,5.,YY); /*Переходим в точку*/
    pango_cairo_show_layout (cr, layout);/*выводим текст*/
    YY+=20;

   }
//Напоследок почистим память:
g_object_unref(context); 
g_object_unref(layout);

return(TRUE);
}
/****************************************************************/
gboolean xdkdok_draw(GtkWidget *widget,cairo_t *cr,class xdkdok_data *data)
{

if(iceb_u_sravmydat(13,1,2020,rec.dd,rec.md,rec.gd) <= 0)
 return(xdkdok_draw127(widget,cr,data));
else
 return(xdkdok_draw1(widget,cr,data));

}

/*************************/
/*Формирование заголовка */
/*************************/
void xdkdok_zagolovok(class xdkdok_data *data)
{
short dp=0,mp=0,gp=0;
double suma_pos_podt=0.;

data->sump=sumpzpdw(data->tablica,&dp,&mp,&gp,&suma_pos_podt,data->window);

data->naim_regim.new_plus("");
if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0)
  data->naim_regim.plus(gettext("Работа с платёжным поручением"));
if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0)
  data->naim_regim.plus(gettext("Работа с платёжным требованием"));
if(iceb_u_SRAV(data->tablica,"Тpltp",0) == 0)
  data->naim_regim.plus(gettext("Работа с типовым платёжным поручением"));
if(iceb_u_SRAV(data->tablica,"Tpltt",0) == 0)
  data->naim_regim.plus(gettext("Работа с типовым платёжным требованием"));

iceb_u_str sapka;
sapka.plus(data->naim_regim.ravno());
sapka.ps_plus(gettext("Операция"));
sapka.plus(":");
sapka.plus(rec.kodop.ravno());

sapka.plus(" ");

sapka.plus(gettext("НДС"));
sapka.plus(":");
sapka.plus(rec.nds);

sapka.plus(" ");
sapka.plus(gettext("Количество копий"));
sapka.plus(":");
sapka.plus(kp);

sapka.plus(" ");
sapka.plus(gettext("Контрагенты"));
sapka.plus(":");
sapka.plus(rec.kodor.ravno());
sapka.plus("/");
sapka.plus(rec.kodor1.ravno());

sapka.ps_plus(gettext("Подтверждённая сумма"));
sapka.plus(":");
sapka.plus(data->sump);

gtk_label_set_text(GTK_LABEL(data->label_nadpis),sapka.ravno());

if(rec.prov == 0)
  gtk_label_set_text(GTK_LABEL(data->label_prov),"");
else
  iceb_label_set_text_color(data->label_prov,gettext("Проводки не сделаны !"),"red");

}

/***************************/
/*Меню распечатки*/
/*************************/
void xdkdok_menu_ras(class xdkdok_data *data)
{
int kom=0;
iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Распечатка"));

punkt_m.plus(gettext("Распечатка на системном принтере"));//0
punkt_m.plus(gettext("Запись в журнал регистрации"));//1
punkt_m.plus(gettext("Запись распечатки в файл"));//2
punkt_m.plus(gettext("Просмотр перед печатью"));//3

char strsql[1024];
zagolovok.new_plus(iceb_get_pnk("00",0,data->window));
zagolovok.ps_plus(gettext("Распечатка"));



kom=iceb_menu_mv(&titl,&zagolovok,&punkt_m,kom,data->window);

if(kom == -1)
 return;


/*Проверяем номер платежного поручения*/
if(iceb_u_SRAV(rec.nomdk.ravno(),rec.nomdk_i.ravno(),0) != 0)
 {

  sprintf(strsql,"select nomd from %s where datd >= '%04d-01-01' and \
datd <= '%04d-12-31' and nomd='%s'",data->tablica,rec.gd,rec.gd,rec.nomdk.ravno());   

  if(iceb_sql_readkey(strsql,data->window) >= 1)
   {
    sprintf(strsql,gettext("С номером %s документ уже есть !"),rec.nomdk.ravno());
    iceb_menu_soob(strsql,data->window);
    return;
    
   }
 }


if(iceb_pbpds(rec.md,rec.gd,data->window) != 0)
  goto vpr;


//Удаляем предыдущий документ 
if(rec.nomdk_i.getdlinna() > 1)
 {
  udpldw(data->tablica,rec.ddi,rec.mdi,rec.gdi,rec.nomdk_i.ravno(),1,rec.kodop_i.ravno(),0,rec.tipz,data->window);
 }   


zappdokw(data->tablica,data->window);
    
if(kom == 1)
  return;
      
vpr:;

char imaf[64];
FILE *ff;
    
sprintf(imaf,"pp%d.lst",rec.nomdk.ravno_atoi());
    
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }


for(int kolp=0; kolp <kp ; kolp++)
 {
  
  if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0 || iceb_u_SRAV(data->tablica,"Tpltp",0) == 0)
   if(platpw(imafkomtmp,kp,kolp,ff,0,data->window) != 0)
    {
     fclose(ff);
     iceb_ustpeh(imaf,3,data->window);
     return;
    }
  if(iceb_u_SRAV(data->tablica,"Pltt",0) == 0 || iceb_u_SRAV(data->tablica,"Tpltt",0) == 0)
   if(platpw(imafkomtmp,kp,kolp,ff,1,data->window) != 0)
    {
     fclose(ff);
     iceb_ustpeh(imaf,3,data->window);
     return;
    }
 }

fclose(ff);
iceb_ustpeh(imaf,3,data->window);
   
if(kom == 2)
 {
//  sprintf(strsql,"%s:\n%s/%s",gettext("Распечатка выгружена в файл"),iceb_u_getcurrentdir(),imaf);
//  iceb_menu_soob(strsql,data->window);
  class iceb_u_spisok imafil;
  class iceb_u_spisok naimfil;
  imafil.plus(imaf);
  naimfil.plus(gettext("Распечатка платёжного документа"));
  iceb_rabfil(&imafil,&naimfil,data->window);
  return;
 }


if(kom == 3) //Просмотр перед печатью
  {
   iceb_prospp(imaf,data->window);
   return;
  }

//iceb_print(imaf,data->window);
iceb_print_operation(GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,imaf,data->window);


}
/**********************************/
/*Смена счёта*/
/******************************/
void xdkdok_smsh(class xdkdok_data *data)
{
char strsql[512];
char kontr[64];
memset(kontr,'\0',sizeof(kontr));

if(vibshbnw(kontr,data->tablica,data->window) == 0)
 {
  SQL_str row;
  SQLCURSOR cur;
  
  sprintf(strsql,"select naikon,kod,naiban,mfo,nomsh,adresb from \
Kontragent where kodkon='%s'",kontr);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr);
    iceb_menu_soob(strsql,data->window);
    return;
   }
  else
   {
    if(iceb_u_SRAV(data->tablica,"Pltp",0) == 0 || iceb_u_SRAV(data->tablica,"Tpltp",0) == 0)
     {
      rec.kodor.new_plus(kontr);

//      if(iceb_u_pole(row[0],rec.naior,1,'(') != 0)    
//       strcpy(rec.naior,row[0]);
      
      if(iceb_u_pole(row[0],strsql,1,'(') != 0)    
       strcpy(strsql,row[0]);
      rec.naior.new_plus(strsql);
            
      rec.kod.new_plus(row[1]);
      rec.naiban.new_plus(row[2]);
      rec.mfo.new_plus(row[3]);
      rec.nsh.new_plus(row[4]);
      rec.gorod.new_plus(row[5]);
     }
    else
     {
      rec.kodor1.new_plus(kontr);

      if(iceb_u_pole(row[0],strsql,1,'(') != 0)    
       strcpy(strsql,row[0]);
      rec.naior1.new_plus(strsql);
      
      rec.kod1.new_plus(row[1]);
      rec.naiban1.new_plus(row[2]);
      rec.mfo1.new_plus(row[3]);
      rec.nsh1.new_plus(row[4]);
      rec.gorod1.new_plus(row[5]);
     }
   }


 }


}
