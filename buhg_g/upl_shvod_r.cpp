/*$Id: upl_shvod_r.c,v 1.17 2013/09/26 09:46:58 sasa Exp $*/
/*23.05.2016	02.03.2008	Белых А.И.	upl_shvod_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "upl_shvod.h"

class upl_shvod_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class upl_shvod_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  upl_shvod_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   upl_shvod_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_shvod_r_data *data);
gint upl_shvod_r1(class upl_shvod_r_data *data);
void  upl_shvod_r_v_knopka(GtkWidget *widget,class upl_shvod_r_data *data);

double upl_ptw(short dn,short mn,short gn,short dk,short mk,short gk,char *kod_avt,char *kod_vod,GtkWidget *wpredok);


extern SQL_baza bd;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int upl_shvod_r(class upl_shvod_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class upl_shvod_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка ведомости списания топлива по водителям и счетам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(upl_shvod_r_key_press),&data);

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

repl.plus(gettext("Распечатка ведомости списания топлива по водителям и счетам"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(upl_shvod_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)upl_shvod_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  upl_shvod_r_v_knopka(GtkWidget *widget,class upl_shvod_r_data *data)
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

gboolean   upl_shvod_r_key_press(GtkWidget *widget,GdkEventKey *event,class upl_shvod_r_data *data)
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
/*************************************/
/*Шапка                              */
/*************************************/

void    uplvstpsh_svst(int *kollist,int *kolstrok,FILE *ff)
{
*kollist+=1;
*kolstrok+=4;

fprintf(ff,"%60s%s%d\n","",gettext("Лист N"),*kollist);

fprintf(ff,"\
------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Счёт |К/в |Фамилия водителя|К.топ.|  Дата    |Ном.док.|Фак.з.т.|По норме|Недорасход|Перерасход|\n"));
fprintf(ff,"\
------------------------------------------------------------------------------------------------\n");
//123456 1234 1234567890123456 123456 1234567890 12345678 123456 12345678 1234
}

/************************************/
/*Итоговая строка                   */
/************************************/

void  itogvst(int metkai, //0-Итог по топливу 1-по водителю 2-по счету
const char *kodi, //Код итога
double toplivo,
double zat_top_nor,
double ned,
double per,
FILE *ff,
FILE *ff_shet)
{
char  strsql[512];
char  stroka[1024];
SQL_str  row;
SQLCURSOR curr;
if( toplivo == 0.)
  return;
  
memset(stroka,'\0',sizeof(stroka));

if(metkai == 0) //По топливу
 sprintf(stroka,"%s %s %s",
 gettext("Итого"),
 gettext("по"),
 kodi);

if(metkai == 1) //По водителю
 {
  sprintf(stroka,"%s %s %s ",
  gettext("Итого"),
  gettext("по"),
  kodi);

  sprintf(strsql,"select naik from Uplouot where kod=%s",kodi);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strcat(stroka,row[0]);
 }

if(metkai == 2) //По счету
 {
  sprintf(stroka,"%s %s %s ",
  gettext("Итого"),
  gettext("по"),
  kodi);

  sprintf(strsql,"select nais from Plansh where ns='%s'",kodi);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
    strcat(stroka,row[0]);

  fprintf(ff_shet,"%6s %*.*s %8.8g %8.8g %10.10g %10.10g\n",
  kodi,
  iceb_u_kolbait(20,row[0]),
  iceb_u_kolbait(20,row[0]),
  row[0],
  toplivo,zat_top_nor,ned,per);

 }
 
fprintf(ff,"%*s:%8.8g %8.8g %10.10g %10.10g\n",
iceb_u_kolbait(55,stroka),stroka,toplivo,zat_top_nor,ned,per);

}

/********************************/
/*Счетчик                       */
/********************************/
void uplvstpsh_shetvst(int *kolstrok,int *kollist,FILE *ff)
{
*kolstrok+=1;
if(*kolstrok > kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kolstrok=0;
  uplvstpsh_svst(kollist,kolstrok,ff);
 }  

}
/*********************************/
/*Реквизиты поиска */
/****************************/
void uplvstpsh_rp(int *kolstrok,const char *toplivo,
const char *voditel,
const char *shet,
const char *kod_avtom,
FILE *ff)
{
if(toplivo[0] != '\0')
 {
  *kolstrok+=1;
  fprintf(ff,"%s:%s\n",gettext("Код топлива"),toplivo);  
 }

if(voditel[0] != '\0')
 {
  *kolstrok+=1;
  fprintf(ff,"%s:%s\n",gettext("Код водителя"),voditel);  
 }

if(shet[0] != '\0')
 {
  *kolstrok+=1;
  fprintf(ff,"%s:%s\n",gettext("Счёт"),shet);  
 }
if(kod_avtom[0] != '\0')
 {
  *kolstrok+=1;
  fprintf(ff,"%s:%s\n",gettext("Код автомобиля"),kod_avtom);  
 }
}


/******************************************/
/******************************************/

