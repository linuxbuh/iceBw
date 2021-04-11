/*$Id: dvtmcf1_r.c,v 1.32 2013/09/26 09:46:47 sasa Exp $*/
/*06.12.2018	22.11.2004	Белых А.И.	dvtmcf1_r.c
Расчёт движения товарно-материальных ценностей форма 1
*/
#include <math.h>
#include <errno.h>
#include "buhg_g.h"
#include "dvtmcf1.h"

class dvtmcf1_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class dvtmcf1_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  dvtmcf1_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   dvtmcf1_r_key_press(GtkWidget *widget,GdkEventKey *event,class dvtmcf1_r_data *data);
gint dvtmcf1_r1(class dvtmcf1_r_data *data);
void  dvtmcf1_r_v_knopka(GtkWidget *widget,class dvtmcf1_r_data *data);

void itskl(int skl,double kk1,double kk2,double kk3,double kk4,double ss1,double ss2,double ss3,double ss4,FILE *ffuz,FILE *ff,FILE *ffitog,class dvtmcf1_r_data *data);
void gsapp(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl,const char *nai,FILE *ffuz,FILE *ff);
void itsh(const char *sh,double k1,double k2,double k3,double k4,double s1,double s2,double s3,double s4,FILE *ffuz,FILE *ff,FILE *ffitog,class dvtmcf1_r_data *data);
void itkg(const char *sh,int skl,double k1,double k2,double k3,double k4,short met,FILE *ffuz,FILE *ff);
void shdtmcf1(short dn,short mn,short gn,short dk,short mk,short gk,int sli,FILE *ff,int skl,const char *nai,FILE *ffuz);
void itgrup(int kodgr,double kk1,double kk2,double kk3,double kk4,double ss1,double ss2,double ss3,double ss4,FILE *ffuz,short tr,FILE *ff);

extern SQL_baza bd;
extern double	okrg1;  /*Округление суммы*/
extern double	okrcn;  /*Округление цены*/
extern int      kol_strok_na_liste;

int dvtmcf1_r(class dvtmcf1_rr *datark,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
char strsql[512];
iceb_u_str repl;
class dvtmcf1_r_data data;

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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(dvtmcf1_r_key_press),&data);

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(dvtmcf1_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);
gtk_text_buffer_create_tag(data.buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)dvtmcf1_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  dvtmcf1_r_v_knopka(GtkWidget *widget,class dvtmcf1_r_data *data)
{
//printf("dvtmcf1_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   dvtmcf1_r_key_press(GtkWidget *widget,GdkEventKey *event,class dvtmcf1_r_data *data)
{
// printf("dvtmcf1_r_key_press\n");
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

gint dvtmcf1_r1(class dvtmcf1_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);





short ostkg=0;

iceb_poldan("Отчет в килограммах",strsql,"matnast.alx",data->window);
if(iceb_u_SRAV(strsql,"Вкл",1) == 0)
 ostkg=1;

sprintf(strsql,"select * from Kart");
SQLCURSOR cur,curtmp;
SQLCURSOR cur1;
SQL_str row,rowtmp;
SQL_str row1;

int kolstr;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  

class iceb_tmptab tabtmp;
const char *imatmptab={"dvtmcf1"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
sh char(24) not null,\
skl int not null,\
kgm int not null,\
naim char(112) not null,\
km int not null,\
nk int not null,\
ei char(24) not null,\
cena double(15,6) not null,\
nds float(2) not null,\
mnds smallint not null,\
fas float(2) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  

short dn,mn,gn;
short dk,mk,gk;

iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window);

sprintf(strsql,"%s\n%s %d.%d.%d %s %d.%d.%d\n",
gettext("Сортируем записи"),
gettext("Период с"),
dn,mn,gn,
gettext("по"),
dk,mk,gk);

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

int kgrm=0;
float kolstr1=0.;
class iceb_u_str naim("");

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[0],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),row[5],0,0) != 0)
   continue;

  kgrm=0;
  naim.new_plus("");
  /*Читаем код группы материалла*/
  sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    printf("%s %s !\n",gettext("Не найден код материалла"),row[2]);
    continue;
   }
  else
   {
    kgrm=atoi(row1[0]);
    naim.new_plus(row1[1]);
   }

  if(iceb_u_proverka(data->rk->grupa.ravno(),row1[0],0,0) != 0)
   continue;

  if(provndsw(data->rk->nds.ravno(),row) != 0)
    continue;

   if(iceb_u_proverka(data->rk->kodmat.ravno(),row[2],0,0) != 0)
         continue;

  sprintf(strsql,"insert into %s values ('%s',%s,%d,'%s',%s,%s,'%s',%.10g,%.5g,%s,%s)",
  imatmptab,
  row[5],row[0],kgrm,naim.ravno_filtr(),row[2],row[1],row[4],atof(row[6]),atof(row[9]),
  row[3],row[10]);
  iceb_sql_zapis(strsql,1,0,data->window);    

 }


