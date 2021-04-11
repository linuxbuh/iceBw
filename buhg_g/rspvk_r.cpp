/*$Id: rspvk_r.c,v 1.31 2014/04/30 06:14:58 sasa Exp $*/
/*23.05.2016	28.03.2004	Белых А.И.	rspvk_r.c
Расчёт сальдо по контрагентам
*/
#define	   KOLST  77 /*Количество строк на листе*/
#include <errno.h>
#include <math.h>
#include "buhg_g.h"
#include "rspvk.h"

extern int kol_strok_na_liste_l_u;
class rspvk_r_data
 {
  public:
  
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
  class iceb_u_spisok imaf;
  class iceb_u_spisok naim;

  rspvk_rr *rek_r;
  short dn,mn,gn;
  short dk,mk,gk;
  short  godn;
  class spis_oth *oth;
  int voz;
  rspvk_r_data()
   {
    voz=1;
   }
 };

gboolean   rspvk_r_key_press(GtkWidget *widget,GdkEventKey *event,class rspvk_r_data *data);
gint rspvk_r1(class rspvk_r_data *data);
void  rspvk_r_v_knopka(GtkWidget *widget,class rspvk_r_data *data);
int xbudkzw(int dk,const char *kodkontr,const char *shet,short dz,short mz,short gz,double sum_z,class iceb_u_str *dataz,GtkWidget *wpredok);

extern SQL_baza bd;
extern short	startgodb; /*Стартовый год*/

int rspvk_r(class spis_oth *oth,class rspvk_rr *data_rr,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class rspvk_r_data data;

data.rek_r=data_rr;
data.name_window.plus(__FUNCTION__);
data.oth=oth;

char strsql[512];
class iceb_u_str soob;



if(iceb_rsdatp(&data.dn,&data.mn,&data.gn,data.rek_r->datan.ravno(),
&data.dk,&data.mk,&data.gk,data.rek_r->datak.ravno(),NULL) != 0)
  return(1);
  
data.godn=startgodb;
if(startgodb == 0 || startgodb > data.gn)
 data.godn=data.gn;


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rspvk_r_key_press),&data);

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

sprintf(strsql,"%s %s",gettext("Расчёт сальдо по всем контрагентам"),data.rek_r->shet.ravno());
soob.new_plus(strsql);

sprintf(strsql,"%s %d.%d.%d => %d.%d.%d",gettext("Расчёт за период"),
data.dn,data.mn,data.gn,
data.dk,data.mk,data.gk);
soob.ps_plus(strsql);

GtkWidget *label=gtk_label_new(soob.ravno());

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(rspvk_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)rspvk_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
//iceb_rabfil(&data.imaf,&data.naim,NULL);



return(data.voz);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rspvk_r_v_knopka(GtkWidget *widget,class rspvk_r_data *data)
{
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rspvk_r_key_press(GtkWidget *widget,GdkEventKey *event,class rspvk_r_data *data)
{
// printf("rspvk_r_key_press\n");

switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}










/**********/
/*Шапка 1*/
/*********/
void sal1(short kli,int metka_r,FILE *ff,FILE *ff1)
{

fprintf(ff,"%90s%s%d\n","",gettext("Лист N"),kli);
fprintf(ff1,"%50s%s%d\n","",gettext("Лист N"),kli);

if(metka_r == 1) /*с датой возникновения сальдо*/
 {
  fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff,gettext("\
 Код   | Наименование организации| Счёт |  Сальдо начальное     |  Оборот за период     |    Сальдо конечное    |   Дата   |\n\
       |                         |      |   Дебет   |  Кредит   |   Дебет   |  Кредит   |   Дебет   |  Кредит   |  сальдо  |\n"));

  fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff1,"\
----------------------------------------------------------------------------\n");

  fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |    Сальдо конечное    |   Дата   |\n\
       |                         |      |   Дебет   |  Кредит   |  сальдо  |\n"));

  fprintf(ff1,"\
----------------------------------------------------------------------------\n");
 }
else
 {
  fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff,gettext("\
 Код   | Наименование организации| Счёт |  Сальдо начальное     |  Оборот за период     |    Сальдо конечное    |\n\
       |                         |      |   Дебет   |  Кредит   |   Дебет   |  Кредит   |   Дебет   |  Кредит   |\n"));

  fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff1,"\
-----------------------------------------------------------------\n");

  fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |    Сальдо конечное    |\n\
       |                         |      |   Дебет   |  Кредит   |\n"));

  fprintf(ff1,"\
-----------------------------------------------------------------\n");
 }
}

/**********/
/*Шапка 2*/
/*********/
void sal2(short kli,int metka_dk, /*1-кредитовое сальдо 2-дебетовое*/
int metka_r,FILE *ff1)
{

fprintf(ff1,"%35s%s%d\n","",gettext("Лист N"),kli);
if(metka_r == 1)
 {
  fprintf(ff1,"\
----------------------------------------------------------------\n");

  if(metka_dk == 2)
   fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |  Сальдо   |   Дата   |\n\
       |                         |      | дебетовое |  сальдо  |\n"));
  else
   fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |  Сальдо   |   Дата   |\n\
       |                         |      | кредитовое|  сальдо  |\n"));
  
  fprintf(ff1,"\
----------------------------------------------------------------\n");
 }
else
 {
  fprintf(ff1,"\
-----------------------------------------------------\n");

  if(metka_dk == 2)
   fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |  Сальдо   |\n\
       |                         |      | дебетовое |\n"));
  else
   fprintf(ff1,gettext("\
  Код  | Наименование организации| Счёт |  Сальдо   |\n\
       |                         |      | кредитовое|\n"));

  fprintf(ff1,"\
-----------------------------------------------------\n");
 }

}