gint upl_shvod_r1(class upl_shvod_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,row1;
class SQLCURSOR cur,cur1;


short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(strsql,data->buffer,data->view);




FILE    *ff;
char    imaf[64];
int	kollist=0;
int	kolstrok=0;
short   d,m,g;
int kolstr=0;

sprintf(strsql,"select * from Upldok2a where datd >= '%d-%d-%d' and datd <= '%d-%d-%d' order by datd asc",gn,mn,dn,gk,mk,dk);

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


/*для нужной нам сортировки создаём временную таблицу*/

class iceb_tmptab tabtmp;
const char *imatmptab={"uplvstpsh"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
datd DATE not null default '0000-00-00',\
kp int not null default 0,\
nomd char(32) not null default '',\
kt char(32) not null default '',\
shet char(32) not null default '',\
zt float(8,3) not null default 0.,\
zpn float(8,3) not null default 0.,\
kv int not null default 0,\
index(shet,kv,kt,datd)) ENGINE = MYISAM",imatmptab);
/*****
0 datd дата документа
1 kp   код подразделения
2 nomd номер документа
3 kt   код топлива
4 shet счёт
5 zt   затраты топлива фактические
6 zpn  затраты по норме
7 kv   код водителя
****/

if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
  return(1);
  
float kolstr1=0;

while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(data->rk->kod_top.ravno(),row[3],0,0) != 0)
   continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),row[4],0,0) != 0)
   continue;
  /*читаем шапку документа чтобы узнать водителя и автомобиль*/
  sprintf(strsql,"select ka,kv from Upldok where datd='%s' and kp=%s and nomd='%s'",row[0],row[1],row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(iceb_u_proverka(data->rk->kod_vod.ravno(),row1[1],0,0) != 0)
     continue;

    if(iceb_u_proverka(data->rk->kod_avt.ravno(),row1[0],0,0) != 0)
     continue;
   }     

  sprintf(strsql,"insert into %s values ('%s',%s,'%s','%s','%s',%s,%s,%s)", imatmptab,row[0],row[1],row[2],row[3],row[4],row[5],row[6],row1[1]);
  

  iceb_sql_zapis(strsql,1,0,data->window);
 }


sprintf(strsql,"select * from %s order by shet,kv,kt,datd asc",imatmptab);


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


