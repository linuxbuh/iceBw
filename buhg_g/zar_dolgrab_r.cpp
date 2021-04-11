/*$Id: zar_dolgrab_r.c,v 1.19 2013/09/26 09:46:59 sasa Exp $*/
/*17.05.2018	05.12.2006	Белых А.И.	zar_dolgrab_r.c
*/

#include <errno.h>
#include "buhg_g.h"

class zar_dolgrab_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  const char *data_d;
  class iceb_u_spisok *imaf;
  class iceb_u_spisok *naimf;
    
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_dolgrab_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_dolgrab_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_dolgrab_r_data *data);
gint zar_dolgrab_r1(class zar_dolgrab_r_data *data);
void  zar_dolgrab_r_v_knopka(GtkWidget *widget,class zar_dolgrab_r_data *data);

extern SQL_baza bd;

extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_dolgrab_r(const char *data_d,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zar_dolgrab_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);

data.data_d=data_d;
data.imaf=imaf;
data.naimf=naimf;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать долги работников за месяц"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_dolgrab_r_key_press),&data);

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

repl.plus(gettext("Распечатать долги работников за месяц"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_dolgrab_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);

gtk_text_buffer_create_tag(data.buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zar_dolgrab_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_dolgrab_r_v_knopka(GtkWidget *widget,class zar_dolgrab_r_data *data)
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

gboolean   zar_dolgrab_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_dolgrab_r_data *data)
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

gint zar_dolgrab_r1(class zar_dolgrab_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short mr,gr;
iceb_u_rsdat1(&mr,&gr,data->data_d);


char imaf[54];
FILE *kaw;

sprintf(imaf,"spidol%d.lst",getpid());

if((kaw = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  return(FALSE);
 }

iceb_zagolov(gettext("Список должников"),0,0,0,0,mr,gr,kaw,data->window);



fprintf(kaw,"\
----------------------------------------------------\n");
sprintf(strsql,"select tabn from Zarn where god=%d and mes=%d",
gr,mr);
int kolstr=0;
SQLCURSOR cur;
SQLCURSOR curr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  return(FALSE);
 }

class SQLCURSOR cur1;

int kol=0;
double itogb=0.,itognb=0.,itogo=0.,sum=0.;
float kolstr1=0;
int tabb=0;
SQL_str row,row1;
double sald=0.,saldb=0.;
int kolstr2=0;
double sumnb,sumb;
double sm=0.;
class iceb_u_str fio("");
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  tabb=atoi(row[0]);

  sum=0.;
  /*Читаем сальдо*/
  sum=sald=zarsaldw(1,mr,gr,tabb,&saldb,data->window);

  sprintf(strsql,"select suma,prn,shet from Zarp where datz >= '%d-%02d-01' and \
datz <= '%d-%02d-31' and tabn=%d",
  gr,mr,gr,mr,tabb);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  sumnb=sald-saldb;
  sumb=saldb;

  while(cur1.read_cursor(&row1) != 0)
   {
    sm=atof(row1[0]);
    sum+=sm;
    if(shetbu != NULL && shetb != NULL)
     {
      if(row1[1][0] == '1')
       {
       if(iceb_u_proverka(shetb,row1[2],0,0) == 0)
        sumb+=sm;
       else
        sumnb+=sm;
       }
      if(row1[1][0] == '2')
       {
       if(iceb_u_proverka(shetbu,row1[2],0,0) == 0)
        sumb+=sm;
       else
        sumnb+=sm;
       }
     }
   }
   
  if(sum < -0.009 && shetbu == NULL) 
   {
   
    sprintf(strsql,"select fio from Kartb where tabn=%d",tabb);
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
      fio.new_plus(row1[0]);
    else
      fio.new_plus("");
    sprintf(strsql,"%-5d %-*s %15s\n",tabb,iceb_u_kolbait(40,fio.ravno()),fio.ravno(),iceb_u_prnbr(sum));
    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
    fprintf(kaw,"%-5d %-*s %15s\n",tabb,iceb_u_kolbait(40,fio.ravno()),fio.ravno(),iceb_u_prnbr(sum));
    itogo+=sum;
    kol++;
   }
  if(shetbu != NULL && (sumb < -0.009 || sumnb < -0.009 ))
   {
    sprintf(strsql,"select fio from Kartb where tabn=%d",tabb);

    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
      fio.new_plus(row1[0]);
    else
      fio.new_plus("");
    if( sumb < -0.009)
     {
      sprintf(strsql,"%-5d %-*s %15s %s\n",tabb,iceb_u_kolbait(40,fio.ravno()),fio.ravno(),iceb_u_prnbr(sumb),gettext("Бюджет"));
      iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
      fprintf(kaw,"%-5d %-*s %15s %s\n",tabb,iceb_u_kolbait(40,fio.ravno()),fio.ravno(),iceb_u_prnbr(sumb),gettext("Бюджет"));
      itogb+=sumb;
     }
    if( sumnb < -0.009)
     {
      sprintf(strsql,"%-5d %-*s %15s\n",tabb,iceb_u_kolbait(40,fio.ravno()),fio.ravno(),iceb_u_prnbr(sumnb));
      
      iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
      
      fprintf(kaw,"%-5d %-*s %15s\n",tabb,iceb_u_kolbait(40,fio.ravno()),fio.ravno(),iceb_u_prnbr(sumnb));
      itognb+=sumnb;
     }
    kol++;

   }
 }

if(kol == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного должника !"),data->window);
 }
else
 {
  sprintf(strsql,"%s:%d\n",gettext("Количество должников"),kol);
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
  fprintf(kaw,"\
----------------------------------------------------\n");
  if(shetbu == NULL)
   {
    sprintf(strsql,"%5s %*s:%15s\n"," ",iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(itogo));
    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
    fprintf(kaw,"%5s %*s:%15s\n"," ",iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(itogo));
   }
  else
   {
    if(itogb != 0.)
     {
      sprintf(strsql,"%5s %*s:%15s\n"," ",iceb_u_kolbait(40,gettext("Итого бюджет")),gettext("Итого бюджет"),iceb_u_prnbr(itogb));
      iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
      fprintf(kaw,"%5s %*s:%15s\n"," ",iceb_u_kolbait(40,gettext("Итого бюджет")),gettext("Итого бюджет"),iceb_u_prnbr(itogb));
     }
    if(itognb != 0.)
     {
      sprintf(strsql,"%5s %*s:%15s\n"," ",iceb_u_kolbait(40,gettext("Итого хозрасчёт")),gettext("Итого хозрасчёт"),iceb_u_prnbr(itognb));
      iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
      fprintf(kaw,"%5s %*s:%15s\n"," ",iceb_u_kolbait(40,gettext("Итого хозрасчёт")),gettext("Итого хозрасчёт"),iceb_u_prnbr(itognb));
     }
   }
  fprintf(kaw,"%s:%d\n",gettext("Количество должников"),kol);


 }
iceb_podpis(kaw,data->window);
fclose(kaw);

if(kol != 0)
 {
  data->voz=0;
  data->imaf->plus(imaf);
  data->naimf->plus(gettext("Список должников"));
  iceb_ustpeh(imaf,0,data->window);
 }
else
 {

  data->voz=1;
  unlink(imaf);
 }






data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

return(FALSE);

}
