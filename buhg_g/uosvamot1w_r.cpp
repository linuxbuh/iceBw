/*$Id: uosvamot1w_r.c,v 1.8 2013/09/26 09:46:57 sasa Exp $*/
/*29.12.2016	19.01.2012	Белых А.И.	uosvamot1w_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "uosvamotw.h"

class uosvamot1w_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class uosvamotw_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  uosvamot1w_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosvamot1w_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvamot1w_r_data *data);
gint uosvamot1w_r1(class uosvamot1w_r_data *data);
void  uosvamot1w_r_v_knopka(GtkWidget *widget,class uosvamot1w_r_data *data);


extern SQL_baza bd;
extern short metkabo;  //Если 1 то организация бюджетная

int uosvamot1w_r(class uosvamotw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class uosvamot1w_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка ведомости аморт-отчислений для налогового учёта"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(uosvamot1w_r_key_press),&data);

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

repl.plus(gettext("Распечатка ведомости аморт-отчислений для налогового учёта"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(uosvamot1w_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)uosvamot1w_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosvamot1w_r_v_knopka(GtkWidget *widget,class uosvamot1w_r_data *data)
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

gboolean   uosvamot1w_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvamot1w_r_data *data)
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

/***************************/
/****************************/
void uosvamot_prp(class uosvamotw_data *rk,int *kol_str,FILE *ff)
{

if(rk->grupa.getdlinna() > 1)
 {
  fprintf(ff,"%s: %s\n",gettext("Група"),rk->grupa.ravno());
  *kol_str+=1;
 }
if(rk->podr.getdlinna() > 1)
 {
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),rk->podr.ravno());
  *kol_str+=1;
 }
if(rk->hzt.getdlinna() > 1)
 {
  *kol_str+=1;
  fprintf(ff,"%s: %s\n",gettext("Шифр затрат"),rk->hzt.ravno());
 }
if(rk->innom.getdlinna() > 1)
 {
  *kol_str+=1;
  fprintf(ff,"%s: %s\n",gettext("Инвентарный номер"),rk->innom.ravno());
 }

if(rk->shetu.getdlinna() > 1)
 {
  *kol_str+=1;
  fprintf(ff,"%s: %s\n",gettext("Счёт"),rk->shetu.ravno());
 }

*kol_str+=1;
if(rk->sost_ob == 0)
  fprintf(ff,"%s: %s\n",gettext("Состояние объекта"),gettext("Все основные средства"));
if(rk->sost_ob == 1)
  fprintf(ff,"%s: %s\n",gettext("Состояние объекта"),gettext("Все основные амортизацию для которых считают"));
if(rk->sost_ob == 2)
  fprintf(ff,"%s: %s\n",gettext("Состояние объекта"),gettext("Все основные амортизацию для которых не считают"));

