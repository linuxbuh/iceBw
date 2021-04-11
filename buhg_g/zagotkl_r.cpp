/*$Id: zagotkl_r.c,v 1.22 2013/09/26 09:46:59 sasa Exp $*/
/*23.05.2016	22.11.2004	Белых А.И.	zagotkl_r.c
Заготовка для отчёта
*/
#include <errno.h>
#include "buhg_g.h"
#include "zagotkl.h"

class zagotkl_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class zagotkl_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zagotkl_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zagotkl_r_key_press(GtkWidget *widget,GdkEventKey *event,class zagotkl_r_data *data);
gint zagotkl_r1(class zagotkl_r_data *data);
void  zagotkl_r_v_knopka(GtkWidget *widget,class zagotkl_r_data *data);

void itskl1(int skl,double kk1,double kk2,double kk3,double kk4,double ss1,double ss2,double ss3,double ss4,class iceb_u_str *str,FILE *ff,FILE *ffkl2);
void itgrup_kl(int kodgr,double kk1,double kk2,double kk3,double kk4,double ss1,double ss2,double ss3,double ss4,short tr,FILE *ff);
void shdtmcf1k(short dn,short mn,short gn,int sli,FILE *ff,int skl,const char *nai,FILE *ffkl2);
void itsh1(const char *sh,double k1,double k2,double k3,double k4,double s1,double s2,double s3,double s4,class iceb_u_str *str,FILE *ff,FILE *ffkl2);
void gsapp2(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,int skl,const char *nai,class iceb_u_str *str,FILE *ff,FILE *ffkl2);

extern SQL_baza bd;


extern double	okrg1;  /*Округление суммы*/
extern double	okrcn;  /*Округление цены*/
extern int      kol_strok_na_liste;

