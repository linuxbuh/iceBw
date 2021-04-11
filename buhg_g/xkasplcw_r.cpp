/*$Id: xkasplcw_r.c,v 1.6 2013/09/26 09:46:59 sasa Exp $*/
/*23.05.2016	11.10.2012	Белых А.И.	xkasplcw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"

class xkasplcw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class iceb_u_spisok *imaf;
  class iceb_u_spisok *naimf;  
  int metka_prov;

  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  xkasplcw_r_data()
   {
    voz=1;
    kon_ras=1;
    metka_prov=0;
   }
 };
gboolean   xkasplcw_r_key_press(GtkWidget *widget,GdkEventKey *event,class xkasplcw_r_data *data);
gint xkasplcw_r1(class xkasplcw_r_data *data);
void  xkasplcw_r_v_knopka(GtkWidget *widget,class xkasplcw_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;

int xkasplcw_r(int metka_prov,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok)
{
char strsql[1024];
class iceb_u_str repl;
class xkasplcw_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);

data.metka_prov=metka_prov;
data.imaf=imaf;
data.naimf=naimf;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Проверка логической целосности базы"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(xkasplcw_r_key_press),&data);

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

repl.plus(gettext("Проверка логической целосности базы"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(xkasplcw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)xkasplcw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  xkasplcw_r_v_knopka(GtkWidget *widget,class xkasplcw_r_data *data)
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

gboolean   xkasplcw_r_key_press(GtkWidget *widget,GdkEventKey *event,class xkasplcw_r_data *data)
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

gint xkasplcw_r1(class xkasplcw_r_data *data)
{
time_t vremn=time(NULL);
char strsql[2048];
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;
char imaf_prot[64];
FILE *ff_prot;
int kolstr=0;
float kolstr1=0;
int koloh=0;
int koludzap=0;

sprintf(imaf_prot,"kasplc%d.lst",getpid());

if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_prot,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(data->metka_prov == 0)
 {
  sprintf(strsql,"%s",gettext("Проверка без удаления нелoгичных записей"));
  iceb_printw(strsql,data->buffer,data->view);
  iceb_printw("\n",data->buffer,data->view);
 }  
else
 {
  sprintf(strsql,"%s",gettext("Проверка c удалением нелoгичных записей"));
  iceb_printw(strsql,data->buffer,data->view);
  iceb_printw("\n",data->buffer,data->view);
 }
iceb_u_zagolov(strsql,"","",iceb_get_pnk("00",0,data->window),ff_prot);

fprintf(ff_prot,"\n%s\n",gettext("Подсистема \"Учёт кассовых ордеров\""));


sprintf(strsql,"%s Kasord1\n",gettext("Проверяем таблицу"));
iceb_printw(strsql,data->buffer,data->view);

fprintf(ff_prot,"%s Kasord1\n",gettext("Проверяем таблицу"));

sprintf(strsql,"select * from Kasord1 order by kassa asc,datd asc,tp asc");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
 }

sprintf(strsql,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);
iceb_printw(strsql,data->buffer,data->view);
fprintf(ff_prot,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);


short d,m,g;
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  iceb_u_rsdat(&d,&m,&g,row[2],2);
 
  if(row[1][0] == '0')
   {
    koloh++;
 
    sprintf(strsql,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11]);
    iceb_printw(strsql,data->buffer,data->view);

    sprintf(strsql,"%s tp %s Каsord1 %s !!!\n",gettext("Колонка"),gettext("в таблице"),gettext("равна нолю"));
    iceb_printw(strsql,data->buffer,data->view);
    
    fprintf(ff_prot,"\n%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11]);
    fprintf(ff_prot,"%s tp %s Каsord1 %s !!!\n",gettext("Колонка"),gettext("в таблице"),gettext("равна нолю"));

    if(data->metka_prov == 1)
     {
      sprintf(strsql,"delete from Kasord1 where kassa=%d and god=%d and tp=%d and nomd='%s' and kontr='%s'",
      atoi(row[0]),g,atoi(row[1]),row[4],row[5]);

      if(iceb_sql_zapis(strsql,1,0,data->window) == 0)
       koludzap++;

     }
    continue;
   }

  /*Проверяем наличие шапки документа*/
  sprintf(strsql,"select tp from Kasord where nomd='%s' and kassa=%d and tp=%d and god=%d",
  row[4],atoi(row[0]),atoi(row[1]),g);

  if(iceb_sql_readkey(strsql,data->window) == 0)
   {
    koloh++;

    fprintf(ff_prot,"\n%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11]);
    fprintf(ff_prot,"%s\n",strsql);
    fprintf(ff_prot,"%s!\n",gettext("Не найдена шапка документа"));

    sprintf(strsql,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11]);
    iceb_printw(strsql,data->buffer,data->view);

    sprintf(strsql,"%s!\n",gettext("Не найдена шапка документа"));
    iceb_printw(strsql,data->buffer,data->view);
    

    if(data->metka_prov == 1)
     {
      sprintf(strsql,"delete from Kasord1 where kassa=%d and god=%d and tp=%d and nomd='%s' and kontr='%s'",
      atoi(row[0]),g,atoi(row[1]),row[4],row[5]);

      if(iceb_sql_zapis(strsql,1,0,data->window) == 0)
       koludzap++;

     }

   }  


 }