/*************************/
/*Счетчик строк и листов*/
/*************************/
void sstrsl(short *kst, //Количество строк
short *kli, //Количество листов
int metka_r, /*1-с датой 2-без*/
FILE *ff,FILE *ff1)
{

*kst+=1;
if(*kst >= KOLST)
 {
  fprintf(ff,"\f");
  fprintf(ff1,"\f");
  *kli=*kli+1;
  sal1(*kli,metka_r,ff,ff1);
  *kst=5;
 }  

}

/*************************/
/*Счетчик строк и листов*/
/*************************/
void sstrsl1(short *kst, //Количество строк
short *kli, //Количество листов
int metka_dk,
int metka_r,
FILE *ff)
{

*kst+=1;
if(*kst >= KOLST)
 {
  fprintf(ff,"\f");
  *kli+=1;
  sal2(*kli,metka_dk,metka_r,ff);
  *kst=5;
 }  

}

/*********************/
/*запись контрагента во временную таблицу*/
/*******************************************/
int salallk_zvt(const char *kod_kontr,int *nom_zap,const char *imatab,GtkWidget *wpredok)
{
char strsql[512];

sprintf(strsql,"select kod_kontr from %s where kod_kontr='%s'",imatab,kod_kontr);
if(iceb_sql_readkey(strsql,wpredok) == 1)
 return(0);

sprintf(strsql,"insert into %s values('%s',%d)",imatab,kod_kontr,*nom_zap);

if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 return(1);

*nom_zap+=1;

return(0);
}















/******************************/
/*Подпрограмма быстрого отчёта*/
/******************************/
int saloro3(const char *sheta,
short dn,short mn,
short gn,short dk,short mk,short gk,
short mks, //0-все проводки 1-без взаимно кореспондирующих
const char *grupa,
const char *kontr,
class iceb_u_spisok *sheta_srs,
int metka_r,
class spis_oth *oth,
class rspvk_r_data *data)
{
int kls=sheta_srs->kolih(); //Количество счетов с развернутым сальдо
long		kolstr,kolstr1;
SQL_str         row,row1;
char		strsql[1024];
time_t          tmm;
short		mro,mrodeb,mrokre;
char		imaf[64],imaf1[64],imafdeb[64],imafkre[64];
long		koo=0;
long		i,i1,i2;
class iceb_u_str sh("");
double		db,kr;
class iceb_u_str kor("");
long		rzm;
double		mas1[6],mas2[6];
short		kli=0,kst=0;
short		klideb,kstdeb;
short		klikre,kstkre;
short		d,m,g;
class iceb_u_str bros("");
short		godn;
double		itdeb,itkre;
SQLCURSOR cur;
SQLCURSOR cur1;

time(&tmm);

godn=startgodb;
if(startgodb == 0 || startgodb > gn)
 godn=gn;


class iceb_tmptab tabtmp;
const char *imatmptab={"saldoall"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
kod_kontr char(32) not null DEFAULT '',\
nomz INT NOT NULL DEFAULT 0,\
index(nomz),\
unique(kod_kontr)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  return(1);
 }  


//class iceb_u_spisok spkontr;

/*Смотрим по каким контрагентам введено сальдо*/
sprintf(strsql,"select kodkon from Saldo where kkk=1 and gs=%d",godn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
int nomer_zapisi=0;
while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(kontr,row[0],0,0) != 0)
    continue;
  if(grupa[0] != '\0')
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[0]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Не найден код контрагента"),row[0]);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
    if(iceb_u_proverka(grupa,row1[0],0,0) != 0)
      continue;
   }
  salallk_zvt(row[0],&nomer_zapisi,imatmptab,data->window); /*Запись в таблицу*/
 }

/*Смотрим по каким контрагентам были сделаны проводки*/
sprintf(strsql,"select distinct kodkon from Prov where datp >= '%04d-1-1' and \
datp <= '%04d-%02d-%02d'and kodkon <> ''",godn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }


while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(kontr,row[0],0,0) != 0)
    continue;
  if(grupa[0] != '\0')
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[0]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Не найден код контрагента"),row[0]);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
    if(iceb_u_proverka(grupa,row1[0],0,0) != 0)
      continue;
   }

  salallk_zvt(row[0],&nomer_zapisi,imatmptab,data->window); /*Запись в таблицу*/
 }

sprintf(strsql,"select * from %s",imatmptab);
koo=iceb_sql_readkey(strsql,data->window);


sprintf(strsql,"%s: %ld\n",gettext("Количество контрагентов"),koo);

iceb_printw(strsql,data->buffer,data->view);

if(koo == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного контрагента !"),data->window);
  return(1);
 }

rzm=koo*kls*4;
class iceb_u_double mdo;
mdo.make_class(rzm);

