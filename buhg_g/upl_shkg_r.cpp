/*$Id: upl_shkg_r.c,v 1.16 2013/09/26 09:46:58 sasa Exp $*/
/*23.05.2016	02.03.2008	Белых А.И.	upl_shkg_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "upl_shkg.h"

class upl_shkg_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class upl_shkg_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  upl_shkg_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   upl_shkg_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_shkg_r_data *data);
gint upl_shkg_r1(class upl_shkg_r_data *data);
void  upl_shkg_r_v_knopka(GtkWidget *widget,class upl_shkg_r_data *data);

double upl_ptw(short dn,short mn,short gn,short dk,short mk,short gk,char *kod_avt,char *kod_vod,GtkWidget *wpredok);


extern SQL_baza bd;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int upl_shkg_r(class upl_shkg_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class upl_shkg_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка ведомости списания топлива по счетам в киллограммах"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(upl_shkg_r_key_press),&data);

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

repl.plus(gettext("Распечатка ведомости списания топлива по счетам в киллограммах"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(upl_shkg_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)upl_shkg_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  upl_shkg_r_v_knopka(GtkWidget *widget,class upl_shkg_r_data *data)
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

gboolean   upl_shkg_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_shkg_r_data *data)
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
/*************************************/
/*Шапка                              */
/*************************************/

void    svst_kg(int *kollist,int *kolstrok,FILE *ff)
{
*kollist+=1;
*kolstrok+=5;

fprintf(ff,"%70s%s N%d\n","",gettext("Лист"),*kollist);

fprintf(ff,"\
-----------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Счёт |Код топлива|Расход топлива|Коэффициент|Расход топлива|Средняя| Сумма |\n\
      |           |   в литрах   | перевода  |в киллограммах| цена  |       |\n"));
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
//123456 12345678901 12345678901234 12345678901 12345678901234 1234567 1234567
}

/********************************/
/*Счетчик                       */
/********************************/
void     shetvst_kg(int *kolstrok,int *kollist,FILE *ff)
{
*kolstrok+=1;
if(*kolstrok > kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kolstrok=0;
  svst_kg(kollist,kolstrok,ff);
 }  

}
/****************************/
/*Выдача итоговой строки*/
/**************************/
void uplvst_kg_it(int metka, //0-итог по счёту 1-общий
double *itog,FILE *ff)
{

char strsql[512];
if(metka == 0)
 sprintf(strsql,"%s",gettext("Итого по счёту"));
if(metka == 1)
 sprintf(strsql,"%s",gettext("Общий итог"));

fprintf(ff,"%*s:%11.11g %11s %14.14g %7s %7.2f\n",
iceb_u_kolbait(21,strsql),strsql,itog[0]," ",itog[1]," ",itog[2]);



}

/******************************************/
/******************************************/

gint upl_shkg_r1(class upl_shkg_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,row1;
class SQLCURSOR cur,cur1;

printf("%s\n",__FUNCTION__);

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


long	kolstr=0;
FILE    *ff;
char    imaf[64];
int	kollist=0;
int	kolstrok=0;

sprintf(strsql,"select datd,nomd,kt,shet,zt from Upldok2a where datd >= '%d-%d-%d' and \
datd <= '%d-%d-%d' order by shet asc",gn,mn,dn,gk,mk,dk);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не нашли ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }




class iceb_u_spisok shet_kodt; //Список счёт->код топлива
class iceb_u_double suma_shet_kodt; //Расход топлива в литрах