/*проверяем проводки сделанные к кассовым ордерам*/
sprintf(strsql,"\nПроверяем таблицу Prov\n");
iceb_printw(strsql,data->buffer,data->view);
fprintf(ff_prot,"\nПроверяем таблицу Prov\n");

sprintf(strsql,"select * from Prov where kto='%s' order by datp asc",ICEB_MP_KASA);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
 }

sprintf(strsql,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);
iceb_printw(strsql,data->buffer,data->view);
fprintf(ff_prot,"%s %d\n",gettext("Количесво записей в таблице"),kolstr);

kolstr1=0;

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  iceb_u_rsdat(&d,&m,&g,row[14],2);
  /*Проверяем наличие записей в документе*/
  sprintf(strsql,"select tp from Kasord1 where kassa=%d and god=%d and tp=%d and nomd='%s'",
  atoi(row[7]),g,atoi(row[15]),row[6]);

  if(iceb_sql_readkey(strsql,data->window) == 0)
   {
    koloh++;

    fprintf(ff_prot,"\n%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[12],row[13],row[14],row[15],row[16]);
    fprintf(ff_prot,"%s\n",strsql);
    fprintf(ff_prot,"%s!\n",gettext("Не найдены записи в документе"));

    sprintf(strsql,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],row[10],row[11],row[12],row[13],row[14],row[15],row[16]);
    iceb_printw(strsql,data->buffer,data->view);

    sprintf(strsql,"%s!\n",gettext("Не найдены записи в документе"));
    iceb_printw(strsql,data->buffer,data->view);



    if(data->metka_prov == 1)
     {
      /*проводки парные и они будут удалены по одной так как и к парной проводке тоже не будут найдены записи в документе*/

      sprintf(strsql,"delete from Prov where kto='%s' and pod=%d and nomd='%s' and datd='%s'",
      ICEB_MP_KASA,atoi(row[7]),row[6],row[14]);
      

      if(iceb_sql_zapis(strsql,1,0,data->window) == 0)
       koludzap++;

     }
   }  

 }
sprintf(strsql,"\n%s %d\n",gettext("Количество ошибок"),koloh);
iceb_printw(strsql,data->buffer,data->view);
fprintf(ff_prot,"\n%s %d\n",gettext("Количество ошибок"),koloh);
if(koludzap > 0)
 {
  sprintf(strsql,"%s %d\n",gettext("Количество удалённых записей"),koludzap);
  iceb_printw(strsql,data->buffer,data->view);
  fprintf(ff_prot,"%s %d\n",gettext("Количество удалённых записей"),koludzap);
 }

iceb_podpis(ff_prot,data->window);
fclose(ff_prot);


data->imaf->plus(imaf_prot);
data->naimf->plus(gettext("Протокол найденых ошибок"));

for(int nomer=0; nomer < data->imaf->kolih(); nomer++)
 iceb_ustpeh(data->imaf->ravno(nomer),3,data->window);





gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