int zagotkl_r(class zagotkl_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zagotkl_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать заготовку для отчёта"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zagotkl_r_key_press),&data);

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

repl.plus(gettext("Распечатать заготовку для отчёта"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zagotkl_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zagotkl_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zagotkl_r_v_knopka(GtkWidget *widget,class zagotkl_r_data *data)
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

gboolean   zagotkl_r_key_press(GtkWidget *widget,GdkEventKey *event,class zagotkl_r_data *data)
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

gint zagotkl_r1(class zagotkl_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);



sprintf(strsql,"select * from Kart");
SQLCURSOR cur;
SQLCURSOR cur1;
SQLCURSOR curtmp;
SQL_str row,rowtmp;
SQL_str row1;

int kolstr;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  

class iceb_tmptab tabtmp;
const char *imatmptab={"dvtmcf1"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shu char(24) not null,\
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


short dk=0,mk=0,gk=0;

iceb_u_rsdat(&dk,&mk,&gk,data->rk->datao.ravno(),1);

short dn=1,mn=1,gn=gk;

sprintf(strsql,"%s\n%s %d.%d.%d %s %d.%d.%d\n",
gettext("Сортируем записи"),
gettext("Период с"),
dn,mn,gn,
gettext("по"),
dk,mk,gk);

iceb_printw(strsql,data->buffer,data->view);

int kgrm=0;
float kolstr1=0.;
class iceb_u_str naim;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[0],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),row[5],0,0) != 0)
   continue;

  kgrm=0;
  naim.new_plus("");
  /*Читаем код группы материала*/
  sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    printf("%s %s !\n",gettext("Не найден код материала"),row[2]);
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


if(data->rk->metka_sort == 0) //Сортировать счет-склад-наименование материала.
  sprintf(strsql,"select * from %s order by shu asc,skl asc,naim asc",imatmptab);

if(data->rk->metka_sort == 1) //Сортировать счет-склад-группа-наименование материала.
  sprintf(strsql,"select * from %s order by shu asc,skl asc,kgm asc,naim asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s.\n",gettext("Делаем отчёт"));
iceb_printw(strsql,data->buffer,data->view);

FILE *ff;
char imaf[64];

sprintf(imaf,"zag%d.lst",getpid());
data->rk->imaf.plus(imaf);

data->rk->naimf.plus(gettext("Пустографка для отчёта материально-ответственного лица"));

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imafkl2[64];

sprintf(imafkl2,"zagg%d.lst",getpid());
data->rk->imaf.plus(imafkl2);

data->rk->naimf.plus(gettext("Пустографка для отчёта материально-ответственного лица с приходами"));
FILE *ffkl2;
if((ffkl2 = fopen(imafkl2,"w")) == NULL)
 {
  iceb_er_op_fil(imafkl2,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_u_zagolov(gettext("Оборотная ведомость движенияалов товарно-материальных ценностей"),dn,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ff);

SQL_str row_alx;
class SQLCURSOR cur_alx;
const char *imaf_alx={"matzag.alx"};
char zagol[4][512];
memset(zagol,'\0',sizeof(zagol));

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
 }

if(kolstr > 0)
 {

  int i1=0;
  while(cur_alx.read_cursor(&row_alx) != 0)
   {
    if(row_alx[0][0] == '#')
     continue;
    strncpy(zagol[i1++],row_alx[0],sizeof(zagol[i1++])-1);
    zagol[i1-1][strlen(zagol[i1-1])-1]='\0';
    if(i1 == 4)
       break;
   }  
 }

  struct tm *bf;
  bf=localtime(&vremn);
  fprintf(ffkl2,"\x0F");

  fprintf(ffkl2,"%-*s %s\n%135s%s\n\
  				%s %s %d.%d.%d%s %s %d.%d.%d%s %10s%s\n\
  					       %s %d.%d.%d%s  %s - %d:%d%54s%s\n",
                   iceb_u_kolbait(133,iceb_get_pnk("00",0,data->window)), iceb_get_pnk("00",0,data->window),
  zagol[0],
  " ",
  zagol[1],
  gettext("Оборотная ведомость движения товарно-материальных ценностей"),
  gettext("За период с"),
  dn,mn,gn,
  gettext("г."),
  gettext("по"),
  dk,mk,gk,
  gettext("г."),
  " ",
  zagol[2],
  gettext("По состоянию на"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min," ",zagol[3]);

int kst=4;

if(data->rk->shet.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());
  fprintf(ffkl2,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());
  kst++;
 }
if(data->rk->sklad.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  fprintf(ffkl2,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
  kst++;
 }
if(data->rk->grupa.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Группа"),data->rk->grupa.ravno());
  fprintf(ffkl2,"%s:%s\n",gettext("Группа"),data->rk->grupa.ravno());
  kst++;
 }

if(data->rk->kodmat.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код материала"),data->rk->kodmat.ravno());
  fprintf(ffkl2,"%s:%s\n",gettext("Код материала"),data->rk->kodmat.ravno());
  kst++;
 }
if(data->rk->nds.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("НДС"),data->rk->nds.ravno());
  fprintf(ffkl2,"%s:%s\n",gettext("НДС"),data->rk->nds.ravno());
  kst++;
 }

class iceb_u_str shbm("");
class iceb_u_str naiskl("");

int skl=0;
int sli=1;
double itg1=0.,itg2=0.,itg3=0.,itg4=0.,itg5=0.,itg6=0.,itg7=0.,itg8=0.;
double k1=0.,s1=0.,k2=0.,s2=0.,k3=0.,s3=0.,k4=0.,s4=0.;
double kk1=0.,ss1=0.,kk2=0.,ss2=0.,kk3=0.,ss3=0.,kk4=0.,ss4=0.;
int kgrmzp=0;
class iceb_u_str str("");
for(int nom=0; nom < 240; nom++)
 str.plus(".");
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
char nomn[128];
double ost1=0.,ost2=0.;
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

  
  if(ost.ostm[0] == 0. && ost.ostm[1] == 0.)
    continue;
    
  ei.new_plus(rowtmp[6]);
  cena=atof(rowtmp[7]);
  nds=atof(rowtmp[8]);

  if(atoi(rowtmp[9]) == 0)
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
        itgrup_kl(kgrmzp,itg1,itg2,itg3,itg4,itg5,itg6,itg7,itg8,0,ff);
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
	itskl1(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,&str,ff,ffkl2);
	skl=0;
	itsh1(shbm.ravno(),k1,k2,k3,k4,s1,s2,s3,s4,&str,ff,ffkl2);
	kst+=4;
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
    iceb_printw(strsql,data->buffer,data->view);

    if(kst > kol_strok_na_liste - 10)
     {
      fprintf(ff,"\f");
      kst=0;
      sli++;
     }


    fprintf(ff,"\n%s %s \"%s\"\n",gettext("Счёт"),shb.ravno(),naimshet.ravno());
    kst+=2; 
    gsapp2(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),&str,ff,ffkl2);

    mvsh=1;

    shbm.new_plus(shb.ravno());
   }

  if(skl != skl1)
   {
    if(skl != 0)
     {
      itskl1(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,&str,ff,ffkl2);
      kst+=2;
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
    iceb_printw(strsql,data->buffer,data->view);

    if(kst != 0)
    if(kst > kol_strok_na_liste - 10)
     {
      fprintf(ff,"\f");
      kst=0;
      sli++;
     }
    mvsh=1;
    skl=skl1;

   }
  if(mvsh == 1)
   {
      shdtmcf1k(dn,mn,gn,sli,ff,skl,naiskl.ravno(),ffkl2);
      kst+=6;
    mvsh=0;
   }

  
  sprintf(nomn,"%s.%d.%d",shb.ravno(),kgrm,nk);

  
    ost1=ost.ostm[0];
    ost2=ost.ostmc[0];
    char ost3[40],ost4[40];
    memset(ost3,'\0',sizeof(ost3));
    memset(ost4,'\0',sizeof(ost4));
    if(ost.ostm[1] != 0.)
       sprintf(ost3,"%10.10g",ost.ostm[1]);
    if(ost.ostmc[1] != 0.)
       sprintf(ost4,"%12.2f",ost.ostmc[1]);

    //Остаток по документам
     
    ostdokw(dn,mn,gn,dk,mk,gk,skl1,nk,&ost);

    fprintf(ff,"\
%4d %-*.*s %-*s %-*s %c%2.2g %7.7g %7.7g %15.2f|               |               |               |               |               |               |               |             |\n%s\n",
    kodm,
    iceb_u_kolbait(40,naim.ravno()),iceb_u_kolbait(40,naim.ravno()),naim.ravno(),
    iceb_u_kolbait(20,nomn),nomn,
    iceb_u_kolbait(10,ei.ravno()),ei.ravno(),
    mnds,
    nds,ost1,ost.ostm[0],ost2,str.ravno());

    gsapp2(dn,mn,gn,dk,mk,gk,&sli,&kst,skl1,naiskl.ravno(),&str,ff,ffkl2);

    fprintf(ffkl2,"\
%4d %-*.*s %-*s %-*s %c%2.2g %10.10g %10.10g %12.2f %10s %12s|       |       |       |       |       |       |       |       |       |       |         |        |\n%s\n",
    kodm,
    iceb_u_kolbait(40,naim.ravno()),iceb_u_kolbait(40,naim.ravno()),naim.ravno(),
    iceb_u_kolbait(20,nomn),nomn,
    iceb_u_kolbait(10,ei.ravno()),ei.ravno(),
    mnds,nds,cena,ost1,ost2,ost3,ost4,str.ravno());
     
  k1+=ost.ostm[0];   kk1+=ost.ostm[0]; itg1+=ost.ostm[0];
  s1+=ost.ostmc[0];  ss1+=ost.ostmc[0]; itg5+=ost.ostmc[0];

  k2+=ost.ostm[1];   kk2+=ost.ostm[1];  itg2+=ost.ostm[1];
  s2+=ost.ostmc[1];  ss2+=ost.ostmc[1]; itg6+=ost.ostmc[1];

  k3+=ost.ostm[2];   kk3+=ost.ostm[2];    itg3+=ost.ostm[2];
  s3+=ost.ostmc[2];  ss3+=ost.ostmc[2];   itg7+=ost.ostmc[2];

  k4+=ost.ostm[3];   kk4+=ost.ostm[3];    itg4+=ost.ostm[3];
  s4+=ost.ostmc[3];  ss4+=ost.ostmc[3];   itg8+=ost.ostmc[3];
 }


