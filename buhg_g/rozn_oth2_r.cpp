/*$Id: rozn_oth2_r.c,v 1.20 2013/09/26 09:46:54 sasa Exp $*/
/*23.05.2016	02.05.2005	Белых А.И. 	rozn_oth2_r.c
Расчёт
*/
#include <errno.h>
#include "buhg_g.h"
#include "rozn_oth.h"

class rozn_oth2_r_data
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
  iceb_u_str fam;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  rozn_oth2_r_data()
   {
    voz=1;
    kon_ras=1;
    fam.plus("");
    kassa=0;
   }
 };
gboolean   rozn_oth2_r_key_press(GtkWidget *widget,GdkEventKey *event,class rozn_oth2_r_data *data);
gint rozn_oth2_r1(class rozn_oth2_r_data *data);
void  rozn_oth2_r_v_knopka(GtkWidget *widget,class rozn_oth2_r_data *data);

void		rasnak(int skl,short d,short m,short g,char nn[],
double *suma,double *sumabn,double *sp,double *spbn,
short vt,double *sumkor,int lnds,FILE *ff,FILE *ffnp,double *sumsnds,
GtkWidget *wpredok);

void  sapitsh(FILE *ff);
void  rasoi(iceb_u_spisok *mssi,iceb_u_double *mspsi,FILE *ff);
void            rasis(char orgn[],double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,char naiskl[],char naik[],
iceb_u_spisok *mso,
iceb_u_double *mspo,
iceb_u_spisok *mss,
iceb_u_double *msps,
int*,int*,
FILE *ff);


void            rasid(double itd,short dn,short mn,short gn,short dk,
short mk,short gk,int skl1,char naiskl[],int*,int*,FILE *ff);

void            gsapr(short dn,short mn,short gn,short dk,short mk,
short gk,int *sl,int *kst,int skl1,char naiskl[],FILE *ff);

void  rassvoduz(iceb_u_int *KODIZDEL,iceb_u_double *KOLIHIZ,FILE *ff,GtkWidget *wpredok);

void rppvo1(int skl1,char *naiskl,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,
int *kst,
FILE *ff);

void rasmasop(iceb_u_spisok *spopr,iceb_u_double *mkm,iceb_u_double *mkm1_1r,iceb_u_double *mkm1_1k,
short tip,FILE *ff,class rozn_oth2_r_data *data);


extern SQL_baza bd;
extern double	okrg1;  /*Округление суммы*/

