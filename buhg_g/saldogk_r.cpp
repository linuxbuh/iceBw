/*$Id: saldogk_r.c,v 1.19 2013/09/26 09:46:55 sasa Exp $*/
/*23.05.2016	01.10.2006	Белых А.И.	saldogk_r.c
Расчёт отчёта 
*/
#include <math.h>
#include "buhg_g.h"
#include "glktmpw.h"
class saldogk_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  short godk;  
  short godn;  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  saldogk_r_data()
   {
    voz=1;
    kon_ras=1;
    godn=godk=0;
   }
 };
gboolean   saldogk_r_key_press(GtkWidget *widget,GdkEventKey *event,class saldogk_r_data *data);
gint saldogk_r1(class saldogk_r_data *data);
void  saldogk_r_v_knopka(GtkWidget *widget,class saldogk_r_data *data);
void saldonbsw(short dn,short mn,short gn,short dk,short mk,short gk,GtkTextBuffer *buffer,GtkWidget *view,GtkWidget *bar,GtkWidget *wpredok);


int glkni_zvt(int per,const char *shet,const char *kod_kontr,double deb,double kre,const char *imatabl,GtkWidget *wpredok);

extern SQL_baza bd;
extern double   okrg1;
extern double	okrcn;
extern short startgodb; /*объявлен в файле matnastw */

int saldogk_r(short godn,short godk,GtkWidget *wpredok)
{
char strsql[1024];
int gor=0;
int ver=0;
class iceb_u_str repl;
class saldogk_r_data data;
data.godk=godk;
data.godn=godn;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Перенос сальдо для подсистемы \"Главная книга\""));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(saldogk_r_key_press),&data);

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

