/*$Id: mumow_r.c,v 1.13 2013/09/26 09:46:52 sasa Exp $*/
/*19.06.2019	17.12.2009	Белых А.И.	mumow_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "mumow.h"

class mumow_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class mumow_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  mumow_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   mumow_r_key_press(GtkWidget *widget,GdkEventKey *event,class mumow_r_data *data);
gint mumow_r1(class mumow_r_data *data);
void  mumow_r_v_knopka(GtkWidget *widget,class mumow_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;

int mumow_r(class mumow_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class mumow_r_data data;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(mumow_r_key_press),&data);

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(mumow_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)mumow_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  mumow_r_v_knopka(GtkWidget *widget,class mumow_r_data *data)
{
//printf("mumow_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   mumow_r_key_press(GtkWidget *widget,GdkEventKey *event,class mumow_r_data *data)
{
// printf("mumow_r_key_press\n");
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}

/**********************************/
/*Шапка*/
/**********************************/
void mumow_sap(short dn,short mn,short gn,
short dk,short mk,short gk,
class iceb_u_spisok *kontr_prih,class iceb_u_spisok *kontr_rash,class iceb_u_str *liniq,FILE *ff,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
int kod_skl=0;
class iceb_u_str naim("");
int kolih_kontrp=kontr_prih->kolih();
int kolih_kontrr=kontr_rash->kolih();

liniq->new_plus("---------------------------------------------------------------------------------");

for(int nom=0; nom < kolih_kontrp+kolih_kontrr+1; nom++)
 liniq->plus("----------------------");

fprintf(ff,"%s\n",liniq->ravno());

sprintf(strsql,"%s %02d.%02d.%04d",gettext("Остаток на"),dn,mn,gn);

fprintf(ff,gettext("\
N кар-|Код м-|    Наименование    | Счёт |Един.|  Цена    |"));

fprintf(ff,"%-*s|",iceb_u_kolbait(21,strsql),strsql);
            
if(kolih_kontrp > 0)
 fprintf(ff,"%-*s|",iceb_u_kolbait(kolih_kontrp*22-1,gettext("Приход")),gettext("Приход"));

if(kolih_kontrr > 0)
 fprintf(ff,"%-*s|",iceb_u_kolbait(kolih_kontrr*22-1,gettext("Расход")),gettext("Расход"));

sprintf(strsql,"%s %02d.%02d.%04d",gettext("Остаток на"),dk,mk,gk);
fprintf(ff,"%-*s|\n",iceb_u_kolbait(21,strsql),strsql);

fprintf(ff,gettext("\
точки |атери-|     материалла     |учёта |изме-|          |---------------------|"));

for(int nom=0; nom < kolih_kontrp; nom++)
 {
  naim.new_plus(kontr_prih->ravno(nom));
  if((kod_skl=iceb_00_skl(kontr_prih->ravno(nom))) > 0)
   {
    
    sprintf(strsql,"select naik from Sklad where kod=%d",kod_skl);
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim.plus(" ",row[0]);     
   }
  else
   {
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr_prih->ravno(nom));
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim.plus(" ",row[0]);     

   }

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(21,naim.ravno()),iceb_u_kolbait(21,naim.ravno()),naim.ravno());
 }

for(int nom=0; nom < kolih_kontrr; nom++)
 {
  naim.new_plus(kontr_rash->ravno(nom));
  if((kod_skl=iceb_00_skl(kontr_rash->ravno(nom))) > 0)
   {
    
    sprintf(strsql,"select naik from Sklad where kod=%d",kod_skl);
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim.plus(" ",row[0]);     
   }
  else
   {
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr_rash->ravno(nom));
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim.plus(" ",row[0]);     

   }

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(21,naim.ravno()),iceb_u_kolbait(21,naim.ravno()),naim.ravno());
 }
fprintf(ff,"---------------------|\n");

fprintf(ff,gettext("\
      |ала   |                    |      |рения|          |Количество|  Сумма   |"));
