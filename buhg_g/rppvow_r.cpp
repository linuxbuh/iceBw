/*$Id: rppvow_r.c,v 1.32 2013/09/26 09:46:55 sasa Exp $*/
/*17.05.2018	01.02.2005	Белых А.И.	rppvow_r.c
Расчёт реестра проводок по видам операций в материальном учёте
*/
#include <errno.h>
#include "buhg_g.h"
#include "rppvow.h"

class rppvow_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class rppvow_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  rppvow_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rppvow_r_key_press(GtkWidget *widget,GdkEventKey *event,class rppvow_r_data *data);
gint rppvow_r1(class rppvow_r_data *data);
void  rppvow_r_v_knopka(GtkWidget *widget,class rppvow_r_data *data);

void		rasnak(int skl,short d,short m,short g,char nn[],
double *suma,double *sumabn,double *sp,double *spbn,
short vt,double *sumkor,int lnds,FILE *ff,FILE *ffnp,double *sumsnds,
GtkWidget *wpredok);

void  sapitsh(short dn,short mn,short gn,short dk,short mk,short gk,int *sli,int *kst,int skl1,const char *naiskl,FILE *ff);
void rasoi(iceb_u_spisok *mssi,iceb_u_double *mspsi,short dn,short mn,short gn,short dk,short mk,short gk,int *sli,int *kst,int skl1,const char *naiskl,FILE *ff);

void rasis(const char *orgn,double itd,short dn,short mn,short gn,short dk,short mk,short gk,int skl1,const char *naiskl,const char *naik,iceb_u_spisok *mso,iceb_u_double *mspo,iceb_u_spisok *mss,iceb_u_double *msps,int*,int*,FILE *ff);

void rasio(const char *orgn,double itd,short dn,short mn,short gn,short dk,short mk,short gk,int skl1,const char *naiskl,const char *naik,iceb_u_spisok *mso,iceb_u_double *mspo,int*,int*,FILE *ff);

void rasid(double itd,short dn,short mn,short gn,short dk,short mk,short gk,int skl1,const char *naiskl,int*,int*,FILE *ff);

void gsapr(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl1,const char *naiskl,FILE *ff);

void rppvo1(int skl1,const char *naiskl,short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,FILE *ff);


extern SQL_baza bd;


extern double	nds1;
extern double   okrg1;
extern double	okrcn;
extern int kol_strok_na_liste;

