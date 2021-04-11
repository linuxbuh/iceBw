/*$Id: opss_uw_r.c,v 1.22 2013/09/26 09:46:52 sasa Exp $*/
/*11.01.2018	24.12.2005	Белых А.И. 	opss_uw_r.c
Расчёт отчёта по счетам списания услуг
*/
#include <errno.h>
#include "buhg_g.h"
#include "opss_uw.h"

class opss_uw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class opss_uw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  opss_uw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   opss_uw_r_key_press(GtkWidget *widget,GdkEventKey *event,class opss_uw_r_data *data);
gint opss_uw_r1(class opss_uw_r_data *data);
void  opss_uw_r_v_knopka(GtkWidget *widget,class opss_uw_r_data *data);



extern SQL_baza bd;




extern int kol_strok_na_liste;
extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление суммы*/

int opss_uw_r(class opss_uw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class opss_uw_r_data data;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);
if(data.rk->metka == 1)
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать движение услуг по счетам получения"));
if(data.rk->metka == 2)
 sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать движение услуг по счетам списания"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(opss_uw_r_key_press),&data);

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

if(data.rk->metka == 1)
 repl.plus(gettext("Распечатать движение услуг по счетам получения"));
if(data.rk->metka == 2)
 repl.plus(gettext("Распечатать движение услуг по счетам списания"));

sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,data.window));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(opss_uw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


g_idle_add((GSourceFunc)opss_uw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  opss_uw_r_v_knopka(GtkWidget *widget,class opss_uw_r_data *data)
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

gboolean   opss_uw_r_key_press(GtkWidget *widget,GdkEventKey *event,class opss_uw_r_data *data)
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
/********************************/
/*шапка*/
/**********************************/

void opss_h(int *nomlist,int *kol_str,FILE *ff)
{
*nomlist+=1;
fprintf(ff,"%110s%s N%d\n","",gettext("Лист"),*nomlist);
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
  Код   |    Наименование    |Счёт спис.|   Счёт   |   Сумма  |Един. |Количec-|   Цена   |   Дата   |Номер    |Склад|Опера-|Контра-|\n\
материа.|     материалла     |/получения|  учёта   |          |измер.|  тво   |          | документа|документа|     |ция   |гент   |\n");

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------------\n");
*kol_str+=5;


}

/*****************************/
/*Выдача итога*/
/*****************************/

void opss_itogo(int metka, //1-счёт учёта 2-счёт списания
const char *kod,double itog,FILE *ff,FILE *ffi)
{
if(itog == 0.)
 return;
 
char bros[512];
if(metka == 1)
  sprintf(bros,"%s %s",gettext("Итого по счёту учёта"),kod);
if(metka == 2)
 {
  sprintf(bros,"%s %s",gettext("Итого по счёту списания"),kod);
  fprintf(ffi,"%-10s %10.2f\n",kod,itog);
 }
if(metka == 3)
 {
  sprintf(bros,"%s",gettext("Общий итог"));
  fprintf(ffi,"%*.*s %10.2f\n",
  iceb_u_kolbait(10,gettext("Итого")),
  iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),itog);
 }
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(51,bros),bros,itog);

}
/************************/
/*Счётчик строк*/
/*************************/

void opps_shet_str(int *nomlist,int *kol_str,FILE *ff)
{
*kol_str+=1;
if(*kol_str <= kol_strok_na_liste)
 return;

fprintf(ff,"\f");
*kol_str=0;
opss_h(nomlist,kol_str,ff);
*kol_str+=1;
}