sprintf(strsql,"%s\n",gettext("Вычисляем стартовое сальдо по всем контрагентам"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"select ns,kodkon,deb,kre from Saldo where kkk='%d' and gs=%d",1,godn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if((i=sheta_srs->find(row[0])) < 0)
   continue;

  if(iceb_u_proverka(kontr,row[1],0,0) != 0)
    continue;

  if(grupa[0] != '\0')
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[1]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)    
     if(iceb_u_proverka(grupa,row1[0],0,0) != 0)
       continue;
   }
   
      
  sprintf(strsql,"select nomz from %s where kod_kontr='%s'",imatmptab,row[1]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    i1=atoi(row1[0]);
    rzm=(i1*kls*4)+(i*4);
    mdo.plus(atof(row[2]),rzm);
    mdo.plus(atof(row[3]),rzm+1);
   }
  else
   {

    sprintf(strsql,"%s %s %s",
    gettext("Не найден код контрагента"),
    row[1],
    gettext("в массиве кодов"));

    iceb_menu_soob(strsql,data->window);
   }
 }

sprintf(strsql,"%s\n",gettext("Просматриваем проводки"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre from Prov \
where val=0 and datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
order by datp,sh asc",godn,1,1,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  iceb_beep();
  sprintf(strsql,"%s %d %s\n",gettext("Нет проводок за"),gn,gettext("год"));
  iceb_menu_soob(strsql,data->window);
  return(1);
 } 
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if((i=sheta_srs->find(row[1])) < 0)
    continue;  

  if(iceb_u_proverka(kontr,row[3],0,0) != 0)
    continue;

  if(grupa[0] != '\0' )
   {
    sprintf(strsql,"select grup from Kontragent where kodkon='%s'",row[3]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)    
     if(iceb_u_proverka(grupa,row1[0],0,0) != 0)
       continue;
   }

  sprintf(strsql,"select nomz from %s where kod_kontr='%s'",imatmptab,row[3]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 0)
   {
//    VVOD SOOB(1);
    class iceb_u_spisok spm;
    
    sprintf(strsql,"%s %s %s",gettext("Не найден код контрагента"),row[3],gettext("в массиве кодов"));
    spm.plus(strsql);
//    SOOB.VVOD_SPISOK_add_MD(strsql);

    sprintf(strsql,"%s %s %s %s %s %s",row[0],row[1],row[2],row[3],row[4],row[5]);
    spm.plus(strsql);
//    SOOB.VVOD_SPISOK_add_MD(strsql);

//    soobshw(&SOOB,stdscr,-1,-1,0,1);
    iceb_menu_soob(&spm,data->window);
    continue;
   }

  i1=atoi(row1[0]);

  if(mks == 1)
   {
    if(iceb_u_SRAV(row[1],row[2],1) == 0 || sheta_srs->find(row[2]) >= 0)
     continue;
   }
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  
  rzm=(i1*kls*4)+(i*4);

   /*Период*/

//  if(iceb_u_srav1(g,m,d,gn,mn,dn) <= 0)
//   if(iceb_u_SRAV1(gk,mk,dk,g,m,d) <= 0)
  if(iceb_u_sravmydat(g,m,d,gn,mn,dn) >= 0)
   if(iceb_u_sravmydat(gk,mk,dk,g,m,d) <= 0)
    {
     mdo.plus(atof(row[4]),rzm+2);
     mdo.plus(atof(row[5]),rzm+3);
    }

   /*До периода*/
//  if(iceb_u_SRAV1(g,m,d,gn,mn,dn) > 0)
  if(iceb_u_sravmydat(g,m,d,gn,mn,dn) < 0)
    {
     mdo.plus(atof(row[4]),rzm);
     mdo.plus(atof(row[5]),rzm+1);
    }

 }

if(metka_r == 2)
  sprintf(strsql,"%s\n",gettext("Распечатываем результат"));