int rppvow_r(class rppvow_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class rppvow_r_data data;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать реестр проводок по видам операций"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rppvow_r_key_press),&data);

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

repl.plus(gettext("Распечатать реестр проводок по видам операций"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(), iceb_get_pnk("00",0,data.window));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(rppvow_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);

gtk_text_buffer_create_tag(data.buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)rppvow_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rppvow_r_v_knopka(GtkWidget *widget,class rppvow_r_data *data)
{
//printf("rppvow_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rppvow_r_key_press(GtkWidget *widget,GdkEventKey *event,class rppvow_r_data *data)
{
// printf("rppvow_r_key_press\n");
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

gint rppvow_r1(class rppvow_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;



iceb_u_spisok    mso; /*Массив счетов по организации*/
iceb_u_double mspo; /*Массив сумм проводок по организации*/
iceb_u_spisok    mss; /*Массив счетов по складу*/
iceb_u_double msps; /*Массив сумм проводок по складу*/
iceb_u_spisok    mssi; /*Массив счетов по всем складам*/
iceb_u_double mspsi; /*Массив сумм проводок по всем складам по операции*/
iceb_u_spisok    mssio; /*Массив счетов по всем складам и всем операциям*/
iceb_u_double mspsio; /*Массив сумм проводок по всем складам по всем операциям*/
FILE            *ff;
class iceb_u_str bros("");
struct  tm      *bf;
bf=localtime(&vremn);
short           kom;
class iceb_u_str kome(""); /*Комментарий*/
short           dp,mp,gp;
short           dd,md,gd;
class iceb_u_str orgn("");
char            imaf[64];
class iceb_u_str shett("");
class iceb_u_str shett1("");
double          itd; /*Итого по счёту*/
int             i;
double          k1;
short           tipzz;
int		tz=0; /*1-Приход 2-расход*/
short           klr;
short           dn,mn,gn;
short           dk,mk,gk;
double		deb;
long		kolstr;
float           kolstr1=0.;
SQL_str         row,row1;
int		skl=0,skl1=0;
class iceb_u_str naik("");
class iceb_u_str naimpr("");
class iceb_u_str naiskl("");
class iceb_u_str nomdok("");
class iceb_u_str nomdokv("");
class iceb_u_str kprr(""),kprr1("");
class iceb_u_str kpos("");
class iceb_u_str kor("");		
class iceb_u_str nomnalnak("");
int           kst=0,sli=0;
char imafitog[64];
FILE *ffitog;


if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s %d.%d.%d => %d.%d.%d\n",gettext("Период с"),dn,mn,gn,dk,mk,gk);

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

SQLCURSOR cur,cur1;

sprintf(strsql,"select datp,sh,shk,nomd,pod,deb,datd,komen,kodkon,oper,tz from Prov \
where val=0 and \
datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and kto='%s' and deb != 0.",
gn,mn,dn,gk,mk,dk,ICEB_MP_MATU);

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

/*Сортировка через временную таблицу делается так как сортировать нужно по коду контрагента
а он может отсутствовать в проводке*/

class iceb_tmptab tabtmp;
const char *imatmptab={"rppvow"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
datp DATE not null,\
sh char(32) not null,\
shk char(32) not null,\
nomd char(32) not null,\
pod int not null,\
deb double(14,2) not null,\
datd DATE not null,\
komen char(255) not null,\
kodkon char(32) not null,\
oper char(32) not null,\
tz int not null,\
nomnalnak char(32) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  
class iceb_u_str koment("");
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[4],0,0) != 0)
    continue;
   
  sprintf(strsql,"select * from Dokummat where datd='%s' and \
sklad=%s and nomd='%s'",row[6],row[4],row[3]);
/*  printw("strsql=%s\n",strsql);*/
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    iceb_u_str repl;
    sprintf(strsql,gettext("Не найден документ N %s Склад %s Дата=%s\n"),
    row[3],row[4],row[6]);

    repl.plus(strsql);

    sprintf(strsql,"%s %s %s %s",gettext("Проводка"),
    row[1],row[2],row[3]);

    repl.plus(strsql);
    iceb_menu_soob(&repl,data->window);
    continue;
   }

  nomnalnak.new_plus(row1[5]);    
  
  tz=atoi(row1[0]);
  kor.new_plus(row1[3]);    
  kprr.new_plus(row1[6]);    

  if(data->rk->pr_ras != 0 && data->rk->pr_ras != tz)
   continue;

  if(iceb_u_proverka(data->rk->kontr.ravno(),kor.ravno(),0,0) != 0)
    continue;
   
  if(iceb_u_proverka(data->rk->kodop.ravno(),kprr.ravno(),0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),row[1],0,0) != 0)
    if(iceb_u_proverka(data->rk->shet.ravno(),row[2],0,0) != 0)
      continue;

  nomdok.new_plus(row[3]);
  nomdok.plus(" ",row1[11]);
  koment.new_plus(row[7]);
  
  sprintf(strsql,"insert into %s values ('%s','%s','%s','%s',%s,%s,'%s','%s','%s','%s',%s,'%s')",
  imatmptab,
  row[0],row[1],row[2],nomdok.ravno_filtr(),row[4],row[5],row[6],koment.ravno_filtr(),kor.ravno(),kprr.ravno(),row[10],nomnalnak.ravno());

  iceb_sql_zapis(strsql,1,0,data->window);    

 }
sprintf(strsql,"select * from %s order by tz asc,oper asc,pod asc,kodkon asc,nomd asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

kolstr1=0.;

sprintf(imafitog,"itog%d.lst",getpid());

if((ffitog = fopen(imafitog,"w")) == NULL)
 {
  iceb_er_op_fil(imafitog,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->rk->imaf.plus(imafitog);

sprintf(strsql,"%s (%s)",gettext("Реестр проводок по видам операций"),gettext("только итоги"));

data->rk->naimf.plus(strsql);

fprintf(ffitog,"%s\n",gettext("Материальный учёт"));
iceb_u_zagolov(gettext("Реестр проводок по видам операций"),dn,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ffitog);


sprintf(imaf,"rp%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Реестр проводок по видам операций"));


if(data->rk->sklad.getdlinna() > 1)
 {
  kst++;
  fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  fprintf(ffitog,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
 }

if(data->rk->kontr.getdlinna() > 1)
 {
  kst++;
  fprintf(ff,"%s:%s\n",gettext("Контрагенты"),data->rk->kontr.ravno());
  fprintf(ffitog,"%s:%s\n",gettext("Контрагенты"),data->rk->kontr.ravno());
 }
if(data->rk->pr_ras != 0)
 {
  kst++;
  memset(strsql,'\0',sizeof(strsql));
  if(data->rk->pr_ras == 1)
   strcpy(strsql,"+");
  if(data->rk->pr_ras == 2)
   strcpy(strsql,"-");
   
  fprintf(ff,"%s: %s\n",gettext("Вид операции"),strsql);
  fprintf(ffitog,"%s: %s\n",gettext("Вид операции"),strsql);
 }
if(data->rk->kodop.getdlinna() > 1)
 {
  kst++;
  fprintf(ff,"%s: %s\n",gettext("Код операции"),data->rk->kodop.ravno());
  fprintf(ffitog,"%s: %s\n",gettext("Код операции"),data->rk->kodop.ravno());
 }
if(data->rk->shet.getdlinna() > 1)
 {
  kst++;
  fprintf(ff,"%s: %s\n",gettext("Счёт"),data->rk->shet.ravno());
  fprintf(ffitog,"%s: %s\n",gettext("Счёт"),data->rk->shet.ravno());
 }

skl=0;

tipzz=kom=0;
kolstr1=0.;
klr=0;
itd=0.;
short metka_sap=0;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  iceb_u_rsdat(&dp,&mp,&gp,row[0],2);

  shett.new_plus(row[1]);
  shett.plus(",",row[2]);
  nomdok.new_plus(row[3]);
  skl1=atoi(row[4]);
  deb=atof(row[5]);
  iceb_u_rsdat(&dp,&mp,&gp,row[6],2);
  kome.new_plus(row[7]);
  
  iceb_u_rsdat(&dd,&md,&gd,row[6],2);
  
  orgn.new_plus(row[8]);

  kprr.new_plus(row[9]);
  tz=atoi(row[10]);
  nomnalnak.new_plus(row[11]);
  
  if(iceb_u_SRAV(kprr.ravno(),kprr1.ravno(),0) != 0 || tipzz != tz)
   {
    if(kprr1.ravno()[0] != '\0' )
     {
      rasis(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl,naiskl.ravno(),naik.ravno(),&mso,&mspo,&mss,&msps,&sli,&kst,ff);
      itd=0.;
      rasoi(&mssi,&mspsi,dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);

      fprintf(ff,"\f");
      kst=0;
     }
    skl=0;
    nomdokv.new_plus("");
    kpos.new_plus("");

    /*Определяем наименоввание операции*/
    if(tz == 1)
     sprintf(strsql,"select naik from Prihod where kod='%s'",
     kprr.ravno());
    if(tz == 2)
     sprintf(strsql,"select naik from Rashod where kod='%s'",
     kprr.ravno());
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %s",gettext("Не найден код операции"),kprr.ravno());
      iceb_menu_soob(strsql,data->window);
      naimpr.new_plus("");
     }
    else
      naimpr.new_plus(row1[0]);
    
    kprr1.new_plus(kprr.ravno());
    tipzz=tz;

    sprintf(strsql,"%d/%s %s\n",tz,kprr.ravno(),naimpr.ravno());
    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

    if(tz == 1)
     bros.new_plus(gettext("Приход"));
    if(tz == 2)
     bros.new_plus(gettext("Расход"));

    /*Определяем наименование склада*/
     sprintf(strsql,"select naik from Sklad where kod=%d",
     skl1);
    
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %d",gettext("Не найден склад"),skl1);
      iceb_menu_soob(strsql,data->window);
      naiskl.new_plus("");
     }
    else
      naiskl.new_plus(row1[0]);

    fprintf(ff,"%s\n",gettext("Материальный учёт"));
    fprintf(ff,"%s\n\
%s %s - \"%s\" %s %s %d.%d.%d%s %s %d.%d.%d%s\n\
%s %d.%d.%d%s  %s: %d:%d\n",
iceb_get_pnk("00",0,data->window),gettext("Реестр проводок по виду операции"),
kprr.ravno(),naimpr.ravno(),bros.ravno(),
gettext("за период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

    kst+=3;
    klr+=sli;
    sli=0;
    rppvo1(skl1,naiskl.ravno(),dn,mn,gn,dk,mk,gk,&sli,&kst,ff);
    metka_sap=0;
   }

  if(skl != skl1)
   {
    if(skl != 0)
     {
      rasis(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl,naiskl.ravno(),naik.ravno(),&mso,&mspo,&mss,&msps,&sli,&kst,ff);
      itd=0.;
      fprintf(ff,"\f");
     }
    nomdokv.new_plus("");
    kpos.new_plus("");
    kom=1;

    /*Определяем наименование склада*/
     sprintf(strsql,"select naik from Sklad where kod=%d",
     skl1);
    
    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %d",gettext("Не найден склад"),skl1);
      iceb_menu_soob(strsql,data->window);
      naiskl.new_plus("");
     }
    else
      naiskl.new_plus(row1[0]);

    sprintf(strsql,"%s: %d %s\n",gettext("Склад"),skl1,naiskl.ravno());
    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
    /*fprintf(ff,"Склад %d\n",skl1);*/
    skl=skl1;
    if(metka_sap != 0)
     rppvo1(skl1,naiskl.ravno(),dn,mn,gn,dk,mk,gk,&sli,&kst,ff);
   }

  if(iceb_u_SRAV(kpos.ravno(),orgn.ravno(),0) != 0)
   {
    if(kpos.ravno()[0] != '\0')
     {
      rasio(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl1,naiskl.ravno(),naik.ravno(),&mso,&mspo,&sli,&kst,ff);
      itd=0.;
     }
    nomdokv.new_plus("");
    kom=1;
    /*Читаем наименование организации*/
    kor.new_plus("");
    if(iceb_u_atof(orgn.ravno()) == 0. && iceb_u_polen(orgn.ravno(),&bros,2,'-') == 0 && orgn.ravno()[0] == '0')
     { 
      kor.new_plus(bros.ravno());
      sprintf(strsql,"select naik from Sklad where kod=%s",kor.ravno());
     }
    else
     {
      kor.new_plus(orgn.ravno());
      sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kor.ravno());
     }

    
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %s",gettext("Не найден код контрагента"),kor.ravno());
      iceb_menu_soob(strsql,data->window);
      naik.new_plus("");
     }
    else
      naik.new_plus(row1[0]);

    sprintf(strsql,"%s %s\n",gettext("Контрагент"),orgn.ravno());
    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
    kpos.new_plus(orgn.ravno());
   }

  if(iceb_u_SRAV(nomdok.ravno(),nomdokv.ravno(),0) != 0)
   {
    if(nomdokv.getdlinna() > 1)
     {
      rasid(itd,dn,mn,gn,dk,mk,gk,skl1,naiskl.ravno(),&sli,&kst,ff);
      itd=0.;
     }
    nomdokv.new_plus(nomdok.ravno());
   }


  /*Записываем в массив организаций*/
  if((i=mso.find(shett.ravno())) < 0)
     mso.plus(shett.ravno());
  mspo.plus(deb,i);

   /*Записываем в массив по складу*/
  if((i=mss.find(shett.ravno())) < 0)
     mss.plus(shett.ravno());
  msps.plus(deb,i);

   /*Записываем в массив по складам */
  if((i=mssi.find(shett.ravno())) < 0)
     mssi.plus(shett.ravno());
  mspsi.plus(deb,i);
   
   /*Записываем в массив по складам и операциям*/
  if((i=mssio.find(shett.ravno())) < 0)
     mssio.plus(shett.ravno());
  mspsio.plus(deb,i);
   

  sprintf(strsql,"%02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %-*s %15.2f\n",
  dp,mp,gp,dd,md,gd,
  iceb_u_kolbait(11,nomdok.ravno()),nomdok.ravno(),
  iceb_u_kolbait(6,nomnalnak.ravno()),nomnalnak.ravno(),
  iceb_u_kolbait(10,shett.ravno()),shett.ravno(),
  deb);
  
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

  metka_sap++;
  
  gsapr(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);
  if(kom == 1)
     fprintf(ff,"\
%-*s %-*.*s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %-*s %20s %s\n",
     iceb_u_kolbait(7,orgn.ravno()),orgn.ravno(),
     iceb_u_kolbait(40,naik.ravno()),iceb_u_kolbait(40,naik.ravno()),naik.ravno(),
     dd,md,gd,dp,mp,gp,
     iceb_u_kolbait(11,nomdok.ravno()),nomdok.ravno(),
     iceb_u_kolbait(6,nomnalnak.ravno()),nomnalnak.ravno(),
     iceb_u_kolbait(10,shett.ravno()),shett.ravno(),
     iceb_u_prnbr(deb),kome.ravno());

  if(kom == 0)
   fprintf(ff,"\
%48s %02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %-*s %20s %s\n",
   " ",dd,md,gd,dp,mp,gp,
   iceb_u_kolbait(11,nomdok.ravno()),nomdok.ravno(),
   iceb_u_kolbait(6,nomnalnak.ravno()),nomnalnak.ravno(),
   iceb_u_kolbait(10,shett.ravno()),shett.ravno(),
   iceb_u_prnbr(deb),kome.ravno());

  itd+=deb;
  kom=0;

 }


