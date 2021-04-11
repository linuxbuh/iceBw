/*$Id: rasspdokuw_r.c,v 1.23 2013/09/26 09:46:54 sasa Exp $*/
/*18.05.2018	22.12.2005	Белых А.И.	rasspdokuw_r.c
Расчёт отчёта по списку документов в учёте услуг
*/
#include <errno.h>
#include "buhg_g.h"
#include "rasspdokuw.h"

class rasspdokuw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class rasspdokuw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  rasspdokuw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rasspdokuw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasspdokuw_r_data *data);
gint rasspdokuw_r1(class rasspdokuw_r_data *data);
void  rasspdokuw_r_v_knopka(GtkWidget *widget,class rasspdokuw_r_data *data);

void rasnaku(int skl,short d,short m,short g,const char *nn,double *suma,double *sumabn,double *sumav,double *sp,double *spbn,
short vt,double sumkor,int lnds,FILE *ff,GtkWidget *wpredok);

extern SQL_baza bd;




extern double   okrg1;
extern double	okrcn;

int rasspdokuw_r(class rasspdokuw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class rasspdokuw_r_data data;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Движение услуг"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rasspdokuw_r_key_press),&data);

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

repl.plus(gettext("Движение услуг"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,data.window));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(rasspdokuw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);

gtk_text_buffer_create_tag(data.buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);

//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)rasspdokuw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rasspdokuw_r_v_knopka(GtkWidget *widget,class rasspdokuw_r_data *data)
{
//printf("rasspdokuw_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rasspdokuw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasspdokuw_r_data *data)
{
// printf("rasspdokuw_r_key_press\n");
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

gint rasspdokuw_r1(class rasspdokuw_r_data *data)
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

int tipzz=data->rk->metka_pr;

int vt=0;
memset(strsql,'\0',sizeof(strsql));
iceb_poldan("Код группы возвратная тара",strsql ,"matnast.alx",data->window);
vt=atoi(strsql);

sprintf(strsql,"%s %d.%d.%d %s %d.%d.%d\n",gettext("Период с"),dn,mn,gn,gettext("по"),
dk,mk,gk);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

int kolstr=0;
/*Готовим файл для сортировки*/
sprintf(strsql,"select * from Usldokum where datd >= '%04d-%02d-%02d' \
and datd <= '%04d-%02d-%02d'",gn,mn,dn,gk,mk,dk);
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
SQL_str row,rowtmp;
SQL_str row1;
SQLCURSOR cur1,curtmp;

class iceb_tmptab tabtmp;
const char *imatmptab={"rassdokuw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
podr int not null,\
datd DATE NOT NULL,\
ndpp int not null,\
nomd char(24) not null,\
kontr char(24) not null,\
kodop char(24) not null,\
nomnn char(24) not null,\
nds smallint not null,\
datop char(16) NOT NULL,\
sumkor double(16,6) not null,\
pn char(24) not null,\
nomdokp char(32) not null,\
tipz smallint not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  

char		otsr[64];
float kolstr1=0.;
unsigned int    ndpp;
short d,m,g;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
/*
  printw("%d.%d.%d %d %s %s\n",mk1.d,mk1.m,mk1.g,mk1.tp,mk1.kop,mk1.kor);
*/

  if(tipzz != 0)
   if(tipzz != atoi(row[0]))
       continue;

  if(data->rk->metka_dok == 0)
   if(atoi(row[12]) != 1 )
       continue;

  if(data->rk->metka_dok == 1)
   if(atoi(row[12]) != 0 )
       continue;
  if(iceb_u_proverka(data->rk->kontr.ravno(),row[3],0,0) != 0)
     continue;
  if(iceb_u_proverka(data->rk->kodop.ravno(),row[6],0,0) != 0)
     continue;
     
  if(iceb_u_proverka(data->rk->podr.ravno(),row[2],0,0) != 0)
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

  ndpp=(unsigned int)atoi(strsql);
  memset(otsr,'\0',sizeof(otsr));
  if(iceb_u_rsdat(&d,&m,&g,row[9],2) == 0)
    sprintf(otsr,"%02d.%02d.%04d",d,m,g);
  
  iceb_u_rsdat(&d,&m,&g,row[1],2);

  sprintf(strsql,"insert into %s values (%s,'%04d-%02d-%02d',%d,'%s','%s','%s','%s',%s,'%s',%s,%s,'%s',%s)",
  imatmptab,
  row[2],g,m,d,ndpp,row[4],row[3],row[6],row[5],row[7],otsr,row[11],row[26],row[16],row[0]);

  iceb_sql_zapis(strsql,1,0,data->window);    

 }




if(data->rk->metka_sort == 0)
  sprintf(strsql,"select * from %s order by datd,ndpp,nomd,kontr,kodop asc",imatmptab);
if(data->rk->metka_sort == 0)
  sprintf(strsql,"select * from %s order by podr,ndpp,kontr,kodop asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s\n",gettext("Печатаем"));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");


char imaf[64];
sprintf(imaf,"sp%d.lst",getpid());
data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Распечатка списка документов"));

FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf2[64];
sprintf(imaf2,"spn%d.lst",getpid());
data->rk->imaf.plus(imaf2);
data->rk->naimf.plus(gettext("Распечатка содержимого документов"));

FILE *ff2;
if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_u_zagolov(gettext("Распечатка документов"),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ff);

iceb_u_zagolov(gettext("Распечатка документов"),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ff2);

if(tipzz == 1)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО ПРИХОДЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО ПРИХОДЫ"));
 }
if(tipzz == 2)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО РАСХОДЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО РАСХОДЫ"));
 }
if(data->rk->metka_dok == 0)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
 }
