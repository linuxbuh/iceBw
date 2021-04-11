/*$Id: rasspdok_r.c,v 1.27 2013/09/26 09:46:54 sasa Exp $*/
/*05.02.2020	01.02.2005	Белых А.И.	rasspdok_r.c
Расчёт отчёта по списку документов в материальном учёте
*/
#include <errno.h>
#include "buhg_g.h"
#include "rasspdok.h"

class rasspdok_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class rasspdok_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  rasspdok_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rasspdok_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasspdok_r_data *data);
gint rasspdok_r1(class rasspdok_r_data *data);
void  rasspdok_r_v_knopka(GtkWidget *widget,class rasspdok_r_data *data);

void rasnak(int skl,short d,short m,short g,const char *nn,double *suma,double *sumabn,double *sp,double *spbn,
short vt,double *sumkor,int lnds,FILE *ff,FILE *ffnp,double *sumsnds,
float pnds,
GtkWidget *wpredok);

extern SQL_baza bd;


extern double   okrg1;
extern double	okrcn;

int rasspdok_r(class rasspdok_rr *datark,GtkWidget *wpredok)
{
char strsql[1024];
int gor=0;
int ver=0;
class iceb_u_str repl;
class rasspdok_r_data data;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать движение товарно-материальных ценностей (форма 1)"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rasspdok_r_key_press),&data);

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

repl.plus(gettext("Распечатать движение товарно-материальных ценностей (форма 1)"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(rasspdok_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)rasspdok_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rasspdok_r_v_knopka(GtkWidget *widget,class rasspdok_r_data *data)
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

gboolean   rasspdok_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasspdok_r_data *data)
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

gint rasspdok_r1(class rasspdok_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);


int vt=0;

memset(strsql,'\0',sizeof(strsql));
iceb_poldan("Код группы возвратная тара",strsql,"matnast.alx",data->window);
vt=atoi(strsql);
short dn,mn,gn;
short dk,mk,gk;
iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window);

sprintf(strsql,"%s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Распечатка документов за период c"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."));

iceb_printw(strsql,data->buffer,data->view);


/*Готовим файл для сортировки*/
sprintf(strsql,"select * from Dokummat where datd >= '%d-%02d-%02d' \
and datd <= '%d-%02d-%02d'",gn,mn,dn,gk,mk,dk);
int kolstr;
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  data->kon_ras=0;
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  data->kon_ras=0;
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);
  return(FALSE);
 }

SQL_str row,rowtmp;
char data_nal_nak[32];
short dnn,mnn,gnn;
SQL_str row1;
SQLCURSOR cur1,curtmp;

class iceb_tmptab tabtmp;
const char *imatmptab={"rasspdok"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
skl int not null,\
datd DATE not null,\
ndpp int not null,\
nomdok char(32) not null,\
kontr char(32) not null,\
kodop char(32) not null,\
nomnn char(32) not null,\
pn float(5,2) not null,\
datnnal char(16) not null,\
nomdokp char(32) not null,\
tipz smallint not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  


float kolstr1=0.;
int ndpp;
short d,m,g;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(data->rk->metka_pr != 0)
   if(data->rk->metka_pr != atoi(row[0]))
       continue;

  if(data->rk->metka_dok == 0)
   if(atoi(row[7]) != 1 )
       continue;

  if(data->rk->metka_dok == 1)
   if(atoi(row[7]) != 0 )
       continue;

  if(iceb_u_proverka(data->rk->kontr.ravno(),row[3],0,0) != 0)
     continue;

  if(iceb_u_proverka(data->rk->kodop.ravno(),row[6],0,0) != 0)
     continue;

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[2],0,0) != 0)
     continue;
  if(data->rk->grupk.getdlinna() > 1)
   {
    if(iceb_u_SRAV(row[3],"00-",1) == 0)
     sprintf(strsql,"select grup from Kontragent where kodkon='%s'","00");
    else
     sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[3]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      if(iceb_u_proverka(data->rk->grupk.ravno(),row1[0],0,0) != 0)
       continue;
     }

   }

  /* Период*/

  if(iceb_u_pole(row[4],strsql,2,'-') != 0)
   if(iceb_u_pole(row[4],strsql,2,'+') != 0)
    strcpy(strsql,row[4]);
  ndpp=atoi(strsql);

  iceb_u_rsdat(&dnn,&mnn,&gnn,row[12],2);
  memset(data_nal_nak,'\0',sizeof(data_nal_nak));
  if(dnn > 0)
   sprintf(data_nal_nak,"%02d.%02d.%d",dnn,mnn,gnn);
  
  iceb_u_rsdat(&d,&m,&g,row[1],2);

  sprintf(strsql,"insert into %s values (%d,'%04d-%02d-%02d',%d,'%s','%s','%s','%s',%s,'%s','%s',%s)",
  imatmptab,
  atoi(row[2]),g,m,d,ndpp,row[4],row[3],row[6],row[5],row[13],data_nal_nak,row[11],row[0]);

  iceb_sql_zapis(strsql,1,0,data->window);    


 }