float kolstr1=0;
int nomer=0;
double suma;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->kod_top.ravno(),row[2],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->kod_vod.ravno(),row[9],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),row[8],0,0) != 0)
   continue;

  if(data->rk->kod_vod.getdlinna() > 1)
   {
    sprintf(strsql,"select nst,nstzg,ztvsn,ztvrn from Upldok where datd = '%s' and nomd='%s'",row[0],row[1]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      sprintf(strsql,"select kv from Upldok where datd = '%s' and nomd='%s'",row[0],row[1]);
      if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
       {
        if(iceb_u_proverka(data->rk->kod_vod.ravno(),row1[0],0,0) != 0)
          continue;
       }
     }  
    else
     {
      sprintf(strsql,"%s\n%s %s",gettext("Не нашли шапку документа!"),row[0],row[1]);
      iceb_menu_soob(strsql,data->window);    
      continue;    
     }
   }
  suma=atof(row[4]);
  sprintf(strsql,"%s|%s",row[3],row[2]);

  if((nomer=shet_kodt.find(strsql)) == -1)
   shet_kodt.plus(strsql);

  suma_shet_kodt.plus(suma,nomer);
 }

sprintf(imaf,"vstkg%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


kolstrok=5;
iceb_zagolov(gettext("Ведомость списания топлива по счетам в киллограммах"),dn,mn,gn,dk,mk,gk,ff,data->window);

if(data->rk->kod_top.ravno()[0] != '\0')
 {
  kolstrok++;
  fprintf(ff,"%s:%s\n",gettext("Код топлива"),data->rk->kod_top.ravno());  
 }

if(data->rk->kod_vod.ravno()[0] != '\0')
 {
  kolstrok++;
  fprintf(ff,"%s:%s\n",gettext("Код водителя"),data->rk->kod_vod.ravno());  
 }

if(data->rk->shet.ravno()[0] != '\0')
 {
  kolstrok++;
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());  
 }

svst_kg(&kollist,&kolstrok,ff);
class iceb_u_str shet_sp("");
class iceb_u_str kod_top("");
double koef=0.;
double cn_suma=0.;
class iceb_u_str shet_sp_z("");
double srcn=0.;
double itog_shet[3];

memset(&itog_shet,'\0',sizeof(itog_shet));

double itog[3];
memset(&itog,'\0',sizeof(itog));

for(int i=0; i < shet_kodt.kolih() ; i++)
 {
  iceb_u_polen(shet_kodt.ravno(i),&shet_sp,1,'|');  
  iceb_u_polen(shet_kodt.ravno(i),&kod_top,2,'|');  
  if(iceb_u_SRAV(shet_sp.ravno(),shet_sp_z.ravno(),0) != 0)
   {
    if(shet_sp_z.ravno()[0] != '\0')
     uplvst_kg_it(0,itog_shet,ff);
    
    memset(&itog_shet,'\0',sizeof(itog_shet));
    shet_sp_z.new_plus(shet_sp.ravno());
   }  


  //читаем среднюю цену и коэффициент перевода
  koef=0.;
  srcn=0.;
  sprintf(strsql,"select srcn,kp from Uplmtsc where datsc <= '%04d-%02d-%02d' and kodt='%s' \
order by datsc desc",
  gn,mn,dn,kod_top.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
   {
    srcn=atof(row[0]);
    koef=atof(row[1]);
   }  
  suma=koef*suma_shet_kodt.ravno(i);
  suma=iceb_u_okrug(suma,0.001);
  
  shetvst_kg(&kolstrok,&kollist,ff);
  
  cn_suma=srcn*suma;
  
  itog_shet[0]+=suma_shet_kodt.ravno(i);
  itog_shet[1]+=suma;
  itog_shet[2]+=cn_suma;

  itog[0]+=suma_shet_kodt.ravno(i);
  itog[1]+=suma;
  itog[2]+=cn_suma;
  
  fprintf(ff,"%-*s %-*s %14.14g %11.11g %14.14g %7.2f %7.2f\n",
  iceb_u_kolbait(6,shet_sp.ravno()),shet_sp.ravno(),
  iceb_u_kolbait(11,kod_top.ravno()),kod_top.ravno(),
  suma_shet_kodt.ravno(i),koef,suma,srcn,cn_suma);
  
 }
uplvst_kg_it(0,itog_shet,ff);
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
uplvst_kg_it(1,itog,ff);

iceb_podpis(ff,data->window);
fclose(ff);


data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Ведомость списания топлива по счетам в киллограммах"));
iceb_ustpeh(imaf,0,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
