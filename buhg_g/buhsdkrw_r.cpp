/*$Id: buhsdkrw_r.c,v 1.27 2014/06/30 06:35:54 sasa Exp $*/
/*23.05.2016	01.03.2007	Белых А.И.	buhsdkrw_r.c
Расчёт отчёта по заданному списку дебетовых и кредитовых счетов
*/
#include <errno.h>
#include "buhg_g.h"
#include "buhsdkrw.h"

class buhsdkrw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class buhsdkrw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  buhsdkrw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   buhsdkrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhsdkrw_r_data *data);
gint buhsdkrw_r1(class buhsdkrw_r_data *data);
void  buhsdkrw_r_v_knopka(GtkWidget *widget,class buhsdkrw_r_data *data);


extern SQL_baza bd;

int buhsdkrw_r(class buhsdkrw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class buhsdkrw_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт по заданному списку дебетовых и кредитовых счетов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(buhsdkrw_r_key_press),&data);

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

repl.plus(gettext("Расчёт по заданному списку дебетовых и кредитовых счетов"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(buhsdkrw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)buhsdkrw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  buhsdkrw_r_v_knopka(GtkWidget *widget,class buhsdkrw_r_data *data)
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

gboolean   buhsdkrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhsdkrw_r_data *data)
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
/*******************************/
/*Расчёт по заданным настройкам*/
/********************************/

double buhsdkr_r(short dn,short mn,short gn,
short dk,short mk,short gk,
class iceb_u_spisok *sp_sheta_deb,
class iceb_u_spisok *sp_sheta_kre,
const char *shet_d,const char *shet_k,const char *shet_k_minus,
int kolstrp,
class SQLCURSOR *curp,
FILE *ff_dv,FILE *ff_kv,
GtkWidget *bar)
{
SQL_str row;
class iceb_u_spisok sheta_deb;
class iceb_u_spisok sheta_kre;

for(int ii=0; ii < sp_sheta_deb->kolih(); ii++)
 {
  if(iceb_u_proverka(shet_d,sp_sheta_deb->ravno(ii),0,0) != 0)
   continue;
  sheta_deb.plus(sp_sheta_deb->ravno(ii));
 } 

for(int ii=0; ii < sp_sheta_kre->kolih(); ii++)
 {
  if(iceb_u_proverka(shet_k,sp_sheta_kre->ravno(ii),0,0) != 0)
   continue;
  sheta_kre.plus(sp_sheta_kre->ravno(ii));
 } 
 
class iceb_u_double sum_pr; //Массив сумм проводок
int kolih_d=sheta_deb.kolih();
int kolih_k=sheta_kre.kolih();

sum_pr.make_class(kolih_d*kolih_k);

int nom_sd;
int nom_sk;
float kolstr1=0.;
double suma=0.;
curp->poz_cursor(0);
while(curp->read_cursor(&row) != 0)
 {
  iceb_pbar(bar,kolstrp,++kolstr1);    
  if((nom_sd=sheta_deb.find(row[0])) < 0)
    continue;
  if((nom_sk=sheta_kre.find(row[1])) < 0)
    continue;
  suma=atof(row[2]);
  
  
  if(iceb_u_proverka(shet_k_minus,row[0],1,1) == 0)
   suma-=atof(row[3]);

  sum_pr.plus(suma,nom_sd*kolih_k+nom_sk);
  
 }
//Распечатываем дебет по вертикали
buh_rhw(0,&sheta_deb,&sheta_kre,&sum_pr,ff_dv); 

//Распечатываем кредит по вертикали
suma=buh_rhw(1,&sheta_deb,&sheta_kre,&sum_pr,ff_kv);

//printw("\n%s:%.2f\n\n",gettext("Итого"),suma);

return(suma);

}

/******************************************/
/******************************************/

gint buhsdkrw_r1(class buhsdkrw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);

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

class iceb_u_str zag_ras("");
iceb_poldan("Заголовок распечатки",&zag_ras,"buhsdkr.alx",data->window);

int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

//Создаём список дебетовых счетов
sprintf(strsql,"select distinct sh from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val=0 and deb <> 0. order by sh asc",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_spisok sp_sheta_deb;

while(cur.read_cursor(&row) != 0)
  sp_sheta_deb.plus(row[0]);

//Создаём список кредитовых счетов
sprintf(strsql,"select distinct shk from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val=0 and deb <> 0. order by shk asc",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_spisok sp_sheta_kre;

while(cur.read_cursor(&row) != 0)
  sp_sheta_kre.plus(row[0]);


sprintf(strsql,"select sh,shk,deb,kre from Prov where datp >= '%04d-%02d-%02d' \
and datp <= '%04d-%02d-%02d' and val=0",gn,mn,dn,gk,mk,dk);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstralx=0;
sprintf(strsql,"select str from Alx where fil='buhsdkr.alx' order by ns asc");
if((kolstralx=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstralx == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"buhsdkr.alx");
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



char imaf_dv[64];
sprintf(imaf_dv,"sdkr%d.lst",getpid());
FILE *ff_dv;

if((ff_dv=fopen(imaf_dv,"w")) == NULL) //Открываем файл с распечаткой
 {
  iceb_er_op_fil(imaf_dv,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_zagolov(zag_ras.ravno(),dn,mn,gn,dk,mk,gk,ff_dv,data->window);

char imaf_kv[64];
sprintf(imaf_kv,"skdr%d.lst",getpid());
FILE *ff_kv;

if((ff_kv=fopen(imaf_kv,"w")) == NULL) //Открываем файл с распечаткой
 {
  iceb_er_op_fil(imaf_kv,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_zagolov(zag_ras.ravno(),dn,mn,gn,dk,mk,gk,ff_kv,data->window);

char imaf_it[64];
sprintf(imaf_it,"skdri%d.lst",getpid());
FILE *ff_it;

if((ff_it=fopen(imaf_it,"w")) == NULL) //Открываем файл с распечаткой
 {
  iceb_er_op_fil(imaf_it,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_zagolov(gettext("Свод по вариантам расчёта"),dn,mn,gn,dk,mk,gk,ff_it,data->window);

fprintf(ff_it,"\
------------------------------------------------\n");
fprintf(ff_it,"\
 Код |     Наименование             |  Сумма   |\n");
fprintf(ff_it,"\
------------------------------------------------\n");

class iceb_u_str shet_d;
class iceb_u_str shet_k;
class iceb_u_str stroka1("");
class iceb_u_str variant("");
class iceb_u_str naim_r("");
class iceb_u_str shet_k_minus;

double itogo_var=0.;
double it=0.;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],&stroka1,1,'|') != 0 )
    continue; 
  if(iceb_u_SRAV(stroka1.ravno(),"Вариант расчёта",0) != 0 )
   continue;
  else
   {

    iceb_u_polen(row_alx[0],&variant,2,'|');
    if(variant.ravno()[0] == '\0')
     continue;
    
    
    iceb_printw(row_alx[0],data->buffer,data->view);
    
    if(iceb_u_proverka(data->rk->variant_r.ravno(),variant.ravno(),0,0) != 0)
     continue;

    
    cur_alx.read_cursor(&row_alx);
    iceb_printw(row_alx[0],data->buffer,data->view);
    iceb_u_polen(row_alx[0],&naim_r,2,'|');
    
    shet_d.new_plus("");
    shet_k.new_plus("");
    shet_k_minus.new_plus("");
                
    cur_alx.read_cursor(&row_alx);
    iceb_printw(row_alx[0],data->buffer,data->view);
    iceb_u_polen(row_alx[0],&stroka1,2,'|');
    iceb_fplus(stroka1.ravno(),&shet_d,&cur_alx);    

    cur_alx.read_cursor(&row_alx);
    iceb_printw(row_alx[0],data->buffer,data->view);
    iceb_u_polen(row_alx[0],&stroka1,2,'|');
    iceb_fplus(stroka1.ravno(),&shet_k,&cur_alx);
    
    cur_alx.read_cursor(&row_alx);
    
    if(iceb_u_SRAV("Отнять кредит для счетов",row_alx[0],1) == 0)
     {
      iceb_u_polen(row_alx[0],&stroka1,2,'|');
      iceb_fplus(stroka1.ravno(),&shet_k_minus,&cur_alx);
     }  
    iceb_printw(row_alx[0],data->buffer,data->view);

    fprintf(ff_dv,"\n%s\n",naim_r.ravno());  
    fprintf(ff_kv,"\n%s\n",naim_r.ravno());  
    
    itogo_var=buhsdkr_r(dn,mn,gn,dk,mk,gk,&sp_sheta_deb,&sp_sheta_kre,shet_d.ravno(),shet_k.ravno(),shet_k_minus.ravno(),kolstr,&cur,ff_dv,ff_kv,data->bar);
    sprintf(strsql,"\n%s:%.2f\n\n",gettext("Итого"),itogo_var);

    fprintf(ff_it,"%*s %-*.*s %10.2f\n",
    iceb_u_kolbait(5,variant.ravno()),variant.ravno(),
    iceb_u_kolbait(30,naim_r.ravno()),iceb_u_kolbait(30,naim_r.ravno()),naim_r.ravno(),
    itogo_var);
    
    iceb_printw(strsql,data->buffer,data->view);

    it+=itogo_var;    
      
    fprintf(ff_dv,"\n\n\n\n");  
    fprintf(ff_kv,"\n\n\n\n");  
     
   }
    
 }

fprintf(ff_it,"\
------------------------------------------------\n");
fprintf(ff_it,"%*s %10.2f\n",iceb_u_kolbait(36,gettext("Итого")),gettext("Итого"),it);

iceb_podpis(ff_dv,data->window);
fclose(ff_dv);

iceb_podpis(ff_kv,data->window);
fclose(ff_kv);




iceb_podpis(ff_it,data->window);
fclose(ff_it);

iceb_ustpeh(imaf_dv,0,data->window);
iceb_ustpeh(imaf_kv,0,data->window);
iceb_ustpeh(imaf_it,0,data->window);

data->rk->imaf.plus(imaf_dv);
data->rk->imaf.plus(imaf_kv);
data->rk->imaf.plus(imaf_it);

data->rk->naimf.plus(gettext("Отчёт с дебетами по вертикали"));
data->rk->naimf.plus(gettext("Отчёт с кредитами по вертикали"));
data->rk->naimf.plus(gettext("Отчёт с итогами по вариантам расчёта"));


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