sprintf(strsql,"%s.\n",gettext("Сортируем записи"));

iceb_printw(strsql,data->buffer,data->view);

if(data->rk->metka_sort == 0)
  sprintf(strsql,"select * from %s order by datd,ndpp,kontr,kodop asc",imatmptab);
if(data->rk->metka_sort == 1)
  sprintf(strsql,"select * from %s order by ndpp,kontr,kodop asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s\n",gettext("Печатаем"));
iceb_printw(strsql,data->buffer,data->view);


char imaf[64];
FILE *ff;

sprintf(imaf,"sp%d.lst",getpid());
data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Распечатка списка документов"));

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,NULL);
  data->kon_ras=0;
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);
  return(FALSE);
 }
char imaf2[64];
FILE *ff2;
sprintf(imaf2,"spn%d.lst",getpid());
data->rk->imaf.plus(imaf2);
data->rk->naimf.plus(gettext("Распечатка содержимого документов"));
if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,NULL);
  data->kon_ras=0;
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);
  return(FALSE);
 }
char imafnp[64];
sprintf(imafnp,"spnp%d.lst",getpid());
data->rk->imaf.plus(imafnp);
data->rk->naimf.plus(gettext("Распечатка неподтверждённых записей в документах"));
FILE *ffnp;
if((ffnp = fopen(imafnp,"w")) == NULL)
 {
  iceb_er_op_fil(imafnp,"",errno,NULL);
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
  gtk_widget_grab_focus(data->knopka);
  return(FALSE);
 }

iceb_u_zagolov(gettext("Распечатка документов"),dn,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ff);
iceb_u_zagolov(gettext("Распечатка документов"),dn,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ff2);
iceb_u_zagolov(gettext("Распечатка документов"),dn,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ffnp);



fprintf(ffnp,"\n%s !\n",gettext("Только не подтвержденные записи в документах"));

if(data->rk->metka_pr == 1)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО ПРИХОДЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО ПРИХОДЫ"));
  fprintf(ffnp,"%s\n",gettext("ТОЛЬКО ПРИХОДЫ"));
 }
if(data->rk->metka_pr == 2)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО РАСХОДЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО РАСХОДЫ"));
  fprintf(ffnp,"%s\n",gettext("ТОЛЬКО РАСХОДЫ"));
 }
if(data->rk->metka_dok == 0)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ffnp,"%s\n",gettext("ТОЛЬКО ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
 }
if(data->rk->metka_dok == 1)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО НЕ ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО НЕ ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ffnp,"%s\n",gettext("ТОЛЬКО НЕ ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
 }
if(data->rk->metka_dok == 3)
 {
  fprintf(ff,"%s\n",gettext("ВСЕ ДОКУМЕНТЫ"));
  fprintf(ff2,"%s\n",gettext("ВСЕ ДОКУМЕНТЫ"));
  fprintf(ffnp,"%s\n",gettext("ВСЕ ДОКУМЕНТЫ"));
 }