else
  sprintf(strsql,"%s\n",gettext("Определяем даты возникновения сальдо"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(imaf,"sl%d.lst",getpid());
class iceb_fopen ff;
if(ff.start(imaf,"w",data->window) != 0)
 return(1);

char imaf_csv[64];
sprintf(imaf_csv,"sl%d.csv",getpid());
class iceb_fopen ff_csv;
if(ff_csv.start(imaf_csv,"w",data->window) != 0)
 return(1);

sprintf(imaf1,"slk%d.lst",getpid());
class iceb_fopen ff1;
if(ff1.start(imaf1,"w",data->window) != 0)
 return(1);

sprintf(imafdeb,"sldeb%d.lst",getpid());
class iceb_fopen ffdeb;
if(ffdeb.start(imafdeb,"w",data->window) != 0)
 return(1);

char imafdeb_csv[64];
sprintf(imafdeb_csv,"sldeb%d.csv",getpid());
class iceb_fopen ffdcsv;
if(ffdcsv.start(imafdeb_csv,"w",data->window) != 0)
 return(1);

char imafkre_csv[64];
sprintf(imafkre_csv,"slkre%d.csv",getpid());
class iceb_fopen ffkcsv;
if(ffkcsv.start(imafkre_csv,"w",data->window) != 0)
 return(1);
 
sprintf(imafkre,"slkre%d.lst",getpid());
class iceb_fopen ffkre;
if(ffkre.start(imafkre,"w",data->window) != 0)
 return(1);

fprintf(ff.ff,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

fprintf(ff1.ff,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

fprintf(ffdeb.ff,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

fprintf(ffkre.ff,"\x1B\x33%c",30); /*Уменьшаем межстрочный интервал*/

iceb_zagolov(gettext("Сальдо по всем контрагентам"),dn,mn,gn,dk,mk,gk,ff.ff,data->window);
iceb_zagolov(gettext("Сальдо по всем контрагентам"),dn,mn,gn,dk,mk,gk,ff1.ff,data->window);
iceb_zagolov(gettext("Контрагенты дебиторы"),dn,mn,gn,dk,mk,gk,ffdeb.ff,data->window);
iceb_zagolov(gettext("Контрагенты кредиторы"),dn,mn,gn,dk,mk,gk,ffkre.ff,data->window);
iceb_zagolov(gettext("Сальдо по всем контрагентам"),dn,mn,gn,dk,mk,gk,ff_csv.ff,data->window);


iceb_zagolov(gettext("Контрагенты дебиторы"),dn,mn,gn,dk,mk,gk,ffdcsv.ff,data->window);
fprintf(ffdcsv.ff,"\n%s|%s|%s|%s|%s|%s|\n",gettext("Код контрагента"),gettext("Наименование контрагента"),
gettext("ЕГРПУ"),gettext("Счёт"),gettext("Сальдо дебетовое"),
gettext("Дата возникновения сальдо"));

iceb_zagolov(gettext("Контрагенты кредиторы"),dn,mn,gn,dk,mk,gk,ffkcsv.ff,data->window);
fprintf(ffkcsv.ff,"\n%s|%s|%s|%s|%s|%s|\n",gettext("Код контрагента"),gettext("Наименование контрагента"),
gettext("ЕГРПУ"),gettext("Счёт"),gettext("Сальдо кредитовое"),
gettext("Дата возникновения сальдо"));

kst=kstdeb=kstkre=5;
kli=klideb=klikre=1;
if(sheta[0] != '\0')
 {
  kst++; kstdeb++; kstkre++;
  fprintf(ff.ff,"%s:%s\n",gettext("Счёт"),sheta);
  fprintf(ff_csv.ff,"%s:%s\n",gettext("Счёт"),sheta);
  fprintf(ff1.ff,"%s:%s\n",gettext("Счёт"),sheta);
  fprintf(ffdeb.ff,"%s:%s\n",gettext("Счёт"),sheta);
  fprintf(ffkre.ff,"%s:%s\n",gettext("Счёт"),sheta);
 }
if(grupa[0] != '\0')
 {
  kst++; kstdeb++; kstkre++;
  fprintf(ff.ff,"%s:%s\n",gettext("Группа"),grupa);
  fprintf(ff_csv.ff,"%s:%s\n",gettext("Группа"),grupa);
  fprintf(ff1.ff,"%s:%s\n",gettext("Группа"),grupa);
  fprintf(ffdeb.ff,"%s:%s\n",gettext("Группа"),grupa);
  fprintf(ffkre.ff,"%s:%s\n",gettext("Группа"),grupa);
 }
if(kontr[0] != '\0')
 {
  kst++; kstdeb++; kstkre++;
  fprintf(ff.ff,"%s:%s\n",gettext("Контрагент"),kontr);
  fprintf(ff_csv.ff,"%s:%s\n",gettext("Контрагент"),kontr);
  fprintf(ff1.ff,"%s:%s\n",gettext("Контрагент"),kontr);
  fprintf(ffdeb.ff,"%s:%s\n",gettext("Контрагент"),kontr);
  fprintf(ffkre.ff,"%s:%s\n",gettext("Контрагент"),kontr);
 }

fprintf(ff_csv.ff,"%s|%s|%s|\
%s %d.%d.%d|%s %d.%d.%d|\
%s|%s|\
%s %d.%d.%d|%s %d.%d.%d|%s|\n",
gettext("Код контрагента"),
gettext("Наименование контрагента"),
gettext("Счёт"),
gettext("Дебет на"),
dn,mn,gn,
gettext("Кредит на"),
dn,mn,gn,
gettext("Дебет за период"),
gettext("Кредит за период"),
gettext("Дебет на"),
dk,mk,gk,
gettext("Кредит на"),
dk,mk,gk,
gettext("Дата возникновения сальдо"));


sal1(kli,metka_r,ff.ff,ff1.ff);
sal2(klideb,2,metka_r,ffdeb.ff);
sal2(klikre,1,metka_r,ffkre.ff);

kst+=5;
kstdeb+=5;
kstkre+=5; //Увеличиваем на шапку

double deb=0.;
double kre=0.;
for(i=0; i< 6 ;i++)
  mas2[i]=0.;
class iceb_u_str edrpu("");
class iceb_u_str data_vs(""); /*дата возникновения возникновения сальдо*/

for(i1=0; i1<=koo ;++i1)
 {
//  strzag(LINES-1,0,koo,i1);
  iceb_pbar(data->bar,koo,i1);    

  for(i2=0; i2< 6 ;i2++)
    mas1[i2]=0.;
  mro=mrodeb=mrokre=0;
  itdeb=itkre=db=kr=0;

//  kor.new_plus(spkontr.ravno(i1));
  sprintf(strsql,"select kod_kontr from %s where nomz=%ld",imatmptab,i1);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
    continue;
  kor.new_plus(row1[0]);
  
  for(i=0; i < kls; i++)
   {
    sh.new_plus(sheta_srs->ravno(i),sizeof(sh)-1);
    if(iceb_u_proverka(sheta,sh.ravno(),1,0) != 0)
     continue;
     
    rzm=(i1*kls*4)+(i*4);

    if(fabs(mdo.ravno(rzm)) < 0.009 && 
       fabs(mdo.ravno(rzm+1)) < 0.009 &&
       fabs(mdo.ravno(rzm+2)) < 0.009 &&
       fabs(mdo.ravno(rzm+3)) < 0.009)
        continue;
    if(fabs(mdo.ravno(rzm)-mdo.ravno(rzm+1)) < 0.009 && 
       fabs(mdo.ravno(rzm+2)) < 0.009 &&
       fabs(mdo.ravno(rzm+3)) < 0.009)
        continue;

     mas1[2]+=mdo.ravno(rzm+2);
     mas1[3]+=mdo.ravno(rzm+3);

    if(mro == 0)
     {
      sprintf(strsql,"select naikon,kod from Kontragent where kodkon='%s'",kor.ravno());
      if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
       {
         iceb_beep();
         sprintf(strsql,"%s %s !\n",gettext("Не найден код контрагента"),kor.ravno());
         iceb_menu_soob(strsql,data->window);
         continue;
       }
      bros.new_plus(row[0]);
      edrpu.new_plus(row[1]);
      if(mdo.ravno(rzm) > mdo.ravno(rzm+1))
       {
        fprintf(ff.ff,"%*s %-*.*s %-*s %11.2f %11s %11.2f %11.2f",
        iceb_u_kolbait(7,kor.ravno()),kor.ravno(),
        iceb_u_kolbait(25,bros.ravno()),iceb_u_kolbait(25,bros.ravno()),bros.ravno(),
        iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
        mdo.ravno(rzm)-mdo.ravno(rzm+1)," ",mdo.ravno(rzm+2),mdo.ravno(rzm+3));

        fprintf(ff_csv.ff,"%s|%s|%s|%.2f|%s|%.2f|%.2f|",
        kor.ravno(),
        bros.ravno(),
        sh.ravno(),
        mdo.ravno(rzm)-mdo.ravno(rzm+1)," ",mdo.ravno(rzm+2),mdo.ravno(rzm+3));

        mas1[0]+=mdo.ravno(rzm)-mdo.ravno(rzm+1);
       }
      else
       {
        fprintf(ff.ff,"%*s %-*.*s %-*s %11s %11.2f %11.2f %11.2f",
        iceb_u_kolbait(7,kor.ravno()),kor.ravno(),
        iceb_u_kolbait(25,bros.ravno()),iceb_u_kolbait(25,bros.ravno()),bros.ravno(),
        iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
        " ",mdo.ravno(rzm+1)-mdo.ravno(rzm),mdo.ravno(rzm+2),mdo.ravno(rzm+3));

        fprintf(ff_csv.ff,"%s|%s|%s|%s|%.2f|%.2f|%.2f|",
        kor.ravno(),
        bros.ravno(),
        sh.ravno(),
        " ",mdo.ravno(rzm+1)-mdo.ravno(rzm),mdo.ravno(rzm+2),mdo.ravno(rzm+3));

        mas1[1]+=mdo.ravno(rzm+1)-mdo.ravno(rzm);
       } 
     }
    else
     {
      if(mdo.ravno(rzm) > mdo.ravno(rzm+1))
       {
        fprintf(ff.ff,"%7s %-25.25s %-*s %11.2f %11s %11.2f %11.2f",
        " "," ",
        iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
        mdo.ravno(rzm)-mdo.ravno(rzm+1)," ",mdo.ravno(rzm+2),mdo.ravno(rzm+3));

        fprintf(ff_csv.ff,"%s|%s|%s|%.2f|%s|%.2f|%.2f|",
        " "," ",
        sh.ravno(),
        mdo.ravno(rzm)-mdo.ravno(rzm+1)," ",mdo.ravno(rzm+2),mdo.ravno(rzm+3));

        mas1[0]+=mdo.ravno(rzm)-mdo.ravno(rzm+1);
       }
      else
       {
        fprintf(ff.ff,"%7s %-25.25s %-*s %11s %11.2f %11.2f %11.2f",
        " "," ",
        iceb_u_kolbait(6,sh.ravno()),sh.ravno()," ",mdo.ravno(rzm+1)-mdo.ravno(rzm),mdo.ravno(rzm+2),mdo.ravno(rzm+3));

        fprintf(ff_csv.ff,"%s|%s|%s|%s|%.2f|%.2f|%.2f|",
        " "," ",
        sh.ravno()," ",mdo.ravno(rzm+1)-mdo.ravno(rzm),mdo.ravno(rzm+2),mdo.ravno(rzm+3));

        mas1[1]+=mdo.ravno(rzm+1)-mdo.ravno(rzm);
       }
     }    

    if(mdo.ravno(rzm)+mdo.ravno(rzm+2) > mdo.ravno(rzm+1)+mdo.ravno(rzm+3))
     {
      deb=(mdo.ravno(rzm)+mdo.ravno(rzm+2))-(mdo.ravno(rzm+1)+mdo.ravno(rzm+3));
      if(metka_r == 1)
        xbudkzw(2,kor.ravno(),sh.ravno(),dk,mk,gk,deb,&data_vs,data->window);
      if(mro == 0)
       {
        fprintf(ff1.ff,"%*s %-*.*s %-*s %11.2f %11s %s\n",
        iceb_u_kolbait(7,kor.ravno()),kor.ravno(),
        iceb_u_kolbait(25,bros.ravno()),iceb_u_kolbait(25,bros.ravno()),bros.ravno(),
        iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
        deb,
        "",
        data_vs.ravno());
       }
      else
       {
        fprintf(ff1.ff,"%7s %-25.25s %-*s %11.2f %11s %s\n",
        " "," ",iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
        deb,
        "",
        data_vs.ravno());

       }


      if(fabs(deb) > 0.009)
       {
        itdeb+=deb;
        if(mrodeb == 0)
         {

          fprintf(ffdeb.ff,"%*s %-*.*s %-*s %11.2f %s\n",
          iceb_u_kolbait(7,kor.ravno()),kor.ravno(),
          iceb_u_kolbait(25,bros.ravno()),iceb_u_kolbait(25,bros.ravno()),bros.ravno(),
          iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
          deb,
          data_vs.ravno());


          fprintf(ffdcsv.ff,"%s|%s|%s|%s|%s|%s|\n",
          kor.ravno(),
          bros.ravno(),
          edrpu.ravno(),
          sh.ravno(),
          iceb_u_double_to_char_zp(deb,2),
          data_vs.ravno());

         }
        else
         {
          fprintf(ffdeb.ff,"%7s %-25.25s %-*s %11.2f %s\n",
          " "," ",iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
          deb,
          data_vs.ravno());


          fprintf(ffdcsv.ff,"%s|%s||%s|%s|%s|\n",
          " "," ",sh.ravno(),
          iceb_u_double_to_char_zp(deb,2),
          data_vs.ravno());
         }
        mrodeb++;
        sstrsl1(&kstdeb,&klideb,2,metka_r,ffdeb.ff);
       }        
     
     
      fprintf(ff.ff," %11.2f %11s %s\n",deb,"",data_vs.ravno());
      fprintf(ff_csv.ff,"|%.2f|%s|%s|\n",deb,"",data_vs.ravno());
      mas1[4]+=deb;
     }
    else
     {
      kre=(mdo.ravno(rzm+1)+mdo.ravno(rzm+3))-(mdo.ravno(rzm)+mdo.ravno(rzm+2));
      if(metka_r == 1)
        xbudkzw(1,kor.ravno(),sh.ravno(),dk,mk,gk,kre,&data_vs,data->window);
      if(mro == 0)
       {
        fprintf(ff1.ff,"%*s %-*.*s %-*s %11s %11.2f %s\n",
        iceb_u_kolbait(7,kor.ravno()),kor.ravno(),
        iceb_u_kolbait(25,bros.ravno()),iceb_u_kolbait(25,bros.ravno()),bros.ravno(),
        iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
        " ",
        kre,
        data_vs.ravno());
       }
      else
       {
        fprintf(ff1.ff,"%7s %-25.25s %-*s %11s %11.2f %s\n",
        " "," ",iceb_u_kolbait(6,sh.ravno()),sh.ravno()," ",
        kre,
        data_vs.ravno());

       }


      if(fabs(kre) > 0.009)
       {
        itkre+=kre;
        if(mrokre == 0)
         {
          fprintf(ffkre.ff,"%*s %-*.*s %-*s %11.2f %s\n",
          iceb_u_kolbait(7,kor.ravno()),kor.ravno(),
          iceb_u_kolbait(25,bros.ravno()),iceb_u_kolbait(25,bros.ravno()),bros.ravno(),
          iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
          kre,
          data_vs.ravno());

          fprintf(ffkcsv.ff,"%s|%s|%s|%s|%s|%s|\n",
          kor.ravno(),
          bros.ravno(),
          edrpu.ravno(),
          sh.ravno(),
          iceb_u_double_to_char_zp(kre,2),
          data_vs.ravno());
         }
        else
         {
          fprintf(ffkre.ff,"%7s %-25.25s %-*s %11.2f %s\n",
          " "," ",iceb_u_kolbait(6,sh.ravno()),sh.ravno(),
          kre,
          data_vs.ravno());

          fprintf(ffkcsv.ff,"%s|%s||%s|%s|%s|\n",
          " "," ",sh.ravno(),
          iceb_u_double_to_char_zp(kre,2),
          data_vs.ravno());

         }
        mrokre++;

        sstrsl1(&kstkre,&klikre,1,metka_r,ffkre.ff);
       }      
      fprintf(ff.ff," %11s %11.2f %s\n"," ",kre,data_vs.ravno());
      fprintf(ff_csv.ff,"|%s|%.2f|%s|\n"," ",kre,data_vs.ravno());
      mas1[5]+=kre;
     }
    sstrsl(&kst,&kli,metka_r,ff.ff,ff1.ff);
    mro++;
   }

  if(mro > 1)
   {
//    sprintf(bros,"------%s %s :",gettext("Итого по"),kor.ravno());
    bros.new_plus("------");
    bros.plus(gettext("Итого по")," ");
    bros.plus(kor.ravno()," :");
    
    if(mas1[0] > mas1[1])
     {
      fprintf(ff.ff,"%*s %11.2f %11s %11.2f %11.2f",
      iceb_u_kolbait(40,bros.ravno()),bros.ravno(),mas1[0]-mas1[1]," ",mas1[2],mas1[3]);
      fprintf(ff_csv.ff,"%s|%.2f|%s|%.2f|%.2f|",
      bros.ravno(),mas1[0]-mas1[1]," ",mas1[2],mas1[3]);
     }
    else
     {
      fprintf(ff.ff,"%*s %11s %11.2f %11.2f %11.2f",
      iceb_u_kolbait(40,bros.ravno()),bros.ravno()," ",mas1[1]-mas1[0],mas1[2],mas1[3]);
      fprintf(ff_csv.ff,"%s|%s|%.2f|%.2f|%.2f|",
      bros.ravno()," ",mas1[1]-mas1[0],mas1[2],mas1[3]);
     }

    if(mas1[0]+mas1[2] > mas1[1]+mas1[3])
     {
      fprintf(ff1.ff,"%*s %11.2f\n",
      iceb_u_kolbait(40,bros.ravno()),bros.ravno(),
      (mas1[0]+mas1[2])-(mas1[1]+mas1[3]));

      fprintf(ff.ff," %11.2f\n",(mas1[0]+mas1[2])-(mas1[1]+mas1[3]));
      fprintf(ff_csv.ff,"|%.2f|\n",(mas1[0]+mas1[2])-(mas1[1]+mas1[3]));

     }
    else
     {
      fprintf(ff1.ff,"%*s %11s %11.2f\n",
      iceb_u_kolbait(40,bros.ravno()),bros.ravno()," ",
      (mas1[1]+mas1[3])-(mas1[0]+mas1[2]));

      fprintf(ff.ff," %11s %11.2f\n"," ",(mas1[1]+mas1[3])-(mas1[0]+mas1[2]));
      fprintf(ff_csv.ff,"|%s|%.2f|\n"," ",(mas1[1]+mas1[3])-(mas1[0]+mas1[2]));
     }
    sstrsl(&kst,&kli,metka_r,ff.ff,ff1.ff);

   }

  if(mrodeb > 1)
   {
//    sprintf(bros,"------%s %s :",gettext("Итого по"),kor.ravno());
    bros.new_plus("------");
    bros.plus(gettext("Итого по")," ");
    bros.plus(kor.ravno()," :");

    fprintf(ffdeb.ff,"%*s %11.2f\n",
    iceb_u_kolbait(40,bros.ravno()),bros.ravno(),itdeb);
    sstrsl1(&kstdeb,&klideb,2,metka_r,ffdeb.ff);
   

    fprintf(ffdcsv.ff,"|%s|||%s\n",
    bros.ravno(),iceb_u_double_to_char_zp(itdeb,2));

   }
  if(mrokre > 1)
   {
//    sprintf(bros,"------%s %s :",gettext("Итого по"),kor.ravno());
    bros.new_plus("------");
    bros.plus(gettext("Итого по")," ");
    bros.plus(kor.ravno()," :");

    fprintf(ffkre.ff,"%*s %11.2f\n",
    iceb_u_kolbait(40,bros.ravno()),bros.ravno(),itkre);
    sstrsl1(&kstkre,&klikre,1,metka_r,ffkre.ff);

    fprintf(ffkcsv.ff,"|%s|||%s|\n",
    bros.ravno(),iceb_u_double_to_char_zp(itkre,2));

   }
  mas2[0]+=mas1[0];
  mas2[1]+=mas1[1];
  mas2[2]+=mas1[2];
  mas2[3]+=mas1[3];
  mas2[4]+=mas1[4];
  mas2[5]+=mas1[5];

 }


if(metka_r == 1)
 {
  fprintf(ff.ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff1.ff,"\
----------------------------------------------------------------------------\n");
  fprintf(ffdeb.ff,"\
----------------------------------------------------------------\n");
  fprintf(ffkre.ff,"\
----------------------------------------------------------------\n");
 }
else
 {
  fprintf(ff.ff,"\
-----------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff1.ff,"\
-----------------------------------------------------------------\n");
  fprintf(ffdeb.ff,"\
-----------------------------------------------------\n");
  fprintf(ffkre.ff,"\
-----------------------------------------------------\n");
 } 


//gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace","foreground","red",NULL);


gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);

sprintf(strsql,"\n%20s",iceb_u_prnbr(mas2[0]));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql," %20s\n",iceb_u_prnbr(mas2[1]));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"%20s",iceb_u_prnbr(mas2[2]));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql," %20s\n",iceb_u_prnbr(mas2[3]));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"%20s",iceb_u_prnbr(mas2[4]));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql," %20s\n",iceb_u_prnbr(mas2[5]));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");