if(data->rk->metka_dok == 1)
 {
  fprintf(ff,"%s\n",gettext("ТОЛЬКО НЕ ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
  fprintf(ff2,"%s\n",gettext("ТОЛЬКО НЕ ПОДТВЕРЖДЕННЫЕ ДОКУМЕНТЫ"));
 }
if(data->rk->metka_dok == 3)
 {
  fprintf(ff,"%s\n",gettext("ВСЕ ДОКУМЕНТЫ"));
  fprintf(ff2,"%s\n",gettext("ВСЕ ДОКУМЕНТЫ"));
 }

if(data->rk->kodop.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
 }
if(data->rk->kontr.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
 }

if(data->rk->podr.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код подразделения"),data->rk->kontr.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код подразделения"),data->rk->kontr.ravno());
 }

if(data->rk->grupk.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код группы контрагента"),data->rk->grupk.ravno());
  fprintf(ff2,"%s:%s\n",gettext("Код группы контрагента"),data->rk->grupk.ravno());
 }

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("  Дата    |Дата опла.|Ном.док. |Нал. накл.|  Сумма  |Сум.кop.|  НДС   |Сумма с НДС|Контр.|  Наименование контрагента    |Код оп.|   Телефон   |\n"));

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
double itog=0.,itogsk=0.,itognds=0.;
double sp=0.,spbn=0.;
kolstr1=0.;
SQLCURSOR curr;
double nds=0.;
double itogo_nds=0.;
float pnds=0.;
double bb=0.,bb1=0.;
int skl=0;
class iceb_u_str kop(""); /*код операции*/
class iceb_u_str kor(""); /*код контрагента*/
class iceb_u_str nn(""),nnn("");  /*номер накладной*/
class iceb_u_str telefon("");
class iceb_u_str naior("");
int		lnds;
double		sumkor;
double suma=0.;
double sumabn=0.,sumav=0.;
int tipz=0;
class iceb_u_str nomdokp("");
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  iceb_pbar(data->bar,kolstrtmp,++kolstr1);    

  skl=atoi(rowtmp[0]);
  iceb_u_rsdat(&d,&m,&g,rowtmp[1],2);
  nn.new_plus(rowtmp[3]);
  kor.new_plus(rowtmp[4]);
  kop.new_plus(rowtmp[5]);
  nnn.new_plus(rowtmp[6]);
  lnds=atoi(rowtmp[7]);
  strncpy(otsr,rowtmp[8],sizeof(otsr)-1);
  sumkor=atof(rowtmp[9]);
  pnds=atof(rowtmp[10]);
  nomdokp.new_plus(rowtmp[11]);
  tipz=atoi(rowtmp[12]);
              

  if(kor.ravno()[0] == '0' && kor.ravno()[1] == '0' && iceb_u_pole(kor.ravno(),strsql,1,'-') == 0)
   kor.new_plus(strsql);
      
  sprintf(strsql,"select naikon,telef from Kontragent where kodkon='%s'",kor.ravno());
  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    fprintf(ff,"%s %s !\n",gettext("Не нашли код контрагента"),kor.ravno());

    sprintf(strsql,"%s %s !",gettext("Не нашли код контрагента"),kor.ravno());
    iceb_menu_soob(strsql,data->window);
    telefon.new_plus("");   
    naior.new_plus("");
   }
  else
   {
    naior.new_plus(row1[0]);
    telefon.new_plus(row1[1]);
   }



  /*Узнаем сумму записей по накладной*/
  
  fprintf(ff2,"\n%d.%d.%d %s (%s) %s %s\n",d,m,g,nn.ravno(),nomdokp.ravno(),kor.ravno(),naior.ravno());

  rasnaku(skl,d,m,g,nn.ravno(),&suma,&sumabn,&sumav,&sp,&spbn,vt,sumkor,lnds,ff2,data->window);

  itogsk+=sumkor;
  bb=suma+sumkor+sumabn+(suma+sumkor)*pnds/100.;
  bb=iceb_u_okrug(bb,okrg1);

  bb1=suma+sumabn;
  itog+=bb1;
  itognds+=bb;
  nds=bb-sumkor-bb1;
  itogo_nds+=nds;

  sprintf(strsql,"%02d.%02d.%d %-*s %-*.*s %-*s %8.2f %8.8g %10.2f\n",
  d,m,g,
  iceb_u_kolbait(5,kor.ravno()),kor.ravno(),
  iceb_u_kolbait(20,naior.ravno()),iceb_u_kolbait(20,naior.ravno()),naior.ravno(),
  iceb_u_kolbait(9,nn.ravno()),nn.ravno(),
  bb1,sumkor,bb);

  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
  
  fprintf(ff,"%02d.%02d.%d %10.10s %-*s %-*s %9.2f %8.8g %8.2f %11.2f %-*s %-*.*s %-*s %s\n",
  d,m,g,otsr,
  iceb_u_kolbait(9,nn.ravno()),nn.ravno(),
  iceb_u_kolbait(10,nnn.ravno()),nnn.ravno(),
  bb1,
  sumkor,
  nds,
  bb,
  iceb_u_kolbait(6,kor.ravno()),kor.ravno(),
  iceb_u_kolbait(30,naior.ravno()),iceb_u_kolbait(30,naior.ravno()),naior.ravno(),
  iceb_u_kolbait(7,kop.ravno()),kop.ravno(),
  telefon.ravno());

  if(tipz == 1)  
   {
    sprintf(strsql,"select vido from Usloper1 where kod='%s'",kop.ravno());
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     if(atoi(row1[0]) == 0)    
      if(nomdokp.getdlinna() > 1)
        fprintf(ff,"%10s %10s %-*s\n","","",iceb_u_kolbait(9,nomdokp.ravno()),nomdokp.ravno());
   }

 }

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n\
%*s: %10.2f %8.8g %8.2f %11.2f\n",iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),itog,itogsk,itogo_nds,itognds);