if(data->rk->kodop.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
  fprintf(ffnp,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
 }
if(data->rk->kontr.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
  fprintf(ffnp,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
 }

if(data->rk->sklad.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  fprintf(ffnp,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
 }

if(data->rk->grupk.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код группы контрагента"),data->rk->grupk.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код группы контрагента"),data->rk->grupk.ravno());
  fprintf(ffnp,"%s:%s\n",gettext("Код группы контрагента"),data->rk->grupk.ravno());
 }


fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("  Дата    |Дата отср.|Ном.док. |Нал. накл.|Дата н.нак|  Сумма  |Сум.кop.|Сумма с НДС|  По кассе |Контр.|  Наименование контрагента    |Код оп.|   Телефон   |\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

double itog=0.,itogsk=0.,itognds=0.;
double sp=0.,spbn=0.;
double isumsnds=0.,sumsnds=0.;
double sumndspr=0.;
double bb,bb1;
class iceb_u_str nn("");
class iceb_u_str kor("");
class iceb_u_str kop("");
class iceb_u_str nnn("");
class iceb_u_str naior("");
class iceb_u_str telefon("");
class iceb_u_str kor1("");
class iceb_u_str otsr("");
int lnds;
int skl;
double suma=0.;
double sumabn=0.;
double sumkor=0.;
double ndssum=0.;
short		shek;
kolstr1=0;
float pnds=0.;
class iceb_u_str nomdokp("");
int tipz=0;
gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);
while(curtmp.read_cursor(&rowtmp) != 0)
 {

  skl=atoi(rowtmp[0]);
  iceb_u_rsdat(&d,&m,&g,rowtmp[1],2);
  nn.new_plus(rowtmp[3]);
  kor.new_plus(rowtmp[4]);
  kop.new_plus(rowtmp[5]);
  nnn.new_plus(rowtmp[6]);
  pnds=(float)atof(rowtmp[7]);
  strncpy(data_nal_nak,rowtmp[8],sizeof(data_nal_nak)-1);
  nomdokp.new_plus(rowtmp[9]);
  tipz=atoi(rowtmp[10]);
        
  naior.new_plus("");
  telefon.new_plus("");
  kor1.new_plus("");
  if(kor.ravno()[0] == '0' && kor.ravno()[1] == '0' && iceb_u_polen(kor.ravno(),&kor1,2,'-') == 0)
    sprintf(strsql,"select naik from Sklad where kod='%s'",kor1.ravno());
  else
    sprintf(strsql,"select naikon,telef from Kontragent where kodkon='%s'",kor.ravno());

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    if(kor1.ravno()[0] == '\0')
     {
      fprintf(ff,"%s - %s\n",gettext("Не найден контрагент"),kor.ravno());
      sprintf(strsql,"%s %s",gettext("Не найден контрагент"),kor.ravno());
      iceb_menu_soob(strsql,data->window);
     }
    else
     {
      fprintf(ff,"%s %s\n",gettext("Не найден код склада"),kor1.ravno());
      sprintf(strsql,"%s %s",gettext("Не найден код склада"),kor1.ravno());
      iceb_menu_soob(strsql,data->window);
     }
   }
  else
   {
    naior.new_plus(row1[0]);
    if(kor1.ravno()[0] == '\0')
      telefon.new_plus(row1[1]);
   }
 
  /*Узнаем дату отсрочки платежа*/

  sprintf(strsql,"select sodz from Dokummat2 \
  where god=%d and nomd='%s' and sklad=%d and nomerz=9",
  g,nn.ravno(),skl);

  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    otsr.new_plus(row1[0]);   
  else
    otsr.new_plus("");
  /*Узнаем НДС документа*/
  sprintf(strsql,"select sodz from Dokummat2 \
  where god=%d and nomd='%s' and sklad=%d and nomerz=11",
  g,nn.ravno(),skl);
  lnds=0;
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     lnds=atoi(row1[0]);

  /*Узнаем Был ли распечатан кассовый чек*/
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=5",
  g,nn.ravno(),skl);
  shek=0;
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     shek=atoi(row1[0]);

  sumndspr=0.;

  //Читаем введенную сумму НДС для приходного документа
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=6",
  g,nn.ravno(),skl);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     sumndspr=atoi(row1[0]);

  /*Узнаем сумму записей по накладной*/
  
  fprintf(ff2,"\n%d.%d.%d %s (%s) %s %s\n",d,m,g,nn.ravno(),nomdokp.ravno(),kor.ravno(),naior.ravno());

  fprintf(ffnp,"\n%d.%d.%d %s (%s) %s %s\n",d,m,g,nn.ravno(),nomdokp.ravno(),kor.ravno(),naior.ravno());

  rasnak(skl,d,m,g,nn.ravno(),&suma,&sumabn,&sp,&spbn,vt,&sumkor,lnds,ff2,ffnp,
  &sumsnds,pnds,data->window);

  if(lnds == 0 || lnds == 4)
   {
    if(sumndspr != 0.)
     ndssum=sumndspr;
    else
     ndssum=(suma+sumkor)*pnds/100.;
    bb=suma+sumkor+sumabn+ndssum;
   }
  else
    bb=suma+sumkor+sumabn;

  bb=iceb_u_okrug(bb,okrg1);
  bb1=suma+sumabn;
  itog+=bb1;
  itognds+=bb;

  if(shek != 0)
   {
    isumsnds+=sumsnds;
   }
  else
   sumsnds=0.;  
  itogsk+=sumkor;

  sprintf(strsql,"%02d.%02d.%d %-*s %-*.*s %-*s %9.2f %9.8g %10.2f %9.2f\n",
  d,m,g,
  iceb_u_kolbait(7,kor.ravno()),kor.ravno(),
  iceb_u_kolbait(15,naior.ravno()),iceb_u_kolbait(15,naior.ravno()),naior.ravno(),
  iceb_u_kolbait(9,nn.ravno()),nn.ravno(),
  bb1,sumkor,bb,sumsnds);