fprintf(ff.ff,"%*s %11.2f %11.2f %11.2f %11.2f %11.2f %11.2f\n",
iceb_u_kolbait(40,gettext("Итого :")),gettext("Итого :"),mas2[0],mas2[1],mas2[2],mas2[3],mas2[4],mas2[5]);

fprintf(ff1.ff,"%*s %11.2f %11.2f\n",
iceb_u_kolbait(40,gettext("Итого :")),gettext("Итого :"),mas2[4],mas2[5]);



fprintf(ffdeb.ff,"%*s %11.2f\n",
iceb_u_kolbait(40,gettext("Итого :")),gettext("Итого :"),mas2[4]);


fprintf(ffdcsv.ff,"|%s|||%s\n",
gettext("Итого :"),iceb_u_double_to_char_zp(mas2[4],2));

fprintf(ffkre.ff,"%*s %11.2f\n",
iceb_u_kolbait(40,gettext("Итого :")),gettext("Итого :"),mas2[5]);


fprintf(ffkcsv.ff,"|%s|||%s|\n",
gettext("Итого :"),iceb_u_double_to_char_zp(mas2[5],2));

if(mas2[0] > mas2[1])
    fprintf(ff.ff,"%*s %11.2f %11s %11.2f %11.2f",
    iceb_u_kolbait(40,gettext("Итого (свернуто):")),gettext("Итого (свернуто):"),mas2[0]-mas2[1]," ",mas2[2],mas2[3]);
  else
    fprintf(ff.ff,"%*s %11s %11.2f %11.2f %11.2f",
    iceb_u_kolbait(40,gettext("Итого (свернуто):")),gettext("Итого (свернуто):")," ",mas2[1]-mas2[0],mas2[2],mas2[3]);

