/*$Id: zar_f1pv_r.c,v 1.21 2013/09/26 09:46:59 sasa Exp $*/
/*23.05.2016	22.01.2007	Белых А.И.	zar_f1pv_r.c
Расчёт формы 1ПВ	
*/
#include <math.h>
#include <errno.h>
#include "buhg_g.h"

class zar_f1pv_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  const char *data_d;
  class iceb_u_spisok *imaf;
  class iceb_u_spisok *naimf;
    
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_f1pv_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_f1pv_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_f1pv_r_data *data);
gint zar_f1pv_r1(class zar_f1pv_r_data *data);
void  zar_f1pv_r_v_knopka(GtkWidget *widget,class zar_f1pv_r_data *data);

void zarspek1_r(int kold,double suman,float dni,float dnei,int *kolrab,int *kolrab1,class iceb_u_double *DIAP);
void zarspek1_ras(short mn,short gn,int kold,int *kolrab,int *kolrab1,class iceb_u_double *DIAP,int metka,FILE *ff,GtkWidget*);

extern SQL_baza bd;

extern short    *knvr;/*Коды начислений не входящие в расчёт подоходного налога*/
extern char	*shetb; /*Бюджетные счета начислений*/

int zar_f1pv_r(const char *data_d,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zar_f1pv_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);

data.data_d=data_d;
data.imaf=imaf;
data.naimf=naimf;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт статистической формы 1-ПВ"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_f1pv_r_key_press),&data);

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