if(data->rk->metka_sort == 0) //Сортировать счет-склад-наименование материалла.
 {
  sprintf(strsql,"select * from %s order by sh asc,skl asc,naim asc",imatmptab);
 }
if(data->rk->metka_sort == 1) //Сортировать счет-склад-группа-наименование материалла.
 {
  sprintf(strsql,"select * from %s order by sh asc,skl asc,kgm asc,naim asc",imatmptab);
 }
int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s.\n",gettext("Делаем отчёт"));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

FILE *ff;
char imaf[64];

sprintf(imaf,"zag%d.lst",getpid());
data->rk->imaf.plus(imaf);

repl.new_plus(gettext("Оборотная ведомость движния товарно-материальных ценностей"));
repl.plus(" (");
repl.plus(gettext("формат"));
repl.plus(" A3)");

data->rk->naimf.plus(repl.ravno());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_csv[64];
sprintf(imaf_csv,"dtmc%d.csv",getpid());
class iceb_fopen fil_csv;
if(fil_csv.start(imaf_csv,"w",data->window) != 0)
 return(1);

FILE *ffuz;
char imafuz[64];

sprintf(imafuz,"zaguz%d.lst",getpid());
data->rk->imaf.plus(imafuz);

repl.new_plus(gettext("Оборотная ведомость движния товарно-материальных ценностей"));
repl.plus(" (");
repl.plus(gettext("формат"));
repl.plus(" A4)");

data->rk->naimf.plus(repl.ravno());