rasis(orgn.ravno(),itd,dn,mn,gn,dk,mk,gk,skl,naiskl.ravno(),naik.ravno(),&mso,&mspo,&mss,&msps,&sli,&kst,ff);
rasoi(&mssi,&mspsi,dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);



class iceb_u_str shdeb("");
class iceb_u_str shkre("");

if(data->rk->kodop.getdlinna() <= 0 || 
(data->rk->kodop.getdlinna() > 1 && iceb_u_pole(data->rk->kodop.ravno(),&shett1,2,',') == 0))
 {
  k1=0.;
  fprintf(ff,"\n==================================================\n\
%s\n",gettext("Общий итог по всем операциям:"));
  sapitsh(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ff);
  for(i=0; i< mssio.kolih() ;i++)
   {
    iceb_u_polen(mssio.ravno(i),&shdeb,1,',');
    iceb_u_polen(mssio.ravno(i),&shkre,2,',');
    fprintf(ff,"%-*s %-*s %15s\n",
    iceb_u_kolbait(6,shdeb.ravno()),shdeb.ravno(),
    iceb_u_kolbait(6,shkre.ravno()),shkre.ravno(),
    iceb_u_prnbr(mspsio.ravno(i)));
    k1+=mspsio.ravno(i);
   }
  fprintf(ff,"%*s:%15s\n",iceb_u_kolbait(13,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));
 }