repl.plus(gettext("Расчёт статистической формы 1-ПВ"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_f1pv_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zar_f1pv_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_f1pv_r_v_knopka(GtkWidget *widget,class zar_f1pv_r_data *data)
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

gboolean   zar_f1pv_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_f1pv_r_data *data)
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

gint zar_f1pv_r1(class zar_f1pv_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short mn,gn;
iceb_u_rsdat1(&mn,&gn,data->data_d);


FILE	*ff,*ffsovm;
char	imaf[64];
char	imafsovm[64];
int	kolstr=0;
float kolstr1=0;
int kolstr2=0;
SQL_str row,row1;
long    tabn=0;
class SQLCURSOR cur;
class SQLCURSOR cur1;
double	suma;
double  suman=0.;
short   knah=0;
class iceb_u_double DIAP; //Массив диапазонов
int	kold=0;

sprintf(strsql,"%s:%d.%d%s\n",gettext("Дата"),
mn,gn,gettext("г."));
iceb_printw(strsql,data->buffer,data->view);


class iceb_u_str kodtabotrv(""); //Коды табеля отработанного времени

iceb_poldan("Коды видов табеля отработанного времени",&kodtabotrv,"zar1-pv.alx",data->window);
if(kodtabotrv.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введены \"Коды видов табеля отработанного времени\" !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
float dni,dnei,hasov;
redkalw(mn,gn,&dnei,&hasov,data->window);
if(dnei == 0.)
 {
  iceb_menu_soob(gettext("Не ввели количество рабочих дней в месяце !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

SQL_str row_alx;
class SQLCURSOR cur_alx;
const char *imaf_alx={"zar1-pv.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  suma=atof(row_alx[0]);
  if(fabs(suma) < 0.009)
    continue;
  DIAP.plus(suma,-1);
  kold++;
 }

//общий отчёт
int kolrab[kold+1]; //полностью отработанное время за месяц
memset(&kolrab,'\0',sizeof(kolrab));
int kolrab1[kold+1]; //отработано больше 50% времени за месяц
memset(&kolrab1,'\0',sizeof(kolrab1));

//отчёт по хозрасчёту
int kolrab_hoz[kold+1]; //полностью отработанное время за месяц
memset(&kolrab_hoz,'\0',sizeof(kolrab_hoz));
int kolrab1_hoz[kold+1]; //отработано больше 50% времени за месяц
memset(&kolrab1_hoz,'\0',sizeof(kolrab1_hoz));

//отчёт по бюджету
int kolrab_bud[kold+1]; //полностью отработанное время за месяц
memset(&kolrab_bud,'\0',sizeof(kolrab_bud));
int kolrab1_bud[kold+1]; //отработано больше 50% времени за месяц
memset(&kolrab1_bud,'\0',sizeof(kolrab1_bud));

sprintf(strsql,"select tabn,sovm from Zarn where god=%d and mes=%d",gn,mn);

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

sprintf(imafsovm,"f1-sv%d.lst",getpid());

if((ffsovm = fopen(imafsovm,"w")) == NULL)
 {
  iceb_er_op_fil(imafsovm,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_zagolov(gettext("Список совместителей"),0,0,0,0,mn,gn,ffsovm,data->window);

fprintf(ffsovm,"\
--------------------------------------------------------------\n");
fprintf(ffsovm,"\
 N | Т/н |       Фамилия, имя, отчество           |  Сумма   |\n");

fprintf(ffsovm,"\
--------------------------------------------------------------\n");
class iceb_u_str fio("");
double sumanah=0.;
double itsovm=0.;
short  nomerpp=0;
kolstr1=0;
SQLCURSOR curr;
double suman_hoz;
double suman_bud;

while(cur.read_cursor(&row) != 0)
 {
//  printw("%s\n",row[0]);
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  tabn=atol(row[0]);

  if(row[1][0] == '1')
   {
    sumanah=0.;
    sprintf(strsql,"select fio from Kartb where tabn=%s",row[0]);
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
      fio.new_plus(row1[0]);         

    sprintf(strsql,"select suma from Zarp where datz >= '%d-%d-%d' and \
datz <= '%d-%d-%d' and tabn=%s and prn='1'",
    gn,mn,1,gn,mn,31,row[0]);

    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
      continue;
     }
    while(cur1.read_cursor(&row1) != 0)
      sumanah+=atof(row1[0]);
    itsovm+=sumanah;          
    fprintf(ffsovm,"%3d %5s %-*.*s %10.2f\n",
    ++nomerpp,row[0],iceb_u_kolbait(40,fio.ravno()),iceb_u_kolbait(40,fio.ravno()),fio.ravno(),sumanah);
   }




  sprintf(strsql,"select kodt,dnei from Ztab where tabn=%ld and god=%d \
and mes=%d",tabn,gn,mn);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  if(kolstr2 == 0)
    continue;
  dni=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    if(iceb_u_proverka(kodtabotrv.ravno(),row1[0],0,1) == 0)
      dni+=atof(row1[1]);
   }

  if(dni < dnei/2)
    continue;
    
  sprintf(strsql,"select knah,suma,shet from Zarp where datz >= '%04d-%02d-01' and \
datz <= '%04d-%02d-31' and tabn=%ld and prn='1' and suma <> 0.",
  gn,mn,gn,mn,tabn);
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  if(kolstr2 == 0)
    continue;
  suman=0.;
  suman_hoz=0.;
  suman_bud=0.;
  
  while(cur1.read_cursor(&row1) != 0)
   {
    knah=atoi(row1[0]);
    suma=atof(row1[1]);
    if(provkodw(knvr,knah) >= 0)
      continue;
    suman+=suma;
    if(iceb_u_proverka(shetb,row[2],0,0) == 0)
     suman_bud+=suma;
    else
     suman_hoz+=suma;
   }

  zarspek1_r(kold,suman,dni,dnei,kolrab,kolrab1,&DIAP);
  zarspek1_r(kold,suman_hoz,dni,dnei,kolrab_hoz,kolrab1_hoz,&DIAP);
  zarspek1_r(kold,suman_bud,dni,dnei,kolrab_bud,kolrab1_bud,&DIAP);

 }

fprintf(ffsovm,"\
--------------------------------------------------------------\n");
fprintf(ffsovm,"%*s:%10.2f\n",iceb_u_kolbait(50,gettext("Итого")),gettext("Итого"),itsovm);
iceb_podpis(ffsovm,data->window);
fclose(ffsovm);

sprintf(imaf,"f1-pv%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


//Распечатываем общий свод
zarspek1_ras(mn,gn,kold,kolrab,kolrab1,&DIAP,0,ff,data->window);

fclose(ff);

char imaf_hoz[30];
char imaf_bud[30];

/*********************/
if(shetb != NULL)
 {
  sprintf(imaf_hoz,"f1-pvh%d.lst",getpid());

  if((ff = fopen(imaf_hoz,"w")) == NULL)
   {
    iceb_er_op_fil(imaf_hoz,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  //Распечатываем хозрасчёт
  zarspek1_ras(mn,gn,kold,kolrab_hoz,kolrab1_hoz,&DIAP,1,ff,data->window);

  fclose(ff);

  sprintf(imaf_bud,"f1-pvb%d.lst",getpid());

  if((ff = fopen(imaf_bud,"w")) == NULL)
   {
    iceb_er_op_fil(imaf_bud,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  //Распечатываем бюджет
  zarspek1_ras(mn,gn,kold,kolrab_bud,kolrab1_bud,&DIAP,2,ff,data->window);

  fclose(ff);

 }

data->imaf->plus(imaf);
data->imaf->plus(imafsovm);
data->naimf->plus(gettext("Расчёт статистической формы 1-ПВ"));
data->naimf->plus(gettext("Список совместителей"));
if(shetb != NULL)
 {
  data->imaf->plus(imaf_hoz);
  data->imaf->plus(imaf_bud);
  data->naimf->plus(gettext("Расчёт статистической формы 1-ПВ (хозрасчёт)")); 
  data->naimf->plus(gettext("Расчёт статистической формы 1-ПВ (бюджет)"));  
 }

for(int nom=0; nom < data->imaf->kolih(); nom++)
 iceb_ustpeh(data->imaf->ravno(nom),3,data->window);





data->kon_ras=0;
data->voz=0;
gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

return(FALSE);

}
/****************************/
/*Расчёт*/
/*********************/
void zarspek1_r(int kold,double suman,float dni,float dnei,
int *kolrab,
int *kolrab1,
class iceb_u_double *DIAP) //Массив диапазонов
{
int i=0;
double suma=0.;
for(i=0; i < kold; i++)
 {
  suma=DIAP->ravno(i);

  if(suman > suma)
    continue;
  else
   {
    if(dni == dnei)
      kolrab[i]+=1;
    else
      kolrab1[i]+=1;
    break;
   }
 }
if(i == kold)
 {
  if(dni == dnei)
    kolrab[kold]+=1;
  else
    kolrab1[kold]+=1;
 }  


}
/***************************/
/*Распечатка*/
/*************************/
void zarspek1_ras(short mn,short gn,
int kold,
int *kolrab,
int *kolrab1,
class iceb_u_double *DIAP, //Массив диапазонов
int metka, //0-общий свод 1-хозрасчёт 2-бюджет
FILE *ff,
GtkWidget *wpredok)
{

iceb_zagolov(gettext("Расчёт статистической формы 1-ПВ"),0,0,0,0,mn,gn,ff,wpredok);

if(metka == 1)
 fprintf(ff,"%s\n",gettext("Хозрасчёт"));

if(metka == 2)
 fprintf(ff,"%s\n",gettext("Бюджет"));

fprintf(ff,"\
---------------------------------\n");
fprintf(ff,gettext("\
  Размер зарплаты   |Дней |Дней |\n"));
fprintf(ff,"\
                    |100%% | 50%% |\n");
fprintf(ff,"\
---------------------------------\n");

int ikol=0;
int ikol1=0;
double suma=0.;
double suman=0.;
for(int i=0; i < kold; i++)
 {
  suma=DIAP->ravno(i);
//  printw("suma=%.2f\n",suma);
  if(i == 0)
    fprintf(ff,"%8.2f => %8.2f %5d %5d\n",0.,suma,kolrab[i],kolrab1[i]);
  else
    fprintf(ff,"%8.2f => %8.2f %5d %5d\n",suman,suma,kolrab[i],kolrab1[i]);
  suman=suma;
  ikol+=kolrab[i];
  ikol1+=kolrab1[i];
 }
fprintf(ff,"%8.2f => %-8s %5d %5d\n",suma," ",kolrab[kold],kolrab1[kold]);
ikol+=kolrab[kold];
ikol1+=kolrab1[kold];
fprintf(ff,"\
---------------------------------\n");
fprintf(ff,"%*s %5d %5d\n",iceb_u_kolbait(20,gettext("Итого")),gettext("Итого"),ikol,ikol1);

iceb_podpis(ff,wpredok);

}