repl.plus(gettext("Перенос сальдо для подсистемы \"Главная книга\""));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(saldogk_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)saldogk_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  saldogk_r_v_knopka(GtkWidget *widget,class saldogk_r_data *data)
{
//printf("saldogk_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   saldogk_r_key_press(GtkWidget *widget,GdkEventKey *event,class saldogk_r_data *data)
{
//printf("saldogk_r_key_press\n");
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
/*Распечатка обычных счетов*/
/***************************/

void ras1(short i,
const char *ns, //Счет
const char *nash,  //Наименование
double *dpp,
double *kpp,
double *dns,
double *kns,
double *di,
double *ki,
class GLKTMP *PROM,
GtkWidget *wpredok)
{
short           i1;
short		mos; /*Метка оборотов по счетам*/
double		deb,kre;
char		strsql[1024];
time_t		vrem;

deb=kre=0.;
mos=0;
double sd=0.,sk=0.;
double dpe=0.,kpe=0.;
int ks=PROM->masf.kolih();

for(i1=0;i1<ks;i1++)
 {
  sd+=PROM->sl.ravno(i*ks+i1);
  sk+=PROM->sl.ravno(i+ks*i1);

  dpe+=PROM->pe.ravno(i*ks+i1);
  kpe+=PROM->pe.ravno(i+ks*i1);
  if(fabs(PROM->pe.ravno(i*ks+i1)) > 0.009 || fabs(PROM->pe.ravno(i+ks*i1)) > 0.009)
    mos=1;
 }

if(mos == 0)
 {
  if(fabs(sd) < 0.01 && fabs(sk) < 0.01 && fabs(dpe) < 0.01 && fabs(kpe) < 0.01
  && fabs(PROM->ssd.ravno(i)) < 0.01 && fabs(PROM->ssk.ravno(i)) < 0.01)
    return;

  if(fabs(sd+PROM->ssd.ravno(i)-sk-PROM->ssk.ravno(i)) < 0.01 && fabs(dpe) < 0.01 && fabs(kpe) < 0.01)
   return;
 }

*dpp+=dpe;
*kpp+=kpe;
if(PROM->ssd.ravno(i)+sd + dpe >= PROM->ssk.ravno(i)+sk+kpe)
 {
  if(PROM->ssd.ravno(i)+sd >= PROM->ssk.ravno(i)+sk)
   {
    *dns+=(PROM->ssd.ravno(i)+sd)-(PROM->ssk.ravno(i)+sk);
   }
  else
   {
    *kns+=(PROM->ssk.ravno(i)+sk)-(PROM->ssd.ravno(i)+sd);
   }
  *di=*di+PROM->ssd.ravno(i)+sd+dpe - (PROM->ssk.ravno(i)+sk+kpe);
  deb=PROM->ssd.ravno(i)+sd+dpe - (PROM->ssk.ravno(i)+sk+kpe);
 }
else
 {
  if(PROM->ssd.ravno(i)+sd >= PROM->ssk.ravno(i)+sk)
   {
    *dns+=(PROM->ssd.ravno(i)+sd)-(PROM->ssk.ravno(i)+sk);
   }
  else
   {
    *kns+=(PROM->ssk.ravno(i)+sk)-(PROM->ssd.ravno(i)+sd);
   }
  *ki=*ki+PROM->ssk.ravno(i)+sk+kpe-(PROM->ssd.ravno(i)+sd+dpe);
  kre=PROM->ssk.ravno(i)+sk+kpe-(PROM->ssd.ravno(i)+sd+dpe);
 }

if(fabs(deb) > 0.009 || fabs(kre) > 0.009) 
 {

  time(&vrem);

  sprintf(strsql,"insert into Saldo \
  values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
  "0",0,ns,"",deb,kre,iceb_getuid(wpredok),vrem);   

  iceb_sql_zapis(strsql,0,0,wpredok);
 }
}


/******************************************/
/*Работаем со счетами с развернутым сальдо*/
/******************************************/
void ras2n(const char *sh,
double *dns,
double *kns,
double *dpp,
double *kpp,
double *di,
double *ki,
const char *imatabl,
GtkWidget *wpredok)
{
double          d1=0.,k1=0.;
char		strsql[1024];
double		debs=0.,kres=0.;
double		debp=0.,krep=0.;
double          sd=0.,sk=0.,dpe=0.,kpe=0.;
SQL_str row;
class SQLCURSOR cur;
double deb=0.,kre=0.;
/*******************
for(int nomer_kontr=0; nomer_kontr < ns_kontr->kolih(); nomer_kontr++)
 {
  debs=ns_kontr_debs->ravno(nomer_kontr);
  kres=ns_kontr_kres->ravno(nomer_kontr);
  debp=ns_kontr_debp->ravno(nomer_kontr);
  krep=ns_kontr_krep->ravno(nomer_kontr);

  if(debs > kres)
   sd+=debs-kres;
  else
   sk+=kres-debs;
  deb=kre=0.;
  if(debs+debp > kres+krep)
   {
    d1+=debs+debp-kres-krep;  
    deb=debs+debp-kres-krep;  
   }
  else
   {
    k1+=kres+krep-debs-debp;
    kre=kres+krep-debs-debp;
   }

  if(fabs(deb) > 0.009 || fabs(kre) > 0.009) 
    {
     sprintf(strsql,"insert into Saldo \
values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
     "1",0,sh,ns_kontr->ravno(nomer_kontr),deb,kre,iceb_getuid(wpredok),time(NULL));   

     iceb_sql_zapis(strsql,0,0,wpredok);
    }

  dpe+=debp;
  kpe+=krep;
 }
*********************/
sprintf(strsql,"select debs,kres,debp,krep,kod_kontr from %s where shet='%s'",imatabl,sh);

if(cur.make_cursor(&bd,strsql) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {

  debs=atof(row[0]);
  kres=atof(row[1]);
  debp=atof(row[2]);
  krep=atof(row[3]);

  if(debs > kres)
   sd+=debs-kres;
  else
   sk+=kres-debs;

  deb=kre=0.;
  if(debs+debp > kres+krep)
   {
    d1+=debs+debp-kres-krep;  
    deb=debs+debp-kres-krep;  
   }
  else
   {
    k1+=kres+krep-debs-debp;
    kre=kres+krep-debs-debp;
   }

  if(fabs(deb) > 0.009 || fabs(kre) > 0.009) 
    {
     sprintf(strsql,"insert into Saldo \
values ('%s',%d,'%s','%s',%.2f,%.2f,%d,%ld)",
     "1",0,sh,row[4],deb,kre,iceb_getuid(wpredok),time(NULL));   

     iceb_sql_zapis(strsql,0,0,wpredok);
    }

  dpe+=debp;
  kpe+=krep;
 }

*dns+=sd;
*kns+=sk;

*dpp+=dpe;
*kpp+=kpe;
/*printw("dpp=%f kpp=%d\n",dpp,kpp);*/

*di+=d1; 
*ki+=k1;

}

/******************************************/
/******************************************/

gint saldogk_r1(class saldogk_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;


short gn =data->godn;
short dn=1;
short mn=1;
short dk=31;
short mk=12;
short gk=data->godk-1;

if(iceb_pblok(1,data->godk,ICEB_PS_GK,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
sprintf(strsql,"Перенос сальдо по счетам в \"Главной книге\"\nПеренести сальдо с %dг. на %dг. ? Вы уверены ?",gn,data->godk);
if(iceb_menu_danet(strsql,2,data->window) == 2)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


int metka_r=1; /*Расчитать по субсчетам*/


sprintf(strsql,"Перенос остатков с %d на %d\n",gn,data->godk);
iceb_printw(strsql,data->buffer,data->view);


class GLKTMP PROM;

//int mp=rozraxw(dn,mn,gn,dk,mk,gk,sb,&PROM,data->buffer,data->view,data->bar,data->window);
/************************************************************/
sprintf(strsql,"%s\n",gettext("Формируем массив счетов"));
iceb_printw(strsql,data->buffer,data->view);

int msrv=0;
if(metka_r == 0)
 msrv=1;
/*************************/
/*Формируем массив счетов*/
/*************************/
int kolstr=0;
sprintf(strsql,"select ns,vids,saldo from Plansh where stat=0 order by ns asc");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не введён план счетов !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int pozz=0;
class iceb_u_str sh("");
while(cur.read_cursor(&row) != 0)
 {
  pozz++;

  if(atoi(row[1]) == 1 && metka_r == 0)
    continue;

  sh.new_plus(row[0]);
  if(metka_r == 1 && atoi(row[1]) == 0 && pozz < kolstr-1)
   {
    cur.read_cursor(&row);
    /*Если нет субсчетов запоминаем счет*/
    if(iceb_u_SRAV(sh.ravno(),row[0],1) != 0)
     {
      cur.poz_cursor(pozz-1);
      cur.read_cursor(&row);
     }
    else
     pozz++;
   }
  PROM.masf.plus(row[0]);

  if(atoi(row[2]) == 3)
     PROM.msr.plus(row[0]);
 }
int ks=PROM.masf.kolih();

sprintf(strsql,"%s: %d\n",gettext("Количество счетов"),ks);
iceb_printw(strsql,data->buffer,data->view);
/************
sprintf(strsql,"%s:",gettext("Счета"));
iceb_printw(strsql,data->buffer,data->view);

for(int ish=0; ish < PROM.masf.kolih() ; ish++)
 {
  sprintf(strsql,"%s\n",PROM.masf.ravno(ish));
  iceb_printw(strsql,data->buffer,data->view);
 }
*****************/
class iceb_tmptab tabtmp;
const char *imatmptab={"glkni"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shet char(32) not null DEFAULT '',\
kod_kontr char(32) not null DEFAULT '',\
debs double(16,2) not null DEFAULT 0.,\
kres double(16,2) not null DEFAULT 0.,\
debp double(16,2) not null DEFAULT 0.,\
krep double(16,2) not null DEFAULT 0.,\
unique(shet,kod_kontr)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  


int ll1=ks*ks; /*Шахматка для заполнения*/
PROM.ssd.make_class(ks);
PROM.ssk.make_class(ks);
PROM.deb_end.make_class(ks);
PROM.kre_end.make_class(ks);
PROM.sl.make_class(ll1);
PROM.pe.make_class(ll1);

int ksrs=PROM.msr.kolih();
sprintf(strsql,"%s:%d\n",gettext("Количество счетов с развернутым сальдо"),ksrs);
iceb_printw(strsql,data->buffer,data->view);
/***********************************/
/*Узнаем стартовые сальдо по счетам*/
/***********************************/
sprintf(strsql,"%s\n",gettext("Вычисляем стартовые сальдо по счетам"));
iceb_printw(strsql,data->buffer,data->view);
int ts=0;
int vids=0;
int i=0;
double deb=0.,kre=0.;
for(i=0; i < ks ; i++)
 {
  sh.new_plus(PROM.masf.ravno(i));
  sprintf(strsql,"select saldo,vids from Plansh where ns='%s'",sh.ravno());
  
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,"%s %s",gettext("Не найден счёт %s в плане счетов !"),sh.ravno());
    iceb_menu_soob(strsql,data->window);
    continue;
   }

  ts=0;
  if(atoi(row[0]) == 3)
    ts=1;
  vids=atoi(row[1]);
  
  if(vids == 0) /*Счет*/
   sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='%d' and gs=%d \
and ns like '%s%%'",ts,gn,sh.ravno());
  if(vids == 1) /*Субсчёт*/
   sprintf(strsql,"select kodkon,deb,kre from Saldo where kkk='%d' and gs=%d \
and ns='%s'",ts,gn,sh.ravno());

   if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
     continue;
    }
   if(kolstr == 0)
     continue;

  while(cur.read_cursor(&row) != 0)
   {
    deb=atof(row[1]);
    kre=atof(row[2]);
    if(row[0][0] != '\0')
     {
      glkni_zvt(0,sh.ravno(),row[0],deb,kre,imatmptab,data->window);
     }
    PROM.ssd.plus(deb,i);
    PROM.ssk.plus(kre,i);
   }
/*  
  printw("%s %.2f %.2f\n",sh,ssd[i],ssk[i]);
  getch();
*/
  if(ts == 0)  /*Счета с не развернутым сальдо*/
   {
    if(PROM.ssd.ravno(i) >= PROM.ssk.ravno(i))
     {
      PROM.ssd.plus(PROM.ssk.ravno(i)*-1,i);
      PROM.ssk.new_plus(0.,i);
     }
    else
     {
      PROM.ssk.plus(PROM.ssd.ravno(i)*-1,i);
      PROM.ssd.new_plus(0.,i);
     }
   }
 }

/******************************************/
/*Просматриваем записи и заполняем массивы*/
/******************************************/

sprintf(strsql,"%s\n",gettext("Просматриваем проводки, заполняем массивы"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov \
where val=0 and datp >= '%d-%02d-%02d' and \
datp <= '%d-%02d-%02d'",gn,1,1,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено проводок !"),data->window);
 }
else
 {
  double cel=kolstr;
  double drob=0.;

  drob=modf(cel,&drob);
  if(drob > 0.000001)
   {
    class iceb_u_str repl;
    
    sprintf(strsql,"%s !!!",gettext("Внимание"));
    repl.plus(strsql);
    
    repl.ps_plus(gettext("Не парное количество проводок"));
    repl.ps_plus(gettext("Проверте базу данных на логическую целосность"));
    iceb_menu_soob(&repl,data->window);
   }
 }
sprintf(strsql,"%s:%d\n",gettext("Количество проводок"),kolstr);
iceb_printw(strsql,data->buffer,data->view);

int mpe=0;
float kolstr1=0.;
short den,mes,god;
class iceb_u_str kor("");
class iceb_u_str shk("");
double sm=0.;
int i1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  iceb_u_rsdat(&den,&mes,&god,row[0],2);
  kor.new_plus(row[3]);
  deb=atof(row[4]);   
  kre=atof(row[5]);
  

  if(iceb_u_sravmydat(dn,mn,gn,den,mes,god) > 0) /*До периода*/
   {
     mpe=0;
   }

  if(iceb_u_sravmydat(dn,mn,gn,den,mes,god) <= 0) /*период*/
  if(iceb_u_sravmydat(dk,mk,gk,den,mes,god) >= 0) /*период*/
   {
    mpe=1;
   }
  if(row[3][0] != '\0')
    glkni_zvt(mpe,row[1],row[3],deb,kre,imatmptab,data->window);

  if(fabs(deb) > 0.009)
   {
    sh.new_plus(row[1]);
    shk.new_plus(row[2]);
    sm=deb;
   }
  else
   {
    sh.new_plus(row[2]);
    shk.new_plus(row[1]);
    sm=kre;
   }
  if(sh.ravno()[0] == '\0')
   {
    class iceb_u_str repl;
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    repl.plus(strsql);
    
    repl.ps_plus(gettext("Не введён счёт !"));
    iceb_menu_soob(&repl,data->window);
    continue;
   }

  if(shk.ravno()[0] == '\0')
   {
    class iceb_u_str repl;
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    repl.plus(strsql);
    repl.ps_plus(gettext("Не введён коресподирующий счёт !"));
    iceb_menu_soob(&repl,data->window);
    continue;
   }

  if((i=PROM.masf.find(sh.ravno(),msrv)) == -1)
   {
    class iceb_u_str repl;
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    repl.plus(strsql);
    
    sprintf(strsql,"%s %s",gettext("Не найден счёт в массиве счетов !"),sh.ravno());
    repl.ps_plus(strsql);

    iceb_menu_soob(&repl,data->window);


    continue;
   }

  if((i1=PROM.masf.find(shk.ravno(),msrv)) == -1)
   {
    class iceb_u_str repl;
    sprintf(strsql,"%s %s %s %s %s %s",
    row[0],row[1],row[2],row[3],row[4],row[5]);
    repl.plus(strsql);

    sprintf(strsql,gettext("Не найден счёт корреспондент %s в масиве счетов !"),shk.ravno());
    repl.ps_plus(strsql);
    repl.ps_plus("rozraxw");
    iceb_menu_soob(&repl,data->window);

    continue;
   }

  if(mpe == 0)
    PROM.sl.plus(sm,i*ks+i1);
  else
    PROM.pe.plus(sm,i*ks+i1);
  
/*Проверяем есть ли неправильные проводки*/
  if(ksrs != 0)
   if(PROM.msr.find(row[2],msrv) >= 0)
   {

    if(kor.ravno()[0] == '\0')
     {

      class iceb_u_str repl;
      sprintf(strsql,"%d.%d.%d %-5s %-5s %-5s %10s %10s",
      den,mes,god,row[1],row[2],row[3],row[4],row[5]);
      repl.plus(strsql);

      repl.ps_plus(gettext("В проводке нет кода контрагента !"));
      repl.ps_plus(strsql);

      iceb_menu_soob(&repl,data->window);

     } 
   }

  /*Проверяем есть ли счёт в списке счетов с развернутым сальдо*/
  if(ksrs != 0)
   if(PROM.msr.find(row[1],msrv) >= 0)
   {
   
    if(kor.ravno()[0] == '\0')
     {

      class iceb_u_str repl;
      sprintf(strsql,"%d.%d.%d %-5s %-5s %-5s %10s %10s",
      den,mes,god,row[1],row[2],row[3],row[4],row[5]);
      repl.plus(strsql);

      repl.ps_plus(gettext("В проводке нет кода контрагента !"));
      repl.ps_plus(strsql);

      iceb_menu_soob(&repl,data->window);

     } 

   }



 }





/****************************************************************/
sprintf(strsql,"%s\n",gettext("Расчёт закончен"));
iceb_printw(strsql,data->buffer,data->view);

/*Чистим нулевой год куда будут записаны сальдо*/
sprintf(strsql,"delete from Saldo where gs=0");

iceb_sql_zapis(strsql,0,0,data->window);



/************************/
/*Распечатка результатов*/
/************************/
/*Так как все проводки встречаются дважды то все делим на 2*/
for(int i=0;i<ks*ks;i++)
  {
   if(PROM.sl.ravno(i) != 0.)
    PROM.sl.new_plus(PROM.sl.ravno(i)/2.,i);
   if(PROM.pe.ravno(i) != 0.)
    PROM.pe.new_plus(PROM.pe.ravno(i)/2.,i);
   /*printw("sl[%d]=%.2f ",i,sl[i]);*/
  }


double di=0,ki=0;
double dns=0,kns=0,dpp=0,kpp=0.;
//getyx(stdscr,Y,X);
short metkasaldo=0;
for(int i=0; i <ks ; i++)
 {
//  strzag(LINES-1,0,ks,i);
  iceb_pbar(data->bar,ks,i);    
  
  sprintf(strsql,"select saldo,nais,vids from Plansh where ns='%s'",PROM.masf.ravno(i));
  
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,gettext("Нет счета %s в плане счетов !"),PROM.masf.ravno(i));
    iceb_menu_soob(strsql,data->window);
    continue;
   }
  metkasaldo=atoi(row[0]);

  sprintf(strsql,"%-5s %s\n",PROM.masf.ravno(i),row[1]);
  iceb_printw(strsql,data->buffer,data->view);
  
  if(metkasaldo == 3)
   {
    ras2n(PROM.masf.ravno(i),&dns,&kns,&dpp,&kpp,&di,&ki,imatmptab,data->window);
/*********************
    nomer_sheta_srs=PROM.msr.find(PROM.masf.ravno(i),msrv);

    ras2(i,PROM.masf.ravno(i),row[1],&dns,&kns,&dpp,&kpp,&di,&ki,
    &ns_kontr[nomer_sheta_srs],
    &ns_kontr_debs[nomer_sheta_srs],
    &ns_kontr_kres[nomer_sheta_srs],
    &ns_kontr_debp[nomer_sheta_srs],
    &ns_kontr_krep[nomer_sheta_srs],
    data->window);
*****************/
   }
  else
   {
    ras1(i,PROM.masf.ravno(i),row[1],&dpp,&kpp,&dns,&kns,&di,&ki,&PROM,data->window);
   }
 }