itskl1(skl,kk1,kk2,kk3,kk4,ss1,ss2,ss3,ss4,&str,ff,ffkl2);
itsh1(shbm.ravno(),k1,k2,k3,k4,s1,s2,s3,s4,&str,ff,ffkl2);


iceb_podpis(ffkl2,data->window);

fclose(ffkl2);



iceb_podpis(ff,data->window);
fclose(ff);

 

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),0,data->window);

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
void            itgrup_kl(int kodgr,double kk1,double kk2,double kk3,
double kk4,double ss1,double ss2,double ss3,double ss4,
short tr,
FILE *ff)
{
char		bros[1024];
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
  naigr.new_plus("");
 }
else
 naigr.new_plus(row[0]);

sprintf(bros,"%s %d %s:",gettext("Итого по группе"),kodgr,naigr.ravno());


fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%*s %14.10g %15.2f %15.10g %15.2f %15.10g %15.2f \
%15.10g %15.2f\n",iceb_u_kolbait(90,bros),bros,
kk1,ss1,kk2,ss2,kk3,ss3,kk4,ss4);

}
/*******/
/*Шапка для кладовщика*/
/*******/
void shdtmcf1k(short dn,short mn,short gn,int sli,FILE *ff,
int skl,const char *nai,FILE *ffkl2)
{
char		str[1024];
char		bros[512];

memset(str,'\0',sizeof(str));

memset(str,'-',240);
sprintf(bros,"%s: %d %s",gettext("Склад"),skl,nai);

fprintf(ff,"%-*s %s N%d\n%s\n",iceb_u_kolbait(196,bros),bros,gettext("Лист"),sli,str);

fprintf(ff,gettext("    |             Наименование               |  Номенклатурный    | Единица  |   | Остаток на  %02d.%02d.%4dг.      |               |               |               |               |               |               |               |             |\n"),
dn,mn,gn),
fprintf(ff,gettext("Код |        товарно-материальных            |      номер         | измерения|НДС|-------------------------------|               |               |               |               |               |               |               |             |\n"));
fprintf(ff,gettext("    |             ценностей                  |                    |          |   |   Количество  |    Сумма      |               |               |               |               |               |               |               |             |\n"));
fprintf(ff,"%s\n",str);

memset(str,'\0',sizeof(str));
memset(str,'-',240);

fprintf(ffkl2,"%-*s %s N%d\n%s\n",iceb_u_kolbait(196,bros),bros,gettext("Лист"),sli,str);

fprintf(ffkl2,gettext("\
    |             Наименование               |  Номенклатурный    | Единица  |   |          |Остаток на %02d.%02d.%4dг.|       Приход          |               |               |               |               |               |                  |\n"),
dn,mn,gn),
fprintf(ffkl2,gettext("\
Код |        товарно-материальных            |       номер        | измерения|НДС|   Цена   |-----------------------|-----------------------|---------------|---------------|---------------|---------------|---------------|------------------|\n"));
fprintf(ffkl2,gettext("\
    |             ценностей                  |                    |          |   |          |Количество|   Сумма    |Количество|   Сумма    |       |       |       |       |       |       |       |       |       |       |         |        |\n"));

fprintf(ffkl2,"%s\n",str);

}
/****************/
/*Итого по счёту для кладовщика*/
/****************/
void itsh1(const char *sh,double k1,double k2,double k3,double k4,
double s1,double s2,double s3,double s4,class iceb_u_str *str,FILE *ff,FILE *ffkl2)
{
char		bros[512];

sprintf(bros,"%s %s",gettext("Итого по счёту"),sh);

str->new_plus("");
for(int nom=0; nom < 240; nom++)
 str->plus("-");

fprintf(ff,"%-*s %15.2f\n%s\n",iceb_u_kolbait(90,bros),bros,s1,str->ravno());
fprintf(ffkl2,"%-*s %10.10g %12.2f %10.10g %12.2f\n%s\n",iceb_u_kolbait(85,bros),bros,k1,s1,k2,s2,str->ravno());

str->new_plus("");
for(int nom=0; nom < 240; nom++)
 str->plus(".");
}

