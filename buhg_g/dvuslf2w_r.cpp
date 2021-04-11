/*$Id: dvuslf2w_r.c,v 1.19 2013/09/26 09:46:47 sasa Exp $*/
/*23.05.2016	16.12.2005	Белых А.И.	dvuslf2w_r.c
Расчёт движения услуг
*/
#include <errno.h>
#include "buhg_g.h"
#include "dvuslf2w.h"

class dvuslf2w_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class dvuslf2w_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  dvuslf2w_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   dvuslf2w_r_key_press(GtkWidget *widget,GdkEventKey *event,class dvuslf2w_r_data *data);
gint dvuslf2w_r1(class dvuslf2w_r_data *data);
void  dvuslf2w_r_v_knopka(GtkWidget *widget,class dvuslf2w_r_data *data);

void gsapp1(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,FILE *ff);
void itgdvus(short met,const char kod[],const char naik[],double *ikol,double *isum,int *kst,FILE *ff);
void gsapp1_shetl(short dn,short mn,short gn,short dk,short mk,short gk,int *sl,int *kst,FILE *ff);

extern SQL_baza bd;
extern double	okrg1;  /*Округление суммы*/
extern double	okrcn;  /*Округление цены*/
extern int      kol_strok_na_liste;

int dvuslf2w_r(class dvuslf2w_rr *datark,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
char strsql[512];
iceb_u_str repl;
class dvuslf2w_r_data data;

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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(dvuslf2w_r_key_press),&data);

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(dvuslf2w_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)dvuslf2w_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  dvuslf2w_r_v_knopka(GtkWidget *widget,class dvuslf2w_r_data *data)
{
//printf("dvuslf2w_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   dvuslf2w_r_key_press(GtkWidget *widget,GdkEventKey *event,class dvuslf2w_r_data *data)
{
// printf("dvuslf2w_r_key_press\n");
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

gint dvuslf2w_r1(class dvuslf2w_r_data *data)
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

int tipz=data->rk->prr;


sprintf(strsql,"select * from Usldokum2 where datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' order by shetu,podr,datp asc",
gn,mn,dn,gk,mk,dk);
SQLCURSOR cur;
int kolstr=0;
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

char imaf[64];
sprintf(imaf,"dv%d.lst",getpid());

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Движение услуг"));

FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

//fprintf(ff,"\x1B\x4D");
//fprintf(ff,"\x0F");

iceb_zagolov(gettext("Движение услуг"),dn,mn,gn,dk,mk,gk,ff,data->window);
int kst=5;

if(tipz == 0)
 {
  fprintf(ff,"%s\n",gettext("Перечень приходов/расходов"));
  kst++;
 }
if(tipz == 1)
 {
  fprintf(ff,"%s\n",gettext("Перечень приходов"));
  kst++;
 }
if(tipz == 2)
 {
  fprintf(ff,"%s\n",gettext("Перечень расходов"));
  kst++;
 }

if(data->rk->podr.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Подразделение"),data->rk->podr.ravno());
  kst++;
 }
if(data->rk->kontr.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
  kst++;
 }
if(data->rk->kodop.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
  kst++;
 }
if(data->rk->grupa.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Группа"),data->rk->grupa.ravno());
  kst++;
 }
if(data->rk->kodusl.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Код услуги"),data->rk->kodusl.ravno());
  kst++;
 }
if(data->rk->shet.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Счета учета"),data->rk->shet.ravno());
  kst++;
 }
 
int podr=0;
int mvstr=0,podrl=0;

sprintf(strsql,"%s %d.%d.%d %s %d.%d.%d\n",gettext("Период с"),dn,mn,gn,gettext("по"),
dk,mk,gk);

iceb_printw(strsql,data->buffer,data->view);

float kolstr1=0.;
class iceb_u_str shet1("");
class iceb_u_str shet("");
double ishetk=0.,ishets=0.;
int sli=0;
class iceb_u_str naimsh("");
double itk=0,its=0.;
SQL_str row;
SQL_str row1;
SQLCURSOR cur1;
double cena=0.;
double suma=0.;
double kolih=0.;
class iceb_u_str ei("");
class iceb_u_str kontr("");
class iceb_u_str kodop("");
short metka=0;
int kodzap=0;
class iceb_u_str naim("");
short d,m,g;
short dp,mp,gp;
class iceb_u_str naim_kontr("");

gsapp1(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);


while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  podr=atoi(row[10]);
  shet.new_plus(row[9]);  

  if(iceb_u_proverka(data->rk->shet.ravno(),shet.ravno(),1,0) != 0)
     continue;

  if(iceb_u_proverka(data->rk->podr.ravno(),row[10],0,0) != 0)
    continue;

  if(tipz != 0 )
   if(tipz != atoi(row[0]))
       continue;


  
  /*Читаем накладную*/
  sprintf(strsql,"select kontr,kodop from Usldokum \
where datd='%s' and podr=%d and nomd='%s' and tp=%s",
  row[1],podr,row[3],row[0]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найден документ !"));

    sprintf(strsql,"%s=%s %s=%s %s=%d",
    gettext("Дата"),row[4],
    gettext("Документ"),row[2],
    gettext("Подраделение"),podr);   
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,data->window);

    fprintf(ff,gettext("Не найден документ !"));
    fprintf(ff,"%s=%s %s=%s %s=%d\n",
    gettext("Дата"),row[4],
    gettext("Документ"),row[2],
    gettext("Подраделение"),podr);   
    continue;
   }

  kontr.new_plus(row1[0]);
  kodop.new_plus(row1[1]);

  if(iceb_u_proverka(data->rk->kontr.ravno(),kontr.ravno(),0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->kodop.ravno(),kodop.ravno(),0,0) != 0)
    continue;

  metka=atoi(row[4]);
  kodzap=atoi(row[5]);
  cena=atof(row[7]);
  ei.new_plus(row[8]);


  if(iceb_u_proverka(data->rk->kodusl.ravno(),row[5],0,0) != 0)
    continue;

  if(metka == 0)
   {
    /*Узнаем наименование материалла*/
    sprintf(strsql,"select naimat,kodgr from Material where kodm=%d",
    kodzap);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      fprintf(ff,"%s %d !\n",gettext("Не найден код материалла"),kodzap);
      sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodzap);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
   }

  if(metka == 1)
   {
    /*Читаем наименование услуги*/
    sprintf(strsql,"select naius,kodgr from Uslugi where kodus=%d",
    kodzap);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s %d !\n",gettext("Не найден код услуги"),kodzap);
      fprintf(ff,"%s %d !\n",gettext("Не найден код услуги"),kodzap);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
   }

  naim.new_plus(row1[0]);

  if(iceb_u_proverka(data->rk->grupa.ravno(),row1[1],0,0) != 0)
     continue;

  if(podrl != podr)
   {
    if(podrl != 0)
     {
      gsapp1_shetl(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);
      fprintf(ff,"%s %d",gettext("Подразделение"),podr);
      fprintf(ff,"----------------------------------\n");
     } 
    podrl=podr;
   } 


  if(iceb_u_SRAV(shet1.ravno(),shet.ravno(),0) != 0)
   {
    if(shet1.ravno()[0] != '\0')
      itgdvus(0,shet1.ravno(),naimsh.ravno(),&ishetk,&ishets,&kst,ff);

    naimsh.new_plus("");
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),shet.ravno());
      iceb_menu_soob(strsql,data->window);
     }
    else
     naimsh.new_plus(row1[0]);   

    gsapp1_shetl(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);

    fprintf(ff,"%s %s %s\n",gettext("Счёт"),shet.ravno(),naimsh.ravno());
    shet1.new_plus(shet.ravno());
   }

  if(metka == 1) //Смотрим дополнение к наименованию
   {
    sprintf(strsql,"select dnaim from Usldokum1 where datd='%s' and \
podr=%s and nomd='%s' and metka=1 and kodzap=%s and tp=%s",
    row[1],row[10],row[3],row[5],row[0]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      naim.plus(" ",row1[0]);
     }    
   }     
  

  /*printw("%d %s\n",mt5.nkr,naim);*/
  mvstr=0;
  
  iceb_u_rsdat(&dp,&mp,&gp,row[2],2);
  iceb_u_rsdat(&d,&m,&g,row[1],2);
  kolih=atof(row[6]);
  

  /*Узнаем наименование контрагента*/
  if(kontr.ravno()[0] == '0' && iceb_u_pole(kontr.ravno(),strsql,1,'-') == 0)
     kontr.new_plus(strsql);   

  naim_kontr.new_plus("");
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'", kontr.ravno());
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
     fprintf(ff,"%s %s !\n",gettext("Не найден код контрагента"),kontr.ravno());
     sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr.ravno());
     iceb_menu_soob(strsql,data->window);
   }
  else
    naim_kontr.new_plus(row1[0]);

  gsapp1_shetl(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);

  if(atoi(row[0]) == 1)
     kodop.plus("+");
  if(atoi(row[0]) == 2)
     kodop.plus("-");

  sprintf(strsql,"%s\n",naim.ravno());
  iceb_printw(strsql,data->buffer,data->view);

  if(mvstr == 0)
   {
     
    fprintf(ff,"\
%-5d %-*.*s %-*s %-*s %14.10g %12.12g %15.2f %-*s %02d.%02d.%d %02d.%02d.%d %-7s %3s %s\n",
    kodzap,
    iceb_u_kolbait(35,naim.ravno()),iceb_u_kolbait(35,naim.ravno()),naim.ravno(),
    iceb_u_kolbait(5,ei.ravno()),ei.ravno(),
    iceb_u_kolbait(7,row[9]),row[9],
    cena,
    kolih,kolih*cena,
    iceb_u_kolbait(15,row[3]),
    row[3],
    d,m,g,dp,mp,gp,kodop.ravno(),kontr.ravno(),naim_kontr.ravno());
    if(iceb_u_strlen(naim.ravno()) > 35)
     {
      gsapp1_shetl(dn,mn,gn,dk,mk,gk,&sli,&kst,ff);
      fprintf(ff,"%5s %s\n"," ",iceb_u_adrsimv(35,naim.ravno()));
     }
   }
  else
   {
    fprintf(ff,"\
%5s %-35s %-15s %-*s %-*s %14.10g %12.12g %15.2f %-*s %02d.%02d.%d %02d.%02d.%d %-7s %3s %s\n",
    " "," "," ",
    iceb_u_kolbait(5,ei.ravno()),ei.ravno(),
    iceb_u_kolbait(7,row[9]),row[9],
    cena,
    kolih,kolih*cena,
    iceb_u_kolbait(15,row[3]),
    row[3],
    d,m,g,dp,mp,gp,kodop.ravno(),kontr.ravno(),naim_kontr.ravno());
   }  
  
  suma=cena*kolih;
  suma=iceb_u_okrug(suma,0.01);
  its+=suma;
  itk+=kolih;
  ishetk+=kolih;
  ishets+=suma;  

  mvstr++;

 }