/*                                                  
123456 123456 12345678901234567890 12345 1234567890 1234567890 1234567890
*/

for(int nom=0; nom < kolih_kontrp+kolih_kontrr+1; nom++)
 fprintf(ff,"%-*.*s|%-*.*s|",
 iceb_u_kolbait(10,gettext("Количество")),
 iceb_u_kolbait(10,gettext("Количество")),
 gettext("Количество"),
 iceb_u_kolbait(10,gettext("Сумма")),
 iceb_u_kolbait(10,gettext("Сумма")),
 gettext("Сумма"));

fprintf(ff,"\n");

fprintf(ff,"%s\n",liniq->ravno());
}

/******************************************/
/******************************************/

gint mumow_r1(class mumow_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_clock sss(data->window);

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



class SQLCURSOR cur,cur1,cur2;
SQL_str row,row1,row2;

int kolstr=0;
float kolstr1=0.;

sprintf(strsql,"select sklad,nomk,kodm,shetu,cena from Kart order by sklad asc,shetu asc");
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
class iceb_u_spisok skl_nk;
class iceb_u_spisok kontr_prih;
class iceb_u_spisok kontr_rash;
class iceb_u_spisok shetu;
class iceb_u_double cena_kart;

class iceb_u_double prihod_suma;
class iceb_u_double rashod_suma;
class iceb_u_double prihod_kolih;
class iceb_u_double rashod_kolih;

class iceb_u_double saldon_suma;
class iceb_u_double saldok_suma;
class iceb_u_double saldon_kolih;
class iceb_u_double saldok_kolih;

class iceb_u_double ikol_kolih_prih;
class iceb_u_double ikol_suma_prih;

class iceb_u_double ikol_kolih_rash;
class iceb_u_double ikol_suma_rash;



int nomer=0; 
class ostatok ost;
int kolstr2=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[0],0,0) != 0)
    continue;

//  printw("Подходит-%s %s\n",row[0],rekp.sklad.ravno());
//  OSTANOV();
  
  if(iceb_u_proverka(data->rk->kodmat.ravno(),row[2],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->shet.ravno(),row[3],0,0) != 0)
    continue;

  if(data->rk->grupm.getdlinna() > 1)
   {
    sprintf(strsql,"select kodgr from Material where kodm=%s",row[2]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     if(iceb_u_proverka(data->rk->grupm.ravno(),row1[0],0,0) != 0)
      continue;
   }

  
  ostkarw(dn,mn,gn,dk,mk,gk,atoi(row[0]),atoi(row[1]),&ost);

  if(ost.ostm[0] == 0. && ost.ostm[1] == 0. && ost.ostm[3] == 0.)
   continue;  

  sprintf(strsql,"%s|%s",row[0],row[1]);
  if((nomer=skl_nk.find(strsql)) < 0)
   {
    skl_nk.plus(strsql);
    shetu.plus(row[3]);
   }
  cena_kart.plus(atof(row[4]),nomer);
  /*смотрим приходы-расходы*/
  sprintf(strsql,"select nomd,datd,tipz from Zkart where datdp >= '%04d-%02d-%02d' and \
datdp <= '%04d-%02d-%02d' and sklad=%s and nomk=%s",
  gn,mn,dn,gk,mk,dk,row[0],row[1]);

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(1);
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    if(iceb_u_SRAV("000",row1[0],0) == 0)
      continue;
      
    sprintf(strsql,"select kontr from Dokummat where datd='%s' and sklad=%s and nomd='%s' and tip=%s",
    row1[1],row[0],row1[0],row1[2]);
    if(iceb_sql_readkey(strsql,&row2,&cur2,data->window) != 1)
     {
      sprintf(strsql,"%s %s %s %s %s",gettext("Не найдена шапка документа!"),
      row1[1],row[0],row1[0],row1[2]);
      iceb_menu_soob(strsql,data->window);
      continue;
     }

    if(atoi(row1[2]) == 1) /*Приход*/
     if(kontr_prih.find_r(row2[0]) < 0)
      kontr_prih.plus(row2[0]);

    if(atoi(row1[2]) == 2) /*Расход*/
     if(kontr_rash.find_r(row2[0]) < 0)
      kontr_rash.plus(row2[0]);
   }
 }
