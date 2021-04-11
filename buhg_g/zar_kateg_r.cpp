/*$Id: zar_kateg_r.c,v 1.24 2013/09/26 09:47:00 sasa Exp $*/
/*17.05.2018	08.11.2006	Белых А.И.	zar_kateg_r.c
Расчёт свода начислений/удержаний по категориям
*/
#include <errno.h>
#include "buhg_g.h"
#include "zar_kateg.h"

class zar_kateg_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class zar_kateg_rek *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_kateg_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_kateg_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_kateg_r_data *data);
gint zar_kateg_r1(class zar_kateg_r_data *data);
void  zar_kateg_r_v_knopka(GtkWidget *widget,class zar_kateg_r_data *data);

int kategw(class zar_kateg_rek *data,FILE *kaw,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *bar,GtkWidget *wpredok);
int kategshw(class zar_kateg_rek *data,FILE *kaw,FILE *ffhoz,FILE *ffbu,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *bar,GtkWidget *wpredok);
void komplras(class zar_kateg_r_data *data,char imaf[],char *imafhoz,char *imafbu);

extern SQL_baza bd;

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_kateg_r(class zar_kateg_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zar_kateg_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать свод начислений и удержаний по категориям"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_kateg_r_key_press),&data);

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

repl.plus(gettext("Распечатать свод начислений и удержаний по категориям"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_kateg_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);

gtk_text_buffer_create_tag(data.buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zar_kateg_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_kateg_r_v_knopka(GtkWidget *widget,class zar_kateg_r_data *data)
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

gboolean   zar_kateg_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_kateg_r_data *data)
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

gint zar_kateg_r1(class zar_kateg_r_data *data)
{
time_t vremn;
time(&vremn);
class iceb_clock sss(data->window);

//printf("metka_ras=%d metka_vr=%d\n",data->rk->metka_ras,data->rk->metka_vr);

char imafhoz[64];
char imafbu[64];
char imaf[64];

memset(imafhoz,'\0',sizeof(imafhoz));
memset(imafhoz,'\0',sizeof(imafhoz));
memset(imaf,'\0',sizeof(imaf));

if(data->rk->metka_ras == 1)
 {
  sprintf(imaf,"kategk%d.lst",getpid());
  sprintf(imafhoz,"kategh%d.lst",getpid());
  sprintf(imafbu,"kategb%d.lst",getpid());
  komplras(data,imaf,imafhoz,imafbu);
 }
else
 {
  sprintf(imaf,"kateg%d.lst",getpid());
  FILE *kaw;
  if((kaw = fopen(imaf,"w")) == NULL)
   {
    iceb_er_op_fil(imaf,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  if(data->rk->metka_vr == 0)
    kategw(data->rk,kaw,data->view,data->buffer,data->bar,data->window);
  if(data->rk->metka_vr == 1)
   {
    FILE *ffhoz=NULL;
    FILE *ffbu=NULL;
    if(shetb != NULL)
     {
      sprintf(imafhoz,"kategh%d.lst",getpid());
      if((ffhoz = fopen(imafhoz,"w")) == NULL)
       {
        iceb_er_op_fil(imafhoz,"",errno,data->window);
        sss.clear_data();
        gtk_widget_destroy(data->window);
        return(FALSE);
       }

      sprintf(imafbu,"kategb%d.lst",getpid());
      if((ffbu = fopen(imafbu,"w")) == NULL)
       {
        iceb_er_op_fil(imafbu,"",errno,data->window);
        sss.clear_data();
        gtk_widget_destroy(data->window);
        return(FALSE);
       }
     }

     kategshw(data->rk,kaw,ffhoz,ffbu,data->view,data->buffer,data->bar,data->window);
    
    if(shetb != NULL)
     {
      fclose(ffhoz);

      fclose(ffbu);
     }
   }

  fclose(kaw);
 }


data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчёт начислений и удержаний по категориям"));

if(shetb != NULL && data->rk->metka_ras == 1)
 {
  data->rk->imaf.plus(imafhoz);
  data->rk->naimf.plus(gettext("Расчёт начислений и удержаний по категориям (хозрасчёт)"));
  data->rk->imaf.plus(imafbu);
  data->rk->naimf.plus(gettext("Расчёт начислений и удержаний по категориям (бюджет)"));

 }

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),1,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/**********************************************************/
/*Комплексний расчёт, данные для расчёта беруться из файла*/
/**********************************************************/
void komplras(class zar_kateg_r_data *data,
char imaf[],char *imafhoz,char *imafbu)
{
char		strsql[1024];
class iceb_u_str bros("");
short		metka;
FILE		*kaw=NULL,*ffhoz=NULL,*ffbu=NULL;
class iceb_u_str kompl("");
class iceb_u_str komplz("");

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
const char *imaf_alx={"zarpodrk.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,NULL);
  return;
 }


if((kaw = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return;
 }


if(shetb != NULL)
 {
  if((ffhoz = fopen(imafhoz,"w")) == NULL)
   {
    iceb_er_op_fil(imafhoz,"",errno,data->window);
    return;
   }

  if((ffbu = fopen(imafbu,"w")) == NULL)
   {
    iceb_er_op_fil(imafbu,"",errno,data->window);
    return;
   }
 }
data->rk->clear_nast();


metka=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],&kompl,1,':') == 0)
   {
    if(metka != 0)
     {
      if(metka > 1)
       fprintf(kaw,"\f");
      fprintf(kaw,"%s\n",komplz.ravno());       
      
      sprintf(strsql,"\n%s\n",komplz.ravno());       
      iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

      if(data->rk->metka_vr == 0)
       kategw(data->rk,kaw,data->view,data->buffer,data->bar,data->window);

      if(data->rk->metka_vr == 1)
       kategshw(data->rk,kaw,ffhoz,ffbu,data->view,data->buffer,data->bar,data->window);

      
      data->rk->clear_nast();
     }
    metka++;    
    komplz.new_plus(kompl.ravno());
   }

  if(iceb_u_polen(row_alx[0],&bros,1,'|') == 0) 
   if(iceb_u_SRAV("Подразделения",bros.ravno(),0) == 0)
     iceb_u_polen(row_alx[0],&data->rk->podr,2,'|');

  if(iceb_u_polen(row_alx[0],&bros,1,'|') == 0) 
   if(iceb_u_SRAV("Категории",bros.ravno(),0) == 0)
    iceb_u_polen(row_alx[0],&data->rk->kod_kat,2,'|');

  if(iceb_u_polen(row_alx[0],&bros,1,'|') == 0) 
   if(iceb_u_SRAV("Начисления",bros.ravno(),0) == 0)
    iceb_u_polen(row_alx[0],&data->rk->kod_nah,2,'|');

  if(iceb_u_polen(row_alx[0],&bros,1,'|') == 0) 
   if(iceb_u_SRAV("Удержания",bros.ravno(),0) == 0)
    iceb_u_polen(row_alx[0],&data->rk->kod_ud,2,'|');

 }
if(metka > 1)
 fprintf(kaw,"\f");

sprintf(strsql,"\n%s\n",komplz.ravno());       
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

fprintf(kaw,"%s\n",komplz.ravno());

if(data->rk->metka_vr == 0)
 kategw(data->rk,kaw,data->view,data->buffer,data->bar,data->window);

if(data->rk->metka_vr == 1)
 kategshw(data->rk,kaw,ffhoz,ffbu,data->view,data->buffer,data->bar,data->window);


fclose(kaw);

if(shetb != NULL)
 {
  fclose(ffhoz);

  fclose(ffbu);
 }
}
