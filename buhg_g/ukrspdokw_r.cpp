/*$Id: ukrspdokw_r.c,v 1.20 2013/09/26 09:46:56 sasa Exp $*/
/*23.05.2016	29.02.2008	Белых А.И.	ukrspdokw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "ukrspdokw.h"

class ukrspdokw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class ukrspdokw_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  ukrspdokw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   ukrspdokw_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrspdokw_r_data *data);
gint ukrspdokw_r1(class ukrspdokw_r_data *data);
void  ukrspdokw_r_v_knopka(GtkWidget *widget,class ukrspdokw_r_data *data);


extern SQL_baza bd;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int ukrspdokw_r(class ukrspdokw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class ukrspdokw_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка списка командировочных документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(ukrspdokw_r_key_press),&data);

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

repl.plus(gettext("Распечатка списка командировочных документов"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(ukrspdokw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)ukrspdokw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  ukrspdokw_r_v_knopka(GtkWidget *widget,class ukrspdokw_r_data *data)
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

gboolean   ukrspdokw_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrspdokw_r_data *data)
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
/*******************************************************/
/*Получить сумму по вертикальной колонке*/
/*******************************************************/

double ukrspdok_svk(int kolih_dat_nomd,
int kolih_kod_zat,
int nomer_kol,
class iceb_u_double *kol_kod,
class iceb_u_double *sum_kod,
double *kolih)
{
*kolih=0.;
double suma=0;
for(int ii=0; ii < kolih_dat_nomd; ii++)
 {
  *kolih+=kol_kod->ravno(ii*kolih_kod_zat+nomer_kol);
  suma+=sum_kod->ravno(ii*kolih_kod_zat+nomer_kol);
 }

return(suma);

}
/*******************/
/*вывод реквизитов поиска*/
/*******************/