if(rk->hau.getdlinna() > 1)
 {
  *kol_str+=1;
  fprintf(ff,"%s: %s\n",gettext("Шифр аналитического учёта"),rk->hau.ravno());
 }

}
/******************************************/
/*шапка для нового отчёта*/
/*******************************/
void uosvamot1_sh(short mn,short mk,short god,FILE *ffi)
{

fprintf(ffi,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------");

strpod(mn,mk,11,ffi);

fprintf(ffi,gettext("\
Инв-рный|        Н а и м е н о в а н и е         | Номерной |Шифр |"));

fprintf(ffi,gettext("\
Норма|Балансовая  |   Износ    | Остаточная |Ликвидаци.|Годов.сумм|Мес. сумма|Коэфи|"));

strms(0,mn,mk,ffi);

fprintf(ffi,gettext("\
 номер  |             объекта                    |   знак   |н.ам.|"));

fprintf(ffi,gettext("\
а.отч|  стоимость |            |бал.стои-сть|стоимость |амортотчис|амортотчи.|циент|"));

strms(1,mn,mk,ffi);

fprintf(ffi,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------");

strpod(mn,mk,11,ffi);



}
/*******************************************/
/*распечатка по месяцам для нового отчёта*/
/*****************************************/
void rasmes1(short mn,short mk,double *maot,
int metka_ob, /*0-обнулять 1-нет*/
FILE *ff)
{
short           i;
double          vs;
vs=0.;

for(i=mn-1 ;i<mk ;i++)
 {
  vs+=maot[i];
  fprintf(ff,"%10.2f|",maot[i]);
  if(metka_ob == 0)
    maot[i]=0.;
 }
if(mn != mk)
   fprintf(ff,"%10.2f|",vs);
fprintf(ff,"\n");
}
/***********************************************/
/*Распечтатка итога по группе или шифру затрат*/
/************************************************/

void uosvamot1_itog(int metka,const char *kod,short mn,short mk,double *bsiz,double *amort,FILE *ff,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str strit("");
SQL_str row;
class SQLCURSOR cur;

if(metka == 1) /*по группе*/
 {
  strit.new_plus(gettext("Итого по группе"));
  strit.plus(" ",kod);
  sprintf(strsql,"select naik from Uosgrup where kod='%s'",kod);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strit.plus(" ",row[0]);
   
 }
if(metka == 2) /*по шифру затрат*/
 {
  strit.new_plus(gettext("Итого по шифру затрат"));
  strit.plus(" ",kod);
  sprintf(strsql,"select naik from Uoshz where kod='%s'",kod);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strit.plus(" ",row[0]);
 }
if(metka == 3) /*общий иток*/
 {
  strit.new_plus(gettext("Общий итог"));
 }
fprintf(ff,"\
-------------------------------------------------------------------------------------------------------------------------------------------------------");
strpod(mn,mk,11,ff);

fprintf(ff,"%*.*s|",
iceb_u_kolbait(72,strit.ravno()),
iceb_u_kolbait(72,strit.ravno()),
strit.ravno());

fprintf(ff,"%12.2f|%12.2f|%12.2f|%10.2f|%10.2f|%10.2f|     |",bsiz[0],bsiz[1],bsiz[2],bsiz[3],bsiz[4],bsiz[5]);

bsiz[0]=bsiz[1]=bsiz[2]=bsiz[3]=bsiz[4]=bsiz[5]=0.;

rasmes1(mn,mk,amort,0,ff);
if(metka == 2)
 fprintf(ff,"\n");
}
/***********************************************/
/*Распечтатка общих итогов*/
/************************************************/

void uosvamot1_itogo(int metka,const char *kod,short mn,short mk,double *bsiz,double *amort,FILE *ff,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str strit("");
SQL_str row;
class SQLCURSOR cur;

if(metka == 1) /*по группе*/
 {
  strit.new_plus(gettext("Итого по группе"));
  strit.plus(" ",kod);
  sprintf(strsql,"select naik from Uosgrup where kod='%s'",kod);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strit.plus(" ",row[0]);
   
 }
if(metka == 2) /*по шифру затрат*/
 {
  strit.new_plus(gettext("Итого по шифру затрат"));
  strit.plus(" ",kod);
  sprintf(strsql,"select naik from Uoshz where kod='%s'",kod);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   strit.plus(" ",row[0]);
 }
if(metka == 3) /*общий иток*/
 {
  strit.new_plus(gettext("Общий итог"));
 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------");
strpod(mn,mk,11,ff);

fprintf(ff,"%*.*s|",
iceb_u_kolbait(50,strit.ravno()),
iceb_u_kolbait(50,strit.ravno()),
strit.ravno());

fprintf(ff,"%12.2f|%12.2f|%12.2f|%10.2f|%10.2f|%10.2f|",bsiz[0],bsiz[1],bsiz[2],bsiz[3],bsiz[4],bsiz[5]);

rasmes1(mn,mk,amort,1,ff);

if(metka == 1)
 fprintf(ff,"\n");
}
/*******************************************/
/* */
/**********************************/
void uosvamot_nf(int innom,short mn,short mk,short gr,double *amort_innom,double *itogo_raoz,const char *naios,double god_proc_am,double popkf,FILE *raoz,GtkWidget *wpredok)
{
/*
-----------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
Субрахунок|Інвентарний|     Назва об’єкта            |Вартість яка|Річна сума |Кількість місяців     | Сума зносу |Сума нарахо-|Сума зносу  |Субрахунок|Примітка|\n\
          |   номер   |                              |амортизуєть-|амортизації|корисного використання| на початок |ваної аморт-|на кінець   |  витрат  |        |\n\
          |           |                              |     ся     |           |(експлуанації) у пері-|  періоду   |изації за   |періоду     |          |        |\n\
          |           |                              |            |           |оді                   |            |  період    |(гр.7+гр.8) |          |        |\n\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
    1     |     2     |              3               |     4      |     5     |          6           |      7     |      8     |     9      |    10    |   11   |\n\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

1234567890 12345678901 123456789012345678901234567890 123456789012 12345678901 1234567890123456789012 123456789012 123456789012 123456789012
*/


class bsizw_data bal_st;
short d=1;
short m=mk;
short g=gr;
iceb_u_dpm(&d,&m,&g,3); /*увеличиваем на месяц нужна стоимость на начало следующего месяца*/

int podr=0;
int kodotl=0;
int metka_bs=0; /*0-балансовая стоимость и износ взяты на конец периода 1- на начало*/

/*проверяем числится ли на конец периода*/
if(metkabo == 1)  //Если 1 то организация бюджетная
 {
  if(poiinw(innom,1,m,g,&podr,&kodotl,wpredok) != 0)
   return;
 }
else
 {
  /*если на конец периода нет то нужно смотреть балансовую стоимость на начало периода*/
  if(poiinw(innom,1,m,g,&podr,&kodotl,wpredok) != 0)
   {
    m=mn;
    g=gr;
    metka_bs=1;
   }
 }


bsizw(innom,0,1,m,g,&bal_st,NULL,wpredok);

double bal_stoim=bal_st.bs+bal_st.sbs;
double bal_iznos=bal_st.iz+bal_st.iz1+bal_st.siz;

double lik_st=uosgetlsw(innom,1,mk,gr,0,wpredok);

double sgod_amort=(bal_stoim-lik_st)*god_proc_am/100.*popkf;

int kolmes=mk-mn+1;

double amort_za_period=0.;
for(int i=mn-1 ;i<mk ;i++)
 {
  amort_za_period+=amort_innom[i];
 }

if(metka_bs == 0)/*балансовая стоимость и износ взяты на конец периода*/
 {
  bal_iznos-=amort_za_period;
 }


double iznos_end=bal_iznos+amort_za_period;

fprintf(raoz,"%10s %-11d %-*.*s %12.2f %11.2f %22d %12.2f %12.2f %12.2f\n",
"",
innom,
iceb_u_kolbait(30,naios),iceb_u_kolbait(30,naios),naios,
bal_stoim,
sgod_amort,
kolmes,
bal_iznos,
amort_za_period,
iznos_end);

if(iceb_u_strlen(naios) > 30)
 fprintf(raoz,"%10s %11s %s\n","","",iceb_u_adrsimv(30,naios));

itogo_raoz[0]+=bal_iznos;
itogo_raoz[1]+=amort_za_period;
itogo_raoz[2]+=iznos_end;
}

/******************************************/
/******************************************/

gint uosvamot1w_r1(class uosvamot1w_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_clock sss(data->window);
SQL_str row,row1;
SQLCURSOR cur,cur1;

short mn,gn;
short mk,gk;

mn=data->rk->mesn.ravno_atoi();
mk=data->rk->mesk.ravno_atoi();
gn=gk=data->rk->god.ravno_atoi();
if(mk == 0)
 mk=mn;

int kolstr=0;

sprintf(strsql,"select innom,da,podr,hzt,hna,suma from Uosamor where da >= '%04d-%02d-01' and da <= '%04d-%02d-31' order by hna asc,hzt asc, innom asc",gn,mn,gk,mk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);  
 }


FILE *ff_innom;
char imaf_innom[64];
sprintf(imaf_innom,"amotnu1%d.lst",getpid());

if((ff_innom = fopen(imaf_innom,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_innom,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

FILE *ff_it;
char imaf_it[64];
sprintf(imaf_it,"amotnui1%d.lst",getpid());

if((ff_it = fopen(imaf_it,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_it,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_fopen raoz;
char imaf_raoz[64];
sprintf(imaf_raoz,"raoz%d.lst",getpid());
if(raoz.start(imaf_raoz,"w",data->window) != 0)
 return(1);
class iceb_vrvr rv;

rv.ns.plus(4);
rv.begin.plus(0);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(iceb_get_pnk("00",0,data->window));

rv.ns.plus(5);
rv.begin.plus(31);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(iceb_get_edrpou("00",data->window));

sprintf(strsql,"%02d.%d по %02d.%d р.",mn,gn,mk,gk);
rv.ns.plus(7);
rv.begin.plus(45);
rv.end.plus(100);
rv.hag.plus(1);
rv.str.plus(strsql);

iceb_vrvr("uosvamot_ra_h.alx",&rv,raoz.ff,data->window); /*печать заголовка документа*/
int kol=0;
uosvamot_hw(1,&kol,raoz.ff,data->window); /*печать шапки документа*/



short dkm=0;
iceb_u_dpm(&dkm,&mk,&gk,5);

iceb_zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,dkm,mk,gk,ff_innom,data->window);
fprintf(ff_innom,"%s\n",gettext("Налоговый учёт"));

int kol_str=0;

uosvamot_prp(data->rk,&kol_str,ff_innom);

uosvamot1_sh(mn,mk,gn,ff_innom);

iceb_zagolov(gettext("Ведомость амортизационных отчислений"),1,mn,gn,dkm,mk,gk,ff_it,data->window);
fprintf(ff_it,"%s\n",gettext("Налоговый учёт"));

kol_str=0;
uosvamot_prp(data->rk,&kol_str,ff_it);

fprintf(ff_it,"\
---------------------------------------------------------------------------------------------------------------------------");
strpod(mn,mk,11,ff_it);

fprintf(ff_it,gettext("\
                Наименование итога                | Балансовая |   Износ    | Остаточная |Ликвидаци.|Годов.сумм|Мес. сумма|"));
strms(0,mn,mk,ff_it);

fprintf(ff_it,gettext("\
                                                  | стоимость  |            |  стоимость | стоимость|амортизац.|амортизац.|"));


strms(1,mn,mk,ff_it);

fprintf(ff_it,"\
---------------------------------------------------------------------------------------------------------------------------");
strpod(mn,mk,11,ff_it);


char hzt[64];
char hna[64];
memset(hzt,'\0',sizeof(hzt));
memset(hna,'\0',sizeof(hna));
class iceb_u_str naim("");
int innom=0;
int innomz=0;
double itogo_hzt_bsiz[6];
double itogo_hna_bsiz[6];
double itogo_all_bsiz[6];

double itogo_hzt_amort[12];
double itogo_hna_amort[12];
double itogo_all_amort[12];
double amort_innom[12];

memset(itogo_hzt_amort,'\0',sizeof(itogo_hzt_amort));
memset(itogo_hna_amort,'\0',sizeof(itogo_hna_amort));
memset(itogo_all_amort,'\0',sizeof(itogo_hna_amort));
memset(itogo_hzt_bsiz,'\0',sizeof(itogo_hzt_bsiz));
memset(itogo_hna_bsiz,'\0',sizeof(itogo_hna_bsiz));
memset(itogo_all_bsiz,'\0',sizeof(itogo_hna_bsiz));
memset(amort_innom,'\0',sizeof(amort_innom));

class poiinpdw_data rekin;
class bsizw_data bal_st;

double bal_stoim=0.;
double bal_iznos=0.;
double ost_bal_st=0.;
double god_proc_am=0.;
double sgod_amort=0.;
double smes_amort=0.;
class iceb_u_str naios("");
double suma_amort=0.;
int metka_vs_hna=0; /*метка вывода строки*/
int metka_vs_hzt=0;
int metka_vs_innom=0;
float kolstr1=0;
double itogo_raoz[3];
memset(itogo_raoz,'\0',sizeof(itogo_raoz));

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->innom.ravno(),row[0],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->podr.ravno(),row[2],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->hzt.ravno(),row[3],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->grupa.ravno(),row[4],0,0) != 0)
   continue;
   
  innom=atoi(row[0]);
  if(iceb_u_SRAV(hna,row[4],0) != 0)
   {
    if(hna[0] != '\0' && metka_vs_hna > 0)
     {
      if(metka_vs_innom > 0)
       {
        uosvamot_nf(innomz,mn,mk,gk,amort_innom,itogo_raoz,naios.ravno(),god_proc_am,rekin.popkf,raoz.ff,data->window);
        rasmes1(mn,mk,amort_innom,0,ff_innom);
        metka_vs_innom=0;
       }
      if(metka_vs_hzt > 0)
       {
        uosvamot1_itogo(2,hzt,mn,mk,itogo_hzt_bsiz,itogo_hzt_amort,ff_it,data->window);
        uosvamot1_itog(2,hzt,mn,mk,itogo_hzt_bsiz,itogo_hzt_amort,ff_innom,data->window);
        metka_vs_hzt=0;
       }     
      uosvamot1_itogo(1,hna,mn,mk,itogo_hna_bsiz,itogo_hna_amort,ff_it,data->window);
      uosvamot1_itog(1,hna,mn,mk,itogo_hna_bsiz,itogo_hna_amort,ff_innom,data->window);
      metka_vs_hna=0;
     }
    naim.new_plus("");
    god_proc_am=0.;    
    sprintf(strsql,"select naik,kof from Uosgrup where kod='%s'",row[4]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      naim.new_plus(row1[0]);
      god_proc_am=atof(row1[1]);
     }
    fprintf(ff_innom,"\n%s:%s %s\n",gettext("Группа"),row[4],naim.ravno());

    strncpy(hna,row[4],sizeof(hna)-1);
   }

  if(iceb_u_SRAV(hzt,row[3],0) != 0)
   {
    if(hzt[0] != '\0' && metka_vs_hzt > 0)
     {
      if(metka_vs_innom > 0)
       {
        uosvamot_nf(innomz,mn,mk,gk,amort_innom,itogo_raoz,naios.ravno(),god_proc_am,rekin.popkf,raoz.ff,data->window);
        rasmes1(mn,mk,amort_innom,0,ff_innom);
        metka_vs_innom=0;
       }
      uosvamot1_itogo(2,hzt,mn,mk,itogo_hzt_bsiz,itogo_hzt_amort,ff_it,data->window);
      uosvamot1_itog(2,hzt,mn,mk,itogo_hzt_bsiz,itogo_hzt_amort,ff_innom,data->window);
      metka_vs_hzt=0;
     }    
    naim.new_plus("");
    sprintf(strsql,"select naik from Uoshz where kod='%s'",row[3]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     naim.new_plus(row1[0]);

    fprintf(ff_innom,"\n%s:%s %s\n",gettext("Шифр затрат"),row[3],naim.ravno());

    strncpy(hzt,row[3],sizeof(hzt)-1);
   }
  short mes_amort=0;
  short d=0,g=0;
  iceb_u_rsdat(&d,&mes_amort,&g,row[1],2);
  if(poiinpdw(atoi(row[0]),mes_amort,gn,&rekin,data->window) != 0)
   {
    fprintf(ff_innom,"%d инвентарный номер не числится %d.%dг. !!!\n",innom,mes_amort,gn);


    sprintf(strsql,"%d инвентарный номер не числится %d.%dг. !!!",innom,mes_amort,gn);
    iceb_menu_soob(strsql,data->window);
    continue;
   }

  if(iceb_u_proverka(data->rk->shetu.ravno(),rekin.shetu.ravno(),0,0) != 0)
   {
    continue;
   }
  
  if(data->rk->sost_ob == 1)
   if(rekin.soso == 1 || rekin.soso == 2)
    {
     continue;
    }
  if(data->rk->sost_ob == 2)
   if(rekin.soso == 0 || rekin.soso == 3)
    {
     continue;
    }         
  if(iceb_u_proverka(data->rk->hau.ravno(),rekin.hau.ravno(),0,0) != 0)
   continue;


  if(innomz != innom)
   {
    if(innomz != 0 && metka_vs_innom > 0 )
     {
      uosvamot_nf(innomz,mn,mk,gk,amort_innom,itogo_raoz,naios.ravno(),god_proc_am,rekin.popkf,raoz.ff,data->window);
      rasmes1(mn,mk,amort_innom,0,ff_innom);
      metka_vs_innom=0;
     }
    bal_st.clear();
    bsizw(innom,atoi(row[2]),1,mn,gn,&bal_st,NULL,data->window);
    
    bal_stoim=bal_st.bs+bal_st.sbs;
    bal_iznos=bal_st.iz+bal_st.iz1+bal_st.siz;
    ost_bal_st=bal_stoim-bal_iznos;
    
    double lik_st=uosgetlsw(innom,1,mn,gn,0,data->window);

    sgod_amort=(bal_stoim-lik_st)*god_proc_am/100.*rekin.popkf;
    smes_amort=sgod_amort/12.;
    
    itogo_hna_bsiz[0]+=bal_stoim;
    itogo_hna_bsiz[1]+=bal_iznos;
    itogo_hna_bsiz[2]+=ost_bal_st;
    itogo_hna_bsiz[3]+=lik_st;
    itogo_hna_bsiz[4]+=sgod_amort;
    itogo_hna_bsiz[5]+=smes_amort;

    itogo_hzt_bsiz[0]+=bal_stoim;
    itogo_hzt_bsiz[1]+=bal_iznos;
    itogo_hzt_bsiz[2]+=ost_bal_st;
    itogo_hzt_bsiz[3]+=lik_st;
    itogo_hzt_bsiz[4]+=sgod_amort;
    itogo_hzt_bsiz[5]+=smes_amort;

    itogo_all_bsiz[0]+=bal_stoim;
    itogo_all_bsiz[1]+=bal_iznos;
    itogo_all_bsiz[2]+=ost_bal_st;
    itogo_all_bsiz[3]+=lik_st;
    itogo_all_bsiz[4]+=sgod_amort;
    itogo_all_bsiz[5]+=smes_amort;
        
    naios.new_plus("");
    sprintf(strsql,"select naim from Uosin where innom=%d",innom);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден инвентарный номер"),innom);
      iceb_menu_soob(strsql,data->window);
     }
    else
     naios.new_plus(row1[0]);

    fprintf(ff_innom,"%-8s|%-*.*s|%-*.*s|%-*s|",
    row[0],
    iceb_u_kolbait(40,naios.ravno()),iceb_u_kolbait(40,naios.ravno()),naios.ravno(),
    iceb_u_kolbait(10,rekin.nomz.ravno()),iceb_u_kolbait(10,rekin.nomz.ravno()),rekin.nomz.ravno(),
    iceb_u_kolbait(5,rekin.hna.ravno()),rekin.hna.ravno());

    fprintf(ff_innom,"%5.2f|%12.2f|%12.2f|%12.2f|%10.2f|%10.2f|%10.2f|%5.2f|",god_proc_am,bal_stoim,bal_iznos,ost_bal_st,lik_st,sgod_amort,smes_amort,rekin.popkf);

    
    innomz=innom;
   }  
  suma_amort=atof(row[5]);

  itogo_hzt_amort[mes_amort-1]+=suma_amort;
  itogo_hna_amort[mes_amort-1]+=suma_amort;
  itogo_all_amort[mes_amort-1]+=suma_amort;
  amort_innom[mes_amort-1]+=suma_amort;

  metka_vs_hna++;  
  metka_vs_hzt++;  
  metka_vs_innom++;  
 }

if(metka_vs_innom > 0)
 {
  uosvamot_nf(innomz,mn,mk,gk,amort_innom,itogo_raoz,naios.ravno(),god_proc_am,rekin.popkf,raoz.ff,data->window);
  rasmes1(mn,mk,amort_innom,0,ff_innom);
 }

if(metka_vs_hzt > 0)
 {
  uosvamot1_itogo(2,hzt,mn,mk,itogo_hzt_bsiz,itogo_hzt_amort,ff_it,data->window);
  uosvamot1_itog(2,hzt,mn,mk,itogo_hzt_bsiz,itogo_hzt_amort,ff_innom,data->window);
 }

if(metka_vs_hna > 0)
 {
  uosvamot1_itogo(1,hna,mn,mk,itogo_hna_bsiz,itogo_hna_amort,ff_it,data->window);
  uosvamot1_itog(1,hna,mn,mk,itogo_hna_bsiz,itogo_hna_amort,ff_innom,data->window);
 }

double iamort=0.;

for(int nom=0; nom < 12; nom++)
  iamort+=itogo_all_amort[nom];

gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace","foreground","red",NULL);

sprintf(strsql,"\
%*s:%15s\n",
iceb_u_kolbait(25,gettext("Балансовая стоимость")),
gettext("Балансовая стоимость"),
iceb_u_prnbr(itogo_all_bsiz[0]));

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"\
%*s:%15s\n",
iceb_u_kolbait(25,gettext("Износ")),
gettext("Износ"),
iceb_u_prnbr(itogo_all_bsiz[1]));

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"\
%*s:%15s\n",
iceb_u_kolbait(25,gettext("Остаточная стоимость")),
gettext("Остаточная стоимость"),
iceb_u_prnbr(itogo_all_bsiz[2]));

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"\
%*s:%15s\n",
iceb_u_kolbait(25,gettext("Амортизация")),
gettext("Амортизация"),
iceb_u_prnbr(iamort));

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

uosvamot1_itogo(3,"",mn,mk,itogo_all_bsiz,itogo_all_amort,ff_it,data->window);
uosvamot1_itog(3,"",mn,mk,itogo_all_bsiz,itogo_all_amort,ff_innom,data->window);

iceb_podpis(ff_innom,data->window);
fclose(ff_innom);

iceb_podpis(ff_it,data->window);
fclose(ff_it);

fprintf(raoz.ff,"%*s %12.2f %12.2f %12.2f\n",
iceb_u_kolbait(101,"Всього"),"Всього",
itogo_raoz[0],
itogo_raoz[1],
itogo_raoz[2]);

fprintf(raoz.ff,"ICEB_LST_END\n");
iceb_vrvr("uosvamot_ra_end.alx",NULL,raoz.ff,data->window); /*печать концовки документа*/

iceb_podpis(raoz.ff,data->window);
raoz.end();


data->rk->imaf.plus(imaf_innom);
data->rk->naim.plus(gettext("Ведомость амортизационных отчислений"));
data->rk->imaf.plus(imaf_it);
data->rk->naim.plus(gettext("Общий итог"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);

data->rk->imaf.plus(imaf_raoz);
data->rk->naim.plus(gettext("Расчёт амортизации основных средств"));

iceb_ustpeh(imaf_raoz,3,&kolstr,data->window);
iceb_rnl(imaf_raoz,kolstr,"",&uosvamot_hw,data->window);







gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