//  iceb_printw(strsql,data->buffer,data->view);
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

  fprintf(ff,"%02d.%02d.%d %10.10s %-*s %-*s %10s %9.2f %8.8g %11.2f %11.2f %-*s %-*.*s \
%-*s %s\n",
  d,m,g,
  otsr.ravno(),
  iceb_u_kolbait(9,nn.ravno()),nn.ravno(),
  iceb_u_kolbait(10,nnn.ravno()),nnn.ravno(),
  data_nal_nak,bb1,sumkor,bb,sumsnds,
  iceb_u_kolbait(6,kor.ravno()),kor.ravno(),
  iceb_u_kolbait(30,naior.ravno()),iceb_u_kolbait(30,naior.ravno()),naior.ravno(),
  iceb_u_kolbait(7,kop.ravno()),kop.ravno(),
  telefon.ravno());

  if(tipz == 1)  
   {
    sprintf(strsql,"select vido from Prihod where kod='%s'",kop.ravno());
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     if(atoi(row1[0]) == 0)    
      if(nomdokp.getdlinna() > 1)
        fprintf(ff,"%10s %10s %-*s\n","","",iceb_u_kolbait(9,nomdokp.ravno()),nomdokp.ravno());
   }

 }

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"%*s: %10.2f %8.8g %11.2f %11.2f\n",
iceb_u_kolbait(51,gettext("Итого")),gettext("Итого"),itog,itogsk,itognds,isumsnds);

sprintf(strsql,"%*s: %9.2f %9.8g %10.2f %9.2f\n",
iceb_u_kolbait(43,gettext("Итого")),gettext("Итого"),itog,itogsk,itognds,isumsnds);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
//iceb_printw(strsql,data->buffer,data->view);

iceb_podpis(ff,data->window);


fprintf(ff2,"%s:\n\
-----------------------------------------------------------------------------------------------\n",
gettext("Общий итог"));

fprintf(ff2,"%*s: %10.2f %10s %10.2f\n",
iceb_u_kolbait(60,gettext("Итого")),gettext("Итого"),itog," ",sp+spbn);

bb=sp+spbn+sp*pnds/100.;
bb=iceb_u_okrug(bb,okrg1);
fprintf(ff2,"%*s %10.2f %10s %10.2f\n",
iceb_u_kolbait(61,gettext("Итого с НДС")),gettext("Итого с НДС"),itognds," ",bb);

iceb_podpis(ff2,data->window);
iceb_podpis(ffnp,data->window);

fclose(ff);
fclose(ff2);
fclose(ffnp);


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
/*********************************************************************/