if(mas2[0]+mas2[2] > mas2[1]+mas2[3])
 {
  fprintf(ff1.ff,"%*s %11.2f\n",
  iceb_u_kolbait(40,gettext("Итого (свернуто):")),gettext("Итого (свернуто):"),
  (mas2[0]+mas2[2])-(mas2[1]+mas2[3]));
  fprintf(ff.ff," %11.2f\n",(mas2[0]+mas2[2])-(mas2[1]+mas2[3]));

 }
else
 {
  fprintf(ff1.ff,"%*s %11s %11.2f\n",
  iceb_u_kolbait(40,gettext("Итого (свернуто):")),gettext("Итого (свернуто):")," ",
  (mas2[1]+mas2[3])-(mas2[0]+mas2[2]));
  fprintf(ff.ff," %11s %11.2f\n"," ",(mas2[1]+mas2[3])-(mas2[0]+mas2[2]));
 }

iceb_podpis(ff.ff,data->window);

iceb_podpis(ff1.ff,data->window);

iceb_podpis(ffdeb.ff,data->window);

iceb_podpis(ffkre.ff,data->window);

iceb_podpis(ff_csv.ff,data->window);

iceb_podpis(ffdcsv.ff,data->window);
ffdcsv.end();
iceb_podpis(ffkcsv.ff,data->window);
ffkcsv.end();

