/*$Id: zar_srninu_r.c,v 1.17 2013/09/26 09:47:00 sasa Exp $*/
/*17.05.2018	29.11.2006	Белых А.И.	zar_srninu_r.c
список работников не имеющих начисления/удержания
*/
#include <errno.h>
#include "buhg_g.h"
#include "zar_srninu.h"

class zar_srninu_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class zar_srninu_rek *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_srninu_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_srninu_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_srninu_r_data *data);
gint zar_srninu_r1(class zar_srninu_r_data *data);
void  zar_srninu_r_v_knopka(GtkWidget *widget,class zar_srninu_r_data *data);


extern SQL_baza bd;

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_srninu_r(class zar_srninu_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zar_srninu_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать список работников не имеющих начисления/удержания"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_srninu_r_key_press),&data);

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

repl.plus(gettext("Распечатать список работников не имеющих начисления/удержания"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_srninu_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);

gtk_text_buffer_create_tag(data.buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);

//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zar_srninu_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_srninu_r_v_knopka(GtkWidget *widget,class zar_srninu_r_data *data)
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

gboolean   zar_srninu_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_srninu_r_data *data)
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

gint zar_srninu_r1(class zar_srninu_r_data *data)
{
//struct  tm      *bf;
time_t vremn;
time(&vremn);
//bf=localtime(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);

short mn,gn;
short mk,gk;
iceb_u_rsdat1(&mn,&gn,data->rk->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->rk->datak.ravno());
if(mk == 0)
 {
  mk=mn;
  gk=gn;
 }


sprintf(strsql,"select tabn,fio,podr from Kartb order by tabn asc");
class SQLCURSOR cur;
int kolstr=0;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);  
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf[64];
sprintf(imaf,"sn%d.lst",getpid());
FILE *kaw;
if((kaw = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
short dk=0;
iceb_u_dpm(&dk,&mk,&gk,5);
iceb_zagolov(gettext("Список работников неимеющих начисление/удержание"),1,mn,gn,dk,mk,gk,kaw,data->window);

int klst=0;
if(data->rk->podr.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Код подраздедения"),data->rk->podr.ravno());
  iceb_printcod(kaw,"Podr","kod","naik",0,data->rk->podr.ravno(),&klst);
 }
else
 fprintf(kaw,"%s\n",gettext("По всем подразделениям"));

if(data->rk->kod_nah.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Код начисления"),data->rk->kod_nah.ravno());
  iceb_printcod(kaw,"Nash","kod","naik",0,data->rk->kod_nah.ravno(),&klst);
 }
if(data->rk->kod_ud.ravno()[0] != '\0')
 {
  fprintf(kaw,"%s:%s\n",gettext("Код удержания"),data->rk->kod_ud.ravno());
  iceb_printcod(kaw,"Uder","kod","naik",0,data->rk->kod_ud.ravno(),&klst);
 }

fprintf(kaw,"-----------------------------------------------------------------------\n");


int kolp=0;
sprintf(strsql,"----------------------------------------------\n");
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
float kolstr1=0;
class SQLCURSOR cur1;
SQL_str row,row1;
int tabn;
char fio[512];
int kolstr2=0;
int metka=0;
int prn=0;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->podr.ravno(),row[2],0,0) != 0)
    continue;
  
  tabn=atol(row[0]);
  memset(fio,'\0',sizeof(fio));
  strncpy(fio,row[1],sizeof(fio)-1);
  
  sprintf(strsql,"select prn,knah from Zarp where \
datz >= '%d-%02d-01' and datz <= '%d-%02d-%02d' and tabn=%d",
  gn,mn,gk,mk,dk,tabn);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr2 == 0)
    continue;

  metka=0;
  while(cur1.read_cursor(&row1) != 0)
   {

    prn=atoi(row1[0]);

    if(prn == 1 && data->rk->kod_nah.ravno()[0] == '-')
       continue;
    if(prn == 1 && data->rk->kod_nah.ravno()[0] != '\0')
     if(iceb_u_proverka(data->rk->kod_nah.ravno(),row1[1],0,0) == 0)
   //  if(iceb_u_pole1(nahis,row1[1],',',0,&i1) == 0 || SRAV(nahis,row1[1],0) == 0)
      {
       metka++;        
       continue;
      }

    if(prn == 2 && data->rk->kod_ud.ravno()[0] == '-')
       continue;
       
    if(prn == 2 && data->rk->kod_ud.ravno()[0] != '\0')
     if(iceb_u_proverka(data->rk->kod_ud.ravno(),row1[1],0,0) == 0)
      {
       metka++;        
       continue;
      }
   }

  if(metka == 0)
   {
    sprintf(strsql,"%-6d %s\n",tabn,fio);
    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
    fprintf(kaw,"%-7d %s\n",tabn,fio);
    kolp++;
   }
 }

fprintf(kaw,"-----------------------------------------------------------------------\n");
fprintf(kaw,"%s: %d",
gettext("Количество работников"),kolp);

sprintf(strsql,"----------------------------------------------\n");
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"%s: %d\n",gettext("Количество работников"),kolp);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

iceb_podpis(kaw,data->window);
fclose(kaw);

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Список работников неимеющих начисление/удержание"));
iceb_ustpeh(imaf,3,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