//Итого по счёту
itgdvus(0,shet1.ravno(),naimsh.ravno(),&ishetk,&ishets,&kst,ff);

//Общий итог
ishetk=itk; //Чтобы не обнулились результаты
ishets=its;
itgdvus(1,shet1.ravno(),naimsh.ravno(),&ishetk,&ishets,&kst,ff);

sprintf(strsql,"%s:\n\
%-*s %12.12g\n\
%-*s %12.2f%s\n",
gettext("Итого"),
iceb_u_kolbait(10,gettext("количество")),gettext("количество"),
itk,
iceb_u_kolbait(10,gettext("сумма")),gettext("сумма"),
its,
gettext("грн.")); 

iceb_printw(strsql,data->buffer,data->view);

//fprintf(ff,"\x1B\x50");
//fprintf(ff,"\x12");

fclose(ff);

iceb_ustpeh(imaf,3,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/***********************************/
/*Выдача шапки*/
/**********************************/
void            gsapp1(short dn,short mn,short gn,short dk,short mk,
short gk,
int *sl, //Счётчик листов
int *kst,  //Счётчик строк
FILE *ff)
{
*sl+=1;

fprintf(ff,"\
%s %02d.%02d.%4d%s %s %02d.%02d.%4d%s %100s%sN%d\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
"",
gettext("Лист"),
*sl);

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext(" Код |        Наименование               |Един.| Счёт  |              |            |               |               | Д а т а  | Д а т а  |       |\n\
     |          услуги                   |изме-| учета |   Ц е н а    | Количество |     Сумма     |Номер документа| выписки  |подтверж- |Код оп.|         К о н т р а г е н т\n\
     |                                   |рения|       |              |            |               |               |документа | дения    |       |\n"));

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

*kst+=6;

}
/**************/
/*Выдача итога*/
/**************/

void itgdvus(short met, //0-по счету 1 общий итог
const char kod[],const char naik[],double *ikol,double *isum,
int *kst,FILE *ff)
{
char		bros[512];

if(met == 0)
 sprintf(bros,"%s %s %s",gettext("Итого по счёту"),kod,naik);
if(met == 1)
 sprintf(bros,"%s",gettext("Общий итог"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%*.*s:%12.12g %15.2f\n",iceb_u_kolbait(70,bros),iceb_u_kolbait(70,bros),bros,*ikol,*isum);

*kst=*kst+2;

*ikol=0.;
*isum=0.;
}
/*******/
/*Шапка*/
/*******/
void            gsapp1_shetl(short dn,short mn,short gn,short dk,short mk,
short gk,
int *sl, //Счетчик листов
int *kst,  //Счетчик строк
FILE *ff)
{
*kst+=1;

if( *kst <= kol_strok_na_liste)
 return;

*kst=0;
fprintf(ff,"\f");

gsapp1(dn,mn,gn,dk,mk,gk,sl,kst,ff);
*kst+=1;
}
