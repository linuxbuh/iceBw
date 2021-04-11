/*$Id: musspiw_r.c,v 1.13 2013/09/26 09:46:52 sasa Exp $*/
/*26.02.2017	26.02.2017	Белых А.И.	musspiw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "musspiw.h"

class musspiw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class musspiw_rr *rk;
  class spis_oth *oth;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  musspiw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   musspiw_r_key_press(GtkWidget *widget,GdkEventKey *event,class musspiw_r_data *data);
gint musspiw_r1(class musspiw_r_data *data);
void  musspiw_r_v_knopka(GtkWidget *widget,class musspiw_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;

int musspiw_r(class spis_oth *oth,class musspiw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class musspiw_r_data data;

data.rk=datark;
data.name_window.plus(__FUNCTION__);
data.oth=oth;
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Материальный отчёт"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(musspiw_r_key_press),&data);

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

repl.plus(gettext("Материальный отчёт"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(musspiw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)musspiw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  musspiw_r_v_knopka(GtkWidget *widget,class musspiw_r_data *data)
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

gboolean   musspiw_r_key_press(GtkWidget *widget,GdkEventKey *event,class musspiw_r_data *data)
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

/*********************************/
/*печать реквизитов поиска*/
/*********************************/

void musspi_prp(class musspiw_rr *rp,FILE *ff)
{

if(rp->sklad.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Склад"),rp->sklad.ravno());
if(rp->grup_mat.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Группа материала"),rp->grup_mat.ravno());
if(rp->kodmat.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код материала"),rp->kodmat.ravno());
if(rp->shetz.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счёт"),rp->shetz.ravno());

}
/***************/
/*печать шапки свода по материалам*/
/***************/
void musspi_sapkm(int nom_str,int *kolstr,FILE *ff,GtkWidget *wpredok)
{

if(kolstr != NULL)
 *kolstr+=4;

fprintf(ff,"%80s %s N%d\n","",gettext("Страница"),nom_str);

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код м.|   Наименование материала     |Код д.|   Наименование детали        |Склад|Ном.к.|Ед.изм.|   Цена   |Количество|   Сумма  |\n"));

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
}
/***************/
/*печать шапки свода по деталям*/
/***************/
void musspi_sapkd(int nom_str,int *kolstr,FILE *ff,GtkWidget *wpredok)
{

if(kolstr != NULL)
 *kolstr+=4;

fprintf(ff,"%80s %s N%d\n","",gettext("Страница"),nom_str);

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код д.|   Наименование детали        |Код м.|   Наименование материала     |Склад|Ном.к.|Ед.изм.|   Цена   |Количество|   Сумма  |\n"));

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
}
/***************/
/*печать шапки списка документов*/
/***************/
void musspi_sd(int nom_str,int *kolstr,FILE *ff,GtkWidget *wpredok)
{

if(kolstr != NULL)
 *kolstr+=4;

fprintf(ff,"%90s %s N%d\n","",gettext("Страница"),nom_str);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Дата докум|Номер док.|Склад|Ном к.|Код м.|   Наименование материала     |Код д.|   Наименование детали        |Ед.изм.|   Цена   |Количество|   Сумма  |\n"));

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}
/***************/
/*печать шапки свода списания по изделиям*/
/***************/
void musspi_iz(int nom_str,int *kolstr,FILE *ff,GtkWidget *wpredok)
{

if(kolstr != NULL)
 *kolstr+=4;

fprintf(ff,"%90s %s N%d\n","",gettext("Страница"),nom_str);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код м.|   Наименование материала     |Код д.|   Наименование детали        |   Цена   |Количество|   Сумма  |\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------\n");
}
/****************************************/
/*проверка реквизитов поиска*/
/**************************************/
int musspi_prov(SQL_str row,class musspiw_rr *rekp,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row1;
class SQLCURSOR cur1;

if(iceb_u_proverka(rekp->sklad.ravno(),row[1],0,0) != 0)
  return(1);

if(iceb_u_proverka(rekp->kodmat.ravno(),row[3],0,0) != 0)
  return(1);

if(rekp->shetz.getdlinna() > 1)
 {
  sprintf(strsql,"seslect shetu from Kart where sklad=%s and nomk=%s",row[1],row[4]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   if(iceb_u_proverka(rekp->shetz.ravno(),row1[0],1,0) != 0)
    return(1);
 }
 
if(rekp->grup_mat.getdlinna() > 1)
 {
  sprintf(strsql,"select kodgr from Material where kodm=%s",row[3]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   if(iceb_u_proverka(rekp->grup_mat.ravno(),row1[0],0,0) != 0)
    return(1);
 }
return(0);

}
/***************************/
/*узнаём наименования*/
/***************************/
void musspi_naim(SQL_str row,class iceb_u_str *naim_km,class iceb_u_str *naim_kd,class iceb_u_str *ei,double *cena,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row1;
class SQLCURSOR cur1;
/*узнаём наименование материала*/
sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[3]));
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
 naim_km->new_plus(row1[0]);
else
 naim_km->new_plus("");

/*узнаём наименование детали*/
sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[5]));
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
 naim_kd->new_plus(row1[0]);
else
 naim_kd->new_plus("");
 

/*узнаём единицу измерения*/
sprintf(strsql,"select ei,cena from Kart where sklad=%s and nomk=%s",row[1],row[4]);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
 {
  ei->new_plus(row1[0]);
  *cena=atof(row1[1]);
 }
else
 {
  ei->new_plus("");
  *cena=0.;
 }
}

/******************************************/
/******************************************/

gint musspiw_r1(class musspiw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);

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



SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;

sprintf(strsql,"select * from Dokummat4 where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by km asc,nk asc,kd asc",gn,mn,dn,gk,mk,dk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
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
class iceb_u_str naim_km("");
class iceb_u_str naim_kd("");
class iceb_u_str ei("");

class iceb_fopen fkm;

char imaf_km[64];
sprintf(imaf_km,"sspim%d.lst",getpid());

if(fkm.start(imaf_km,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_zagolov(gettext("Свод списания по материалам"),dn,mn,gn,dk,mk,gk,fkm.ff,data->window);

musspi_prp(data->rk,fkm.ff);


musspi_sapkm(1,NULL,fkm.ff,data->window);




double kolih_mat=0.;
double kolih_zap=0.;
int kod_mat=0;
int kod_matz=0;
int kolv=0;
float kolstr1=0;
double cena=0.;
double suma=0.;
double itogo_suma=0.;
double suma_mat=0.;


while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
//  strzag(LINES-1,0,kolstr,++kolstr1);

  if(musspi_prov(row,data->rk,data->window) != 0)
   continue;

  /*узнаём наименования*/ 
  musspi_naim(row,&naim_km,&naim_kd,&ei,&cena,data->window);


  kod_mat=atoi(row[3]);

  if(kod_mat != kod_matz)
   {
    if(kod_matz != 0)
     {
      if(kolv > 1)
       {
        sprintf(strsql,"%s %s",gettext("Итого по материалу"),row[3]);
        fprintf(fkm.ff,"%*s:%10.6g,%10.2f\n",iceb_u_kolbait(107,strsql),strsql,kolih_mat,suma_mat);
       }
      suma_mat=0.;
      kolih_mat=0.;
      kolv=0;
     }
    
    kod_matz=kod_mat;
    
   }  

  kolih_zap=atof(row[6]);
  
  kolih_mat+=kolih_zap;

  suma=cena*kolih_zap;
  suma=iceb_u_okrug(suma,0.01);

  itogo_suma+=suma;
  suma_mat+=suma;
        
  if(kolv == 0)
   fprintf(fkm.ff,"%-6s %-*.*s %-6s %-*.*s %-5s %-6s %*s %10.10g %10.10g %10.2f\n",
   row[3],
   iceb_u_kolbait(30,naim_km.ravno()),
   iceb_u_kolbait(30,naim_km.ravno()),
   naim_km.ravno(),
   row[5],
   iceb_u_kolbait(30,naim_kd.ravno()),
   iceb_u_kolbait(30,naim_kd.ravno()),
   naim_kd.ravno(),  
   row[1],
   row[4],
   iceb_u_kolbait(7,ei.ravno()),
   ei.ravno(),  
   cena,
   kolih_zap,
   suma);
  else
   fprintf(fkm.ff,"%-6s %-30.30s %-6s %-*.*s %-5s %-6s %*s %10.10g %10.10g %10.2f\n",
   "",
   "",
   row[5],
   iceb_u_kolbait(30,naim_kd.ravno()),
   iceb_u_kolbait(30,naim_kd.ravno()),
   naim_kd.ravno(),  
   row[1],
   row[4],
   iceb_u_kolbait(7,ei.ravno()),
   ei.ravno(),  
   cena,
   kolih_zap,
   suma);
  
  kolv++;


 }

fprintf(fkm.ff,"ICEB_LST_END\n");

if(kolv > 1)
 {
  sprintf(strsql,"%s %d",gettext("Итого по материалу"),kod_mat);
  fprintf(fkm.ff,"%*s:%10.6g %10.2f\n",iceb_u_kolbait(107,strsql),strsql,kolih_mat,suma_mat);
 }


fprintf(fkm.ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(fkm.ff,"%*s:%10.2f\n",
iceb_u_kolbait(118,gettext("Общий итог")),
gettext("Общий итог"),
itogo_suma);

iceb_podpis(fkm.ff,data->window);
fkm.end();

/*****************************************************/
sprintf(strsql,"select * from Dokummat4 where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by kd asc,km asc",gn,mn,dn,gk,mk,dk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }






class iceb_tmptab tabtmp;
const char *imatmptab={"sspi"};
char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
iz INT NOT NULL DEFAULT 0,\
km INT NOT NULL DEFAULT 0,\
kd INT NOT NULL DEFAULT 0,\
nm CHAR(100) NOT NULL DEFAULT '',\
nd CHAR(100) NOT NULL DEFAULT '',\
cena DOUBLE(15,6) NOT NULL DEFAULT 0.,\
kol DOUBLE(15,6) NOT NULL DEFAULT 0.,\
unique(iz,km,kd,cena)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  







char imaf_kd[64];
sprintf(imaf_kd,"sspid%d.lst",getpid());

if(fkm.start(imaf_kd,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_zagolov(gettext("Свод списания по деталям"),dn,mn,gn,dk,mk,gk,fkm.ff,data->window);

musspi_prp(data->rk,fkm.ff);

musspi_sapkd(1,NULL,fkm.ff,data->window);
int kod_det=0;
int kod_detz=0;
kolstr1=0;
kolv=0;
kolih_mat=0.;
itogo_suma=0.;
suma_mat=0.;
int kod_izdel=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
//  strzag(LINES-1,0,kolstr,++kolstr1);

  if(musspi_prov(row,data->rk,data->window) != 0)
   continue;

  /*узнаём наименования*/ 
  musspi_naim(row,&naim_km,&naim_kd,&ei,&cena,data->window);




  kod_det=atoi(row[5]);
  kod_mat=atoi(row[3]);
  
  if(kod_det != kod_detz)
   {
    if(kod_detz != 0)
     {
      if(kolv > 1)
       {
        sprintf(strsql,"%s %s",gettext("Итого по материалу"),row[3]);
        fprintf(fkm.ff,"%*s:%10.6g %10.2f\n",iceb_u_kolbait(107,strsql),strsql,kolih_mat,suma_mat);
       }

      /*узнаём в какое изделие входит*/
      sprintf(strsql,"select kodi from Specif where kodd=%d limit 1",kod_det);
      if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
        kod_izdel=atoi(row1[0]);
      else
        kod_izdel=0.;
               

      suma_mat=0.;
      kolih_mat=0.;
      kolv=0;
     }
    
    kod_detz=kod_det;
    
   }  
  
  kolih_zap=atof(row[6]);
  
  kolih_mat+=kolih_zap;

  suma=cena*kolih_zap;
  suma=iceb_u_okrug(suma,0.01);

  itogo_suma+=suma;
  suma_mat+=suma;

  if(kolv == 0)
   fprintf(fkm.ff,"%-6s %-*.*s %-6s %-*.*s %-5s %-6s %*s %10.10g %10.6g %10.2f\n",
   row[5],
   iceb_u_kolbait(30,naim_kd.ravno()),
   iceb_u_kolbait(30,naim_kd.ravno()),
   naim_kd.ravno(),
   row[3],
   iceb_u_kolbait(30,naim_km.ravno()),
   iceb_u_kolbait(30,naim_km.ravno()),
   naim_km.ravno(),  
   row[1],
   row[4],
   iceb_u_kolbait(7,ei.ravno()),
   ei.ravno(),  
   cena,
   kolih_zap,
   suma);
  else
   fprintf(fkm.ff,"%-6s %-30.30s %-6s %-*.*s %-5s %-6s %*s %10.10g %10.6g %10.2f\n",
   "",
   "",
   row[3],
   iceb_u_kolbait(30,naim_km.ravno()),
   iceb_u_kolbait(30,naim_km.ravno()),
   naim_km.ravno(),  
   row[1],
   row[4],
   iceb_u_kolbait(7,ei.ravno()),
   ei.ravno(),  
   cena,
   kolih_zap,
   suma);
  
  kolv++;


  /*проверяем есть ли запись в таблице*/
  sprintf(strsql,"select kol from %s where iz=%d and km=%d and kd=%d and cena=%.10g",imatmptab,kod_izdel,kod_mat,kod_det,cena);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    kolih_zap+=atof(row1[0]);
    sprintf(strsql,"update %s set kol=%.6g where iz=%d and km=%d and kd=%d",
    imatmptab,
    kolih_zap,
    kod_izdel,
    kod_mat,
    kod_det);

    iceb_sql_zapis(strsql,1,0,data->window);
   }
  else
   {
    /*записываем во временную талицу*/
  
    sprintf(strsql,"insert into %s values(%d,%d,%d,'%s','%s',%.6g,%.6g)",
    imatmptab,
    kod_izdel,
    atoi(row[3]),
    atoi(row[5]),
    naim_km.ravno_filtr(),
    naim_kd.ravno_filtr(),
    cena,
    kolih_zap);

    iceb_sql_zapis(strsql,1,0,data->window);
  }


 }
fprintf(fkm.ff,"ICEB_LST_END\n");

if(kolv > 1)
 {
  sprintf(strsql,"%s %d",gettext("Итого по детали"),kod_det);
  fprintf(fkm.ff,"%*s:%10.6g\n",iceb_u_kolbait(107,strsql),strsql,kolih_mat);
 }
fprintf(fkm.ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(fkm.ff,"%*s:%10.2f\n",
iceb_u_kolbait(118,gettext("Общий итог")),
gettext("Общий итог"),
itogo_suma);

iceb_podpis(fkm.ff,data->window);
fkm.end();


/**********************************************************************/

sprintf(strsql,"select * from Dokummat4 where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' order by datd asc,skl asc,nomd asc",gn,mn,dn,gk,mk,dk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_dok[64];
sprintf(imaf_dok,"sspidok%d.lst",getpid());

if(fkm.start(imaf_dok,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_zagolov(gettext("Список документов списания"),dn,mn,gn,dk,mk,gk,fkm.ff,data->window);

musspi_prp(data->rk,fkm.ff);

musspi_sd(1,NULL,fkm.ff,data->window);
kolstr1=0;
kolv=0;
class iceb_u_str data_dok("");
class iceb_u_str nomer_dok("");
itogo_suma=0.;
suma_mat=0.;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
//  strzag(LINES-1,0,kolstr,++kolstr1);

  if(musspi_prov(row,data->rk,data->window) != 0)
   continue;

  /*узнаём наименования*/ 
//  musspi_naim(row,&naim_km,&naim_kd,&ei,&cena);
  musspi_naim(row,&naim_km,&naim_kd,&ei,&cena,data->window);

  if(iceb_u_SRAV(data_dok.ravno(),row[0],0) != 0)
   {
    data_dok.new_plus(row[0]);
    kolv=0;
   }

  if(iceb_u_SRAV(nomer_dok.ravno(),row[2],0) != 0)
   {
    nomer_dok.new_plus(row[2]);
    kolv=0;
   }
  
  kolih_zap=atof(row[6]);
  
  kolih_mat+=kolih_zap;

  suma=cena*kolih_zap;
  suma=iceb_u_okrug(suma,0.01);

  itogo_suma+=suma;

  if(kolv == 0)
   fprintf(fkm.ff,"%10s %-*.*s %-5s %-6s %-6s %-*.*s %-6s %-*.*s %*s %10.10g %10.6g %10.2f\n",
   iceb_u_datzap(row[0]),
   iceb_u_kolbait(10,row[2]),
   iceb_u_kolbait(10,row[2]),
   row[2],
   row[1],
   row[4],
   row[3],
   iceb_u_kolbait(30,naim_km.ravno()),
   iceb_u_kolbait(30,naim_km.ravno()),
   naim_km.ravno(),
   row[5],
   iceb_u_kolbait(30,naim_kd.ravno()),
   iceb_u_kolbait(30,naim_kd.ravno()),
   naim_kd.ravno(),
   iceb_u_kolbait(7,ei.ravno()),
   ei.ravno(),
   cena,
   kolih_zap,
   suma);  
  else
   fprintf(fkm.ff,"%10s %10s %-5s %-6s %-6s %-*.*s %-6s %-*.*s %*s %10.10g %10.6g %10.2f\n",
   "",
   "",
   row[1],
   row[4],
   row[3],
   iceb_u_kolbait(30,naim_km.ravno()),
   iceb_u_kolbait(30,naim_km.ravno()),
   naim_km.ravno(),
   row[5],
   iceb_u_kolbait(30,naim_kd.ravno()),
   iceb_u_kolbait(30,naim_kd.ravno()),
   naim_kd.ravno(),
   iceb_u_kolbait(7,ei.ravno()),
   ei.ravno(),
   cena,
   kolih_zap,
   suma);  

  kolv++;
  
 }
fprintf(fkm.ff,"ICEB_LST_END\n");
fprintf(fkm.ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(fkm.ff,"%*s:%10.2f\n",
iceb_u_kolbait(140,gettext("Общий итог")),
gettext("Общий итог"),
itogo_suma);

iceb_podpis(fkm.ff,data->window);
fkm.end();
 



/*распечатка по изделиям*/


sprintf(strsql,"select * from %s order by iz asc,km asc,kd asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf_izd[64];
sprintf(imaf_izd,"sspiiz%d.lst",getpid());

if(fkm.start(imaf_izd,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_zagolov(gettext("Свод списания материалов по изделиям"),dn,mn,gn,dk,mk,gk,fkm.ff,data->window);

musspi_prp(data->rk,fkm.ff);

musspi_iz(1,NULL,fkm.ff,data->window);
kolstr1=0;
kolv=0;
itogo_suma=0.;
int kod_iz=0;
int kod_izz=-1;
double itogo_iz=0.;
double itogo_mat=0.;
kod_matz=0;
class iceb_u_str naim_iz("");
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
//  strzag(LINES-1,0,kolstr,++kolstr1);

  kod_iz=atoi(row[0]);
  kod_mat=atoi(row[1]);
  cena=atof(row[5]);

  if(kod_mat != kod_matz)
   {
    if(kod_matz != 0)
     {
      if(kolv > 1)
       {
        sprintf(strsql,"%s %d",gettext("Итого по материалу"),kod_matz);
        fprintf(fkm.ff,"%*s:%10.2f\n",iceb_u_kolbait(97,strsql),strsql,itogo_mat);
       }
     }    
    itogo_mat=0.;
    kolv=0;
    kod_matz=kod_mat;
   }
  
  if(kod_iz != kod_izz)
   {
    if(kod_izz != -1)
     {
      fprintf(fkm.ff,"%*s:%10.2f\n",iceb_u_kolbait(97,gettext("Итого по изделию")),gettext("Итого по изделию"),itogo_iz);
      itogo_iz=0.;
     }    

    if(kod_iz == 0)
     {
      naim_iz.new_plus(gettext("Детали не введённые в спецификацию ни одного изделия"));
     }
    else
     {
      /*узнаём наименование изделия*/
      sprintf(strsql,"select naimat from Material where kodm=%d",kod_iz);
      if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
       naim_iz.new_plus(row1[0]);
      else
       {
        sprintf(strsql,"%s %d %s",gettext("Изделие"),kod_iz,gettext("не найдено в справочнике материалов"));
        naim_iz.new_plus(strsql);
       }      
     }
    fprintf(fkm.ff,"%s:%d %s\n",gettext("Изделие"),kod_iz,naim_iz.ravno());
    fprintf(fkm.ff,"\
-------------------------------------------------------------------------------------------------------------\n");
    kod_izz=kod_iz;
   }



  kolih_zap=atof(row[6]);
  
  suma=cena*kolih_zap;
  suma=iceb_u_okrug(suma,0.01);
  
  itogo_suma+=suma;
  itogo_iz+=suma;
  itogo_mat+=suma;
          
  fprintf(fkm.ff,"%6s %-*.*s %6s %-*.*s %10.10g %10.10g %10.2f\n",
  row[1],
  iceb_u_kolbait(30,row[3]),
  iceb_u_kolbait(30,row[3]),
  row[3],
  row[2],
  iceb_u_kolbait(30,row[4]),
  iceb_u_kolbait(30,row[4]),
  row[4],
  cena,
  kolih_zap,
  suma);    

  kolv++;
  
 }
fprintf(fkm.ff,"ICEB_LST_END\n");

if(kolv > 1)
 {
  sprintf(strsql,"%s %d",gettext("Итого по материалу"),kod_mat);
  fprintf(fkm.ff,"%*s:%10.2f\n",iceb_u_kolbait(97,strsql),strsql,itogo_mat);
 }

fprintf(fkm.ff,"%*s:%10.2f\n",iceb_u_kolbait(97,gettext("Итого по изделию")),gettext("Итого по изделию"),itogo_iz);

fprintf(fkm.ff,"\
-------------------------------------------------------------------------------------------------------------\n");

fprintf(fkm.ff,"%*s:%10.2f\n",
iceb_u_kolbait(97,gettext("Общий итог")),
gettext("Общий итог"),
itogo_suma);

iceb_podpis(fkm.ff,data->window);
fkm.end();



int orient=iceb_ustpeh(imaf_km,3,data->window);
iceb_rnl(imaf_km,orient,NULL,&musspi_sapkm,data->window);

orient=iceb_ustpeh(imaf_kd,3,data->window);
iceb_rnl(imaf_kd,orient,NULL,&musspi_sapkd,data->window);

orient=iceb_ustpeh(imaf_dok,3,data->window);
iceb_rnl(imaf_dok,orient,NULL,&musspi_sd,data->window);

orient=iceb_ustpeh(imaf_izd,3,data->window);
iceb_rnl(imaf_izd,orient,NULL,&musspi_iz,data->window);


data->oth->spis_imaf.plus(imaf_km);
data->oth->spis_naim.plus(gettext("Cвод списания по материалам"));
data->oth->spis_imaf.plus(imaf_kd);
data->oth->spis_naim.plus(gettext("Cвод списания по деталям"));
data->oth->spis_imaf.plus(imaf_dok);
data->oth->spis_naim.plus(gettext("Cписок документов списания"));
data->oth->spis_imaf.plus(imaf_izd);
data->oth->spis_naim.plus(gettext("Cвод списания материалов по изделиям"));


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
