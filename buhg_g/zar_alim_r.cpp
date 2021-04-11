/*$Id: zar_alim_r.c,v 1.21 2013/09/26 09:46:59 sasa Exp $*/
/*19.06.2019	08.12.2006	Белых А.И.	zar_alim_r.c
Расчёт реестра почтовых переводов алиментов
*/
#include <errno.h>
#include "buhg_g.h"

class zar_alim_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  const char *data_r;
  class iceb_u_spisok *imaf;
  class iceb_u_spisok *naimf;
    
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_alim_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_alim_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_alim_r_data *data);
gint zar_alim_r1(class zar_alim_r_data *data);
void  zar_alim_r_v_knopka(GtkWidget *widget,class zar_alim_r_data *data);

extern SQL_baza bd;
extern int kol_strok_na_liste;


int zar_alim_r(const char *data_r,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zar_alim_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);

data.data_r=data_r;
data.imaf=imaf;
data.naimf=naimf;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать реестр почтовых переводов алиментов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_alim_r_key_press),&data);

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

repl.plus(gettext("Распечатать реестр почтовых переводов алиментов"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_alim_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zar_alim_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_alim_r_v_knopka(GtkWidget *widget,class zar_alim_r_data *data)
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

gboolean   zar_alim_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_alim_r_data *data)
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
/*Шапка*/
/**************************************/
void reestal_sap(int *kolstr,FILE *ff)
{

*kolstr+=5;

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n\
 N |          Куди                |   Кому (найменування    |  Сумма  |Плата за|      |Відмітка|        |        |\n\
   |  (повна адреса одержувача)   |   підприємства або      |переказу|пере-ння| ПДВ  |   про  |   N    |Примітка|\n\
   |                              |   прізвище одержувача)  | (грн)  |без ПДВ |(грн) |  інші  |переказу|        |\n\
   |                              |                         |        | (грн)  |      |послуги |        |        |\n\
-----------------------------------------------------------------------------------------------------------------\n");
}

/*******/
/*Шапка*/
/*******/
void            restsp(int *kolstrok,FILE *ff,GtkWidget *wpredok)
{
*kolstrok=0;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"zar_pp_start.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return;
 }

SQL_str row;
SQLCURSOR cur;
class iceb_u_str kod("");
class iceb_u_str naim_org("");
class iceb_u_str adres_org("");
class iceb_u_str stroka("");

sprintf(strsql,"select naikon,adres,kod from Kontragent where kodkon='00'");
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  naim_org.new_plus(row[0]);
  adres_org.new_plus(row[1]);
  kod.new_plus(row[2]);
 }

int nomer_str=0;
class iceb_u_str stroka1("");
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer_str++;
  stroka.new_plus(row_alx[0]);
  switch(nomer_str)
   {
    case 1:
      iceb_u_vstav(&stroka,naim_org.ravno(),0,66,1);
      break;

    case 4:
      iceb_u_vstav(&stroka,kod.ravno(),21,42,2);
      break;

    case 10:
//      sprintf(stroka1,"%s %s",naim_org.ravno(),adres_org.ravno());
      stroka1.new_plus(naim_org.ravno());
      stroka1.plus(" ",adres_org.ravno());
      iceb_u_vstav(&stroka,stroka1.ravno(),11,119,1);
      break;
   }
  *kolstrok+=1;   
  fprintf(ff,"%s",stroka.ravno());
 }

reestal_sap(kolstrok,ff);

}

/*******/
/*Конец*/
/*******/
void konre(double razom,double pdv,double vsego,double p_zbir,
FILE *ff,GtkWidget *wpredok)
{

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%7.2f %8.2f %6.2f\n",
iceb_u_kolbait(60,gettext("Итого")),gettext("Итого"),razom,p_zbir,pdv);

class iceb_u_str stroka("");
char stroka1[1024];

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[2048];
const char *imaf_alx={"zar_pp_end.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return;
 }

SQL_str row;
SQLCURSOR cur;

class iceb_u_str fio_ruk("");

iceb_poldan("Табельный номер руководителя",stroka1,"zarnast.alx",wpredok);

if(stroka1[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",stroka1);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
    fio_ruk.new_plus(row[0]);
 } 

class iceb_u_str fio_buh("");

iceb_poldan("Табельный номер бухгалтера",stroka1,"zarnast.alx",wpredok);

if(stroka1[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",stroka1);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
    fio_buh.new_plus(row[0]);
 } 


int nomer_str=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomer_str++;
  stroka.new_plus(row_alx[0]);
  switch(nomer_str)
   {
    case 1:
      iceb_u_preobr(razom,stroka1,0);
      iceb_u_vstav(&stroka,stroka1,25,77,1);
      break;

    case 2:
      memset(stroka1,'\0',sizeof(stroka1));
      iceb_u_preobr(pdv,stroka1,0);
      iceb_u_vstav(&stroka,stroka1,25,77,1);
      break;

    case 3:
      memset(stroka1,'\0',sizeof(stroka1));
      iceb_u_preobr(vsego,stroka1,0);
      iceb_u_vstav(&stroka,stroka1,25,85,1);
      break;

    case 7:
      iceb_u_vstav(&stroka,fio_ruk.ravno(),20,85,1);
      break;

    case 10:
      iceb_u_vstav(&stroka,fio_buh.ravno(),20,85,1);
      break;
   }
  fprintf(ff,"%s",stroka.ravno());
 }


}
/*************************************/
/*счетчик строк*/
void reestal_ss(int *kolstrok,FILE *ff)
{
*kolstrok+=1;
if(*kolstrok <= kol_strok_na_liste)
 return;
fprintf(ff,"\f");
*kolstrok=0; 

reestal_sap(kolstrok,ff);
*kolstrok+=1;
}