void rasnak(int skl,short d,short m,short g,const char *nn,
double *suma,double *sumabn,double *sp,double *spbn,
short vt,double *sumkor,int lnds,FILE *ff,FILE *ffnp,double *sumsnds,
float pnds,
GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row,row1;
char		strsql[1024];
double		bb,bb1;
short		ks;
double		kol;
double		itog[2];
double		sumap,sumapbn;
class iceb_u_str naim("");
class iceb_u_str ei("");
short		vtr;
double		kolih,cena;
int		nk;
short		kgrm;
short		mnds; /*1-НДС в том числе*/
char		nomn[128]; /*Номенклатурный номер*/
class iceb_u_str shu("");
double		sumands=0.;
double		sumandspod=0.;
double		sumandspr=0.; //Сумма НДС для приходного документа введенная вручную
SQLCURSOR cur;
SQLCURSOR cur1;


/*Узнаем сумму скидки*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=13",
g,nn,skl);

*sumkor=0.;
if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  *sumkor=atof(row[0]);

//Узнаем сумму НДС
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=6",
g,nn,skl);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  sumandspr=atof(row[0]);


/*Узнаем сумму всех записей*/
*sumsnds=*suma=*sumabn=0.;
sumap=sumapbn=0.;

sprintf(strsql,"select kodm,kolih,cena,voztar,ei,nomkar,mnds \
from Dokummat1 where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' \
order by kodm asc",g,m,d,skl,nn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
  return;

fprintf(ff,"-----------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext(" N |       Наименование продукции |Ед. |Количество|  Ц е н а | С у м м а|Количество|Сумма под-|\n"));
fprintf(ff,gettext("   |              (товару)        |изм.|          |          |          |подтвержд.|тверждена |\n"));
fprintf(ff,"-----------------------------------------------------------------------------------------------\n");

fprintf(ffnp,"-----------------------------------------------------------------------------------------------\n");
fprintf(ffnp,gettext(" N |       Наименование продукции |Ед. |Количество|  Ц е н а | С у м м а|Количество|Сумма под-|\n"));
fprintf(ffnp,gettext("   |              (товару)        |изм.|          |          |          |подтвержд.|тверждена |\n"));
fprintf(ffnp,"-----------------------------------------------------------------------------------------------\n");

ks=0;
memset(nomn,'\0',sizeof(nomn));
itog[0]=itog[1]=0.;
while(cur.read_cursor(&row) != 0)
 {
/*   printw("*%d %d %s\n",mk2.g,mk2.skl,mk2.nn);*/
  kolih=atof(row[1]);
  cena=atof(row[2]);
//Для вложенного налога
  if(lnds == 0 || lnds == 4)
    bb=cena+cena*pnds/100.;
  else
    bb=cena;

  bb=iceb_u_okrug(bb,okrg1);
 
  bb=bb*kolih;

  *sumsnds+=iceb_u_okrug(bb,okrg1);

  cena=iceb_u_okrug(cena,okrcn);
  vtr=atoi(row[3]);
  ei.new_plus(row[4]);
  nk=atoi(row[5]);
  mnds=atoi(row[6]);  
  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);

  /*Узнаем код группы*/
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%s",
  row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    printf("%s %s !\n",gettext("Не найден код материала"),row[0]);
    naim.new_plus("");
    kgrm=0;
   }
  else
   {
    naim.new_plus(row1[0]);
    kgrm=atoi(row1[1]);
   }  
  if(vtr == 1 || kgrm == vt || (lnds != 0 && lnds != 4) || mnds == 1)
    *sumabn=*sumabn+bb;
  else
    *suma=*suma+bb;

  kol=readkolkw(skl,nk,d,m,g,nn,wpredok);  
    
  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);

  bb1=cena*kol;
  bb1=iceb_u_okrug(bb1,okrg1);

  ks++;
  /*Узнаем счёт учета для номенклатурного номера*/
  memset(nomn,'\0',sizeof(nomn));
  if(nk != 0)
   {
    sprintf(strsql,"select shetu from Kart where sklad=%d and nomk=%d",
    skl,nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      printf("Не найдена карточка %d на складе %d !\n",nk,skl);
      shu.new_plus("");
     }
    else
     {
      shu.new_plus(row1[0]);
     }
    sprintf(nomn,"%d.%s.%s.%d",skl,shu.ravno(),row[0],nk);
   }

  fprintf(ff,"%3d %-*.*s %-*.*s %10.10g %10.10g %10.2f %10.10g %10.2f %s\n",
  ks,
  iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(4,ei.ravno()),iceb_u_kolbait(4,ei.ravno()),ei.ravno(),
  kolih,cena,bb,kol,bb1,nomn);

  if(kol == 0.)
   fprintf(ffnp,"%3d %-*.*s %-*.*s %10.10g %10.10g %10.2f %10.10g %10.2f %s\n",
   ks,
  iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(4,ei.ravno()),iceb_u_kolbait(4,ei.ravno()),ei.ravno(),
   kolih,cena,bb,kol,bb1,nomn);

  if(vtr == 1 || kgrm == vt || (lnds != 0 && lnds != 4) || mnds == 1)
    sumapbn+=bb1;
  else
    sumap+=bb1;

 }