void ukrspdok_rp(const char *kontr,const char *vkom,const char *kod_zatrat,FILE *ff)
{
if(kontr[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Контрагент"),kontr);
if(vkom[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Вид командировки"),vkom);
if(kod_zatrat[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код затрат"),kod_zatrat);

}


/******************************************/
/******************************************/

gint ukrspdokw_r1(class ukrspdokw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;


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




SQL_str row1;
class SQLCURSOR cur1;
int kolstr=0;

sprintf(strsql,"select nomd,datd,kont,vkom from Ukrdok where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' \
order by datd,nomd asc",gn,mn,dn,gk,mk,dk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    

//Составляем список документов
class iceb_u_spisok data_nomd;
float kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->kontr.ravno(),row[2],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->vidkom.ravno(),row[3],0,0) != 0)
    continue;

  sprintf(strsql,"%s|%s",row[1],row[0]);
  if(data_nomd.find(strsql) < 0)
   data_nomd.plus(strsql);  
 }

int kolih_datnomd=data_nomd.kolih();
if(kolih_datnomd == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

//Читаем список кодов затрат
sprintf(strsql,"select kod from Ukrkras");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_u_int kod_zat;

while(cur.read_cursor(&row) != 0)
 kod_zat.plus(row[0]);

int kolih_kod_zat=kod_zat.kolih();
class iceb_u_double kol_kod;
class iceb_u_double sum_kod;

kol_kod.make_class(kolih_kod_zat*kolih_datnomd);
sum_kod.make_class(kolih_kod_zat*kolih_datnomd);
class iceb_u_str datadok("");
class iceb_u_str nomdok("");
kolstr1=0.;
double suma=0.;
short dd,md,gd;
short dvnn,mvnn,gvnn;

FILE *ff;
char imaf_rt[64];
sprintf(imaf_rt,"ukrspdrt%d.lst",getpid());

if((ff = fopen(imaf_rt,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_rt,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_zagolov(gettext("Распечатка списка документов на командировки"),dn,mn,gn,dk,mk,gk,ff,data->window);

ukrspdok_rp(data->rk->kontr.ravno(),data->rk->vidkom.ravno(),data->rk->kod_zat.ravno(),ff);
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 Дата     |Номер док.|Наименование расхода|Счёт  |Ед.из.|Колич.|Сумма б.НДС|Cумма НДС|Номер н.н.|Дата в.н.н.|Счёт НДС|Контрагент НДС\n");
//         1234567890 12345678901234567890 123456 123456 123456 1234567890 123456789 1234567890 12345678901 12345678
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
//Заряжаем массивы
class iceb_u_str naim_kontr("");
class iceb_u_str naim_rash("");
double i_suma_bnds=0.;
double i_suma_nds=0.;
//Заряжаем массивы
for(int ii=0; ii < kolih_datnomd; ii++)
 {
  iceb_pbar(data->bar,kolih_datnomd,++kolstr1);    
  iceb_u_polen(data_nomd.ravno(ii),&datadok,1,'|');
  iceb_u_polen(data_nomd.ravno(ii),&nomdok,2,'|');
  sprintf(strsql,"select kodr,kolih,cena,snds,shet,ediz,ndrnn,dvnn,kdrnn,sn,ss from Ukrdok1 where datd='%s' and nomd='%s'",datadok.ravno(),nomdok.ravno());
  
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  suma=0.;
  int kodr=0;
  int kolih_zat=0;
  int nomer_kodzat=0;
  while(cur.read_cursor(&row) != 0)
   {
    if(iceb_u_proverka(data->rk->kod_zat.ravno(),row[0],0,0) != 0)
     continue;
    kodr=atoi(row[0]);
    kolih_zat=atoi(row[1]);
    suma=atof(row[2])+atof(row[3])+atof(row[10]);
    nomer_kodzat=kod_zat.find(kodr);
    kol_kod.plus(kolih_zat,ii*kolih_kod_zat+nomer_kodzat);
    sum_kod.plus(suma,ii*kolih_kod_zat+nomer_kodzat);

    i_suma_bnds+=atof(row[2])+atof(row[10]);
    i_suma_nds+=atof(row[3]);
    /*Узнаём наименование расхода*/
    naim_rash.new_plus(row[0]);
    sprintf(strsql,"select naim from Ukrkras where kod=%d",naim_rash.ravno_atoi());
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      naim_rash.plus(" ");
      naim_rash.plus(row1[0]);
     }  
    /*узнаём наименование контрагента*/
    naim_kontr.new_plus(row[8]);
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",naim_kontr.ravno());
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      naim_kontr.plus(" ");
      naim_kontr.plus(row1[0]);
     }    
    iceb_u_rsdat(&dd,&md,&gd,datadok.ravno(),2);
    iceb_u_rsdat(&dvnn,&mvnn,&gvnn,row[7],2);

    
    fprintf(ff,"%02d.%02d.%04d %-*s %-*s %-*s %-*s %6.6g %10.2f %9.2f %-10s %02d.%02d.%04d  %-*s %.*s\n",
    dd,md,gd,
    iceb_u_kolbait(10,row[0]),row[0],
    iceb_u_kolbait(20,naim_rash.ravno()),naim_rash.ravno(),
    iceb_u_kolbait(6,row[4]),row[4],
    iceb_u_kolbait(6,row[5]),row[5],
    atof(row[1]),atof(row[2]),atof(row[3]),row[6],dvnn,mvnn,gvnn,
    iceb_u_kolbait(8,row[9]),row[9],
    iceb_u_kolbait(35,naim_kontr.ravno()),naim_kontr.ravno());
            
   } 
 }
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s %10.2f %9.2f\n",iceb_u_kolbait(63,gettext("Итого")),gettext("Итого"),i_suma_bnds,i_suma_nds);
iceb_podpis(ff,data->window);
fclose(ff);

char imaf[64];
sprintf(imaf,"ukrspd%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_zagolov(gettext("Список авансовых отчётов"),dn,mn,gn,dk,mk,gk,ff,data->window);

ukrspdok_rp(data->rk->kontr.ravno(),data->rk->vidkom.ravno(),data->rk->kod_zat.ravno(),ff);

 
class iceb_u_str liniq;
class iceb_u_str strpb("");

liniq.plus("-------------------------------------------------------------------------------------------");

double kolih_kol;
int kolih_znk=0;
for(int ii=0; ii < kolih_kod_zat; ii++)
 {
  if(ukrspdok_svk(kolih_datnomd,kolih_kod_zat,ii,&kol_kod,&sum_kod,&kolih_kol) == 0.)
   continue;
  kolih_znk++;
  liniq.plus("-----------------");
  strpb.plus("                 ");
 }
//полка над Итого
if(kolih_znk > 1)
 {
  liniq.plus("-----------------");
  strpb.plus("                 ");
 }

liniq.plus("--------------------");

fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,gettext("\
   Дата   | Номер  |     Фамилия        |  Дата    |   Дата   | Номер  | Номер |   Дата   |"));
class iceb_u_str naim_kod_zat("");
for(int ii=0; ii < kolih_kod_zat; ii++)
 {
  if(ukrspdok_svk(kolih_datnomd,kolih_kod_zat,ii,&kol_kod,&sum_kod,&kolih_kol) == 0.)
   continue;
  naim_kod_zat.new_plus(kod_zat.ravno(ii));
  sprintf(strsql,"select naim from Ukrkras where kod=%d",kod_zat.ravno(ii));

  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   naim_kod_zat.plus(" ",row[0]);  
  fprintf(ff,"%-*.*s|",iceb_u_kolbait(16,naim_kod_zat.ravno()),iceb_u_kolbait(16,naim_kod_zat.ravno()),naim_kod_zat.ravno());

 }

if(kolih_znk > 1)
 fprintf(ff,"%-*.*s|",iceb_u_kolbait(16,gettext("Итого")),iceb_u_kolbait(16,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",gettext("Пунк назначения"));

fprintf(ff,gettext("\
документа |доку-нта|                    | начала   |   конца  |приказа |авансо.|авансового|"));

for(int ii=0; ii < kolih_kod_zat; ii++)
 {
  if(ukrspdok_svk(kolih_datnomd,kolih_kod_zat,ii,&kol_kod,&sum_kod,&kolih_kol) == 0.)
   continue;
  naim_kod_zat.new_plus(kod_zat.ravno(ii));
  sprintf(strsql,"select naim from Ukrkras where kod=%d",kod_zat.ravno(ii));
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   naim_kod_zat.plus(" ",row[0]);  

  fprintf(ff,"%-*.*s|",
  iceb_u_kolbait(16,iceb_u_adrsimv(16,naim_kod_zat.ravno())),
  iceb_u_kolbait(16,iceb_u_adrsimv(16,naim_kod_zat.ravno())),
  iceb_u_adrsimv(16,naim_kod_zat.ravno()));

 }

if(kolih_znk > 1)
 fprintf(ff,"%-16.16s|","");

fprintf(ff,"%s\n",gettext("Организация"));


fprintf(ff,gettext("\
          |        |                    |командиро.|командиро.|        |отчёта |  отчёта  |"));

for(int ii=0; ii < kolih_kod_zat; ii++)
 {
  if(ukrspdok_svk(kolih_datnomd,kolih_kod_zat,ii,&kol_kod,&sum_kod,&kolih_kol) == 0.)
   continue;
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_u_kolbait(5,gettext("Коли.")),
  iceb_u_kolbait(5,gettext("Коли.")),
  gettext("Коли."),
  iceb_u_kolbait(10,gettext("Сумма")),
  iceb_u_kolbait(10,gettext("Сумма")),
  gettext("Сумма"));
 }

if(kolih_znk > 1)
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_u_kolbait(5,gettext("Коли.")),
  iceb_u_kolbait(5,gettext("Коли.")),
  gettext("Коли."),
  iceb_u_kolbait(10,gettext("Сумма")),
  iceb_u_kolbait(10,gettext("Сумма")),
  gettext("Сумма"));

fprintf(ff,"%s\n",gettext("Цель командировки"));
   
fprintf(ff,"%s\n",liniq.ravno());

/*****************
fprintf(ff,gettext("\
   Дата   | Номер  |     Фамилия        |  Дата    |   Дата   | Номер  | Номер |   Дата   |Пунк назначения\n\
документа |доку-нта|                    | начала   |   конца  |приказа |авансо.|авансового|Организация\n\
          |        |                    |командиро.|командиро.|        |отчёта |  отчёта  |Цель командировки\n"));
fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
1234567890 12345678 12345678901234567890 1234567890 1234567890 12345678 1234567 1234567890
*/


short d,m,g; 
class iceb_u_str data_n("");
class iceb_u_str data_k("");
class iceb_u_str data_ao("");
class iceb_u_str fio("");
kolstr1=0;
double itogo_gor_kol=0.;
double itogo_gor_sum=0.;

for(int ii=0; ii < kolih_datnomd; ii++)
 {
//  strzag(LINES-1,0,kolih_datnomd,++kolstr1);
  iceb_pbar(data->bar,kolih_datnomd,++kolstr1);    

  iceb_u_polen(data_nomd.ravno(ii),&datadok,1,'|');
  iceb_u_polen(data_nomd.ravno(ii),&nomdok,2,'|');

  sprintf(strsql,"select * from Ukrdok where datd='%s' and nomd='%s'",datadok.ravno(),nomdok.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,"%s:%s %s",gettext("Не найден документ"),datadok.ravno(),nomdok.ravno());
    iceb_menu_soob(strsql,data->window);    
    continue;
   }
  
  fio.new_plus(row[3]);
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[3]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    fio.plus(" ",row1[0]);
   }

  iceb_u_rsdat(&dd,&md,&gd,row[2],2);
  
  iceb_u_rsdat(&d,&m,&g,row[7],2);
  if(d != 0)
   {
    sprintf(strsql,"%02d.%02d.%d",d,m,g);
    data_n.new_plus(strsql);
   }   
  else
   data_n.new_plus("");

  iceb_u_rsdat(&d,&m,&g,row[8],2);
  if(d != 0)
   {
    sprintf(strsql,"%02d.%02d.%d",d,m,g);
    data_k.new_plus(strsql);
   }  
  else
   data_k.new_plus("");

  iceb_u_rsdat(&d,&m,&g,row[15],2);
  if(d != 0)
   {
    sprintf(strsql,"%02d.%02d.%d",d,m,g);
    data_ao.new_plus(strsql);
   }
  else
   data_ao.new_plus("");
  fprintf(ff,"%02d.%02d.%d %-*s %-*.*s %10s %10s %-*s %-*s %10s ",
  dd,md,gd,
  iceb_u_kolbait(8,row[1]),row[1],
  iceb_u_kolbait(20,fio.ravno()),iceb_u_kolbait(20,fio.ravno()),fio.ravno(),
  data_n.ravno(),data_k.ravno(),
  iceb_u_kolbait(8,row[9]),row[9],
  iceb_u_kolbait(7,row[14]),row[14],
  data_ao.ravno());
  
  itogo_gor_kol=0.;
  itogo_gor_sum=0.;
  for(int ii1=0; ii1 < kolih_kod_zat; ii1++)
   {
    if(ukrspdok_svk(kolih_datnomd,kolih_kod_zat,ii1,&kol_kod,&sum_kod,&kolih_kol) == 0.)
     continue;
  
    fprintf(ff,"%5.5g %10.2f ",kol_kod.ravno(ii*kolih_kod_zat+ii1),sum_kod.ravno(ii*kolih_kod_zat+ii1));
    itogo_gor_kol+=kol_kod.ravno(ii*kolih_kod_zat+ii1);
    itogo_gor_sum+=sum_kod.ravno(ii*kolih_kod_zat+ii1);
   }
  if(kolih_znk > 1)
   fprintf(ff,"%5.5g %10.2f ",itogo_gor_kol,itogo_gor_sum);

  fprintf(ff,"%s\n",row[4]);

  fprintf(ff,"%19s %-*.*s %49s %s%s\n",
  " ",
  iceb_u_kolbait(20,iceb_u_adrsimv(20,fio.ravno())),
  iceb_u_kolbait(20,iceb_u_adrsimv(20,fio.ravno())),
  iceb_u_adrsimv(20,fio.ravno()),
  " ",strpb.ravno(),row[5]);    
  
  fprintf(ff,"%19s %-*.*s %49s %s%s\n",
  " ",
  iceb_u_kolbait(20,iceb_u_adrsimv(40,fio.ravno())),
  iceb_u_kolbait(20,iceb_u_adrsimv(40,fio.ravno())),
  iceb_u_adrsimv(40,fio.ravno()),
  " ",strpb.ravno(),row[6]);    

 }
fprintf(ff,"%s\n",liniq.ravno());

if(kolih_znk > 0)
 {
  fprintf(ff,"%*s ",iceb_u_kolbait(90,gettext("Итого")),gettext("Итого"));

  itogo_gor_kol=0.;
  itogo_gor_sum=0.;
  for(int ii=0; ii < kolih_kod_zat; ii++)
   {
    
    if((suma=ukrspdok_svk(kolih_datnomd,kolih_kod_zat,ii,&kol_kod,&sum_kod,&kolih_kol)) == 0.)
       continue;
    fprintf(ff,"%5.5g %10.2f ",kolih_kol,suma);
    itogo_gor_kol+=kolih_kol;
    itogo_gor_sum+=suma;  
   }
  if(kolih_znk > 1)
    fprintf(ff,"%5.5g %10.2f",itogo_gor_kol,itogo_gor_sum);
  fprintf(ff,"\n");  
 }
iceb_podpis(ff,data->window);

fclose(ff);


data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Список документов"));
data->rk->imaf.plus(imaf_rt);
data->rk->naim.plus(gettext("Содержимое авансовых документов"));

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