sprintf(strsql,"\n\
%*s:%15s",iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),iceb_u_prnbr(dns));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql," %15s\n",iceb_u_prnbr(kns));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%*s:%15s",iceb_u_kolbait(20,gettext("Оборот за период")),gettext("Оборот за период"),iceb_u_prnbr(dpp));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql," %15s\n",iceb_u_prnbr(kpp));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%*s:%15s",iceb_u_kolbait(20,gettext("Сальдо конечное ")),gettext("Сальдо конечное "),iceb_u_prnbr(di));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql," %15s\n",iceb_u_prnbr(ki));
iceb_printw(strsql,data->buffer,data->view);



sprintf(strsql,"select ns from Plansh where stat=1");
if(iceb_sql_readkey(strsql,&row,&cur,data->window) > 0)
 {


  if(iceb_menu_danet("Перенести сальдо по небалансовым счетам ?",2,data->window) == 1)
   {
//    saldonbs(dn,mn,gn,dk,mk,gk);
    saldonbsw(dn,mn,gn,dk,mk,gk,data->buffer,data->view,data->bar,data->window);

    sprintf(strsql,"\nБалансовые счета:\n\
%*s:%15s",iceb_u_kolbait(20,gettext("Сальдо начальное")),gettext("Сальдо начальное"),iceb_u_prnbr(dns));
    iceb_printw(strsql,data->buffer,data->view);

    sprintf(strsql," %15s\n",iceb_u_prnbr(kns));
    iceb_printw(strsql,data->buffer,data->view);

    sprintf(strsql,"%*s:%15s",iceb_u_kolbait(20,gettext("Оборот за период")),gettext("Оборот за период"),iceb_u_prnbr(dpp));
    iceb_printw(strsql,data->buffer,data->view);

    sprintf(strsql," %15s\n",iceb_u_prnbr(kpp));
    iceb_printw(strsql,data->buffer,data->view);

    sprintf(strsql,"%*s:%15s",iceb_u_kolbait(20,gettext("Сальдо конечное ")),gettext("Сальдо конечное "),iceb_u_prnbr(di));
    iceb_printw(strsql,data->buffer,data->view);

    sprintf(strsql," %15s\n",iceb_u_prnbr(ki));
    iceb_printw(strsql,data->buffer,data->view);
   }
 }
sprintf(strsql,"Чистим перед записью\n");
iceb_printw(strsql,data->buffer,data->view);

/*Чистим год куда будут записаны сальдо*/
sprintf(strsql,"delete from Saldo where gs=%d",data->godk);
iceb_sql_zapis(strsql,0,0,data->window);

sprintf(strsql,"Устанавливаем год\n");
iceb_printw(strsql,data->buffer,data->view);

/*Устанавливаем год куда будут записаны сальдо*/
sprintf(strsql,"update Saldo set gs=%d where gs=0",data->godk);
iceb_sql_zapis(strsql,0,0,data->window);



short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
if(data->godk <= gt)
 {
  sprintf(strsql,"update Alx set str='Стартовый год|%d\n' where fil='nastrb.alx' and str like 'Стартовый год|%d%%'",
  data->godk,data->godn);

  if(iceb_sql_zapis(strsql,1,0,data->window) == 0)
   startgodb=data->godk;

 }





gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}