if((ffuz = fopen(imafuz,"w")) == NULL)
 {
  iceb_er_op_fil(imafuz,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imafitog[64];
sprintf(imafitog,"zai%d.lst",getpid());

data->rk->imaf.plus(imafitog);
data->rk->naimf.plus(gettext("Свод движения материальных ценностей"));

FILE *ffitog;
if((ffitog = fopen(imafitog,"w")) == NULL)
 {
  iceb_er_op_fil(imafitog,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_zagolov(gettext("Свод движения материальных ценностей"),dn,mn,gn,dk,mk,gk,ffitog,data->window);


iceb_zagolov(gettext("Оборотная ведомость движния товарно-материальных ценностей"),dn,mn,gn,dk,mk,gk,ff,data->window);
iceb_zagolov(gettext("Оборотная ведомость движния товарно-материальных ценностей"),dn,mn,gn,dk,mk,gk,fil_csv.ff,data->window);
iceb_zagolov(gettext("Оборотная ведомость движния товарно-материальных ценностей"),dn,mn,gn,dk,mk,gk,ffuz,data->window);


int kst=4;

if(data->rk->shet.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());
  fprintf(fil_csv.ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());
  fprintf(ffuz,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());
  kst++;
 }
if(data->rk->sklad.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  fprintf(fil_csv.ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  fprintf(ffuz,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  kst++;
 }
if(data->rk->grupa.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Група"),data->rk->grupa.ravno());
  fprintf(fil_csv.ff,"%s:%s\n",gettext("Група"),data->rk->grupa.ravno());
  fprintf(ffuz,"%s:%s\n",gettext("Група"),data->rk->grupa.ravno());
  kst++;
 }

if(data->rk->kodmat.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код материалла"),data->rk->kodmat.ravno());
  fprintf(fil_csv.ff,"%s:%s\n",gettext("Код материалла"),data->rk->kodmat.ravno());
  fprintf(ffuz,"%s:%s\n",gettext("Код материалла"),data->rk->kodmat.ravno());
  kst++;
 }
if(data->rk->nds.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("НДС"),data->rk->nds.ravno());
  fprintf(fil_csv.ff,"%s:%s\n",gettext("НДС"),data->rk->nds.ravno());
  fprintf(ffuz,"%s:%s\n",gettext("НДС"),data->rk->nds.ravno());
  kst++;
 }
class iceb_u_str shbm("");
class iceb_u_str naiskl("");

int skl=0;
int sli=1;
double itg1=0.,itg2=0.,itg3=0.,itg4=0.,itg5=0.,itg6=0.,itg7=0.,itg8=0.;
double k1=0.,s1=0.,k2=0.,s2=0.,k3=0.,s3=0.,k4=0.,s4=0.;
double kk1=0.,ss1=0.,kk2=0.,ss2=0.,kk3=0.,ss3=0.,kk4=0.,ss4=0.;
double kg0=0.,kg1=0.,kg2=0.,kg3=0.;
double kgs0=0.,kgs1=0.,kgs2=0.,kgs3=0.;
int kgrmzp=0;
char str[512];
memset(str,'\0',sizeof(str));
memset(str,'.',233);
int mvsh=0;
kolstr1=0;
class ostatok ost;
class iceb_u_str shb("");
int skl1;
int kodm;
int nk;
class iceb_u_str ei("");
char		mnds='\0';
double cena,nds;
class iceb_u_str naimshet("");
char nomn[112];

while(curtmp.read_cursor(&rowtmp) != 0)
 {
  iceb_pbar(data->bar,kolstrtmp,++kolstr1);    
 
  shb.new_plus(rowtmp[0]);
  skl1=atoi(rowtmp[1]);
  kgrm=atoi(rowtmp[2]);
  naim.new_plus(rowtmp[3]);
  kodm=atoi(rowtmp[4]);
  nk=atoi(rowtmp[5]);

  /*Остаток по карточкам*/
  ostkarw(dn,mn,gn,dk,mk,gk,skl1,nk,&ost);

  if(ost.ostm[0] == 0. && ost.ostm[1] == 0. && ost.ostm[3] == 0. && fabs(ost.ostmc[3]) < 0.009)
     continue;

  ei.new_plus(rowtmp[6]);
  cena=atof(rowtmp[7]);
  nds=atof(rowtmp[8]);
  strncpy(strsql,rowtmp[9],sizeof(strsql)-1);

  if(atoi(strsql) == 0)
   mnds=' ';
  if(atoi(strsql) == 1)
   mnds='+';
      
  if(kgrmzp != kgrm)
   {
    if(kgrmzp != 0)
     {
      if(data->rk->metka_sort == 1)
      if(itg1 != 0. || itg2 != 0. || itg3 != 0. || itg4 != 0.)
       {
        itgrup(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ffuz,0,ff);
        kst+=2;
       }
     }
    itg1=itg2=itg3=itg4=itg5=itg6=itg7=itg8=0.;
    kgrmzp=kgrm;
   }

  if(iceb_u_SRAV(shb.ravno(),shbm.ravno(),1) != 0)
   {
    if(shbm.ravno()[0] != '\0')
     {
        if(data->rk->metka_sort == 1)
        if(itg1 != 0. || itg2 != 0. || itg3 != 0. || itg4 != 0.)
         {
          itgrup(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ffuz,0,ff);
          itg1=itg2=itg3=itg4=itg5=itg6=itg7=itg8=0.;
          kst+=2;
         }
	itskl(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ffuz,ff,ffitog,data);
        kst+=2;
        if(ostkg == 1 && fabs((kk1+kk2+kk3+kk4) - (kg0+kg1+kg2+kg3)) > 0.0001 &&
        kg0+kg1+kg2+kg3 > 0.0001)
         { 
          itkg(shbm.ravno(),skl,kg0,kg1,kg2,kg3,1,ffuz,ff);
          kst+=2;
          kg0=kg1=kg2=kg3=0.;
         }
	itsh(shbm.ravno(),k1,k2,k3,k4,s1,s2,s3,s4,ffuz,ff,ffitog,data);
        kst+=2;
        if(ostkg == 1 && fabs((k1+k2+k3+k4) - (kgs0+kgs1+kgs2+kgs3)) > 0.0001 &&
         kgs0+kgs1+kgs2+kgs3 > 0.0001)
         { 
          itkg(shbm.ravno(),skl,kgs0,kgs1,kgs2,kgs3,0,ffuz,ff);
          kst+=2;
          kgs0=kgs1=kgs2=kgs3=0.;
         }
	skl=0;
     }
    k1=s1=k2=s2=k3=s3=k4=s4=0.;

    /*Читаем наименование счета*/
    naimshet.new_plus("");
    sprintf(strsql,"select nais from Plansh where ns='%s'",shb.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
     {
      printf("Не найден счёт %s в плане счетов !\n",shb.ravno());
      fprintf(ff,"%s %s !\n",gettext("Не найден счет"),shb.ravno());
     }
    else
      naimshet.new_plus(row[0]);       

    sprintf(strsql,"\n%s %s \"%s\"\n",gettext("Счёт"),shb.ravno(),naimshet.ravno());
    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

    if(kst > kol_strok_na_liste - 10)
     {
      fprintf(ff,"\f");
      fprintf(ffuz,"\f");
      kst=0;
      sli++;
     }


    fprintf(ff,"\n%s %s \"%s\"\n",gettext("Счёт"),shb.ravno(),naimshet.ravno());
    fprintf(ffuz,"\n%s %s \"%s\"\n",gettext("Счёт"),shb.ravno(),naimshet.ravno());
    if(ffitog != NULL)
      fprintf(ffitog,"\n%s %s \"%s\"\n",gettext("Счёт"),shb.ravno(),naimshet.ravno());
    kst+=2; 
    gsapp(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ffuz,ff);

    mvsh=1;

    shbm.new_plus(shb.ravno());
   }

  if(skl != skl1)
   {
    if(skl != 0)
     {
        if(data->rk->metka_sort == 1)
        if(itg1 != 0. || itg2 != 0. || itg3 != 0. || itg4 != 0.)
         {
          itgrup(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ffuz,0,ff);
          itg1=itg2=itg3=itg4=itg5=itg6=itg7=itg8=0.;
          kst+=2;
         }

	itskl(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ffuz,ff,ffitog,data);
        kst+=2;
        if(ostkg == 1 && fabs((kk1+kk2+kk3+kk4) - (kg0+kg1+kg2+kg3)) > 0.0001 &&
        kg0+kg1+kg2+kg3 > 0.0001)
         { 
          itkg(shbm.ravno(),skl,kg0,kg1,kg2,kg3,1,ffuz,ff);
          kst+=2;
          kg0=kg1=kg2=kg3=0.;
         }

     }
     kk1=ss1=kk2=ss2=kk3=ss3=kk4=ss4=0.;

    /*Читаем наименование склада*/
    naiskl.new_plus("");
    sprintf(strsql,"select naik from Sklad where kod='%d'",skl1);
    if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
     {
      printf("Не найден склад %d в списке складов\n",skl1);
      fprintf(ff,"%s %d !\n",gettext("Не найден склад"),skl1);
     }
    else
      naiskl.new_plus(row[0]);

    sprintf(strsql,"%s: %d %s\n",gettext("Склад"),skl1,naiskl.ravno());
    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

    if(kst != 0)
    if(kst > kol_strok_na_liste - 10)
     {
      fprintf(ff,"\f");
      fprintf(ffuz,"\f");
      kst=0;
      sli++;
     }
    mvsh=1;
    skl=skl1;

   }
  if(mvsh == 1)
   {
    shdtmcf1(dn,mn,gn,dk,mk,gk,sli,ff,skl,naiskl.ravno(),ffuz);
    kst+=6; 
    mvsh=0;
   }

  
  /*Остаток по карточкам*/
  sprintf(nomn,"%s.%d.%d",shb.ravno(),kgrm,nk);


    gsapp(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ffuz,ff);

    fprintf(ff,"\
%4d %-*.*s %-*s %-*s %c%2.2g %14.10g %15.10g %15.2f %15.10g %15.2f %15.10g %15.2f \
%15.10g %15.2f\n",
    kodm,
    iceb_u_kolbait(40,naim.ravno()),iceb_u_kolbait(40,naim.ravno()),naim.ravno(),
    iceb_u_kolbait(13,nomn),nomn,
    iceb_u_kolbait(10,ei.ravno()),ei.ravno(),
    mnds,nds,cena,
    ost.ostm[0],ost.ostmc[0],ost.ostm[1],ost.ostmc[1],ost.ostm[2],ost.ostmc[2],ost.ostm[3],ost.ostmc[3]);

    fprintf(fil_csv.ff,"\
%d|%s|%s|%s|%c%.2g|%.10g|%.10g|%.2f|%.10g|%.2f|%.10g|%.2f|\
%.10g|%.2f\n",kodm,
    naim.ravno(),
    nomn,
    ei.ravno(),
    mnds,nds,
    cena,
    ost.ostm[0],ost.ostmc[0],ost.ostm[1],ost.ostmc[1],ost.ostm[2],ost.ostmc[2],ost.ostm[3],ost.ostmc[3]);

    fprintf(ffuz,"\
%4d %-*.*s %-*s %-*s %c%2.2g %7s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",
    kodm,
    iceb_u_kolbait(23,naim.ravno()),iceb_u_kolbait(23,naim.ravno()),naim.ravno(),
    iceb_u_kolbait(13,nomn),nomn,
    iceb_u_kolbait(3,ei.ravno()),ei.ravno(),
    mnds,nds,iceb_prcn(cena),
    ost.ostm[0],ost.ostmc[0],ost.ostm[1],ost.ostmc[1],ost.ostm[2],ost.ostmc[2],ost.ostm[3],ost.ostmc[3]);


     if(ostkg == 1)
      {
       double ost1=0.,ost2=0.,ost3=0.,ost4=0.;
       double fas=0.;
       fas=atof(rowtmp[10]);
       if(fas > 0.0000001)
        { 
         ost1=ost.ostm[0]*fas;
         ost2=ost.ostm[1]*fas;
         ost3=ost.ostm[2]*fas;
         ost4=ost.ostm[3]*fas;
         kg0+=ost1;
         kg1+=ost2;
         kg2+=ost3;
         kg3+=ost4;
         kgs0+=ost1;
         kgs1+=ost2;
         kgs2+=ost3;
         kgs3+=ost4;

         gsapp(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),ffuz,ff);

         fprintf(ff,"\
%4s %-40.40s %-13s %-10s %3s %14s %15.10g %15s %15.10g %15s %15.10g %15s \
%15.10g %15s\n"," "," "," "," "," "," ",
ost1," ",ost2," ",ost3," ",ost4," ");
         fprintf(fil_csv.ff,"\
%s|%s|%s|%s|%s|%s|%.10g|%s|%.10g|%s|%.10g|%s|\
%.10g|%s\n"," "," "," "," "," "," ",ost1," ",ost2," ",ost3," ",ost4," ");

         fprintf(ffuz,"\
%4s %-23.23s %-13s %-3s %3s %7s %8.8g %8s %8.8g %8s %8.8g %8s \
%8.8g %8s\n"," "," "," "," "," "," ",
ost1," ",ost2," ",ost3," ",ost4," ");

        } 
       else
         if(iceb_u_SRAV(ei.ravno(),"Кг",0) == 0 || iceb_u_SRAV(ei.ravno(),"кг",0) == 0 ||
          iceb_u_SRAV(ei.ravno(),"КГ",0) == 0)
          {
           kg0+=ost.ostm[0];
           kg1+=ost.ostm[1];
           kg2+=ost.ostm[2];
           kg3+=ost.ostm[3];
           kgs0+=ost.ostm[0];
           kgs1+=ost.ostm[1];
           kgs2+=ost.ostm[2];
           kgs3+=ost.ostm[3];
          }             
      }     

  k1+=ost.ostm[0];   kk1+=ost.ostm[0]; itg1+=ost.ostm[0];
  s1+=ost.ostmc[0];  ss1+=ost.ostmc[0]; itg5+=ost.ostmc[0];

  k2+=ost.ostm[1];   kk2+=ost.ostm[1];  itg2+=ost.ostm[1];
  s2+=ost.ostmc[1];  ss2+=ost.ostmc[1]; itg6+=ost.ostmc[1];

  k3+=ost.ostm[2];   kk3+=ost.ostm[2];    itg3+=ost.ostm[2];
  s3+=ost.ostmc[2];  ss3+=ost.ostmc[2];   itg7+=ost.ostmc[2];

  k4+=ost.ostm[3];   kk4+=ost.ostm[3];    itg4+=ost.ostm[3];
  s4+=ost.ostmc[3];  ss4+=ost.ostmc[3];   itg8+=ost.ostmc[3];
 }

if(data->rk->metka_sort == 1)
    itgrup(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,ffuz,0,ff);

itskl(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,ffuz,ff,ffitog,data);

if(ostkg == 1 && fabs((kk1+kk2+kk3+kk4) - (kg0+kg1+kg2+kg3)) > 0.0001 &&  kg0+kg1+kg2+kg3 > 0.0001)
  itkg(shbm.ravno(),skl,kg0,kg1,kg2,kg3,1,ffuz,ff);

itsh(shbm.ravno(),k1,k2,k3,k4,s1,s2,s3,s4,ffuz,ff,ffitog,data);

if(ostkg == 1 && fabs((k1+k2+k3+k4) - (kgs0+kgs1+kgs2+kgs3)) > 0.0001 &&  kgs0+kgs1+kgs2+kgs3 > 0.0001)
   itkg(shbm.ravno(),skl,kgs0,kgs1,kgs2,kgs3,0,ffuz,ff);

iceb_podpis(ffitog,data->window);
fclose(ffitog);

iceb_podpis(fil_csv.ff,data->window);
fil_csv.end();


iceb_podpis(ff,data->window);
fclose(ff);

iceb_podpis(ffuz,data->window);
fclose(ffuz);

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);

data->rk->imaf.plus(imaf_csv);
data->rk->naimf.plus(gettext("Оборотная ведомость для загрузки в электронные таблицы"));


data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/*****************/
/*Итого по группе*/
/*****************/
void itgrup(int kodgr,double kk1,double kk2,double kk3,
double kk4,double ss1,double ss2,double ss3,double ss4,
FILE *ffuz,  //узкая распечтака
short tr,
FILE *ff)
{
char		bros[512];
class iceb_u_str naigr("");
SQL_str         row;
SQLCURSOR       cur;
if(tr == 1)
  return;

/*Читаем наименование группы*/
sprintf(bros,"select naik from Grup where kod=%d",kodgr);
if(sql_readkey(&bd,bros,&row,&cur) != 1)
 {
  printf("%s %d !\n","Не нашли группу",kodgr);
 }
else
 naigr.new_plus(row[0]);

sprintf(bros,"%s %d %s:",gettext("Итого по группе"),kodgr,naigr.ravno());


fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%*s %14.10g %15.2f %15.10g %15.2f %15.10g %15.2f \
%15.10g %15.2f\n",
iceb_u_kolbait(90,bros),bros,
kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4);

fprintf(ffuz,"\
----------------------------------------------------------------------------------------------------------------------------------\n\
%*s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",iceb_u_kolbait(58,bros),bros,
kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4);
}
/*******/
/*Шапка*/
/*******/
void shdtmcf1(short dn,short mn,short gn,short dk,short mk,
short gk,int sli,FILE *ff,int skl,const char *nai,
FILE *ffuz) //узкая распечтака*/
{
char		bros[512];

  sprintf(bros,"%s: %d %s",gettext("Склад"),skl,nai);
  fprintf(ff,"%-*s %s N%d\n",iceb_u_kolbait(196,bros),bros,gettext("Лист"),sli);
  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff,gettext("    |             Наименование               |  Номенкла-  | Единица  |   |              | Остаток на  %02d.%02d.%4dг.      |     Приход   за период        |    Расход  за период          |  Остаток на %02d.%02d.%4dг.      |\n"),
  dn,mn,gn,dk,mk,gk);
  fprintf(ff,gettext("Код |        товарно-материальных            |турний номер | измерения|НДС|   Ц е н а    |-------------------------------|------------------------------------------------------------------------------------------------\n"));
  fprintf(ff,gettext("    |             ценностей                  |             |          |   |              |   Количество  |  Сумма        |    Количество |     Сумма     |    Количество |     Сумма     |   Количество  |    Сумма      |\n"));


  fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  sprintf(bros,"%s: %d %s %s %02d.%02d.%04d%s => %02d.%02d.%04d%s",
  gettext("Склад"),skl,nai,
  gettext("Период с"),
  dn,mn,gn,
  gettext("г."),
  dk,mk,gk,
  gettext("г."));
  
  fprintf(ffuz,"%-*s %s N%d\n",iceb_u_kolbait(120,bros),bros,gettext("Лист"),sli);

  fprintf(ffuz,"\
-----------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ffuz,gettext("\
    |     Наименование      |  Номенкла-  |Ед.|   |       |     Остаток     |      Приход     |      Расход     |     Остаток     |\n\
Код |товарно-материальных   |турный номер |изм|НДС|Ц е н а|------------------------------------------------------------------------\n\
    |     ценностей         |             |   |   |       |Колич.  |  Сумма |Колич.  | Сумма  |Колич.  | Сумма  |Колич.  | Сумма  |\n"));

  fprintf(ffuz,"\
-----------------------------------------------------------------------------------------------------------------------------------\n");
}
/****************/
/*Итого в килограммах по счёту и по складу*/
/****************/
void itkg(const char *sh,int skl,double k1,double k2,double k3,
double k4,
short met, //0-по счету 1-по складу*/
FILE *ffuz,  //узкая распечатка
FILE *ff)
{
char		bros[512];
if(met == 1)
  sprintf(bros,"%s %d %s",gettext("Итого по складу"),
  skl,
  gettext("Кг."));

if(met == 0)
  sprintf(bros,"%s %s %s",
  gettext("Итого по счёту"),
  sh,
  gettext("Кг."));

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%4s %-*.*s %-13s %-10s %4s %14s %15.10g %15s %15.10g %15s %15.10g %15s \
%15.10g %15s\n"," ",iceb_u_kolbait(40,bros),iceb_u_kolbait(40,bros),bros," "," "," "," ",
k1," ",k2," ",k3," ",k4," ");

fprintf(ffuz,"\
----------------------------------------------------------------------------------------------------------------------------------\n\
%*s %-13s %-3s %3s %7s %8.8g %8s %8.8g %8s %8.8g %8s \
%8.8g %8s\n",iceb_u_kolbait(58,bros),bros," "," "," "," ",
k1," ",k2," ",k3," ",k4," ");

}
/****************/
/*Итого по счёту*/
/****************/
void itsh(const char *sh,double k1,double k2,double k3,double k4,
double s1,double s2,double s3,double s4,
FILE *ffuz, //узкая распечтака
FILE *ff,FILE *ffitog,
class dvtmcf1_r_data *data)
{
char		bros[1024];

sprintf(bros,"%s %s",gettext("Итого по счёту"),sh);


fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%4s %-*.*s %-13s %-10s %14s %-4s %14.10g %15.2f %15.10g %15.2f %15.10g %15.2f \
%15.10g %15.2f\n"," ",iceb_u_kolbait(40,bros),iceb_u_kolbait(40,bros),bros," "," "," "," ",
k1,s1,k2,s2,k3,s3,k4,s4);

fprintf(ffuz,"\
----------------------------------------------------------------------------------------------------------------------------------\n\
%*s %-13s %-3s %3s %7s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",iceb_u_kolbait(28,bros),bros," "," "," "," ",
k1,s1,k2,s2,k3,s3,k4,s4);


sprintf(bros,"%s %s:\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\n",
gettext("Итого по счёту"),
sh,
iceb_u_kolbait(10,gettext("Остаток")),
gettext("Остаток"),
k1,s1,
iceb_u_kolbait(10,gettext("Приход")),
gettext("Приход"),
k2,s2,
iceb_u_kolbait(10,gettext("Расход")),
gettext("Расход"),
k3,s3,
iceb_u_kolbait(10,gettext("Остаток")),
gettext("Остаток"),
k4,s4);

iceb_printw(bros,data->buffer,data->view,"naim_shrift");

if(ffitog != NULL)
  fprintf(ffitog,"%s %s:\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\n",
gettext("Итого по счёту"),
sh,
iceb_u_kolbait(10,gettext("Остаток")),gettext("Остаток"),
k1,s1,
iceb_u_kolbait(10,gettext("Приход")),gettext("Приход"),
k2,s2,
iceb_u_kolbait(10,gettext("Расход")),gettext("Расход"),
k3,s3,
iceb_u_kolbait(10,gettext("Остаток")),gettext("Остаток"),
k4,s4);


}
/*******/
/*Счётчик*/
/*******/
void gsapp(short dn,short mn,short gn,short dk,short mk,
short gk,
int *sl, //Счетчик листов
int *kst, //Счетчик строк
int skl,const char *nai,
FILE *ffuz, //узкая распечтака
FILE *ff)
{

*kst+=1;
/*
printw("gsapp kst=%d sl=%d\n",*kst,*sl);
refresh();
*/
if(*kst >= kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  fprintf(ffuz,"\f");

  *sl+=1;
  shdtmcf1(dn,mn,gn,dk,mk,gk,*sl,ff,skl,nai,ffuz);
  *kst=6;  
 }

}