sprintf(imaf,"vst%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


kolstrok=5;
iceb_zagolov(gettext("Ведомость списания топлива по счетам и водителям"),dn,mn,gn,dk,mk,gk,ff,data->window);

uplvstpsh_rp(&kolstrok,data->rk->kod_top.ravno(),data->rk->kod_vod.ravno(),data->rk->shet.ravno(),data->rk->kod_avt.ravno(),ff);

char imaf_shet[64];
FILE *ff_shet;
sprintf(imaf_shet,"vsts%d.lst",getpid());

if((ff_shet = fopen(imaf_shet,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_shet,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_zagolov(gettext("Cписание топлива по счетам"),dn,mn,gn,dk,mk,gk,ff_shet,data->window);

uplvstpsh_rp(&kolstrok,data->rk->kod_top.ravno(),data->rk->kod_vod.ravno(),data->rk->shet.ravno(),data->rk->kod_avt.ravno(),ff_shet);

fprintf(ff_shet,"\
--------------------------------------------------------------------\n");
fprintf(ff_shet,gettext("\
 Счёт |Наименование счёта  |Фак.з.т.|По норме|Недорасход|Перерасход|\n"));
fprintf(ff_shet,"\
--------------------------------------------------------------------\n");




uplvstpsh_svst(&kollist,&kolstrok,ff);

double itoplivo[3];
double izat_top_nor[3];
double stoplivo=0.;
class iceb_u_str naimvod;

memset(&itoplivo,'\0',sizeof(itoplivo));
memset(&izat_top_nor,'\0',sizeof(izat_top_nor));

class iceb_u_str kodvod("");
class iceb_u_str kodmt("");
class iceb_u_str shetz("");


kolstr1=0;
//double nst,nstzg,ztvsn,ztvrn;
double zat_top_nor=0.;
double raznica=0.;
double nedorashod_top=0.,pererashod_top=0.;
double nedorashod_vod=0.,pererashod_vod=0.;
double nedorashod_shet=0.,pererashod_shet=0.;

while(cur.read_cursor(&row) != 0)
 {
//  sprintf(strsql,"%s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4]);
//  iceb_printw(strsql,data->buffer,data->view);
  
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    


  if(iceb_u_SRAV(kodmt.ravno(),row[3],0) != 0)
   {
    if(kodmt.ravno()[0] != '\0')
     {
      uplvstpsh_shetvst(&kolstrok,&kollist,ff);
      itogvst(0,kodmt.ravno(),itoplivo[0],izat_top_nor[0],nedorashod_top,pererashod_top,ff,NULL);
     }
    nedorashod_top=pererashod_top=0.;
    itoplivo[0]=0.;
    izat_top_nor[0]=0.;
    kodmt.new_plus(row[3]);
   }

  if(iceb_u_SRAV(kodvod.ravno(),row[7],0) != 0)
   {
    if(kodvod.ravno()[0] != '\0')
     {
      uplvstpsh_shetvst(&kolstrok,&kollist,ff);
      itogvst(0,kodmt.ravno(),itoplivo[0],izat_top_nor[0],nedorashod_top,pererashod_top,ff,NULL);

      uplvstpsh_shetvst(&kolstrok,&kollist,ff);
      itogvst(1,kodvod.ravno(),itoplivo[1],izat_top_nor[1],nedorashod_vod,pererashod_vod,ff,NULL);
     }
    nedorashod_top=pererashod_top=0.;
    nedorashod_vod=pererashod_vod=0.;

    itoplivo[0]=0.;
    izat_top_nor[0]=0.;

    itoplivo[1]=0.;
    izat_top_nor[1]=0.;

    kodvod.new_plus(row[7]);

    naimvod.new_plus("");
    sprintf(strsql,"select naik from Uplouot where kod=%s",kodvod.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     naimvod.new_plus(row1[0]);
   }

  if(iceb_u_SRAV(shetz.ravno(),row[4],0) != 0)
   {
    if(shetz.ravno()[0] != '\0')
     {
      uplvstpsh_shetvst(&kolstrok,&kollist,ff);
      itogvst(0,kodmt.ravno(),itoplivo[0],izat_top_nor[0],nedorashod_top,pererashod_top,ff,NULL);

      uplvstpsh_shetvst(&kolstrok,&kollist,ff);
      itogvst(1,kodvod.ravno(),itoplivo[1],izat_top_nor[1],nedorashod_vod,pererashod_vod,ff,NULL);

      uplvstpsh_shetvst(&kolstrok,&kollist,ff);
      itogvst(2,shetz.ravno(),itoplivo[2],izat_top_nor[2],nedorashod_shet,pererashod_shet,ff,ff_shet);

     }
    nedorashod_top=pererashod_top=0.;
    nedorashod_vod=pererashod_vod=0.;
    nedorashod_shet=pererashod_shet=0.;
    memset(&itoplivo,'\0',sizeof(itoplivo));
    memset(&izat_top_nor,'\0',sizeof(izat_top_nor));

    shetz.new_plus(row[4]);
   }
  //читаем нормы расхода топлива

//  nst=nstzg=ztvsn=ztvrn=0.;


  iceb_u_rsdat(&d,&m,&g,row[0],2);

  uplvstpsh_shetvst(&kolstrok,&kollist,ff);


  stoplivo=atof(row[5]);
  stoplivo=iceb_u_okrug(stoplivo,0.001);  

  //определяем затраты топлива по норме
  zat_top_nor=atof(row[6]);
  zat_top_nor=iceb_u_okrug(zat_top_nor,0.001);

  raznica=stoplivo-zat_top_nor;
  raznica=iceb_u_okrug(raznica,0.001);
  
  fprintf(ff,"%-*s %-*s %-*.*s %-*s %02d.%02d.%04d %-8s %8.8g %8.8g",
  iceb_u_kolbait(6,row[4]),row[4],
  iceb_u_kolbait(4,row[7]),row[7],
  iceb_u_kolbait(16,naimvod.ravno()),iceb_u_kolbait(16,naimvod.ravno()),naimvod.ravno(),
  iceb_u_kolbait(6,row[3]),row[3],
  d,m,g,row[2],stoplivo,zat_top_nor);  

  if(raznica < 0)
   {
    fprintf(ff," %10.10g\n",raznica);
    nedorashod_top+=raznica;
    nedorashod_vod+=raznica;
    nedorashod_shet+=raznica;
   }
  else
   {
    fprintf(ff," %10s %10.10g\n"," ",raznica);
    pererashod_top+=raznica;    
    pererashod_vod+=raznica;    
    pererashod_shet+=raznica;    
   }
  

  itoplivo[0]+=stoplivo;
  izat_top_nor[0]+=zat_top_nor;
  
  itoplivo[1]+=stoplivo;
  izat_top_nor[1]+=zat_top_nor;

  itoplivo[2]+=stoplivo;
  izat_top_nor[2]+=zat_top_nor;

 }

itogvst(0,kodmt.ravno(),itoplivo[0],izat_top_nor[0],nedorashod_top,pererashod_top,ff,NULL);
itogvst(1,kodvod.ravno(),itoplivo[1],izat_top_nor[1],nedorashod_vod,pererashod_vod,ff,NULL);
itogvst(2,shetz.ravno(),itoplivo[2],izat_top_nor[2],nedorashod_shet,pererashod_shet,ff,ff_shet);

iceb_podpis(ff,data->window);
iceb_podpis(ff_shet,data->window);
fclose(ff);
fclose(ff_shet);

sprintf(strsql,"%s:%d\n",gettext("Количество листов"),kollist);
iceb_printw(strsql,data->buffer,data->view);







data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Ведомость списания топлива по счетам и водителям"));
data->rk->imaf.plus(imaf_shet);
data->rk->naim.plus(gettext("Cписание топлива по счетам"));

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
