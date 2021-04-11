/*$Id: buh_rnr_shw_r.c,v 1.7 2014/08/31 06:19:19 sasa Exp $*/
/*16.04.2017	01.10.2006	Белых А.И.	buh_rnr_shw_r.c
Расчёт отчёта 
*/
#include <math.h>
#include "buhg_g.h"
#include "buh_rnr_shw.h"

class buh_rnr_shw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class buh_rnr_shw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  buh_rnr_shw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   buh_rnr_shw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buh_rnr_shw_r_data *data);
gint buh_rnr_shw_r1(class buh_rnr_shw_r_data *data);
void  buh_rnr_shw_r_v_knopka(GtkWidget *widget,class buh_rnr_shw_r_data *data);

void buh_hpsw(class iceb_u_spisok *deb,class iceb_u_spisok *kre,class iceb_u_double *suma,
class iceb_u_spisok *debsh,class iceb_u_spisok *kresh,class iceb_u_double *sumash,GtkWidget *wpredok);

extern SQL_baza bd;

int buh_rnr_shw_r(class buh_rnr_shw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class buh_rnr_shw_r_data data;
int gor=0;
int ver=0;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт шахматки по счёту"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(buh_rnr_shw_r_key_press),&data);

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

repl.plus(gettext("Расчёт шахматки по счёту"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(buh_rnr_shw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)buh_rnr_shw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  buh_rnr_shw_r_v_knopka(GtkWidget *widget,class buh_rnr_shw_r_data *data)
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

gboolean   buh_rnr_shw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buh_rnr_shw_r_data *data)
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

gint buh_rnr_shw_r1(class buh_rnr_shw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(strsql,"select nais,saldo,vids,stat from Plansh where ns='%s'",data->rk->shet.ravno());

if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),data->rk->shet.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


int kolstr=0;
//узнаём список субсчетов заданого счёта
class iceb_u_spisok shet_r;

sprintf(strsql,"select distinct sh from Prov where datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
and sh like '%s%%' order by sh asc",gn,mn,dn,gk,mk,dk,data->rk->shet.ravno());

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
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(shet_r.find(row[0]) < 0)
   shet_r.plus(row[0]);  
 }

//узнаём список счетов корреспондентов по дебету
class iceb_u_spisok shet_rd;

sprintf(strsql,"select distinct shk from Prov where datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
and sh like '%s%%' and deb <> 0. and kre = 0. order by shk asc",gn,mn,dn,gk,mk,dk,data->rk->shet.ravno());

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
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(shet_rd.find(row[0]) < 0)
   shet_rd.plus(row[0]);  
 }

//узнаём список счетов корреспондентов по кредиту
class iceb_u_spisok shet_rk;

sprintf(strsql,"select distinct shk from Prov where datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
and sh like '%s%%' and kre <> 0. and deb = 0. order by shk asc",gn,mn,dn,gk,mk,dk,data->rk->shet.ravno());

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
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(shet_rk.find(row[0]) < 0)
   shet_rk.plus(row[0]);  
 }

//создаём массивы дебетов и кредитов
class iceb_u_double sum_deb;
class iceb_u_double sum_kre;

sum_deb.make_class(shet_r.kolih()*shet_rd.kolih());
sum_kre.make_class(shet_r.kolih()*shet_rk.kolih());

//заполняем массивы
sprintf(strsql,"select sh,shk,deb,kre from Prov where datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
and sh like '%s%%'",gn,mn,dn,gk,mk,dk,data->rk->shet.ravno());

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

kolstr1=0;
double deb=0.,kre=0.;
int nomer_shet_r;
int nomer_shet_k;

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  deb=atof(row[2]);
  kre=atof(row[3]);

  if(deb == 0. && kre == 0.) //бывает и такое
   continue;

  nomer_shet_r=shet_r.find(row[0]);

  if(deb != 0. && fabs(deb) > 0.009)
   {
    if((nomer_shet_k=shet_rd.find(row[1])) < 0)
     {
      sprintf(strsql,"Не найден счёт %s в списке дебетовых счетов !",row[1]);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
    
//    printw("дебет %d\n",nomer_shet_k*shet_rd.kolih()+nomer_shet_r);
    sum_deb.plus(deb,nomer_shet_k*shet_r.kolih()+nomer_shet_r);
   }  
  else
   {
    if((nomer_shet_k=shet_rk.find(row[1])) < 0)
     {
      sprintf(strsql,"%s %s %s %s\n",row[0],row[1],row[2],row[3]);
      iceb_printw(strsql,data->buffer,data->view);

      sprintf(strsql,"Не найден счёт %s в списке кредитовых счетов !",row[1]);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
//    printw("кредит %d nomer_shet_k=%d kolih=%d nomer_shet_r=%d\n",nomer_shet_k*shet_rk.kolih()+nomer_shet_r,
//    nomer_shet_k,shet_rk.kolih(),nomer_shet_r);
    
    sum_kre.plus(kre,nomer_shet_k*shet_r.kolih()+nomer_shet_r);
   }  
 }


/*Распечатываем шахматку--------------------------------*/ 
char imaf_hax[64];
sprintf(imaf_hax,"haxg%d.lst",getpid());
class iceb_fopen ff_h;
if(ff_h.start(imaf_hax,"w",data->window) != 0)
 return(1);

iceb_u_zagolov(gettext("Расчёт шахматки по счёту"),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ff_h.ff);

fprintf(ff_h.ff,"%s %s\n",gettext("Дебет счёта"),data->rk->shet.ravno());


buh_rhw(0,&shet_rd,&shet_r,&sum_deb,ff_h.ff);

fprintf(ff_h.ff,"\n\n%s %s\n",gettext("Кредит счёта"),data->rk->shet.ravno());


buh_rhw(0,&shet_rk,&shet_r,&sum_kre,ff_h.ff);



iceb_podpis(ff_h.ff,data->window);
ff_h.end();



char imaf_hax1[64];
sprintf(imaf_hax1,"haxv%d.lst",getpid());
if(ff_h.start(imaf_hax1,"w",data->window) != 0)
 return(1);

iceb_u_zagolov(gettext("Расчёт шахматки по счёту"),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ff_h.ff);

fprintf(ff_h.ff,"%s %s\n",gettext("Дебет счёта"),data->rk->shet.ravno());


buh_rhw(1,&shet_rd,&shet_r,&sum_deb,ff_h.ff);

fprintf(ff_h.ff,"\n\n%s %s\n",gettext("Кредит счёта"),data->rk->shet.ravno());


buh_rhw(1,&shet_rk,&shet_r,&sum_kre,ff_h.ff);



iceb_podpis(ff_h.ff,data->window);
ff_h.end();

/*Распечатка шахматки по счетам*/
class iceb_u_spisok shet_r_s;

class iceb_u_spisok shet_rd_s;
class iceb_u_double suma_deb_s;

class iceb_u_spisok shet_rk_s;
class iceb_u_double suma_kre_s;

/*получение шахматки по счетам*/
buh_hpsw(&shet_rd,&shet_r,&sum_deb,&shet_rd_s,&shet_r_s,&suma_deb_s,data->window);

shet_r_s.free_class(); /*повторно создаётся*/

buh_hpsw(&shet_rk,&shet_r,&sum_kre,&shet_rk_s,&shet_r_s,&suma_kre_s,data->window);



char imaf_haxs[64];
sprintf(imaf_haxs,"haxs%d.lst",getpid());
if(ff_h.start(imaf_haxs,"w",data->window) != 0)
 return(1);
iceb_u_zagolov(gettext("Расчёт шахматки по счёту"),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ff_h.ff);

fprintf(ff_h.ff,"%s %s\n",gettext("Дебет счёта"),data->rk->shet.ravno());

buh_rhw(0,&shet_rd_s,&shet_r_s,&suma_deb_s,ff_h.ff);

fprintf(ff_h.ff,"\n\n%s %s\n",gettext("Кредит счёта"),data->rk->shet.ravno());


buh_rhw(0,&shet_rk_s,&shet_r_s,&suma_kre_s,ff_h.ff);



iceb_podpis(ff_h.ff,data->window);
ff_h.end();


data->rk->imaf.new_plus(imaf_hax);

sprintf(strsql,"%s %s %s 1 (%s)",
gettext("Шахматка по счёту"),
data->rk->shet.ravno(),
gettext("форма"),
gettext("по субсчетам"));

data->rk->naimf.new_plus(strsql);

data->rk->imaf.plus(imaf_hax1);
sprintf(strsql,"%s %s %s 2 (%s)",
gettext("Шахматка по счёту"),
data->rk->shet.ravno(),
gettext("форма"),
gettext("по субсчетам"));
data->rk->naimf.plus(strsql);

data->rk->imaf.plus(imaf_haxs);
sprintf(strsql,"%s %s (%s)",
gettext("Шахматка по счёту"),
data->rk->shet.ravno(),
gettext("по счетам"));
data->rk->naimf.plus(strsql);

for(int nomer=0; nomer < data->rk->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rk->imaf.ravno(nomer),3,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}