int kolih_skl_nk=skl_nk.kolih();
int kolih_kontrp=kontr_prih.kolih();
int kolih_kontrr=kontr_rash.kolih();

prihod_suma.make_class(kolih_skl_nk*kolih_kontrp);
rashod_suma.make_class(kolih_skl_nk*kolih_kontrr);
prihod_kolih.make_class(kolih_skl_nk*kolih_kontrp);
rashod_kolih.make_class(kolih_skl_nk*kolih_kontrr);

saldon_suma.make_class(kolih_skl_nk);
saldok_suma.make_class(kolih_skl_nk);
saldon_kolih.make_class(kolih_skl_nk);
saldok_kolih.make_class(kolih_skl_nk);

ikol_kolih_prih.make_class(kolih_kontrp);
ikol_suma_prih.make_class(kolih_kontrp);

ikol_kolih_rash.make_class(kolih_kontrr);
ikol_suma_rash.make_class(kolih_kontrr);


class iceb_u_str sklsp("");
class iceb_u_str nomkart("");
int nomer_kontr=0;
double kolih=0.;
double suma=0.;

for(int nom_skl_nk=0; nom_skl_nk < kolih_skl_nk; nom_skl_nk++)
 {
  iceb_u_polen(skl_nk.ravno(nom_skl_nk),&sklsp,1,'|');
  iceb_u_polen(skl_nk.ravno(nom_skl_nk),&nomkart,2,'|');

  ostkarw(dn,mn,gn,dk,mk,gk,sklsp.ravno_atoi(),nomkart.ravno_atoi(),&ost);

  saldon_kolih.plus(ost.ostm[0],nom_skl_nk);
  saldok_kolih.plus(ost.ostm[3],nom_skl_nk);

  saldon_suma.plus(ost.ostmc[0],nom_skl_nk);
  saldok_suma.plus(ost.ostmc[3],nom_skl_nk);
        
  /*смотрим приходы-расходы*/
  sprintf(strsql,"select nomd,datd,tipz,kolih from Zkart where datdp >= '%04d-%02d-%02d' and \
datdp <= '%04d-%02d-%02d' and sklad=%s and nomk=%s",
  gn,mn,dn,gk,mk,dk,sklsp.ravno(),nomkart.ravno());

  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(1);
   }

  while(cur1.read_cursor(&row1) != 0)
   {
    if(iceb_u_SRAV("000",row1[0],0) == 0)
      continue;
    sprintf(strsql,"select kontr from Dokummat where datd='%s' and sklad=%s and nomd='%s' and tip=%s",
    row1[1],sklsp.ravno(),row1[0],row1[2]);
    if(iceb_sql_readkey(strsql,&row2,&cur2,data->window) != 1)
     {
      sprintf(strsql,"%s %s %s %s %s",gettext("Не найдена шапка документа!"),
      row1[1],row[0],row1[0],row1[2]);
      iceb_menu_soob(strsql,data->window);
      continue;
     }
    kolih=atof(row1[3]);
    suma=kolih*cena_kart.ravno(nom_skl_nk);
        
    if(atoi(row1[2]) == 1) /*Приход*/
     {
      if((nomer_kontr=kontr_prih.find_r(row2[0])) < 0)
       {
        sprintf(strsql,"%s %s!",gettext("Не нашли код контрагента"),row2[0]);
        iceb_menu_soob(strsql,data->window);
        continue;
       }
      prihod_suma.plus(suma,nom_skl_nk*kolih_kontrp+nomer_kontr);
      prihod_kolih.plus(kolih,nom_skl_nk*kolih_kontrp+nomer_kontr);

      ikol_kolih_prih.plus(kolih,nomer_kontr);
      ikol_suma_prih.plus(suma,nomer_kontr);
     }
    if(atoi(row1[2]) == 2) /*Расход*/
     {
      if((nomer_kontr=kontr_rash.find_r(row2[0])) < 0)
       {
        sprintf(strsql,"%s %s!",gettext("Не нашли код контрагента"),row2[0]);
        iceb_menu_soob(strsql,data->window);
        continue;
       }
      rashod_suma.plus(suma,nom_skl_nk*kolih_kontrr+nomer_kontr);
      rashod_kolih.plus(kolih,nom_skl_nk*kolih_kontrr+nomer_kontr);
      ikol_kolih_rash.plus(kolih,nomer_kontr);
      ikol_suma_rash.plus(suma,nomer_kontr);
     }
   }
  

 }