sprintf(strsql,"%*s: %8.2f %8.8g %10.2f\n",iceb_u_kolbait(45,gettext("Итого")),gettext("Итого"),itog,itogsk,itognds);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");


fprintf(ff2,"%s:\n\
-----------------------------------------------------------------------------------------------\n",
gettext("Общий итог"));

fprintf(ff2,"%*s: %10.2f %10s %10.2f\n",
iceb_u_kolbait(60,gettext("Итого")),gettext("Итого"),itog," ",sp+spbn);

bb=sp+spbn+sp*pnds/100.;
bb=iceb_u_okrug(bb,okrg1);
fprintf(ff2,"%*s: %10.2f %10s %10.2f\n",
iceb_u_kolbait(60,gettext("Итого с НДС")),gettext("Итого с НДС"),itognds," ",bb);

iceb_podpis(ff,data->window);
iceb_podpis(ff2,data->window);


fclose(ff);
fclose(ff2);


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

void rasnaku(int skl,short d,short m,short g,const char *nn,
double *suma,double *sumabn,double *sumav,double *sp,double *spbn,
short vt,double sumkor,int lnds,FILE *ff,GtkWidget *wpredok)
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
double		kolih,cena;
int		kodzap,tipz;
short		metka;


float pnds=iceb_pnds(wpredok);

/*Узнаем сумму всех записей*/
*suma=*sumabn=*sumav=0.;
sumap=sumapbn=0.;

sprintf(strsql,"select tp,metka,kodzap,kolih,cena,ei,shetu,nz from Usldokum1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' order by kodzap asc",
g,m,d,skl,nn);

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
  return;

