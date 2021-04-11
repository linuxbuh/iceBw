/*$Id: buhgspw_r.c,v 1.25 2014/07/31 07:08:24 sasa Exp $*/
/*23.05.2016	01.03.2007	Белых А.И.	buhgspw_r.c
Расчёт распределения административных затрат на доходы
*/
#include <math.h>
#include <errno.h>
#include "buhg_g.h"
#include "buhgspw.h"

class buhgspw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class buhgspw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  buhgspw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   buhgspw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhgspw_r_data *data);
gint buhgspw_r1(class buhgspw_r_data *data);
void  buhgspw_r_v_knopka(GtkWidget *widget,class buhgspw_r_data *data);

void rpsrsw(const char *shet,
short dn,short mn,short gn,
short dk,short mk,short gk,
class iceb_u_spisok *sp_kontr,
class iceb_u_spisok *sp_grup,
class iceb_u_double *deb_saldon,
class iceb_u_double *kre_saldon,
class iceb_u_double *deb_oborot,
class iceb_u_double *kre_oborot,
class iceb_u_double *deb_saldok,
class iceb_u_double *kre_saldok,
GtkWidget *wpredok);

extern SQL_baza bd;
extern int kol_strok_na_liste;

int buhgspw_r(class buhgspw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class buhgspw_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт по спискам групп контрагентов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(buhgspw_r_key_press),&data);

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

repl.plus(gettext("Расчёт по спискам групп контрагентов"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,wpredok));
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
//gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_ALWAYS,GTK_POLICY_ALWAYS);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(buhgspw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)buhgspw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  buhgspw_r_v_knopka(GtkWidget *widget,class buhgspw_r_data *data)
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

gboolean   buhgspw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhgspw_r_data *data)
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

/***************************/
/*счётчик строк*/
/*********************/

void buhgsp_ss(short dn,short mn,short gn,
short dk,short mk,short gk,
int *kollist,int *kolstrok_ras,FILE *ff)
{

*kolstrok_ras+=1;
if(*kolstrok_ras <= kol_strok_na_liste)
 return;

*kolstrok_ras=0;

fprintf(ff,"\f");
sapgorsw(dn,mn,gn,dk,mk,gk,kollist,kolstrok_ras,ff);
*kolstrok_ras+=1;

}


/******************************************/
/******************************************/

gint buhgspw_r1(class buhgspw_r_data *data)
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


SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

sprintf(strsql,"select str from Alx where fil='buhgsp.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"buhgsp.alx");
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

class iceb_u_spisok naim_sp;
class iceb_u_spisok kod_sp;
class iceb_u_spisok kod_gr;

class iceb_u_str bros("");

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],&bros,1,'|') != 0)
   continue;  
  if(iceb_u_SRAV(bros.ravno(),"Наименование списка групп",0) == 0)
   {

    iceb_u_polen(row_alx[0],&bros,2,'|');
    if(bros.ravno()[0] == '\0')
     continue;

    naim_sp.plus(bros.ravno());

    cur_alx.read_cursor(&row_alx);
    iceb_u_polen(row_alx[0],&bros,2,'|');
    kod_sp.plus(bros.ravno());

    cur_alx.read_cursor(&row_alx);
    iceb_u_polen(row_alx[0],&bros,2,'|');
    iceb_fplus(0,bros.ravno(),&kod_gr,&cur_alx);
       
   }
 }

int kol_spiskov=naim_sp.kolih();