/******************************************/
/******************************************/

gint zar_alim_r1(class zar_alim_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short mr,gr;
iceb_u_rsdat1(&mr,&gr,data->data_r);





double nds1=iceb_pnds(data->window);

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
const char *imaf_alx={"zaralim.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,data->window);
  return(1);
 }


char imaf[64];
sprintf(imaf,"reestr%d.lst",mr);
FILE *f1;
if((f1 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf1[64];

sprintf(imaf1,"vua%d.lst",mr);
FILE *f2;
if((f2 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

fprintf(f2,"%s\n\n%s\n",
gettext("Ведомость удержания алиментов"),iceb_get_pnk("00",0,data->window));

int kolstrok=0;
restsp(&kolstrok,f1,data->window);

double prsb=0.;
int tabn1=0;
double sal=0,sps=0,osym=0,spr=0,ssb=0,proc=0.;
int tabn=0;
int pnom=0;

double razom=0.,pdv=0.,vsego=0.;
double nds=0.;
double sps_b_nds=0.; //Сумма почтового сбора без НДС
double isps_b_nds=0.; //Итоговая сумма почтового сбора без НДС
double sumps=0;
double sumalim=0;
class iceb_u_str br("");
int kodalim=0;
int kodps=0;
SQL_str row;
class SQLCURSOR curr;
int mpr=0;
class iceb_u_str fam("");
class iceb_u_str adr("");
class iceb_u_str famr("");
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  proc=prsb=sumps=sumalim=0.;

  if(iceb_u_SRAV(row_alx[0],"***",1) == 0)  //Метка нового документа
   {
    konre(razom,pdv,vsego,isps_b_nds,f1,data->window);
    pnom=0;
    osym=spr=ssb=proc=0.;
    razom=pdv=vsego=0.;
    isps_b_nds=0.;

    fprintf(f1,"\f");
    restsp(&kolstrok,f1,data->window);

    continue;
   }

  if(iceb_u_polen(row_alx[0],&br,1,'|') != 0)
    continue;

  if(iceb_u_SRAV("Код удержания алиментов",br.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&kodalim,2,'|');
//    kodalim=(short)atoi(br);
    continue;
   }

  if(iceb_u_SRAV("Код удержания почтового сбора",br.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&kodps,2,'|');
//    kodps=(short)atoi(br);
    continue;
   }

  tabn=br.ravno_atoi();
  if(tabn != 0)
   {
    //Проверяем табельный номер
    sprintf(strsql,"select tabn from Kartb where tabn=%d",tabn);
    if(iceb_sql_readkey(strsql,&row,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %d !\n",gettext("Не найден табельный номер"),tabn);
      iceb_menu_soob(strsql,data->window);
      continue;
     }     
   }
  if(tabn != 0 && tabn != tabn1)
   {
    tabn1=tabn;
   }

  if(tabn != 0)
    mpr=0;

  iceb_u_polen(row_alx[0],&proc,2,'|');
//  proc=iceb_u_atof(br);

  iceb_u_polen(row_alx[0],&prsb,3,'|');
//  prsb=iceb_u_atof(br);

  /*Получает на месте*/
  if(tabn != 0 && prsb == 0.)
    mpr=1;

  if(mpr == 1)
    continue;
  iceb_u_polen(row_alx[0],&fam,4,'|');

  iceb_u_polen(row_alx[0],&adr,5,'|');
  if(tabn != 0)
   {
    
    if(kodalim == 0)
     {
      iceb_menu_soob(gettext("В настоечном файле не найден код алиментов !"),data->window);
      break;      
     }
    sprintf(strsql,"select fio from Kartb where tabn=%d",tabn);
    if(iceb_sql_readkey(strsql,&row,&curr,data->window) != 1)
     {
      sprintf(strsql,"%s %d !\n",gettext("Не найден табельный номер"),tabn);
      iceb_menu_soob(strsql,data->window);
       continue;
     }
    
    famr.new_plus(row[0]);
    
    sprintf(strsql,"%5d %-*.*s\n",
    tabn,
    iceb_u_kolbait(20,fam.ravno()),iceb_u_kolbait(20,fam.ravno()),fam.ravno());
    iceb_printw(strsql,data->buffer,data->view);

    fprintf(f2,"\
----------------------------------------------------------------\n\
%5d %s -> %s  \n",tabn,famr.ravno(),fam.ravno());

    
    /*Читаем алименты*/
    sumalim=0.;
    sprintf(strsql,"select SUM(suma) from Zarp where datz >= '%d-%d-01' \
and datz <= '%d-%d-31' and tabn=%d and prn='2' \
and knah=%d",gr,mr,gr,mr,tabn,kodalim); 
    if(iceb_sql_readkey(strsql,&row,&curr,data->window) >= 1)
      if(row[0] != NULL)
       sumalim=atof(row[0])*-1;      

    if(sumalim == 0.)
     {
      tabn=tabn1=0;
      continue;
     }      

    sumps=0.;
    sps_b_nds=0.;
    /*Читаем почтовый сбор*/
    sprintf(strsql,"select SUM(suma) from Zarp where datz >= '%d-%d-01' \
and datz <= '%d-%d-31' and tabn=%d and prn='2'\
and knah=%d",gr,mr,gr,mr,tabn,kodps); 
    if(iceb_sql_readkey(strsql,&row,&curr,data->window) >= 1)
     {
      if(row[0] != NULL)
       {
        sumps=atof(row[0])*-1;      
        if(sumps != 0.)
         {
          sps_b_nds=sumps-sumps*nds1/(100.+nds1);
          sps_b_nds=iceb_u_okrug(sps_b_nds,0.01);
         }
       }
     }
//    if(sumps == 0.)
//     continue;
     

    nds=sumps-sps_b_nds;
    nds=iceb_u_okrug(nds,0.01);

    pdv+=nds;
    razom+=sumalim;
    vsego+=sumalim+sumps;
    
    reestal_ss(&kolstrok,f1);

    fprintf(f1,"%3d %-*.*s %-*.*s %7.2f %8.2f %6.2f\n",
    ++pnom,
    iceb_u_kolbait(30,adr.ravno()),iceb_u_kolbait(30,adr.ravno()),adr.ravno(),
    iceb_u_kolbait(25,fam.ravno()),iceb_u_kolbait(25,fam.ravno()),fam.ravno(),
    sumalim,sps_b_nds,nds);

    fprintf(f2,"\n\
%*s:%20s (%.2f%%)\n",
    iceb_u_kolbait(30,gettext("Алименты")),gettext("Алименты"),
    iceb_u_prnbr(sumalim),proc);

    fprintf(f2,"\
%*s:%20s\n",
    iceb_u_kolbait(30,gettext("Почтовый сбор")),gettext("Почтовый сбор"),
    iceb_u_prnbr(sumps));

    osym+=sumalim;
    spr+=sumalim;
    sps+=sumps;
    ssb+=sumps;
    sal+=sumalim;
    isps_b_nds+=sps_b_nds;
   }
  else
   if(tabn1 != 0)
    {
     reestal_ss(&kolstrok,f1);
     fprintf(f1,"%3s %-*.*s %-*.*s\n",
     " ",
     iceb_u_kolbait(30,adr.ravno()),iceb_u_kolbait(30,adr.ravno()),adr.ravno(),
     iceb_u_kolbait(25,fam.ravno()),iceb_u_kolbait(25,fam.ravno()),fam.ravno());
    }
 }
sprintf(strsql,"\n-------------------------------------------------------\n\
%-*s:%20s\n",
iceb_u_kolbait(20,gettext("Итого алиментов")),gettext("Итого алиментов"),
iceb_u_prnbr(sal));

iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%-*s:%20s\n\n",
iceb_u_kolbait(20,gettext("Итого почтовый сбор")),gettext("Итого почтовый сбор"),iceb_u_prnbr(sps));

iceb_printw(strsql,data->buffer,data->view);

fprintf(f2,"-----------------------------------------------------------------\n\
%-*s:%20s\n",
iceb_u_kolbait(20,gettext("Итого алиментов")),gettext("Итого алиментов"),iceb_u_prnbr(sal));

fprintf(f2,"\
%-*s:%20s\n",
iceb_u_kolbait(20,gettext("Итого почтовый сбор")),gettext("Итого почтовый сбор"),iceb_u_prnbr(sps));

konre(razom,pdv,vsego,isps_b_nds,f1,data->window);


iceb_podpis(f1,data->window);
iceb_podpis(f2,data->window);

fclose(f1);
fclose(f2);

data->imaf->plus(imaf);
data->imaf->plus(imaf1);
data->naimf->plus(gettext("Реестр почтовых переводов алиментов"));
data->naimf->plus(gettext("Ведомость удержания алиментов"));

for(int nom=0; nom < data->imaf->kolih(); nom++)
 iceb_ustpeh(data->imaf->ravno(nom),0,data->window);


data->kon_ras=0;
data->voz=0;
gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

return(FALSE);

}