FILE *ff;
char imaf[56];
sprintf(imaf,"matot%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_zagolov(gettext("Материальный отчёт"),dn,mn,gn,dk,mk,gk,ff,data->window);

if(data->rk->sklad.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Склад"),data->rk->sklad.ravno());
if(data->rk->grupm.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Группа"),data->rk->grupm.ravno());
if(data->rk->kodmat.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код материалла"),data->rk->kodmat.ravno());
if(data->rk->shet.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());

class iceb_u_str liniq;

int nomer_kart=0;
int skl1=0;
int skl2=0;
int kodmat=0;
double cena=0.;
class iceb_u_str ei("");
class iceb_u_str naim("");
for(int nomer_skl_nk=0; nomer_skl_nk < kolih_skl_nk; nomer_skl_nk++)
 {
  iceb_u_polen(skl_nk.ravno(nomer_skl_nk),&skl2,1,'|');
  iceb_u_polen(skl_nk.ravno(nomer_skl_nk),&nomer_kart,2,'|');
  
  if(skl1 != skl2)
   {
    if(skl1 != 0)
     fprintf(ff,"%s\n",liniq.ravno());

    naim.new_plus("");
    sprintf(strsql,"select naik from Sklad where kod=%d",skl2);

    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     naim.new_plus(row[0]);

    fprintf(ff,"%s:%d %s\n",gettext("Склад"),skl2,naim.ravno());        
    mumow_sap(dn,mn,gn,dk,mk,gk,&kontr_prih,&kontr_rash,&liniq,ff,data->window); /*Печать шапки*/

    skl1=skl2;
   }  

  sprintf(strsql,"select kodm,ei,cena from Kart where sklad=%d and nomk=%d",skl2,nomer_kart);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,"Не найдена карточка %d на складе %d!",nomer_kart,skl2);
    iceb_menu_soob(strsql,data->window);
    continue;   
   }
  kodmat=atoi(row[0]);
  ei.new_plus(row[1]);
  cena=atof(row[2]);  
  naim.new_plus("");
  sprintf(strsql,"select naimat from Material where kodm=%d",kodmat);

  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   naim.new_plus(row[0]);

  fprintf(ff,"%6d|%6d|%-*.*s|%-*.*s|%-*.*s|%10.10g|",
  nomer_kart,
  kodmat,
  iceb_u_kolbait(20,naim.ravno()),
  iceb_u_kolbait(20,naim.ravno()),
  naim.ravno(),
  iceb_u_kolbait(6,shetu.ravno(nomer_skl_nk)),
  iceb_u_kolbait(6,shetu.ravno(nomer_skl_nk)),
  shetu.ravno(nomer_skl_nk),
  iceb_u_kolbait(5,ei.ravno()),
  iceb_u_kolbait(5,ei.ravno()),
  ei.ravno(),
  cena);

  if(saldon_kolih.ravno(nomer_skl_nk) != 0. || saldon_suma.ravno(nomer_skl_nk) != 0.)
    fprintf(ff,"%10.2f|%10.2f|",saldon_kolih.ravno(nomer_skl_nk),saldon_suma.ravno(nomer_skl_nk));
  else
    fprintf(ff,"%10s|%10s|","","");
  

  for(nomer_kontr=0; nomer_kontr < kolih_kontrp; nomer_kontr++)
   {
    kolih=prihod_kolih.ravno(nomer_skl_nk*kolih_kontrp+nomer_kontr); 
    suma=prihod_suma.ravno(nomer_skl_nk*kolih_kontrp+nomer_kontr); 

    if(suma == 0. && kolih == 0.)
     fprintf(ff,"%10s|%10s|","","");
    else
     fprintf(ff,"%10.10g|%10.2f|",kolih,suma);
   }

  for(nomer_kontr=0; nomer_kontr < kolih_kontrr; nomer_kontr++)
   {
    kolih=rashod_kolih.ravno(nomer_skl_nk*kolih_kontrr+nomer_kontr); 
    suma=rashod_suma.ravno(nomer_skl_nk*kolih_kontrr+nomer_kontr); 

    if(suma == 0. && kolih == 0.)
     fprintf(ff,"%10s|%10s|","","");
    else
     fprintf(ff,"%10.10g|%10.2f|",kolih,suma);
   }

  fprintf(ff,"%10.10g|%10.2f|\n",
  saldok_kolih.ravno(nomer_skl_nk),
  saldok_suma.ravno(nomer_skl_nk));

 }

if(liniq.getdlinna() > 1)
 {
  fprintf(ff,"%s\n",liniq.ravno());

  fprintf(ff,"%*s|%10.10g|%10.2f|",
  iceb_u_kolbait(58,gettext("Итого")),gettext("Итого"),
  saldon_kolih.suma(),
  saldon_suma.suma());


  for(nomer_kontr=0; nomer_kontr < kolih_kontrp; nomer_kontr++)
   fprintf(ff,"%10.10g|%10.2f|",
   ikol_kolih_prih.ravno(nomer_kontr),
   ikol_suma_prih.ravno(nomer_kontr));

  for(nomer_kontr=0; nomer_kontr < kolih_kontrr; nomer_kontr++)
   fprintf(ff,"%10.10g|%10.2f|",
   ikol_kolih_rash.ravno(nomer_kontr),
   ikol_suma_rash.ravno(nomer_kontr));

  fprintf(ff,"%10.10g|%10.2f|\n",
  saldok_kolih.suma(),
  saldok_suma.suma());

  fprintf(ff,"%s\n",liniq.ravno());
 }

iceb_podpis(ff,data->window);
fclose(ff);


gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace",NULL);


char stroka[2048];
sprintf(strsql,"%s %02d.%02d.%04d",gettext("Остаток на"),dn,mn,gn);

sprintf(stroka,"%-*s:%12.12g %12.2f\n",
iceb_u_kolbait(25,strsql),
strsql,
saldon_kolih.suma(),
saldon_suma.suma());

iceb_printw(stroka,data->buffer,data->view,"naim_shrift");

sprintf(stroka,"%-*s:%12.12g %12.2f\n",
iceb_u_kolbait(25,gettext("Приход")),
gettext("Приход"),
ikol_kolih_prih.suma(),
ikol_suma_prih.suma());

iceb_printw(stroka,data->buffer,data->view,"naim_shrift");

sprintf(stroka,"%-*s:%12.12g %12.2f\n",
iceb_u_kolbait(25,gettext("Расход")),
gettext("Расход"),
ikol_kolih_rash.suma(),
ikol_suma_rash.suma());

iceb_printw(stroka,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"%s %02d.%02d.%04d",gettext("Остаток на"),dk,mk,gk);
sprintf(stroka,"%-*s:%12.12g %12.2f\n",
iceb_u_kolbait(25,strsql),
strsql,
saldok_kolih.suma(),
saldok_suma.suma());

iceb_printw(stroka,data->buffer,data->view,"naim_shrift");


data->rk->imaf.new_plus(imaf);
data->rk->naimf.new_plus(gettext("Материальный отчёт"));





for(int nomer=0; nomer < data->rk->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rk->imaf.ravno(nomer),0,data->window);





gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
