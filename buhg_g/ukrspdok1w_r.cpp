/*$Id: ukrspdok1w_r.c,v 1.12 2013/09/26 09:46:56 sasa Exp $*/
/*23.05.2016	29.02.2008	Белых А.И.	ukrspdok1w_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "ukrspdok1w.h"

class ukrspdok1w_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class ukrspdok1w_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  ukrspdok1w_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   ukrspdok1w_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrspdok1w_r_data *data);
gint ukrspdok1w_r1(class ukrspdok1w_r_data *data);
void  ukrspdok1w_r_v_knopka(GtkWidget *widget,class ukrspdok1w_r_data *data);


extern SQL_baza bd;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int ukrspdok1w_r(class ukrspdok1w_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class ukrspdok1w_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);
data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка списка документов под датам приказов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(ukrspdok1w_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

repl.plus(gettext("Распечатка списка документов по датам приказов"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,wpredok));
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(ukrspdok1w_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)ukrspdok1w_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  ukrspdok1w_r_v_knopka(GtkWidget *widget,class ukrspdok1w_r_data *data)
{
if(data->kon_ras == 1) 
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   ukrspdok1w_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrspdok1w_r_data *data)
{
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}


/******************************************/
/******************************************/

gint ukrspdok1w_r1(class ukrspdok1w_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;


short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(strsql,data->buffer,data->view);





SQL_str row1;
class SQLCURSOR cur1;
int kolstr=0;

sprintf(strsql,"select nomd,datd,kont,nompr,datp,vkom from Ukrdok where datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
order by datp,nompr asc",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    


FILE *ff;
char imaf[56];
sprintf(imaf,"ukrsp1%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_zagolov(gettext("Распечатка списка документов по датам приказов"),dn,mn,gn,dk,mk,gk,ff,data->window);

fprintf(ff,"\
-----------------------------------------------------------------\n");

fprintf(ff,"\
Дата прик.|Номер пр.|Дата докум|Номер док.|Контрагент\n");
/**********
1234567890 123456789 1234567890 1234567890
*****************/
fprintf(ff,"\
-----------------------------------------------------------------\n");

float kolstr1=0;
short dp,mp,gp;
short dd,md,gd;
class iceb_u_str naim_kontr("");
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->kontr.ravno(),row[2],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->vidkom.ravno(),row[3],0,0) != 0)
    continue;
  /*узнаём наименование контрагента*/
  naim_kontr.new_plus("");
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim_kontr.new_plus(row1[0]);
   
  iceb_u_rsdat(&dp,&mp,&gp,row[4],2);
  iceb_u_rsdat(&dd,&md,&gd,row[1],2);
  
  fprintf(ff,"%02d.%02d.%04d %-9s %02d.%02d.%04d %-*s %s %s\n",
  dp,mp,gp,row[3],dd,md,gd,
  iceb_u_kolbait(10,row[0]),row[0],
  row[2],
  naim_kontr.ravno());

 }


fprintf(ff,"\
-----------------------------------------------------------------\n");
iceb_podpis(ff,data->window);
fclose(ff);


data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Распечатка списка документов по датам приказов"));

for(int nomer=0; nomer < data->rk->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rk->imaf.ravno(nomer),0,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
