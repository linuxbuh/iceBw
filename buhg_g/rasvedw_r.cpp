/*$Id: rasvedw_r.c,v 1.30 2013/09/26 09:46:54 sasa Exp $*/
/*11.01.2018	31.10.2006	Белых А.И.	rasvedw_r.c
Расчёт платёжной ведомости
*/
#include <errno.h>
#include "buhg_g.h"
#include "rasvedw.h"
/*****************
В ТЕРАМИНАЛЬНОЙ ВЕРСИИ БОЛЬШЕ ПОМЕЩАЕТСЯ НА ЛИСТ
#define         STR1    16   //Количество строк на 1-ом листе
#define         STR2    28  //Количество строк на 2-ом листе
#define         STRKOR  76  //Количество строк на листах с корешками
******************/
#define         STR1    11   //Количество строк на 1-ом листе
#define         STR2    23  //Количество строк на 2-ом листе
//#define         STRKOR  71  //Количество строк на листах с корешками
extern int kol_strok_na_liste_l_u;
class rasvedw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class rasvedw_rek *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  rasvedw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rasvedw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasvedw_r_data *data);
gint rasvedw_r1(class rasvedw_r_data *data);
void  rasvedw_r_v_knopka(GtkWidget *widget,class rasvedw_r_data *data);

void  srasved(short mr,short gr,FILE *kaw,struct tm *bf);
void razbiv(char imaf[],char imafkor1[],char imafkor2[],char imafkor2s[],GtkWidget *wpredok);

extern SQL_baza bd;




extern int      kol_strok_na_liste;
extern short	*obud; /*Обязательные удержания*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern short mmm,ggg;
extern class iceb_u_str shrpz; /*Счет расчётов по зарплате*/
extern double   okrg; /*Округление*/