int rozn_oth2_r(class rozn_oth_rr *datark,int kassa,const char *fam,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class rozn_oth2_r_data data;

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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать свод розничной реализации"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rozn_oth2_r_key_press),&data);

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

repl.plus(gettext("Распечатать свод розничной реализации"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(rozn_oth2_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)rozn_oth2_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rozn_oth2_r_v_knopka(GtkWidget *widget,class rozn_oth2_r_data *data)
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

gboolean   rozn_oth2_r_key_press(GtkWidget *widget,GdkEventKey *event,class rozn_oth2_r_data *data)
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

gint rozn_oth2_r1(class rozn_oth2_r_data *data)
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
sprintf(strsql,"%s %d.%d.%d => %d.%d.%d\n",gettext("Свод розничной торговли"),dn,mn,gn,dk,mk,gk);
iceb_printw(strsql,data->buffer,data->view);

SQLCURSOR cur;

if(data->kassa != 0)
  sprintf(strsql,"select metka,kodm,kolih,cena from Roznica \
where datrp >= '%d-%02d-%02d' and datrp <= '%d-%02d-%02d' and kassa=%d order by nomer asc",
gn,mn,dn,gk,mk,dk,data->kassa);

if(data->kassa == 0)
  sprintf(strsql,"select metka,kodm,kolih,cena from Roznica \
where datrp >= '%d-%02d-%02d' and datrp <= '%d-%02d-%02d' order by kassa,nomer asc",
gn,mn,dn,gk,mk,dk);
int kolstr;
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

iceb_u_spisok KODCENA;
iceb_u_double SUMA;
iceb_u_double KOLIH;

iceb_u_int KODIZDEL;
iceb_u_double KOLIHIZ;

iceb_u_spisok KODCENAU;
iceb_u_double SUMAU;
iceb_u_double KOLIHU;

double cena=0.;
int    nomer=0;
double suma=0.;
double kolih=0;
SQLCURSOR cur1;
SQL_str row;
SQL_str row1;
int kodm;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  iceb_pbar(data->bar,kolstr,++kolstr1);
  kodm=atoi(row[1]);
  
  kolih=atof(row[2]);
  
  cena=atof(row[3]);
  cena=iceb_u_okrug(cena,0.01);

  suma=kolih*cena;
  suma=iceb_u_okrug(suma,0.01);
  if(row[0][0] == '1')
   {
    //Проверяем не является ли изделием
    sprintf(strsql,"select kodi from Specif where kodi=%d",kodm);
    if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
     {
//      printw("Изделие\n");
      if((nomer=KODIZDEL.find(kodm)) < 0)
        KODIZDEL.plus(kodm,nomer);
      KOLIHIZ.plus(kolih,nomer);
     }

    sprintf(strsql,"%d|%.10g",kodm,cena);
    if((nomer=KODCENA.find(strsql)) < 0)
      KODCENA.plus(strsql);

    SUMA.plus(suma,nomer);
    KOLIH.plus(kolih,nomer);
   }
  else
   {
    sprintf(strsql,"%d|%.10g",kodm,cena);
    if((nomer=KODCENAU.find(strsql)) < 0)
      KODCENAU.plus(strsql);
    SUMAU.plus(suma,nomer);
    KOLIHU.plus(kolih,nomer);
   }
 }


sprintf(strsql,"%s\n",gettext("Распечатываем"));
iceb_printw(strsql,data->buffer,data->view);
char imaf[30];

sprintf(imaf,"svd%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Свод розничной торговли"));
iceb_u_startfil(ff);

iceb_zagolov(gettext("Свод розничной торговли"),dn,mn,gn,dk,mk,gk,ff,data->window);

if(data->kassa != 0)
  fprintf(ff,"%s N%d\n",gettext("По рабочему месту"),data->kassa);
else
  fprintf(ff,"%s.\n",gettext("По всем рабочим местам"));


fprintf(ff,"%s.\n",gettext("Материалы"));

fprintf(ff,"\
---------------------------------------------------------------------\n");
fprintf(ff,gettext("К.мт|  Наименование товара         |Количество|  Цена    |  Сумма   |\n"));
fprintf(ff,"\
---------------------------------------------------------------------\n");
double it=0.;
int kolmatcen=KODCENA.kolih();
class iceb_u_str kodmat("");
class iceb_u_str cenach("");
class iceb_u_str naim("");
class iceb_u_str metkaiz("");
for(int i=0; i < kolmatcen; i++)
 {
  iceb_u_polen(KODCENA.ravno(i),&kodmat,1,'|');
  iceb_u_polen(KODCENA.ravno(i),&cenach,2,'|');

  /*Читаем наименование материалла*/
  sprintf(strsql,"select naimat from Material where kodm=%s",kodmat.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur1) == 1)
    naim.new_plus(row[0]);
  else
    naim.new_plus("");

  if(KODIZDEL.find(atoi(kodmat.ravno())) >= 0)
   metkaiz.new_plus(gettext("Изделие"));
  else
   metkaiz.new_plus("");
  it+=SUMA.ravno(i);

  fprintf(ff,"%4s %-*.*s %10.10g %10s %10.2f %s\n",
  kodmat.ravno(),
  iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
  KOLIH.ravno(i),cenach.ravno(),SUMA.ravno(i),metkaiz.ravno());
 }
fprintf(ff,"%56s: %10.2f\n",gettext("Итого"),it);


