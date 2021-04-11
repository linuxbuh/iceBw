/*$Id: usl_spismat_r.c,v 1.21 2014/07/31 07:08:27 sasa Exp $*/
/*23.05.2016	19.10.2004	Белых А.И.	usl_spismat_r.c

*/
#include "buhg_g.h"
#include "usl_spismat.h"

class usl_spismat_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
  short prkk;
  
  class usl_spismat_rek *rk;
    

 };

gboolean   usl_spismat_r_key_press(GtkWidget *widget,GdkEventKey *event,class usl_spismat_r_data *data);
gint usl_spismat_r1(class usl_spismat_r_data *data);
void  usl_spismat_r_v_knopka(GtkWidget *widget,class usl_spismat_r_data *data);

extern SQL_baza bd;
 
void usl_spismat_r(class usl_spismat_rek *rek_ras,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_spisok repl_s;
class usl_spismat_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);
data.rk=rek_ras;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Копировать записи из других документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(usl_spismat_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Копировать записи из других документов"));
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


gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(usl_spismat_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)usl_spismat_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  usl_spismat_r_v_knopka(GtkWidget *widget,class usl_spismat_r_data *data)
{
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   usl_spismat_r_key_press(GtkWidget *widget,GdkEventKey *event,class usl_spismat_r_data *data)
{
 printf("usl_spismat_r_key_press\n");

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

gint usl_spismat_r1(class usl_spismat_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
int kolstr;
iceb_u_str repl;
int kolsz=0;

short d,m,g;
iceb_u_rsdat(&d,&m,&g,data->rk->datas.ravno(),1);

//Блокируем таблицу
class iceb_lock_tables lock("LOCK TABLES Dokummat WRITE,icebuser READ");

/*Проверяем есть ли такой номер документа*/
sprintf(strsql,"select nomd from Dokummat where \
datd >= '%04d-01-01' and datd <= '%d-12-31' and sklad=%d and nomd='%s'",
g,g,
data->rk->sklad.ravno_atoi(),
data->rk->nomdok.ravno());

if(iceb_sql_readkey(strsql,data->window) >= 1)
 {
  sprintf(strsql,gettext("С номером %s документ уже есть !"),data->rk->nomdok.ravno());
  iceb_menu_soob(strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

float pnds=iceb_pnds(d,m,g,data->window);

if(zap_s_mudokw(data->rk->tipz,d,m,g,data->rk->sklad.ravno_atoi(),data->rk->kontr.ravno(),
data->rk->nomdok.ravno(),"",data->rk->kodop.ravno(),0,0,"",0,0,0,0,"00",pnds,data->window) != 0)
 {
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

//Разблокируем все таблицы
lock.unlock();

SQLCURSOR cur;
SQL_str row;

sprintf(strsql,"select kodzap,kolih,cena,ei,shetu,dnaim,shsp from Usldokum1 where datd='%04d-%02d-%02d' \
and podr=%d and nomd='%s' and tp=%d and metka=0",
data->rk->gd,data->rk->md,data->rk->dd,data->rk->podr,data->rk->nomdoku.ravno(),data->rk->tipz);

//iceb_printw(strsql,data->buffer,data->view);
//iceb_printw("\n",data->buffer,data->view);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char osnov[80];
sprintf(osnov,"USL%02d.%02d.%d N%s",data->rk->dd,data->rk->md,data->rk->gd,data->rk->nomdoku.ravno());

//Записываем через кого (есть в документе на приход и на расход)
sprintf(strsql,"insert into Dokummat2 \
values (%d,%d,'%s',%d,'%s')",g,data->rk->sklad.ravno_atoi(),data->rk->nomdok.ravno(),2,osnov);

//iceb_printw(strsql,data->buffer,data->view);
//iceb_printw("\n",data->buffer,data->view);

iceb_sql_zapis(strsql,1,0,data->window);


//программа позволяет записать один и тотже материал любое количество раз
//поэтому нужно просумировать количества одинаковых кодов материалов с тем 
//чтобы списать в материальном учёте одной записью
iceb_u_int kod_mat;
iceb_u_double kolih_mat;
int kodm;
double kolih;
int nomer;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  sprintf(strsql,"%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
  iceb_printw(strsql,data->buffer,data->view);
  
  kodm=atoi(row[0]);
  kolih=atof(row[1]);
  
  if((nomer=kod_mat.find(kodm)) < 0)
   kod_mat.plus(kodm,nomer);
  kolih_mat.plus(kolih,nomer);
  kolsz++;  
 }
double ndss=data->rk->pnds;
if(data->rk->lnds != 0)
 ndss=0.;
 
for(int i=0; i < kod_mat.kolih() ; i++)
 {

  sprintf(strsql,"select cena,ei,shetu,shsp from Usldokum1 where datd='%04d-%02d-%02d' \
and podr=%d and nomd='%s' and tp=%d and metka=0 and kodzap=%d limit 1",
  data->rk->gd,data->rk->md,data->rk->dd,data->rk->podr,data->rk->nomdoku.ravno(),data->rk->tipz,kod_mat.ravno(i));
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) < 1)
   {
    iceb_menu_soob(gettext("Не найдена запись в документе"),data->window);
    continue;
   }

  if(data->rk->tipz == 1)
   zap_prihodw(d,m,g,data->rk->nomdok.ravno(),data->rk->sklad.ravno_atoi(),kod_mat.ravno(i),kolih_mat.ravno(i),atof(row[0]),row[1],row[2],ndss,0,0,"",row[3],data->window);
  if(data->rk->tipz == 2)
    priv_k_kartw(d,m,g,data->rk->nomdok.ravno(),data->rk->sklad.ravno_atoi(),kod_mat.ravno(i),kolih_mat.ravno(i),atof(row[0]),row[2],row[2],ndss,0,0,"",0,row[3],data->window);

 
 }
podtdok1w(d,m,g,data->rk->sklad.ravno_atoi(),data->rk->nomdok.ravno(),data->rk->tipz,d,m,g,1,data->window);
podvdokw(d,m,g,data->rk->nomdok.ravno(),data->rk->sklad.ravno_atoi(),data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен."));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

//sprintf(strsql,"%s %d !",gettext("Количество переписаних документов"),koldok);
//repl.new_plus(strsql);
sprintf(strsql,"%s %d !",gettext("Количество переписаних записей"),kolsz);
repl.new_plus(strsql);
iceb_menu_soob(&repl,data->window);

return(FALSE);
}