/**************************************/
/*Распечатываем свод по услугам*/
/******************************/
void opss_u_svod(FILE *ff,class iceb_u_spisok *shet_kodzap,
class iceb_u_double *shet_kodzap_suma,
class iceb_u_double *shet_kodzap_kolih,
GtkWidget *wpredok)
{

fprintf(ff,"\
-------------------------------------------------------------\n");

fprintf(ff,gettext("\
  Код   |    Наименование    |Счёт спис.|   Сумма  |Количec-|\n\
 услуги |      услуги        |/получения|          |  тво   |\n"));

/*
12345678 12345678901234567890 1234567890 1234567890 12345678
*/

fprintf(ff,"\
-------------------------------------------------------------\n");

class iceb_u_str kodzap("");
class iceb_u_str shet_spis("");
double suma;
double kolih;
class iceb_u_str naim("");
SQL_str row1;
SQLCURSOR cur1;
char strsql[512];
double itogo_suma=0.;
double itogo_kolih=0.;
class iceb_u_str shet_zap("");
double itogo_suma_shet=0.;
double itogo_kolih_shet=0.;
for(int i=0; i < shet_kodzap->kolih(); i++)
 {
  iceb_u_polen(shet_kodzap->ravno(i),&shet_spis,1,'|');
  iceb_u_polen(shet_kodzap->ravno(i),&kodzap,2,'|');

  if(iceb_u_SRAV(shet_zap.ravno(),shet_spis.ravno(),0) != 0)
   {
    if(shet_zap.ravno()[0] != '\0')
     {
      sprintf(strsql,"%s %s",gettext("Итого по счёту"),shet_zap.ravno());
      fprintf(ff,"%40s:%10.2f %8.2f\n",strsql,itogo_suma_shet,itogo_kolih_shet);
     }
    itogo_suma_shet=itogo_kolih_shet=0.;
    shet_zap.new_plus(shet_spis.ravno());
    
   }

  //читаем материал
  naim.new_plus("");
  sprintf(strsql,"select naius from Uslugi where kodus=%s",kodzap.ravno());
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   {
    naim.new_plus(row1[0]);
   }
  else
   {
    sprintf(strsql,"select naimat from Material where kodm=%s",kodzap.ravno());
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
      naim.new_plus(row1[0]);
   } 
  suma=shet_kodzap_suma->ravno(i);
  kolih=shet_kodzap_kolih->ravno(i);
  
  itogo_suma+=suma;
  itogo_kolih+=kolih;
  
  itogo_suma_shet+=suma;
  itogo_kolih_shet+=kolih;
    
  fprintf(ff,"%-8s %-*.*s %-*s %10.2f %8.2f\n",
  kodzap.ravno(),
  iceb_u_kolbait(20,naim.ravno()),iceb_u_kolbait(20,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(10,shet_spis.ravno()),shet_spis.ravno(),
  suma,kolih);

  for(int nom=20; nom < iceb_u_strlen(naim.ravno()); nom+=20)
   {
    fprintf(ff,"%8s %-*.*s\n",
    "",
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim.ravno())),
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim.ravno())),
    iceb_u_adrsimv(nom,naim.ravno()));
   }

 }

sprintf(strsql,"%s %s",gettext("Итого по счёту"),shet_spis.ravno());
fprintf(ff,"%*s:%10.2f %8.2f\n",
iceb_u_kolbait(40,strsql),strsql,
itogo_suma_shet,itogo_kolih_shet);

fprintf(ff,"\
-------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f %8.2f\n",iceb_u_kolbait(40,gettext("Итого")),gettext("Итого"),itogo_suma,itogo_kolih);

iceb_podpis(ff,wpredok);

}

/******************************************/
/******************************************/

gint opss_uw_r1(class opss_uw_r_data *data)
{
iceb_clock sss(data->window);
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;


short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s %d.%d.%d %s %d.%d.%d\n",gettext("Период с"),dn,mn,gn,gettext("по"),
dk,mk,gk);

iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"select datd,podr,nomd,metka,kodzap,kolih,cena,ei,shsp,shetu,nz from Usldokum1 \
where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and tp=%d order by shsp asc",
gn,mn,dn,gk,mk,dk,data->rk->metka);

int kolstr;
SQL_str row;
SQLCURSOR cur;
SQL_str row1;
SQLCURSOR cur1;

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

class iceb_tmptab tabtmp;
const char *imatmptab={"opssuw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shsp char(24) not null,\
shuh char(24) not null,\
datd DATE not null,\
podr int not null,\
nomd char(24) not null,\
kodzap int not null,\
kolih double(16,6) not null,\
cena double(16,6) not null,\
ei char(24) not null,\
kodop char(24) not null,\
kontr char(24) not null,\
naim char(112) not null) ENGINE = MYISAM",imatmptab);

if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  



class iceb_u_str kodop("");
class iceb_u_str kontr("");
class iceb_u_str shet_uheta("");
class iceb_u_str shet_sp("");
double kolih;
short dd,md,gd;
class iceb_u_str naim("");