if((kolmatcen=KODCENAU.kolih()) > 0)
 {
  fprintf(ff,"\n%s.\n",gettext("Услуги"));

  fprintf(ff,"\
---------------------------------------------------------------------\n");
  fprintf(ff,gettext("К.ус|  Наименование услуги         |Количество|  Цена    |  Сумма   |\n"));
  fprintf(ff,"\
---------------------------------------------------------------------\n");
  it=0.;

  for(int i=0; i < kolmatcen; i++)
   {
    iceb_u_polen(KODCENAU.ravno(i),&kodmat,1,'|');
    iceb_u_polen(KODCENAU.ravno(i),&cenach,2,'|');

    /*Читаем наименование услуги*/
    sprintf(strsql,"select naius from Uslugi where kodus=%s",kodmat.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur1) == 1)
      naim.new_plus(row[0]);
    else
      naim.new_plus("");
    it+=SUMAU.ravno(i);
    fprintf(ff,"%4s %-*.*s %10.10g %10s %10.2f\n",
    kodmat.ravno(),iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),KOLIHU.ravno(i),cenach.ravno(),SUMAU.ravno(i));
   }
  fprintf(ff,"%56s: %10.2f\n",gettext("Итого"),it);
 }

//Обрабатываем изделия
rassvoduz(&KODIZDEL,&KOLIHIZ,ff,data->window);


iceb_podpis(ff,data->window);

fclose(ff);        




gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
/********************************************/
/*Распечатка спецификации входящих узлов    */
/********************************************/

void  rassvoduz(iceb_u_int *KODIZDEL,iceb_u_double *KOLIHIZ,FILE *ff,GtkWidget *wpredok)
{
int koliz=KODIZDEL->kolih();

if(koliz == 0)
 return;


iceb_u_int	kodditog(0); /*Массив кодов деталей*/
iceb_u_double kolditog(0); /*Массив количества деталей*/
iceb_u_int   metka_zap_det; //метки 0-материал 1-услуга
int kolndet=0;
int kodiz=0;
int nomer=0;
int nomdet=0,nomdet1=0;
int koddet=0;
double kolizdel=0.;
double koldet=0.;
for(nomer=0; nomer < koliz; nomer ++)
 {
  kodiz=KODIZDEL->ravno(nomer);
  kolizdel=KOLIHIZ->ravno(nomer);

  class iceb_razuz_data data;
  data.kod_izd=kodiz;
  if(iceb_razuz_kod(&data,wpredok) == 0)
   continue;  
  //Записываем в итоговые массивы
  kolndet=data.kod_det_ei.kolih();  
  for(nomdet=0; nomdet < kolndet; nomdet++)
   {
    if(metka_zap_det.ravno(nomdet) == 1)
     continue;
     
    koddet=data.kod_det.ravno(nomdet);
    koldet=data.kolih_det(koddet);

    if((nomdet1=kodditog.find(koddet)) < 0)
       kodditog.plus(koddet,nomdet1);
    kolditog.plus(koldet*kolizdel,nomdet1);

   }

 }

class iceb_u_str naim("");
double cena=0.,suma=0.;
char strsql[512];
SQL_str row;

fprintf(ff,"\n%s:\n",gettext("Материалы (детали) входящие в изделия"));
kolndet=kodditog.kolih();
SQLCURSOR cur;
for(nomer=0 ; nomer < kolndet; nomer++)
 {

  koddet=kodditog.ravno(nomer);
  koldet=kolditog.ravno(nomer);

  cena=suma=0.;
  /*Читаем наименование материалла*/
  sprintf(strsql,"select naimat,cenapr from Material where kodm=%d",koddet);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    naim.new_plus(row[0]);
    cena=atof(row[1]);
   }
  else
    naim.new_plus("");
  suma=cena*koldet;
  suma=iceb_u_okrug(suma,okrg1);
  
  fprintf(ff,"%4d %-*.*s %10.10g %10s %10.2f\n",
  koddet,iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),koldet,iceb_prcn(cena),suma);

 }

}