/*****************/
/*Итого по складу для кладовщика*/
/*****************/
void itskl1(int skl,double kk1,double kk2,double kk3,
double kk4,double ss1,double ss2,double ss3,double ss4,class iceb_u_str *str,
FILE *ff,FILE *ffkl2)
{
char		bros[512];

sprintf(bros,"%s %d",gettext("Итого по складу"),skl);

str->new_plus("");
for(int nom=0; nom < 240; nom++)
 str->plus("-");

fprintf(ff,"%-*s %15.2f\n%s\n",iceb_u_kolbait(90,bros),bros,ss1,str->ravno());
fprintf(ffkl2,"%-*s %10.10g %12.2f %10.10g %12.2f\n%s\n",iceb_u_kolbait(85,bros),bros,kk1,ss1,kk2,ss2,str->ravno());
str->new_plus("");
for(int nom=0; nom < 240; nom++)
 str->plus(".");
}
/*******/
/*Счетчик для кладовщика*/
/*******/
void gsapp2(short dn,short mn,short gn,short dk,short mk,
short gk,
int *sl,  //Счетчик листов
int *kst, //Счетчик строк
int skl,const char *nai,
class iceb_u_str *str,
FILE *ff,FILE *ffkl2)
{

*kst+=2;

if(*kst >= kol_strok_na_liste-1)
 {
  fprintf(ff,"\f");
  fprintf(ffkl2,"\f");

  *sl+=1;
  shdtmcf1k(dn,mn,gn,*sl,ff,skl,nai,ffkl2);
  *kst=6;

  str->new_plus("");
  for(int nom=0; nom < 240; nom++)
   str->plus(".");

 }

}
