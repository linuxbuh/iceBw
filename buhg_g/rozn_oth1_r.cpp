/*$Id: rozn_oth1_r.c,v 1.22 2013/09/26 09:46:54 sasa Exp $*/
/*23.05.2016	01.05.2005	Белых А.И. 	rozn_oth1_r.c
Расчёт 
*/
#include <errno.h>
#include "buhg_g.h"
#include "rozn_oth.h"

class rozn_oth1_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class rozn_oth_rr *rk;
  int kassa;
  class iceb_u_str fam;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  rozn_oth1_r_data()
   {
    voz=1;
    kon_ras=1;
    fam.plus("");
    kassa=0;
   }
 };

gboolean rozn_oth1_r_key_press(GtkWidget *widget,GdkEventKey *event,class rozn_oth1_r_data *data);
gint rozn_oth1_r1(class rozn_oth1_r_data *data);
void  rozn_oth1_r_v_knopka(GtkWidget *widget,class rozn_oth1_r_data *data);

void rasnak(int skl,short d,short m,short g,char nn[],double *suma,double *sumabn,double *sp,double *spbn,short vt,double *sumkor,int lnds,FILE *ff,FILE *ffnp,double *sumsnds,GtkWidget *wpredok);

void  sapitsh(FILE *ff);
void  rasoi(iceb_u_spisok *mssi,iceb_u_double *mspsi,FILE *ff);
void rasis(char orgn[],double itd,short dn,short mn,short gn,short dk,short mk,short gk,int skl1,char naiskl[],char naik[],iceb_u_spisok *mso,iceb_u_double *mspo,iceb_u_spisok *mss,iceb_u_double *msps,int*,int*,FILE *ff);


void rasid(double itd,short dn,short mn,short gn,short dk,short mk,short gk,int skl1,char naiskl[],int*,int*,FILE *ff);

void gsapr(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl1,char naiskl[],FILE *ff);

void rppvo1(int skl1,char *naiskl,short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,FILE *ff);

void rasmasop(iceb_u_spisok *spopr,iceb_u_double *mkm,iceb_u_double *mkm1_1r,iceb_u_double *mkm1_1k,short tip,FILE *ff,class rozn_oth1_r_data *data);


extern SQL_baza bd;

int rozn_oth1_r(class rozn_oth_rr *datark,int kassa,const char *fam,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class rozn_oth1_r_data data;

data.rk=datark;
data.kassa=kassa;
data.fam.new_plus(fam);
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать розничную реализацию по торговим местам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rozn_oth1_r_key_press),&data);

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

repl.plus(gettext("Распечатать розничную реализацию по торговим местам"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(rozn_oth1_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)rozn_oth1_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rozn_oth1_r_v_knopka(GtkWidget *widget,class rozn_oth1_r_data *data)
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

gboolean   rozn_oth1_r_key_press(GtkWidget *widget,GdkEventKey *event,class rozn_oth1_r_data *data)
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

gint rozn_oth1_r1(class rozn_oth1_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s %d.%d.%d => %d.%d.%d\n",gettext("Распечатка списка продаж"),dn,mn,gn,dk,mk,gk);
iceb_printw(strsql,data->buffer,data->view);

if(data->kassa != 0)
  sprintf(strsql,"select * from Roznica \
where datrp >= '%d-%02d-%02d' and datrp<= '%d-%02d-%02d' and kassa=%d order by datrp,nomer asc",
gn,mn,dn,gk,mk,dk,data->kassa);

if(data->kassa == 0)
  sprintf(strsql,"select * from Roznica \
where datrp >= '%d-%02d-%02d' and datrp<= '%d-%02d-%02d' order by kassa,datrp,nomer asc",
gn,mn,dn,gk,mk,dk);

//iceb_printw(strsql,data->buffer,data->view);

int kolstr;
SQLCURSOR cur;
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
FILE *ff;

sprintf(imaf,"vse%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Распечатка списка продаж"));


iceb_zagolov(gettext("Распечатка списка продаж"),dn,mn,gn,dk,mk,gk,ff,data->window);

if(data->kassa != 0)
  fprintf(ff,"%s N%d\n",gettext("По рабочему месту"),data->kassa);
else
  fprintf(ff,"%s.\n",gettext("По всем рабочим местам"));


fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext(" Дата     |  Наименование товара         |Е и|Количество|  Цена    |  Сумма   |К.мт|N м|N пр|Время| Кто |\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n");


int kas=0,kasz=0,npz=0,np=0;
double ito=0.,itk=0.,itp=0.,it=0.;
SQLCURSOR cur1;
SQL_str row;
SQL_str row1;
time_t vrem;
struct tm *bf;
class iceb_u_str naim("");
short d,m,g;
double bb;
float kolstr1=0.;

while((cur.read_cursor(&row)) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);
  np=atoi(row[2]);
  kas=atoi(row[1]);

  if(npz != 0 && npz != np)
   {
    fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(67,gettext("Итого за покупку")),gettext("Итого за покупку"),itp);
    itp=0.;
   }   

  if(kasz != 0 && kasz != kas)
   {
    sprintf(strsql,"%s %d:",gettext("Итого по месту"),kasz);
    fprintf(ff,"%*s %10.2f\n\n",iceb_u_kolbait(67,strsql),strsql,itk);
    itk=0.;
   }   
  npz=np;
  kasz=kas;

  if(row[3][0] == '1')
   {
    /*Читаем наименование материалла*/
    sprintf(strsql,"select naimat from Material where kodm=%s",row[4]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
       naim.new_plus(row1[0]);
    else
       naim.new_plus("");
   }
  else
   {
    /*Читаем наименование услуги*/
    sprintf(strsql,"select naiust from Uslugi where kodus=%s",row[4]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
       naim.new_plus(row1[0]);
    else
       naim.new_plus("");
   }

  iceb_u_rsdat(&d,&m,&g,row[0],2);      
  bb=atof(row[6])*atof(row[7]);  
  bb=iceb_u_okrug(bb,0.01);

  it+=bb;
  itp+=bb;
  itk+=bb;
  ito+=bb;
  vrem=atol(row[11]);
  
  
  bf=localtime(&vrem);

  fprintf(ff,"%02d.%02d.%d %-*.*s %-*s %10.10g %10s %10.2f\
 %-4s %-4s %-3s %02d:%02d %s\n",
  d,m,g,
  iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(3,row[5]),row[5],
  atof(row[6]),row[7],bb,
  row[3],row[1],row[2],bf->tm_hour,bf->tm_min,row[10]);

 }

fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(67,gettext("Итого за покупку")),gettext("Итого за покупку"),itp);

sprintf(strsql,"%s %d:",gettext("Итого по месту"),kasz);
fprintf(ff,"%*s %10.2f\n\n",iceb_u_kolbait(67,strsql),strsql,itk);

if(data->kassa == 0)
  fprintf(ff,"%*s: %10.2f\n\n",iceb_u_kolbait(66,gettext("Общий итог")),gettext("Общий итог"),ito);

iceb_podpis(ff,data->window);


fclose(ff);        

iceb_ustpeh(imaf,3,data->window);




gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
