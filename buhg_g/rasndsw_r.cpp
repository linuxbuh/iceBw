/*$Id: rasndsw_r.c,v 1.29 2013/09/26 09:46:54 sasa Exp $*/
/*23.05.2016	24.04.2005	Белых А.И. 	rasndsw_r.c
Расчёт реестра проводок по видам операций
*/
#include <math.h>
#include <errno.h>
#include "buhg_g.h"
#include "rasndsw.h"

class rasndsw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class rasndsw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  rasndsw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rasndsw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasndsw_r_data *data);
gint rasndsw_r1(class rasndsw_r_data *data);
void  rasndsw_r_v_knopka(GtkWidget *widget,class rasndsw_r_data *data);

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

void rppvo1(int skl1,char *naiskl,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,
int *kst,
FILE *ff);

void rasmasop(iceb_u_spisok *spopr,iceb_u_double *mkm,iceb_u_double *mkm1_1r,iceb_u_double *mkm1_1k,
short tip,FILE *ff,class rasndsw_r_data *data);


extern SQL_baza bd;
extern double   okrg1;
extern double	okrcn;
extern short	vtara;

int rasndsw_r(class rasndsw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class rasndsw_r_data data;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать распределение НДС по видам операций"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rasndsw_r_key_press),&data);

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

repl.plus(gettext("Распечатать распределение НДС по видам операций"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(rasndsw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)rasndsw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rasndsw_r_v_knopka(GtkWidget *widget,class rasndsw_r_data *data)
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

gboolean   rasndsw_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasndsw_r_data *data)
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

gint rasndsw_r1(class rasndsw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;

iceb_u_spisok           spoprp;/*Массив с перечнем операций приходов*/
iceb_u_spisok           spoprr;/*Массив с перечнем операций расходов*/
iceb_u_double mkm1p; /*Массив приходов*/
iceb_u_double mkm1r; /*Массив расходов в  ценах накладной*/
iceb_u_double mkm1_1r; /*Массив расходов в  ценах карточек*/
iceb_u_double mkm1_1k; /*Массив кореектировки сумм*/
short dn,mn,gn;
short dk,mk,gk;

iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window);

sprintf(strsql,"%s %d.%d.%d%s %s %d.%d.%d%s\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."));

iceb_printw(strsql,data->buffer,data->view);

/*Создаем массивы операций-материалов*/
sprintf(strsql,"select kod from Prihod");

//dlmp=dlmr=kolopp=kolopr=0;
SQLCURSOR cur;
SQLCURSOR cur1;
SQLCURSOR cur2;
SQL_str row;
SQL_str row1;
SQL_str row2;

int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
  spoprp.plus(row[0]);

sprintf(strsql,"select kod from Rashod");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
  spoprr.plus(row[0]);

int kolopp=spoprp.kolih();
int kolopr=spoprr.kolih();

sprintf(strsql,"%s:%d\n",gettext("Количество операций приходов"),kolopp);
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s:%d\n",gettext("Количество операций расходов"),kolopr);
iceb_printw(strsql,data->buffer,data->view);

if(kolopp == 0 && kolopr == 0)
 {
  iceb_menu_soob(gettext("Не нашли ни одной операции"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
//Маcсивы приходов
mkm1p.make_class(kolopp*6);

//*Маcсивы расходов
mkm1r.make_class(kolopr*6);
mkm1_1r.make_class(kolopr*6);
mkm1_1k.make_class(kolopr*6);


/*Просматриваем документы и заполняем массивы*/


sprintf(strsql,"select * from Dokummat where datd >= '%d-%02d-%02d' and \
datd <= '%d-%02d-%02d'",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного документа !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
 
char imaf[64];
char imaf1[64];
char imaf2[64];

sprintf(imaf,"op%d.lst",getpid());
sprintf(imaf1,"oh%d.lst",getpid());
sprintf(imaf2,"ohb%d.lst",getpid());
FILE *ff;
FILE *ff1;
FILE *ff2;

data->rk->imaf.new_plus(imaf);
data->rk->imaf.plus(imaf1);
data->rk->imaf.plus(imaf2);

data->rk->naimf.new_plus(gettext("Расчёт распределения НДС по видам операций"));
data->rk->naimf.plus(gettext("Протокол ошибок"));
data->rk->naimf.plus(gettext("Протокол ошибок"));


if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_startfil(ff);
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_startfil(ff1);

if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_u_startfil(ff2);

fprintf(ff1,"%s\n\n%s.\n",iceb_get_pnk("00",0,data->window),gettext("Контроль ошибок"));

fprintf(ff1,"\n\
--------------------------------------------------------------------------\n");

fprintf(ff1,gettext("\
  Дата    | Документ  |Опер|Код м|Колич.|Цена в док|Цена в кар|  Сумма   |\n"));
  
fprintf(ff1,"\
--------------------------------------------------------------------------\n");

fprintf(ff2,"%s\n\n%s.\n",iceb_get_pnk("00",0,data->window),gettext("Контроль ошибок"));

fprintf(ff2,"\n\
--------------------------------------------------------------------------\n");

fprintf(ff2,gettext("\
  Дата    | Документ  |Опер|Код м|Колич.|Цена в док|Цена в кар|  Сумма   |\n"));
  
fprintf(ff2,"\
--------------------------------------------------------------------------\n");
fprintf(ff,"\x0F\n");

iceb_zagolov(gettext("Расчёт распределения НДС по видам операций"),dn,mn,gn,dk,mk,gk,ff,data->window);


if(data->rk->sklad.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());

if(data->rk->pr_ras == 1)
  fprintf(ff,"%s\n",gettext("Только приходы"));
if(data->rk->pr_ras == 2)
  fprintf(ff,"%s\n",gettext("Только расходы"));

if(data->rk->kodop.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());

if(data->rk->kontr.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Контрагент"),data->rk->kontr.ravno());

if(data->rk->kodgrmat.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код группы материалла"),data->rk->kodgrmat.ravno());

if(data->rk->kodmat.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код материалла"),data->rk->kodmat.ravno());

if(data->rk->shet.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счета учета"),data->rk->shet.ravno());



float kolstr1=0;
double itog=0.; 
double itog1=0.; 
class iceb_u_str kop("");
int tp;
int skl;
int vo;
int kolstr2;
int nomop=0;
short d,m,g;
double suma=0.;
double sumao=0.;
double sumaok=0.;
double sumkor=0.;
double sumak=0.;
int mnds=0;
int nk;
double kolih;
double cena;
double cenak;
int vtr=0;
class iceb_u_str shu("");
double bb;
int kgrm;
double kol=0.;
float pnds=0.;
while(cur.read_cursor(&row) != 0)
 {
/*Если документ не подтвержден то его не смотрим*/
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(atoi(row[7]) == 0)
    continue;

  tp=atoi(row[0]);
  skl=atoi(row[2]);
  kop.new_plus(row[6]);

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[2],0,0) != 0)
    continue;  

  if(data->rk->pr_ras != 0 && data->rk->pr_ras != tp)
     continue;

  if(iceb_u_proverka(data->rk->kodop.ravno(),row[6],0,0) != 0)
    continue;  


  if(iceb_u_proverka(data->rk->kontr.ravno(),row[3],0,0) != 0)
    continue;  
  pnds=atof(row[13]);
  
  /*Читем код операции*/
  if(tp == 1)
    sprintf(strsql,"select vido from Prihod where kod='%s'",kop.ravno());
  if(tp == 2)
    sprintf(strsql,"select vido from Rashod where kod='%s'",kop.ravno());
  vo=0;
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код операции"),kop.ravno());
    iceb_menu_soob(strsql,data->window);
   }
  else
   vo=atoi(row1[0]);
  
  /*Читаем запис в накладной*/
  sprintf(strsql,"select * from Dokummat1 where datd='%s' and \
  sklad=%s and nomd='%s'",row[1],row[2],row[4]);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr2 == 0)
   {
    sprintf(strsql,"%s:%s\n",gettext("Нет записей в документе"),row[4]);
    iceb_printw(strsql,data->buffer,data->view);
    continue;
   }
  if(tp == 1)
    {
     if((nomop=spoprp.find(kop.ravno())) < 0 )
      {
       sprintf(strsql,gettext("Не найден код операции %s в массиве операций приходов !\n"),kop.ravno());
       iceb_menu_soob(strsql,data->window);
       continue;
      }
    }
  if(tp == 2)
    {
     if((nomop=spoprr.find(kop.ravno())) < 0 )
      {
       sprintf(strsql,gettext("Не найден код операции %s в массиве операций расходов !\n"),kop.ravno());
       iceb_menu_soob(strsql,data->window);
       continue;
      }
    }   
   iceb_u_rsdat(&d,&m,&g,row[1],2);    
   sumao=sumaok=0.;

    
   while(cur1.read_cursor(&row1) != 0)
    {
     if(iceb_u_proverka(data->rk->kodmat.ravno(),row1[4],0,0) != 0)
       continue;

     nk=atoi(row1[3]);
     cena=atof(row1[6]);
     cena=iceb_u_okrug(cena,okrcn);
     vtr=atoi(row1[8]);     
     kolih=atof(row1[5]);

     
     /*Читаем карточку материалла*/
     if(nk > 0)
      {
       sprintf(strsql,"select cena,shetu from Kart where sklad=%d and nomk=%d",
       skl,nk);
       if(sql_readkey(&bd,strsql,&row2,&cur2) != 1)
        {
         sprintf(strsql,"%s %d %s %d !\n",gettext("Не нашли карточку"),
         nk,
         gettext("Склад"),
         skl);
         iceb_menu_soob(strsql,data->window);

         continue;
        }
      }
     cenak=atof(row2[0]);
     cenak=iceb_u_okrug(cenak,okrcn);
     shu.new_plus(row2[1]);

     if(cenak-cena > 0.009)
      {
       sprintf(strsql,"%02d.%02d.%d %-*s %*s %10.10g %10.10g\n",
       d,m,g,
       iceb_u_kolbait(11,row[4]),row[4],
       iceb_u_kolbait(5,row1[4]),row1[4],
       cena,cenak);
       iceb_printw(strsql,data->buffer,data->view);
       
       bb=kolih*cenak - kolih*cena;

       fprintf(ff1,"%02d.%02d.%d %-*s %-*s %*s %6.6g %10.10g %10.10g %10.10g\n",
       d,m,g,
       iceb_u_kolbait(11,row[4]),row[4],
       iceb_u_kolbait(4,kop.ravno()),kop.ravno(),
       iceb_u_kolbait(5,row1[4]),row1[4],
       kolih,cena,cenak,bb);
       itog+=bb;
      }
     if(cenak-cena < 0.009)
      {
       bb=kolih*cena - kolih*cenak;
       fprintf(ff2,"%02d.%02d.%d %-*s %-*s %*s %6.6g %10.10g %10.10g %10.10g\n",
       d,m,g,
       iceb_u_kolbait(11,row[4]),row[4],
       iceb_u_kolbait(4,kop.ravno()),kop.ravno(),
       iceb_u_kolbait(5,row1[4]),row1[4],
       kolih,cena,cenak,bb);
       itog1+=bb;
      }
     
     if(vo == 1)
      {
        if(fabs(cenak-cena) > 0.0009)
         {
          sprintf(strsql,"%02d.%02d.%d %-*s %*s %10.10g %10.10g\n",
          d,m,g,
          iceb_u_kolbait(11,row[4]),row[4],
          iceb_u_kolbait(5,row1[4]),row1[4],
          cena,cenak);

          iceb_printw(strsql,data->buffer,data->view);

          bb=kolih*cenak - kolih*cena;
          fprintf(ff1,"%02d.%02d.%d %-*s %-*s %*s %6.6g %10.10g %10.10g %10.10g\n",
          d,m,g,
          iceb_u_kolbait(11,row[4]),row[4],
          iceb_u_kolbait(4,kop.ravno()),kop.ravno(),
          iceb_u_kolbait(5,row1[4]),row1[4],
          kolih,cena,cenak,bb);
         }
      }

     sprintf(strsql,"select kodgr from Material where kodm=%s",
     row1[4]);
     if(sql_readkey(&bd,strsql,&row2,&cur2) != 1)
      {
       sprintf(strsql,"%s %s !\n",gettext("Не найден код материалла"),row1[4]);
       iceb_menu_soob(strsql,data->window);
       continue;
      }

     if(iceb_u_proverka(data->rk->kodgrmat.ravno(),row2[0],0,0) != 0)
       continue;  
     kgrm=atoi(row2[0]);


     if(iceb_u_proverka(data->rk->shet.ravno(),shu.ravno(),0,0) != 0)
       continue;  
  

     kol=readkolkw(skl,nk,d,m,g,row[4],data->window);

     suma=kol*cena;
     suma=iceb_u_okrug(suma,okrg1);
     sumao+=suma;
    
     if(tp == 1)       
     if(vtr == 1 || kgrm == vtara)
      {
       mkm1p.plus(suma,6*nomop+5);
      }

     if(tp == 2)       
      {
       sumak=kol*cenak;
       sumak=iceb_u_okrug(sumak,okrg1);
       sumaok+=sumak;
       /*Возвратная тара*/
       if(vtr == 1 || kgrm == vtara)
        {
         mkm1r.plus(suma,6*nomop+5);
         mkm1_1r.plus(suma,6*nomop+5);
        }
      }
    } 



/* Читаем как была оформлена накладная*/
   mnds=0;
   sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=11",
   g,row[4],skl);

   if(sql_readkey(&bd,strsql,&row2,&cur2) == 1)
    mnds=atoi(row2[0]);

/* Читаем сумму корректировки*/

   sumkor=0.;
   sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=13",
   g,row[4],skl);

   if(sql_readkey(&bd,strsql,&row2,&cur2) == 1)
     sumkor=atof(row2[0]);
           
   if(tp == 1)
    {
     mkm1p.plus(sumao,6*nomop+mnds);
   
     if(mnds == 0)
      { 
       //Читаем вручную введенную сумму НДС
       sprintf(strsql,"select sodz from Dokummat2 \
 where god=%d and nomd='%s' and sklad=%d and nomerz=6",
       g,row[4],skl);
       if(sql_readkey(&bd,strsql,&row2,&cur2) == 1)
         suma=atof(row2[0]);
       else
         suma=sumao*pnds/100.;   

       suma=iceb_u_okrug(suma,okrg1);
       mkm1p.plus(suma,6*nomop+4);
      }
    }
 
   if(tp == 2)
    {
      
     mkm1r.plus(sumao,6*nomop+mnds);
     mkm1_1r.plus(sumaok,6*nomop+mnds);
     mkm1_1k.plus(sumkor,6*nomop+mnds);
  
     if(mnds == 0)
      { 
       suma=sumao*pnds/100.;   
       suma=iceb_u_okrug(suma,okrg1);
       mkm1r.plus(suma,6*nomop+4);

       sumak=sumaok*pnds/100.;
       sumak=iceb_u_okrug(sumak,okrg1);

       mkm1_1r.plus(sumak,6*nomop+4);
       mkm1_1k.plus(sumkor,6*nomop+4);
      }
    }

 }

fprintf(ff1,"%*s: %10.10g\n",iceb_u_kolbait(61,gettext("Итого")),gettext("Итого"),itog);
fprintf(ff2,"%*s: %10.10g\n",iceb_u_kolbait(61,gettext("Итого")),gettext("Итого"),itog1);

sprintf(strsql,"%s\n",gettext("Распечатываем"));
iceb_printw(strsql,data->buffer,data->view);
/*Распечатываем результат*/
rasmasop(&spoprp,&mkm1p,NULL,NULL,1,ff,data);
rasmasop(&spoprr,&mkm1r,&mkm1_1r,&mkm1_1k,2,ff,data);

iceb_podpis(ff,data->window);
iceb_podpis(ff1,data->window);
iceb_podpis(ff2,data->window);

fprintf(ff,"\x12");

fclose(ff);
fclose(ff1);
fclose(ff2);






gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
/********************/
/*Распечатка массивов*/
/*********************/
void		rasmasop(iceb_u_spisok *spopr,iceb_u_double *mkm,
iceb_u_double *mkm1_1r,
iceb_u_double *mkm1_1k,
short tip,FILE *ff,
class rasndsw_r_data *data)
{
double		suma;
double          its,it1,it2,it3,it4,it5,it6;
double          itsu,it1u,it2u,it3u,it4u,it5u,it6u;
double          itsk,it1k,it2k,it3k,it4k,it5k,it6k;
short		i;
class iceb_u_str naimo("");
SQL_str         row;
SQLCURSOR       cur;
char		strsql[1024];
class iceb_u_str bros("");
short kolopr=spopr->kolih();

float pnds=iceb_pnds(data->window);

suma=0.;
suma=mkm->suma();

it1=it2=it3=it4=it5=it6=0.;
it1u=it2u=it3u=it4u=it5u=it6u=0.;
it1k=it2k=it3k=it4k=it5k=it6k=0.;
if(suma > 0.009)
 {  
  if(tip == 1)
   {
    sprintf(strsql,"%s\n",gettext("Приходы"));
    iceb_printw(strsql,data->buffer,data->view);
    fprintf(ff,"%s\n",gettext("Приходы"));
   }
  if(tip == 2)
   {
    sprintf(strsql,"%s\n",gettext("Расходы"));
    iceb_printw(strsql,data->buffer,data->view);
   fprintf(ff,"%s\n",gettext("Расходы"));
   }
  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("Опер.|   Наименование операции       |      20%% НДС       |0%% Украина|0%% Експорт|0%% Cт.5   |Оборотная | Итого    |\n"));
fprintf(ff,gettext("     |                               | С у м м а|   НДС   |          |          |          |  тара    |          |\n"));

  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");

  for(i=0; i < kolopr ;i++)
   {
    bros.new_plus(spopr->ravno(i));
    if(bros.ravno()[0] == '\0')
     break;
    its=mkm->ravno(i*6)+mkm->ravno(i*6+1)+mkm->ravno(i*6+2)+mkm->ravno(i*6+3)+mkm->ravno(i*6+4)+mkm->ravno(i*6+5);
    if(its < 0.01)
     continue;      

    if(tip == 1)
      sprintf(strsql,"select naik from Prihod where kod='%s'",bros.ravno());
    if(tip == 2)
      sprintf(strsql,"select naik from Rashod where kod='%s'",bros.ravno());
    if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     {
      iceb_u_str repl;
      sprintf(strsql,"%s:%s\n",gettext("Не нашли код операции"),bros.ravno());
      repl.plus(strsql);
      fprintf(ff,"%s\n",repl.ravno());
      iceb_menu_soob(&repl,data->window);
      continue;
     }
    naimo.new_plus(row[0]);
    it1+=mkm->ravno(i*6);
    it2+=mkm->ravno(i*6+1);
    it3+=mkm->ravno(i*6+2);
    it4+=mkm->ravno(i*6+3);
    it5+=mkm->ravno(i*6+4);
    it6+=mkm->ravno(i*6+5);


    fprintf(ff,"%-*s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
    iceb_u_kolbait(5,bros.ravno()),bros.ravno(),
    iceb_u_kolbait(30,naimo.ravno()),iceb_u_kolbait(30,naimo.ravno()),naimo.ravno(),
    mkm->ravno(i*6),mkm->ravno(i*6+4),mkm->ravno(i*6+1),mkm->ravno(i*6+2),
    mkm->ravno(i*6+3),mkm->ravno(i*6+5),its);

    if(tip == 2)
     {
      itsu=mkm1_1r->ravno(i*6)+mkm1_1r->ravno(i*6+1)+mkm1_1r->ravno(i*6+2)+mkm1_1r->ravno(i*6+3)+mkm1_1r->ravno(i*6+4)+mkm1_1r->ravno(i*6+5);
      it1u+=mkm1_1r->ravno(i*6);
      it2u+=mkm1_1r->ravno(i*6+1);
      it3u+=mkm1_1r->ravno(i*6+2);
      it4u+=mkm1_1r->ravno(i*6+3);
      it5u+=mkm1_1r->ravno(i*6+4);
      it6u+=mkm1_1r->ravno(i*6+5);

      fprintf(ff,"%-5s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
      " ",
      iceb_u_kolbait(30,gettext("В учётных ценах")),
      iceb_u_kolbait(30,gettext("В учётных ценах")),
      gettext("В учётных ценах"),
      mkm1_1r->ravno(i*6),mkm1_1r->ravno(i*6+4),mkm1_1r->ravno(i*6+1),mkm1_1r->ravno(i*6+2),
      mkm1_1r->ravno(i*6+3),mkm1_1r->ravno(i*6+5),itsu);

      itsk=mkm1_1k->ravno(i*6)+mkm1_1k->ravno(i*6+1)+mkm1_1k->ravno(i*6+2)+mkm1_1k->ravno(i*6+3)+\
      mkm1_1k->ravno(i*6+4)*pnds/100.+mkm1_1k->ravno(i*6+5);
      it1k+=mkm1_1k->ravno(i*6);
      it2k+=mkm1_1k->ravno(i*6+1);
      it3k+=mkm1_1k->ravno(i*6+2);
      it4k+=mkm1_1k->ravno(i*6+3);
      it5k+=mkm1_1k->ravno(i*6+4)*pnds/100.;
      it6k+=mkm1_1k->ravno(i*6+5);
      
      fprintf(ff,"%-5s %-*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
      " ",
      iceb_u_kolbait(30,gettext("Суммы коректировок")),
      iceb_u_kolbait(30,gettext("Суммы коректировок")),
      gettext("Суммы коректировок"),
      mkm1_1k->ravno(i*6),mkm1_1k->ravno(i*6+4)*pnds/100.,mkm1_1k->ravno(i*6+1),mkm1_1k->ravno(i*6+2),
      mkm1_1k->ravno(i*6+3),mkm1_1k->ravno(i*6+5),itsk);
     }
   }
  its=it1+it5+it2+it3+it4+it6;
  itsk=it1k+it5k+it2k+it3k+it4k+it6k;
  itsu=it1u+it5u+it2u+it3u+it4u+it6u;

  fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n\
%-5s %*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  " ",
  iceb_u_kolbait(30,gettext("Итого")),
  iceb_u_kolbait(30,gettext("Итого")),
  gettext("Итого"),
  it1,it5,it2,it3,it4,it6,its);
  
  if(tip == 2)
   {
    fprintf(ff,"\
%-5s %*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  " ",
  iceb_u_kolbait(30,gettext("Итого корректировок")),
  iceb_u_kolbait(30,gettext("Итого корректировок")),
  gettext("Итого корректировок"),
  it1k,it5k,it2k,it3k,it4k,it6k,itsk);

    fprintf(ff,"\
%-5s %*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
  " ",
  iceb_u_kolbait(30,gettext("Итого в учётных ценах")),
  iceb_u_kolbait(30,gettext("Итого в учётных ценах")),
  gettext("Итого в учётных ценах"),
  it1u,it5u,it2u,it3u,it4u,it6u,itsu);

    fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n\
%-5s %*.*s %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f %10.2f\n",
    " ",
    iceb_u_kolbait(30,gettext("Разница")),
    iceb_u_kolbait(30,gettext("Разница")),
    gettext("Разница"),
    it1+it1k-it1u,
    it5+it5k-it5u,
    it2+it2k-it2u,
    it3+it3k-it3u,
    it4+it4k-it4u,
    it6+it6k-it6u,
    its+itsk-itsu);

   }
 }
}