ff.end();
ff1.end();
ffdeb.end();
ffkre.end();
ff_csv.end();

iceb_printw_vr(tmm,data->buffer,data->view);

data->oth->spis_imaf.plus(imaf);
data->oth->spis_naim.plus(gettext("Расчёт оборотов по всем контрагентам"));

data->oth->spis_imaf.plus(imaf1);
data->oth->spis_naim.plus(gettext("Расчёт сальдо по всем контрагентам"));

data->oth->spis_imaf.plus(imafdeb);
data->oth->spis_naim.plus(gettext("Список контрагентов с дебетовым сальдо"));

data->oth->spis_imaf.plus(imafkre);
data->oth->spis_naim.plus(gettext("Список контрагентов с кредитовым сальдо"));

for(int nom=0; nom < data->oth->spis_imaf.kolih(); nom++)
 iceb_ustpeh(data->oth->spis_imaf.ravno(nom),1,data->window);

data->oth->spis_imaf.plus(imaf_csv);
data->oth->spis_naim.plus(gettext("Обороты для импорта в электонные таблицы"));

data->oth->spis_imaf.plus(imafdeb_csv);
data->oth->spis_naim.plus(gettext("Контрагенты с дебетовым сальдо для импорта в электронные таблицы"));

data->oth->spis_imaf.plus(imafkre_csv);
data->oth->spis_naim.plus(gettext("Контрагенты с кредитовым сальдо для импорта в электронные таблицы"));