sprintf(strsql,"\n%s\n",gettext("Общий итог по всем операциям:"));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

fprintf(ffitog,"\n%s\n",gettext("Общий итог по всем операциям:"));
k1=0.;

fprintf(ffitog,"\
------------------------------\n");
fprintf(ffitog,gettext("\
Дебет |Кредит|    Сумма      |\n"));
fprintf(ffitog,"\
------------------------------\n");

for(i=0; i< mssio.kolih() ;i++)
 {
  sprintf(strsql,"%-*s - %15s\n",iceb_u_kolbait(15,mssio.ravno(i)),mssio.ravno(i),iceb_u_prnbr(mspsio.ravno(i)));

  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

  iceb_u_polen(mssio.ravno(i),&shdeb,1,',');
  iceb_u_polen(mssio.ravno(i),&shkre,2,',');
  fprintf(ffitog,"%-*s %-*s %15s\n",
  iceb_u_kolbait(6,shdeb.ravno()),shdeb.ravno(),
  iceb_u_kolbait(6,shkre.ravno()),shkre.ravno(),
  iceb_u_prnbr(mspsio.ravno(i)));
  k1+=mspsio.ravno(i);
 }
fprintf(ffitog,"%*s:%15s\n",iceb_u_kolbait(13,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));

iceb_podpis(ffitog,data->window);
iceb_podpis(ff,data->window);