/*****************/
/*Итого по складу*/
/*****************/
void            itskl(int skl,double kk1,double kk2,double kk3,
double kk4,double ss1,double ss2,double ss3,double ss4,
FILE *ffuz, //узкая распечтака
FILE *ff,FILE *ffitog,
class dvtmcf1_r_data *data)
{
char		bros[1024];

sprintf(bros,"%s %d",gettext("Итого по складу"),skl);


fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%4s %-*.*s %-13s %-10s %14s %-4s %14.10g %15.2f %15.10g %15.2f %15.10g %15.2f \
%15.10g %15.2f\n"," ",iceb_u_kolbait(40,bros),iceb_u_kolbait(40,bros),bros," "," "," "," ",
kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4);

fprintf(ffuz,"\
----------------------------------------------------------------------------------------------------------------------------------\n\
%*s %-13s %-3s %3s %7s %8.8g %8.2f %8.8g %8.2f %8.8g %8.2f \
%8.8g %8.2f\n",iceb_u_kolbait(28,bros),bros," "," "," "," ",
kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4);

sprintf(bros,"%s %d:\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\n",
gettext("Итого по складу"),
skl,
iceb_u_kolbait(10,gettext("Остаток")),gettext("Остаток"),
kk1,ss1,
iceb_u_kolbait(10,gettext("Приход")),gettext("Приход"),
kk2,ss2,
iceb_u_kolbait(10,gettext("Расход")),gettext("Расход"),
kk3,ss3,
iceb_u_kolbait(10,gettext("Остаток")),gettext("Остаток"),
kk4,ss4);

iceb_printw(bros,data->buffer,data->view,"naim_shrift");

if(ffitog != NULL)
  fprintf(ffitog,"%s %d:\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\
%-*s %15.10g %15.2f\n\n",
gettext("Итого по складу"),
skl,
iceb_u_kolbait(10,gettext("Остаток")),gettext("Остаток"),
kk1,ss1,
iceb_u_kolbait(10,gettext("Приход")),gettext("Приход"),
kk2,ss2,
iceb_u_kolbait(10,gettext("Расход")),gettext("Расход"),
kk3,ss3,
iceb_u_kolbait(10,gettext("Остаток")),gettext("Остаток"),
kk4,ss4);

}