return(0);
}







/******************************************/
/******************************************/

gint rspvk_r1(class rspvk_r_data *data)
{
char strsql[1024];
class iceb_u_str soob;
class iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur1;










sprintf(strsql,"select ns from Plansh where saldo=3 and stat=0 \
order by ns asc");
//printw("strsql=%s\n",strsql);
//refresh();
int kolstr=0;
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не нашли счетов с развернутым сальдо !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_u_spisok sheta_srs;
int pozz=0;
class iceb_u_str sh("");
while(cur.read_cursor(&row) != 0)
 {
/*
  printw("%s %s %s\n",row[0],row[1],row[2]);
  refresh();
*/
  sh.new_plus(row[0]);
  if( pozz < kolstr-1)
   {
    cur.poz_cursor(pozz+1);
    cur.read_cursor(&row);

    /*Если нет субсчетов запоминаем счет*/
    if(iceb_u_SRAV(sh.ravno(),row[0],1) != 0)
     {
      cur.poz_cursor(pozz);
      cur.read_cursor(&row);
     }
   }
  pozz++;
  sheta_srs.plus(row[0]);
 }

sprintf(strsql,"%s\n",gettext("Расчёт сальдо по всем контрагентам и по всем счетам с развернутым сальдо"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s %d.%d.%d %s %d.%d.%d\n",
gettext("Период с"),
data->dn,data->mn,data->gn,
gettext("до"),
data->dk,data->mk,data->gk);

iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s %d\n",gettext("Количество счетов с развернутым сальдо"),sheta_srs.kolih());

iceb_printw(strsql,data->buffer,data->view);


data->voz=saloro3(data->rek_r->shet.ravno(),data->dn,data->mn,data->gn,data->dk,data->mk,data->gk,data->rek_r->par,data->rek_r->kodgr.ravno(),
data->rek_r->kontr.ravno(),&sheta_srs,data->rek_r->metka_r,data->oth,data);



//gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));
iceb_label_set_text_color(data->label,gettext("Расчёт закончен"),"red"); /*красый цвет*/

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//Доступна

gtk_widget_grab_focus(data->knopka);
//iceb_printw_vr(vremn,data->buffer,data->view);

iceb_printw(" \n",data->buffer,data->view);

return(FALSE);

}