fclose(ffitog);
fclose(ff);


klr+=sli;
sprintf(strsql,"%s: %d\n",gettext("Количество листов"),klr);

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);
 



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
/******************************/
/* Шапка итога по счетам      */
 /******************************/

void  sapitsh(short dn,short mn,short gn,short dk,short mk,short gk,int *sli,int *kst,int skl1,const char *naiskl,FILE *ff)
{
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"\
------------------------------\n");

gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,gettext("\
Дебет |Кредит|    Сумма      |\n"));

gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"\
------------------------------\n");

}
/*******************************/
/*Распечатка итога по контрагенту*/
/*******************************/
void rasio(const char *orgn,double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,const char *naiskl,const char *naik,
iceb_u_spisok *mso,
iceb_u_double *mspo,
int *sli,
int *kst,
FILE *ff)
{
short           i;
class iceb_u_str shett1("");
double          bb;
class iceb_u_str shdeb("");
class iceb_u_str shkre("");


rasid(itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,sli,kst,ff);

/*Распечатываем результаты по организации*/
bb=0.;
if(mso->kolih() > 0)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%s \"%s\":\n",gettext("Итого по контрагенту"),naik);
 }

sapitsh(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);

for(i=0; i< mso->kolih() ;i++)
 {
  shett1.new_plus(mso->ravno(i));
  iceb_u_polen(shett1.ravno(),&shdeb,1,',');
  iceb_u_polen(shett1.ravno(),&shkre,2,',');

  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%-*s %-*s %15s\n",
  iceb_u_kolbait(6,shdeb.ravno()),shdeb.ravno(),
  iceb_u_kolbait(6,shkre.ravno()),shkre.ravno(),
  iceb_u_prnbr(mspo->ravno(i)));

  bb+=mspo->ravno(i);
 }