fprintf(ff,"-----------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext(" N |       Наименование           |Ед. |Количество|  Ц е н а | С у м м а|Количество|Сумма под-|\n\
   |         услуг                |изм.|          |          |          |подтвержд.|тверждена |\n"));

fprintf(ff,"-----------------------------------------------------------------------------------------------\n");
SQLCURSOR curr;
ks=0;
itog[0]=itog[1]=0.;
while(cur.read_cursor(&row) != 0)
 {
  tipz=atoi(row[0]);
  metka=atoi(row[1]);  
  kodzap=atoi(row[2]);
  kolih=atof(row[3]);
  cena=atof(row[4]);
  cena=iceb_u_okrug(cena,okrcn);
  ei.new_plus(row[5]);
  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);

  if(metka == 0)
   {
    /*Узнаем наименование материала*/
    sprintf(strsql,"select naimat from Material where kodm=%s",
    row[2]);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {

      fprintf(ff,"%s %s !\n",gettext("Не найден код материала"),row[2]);
      sprintf(strsql,"%s %s !",gettext("Не найден код материала"),row[2]);
      iceb_menu_soob(strsql,wpredok);
      continue;
     }
   }

  if(metka == 1)
   {
    /*Читаем наименование услуги*/
    sprintf(strsql,"select naius from Uslugi where kodus=%s",
    row[2]);
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      fprintf(ff,"%s %s !\n",gettext("Не найден код услуги"),row[2]);
      sprintf(strsql,"%s %s !\n",gettext("Не найден код услуги"),row[2]);
      iceb_menu_soob(strsql,wpredok);
      continue;
     }
   }
  naim.new_plus(row1[0]);
  
  if(lnds > 0)
    *sumabn=*sumabn+bb;
  else
    *suma=*suma+bb;
  kol=readkoluw(d,m,g,skl,nn,tipz,metka,kodzap,atoi(row[7]),wpredok);
    
  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);

  bb1=cena*kol;
  bb1=iceb_u_okrug(bb1,okrg1);

  ks++;
  fprintf(ff,"%3d %-*.*s %-*.*s %10.10g %10.10g %10.2f %10.10g %10.2f\n",
  ks,
  iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(4,ei.ravno()),iceb_u_kolbait(4,ei.ravno()),ei.ravno(),
  kolih,cena,bb,kol,bb1);

  if(lnds > 0)
    sumapbn+=bb1;
  else
    sumap+=bb1;

 }


itog[0]=*suma+*sumabn;
itog[1]=sumap+sumapbn;
fprintf(ff,"%*s: %10.2f %10s %10.2f\n",
iceb_u_kolbait(60,gettext("Итого")),gettext("Итого"),itog[0]," ",itog[1]);

if(sumkor != 0)
 {
  bb=sumkor*100./itog[0];
  bb=iceb_u_okrug(bb,0.01);

  if(sumkor < 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Скидка"),bb*(-1));
  if(sumkor > 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Надбавка"),bb);

  fprintf(ff,"%*s %10.2f            %10.2f\n",iceb_u_kolbait(61,strsql),strsql,sumkor,sumkor);

  if(sumkor < 0)
      sprintf(strsql,gettext("Итого со скидкой :"));
  if(sumkor > 0)
      sprintf(strsql,gettext("Итого с надбавкой :"));

   fprintf(ff,"%*s %10.2f            %10.2f\n",iceb_u_kolbait(61,strsql),strsql,sumkor+itog[0],sumkor+itog[1]);

 }

bb=*suma+sumkor+*sumabn+( *suma + sumkor)*pnds/100.;
bb=iceb_u_okrug(bb,okrg1);
bb1=sumap+sumkor+sumapbn+(sumap+sumkor)*pnds/100.;
bb1=iceb_u_okrug(bb1,okrg1);

fprintf(ff,"%*s: %10.2f %10s %10.2f\n",
iceb_u_kolbait(60,gettext("НДС")),gettext("НДС"),bb-sumkor-itog[0]," ",bb1-sumkor-itog[1]);

fprintf(ff,"%*s: %10.2f %10s %10.2f\n",
iceb_u_kolbait(60,gettext("Итого с НДС")),gettext("Итого с НДС"),bb," ",bb1);
*sp=*sp+sumap;
*spbn=*spbn+sumapbn;
}