if(kol_spiskov == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного списка групп контрагентов !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_spisok sp_kontr;
class iceb_u_spisok sp_grup;
class iceb_u_double deb_saldon;
class iceb_u_double kre_saldon;
class iceb_u_double deb_oborot;
class iceb_u_double kre_oborot;
class iceb_u_double deb_saldok;
class iceb_u_double kre_saldok;

//расчёт
rpsrsw(data->rk->shet.ravno(),dn,mn,gn,dk,mk,gk,&sp_kontr,&sp_grup,&deb_saldon,&kre_saldon,&deb_oborot,&kre_oborot,&deb_saldok,&kre_saldok,data->window);


char imaf[64];
sprintf(imaf,"gsp%d.lst",getpid());
FILE *ff;

if((ff=fopen(imaf,"w")) == NULL) 
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_zagolov(gettext("Расчёт по спискам групп контрагентов"),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ff);

int kolstrok_ras=5;
int kollist=0;
int kolgrup=0;
int kol_kontr=sp_kontr.kolih();
class iceb_u_str naim_grup("");
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str naim_kontr("");
double itogo_gr[6];
memset(itogo_gr,'\0',sizeof(itogo_gr));
double itogo_ob[6];
memset(itogo_ob,'\0',sizeof(itogo_ob));
double itogo_sp[6];
memset(itogo_sp,'\0',sizeof(itogo_sp));
short metka_gr=0;
class iceb_u_str grupa("");

for(int ii=0; ii < kol_spiskov; ii++)
 {
  if(iceb_u_proverka(data->rk->kod_spis_kontr.ravno(),kod_sp.ravno(ii),0,0) != 0)
   continue;
  memset(itogo_sp,'\0',sizeof(itogo_sp));
  
  if(ii != 0)
   {
    fprintf(ff,"\f");
    kolstrok_ras=0;
    kollist=0;
   }

  fprintf(ff,"%s\n",naim_sp.ravno(ii));
  kolstrok_ras+=1;

  metka_gr=0;      
  kolgrup=iceb_u_pole2(kod_gr.ravno(ii),',');

  for(int kk=0; kk < kolgrup; kk++)
   {

    iceb_u_polen(kod_gr.ravno(ii),&grupa,kk+1,',');
    if(grupa.ravno()[0] == '\0')
     continue;
    memset(itogo_gr,'\0',sizeof(itogo_gr));

    metka_gr++;
    if(metka_gr > 1)
     {
      fprintf(ff,"\f");
      kolstrok_ras=0;
     }

    sprintf(strsql,"select naik from Gkont where kod=%s",grupa.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     naim_grup.new_plus(row[0]);
    else
     naim_grup.new_plus("");
     
    buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
    fprintf(ff,"%s:%s %s\n",gettext("Группа"),grupa.ravno(),naim_grup.ravno());
    
    sapgorsw(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);

    int kolih_v_gr=0;
    for(int spk=0; spk < kol_kontr; spk++)
     {
      if(iceb_u_SRAV(grupa.ravno(),sp_grup.ravno(spk),0) != 0)
       continue;

      if(fabs(deb_saldon.ravno(spk)) < 0.009 && \
         fabs(kre_saldon.ravno(spk)) < 0.009 && \
         fabs(deb_oborot.ravno(spk)) < 0.009 && \
         fabs(kre_oborot.ravno(spk)) < 0.009 )
            continue;
      //Узнаём наименование контрагента
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",sp_kontr.ravno(spk));
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)       
       naim_kontr.new_plus(row[0]);
      else
       naim_kontr.new_plus("");
      
      buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
            
      if(deb_saldon.ravno(spk) > kre_saldon.ravno(spk))
        fprintf(ff,"%*s %-*.*s %14.2f %14s %14.2f %14.2f ",
        iceb_u_kolbait(10,sp_kontr.ravno(spk)),sp_kontr.ravno(spk),
        iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
        deb_saldon.ravno(spk),"",deb_oborot.ravno(spk),kre_oborot.ravno(spk));
      else
        fprintf(ff,"%*s %-*.*s %14s %14.2f %14.2f %14.2f ",
        iceb_u_kolbait(10,sp_kontr.ravno(spk)),sp_kontr.ravno(spk),
        iceb_u_kolbait(20,naim_kontr.ravno()),iceb_u_kolbait(20,naim_kontr.ravno()),naim_kontr.ravno(),
        "",kre_saldon.ravno(spk),deb_oborot.ravno(spk),kre_oborot.ravno(spk));

      if(deb_saldok.ravno(spk) > kre_saldok.ravno(spk))
        fprintf(ff,"%14.2f\n",deb_saldok.ravno(spk));
      else
        fprintf(ff,"%14s %14.2f\n","",kre_saldok.ravno(spk));
       
      if(strlen(naim_kontr.ravno()) > 20)
       {
        buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
        fprintf(ff,"%-10s %s\n"," ",iceb_u_adrsimv(20,naim_kontr.ravno()));
       }

      itogo_gr[0]+=deb_saldon.ravno(spk);
      itogo_gr[1]+=kre_saldon.ravno(spk);

      itogo_gr[2]+=deb_oborot.ravno(spk);
      itogo_gr[3]+=kre_oborot.ravno(spk);

      itogo_gr[4]+=deb_saldok.ravno(spk);
      itogo_gr[5]+=kre_saldok.ravno(spk);
      kolih_v_gr++;      
     }

    buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
    fprintf(ff,"\
 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");

    buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
    fprintf(ff,"%*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
    iceb_u_kolbait(31,gettext("Итого по группе")),gettext("Итого по группе"),
    itogo_gr[0],itogo_gr[1],itogo_gr[2],itogo_gr[3],itogo_gr[4],itogo_gr[5]);

    buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
    
    fprintf(ff,"%*s %d\n",
    iceb_u_kolbait(31,gettext("Количество контрагентов в группе")),gettext("Количество контрагентов в группе"),
    kolih_v_gr); 
    
    itogo_sp[0]+=itogo_gr[0];
    itogo_sp[1]+=itogo_gr[1];
    itogo_sp[2]+=itogo_gr[2];
    itogo_sp[3]+=itogo_gr[3];
    itogo_sp[4]+=itogo_gr[4];
    itogo_sp[5]+=itogo_gr[5];
    
   }

  buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------\n");

  buhgsp_ss(dn,mn,gn,dk,mk,gk,&kollist,&kolstrok_ras,ff);
  fprintf(ff,"%*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
  iceb_u_kolbait(31,gettext("Итого по списку")),gettext("Итого по списку"),
  itogo_sp[0],itogo_sp[1],itogo_sp[2],itogo_sp[3],itogo_sp[4],itogo_sp[5]);

  itogo_ob[0]+=itogo_sp[0];
  itogo_ob[1]+=itogo_sp[1];
  itogo_ob[2]+=itogo_sp[2];
  itogo_ob[3]+=itogo_sp[3];
  itogo_ob[4]+=itogo_sp[4];
  itogo_ob[5]+=itogo_sp[5];

 }

if(kol_spiskov > 1)
  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*.*s %14.2f %14.2f %14.2f %14.2f %14.2f %14.2f\n",
iceb_u_kolbait(31,gettext(" И т о г о")),iceb_u_kolbait(31,gettext(" И т о г о")),gettext(" И т о г о"),
itogo_ob[0],itogo_ob[1],itogo_ob[2],itogo_ob[3],itogo_ob[4],itogo_ob[5]);



iceb_podpis(ff,data->window);
fclose(ff);


sprintf(strsql,"%-*s:%10.2f %10.2f\n",
iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),
itogo_ob[0],itogo_ob[1]);

iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%-*s:%10.2f %10.2f\n",
iceb_u_kolbait(20,gettext("Оборот за период")),
gettext("Оборот за период"),
itogo_ob[2],itogo_ob[3]);

iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%-*s:%10.2f %10.2f\n",
iceb_u_kolbait(20,gettext("Сальдо конечное")),gettext("Сальдо конечное"),
itogo_ob[4],itogo_ob[5]);

iceb_printw(strsql,data->buffer,data->view);



data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчёт по спискам групп контрагентов"));
iceb_ustpeh(imaf,1,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