if(mso->kolih() > 0)
 {
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%*s:%15s\n",iceb_u_kolbait(13,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(bb));
  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
 }

mso->free_class();
mspo->free_class();
}

/*******************************/
/*Распечатка итога по складу*/
/*******************************/
void rasis(const char *orgn,double itd,short dn,short mn,short gn,
short dk,short mk,short gk,int skl1,const char *naiskl,const char *naik,
iceb_u_spisok *mso,
iceb_u_double *mspo,
iceb_u_spisok *mss,
iceb_u_double *msps,
int *sli,
int *kst,
FILE *ff)
{
short           i;
class iceb_u_str shett1("");
double          bb;
class iceb_u_str shdeb("");
class iceb_u_str shkre("");

rasio(orgn,itd,dn,mn,gn,dk,mk,gk,skl1,naiskl,naik,mso,mspo,sli,kst,ff);

/*Распечатываем результаты по складу*/
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"%s \"%s\":\n",gettext("Итого по складу"),naiskl);

sapitsh(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);

bb=0.;
for(i=0; i< mss->kolih() ;i++)
 {
  shett1.new_plus(mss->ravno(i));

  iceb_u_polen(shett1.ravno(),&shdeb,1,',');
  iceb_u_polen(shett1.ravno(),&shkre,2,',');

  gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
  fprintf(ff,"%-*s %-*s %15s\n",
  iceb_u_kolbait(6,shdeb.ravno()),shdeb.ravno(),
  iceb_u_kolbait(6,shkre.ravno()),shkre.ravno(),
  iceb_u_prnbr(msps->ravno(i)));

  bb+=msps->ravno(i);
 }

gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"%*s:%15s\n",iceb_u_kolbait(13,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(bb));

gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
fprintf(ff,"\
......................................................................................................................................\n");

mss->free_class();
msps->free_class();
}

/*************************/
/*Распечатка общего итога*/
/*************************/
void rasoi(iceb_u_spisok *mssi,
iceb_u_double *mspsi,
short dn,short mn,short gn,short dk,short mk,short gk,int *sli,int *kst,int skl1,const char *naiskl,
FILE *ff)
{
double          k1;
short           i;
class iceb_u_str shett1("");
class iceb_u_str shdeb("");
class iceb_u_str shkre("");


k1=0.;
fprintf(ff,"\n%s:\n",gettext("Общий итог"));

sapitsh(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);
//sapitsh(ff);
for(i=0; i< mssi->kolih() ;i++)
 {
  shett1.new_plus(mssi->ravno(i));
  iceb_u_polen(shett1.ravno(),&shdeb,1,',');
  iceb_u_polen(shett1.ravno(),&shkre,2,',');
  fprintf(ff,"%-*s %-*s %15s\n",
  iceb_u_kolbait(6,shdeb.ravno()),shdeb.ravno(),
  iceb_u_kolbait(6,shkre.ravno()),shkre.ravno(),
  iceb_u_prnbr(mspsi->ravno(i)));
  k1+=mspsi->ravno(i);
 }
fprintf(ff,"%*s:%15s\n",iceb_u_kolbait(13,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(k1));


mspsi->free_class();
mssi->free_class();
}
/*******************************/
/*Распечатка итога по документа*/
/*******************************/
void rasid(double itd,short dn,short mn,short gn,short dk,
short mk,short gk,int skl1,const char *naiskl,int *sli,int *kst,FILE *ff)
{
gsapr(dn,mn,gn,dk,mk,gk,sli,kst,skl1,naiskl,ff);

fprintf(ff,"%*s: %20s\n",
iceb_u_kolbait(99,gettext("Итого по документу")),
gettext("Итого по документу"),
iceb_u_prnbr(itd));

}
/*******/
/*Шапка*/
/*******/
void gsapr(short dn,short mn,short gn,short dk,short mk,
short gk,int *sl,int *kst,int skl1,const char *naiskl,FILE *ff)
{
*kst+=1;

if(*kst <=  kol_strok_na_liste)
 return;

fprintf(ff,"\f");
*kst=1;
rppvo1(skl1,naiskl,dn,mn,gn,dk,mk,gk,sl,kst,ff);
  

}
/************************************/
/*Шапка*/
/************************************/
void rppvo1(int skl1,const char *naiskl,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,
int *kst,
FILE *ff)
{
*sl+=1;
*kst+=4;

fprintf(ff,"\
%s N%-2d %-30s %s %02d.%02d.%4d%s %s %02d.%02d.%4d%s %30s%s N%d\n",
gettext("Склад"),
skl1,naiskl,
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
"",gettext("Лист"),
*sl);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
         Контрагент                             |Дата док. |Дата пров.|N документа|Nнал.н|  Счета   |       Сумма        |Комментарий\n"));
/*
1234567890123456789012345678901234567890123456789
*/
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------\n");

}