float kolstr1=0.;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->shet_sp.ravno(),row[8],1,0) != 0)
    continue;  
  if(iceb_u_proverka(data->rk->shet_uh.ravno(),row[9],1,0) != 0)
    continue;  
  if(iceb_u_proverka(data->rk->podr.ravno(),row[1],0,0) != 0)
    continue;  

  if(iceb_u_proverka(data->rk->kodzap.ravno(),row[4],0,0) != 0)
    continue;  

  //читаем материал
  naim.new_plus("");
  if(row[3][0] == '0')
    sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",row[4]);
  if(row[3][0] == '1')
    sprintf(strsql,"select kodgr,naius from Uslugi where kodus=%s",row[4]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    naim.new_plus(row1[1]);
    if(iceb_u_proverka(data->rk->kodgr.ravno(),row1[0],0,0) != 0)
      continue;  

   }

  iceb_u_rsdat(&dd,&md,&gd,row[0],2);
  kolih=readkoluw(dd,md,gd,atoi(row[1]),row[2],data->rk->metka,atoi(row[3]),atoi(row[4]),atoi(row[10]),data->window);
  if(kolih == 0.)
   continue;  

  kodop.new_plus("");
  kontr.new_plus("");
  
  //читаем шапку документа
  sprintf(strsql,"select kontr,kodop from Usldokum where datd='%s' and podr=%s and \
nomd='%s' and tp=%d",row[0],row[1],row[2],data->rk->metka);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(iceb_u_proverka(data->rk->kodop.ravno(),row1[1],0,0) != 0)
      continue;  
    if(iceb_u_proverka(data->rk->kontr.ravno(),row1[0],0,0) != 0)
      continue;  
    kodop.new_plus(row1[1]);
    kontr.new_plus(row1[0]);
   }

  shet_sp.new_plus(row[8]);
  if(shet_sp.ravno()[0] == '\0')
   shet_sp.new_plus("???");


  sprintf(strsql,"insert into %s values ('%s','%s','%s',%s,'%s',%s,%.10g,%s,'%s','%s','%s','%s')",
  imatmptab,
  shet_sp.ravno(),row[9],
  row[0],row[1],row[2],row[4],kolih,row[6],row[7],
  kodop.ravno(),kontr.ravno(),naim.ravno_filtr());

  iceb_sql_zapis(strsql,1,0,data->window);    



 }




sprintf(strsql,"select * from %s order by shsp asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }





char imaf[64];
FILE *ff;

sprintf(imaf,"opss%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int kol_str=0;

char imafi[64];
FILE *ffi;

sprintf(imafi,"opssi%d.lst",getpid());

if((ffi = fopen(imafi,"w")) == NULL)
 {
  iceb_er_op_fil(imafi,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf_svod[64];
sprintf(imaf_svod,"opsss%d.lst",getpid());
FILE *ff_svod;
if((ff_svod = fopen(imaf_svod,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_svod,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


if(data->rk->metka == 1)
 {
  iceb_u_zagolov(gettext("Движение услуг по счетам получения"),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ff);
  iceb_u_zagolov(gettext("Движение услуг по счетам получения"),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ffi);
  iceb_u_zagolov(gettext("Движение услуг по счетам получения"),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ff_svod);
 }
if(data->rk->metka == 2)
 {
  iceb_u_zagolov(gettext("Движение услуг по счетам списания"),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ff);
  iceb_u_zagolov(gettext("Движение услуг по счетам списания"),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ffi);
  iceb_u_zagolov(gettext("Движение услуг по счетам списания"),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ff_svod);
 }
kol_str=5;

if(data->rk->podr.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код подразделения"),data->rk->podr.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Код подразделения"),data->rk->podr.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Код подразделения"),data->rk->podr.ravno());
  kol_str++;
 }
if(data->rk->shet_sp.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Счёт списания"),data->rk->shet_sp.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Счёт списания"),data->rk->shet_sp.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Счёт списания"),data->rk->shet_sp.ravno());
  kol_str++;
 }
if(data->rk->shet_uh.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),data->rk->shet_uh.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Счёт учёта"),data->rk->shet_uh.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Счёт учёта"),data->rk->shet_uh.ravno());
  kol_str++;
 }
if(data->rk->kodgr.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Группа"),data->rk->kodgr.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Группа"),data->rk->kodgr.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Группа"),data->rk->kodgr.ravno());
  kol_str++;
 }
if(data->rk->kodzap.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код услуги"),data->rk->kodzap.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Код услуги"),data->rk->kodzap.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Код услуги"),data->rk->kodzap.ravno());
  kol_str++;
 }
if(data->rk->kodop.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Код операции"),data->rk->kodop.ravno());
  kol_str++;
 }