/*Читаем услуги*/

sprintf(strsql,"select * from Dokummat3 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s'",g,m,d,skl,nn);
/*printw("\nstrsql=%s\n",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {

  kolih=atof(row[4]);
  cena=atof(row[5]);
  cena=iceb_u_okrug(cena,okrcn);

//Для вложенного налога
  bb=cena+cena*pnds/100.;
  bb=iceb_u_okrug(bb,okrg1);
  bb=bb*kolih;
  *sumsnds+=iceb_u_okrug(bb,okrg1);
  
  if(kolih > 0)
    bb=kolih*cena;
  else
    bb=cena;
  bb=iceb_u_okrug(bb,okrg1);
  *suma=*suma+iceb_u_okrug(bb,okrg1);

  ks++;
  fprintf(ff,"%3d %-*.*s %-*.*s %10.10g %10s",
  ks,
  iceb_u_kolbait(30,row[7]),iceb_u_kolbait(30,row[7]),row[7],
  iceb_u_kolbait(4,row[3]),iceb_u_kolbait(4,row[3]),row[3],
  kolih,iceb_prcn(cena));
  fprintf(ff," %10s\n",iceb_prcn(bb));

 }



itog[0]=*suma+*sumabn;
itog[1]=sumap+sumapbn;

fprintf(ff,"%*s %10.2f %10s %10.2f\n",
iceb_u_kolbait(61,gettext("Итого")),gettext("Итого"),itog[0]," ",itog[1]);

if(*sumkor != 0)
 {
  bb=*sumkor*100./itog[0];
  bb=iceb_u_okrug(bb,0.01);
  if(*sumkor < 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Скидка"),bb*(-1));
  if(*sumkor > 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Надбавка"),bb);

  fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(61,strsql),strsql,*sumkor);

  if(*sumkor < 0)
      sprintf(strsql,gettext("Итого со скидкой :"));
  if(*sumkor > 0)
      sprintf(strsql,gettext("Итого с надбавкой :"));

   fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(61,strsql),strsql,*sumkor+itog[0]);

 }
sumandspod=sumands=0.;
if(lnds == 0 || lnds == 4)
 {
  if(sumandspr != 0.)
   sumands=sumandspr;
  else 
   sumands=( *suma + *sumkor)*pnds/100.;
 }

bb=*suma+*sumkor+*sumabn+sumands;
bb=iceb_u_okrug(bb,okrg1);
if(lnds == 0 || lnds == 4)
  sumandspod=sumap*pnds/100.;

bb1=sumap+sumapbn+sumandspod;
bb1=iceb_u_okrug(bb1,okrg1);
sumands=

fprintf(ff,"%*s: %10.2f %10s %10.2f\n",
iceb_u_kolbait(60,gettext("НДС")),gettext("НДС"),sumands," ",sumandspod);

fprintf(ff,"%*s %10.2f %10s %10.2f\n",
iceb_u_kolbait(61,gettext("Итого с НДС")),gettext("Итого с НДС"),bb," ",bb1);

*sp=*sp+sumap;
*spbn=*spbn+sumapbn;

}