int rasvedw_r(class rasvedw_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class rasvedw_r_data data;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать ведомость заработной платы"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rasvedw_r_key_press),&data);

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

repl.plus(gettext("Распечатать ведомость заработной платы"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(), iceb_get_pnk("00",0,wpredok));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(rasvedw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)rasvedw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rasvedw_r_v_knopka(GtkWidget *widget,class rasvedw_r_data *data)
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

gboolean   rasvedw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasvedw_r_data *data)
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

gint rasvedw_r1(class rasvedw_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
short mr,gr;

iceb_u_rsdat1(&mr,&gr,data->rk->data.ravno());


sprintf(strsql,"%s\n",gettext("Сортируем"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and mes=%d",gr,mr);
SQLCURSOR cur,cur1,curtmp;
int kolstr2=0;
if((kolstr2=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr2 == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_u_str fio;
class iceb_u_str dokument;
class iceb_u_str dolgn;
class iceb_u_str vidan;

class iceb_tmptab tabtmp;
const char *imatmptab={"rasvedw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
podr int not null,\
tabn int not null,\
fio char(112) not null,\
inn char(24) not null,\
dokum char(112) not null,\
vidan char(255) not null,\
dolg char(112) not null,\
kateg int not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  


int kolstr3=0;
float kolstr1=0.;
SQL_str row,rowtmp,row1;
int tabb;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr2,++kolstr1);    

  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->podr.ravno(),row[1],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->kod_kat.ravno(),row[2],0,0) != 0)
    continue;

  if(data->rk->kod_grup_pod.ravno()[0] != '\0')
   {
    //Узнаем в какой группе подразделение
    sprintf(strsql,"select grup from Podr where kod=%s",row[1]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     if(iceb_u_proverka(data->rk->kod_grup_pod.ravno(),row1[0],1,0) != 0)
       continue;
   }

  tabb=atol(row[0]);
  sprintf(strsql,"select fio,inn,nomp,vidan,dolg,kateg from Kartb where tabn=%d",tabb);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabb);
    iceb_menu_soob(strsql,data->window);
    continue;
   }
  kolstr3++;

  
  dokument.new_plus(gettext("Паспорт"));
  dokument.plus(" ",row1[2]);
  fio.new_plus(row1[0]);
  dolgn.new_plus(row1[4]);
  vidan.new_plus(row1[3]);
  
  sprintf(strsql,"insert into %s values (%s,%d,'%s','%s','%s','%s','%s',%s)",
  imatmptab,
  row[1],tabb,fio.ravno_filtr(),row1[1],dokument.ravno_filtr(),vidan.ravno_filtr(),dolgn.ravno_filtr(),row1[5]);

  iceb_sql_zapis(strsql,1,0,data->window);    

 }



if(data->rk->metka_sort == 0) //Сортировать счет-склад-наименование материала.
  sprintf(strsql,"select * from %s order by podr asc,tabn asc",imatmptab);

if(data->rk->metka_sort == 1) //Сортировать счет-склад-группа-наименование материала.
  sprintf(strsql,"select * from %s order by podr asc,fio asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s\n",gettext("Распечатываем"));
iceb_printw(strsql,data->buffer,data->view);



char imaf[64];
char imaf1[64];
char imaf2[64];
char imaf3[64];
char imaftmp[64];
char imafkor1t[64];
char imafkor2t[64];
char imafkor2s[64];
char imafpodr[64];

sprintf(imaf,"kor%d.lst",getpid());
sprintf(imaf1,"vedom%d.lst",getpid());
sprintf(imaf2,"kasord%d.lst",getpid());
sprintf(imaftmp,"tt%d.tmp",getpid());
sprintf(imafkor1t,"kor1%d.tmp",getpid());
sprintf(imafkor2t,"kor2%d.tmp",getpid());
sprintf(imafkor2s,"kor2s%d.tmp",getpid());
sprintf(imafpodr,"zpodr%d.lst",getpid());

class iceb_u_double SUMPODRHOZ;
class iceb_u_double SUMPODRBUD;

class iceb_u_int KOLLISTHOZ;
class iceb_u_int KOLLISTBUD;
FILE *ffpodr;
if((ffpodr = fopen(imafpodr,"w")) == NULL)
 {
  iceb_er_op_fil(imafpodr,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sapreestpw(mr,gr,0,ffpodr);
FILE *ffpodrb=NULL;
char imafpodrb[64];
memset(imafpodrb,'\0',sizeof(imafpodrb));
if(data->rk->shet.getdlinna() > 1)
 {
  sprintf(imafpodrb,"zpodrb%d.lst",getpid());

  if((ffpodrb = fopen(imafpodrb,"w")) == NULL)
   {
    iceb_er_op_fil(imafpodrb,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  sapreestpw(mr,gr,0,ffpodrb);

 }
FILE *kaw=NULL;
if((kaw = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
fprintf(kaw,"\x1B\x33%c",30); //Изменение межстрочного расстояния
FILE *ffkor2s;
if((ffkor2s = fopen(imafkor2s,"w")) == NULL)
 {
  iceb_er_op_fil(imafkor2s,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
fprintf(ffkor2s,"\x1B\x33%c",30); //Изменение межстрочного расстояния
FILE *kawk1;
if((kawk1 = fopen(imafkor1t,"w")) == NULL)
 {
  iceb_er_op_fil(imafkor1t,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
fprintf(kawk1,"\x1B\x33%c",30); //Изменение межстрочного расстояния
FILE *kawk2;
if((kawk2 = fopen(imafkor2t,"w")) == NULL)
 {
  iceb_er_op_fil(imafkor2t,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
fprintf(kawk2,"\x1B\x33%c",30); //Изменение межстрочного расстояния
FILE *ffvedom;
if((ffvedom = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
FILE *ff2=NULL;
memset(imaf3,'\0',sizeof(imaf3));
if(data->rk->shet.getdlinna() > 1)
 {
  sprintf(imaf3,"vvsh%d.lst",getpid());
  if((ff2 = fopen(imaf3,"w")) == NULL)
   {
    iceb_er_op_fil(imaf3,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }
 }
FILE *ff;
if((ff = fopen(imaf2,"w")) == NULL) 
 {
    iceb_er_op_fil(imaf2,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
 }

srasved(mr,gr,kaw,bf);
srasved(mr,gr,ffkor2s,bf);
srasved(mr,gr,kawk1,bf);
srasved(mr,gr,kawk2,bf);

gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);

int podr=0;
double sal=0.,itogo=0.,itogo1=0.,itogoo=0.,itogoopb=0.;
kolstr1=0;
int nomer_vedom=0;
int nomer_vedom_b=0;
kolstr1=0.;
char bros[512];
double oin=0.;
double oiu=0.;
double itsal=0.;
char  str[1024];
double in1=0.;
double inah=0.;
double iu=0.;
double ius=0.;
double iub=0.;
double iusb=0.;
double nahshet=0.;
int kpd=0;
class iceb_u_str inn("");
double saldb=0.;
int metkavs=0; //Метка выдачи шапки
int metkavsb=0; //Метка выдачи шапки для бюджета
class iceb_u_str naipod("");
int  listn=0,listn1=0; /*Номер листа*/
int kolst=0,kolst1=0; /*Счетчик количества строк*/
int kolstr=0;
int im=0;
int prn=0;
int kodnah=0;
int knah=0;
class iceb_u_str naim("");
double sym;
short mz,gz;
class iceb_u_str st("");
double prom=0.;
short mkvk=0; /*0 не выдано кассой 1-выдано кассой*/
short kd=0;
short d,m,g;
double bbr=0.,bb=0.,bb1=0.;
double ius1=0.,iusb1=0.;
short metkakas=0;
double viplata=0.;
int bros_int=0; /*не используемая в алгоритме переменная*/
int kateg=0;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  iceb_pbar(data->bar,kolstrtmp,++kolstr1);    
  in1=inah=nahshet=iu=ius=iub=iusb=0.;


  kpd=atoi(rowtmp[0]);
  tabb=atoi(rowtmp[1]);
  fio.new_plus(rowtmp[2]);
    
  inn.new_plus(rowtmp[3]);
  dokument.new_plus(rowtmp[4]);
  vidan.new_plus(rowtmp[5]);
  dolgn.new_plus(rowtmp[6]);
  kateg=atoi(rowtmp[7]);
        

  dokument.plus(" ",vidan.ravno());
  
  sal=0.;
  /*Читаем сальдо*/

  sal=zarsaldw(1,mr,gr,tabb,&saldb,data->window);
  itsal+=sal;
  if(sal < -0.009 && data->rk->metka_saldo == 1)
   {
    sprintf(strsql,"%s\n%s %d %s",gettext("Внимание ! Отрицательное сальдо !"),
    gettext("Табельный номер"),tabb,fio.ravno());
    iceb_menu_soob(strsql,data->window);
   }            



  if(podr != kpd)
   {
    if(podr != 0)
     {
      if(metkavs == 0)
       {
        itogw(ffvedom,itogo,data->window);
        fprintf(ffvedom,"\f");
       }

      if(data->rk->shet.ravno()[0] != '\0')
       if(metkavsb == 0)
        {
         itogw(ff2,itogo1,data->window);
         fprintf(ff2,"\f");
         fprintf(ffpodrb,"%3d %-*.*s %10.2f\n",
         podr,
         iceb_u_kolbait(40,naipod.ravno()),iceb_u_kolbait(40,naipod.ravno()),naipod.ravno(),
         itogo1);
         
         itogoopb+=itogo1; 
         if(itogo1 != 0.)
          {
           SUMPODRBUD.plus(itogo1,-1);
           KOLLISTBUD.plus(listn1,-1);
          }
        }

      fprintf(ffpodr,"%3d %-*.*s %10.2f\n",
      podr,
      iceb_u_kolbait(40,naipod.ravno()),iceb_u_kolbait(40,naipod.ravno()),naipod.ravno(),
      itogo);
      
      if(itogo != 0.)
       {
        SUMPODRHOZ.plus(itogo,-1);
        KOLLISTHOZ.plus(listn,-1);
       }      

      itogoo+=itogo; 

      nomer_vedom=nomer_vedom_b=0;
      itogo1=itogo=0.;
     }
    sprintf(strsql,"select naik from Podr where kod=%d",kpd);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),kpd);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
    naipod.new_plus(row1[0]);
//    iceb_u_ud_simv(naipod,"#");
    sprintf(strsql,"\n%s N%d %s\n",gettext("Подразделение"),kpd,naipod.ravno());
    iceb_printw(strsql,data->buffer,data->view);

    fprintf(kaw,"\n%s N%d %s\n",gettext("Подразделение"),kpd,naipod.ravno());
    fprintf(ffkor2s,"\n%s N%d %s\n",gettext("Подразделение"),kpd,naipod.ravno());
    fprintf(kawk1,"\n%s N%d %s\n",gettext("Подразделение"),kpd,naipod.ravno());
    fprintf(kawk2,"\n%s N%d %s\n",gettext("Подразделение"),kpd,naipod.ravno());
    listn=listn1=1;
    kolst=kolst1=0;

    metkavs=metkavsb=1;
    podr=kpd;
   }

  sprintf(strsql,"select * from Zarp where datz >= '%04d-%02d-01' and \
datz <= '%04d-%02d-31' and tabn=%d and suma <> 0. order by prn,knah asc",
  gr,mr,gr,mr,tabb);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr == 0)
   {
    continue;
   }

  im=0;
  while(cur.read_cursor(&row) != 0)
   {

    
    if(im == 0)
      peh1w(naipod.ravno(),fio.ravno(),tabb,inn.ravno(),mr,gr,dolgn.ravno(),kateg,kaw,kawk1,kawk2,ffkor2s,data->window);
    prn=atoi(row[2]);
    if(kodnah != prn)
     {
      if(prn == 1)
       {
        sprintf(str,"         %s",gettext("Н а ч и с л е н и я"));
	strfilw(str,str,1,kaw);
	strfilw(str,str,1,ffkor2s);
        fprintf(kawk1,"\n%s\n",str);
        fprintf(kawk2,"\n%s\n",str);
       }

      if(prn == 2)
       {
        sprintf(str,"         %s",gettext("У д е р ж а н и я"));
	strfilw(str,str,1,kaw);
	strfilw(str,str,1,ffkor2s);
        fprintf(kawk1,"\n%s\n",str);
        fprintf(kawk2,"\n%s\n",str);
       }
      kodnah=prn;
     }

    if(prn == 1)
     {
      if(iceb_u_proverka(data->rk->kod_nah.ravno(),row[3],0,1) == 0)
        continue;
      sprintf(strsql,"select naik from Nash where kod=%s",row[3]);
     }

    if(prn == 2)
      sprintf(strsql,"select naik from Uder where kod=%s",row[3]);
       
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
      naim.new_plus(row1[0]);
    else
      naim.new_plus("");
    knah=atoi(row[3]);
    sym=atof(row[4]);
    gz=atoi(row[6]);
    mz=atoi(row[7]);

    prom=sym;
    mkvk=0;
    if(prn == 2)
     {
      prom*=-1;

      if(provkodw(obud,knah) < 0)
        if(gz != gr || mz != mr)
            mkvk=1;
      
     }

    kd=atoi(row[8]);

    iceb_u_prnb(prom,&st);
    bros[0]='\0';
    if(kd != 0)
     sprintf(bros,"%d",kd);     
 
    sprintf(str,"%3d %-*.*s %2s %2d.%4d %-2s %11s",
    knah,
    iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
    bros,
    mz,gz,
    row[13],st.ravno());

    if(prn == 2 && mkvk == 1)
     {
      strfilw(str," ",0,kaw);
      strfilw(str,str,0,ffkor2s);

      iceb_u_rsdat(&d,&m,&g,row[0],2);
      sprintf(str,"%3d %-*.*s %2s %2d.%4d %-2s %12s %02d.%02d.%d %-*s %s",
      knah,
      iceb_u_kolbait(35,naim.ravno()),iceb_u_kolbait(35,naim.ravno()),naim.ravno(),
      bros,
      mz,gz,row[13],st.ravno(),d,m,g,
      iceb_u_kolbait(5,row[5]),row[5],
      row[12]);

      fprintf(kawk1,"%s\n",str);
      fprintf(kawk2,"\n");
     }
    else
     {
      strfilw(str,str,0,kaw);
      strfilw(str,str,0,ffkor2s);

      iceb_u_rsdat(&d,&m,&g,row[0],2);
      sprintf(str,"%3d %-*.*s %2s %2d.%4d %-2s %12s %02d.%02d.%d %-*s %s",
      knah,
      iceb_u_kolbait(35,naim.ravno()),iceb_u_kolbait(35,naim.ravno()),naim.ravno(),
      bros,mz,gz,row[13],st.ravno(),d,m,g,
      iceb_u_kolbait(5,row[5]),row[5],
      row[12]);

      fprintf(kawk1,"%s\n",str);
      fprintf(kawk2,"%s\n",str);
     }

    if(prn == 1)
     {

      in1+=sym;
      inah+=sym;
      oin+=sym;

      if(iceb_u_proverka(data->rk->shet.ravno(),row[5],0,1) == 0)
         nahshet+=sym;
     }

    if(prn == 2)
     {
      iu+=sym;
      if(iceb_u_proverka(shetbu,row[5],0,1) == 0)
         iub+=sym;
      
      if(gz == gr && mz == mr)
       {
	ius+=sym;

        if(iceb_u_proverka(shetbu,row[5],0,1) == 0)
          iusb+=sym;
       }
      else
       {
        if(provkodw(obud,knah) >= 0)
         {
          ius+=sym;
          if(iceb_u_proverka(shetbu,row[5],0,1) == 0)
            iusb+=sym;
         }
       }
      oiu+=sym;


     }

    im++;
   }
  
  if(im > 0 || sal != 0.)
   {
    if(im == 0)
      peh1w(naipod.ravno(),fio.ravno(),tabb,inn.ravno(),mr,gr,dolgn.ravno(),kateg,kaw,kawk1,kawk2,ffkor2s,data->window);

    bbr=inah+iu+sal;
    itogizw(inah,iu,ius,sal,nahshet,iub,iusb,saldb,kaw,kawk1,kawk2,ffkor2s);

    if(data->rk->metka_saldo == 0)
      bbr=in1+iu+sal;

    if(data->rk->metka_saldo == 1)
     {
      ius1=prbperw(mr,gr,mmm,ggg,tabb,&iusb1,0,data->window);
      bbr=in1+ius+ius1;
     }

    if(bbr > 0.009)
     {
      char osnovan[112];
      sprintf(osnovan,"%s %d.%d%s",
      gettext("Заработная плата"),mr,gr,
      gettext("г."));
      sprintf(bros,"%s%d",ICEB_ZAR_PKTN,tabb);
      iceb_kasord2("","",0,bf->tm_mon+1,bf->tm_year+1900,shrpz.ravno(),bros,bbr,fio.ravno(),osnovan,"",dokument.ravno(),"",ff,data->window);
      if(metkakas == 0)
       {
        metkakas++;
        fprintf(ff,"\n\n\n");
       }
      else
       {
        metkakas=0;
        fprintf(ff,"\f");
       }
      
      bb=bbr;

      if(shetbu == NULL)
       {
        if(nahshet != 0.)
         {
          bb=(inah-nahshet)*bbr/inah;
          bb=iceb_u_okrug(bb,okrg);        
         }
        if(bb > 0.009)
         {
          if(metkavs == 1)
           {
            if(zarstfn53w(naipod.ravno(),mr,gr,shrpz.ravno(),"",&bros_int,ffvedom) != 0)
              return(FALSE);
            sspp1w(ffvedom,listn,&bros_int);
            metkavs=0;
           }         

          if(data->rk->proc_vip.ravno_atof() != 100.)
           {
            viplata=bb*data->rk->proc_vip.ravno_atof()/100.;                    
            viplata=iceb_u_okrug(viplata,okrg);
           }
          else
            viplata=bb;

          itogo+=viplata;
          kolst++;

          zarstvedw(&nomer_vedom,tabb,fio.ravno(),viplata,ffvedom);
         }

        if(data->rk->shet.ravno()[0] != '\0' && nahshet != 0.)
         {
          bb1=bbr-bb;
          if(data->rk->proc_vip.ravno_atof() != 100.)
           {
            bb1=bb1*data->rk->proc_vip.ravno_atof()/100.;                    
            bb1=iceb_u_okrug(bb1,okrg);
           }
          if(bb1 > 0.009)
           {
            if(metkavsb == 1)
             {
              if(zarstfn53w(naipod.ravno(),mr,gr,shrpz.ravno(),"",&bros_int,ff2) != 0)
                return(FALSE);
              sspp1w(ff2,listn1,&bros_int);
              metkavsb=0;
             }
            itogo1+=bb1;
            kolst1++;
            zarstvedw(&nomer_vedom_b,tabb,fio.ravno(),bb1,ff2);
           }
         }
       }
     }

    if(shetbu != NULL)
     {
      if(data->rk->metka_saldo == 0 ) /*С учетом сальдо*/
       {
        bb=bbr-(nahshet+iub+saldb);
       }               
      if(data->rk->metka_saldo == 1 ) /*Без учета сальдо*/
       {
        bb=in1-nahshet+(ius-iusb)+(ius1-iusb1);

       }
       
      if(bb > 0.009)
       {
        if(metkavs == 1)
         {
            if(zarstfn53w(naipod.ravno(),mr,gr,shrpz.ravno(),"",&bros_int,ffvedom) != 0)
              return(FALSE);
          sspp1w(ffvedom,listn,&bros_int);
          metkavs=0;
         }         
        if(data->rk->proc_vip.ravno_atof() != 100.)
         {
          viplata=bb*data->rk->proc_vip.ravno_atof()/100.;                    
          viplata=iceb_u_okrug(viplata,okrg);
         }
        else
          viplata=bb;
        itogo+=viplata;
        kolst++;
        
        zarstvedw(&nomer_vedom,tabb,fio.ravno(),viplata,ffvedom);
       }
      bb=bbr-bb;
      if(data->rk->proc_vip.ravno_atof() != 100.)
       {
        bb=bb*data->rk->proc_vip.ravno_atof()/100.;                    
        bb=iceb_u_okrug(bb,okrg);
       }
      if(bb > 0.009)
       {
        if(metkavsb == 1)
          {
           if(zarstfn53w(naipod.ravno(),mr,gr,shrpz.ravno(),"",&bros_int,ff2) != 0)
                return(FALSE);
           sspp1w(ff2,listn1,&bros_int);
           metkavsb=0;
          }
        itogo1+=bb;
        kolst1++;
        zarstvedw(&nomer_vedom_b,tabb,fio.ravno(),bb,ff2);
       }

     }

    sprintf(strsql,"%6d %-*.*s %8.2f\n",
    tabb,
    iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno(),
    bbr);

    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
//    iceb_printw(strsql,data->buffer,data->view);
   }



  if(listn == 1 && kolst >= STR1)
   {
    listn++;
    kolst=1;
    sspp1w(ffvedom,listn,&bros_int);
   }
  if(listn > 1 && kolst > STR2)
   {
    listn++;
    kolst=1;
    sspp1w(ffvedom,listn,&bros_int);
   }

  if(data->rk->shet.ravno()[0] != '\0')
   {
    if(listn1 == 1 && kolst1 >= STR1)
     {
      listn1++;
      kolst1=1;
      sspp1w(ff2,listn1,&bros_int);
     }
    if(listn1 > 1 && kolst1 > STR2)
     {
      listn1++;
      kolst1=1;
      sspp1w(ff2,listn1,&bros_int);
     }
   }
 }

if(metkavs == 0)
  itogw(ffvedom,itogo,data->window);

fprintf(ffpodr,"%3d %-*.*s %10.2f\n",
podr,
iceb_u_kolbait(40,naipod.ravno()),iceb_u_kolbait(40,naipod.ravno()),naipod.ravno(),
itogo);

SUMPODRHOZ.plus(itogo,-1);
KOLLISTHOZ.plus(listn,-1);

itogoo+=itogo;
itogoopb+=itogo1;

fprintf(ffpodr,"\
--------------------------------------------------------\n");
fprintf(ffpodr,"%*s %10.2f\n",iceb_u_kolbait(44,gettext("Итого")),gettext("Итого"),itogoo);

if(data->rk->shet.ravno()[0] != '\0')
 {
  fprintf(ffpodrb,"%3d %-*.*s %10.2f\n",
  podr,
  iceb_u_kolbait(40,naipod.ravno()),iceb_u_kolbait(40,naipod.ravno()),naipod.ravno(),
  itogo1);
  
  SUMPODRBUD.plus(itogo1,-1);
  KOLLISTBUD.plus(listn1,-1);
  fprintf(ffpodrb,"\
--------------------------------------------------------\n");
  fprintf(ffpodrb,"%*s %10.2f\n",iceb_u_kolbait(44,gettext("Итого")),gettext("Итого"),itogoopb);
  if(metkavsb == 0)
   itogw(ff2,itogo1,data->window);

  iceb_podpis(ffpodrb,data->window);
  fclose(ffpodrb);

  fclose(ff2);

  sumprvedw(&SUMPODRBUD,&KOLLISTBUD,imaf3,data->window);
 }

sprintf(strsql,"\n%-*s %15s\n",
iceb_u_kolbait(25,gettext("Сальдо")),gettext("Сальдо"),iceb_u_prnbr(itsal));

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

memset(bros,'\0',sizeof(bros));
iceb_u_prnb(oin,bros);

sprintf(strsql,"\
%-*s %15s\n\
%-*s %15s\n",
iceb_u_kolbait(25,gettext("Итого начислено")),gettext("Итого начислено"),bros,
iceb_u_kolbait(25,gettext("Итого удержано")),gettext("Итого удержано"),iceb_u_prnbr(oiu));

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"%s: %.2f\n",
gettext("Общая сумма к выдаче"),itogoo);

iceb_printw(strsql,data->buffer,data->view);

fprintf(kaw,"%s: %.2f\n",
gettext("Общая сумма к выдаче"),itogoo);

fprintf(kawk1,"%s: %.2f\n",
gettext("Общая сумма к выдаче"),itogoo);

fprintf(kawk2,"%s: %.2f\n",
gettext("Общая сумма к выдаче"),itogoo);

sprintf(strsql,"%s: %.2f\n",
gettext("Общий итог с учётом сальдо"),itsal+oin+oiu);
iceb_printw(strsql,data->buffer,data->view);


fclose(kaw);
fclose(ffkor2s);
fclose(kawk1);
fclose(kawk2);
fclose(ffvedom);

sumprvedw(&SUMPODRHOZ,&KOLLISTHOZ,imaf1,data->window);

iceb_podpis(ffpodr,data->window);

fclose(ffpodr);

fclose(ff);


char imafkor2[64];
char imafkor1[64];

razbiv(imaf,imafkor1,imafkor2,imafkor2s,data->window);

data->rk->imaf.plus(imaf);
data->rk->imaf.plus(imaf1);
data->rk->imaf.plus(imaf2);
data->rk->imaf.plus(imafkor1);
data->rk->imaf.plus(imafkor2);
data->rk->imaf.plus(imafpodr);
data->rk->imaf.plus(imafkor2s);

data->rk->naimf.plus(gettext("Двойные корешки (с сальдо/без сальдо)"));
data->rk->naimf.plus(gettext("Ведомость на выдачу заработной платы"));
data->rk->naimf.plus(gettext("Кассовые ордера на получение зарплаты"));
data->rk->naimf.plus(gettext("Одинарные корешки (с сальдо)"));
data->rk->naimf.plus(gettext("Одинарные корешки (без сальдо)"));
data->rk->naimf.plus(gettext("Реестр платёжных ведомостей"));
data->rk->naimf.plus(gettext("Двойные корешки (с сальдо/с сальдо)"));

if(data->rk->shet.ravno()[0] != '\0')
 {
  data->rk->imaf.plus(imafpodrb);
  data->rk->imaf.plus(imaf3);
  data->rk->naimf.plus(gettext("Реестр платёжных ведомостей (бюджет)"));
  data->rk->naimf.plus(gettext("Кассовые ордера на получение зарплаты (бюджет)"));
 }

for(int nomer=0; nomer < data->rk->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rk->imaf.ravno(nomer),1,data->window);
 
data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/***************************/
/*Шапка распечаток корешков*/
/***************************/
void  srasved(short mr,short gr,FILE *kaw,struct tm *bf)
{
fprintf(kaw,"\
%s\n\
%s %d.%d%s\n\
%s %d.%d.%d%s  %s: %d.%d\n\n",
iceb_get_pnk("00",0,NULL),
gettext("Ведомость на зарплату за"),
mr,gr,
gettext("г."),
gettext("По состоянию на"),
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
gettext("г."),
gettext("Время"),
bf->tm_hour,bf->tm_min);

}

/******************************/
/*Разбивка распечатки на листы*/
/******************************/
void razbiv(char imaf[],char imafkor1[],char imafkor2[],char imafkor2s[],GtkWidget *wpredok)
{
FILE            *ff,*ff1,*ff2,*ffk1,*ffk2,*ffk1t,*ffk2t;
FILE		*ffkor2s,*ffkor2ss;
short           dlkr;
short           mok;
short           i;
char		str[1024];
class iceb_u_str bros("");
char		imaf1[64],imaf2[64];
char		imafkor1t[64],imafkor2t[64];
char		imafkor2ss[64];

if(iceb_poldan("Распечатка корешков на рулоне",&bros,"zarnast.alx",wpredok) == 0)
 if(iceb_u_SRAV(bros.ravno(),"Вкл",1) == 0)
  {
   sprintf(imaf1,"tt%d.tmp",getpid());
   rename(imaf1,imaf);
   if((ff = fopen(imaf,"a")) == NULL)
    {
     iceb_er_op_fil(imaf,"",errno,wpredok);
     return;
    }
   fclose(ff);

   sprintf(imafkor1t,"kor1%d.tmp",getpid());
   sprintf(imafkor1,"kor1_%d.lst",getpid());
   rename(imafkor1t,imafkor1);
   if((ff = fopen(imafkor1,"a")) == NULL)
    {
     iceb_er_op_fil(imafkor1,"",errno,wpredok);
     return;
    }
   fclose(ff);

   sprintf(imafkor2ss,"kor2s%d.tmp",getpid());
   sprintf(imafkor2s,"kor2s%d.lst",getpid());
   rename(imafkor2ss,imafkor2s);
   if((ff = fopen(imafkor2s,"a")) == NULL)
    {
     iceb_er_op_fil(imafkor2s,"",errno,wpredok);
     return;
    }
   fclose(ff);

   sprintf(imafkor2t,"kor2%d.tmp",getpid());
   sprintf(imafkor2,"kor2_%d.lst",getpid());
   rename(imafkor2t,imafkor2);

   if((ff = fopen(imafkor2,"a")) == NULL)
    {
     iceb_er_op_fil(imafkor2,"",errno,wpredok);
     return;
    }
   fclose(ff);

   return;   
  }



sprintf(imaf1,"tt%d.tmp",getpid());
if((ff= fopen(imaf1,"r")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }

sprintf(imaf2,"t%d.tmp",getpid());
if((ff1= fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,wpredok);
  return;
 }

mok=dlkr=0;
while(fgets(str,sizeof(str),ff) != 0)
 {
  dlkr++;
  if(iceb_u_SRAV(str,"------",1) == 0)
   mok++;
  if(mok == 2)
   {
    fprintf(ff1,"%d\n",dlkr);
    mok=dlkr=0;
   }

 }

fclose(ff1);
rewind(ff);

if((ff1= fopen(imaf2,"r")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,wpredok);
  return;
 }

sprintf(imafkor1t,"kor1%d.tmp",getpid());
if((ffk1t= fopen(imafkor1t,"r")) == NULL)
 {
  iceb_er_op_fil(imafkor1t,"",errno,wpredok);
  return;
 }

sprintf(imafkor2t,"kor2%d.tmp",getpid());
if((ffk2t= fopen(imafkor2t,"r")) == NULL)
 {
  iceb_er_op_fil(imafkor2t,"",errno,wpredok);
  return;
 }

sprintf(imafkor2ss,"kor2s%d.tmp",getpid());
if((ffkor2ss= fopen(imafkor2ss,"r")) == NULL)
 {
  iceb_er_op_fil(imafkor2ss,"",errno,wpredok);
  return;
 }


if((ff2= fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
sprintf(imafkor1,"kor1_%d.lst",getpid());
if((ffk1= fopen(imafkor1,"w")) == NULL)
 {
  iceb_er_op_fil(imafkor1,"",errno,wpredok);
  return;
 }

sprintf(imafkor2,"kor2_%d.lst",getpid());
if((ffk2= fopen(imafkor2,"w")) == NULL)
 {
  iceb_er_op_fil(imafkor2,"",errno,wpredok);
  return;
 }

sprintf(imafkor2s,"kor2s%d.lst",getpid());
if((ffkor2s= fopen(imafkor2s,"w")) == NULL)
 {
  iceb_er_op_fil(imafkor2s,"",errno,wpredok);
  return;
 }

mok=dlkr=0;
while(fgets(str,sizeof(str),ff1) != NULL)
 {
  dlkr=atoi(str);
  mok+=dlkr;
  if(mok > kol_strok_na_liste_l_u)
   {
    fprintf(ff2,"\f");
    fprintf(ffk1,"\f");
    fprintf(ffk2,"\f");
    fprintf(ffkor2s,"\f");
    mok=dlkr;
   }

  for(i=0; i < dlkr; i++)
   {
    memset(str,'\0',sizeof(str));
    if(fgets(str,sizeof(str),ff) != NULL)
     fprintf(ff2,"%s",str);

    memset(str,'\0',sizeof(str));
    if(fgets(str,sizeof(str),ffk1t) != NULL)
     fprintf(ffk1,"%s",str);

    memset(str,'\0',sizeof(str));
    if(fgets(str,sizeof(str),ffk2t) != NULL)
     fprintf(ffk2,"%s",str);

    memset(str,'\0',sizeof(str));
    if(fgets(str,sizeof(str),ffkor2ss) != NULL)
     fprintf(ffkor2s,"%s",str);
   }

 }

while(fgets(str,sizeof(str),ff) != NULL)
  fprintf(ff2,"%s",str);

memset(str,'\0',sizeof(str));
while(fgets(str,sizeof(str),ffk1t) != NULL)
 fprintf(ffk1,"%s",str);

memset(str,'\0',sizeof(str));
while(fgets(str,sizeof(str),ffk2t) != NULL)
  fprintf(ffk2,"%s",str);

memset(str,'\0',sizeof(str));
while(fgets(str,sizeof(str),ffkor2ss) != NULL)
 fprintf(ffkor2s,"%s",str);


fclose(ff);
fclose(ff1);
fclose(ff2);
fclose(ffk1);
fclose(ffk2);
fclose(ffk1t);
fclose(ffk2t);
fclose(ffkor2ss);
fclose(ffkor2s);

unlink(imaf1);
unlink(imaf2);
unlink(imafkor1t);
unlink(imafkor2t);
unlink(imafkor2ss);

}