if(data->rk->kontr.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
  fprintf(ffi,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
  fprintf(ff_svod,"%s:%s\n",gettext("Код контрагента"),data->rk->kontr.ravno());
  kol_str++;
 }

int nomlist=0;

//Шапка 

opss_h(&nomlist,&kol_str,ff);

fprintf(ffi,"----------------------\n");
fprintf(ffi,gettext("  Счёт    |   Сумма  |\n"));
fprintf(ffi,"----------------------\n");


double suma;
double cena;
class iceb_u_str shet_sp_z("");
class iceb_u_str shet_uheta_z("");
double itogo_shetu=0.;
double itogo_shets=0.;
double itogo_shets_o=0.;

class iceb_u_str sklad("");
class iceb_u_str nomd("");
class iceb_u_str kodmat("");
class iceb_u_str ei("");
class iceb_u_spisok shet_kodzap;
class iceb_u_double shet_kodzap_suma;
class iceb_u_double shet_kodzap_kolih;
int nomer_shet_kodzap=0;
kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstrtmp,++kolstr1);    

  shet_sp.new_plus(row[0]);
  shet_uheta.new_plus(row[1]);
  iceb_u_rsdat(&dd,&md,&gd,row[2],2);
  sklad.new_plus(row[3]);
  nomd.new_plus(row[4]);
  kodmat.new_plus(row[5]);
  kolih=atof(row[6]);
  cena=atof(row[7]);
  ei.new_plus(row[8]);
  kodop.new_plus(row[9]);
  kontr.new_plus(row[10]);
  naim.new_plus(row[11]);
        
  if(iceb_u_SRAV(shet_uheta_z.ravno(),shet_uheta.ravno(),0) != 0)
   {
    if(shet_uheta_z.ravno()[0] != '\0')
     {

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(1,shet_uheta_z.ravno(),itogo_shetu,ff,ffi);

      opps_shet_str(&nomlist,&kol_str,ff);
      fprintf(ff,"\n");
     }    
    itogo_shetu=0.;
    shet_uheta_z.new_plus(shet_uheta.ravno());
   }

  if(iceb_u_SRAV(shet_sp_z.ravno(),shet_sp.ravno(),0) != 0)
   {
    if(shet_sp_z.ravno()[0] != '\0')
     {

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(1,shet_uheta_z.ravno(),itogo_shetu,ff,ffi);
      itogo_shetu=0.;

      opps_shet_str(&nomlist,&kol_str,ff);
      opss_itogo(2,shet_sp_z.ravno(),itogo_shets,ff,ffi);

      opps_shet_str(&nomlist,&kol_str,ff);
      fprintf(ff,"\n");
     }    
    itogo_shets_o+=itogo_shets;
    itogo_shets=0.;
    shet_sp_z.new_plus(shet_sp.ravno());
   }

  
  cena=iceb_u_okrug(cena,okrcn);
  
  suma=cena*kolih;
  suma=iceb_u_okrug(suma,okrg1);
  itogo_shetu+=suma;
  itogo_shets+=suma;  

  sprintf(strsql,"%s|%s",shet_sp.ravno(),kodmat.ravno());
  
  if((nomer_shet_kodzap=shet_kodzap.find(strsql)) < 0)
    shet_kodzap.plus(strsql);
  
  shet_kodzap_kolih.plus(kolih,nomer_shet_kodzap);
  shet_kodzap_suma.plus(suma,nomer_shet_kodzap);


  opps_shet_str(&nomlist,&kol_str,ff);

  fprintf(ff,"%-8s %-*.*s %-*s %-*s %10.2f %-*s %8.8g %10.10g %02d.%02d.%d %-*s %-5s %-*s %-*s\n",
  kodmat.ravno(),
  iceb_u_kolbait(20,naim.ravno()),iceb_u_kolbait(20,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(10,shet_sp.ravno()),shet_sp.ravno(),
  iceb_u_kolbait(10,shet_uheta.ravno()),shet_uheta.ravno(),
  suma,
  iceb_u_kolbait(6,ei.ravno()),ei.ravno(),
  kolih,cena,dd,md,gd,
  iceb_u_kolbait(9,nomd.ravno()),nomd.ravno(),
  sklad.ravno(),
  iceb_u_kolbait(6,kodop.ravno()),kodop.ravno(),
  iceb_u_kolbait(7,kontr.ravno()),kontr.ravno());

  for(int nom=20; nom < iceb_u_strlen(naim.ravno()); nom+=20)
   {
    fprintf(ff,"%8s %-*.*s\n",
    "",
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim.ravno())),
    iceb_u_kolbait(20,iceb_u_adrsimv(nom,naim.ravno())),
    iceb_u_adrsimv(nom,naim.ravno()));
   }
 }




itogo_shets_o+=itogo_shets;

opss_itogo(1,shet_uheta_z.ravno(),itogo_shetu,ff,ffi);
opss_itogo(2,shet_sp_z.ravno(),itogo_shets,ff,ffi);
opss_itogo(3,"",itogo_shets_o,ff,ffi);

iceb_podpis(ff,data->window);
fclose(ff);
iceb_podpis(ffi,data->window);
fclose(ffi);

opss_u_svod(ff_svod,&shet_kodzap,&shet_kodzap_suma,&shet_kodzap_kolih,data->window);

fclose(ff_svod);

data->rk->imaf.new_plus(imaf);

if(data->rk->metka == 1)
  data->rk->naimf.new_plus(gettext("Движение услуг по счетам получения"));
if(data->rk->metka == 2)
  data->rk->naimf.new_plus(gettext("Движение услуг по счетам списания"));

data->rk->imaf.plus(imafi);
data->rk->naimf.plus(gettext("Общие итоги"));

data->rk->imaf.plus(imaf_svod);
data->rk->naimf.plus(gettext("Свод по кодам услуг и счетам"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
  iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);



data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
