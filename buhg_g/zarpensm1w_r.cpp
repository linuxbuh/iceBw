/*$Id: zarpensm1w_r.c,v 1.27 2013/12/31 11:49:16 sasa Exp $*/
/*19.06.2019	21.02.2010	Белых А.И.	zarpensm1w_r.c
Расчёт отчёта 

*/
#include <errno.h>
#include "buhg_g.h"
#include "zarpensmw.h"
#include        "dbfhead.h"

class zarpensm1_nastr
 {
  public: 
   class iceb_u_str kodpfu; /*код пенсионного фонда Украины*/
   class iceb_u_str kodfil; /*код филиала*/
   class iceb_u_str kodzvna; /*код звания неатестованых*/
   class iceb_u_str kod_tab_bol; /*Коды табеля больничных*/
   class iceb_u_str kod_tab_bsz; /*Коды табеля без сохранения зарплаты*/
   class iceb_u_str kod_tab_kdtv;

   class iceb_u_str kod_nah_berem; /*Коды начисления помощи по беременности и родам*/
   class iceb_u_str kod_tab_berem; /*Код табеля по беременности и родам*/

   class iceb_u_int kodtn; /*коды типов начисления*/
   class iceb_u_spisok kodnah; /*начисления для кодов типов начислений*/
   
  zarpensm1_nastr()
   {
    kodpfu.new_plus("");
    kodfil.new_plus("");
    kodzvna.new_plus("");
    kod_tab_bol.new_plus("");
    kod_tab_bsz.new_plus("");
    kod_tab_kdtv.new_plus("");
    kod_nah_berem.new_plus("");
    kod_tab_berem.new_plus("");
   }
 };

class zarpensm1w_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class zarpensmw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zarpensm1w_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zarpensm1w_r_key_press(GtkWidget *widget,GdkEventKey *event,class zarpensm1w_r_data *data);
gint zarpensm1w_r1(class zarpensm1w_r_data *data);
void  zarpensm1w_r_v_knopka(GtkWidget *widget,class zarpensm1w_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;
extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_bol_inv; /*Код удержания единого социального взноса*/
extern int kodf_esv_bol; /*Код наячисления единого социального взноса*/
extern int kodf_esv_bol_inv; /*Код начисления единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/
extern class iceb_u_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/
extern short *kodbl;  /*Коды начисления больничного*/
extern int kodf_esv_bol; /*Код фонда удержания единого социального взноса*/
extern int kodf_esv_bol_inv; /*Код фонда удержания единого социального взноса*/

int zarpensm1w_r(class zarpensmw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zarpensm1w_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка списка документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zarpensm1w_r_key_press),&data);

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

repl.plus(gettext("Распечатка списка документов"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zarpensm1w_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zarpensm1w_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zarpensm1w_r_v_knopka(GtkWidget *widget,class zarpensm1w_r_data *data)
{
//printf("zarpensm1w_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zarpensm1w_r_key_press(GtkWidget *widget,GdkEventKey *event,class zarpensm1w_r_data *data)
{
// printf("zarpensm1w_r_key_press\n");
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}
/***************************************/
/*шапка всех додатка 4 для таблиц 5,6,7*/
/****************************************/
void zarpensm1_hd4(const char *inn_or,const char *naim_kontr00,short mr,short gr,FILE *ff_xml)
{
class iceb_u_str naim(naim_kontr00);

fprintf(ff_xml," <DECLARBODY>\n");
fprintf(ff_xml,"   <HTIN>%s</HTIN>\n",inn_or);
fprintf(ff_xml,"   <HNAME>%s</HNAME>\n",naim.ravno_filtr_xml());
fprintf(ff_xml,"   <HZM>%d</HZM>\n",mr);
fprintf(ff_xml,"   <HZY>%d</HZY>\n",gr);
fprintf(ff_xml,"   <HZB>1</HZB>\n"); /*тип початкова*/
}
/**************************************/
/*концовка таблиц*/
/*****************************************/
void zarpensm1_end_tab_xml(short dt,short mt,short gt,
class iceb_fioruk_rk *kerivnik,
class iceb_fioruk_rk *glavbuh,
FILE *ff_xml)
{
fprintf(ff_xml,"   <HFILL>%02d%02d%d</HFILL>\n",dt,mt,gt);
fprintf(ff_xml,"   <HBOS>%s</HBOS>\n",kerivnik->fio.ravno_filtr_xml());
fprintf(ff_xml,"   <HKBOS>%s</HKBOS>\n",kerivnik->inn.ravno());
fprintf(ff_xml,"   <HBUH>%s</HBUH>\n",glavbuh->fio.ravno_filtr_xml());
fprintf(ff_xml,"   <HKBUH>%s</HKBUH>\n",glavbuh->inn.ravno());
fprintf(ff_xml,"  </DECLARBODY>\n");
fprintf(ff_xml,"</DECLAR>\n");
}
/********************************/
/*Чтение настроек*/
/********************************/
int zarpensm1_read_nast(class zarpensm1_nastr *nastr,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[1024];
class iceb_u_str naim("");
int kodtn=0;
sprintf(strsql,"select str from Alx where fil='zarpensm.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"zarpensm1.alx");
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],&naim,1,'|') != 0)
   continue;  
  if(iceb_u_SRAV("Код отделения пенсионного фонда Украины",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->kodpfu,2,'|');
    continue;
   }
  if(iceb_u_SRAV("Код филиала пенсионного фонда",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->kodfil,2,'|');
    continue;
   }

  if(iceb_u_SRAV("Коды начислений для кода типа начисления",naim.ravno(),1) == 0)
   {
    if(iceb_u_polen(naim.ravno(),&kodtn,7,' ') == 0)
     {
      if(kodtn == 0)
       continue;
      nastr->kodtn.plus(kodtn,-1);
   
      iceb_u_polen(row_alx[0],&naim,2,'|');
      nastr->kodnah.plus(naim.ravno());
     }   
    continue;
   }
  if(iceb_u_SRAV("Коды звания неатестованных",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->kodzvna,2,'|');
    continue;
   }
  if(iceb_u_SRAV("Коды табеля больничных",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->kod_tab_bol,2,'|');
    continue;
   }
  if(iceb_u_SRAV("Коды табеля без сохранения зарплаты",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->kod_tab_bsz,2,'|');
    continue;
   }
  if(iceb_u_SRAV("Коды табеля прeбывания в трудовых отношениях",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->kod_tab_kdtv,2,'|');
    continue;
   }

  if(iceb_u_SRAV("Коды начисления помощи по беременности и родам",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->kod_nah_berem,2,'|');
    continue;
   }

  if(iceb_u_SRAV("Коды табеля по беременности и родам",naim.ravno(),0) == 0)
   {
    iceb_u_polen(row_alx[0],&nastr->kod_tab_berem,2,'|');
    continue;
   }
 }
return(0);
}
/************************************/
/*концовка распечатки*/
/*************************************/

void zarpensm1_kon_lst(int kolstr,
const char *inn_ker,
const char *fio_ker,
const char *inn_gb,
const char *fio_gb,
FILE *ff)
{
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

fprintf(ff,"\
Кількість заповнених рядків %d\n\
Дата формування звіту у страхувальника %d.%d.%d \n\n\
Керівник           %-10s _______________ %s\n\n\
M.П.\n\n\
Головний бухгалтер %-10s ________________ %s\n",
kolstr,
dt,mt,gt,
inn_ker,
fio_ker,
inn_gb,
fio_gb); 


}

/*********************************************/
/*вывод в таблицу 6*/
/*******************************/
void zarpensm1_str_tab6(int nomer_str_tab6,
short mr,short gr, /*дата расчёта*/
const char *inn,
const char *fio,
int metka_ukr_grom,
int kodtipnah,
const char *metka_pola,
const char *mv_gv, /*Месяц и год в счёт которых выплачено*/
double suma_obh,
double suma,
double suma_esv_v,
int metka_otk,
int metka_exp,
const char *tabnom,
int zo,
int kd_np,
int kd_nzp,
int kd_ptv,
int kd_berem,
int nrm, 
double sum_diff,
double sum_narah,
FILE *ff_tab6,
FILE *ff_xml_tab6)
{
short mv=0,gv=0;
class iceb_u_str prizv("");
class iceb_u_str ima("");
class iceb_u_str othestvo("");

if(iceb_u_polen(fio,&prizv,1,' ') != 0)
 prizv.new_plus(fio);
 
iceb_u_polen(fio,&ima,2,' ');
iceb_u_polen(fio,&othestvo,3,' ');

iceb_u_rsdat1(&mv,&gv,mv_gv);

fprintf(ff_tab6,"%5d|%d|%s|%-*s|%-*.*s|%4d|%4d|%02d.%04d|%2d|%2d|%2d|%2d|%10.2f|%10.2f|%8.2f|%8.2f|%10.2f|%4d|%4d|%s\n",
nomer_str_tab6,
metka_ukr_grom,
metka_pola,
iceb_u_kolbait(10,inn),inn,
iceb_u_kolbait(30,fio), 
iceb_u_kolbait(30,fio), 
fio,
zo,
kodtipnah,
mv,gv,
kd_np,
kd_nzp,
kd_ptv,
kd_berem,
suma_obh,
suma,
sum_diff,
suma_esv_v,
sum_narah,
metka_otk,metka_exp,
tabnom);

int metka_pola_int=0;

if(iceb_u_SRAV(metka_pola,"Ч",0) == 0)
 metka_pola_int=1;

int metka_nrc=0; /*метка если 0 полное рабочее время 1- не полное рабочее время*/


/*формат налоговой*/
fprintf(ff_xml_tab6,"   <T1RXXXXG6 ROWNUM=\"%d\">%d</T1RXXXXG6>\n",nomer_str_tab6,metka_ukr_grom); //6
fprintf(ff_xml_tab6,"   <T1RXXXXG7 ROWNUM=\"%d\">%d</T1RXXXXG7>\n",nomer_str_tab6,metka_pola_int); //7

if(inn[0] != '\0')
 fprintf(ff_xml_tab6,"   <T1RXXXXG8S ROWNUM=\"%d\">%s</T1RXXXXG8S>\n",nomer_str_tab6,inn);          //8

fprintf(ff_xml_tab6,"   <T1RXXXXG9 ROWNUM=\"%d\">%d</T1RXXXXG9>\n",nomer_str_tab6,zo);             //9
fprintf(ff_xml_tab6,"   <T1RXXXXG10 ROWNUM=\"%d\">%d</T1RXXXXG10>\n",nomer_str_tab6,kodtipnah);    //10
fprintf(ff_xml_tab6,"   <T1RXXXXG111 ROWNUM=\"%d\">%d</T1RXXXXG111>\n",nomer_str_tab6,mv);         //11
fprintf(ff_xml_tab6,"   <T1RXXXXG112 ROWNUM=\"%d\">%d</T1RXXXXG112>\n",nomer_str_tab6,gv);         //11

fprintf(ff_xml_tab6,"   <T1RXXXXG121S ROWNUM=\"%d\">%s</T1RXXXXG121S>\n",nomer_str_tab6,iceb_u_filtr_xml(prizv.ravno()));   //12
fprintf(ff_xml_tab6,"   <T1RXXXXG122S ROWNUM=\"%d\">%s</T1RXXXXG122S>\n",nomer_str_tab6,iceb_u_filtr_xml(ima.ravno()));     //12
fprintf(ff_xml_tab6,"   <T1RXXXXG123S ROWNUM=\"%d\">%s</T1RXXXXG123S>\n",nomer_str_tab6,iceb_u_filtr_xml(othestvo.ravno()));//12

fprintf(ff_xml_tab6,"   <T1RXXXXG13 ROWNUM=\"%d\">%d</T1RXXXXG13>\n",nomer_str_tab6,kd_np); //13 кількість календарних днів тимчасової непрацездатності
fprintf(ff_xml_tab6,"   <T1RXXXXG14 ROWNUM=\"%d\">%d</T1RXXXXG14>\n",nomer_str_tab6,kd_nzp); //14 Кількість календарних днів без збереження заробітної плати
fprintf(ff_xml_tab6,"   <T1RXXXXG15 ROWNUM=\"%d\">%d</T1RXXXXG15>\n",nomer_str_tab6,kd_ptv); //15 кількість календарних днів перебування у трудових/цівільно-правових  відносинах протягом звітного місяця
fprintf(ff_xml_tab6,"   <T1RXXXXG16 ROWNUM=\"%d\">%d</T1RXXXXG16>\n",nomer_str_tab6,kd_berem);  /*16 кількість календарних днів відпустки в зв’язку з вагітністю та пологами*/

fprintf(ff_xml_tab6,"   <T1RXXXXG17 ROWNUM=\"%d\">%.2f</T1RXXXXG17>\n",nomer_str_tab6,suma_obh);  //17
fprintf(ff_xml_tab6,"   <T1RXXXXG18 ROWNUM=\"%d\">%.2f</T1RXXXXG18>\n",nomer_str_tab6,suma);      //18
fprintf(ff_xml_tab6,"   <T1RXXXXG19 ROWNUM=\"%d\">%.2f</T1RXXXXG19>\n",nomer_str_tab6,sum_diff);  //19
fprintf(ff_xml_tab6,"   <T1RXXXXG20 ROWNUM=\"%d\">%.2f</T1RXXXXG20>\n",nomer_str_tab6,suma_esv_v);//20
fprintf(ff_xml_tab6,"   <T1RXXXXG21 ROWNUM=\"%d\">%.2f</T1RXXXXG21>\n",nomer_str_tab6,sum_narah); //21

fprintf(ff_xml_tab6,"   <T1RXXXXG22 ROWNUM=\"%d\">%d</T1RXXXXG22>\n",nomer_str_tab6,metka_otk); //22
fprintf(ff_xml_tab6,"   <T1RXXXXG23 ROWNUM=\"%d\">%d</T1RXXXXG23>\n",nomer_str_tab6,metka_exp); //23
fprintf(ff_xml_tab6,"   <T1RXXXXG24 ROWNUM=\"%d\">%d</T1RXXXXG24>\n",nomer_str_tab6,metka_nrc); //24
fprintf(ff_xml_tab6,"   <T1RXXXXG25 ROWNUM=\"%d\">%d</T1RXXXXG25>\n",nomer_str_tab6,nrm);       //25

}
/**********************************************/
/*Вывод строки в таблицу 5*/
/********************************/
void zarpensm1_str_tab5(int tabnom,int nomer_str_tab5,
const char *inn,
const char *fio,
short dpnr,
short dusr,
short mr,short gr,
int metka_ukr_grom,
int zo, /*категория работника*/
FILE *ff_tab5,
FILE *ff_xml_tab5,
GtkWidget *wpredok)
{
class iceb_u_str prizv("");
class iceb_u_str ima("");
class iceb_u_str othestvo("");
if(iceb_u_polen(fio,&prizv,1,' ') != 0)
 prizv.new_plus(fio);
iceb_u_polen(fio,&ima,2,' ');
iceb_u_polen(fio,&othestvo,3,' ');

fprintf(ff_tab5,"%5d|%-*s|%-*.*s|%3d|%7d|%7d|\n",
nomer_str_tab5,
iceb_u_kolbait(10,inn),inn,
iceb_u_kolbait(40,fio), 
iceb_u_kolbait(40,fio), 
fio,
zo,
dpnr,
dusr);

/*формат налоговой*/
#if 0
############################################################
предыдушая форма

fprintf(ff_xml_tab5,"   <T1RXXXXG6 ROWNUM=\"%d\">%d</T1RXXXXG6>\n",nomer_str_tab5,metka_ukr_grom); //6
fprintf(ff_xml_tab5,"   <T1RXXXXG7 ROWNUM=\"%d\">%d</T1RXXXXG7>\n",nomer_str_tab5,zo); //7
fprintf(ff_xml_tab5,"   <T1RXXXXG8 ROWNUM=\"%d\">%d</T1RXXXXG8>\n",nomer_str_tab5,0); //8 договір ЦПХ 1-за основним місцем роботи 0-за сумісництвом
if(inn[0] != '\0')
   fprintf(ff_xml_tab5,"   <T1RXXXXG9S ROWNUM=\"%d\">%s</T1RXXXXG9S>\n",nomer_str_tab5,inn); //9

fprintf(ff_xml_tab5,"   <T1RXXXXG101S ROWNUM=\"%d\">%s</T1RXXXXG101S>\n",nomer_str_tab5,prizv.ravno_filtr_xml()); //10
fprintf(ff_xml_tab5,"   <T1RXXXXG102S ROWNUM=\"%d\">%s</T1RXXXXG102S>\n",nomer_str_tab5,ima.ravno_filtr_xml()); //10
fprintf(ff_xml_tab5,"   <T1RXXXXG103S ROWNUM=\"%d\">%s</T1RXXXXG103S>\n",nomer_str_tab5,othestvo.ravno_filtr_xml()); //10

if(dpnr != 0)
 fprintf(ff_xml_tab5,"   <T1RXXXXG111 ROWNUM=\"%d\">%d</T1RXXXXG111>\n",nomer_str_tab5,dpnr); //11
if(dusr != 0)
 fprintf(ff_xml_tab5,"   <T1RXXXXG112 ROWNUM=\"%d\">%d</T1RXXXXG112>\n",nomer_str_tab5,dusr); //11

#########################################################
#endif
SQL_str row;
class SQLCURSOR cur;
char strsql[1024];

class iceb_u_str voen_zvanie("");
class iceb_u_str dolgn("");

sprintf(strsql,"select dolg,zvan from Kartb where tabn=%d",tabnom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  voen_zvanie.new_plus(row[1]);
  dolgn.new_plus(row[0]);
 }
else
 {
  sprintf(strsql,"%s-%s %d!",__FUNCTION__,gettext("Не найдена карточка для табельного номера"),tabnom);
  iceb_menu_soob(strsql,NULL);
 }



fprintf(ff_xml_tab5,"   <T1RXXXXG6 ROWNUM=\"%d\">%d</T1RXXXXG6>\n",nomer_str_tab5,metka_ukr_grom); //6
fprintf(ff_xml_tab5,"   <T1RXXXXG7 ROWNUM=\"%d\">%d</T1RXXXXG7>\n",nomer_str_tab5,0); //7 договір ЦПХ 1-за основним місцем роботи 0-за сумісництвом
fprintf(ff_xml_tab5,"   <T1RXXXXG8 ROWNUM=\"%d\">%d</T1RXXXXG8>\n",nomer_str_tab5,zo); //8

if(inn[0] != '\0')
   fprintf(ff_xml_tab5,"   <T1RXXXXG9S ROWNUM=\"%d\">%s</T1RXXXXG9S>\n",nomer_str_tab5,inn); //9

fprintf(ff_xml_tab5,"   <T1RXXXXG101S ROWNUM=\"%d\">%s</T1RXXXXG101S>\n",nomer_str_tab5,prizv.ravno_filtr_xml()); //10
fprintf(ff_xml_tab5,"   <T1RXXXXG102S ROWNUM=\"%d\">%s</T1RXXXXG102S>\n",nomer_str_tab5,ima.ravno_filtr_xml()); //10
fprintf(ff_xml_tab5,"   <T1RXXXXG103S ROWNUM=\"%d\">%s</T1RXXXXG103S>\n",nomer_str_tab5,othestvo.ravno_filtr_xml()); //10

if(dpnr != 0)
 fprintf(ff_xml_tab5,"   <T1RXXXXG111 ROWNUM=\"%d\">%d</T1RXXXXG111>\n",nomer_str_tab5,dpnr); //11
if(dusr != 0)
 fprintf(ff_xml_tab5,"   <T1RXXXXG112 ROWNUM=\"%d\">%d</T1RXXXXG112>\n",nomer_str_tab5,dusr); //11

if(dolgn.getdlinna() > 1)
 fprintf(ff_xml_tab5,"   <T1RXXXXG15S ROWNUM=\"%d\">%s</T1RXXXXG15S>\n",nomer_str_tab5,dolgn.ravno_filtr_xml()); //15

sprintf(strsql,"select naik from Zvan where kod=%d",voen_zvanie.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 voen_zvanie.new_plus(row[0]);
else
 voen_zvanie.new_plus("");


if(voen_zvanie.getdlinna() > 1)
 fprintf(ff_xml_tab5,"   <T1RXXXXG19S ROWNUM=\"%d\">%s</T1RXXXXG19S>\n",nomer_str_tab5,voen_zvanie.ravno_filtr_xml()); //15


}



/******************************************/
/*Таблица 5 принятые на работу и уволенные*/
/*Возвращает количество человек в таблице*/
/******************************************/
int zarpensm1_t5(short mr,short gr,
const char *inn_or,
const char *naim_kontr00,
const char *kodpfu,
const char *imaf_tab5,
const char *kodzvna,
char *imaf_xml_tab5,
FILE *ff_prot,
GtkWidget *wpredok)
{
int nomer_str_tab5=0;
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;
int metka_ukr_grom=1;

char strsql[1024];
short dpnr=0;
short dusr=0;
short d,m,g;
class iceb_u_str inn("");
class iceb_u_str fio("");
class iceb_u_int sp_tabn;

class iceb_fioruk_rk kerivnik;
class iceb_fioruk_rk glavbuh;
iceb_fioruk(1,&kerivnik,wpredok);
iceb_fioruk(2,&glavbuh,wpredok);


sprintf(strsql,"select tabn,datn,datk from Zarn where god=%d and mes=%d and ((datn >= '%04d-%02d-01' and datn <= '%04d-%02d-31') \
or (datk >= '%04d-%02d-01' and datk <= '%04d-%02d-31'))",gr,mr,gr,mr,gr,mr,gr,mr,gr,mr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдено ни одного принятого на роботу или уволенного в списке месячных настроек\n");
 }

if(ff_prot != NULL)
 fprintf(ff_prot,"\n\nФормирование таблицы 5\n");

class iceb_fopen ff_tab5;
if(ff_tab5.start(imaf_tab5,"w",wpredok) != 0)
 return(0);


class iceb_rnfxml_data rek_zag_nn;

iceb_rnfxml(&rek_zag_nn,wpredok); /*чтение данных для xml файлов*/

FILE *ff_xml_tab5;
int period_type=0;

if(iceb_openxml(1,mr,gr,31,mr,gr,imaf_xml_tab5,iceb_getkoddok(1,mr,gr,7),1,&period_type,&rek_zag_nn,&ff_xml_tab5,wpredok) != 0) /*открытие файла и получение имени файла*/
 return(1);

iceb_rnn_sap_xml(1,gr,mr,iceb_getkoddok(1,mr,gr,7),1,period_type,&rek_zag_nn,ff_xml_tab5); /*общие теги с шапкой документа*/

zarpensm1_hd4(inn_or,naim_kontr00,mr,gr,ff_xml_tab5); /*теги с шапкой таблицы*/

iceb_zagolov("Таблиця 5. Відомості про трудові відносини осіб та період проходження військової служби",ff_tab5.ff,wpredok);
fprintf(ff_tab5.ff,"Звітний місяць:%d    рік:%d\n",mr,gr);

fprintf(ff_tab5.ff,"\
------------------------------------------------------------------------------\n");
fprintf(ff_tab5.ff,"\
  N  |Ном.обл.к.|       Прізвище, ім'я, по батькові      |Кат|Трудові віднос.|\n\
     |          |                                        |ег.|Дата п.|Дата к.|\n");

fprintf(ff_tab5.ff,"\
------------------------------------------------------------------------------\n");
int zo=1; /*категория работника 1-трудовая книжка на предприятии 2-совместитель 3-договора подряда 4-уход за детьми 5 беременность и роды*/
while(cur.read_cursor(&row) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s %s\n",row[0],row[1],row[2]);
   
  dpnr=dusr=0;
  fio.new_plus("");
  inn.new_plus("");
  sprintf(strsql,"select fio,inn,zvan,sovm from Kartb where tabn=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    sprintf(strsql,"%s %s!",gettext("Не найден табельный номер"),row[0]);
    iceb_menu_soob(strsql,wpredok);
    continue;
   }

  if(iceb_u_proverka(kodzvna,row1[2],0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s Атестованный - в отчёт не входит!\n",row1[0]);
    continue;
   }

  zo=1; /*категория работника 1-трудовая книжка на предприятии 2-совместитель 3-договора подряда 4-уход за детьми 5 беременность и роды*/
  if(atoi(row1[3]) == 1) /*нет трудовой книжки*/
   zo=2;
  if(zarprtnw(mr, gr,atoi(row[0]),3,wpredok) == 1)
    zo=3;

  fio.new_plus(row1[0]);
  inn.new_plus(row1[1]);
     
  if(row[1][0] != '0')
   {
    iceb_u_rsdat(&d,&m,&g,row[1],2);  

    if(iceb_u_sravmydat(1,m,g,1,mr,gr) == 0)
     dpnr=d;
   }
  if(row[2][0] != '0')
   {
    iceb_u_rsdat(&d,&m,&g,row[2],2);  

    if(iceb_u_sravmydat(1,m,g,1,mr,gr) == 0)
     dusr=d;
   }
  sp_tabn.plus(row[0],-1);

 
  nomer_str_tab5++;
  

  zarpensm1_str_tab5(atoi(row[0]),nomer_str_tab5,inn.ravno(),fio.ravno(),dpnr,dusr,mr,gr,metka_ukr_grom,zo,ff_tab5.ff,ff_xml_tab5,wpredok);


 }

/*Если в карточке небыло начислений то промежуточной записи по этому месяцу небудет. Смотрим в карточке даты приёма увольнения*/

sprintf(strsql,"select tabn,fio,datn,datk,inn,zvan,sovm from Kartb where ((datn >= '%04d-%02d-01' and datn <= '%04d-%02d-31') \
or (datk >= '%04d-%02d-01' and datk <= '%04d-%02d-31'))",gr,mr,gr,mr,gr,mr,gr,mr);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдено ни одного принятого на роботу или уволенного в списке карточек\n");
 }

while(cur.read_cursor(&row) != 0)
 {
  if(sp_tabn.find(row[0]) >= 0)
    continue;
  dpnr=dusr=0;
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  if(iceb_u_proverka(kodzvna,row[5],0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s Атестованный - в отчёт не входит!\n",row[1]);
    continue;
   }
  zo=1; /*категория работника 1-трудовая книжка на предприятии 2-совместитель 3-договора подряда 4-уход за детьми 5 беременность и роды*/
  if(atoi(row[6]) == 1) /*нет трудовой книжки*/
   zo=2;
  if(zarprtnw(mr, gr,atoi(row[0]),3,wpredok) == 1)
    zo=3;

  if(row[2][0] != '0')
   iceb_u_rsdat(&dpnr,&m,&g,row[2],2);  
  if(row[3][0] != '0')
   iceb_u_rsdat(&dusr,&m,&g,row[3],2);  
  
  nomer_str_tab5++;
  zarpensm1_str_tab5(atoi(row[0]),nomer_str_tab5,row[4],row[1],dpnr,dusr,mr,gr,metka_ukr_grom,zo,ff_tab5.ff,ff_xml_tab5,wpredok);

 }

fprintf(ff_tab5.ff,"\
------------------------------------------------------------------------------\n");
zarpensm1_kon_lst(nomer_str_tab5,kerivnik.inn.ravno(),kerivnik.fio.ravno(),glavbuh.inn.ravno(),glavbuh.fio.ravno(),ff_tab5.ff);

iceb_podpis(ff_tab5.ff,wpredok);

ff_tab5.end();

short dt=0,mt=0,gt=0;
iceb_u_poltekdat(&dt,&mt,&gt);
zarpensm1_end_tab_xml(dt,mt,gt,&kerivnik,&glavbuh,ff_xml_tab5);/*концоквка таблицы*/
fclose(ff_xml_tab5);

if(nomer_str_tab5 == 0)
 {
//  unlink(imaf_tab5);
  unlink(imaf_xml_tab5);
 }

return(nomer_str_tab5);

}
/***************************/
/*получить количество дней*/
/***************************/
int zarpensm1_kdnp(int tabn,short m,short g,const char *kodtab,FILE *ff_prot,GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
int kolstr=0;

if(kodtab[0] == '\0')
 return(0);

sprintf(strsql,"select kodt,kdnei from Ztab where tabn=%d and god=%d and mes=%d",tabn,g,m);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(0);
 }
if(ff_prot != NULL)
 fprintf(ff_prot,"\n%s-Ищем табеля для %d из списка-%s\n",__FUNCTION__,tabn,kodtab);
int voz=0;
while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(kodtab,row[0],0,1) == 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s %s %s\n",__FUNCTION__,row[0],row[1]);
    voz+=atoi(row[1]);
   }
 }
return(voz);
}
/****************************************/
/*чтение данных на работника*/
/***********************************/
int zarpensm1_readtn(short mr,short gr,
const char *tabnom,
const char *nastr_kodzvna,
class iceb_u_str *fio,class iceb_u_str *ima,class iceb_u_str *othestvo,
class iceb_u_str *inn,
class iceb_u_str *prizv,
class iceb_u_str *data_prik,
class iceb_u_str *nomer_prik,
class iceb_u_str *kodss,
class iceb_u_str *metka_pola,
int *metka_otk,
int *metka_exp,
int *zo,
int *metka_inv,
FILE *ff_prot,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
fio->new_plus("");
inn->new_plus("");
prizv->new_plus("");
ima->new_plus("");
othestvo->new_plus("");
  
*metka_otk=1;  /*1-трудовая книжка на предпиятии 0- нет*/

int kodzvna=0;  
kodss->new_plus("");
metka_pola->new_plus("");
sprintf(strsql,"select fio,datn,datk,sovm,inn,zvan,kss,pl from Kartb where tabn=%s",tabnom);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  fio->new_plus(row[0]);
  inn->new_plus(row[4]);
  if(atoi(row[3]) == 1)
   *metka_otk=0;
  kodzvna=atoi(row[5]);

  if(iceb_u_pole3(row[0],prizv,1,' ') != 0)
   prizv->new_plus(row[0]);

  iceb_u_pole3(row[0],ima,2,' ');
  iceb_u_pole3(row[0],othestvo,3,' ');

  if(iceb_u_polen(row[6],kodss,1,' ') != 0)
    kodss->new_plus(row[6]);
  iceb_u_polen(row[6],data_prik,2,' ');
  iceb_u_polen(row[6],nomer_prik,3,' ');
  
  
  if(atoi(row[7]) == 0)
   metka_pola->new_plus("Ч");
  else
   metka_pola->new_plus("Ж");     
 }
else
 {
   if(ff_prot != NULL)
    fprintf(ff_prot,"Не найдена карточка работника %d!\n",atoi(row[0]));
  return(1);
 }
 
if(iceb_u_proverka(nastr_kodzvna,kodzvna,0,0) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s Атестованный - в отчёт не входит!\n",row[0]);
  return(1);
 }


if(inn->getdlinna() <= 1)
 {
  sprintf(strsql,"%s %s\n%s",row[0],fio->ravno(),gettext("Не введён идентификационный номер"));
  iceb_menu_soob(strsql,wpredok);
 }

*metka_exp=0;
if(kodss->ravno()[0] != '\0')
 *metka_exp=1;   
*zo=1;
*metka_inv=0;
if(zarprtnw(mr,gr,atoi(tabnom),2,wpredok) == 1)
 {
  *metka_inv=1;
  *zo=2;
 }
if(zarprtnw(mr,gr,atoi(tabnom),3,wpredok) == 1)
 {
  *zo=26;
  *metka_otk=0;
 }
return(0);
}

/****************************************************/
void zarpensm1_str_tab7(int nomer_str_tab7,const char *data_prik,int metka_ukr_grom,const char *inn,const char *fio,
const char *nomer_prik,
const char *kodss,
short dn,short dk,
short mr,short gr,
const char *tabnom,
const char *prizv,
const char *ima,
const char *othestvo,
int koldn,
FILE *ff_tab7,
FILE *ff_xml_tab7,
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[1024];

/*Если уволили то день конца периода это день увольнения*/
sprintf(strsql,"select datk from Kartb where tabn=%d",atoi(tabnom));

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(row[0][0] != '0')
   {
    short d,m,g;
    iceb_u_rsdat(&d,&m,&g,row[0],2);
    if(d != 0)
      dk=d;
   }
 }


short dprik=0,mprik=0,gprik=0;

iceb_u_rsdat(&dprik,&mprik,&gprik,data_prik,1);


fprintf(ff_tab7,"\
%5d|%d|%-*s|%-*s|%-*.*s|%3d|%3d|     |      |     |%-*s|%02d.%02d.%04d|    |%s\n",
nomer_str_tab7,
metka_ukr_grom,
iceb_u_kolbait(10,inn),inn,
iceb_u_kolbait(10,kodss),
kodss,
iceb_u_kolbait(30,fio),
iceb_u_kolbait(30,fio),
fio,
dn,
dk,
iceb_u_kolbait(5,nomer_prik),
nomer_prik,
dprik,mprik,gprik,    
tabnom);

/*формат налоговой*/

fprintf(ff_xml_tab7,"   <T1RXXXXG6 ROWNUM=\"%d\">%d</T1RXXXXG6>\n",nomer_str_tab7,metka_ukr_grom); //6
if(inn[0] != '\0')
   fprintf(ff_xml_tab7,"   <T1RXXXXG7S ROWNUM=\"%d\">%s</T1RXXXXG7S>\n",nomer_str_tab7,inn); //7
fprintf(ff_xml_tab7,"   <T1RXXXXG8S ROWNUM=\"%d\">%s</T1RXXXXG8S>\n",nomer_str_tab7,kodss); //8

fprintf(ff_xml_tab7,"   <T1RXXXXG91S ROWNUM=\"%d\">%s</T1RXXXXG91S>\n",nomer_str_tab7,iceb_u_filtr_xml(prizv)); //9
fprintf(ff_xml_tab7,"   <T1RXXXXG92S ROWNUM=\"%d\">%s</T1RXXXXG92S>\n",nomer_str_tab7,iceb_u_filtr_xml(ima)); //9
fprintf(ff_xml_tab7,"   <T1RXXXXG93S ROWNUM=\"%d\">%s</T1RXXXXG93S>\n",nomer_str_tab7,iceb_u_filtr_xml(othestvo)); //9

fprintf(ff_xml_tab7,"   <T1RXXXXG10 ROWNUM=\"%d\">%d</T1RXXXXG10>\n",nomer_str_tab7,dn); //10
fprintf(ff_xml_tab7,"   <T1RXXXXG11 ROWNUM=\"%d\">%d</T1RXXXXG11>\n",nomer_str_tab7,dk); //11
fprintf(ff_xml_tab7,"   <T1RXXXXG12 ROWNUM=\"%d\">%d</T1RXXXXG12>\n",nomer_str_tab7,koldn); //12

fprintf(ff_xml_tab7,"   <T1RXXXXG141 ROWNUM=\"%d\">%d</T1RXXXXG141>\n",nomer_str_tab7,koldn); //14

if(nomer_prik[0] != '\0')
 fprintf(ff_xml_tab7,"   <T1RXXXXG15 ROWNUM=\"%d\">%s</T1RXXXXG15S>\n",nomer_str_tab7,nomer_prik); //15
if(dprik != 0)
 fprintf(ff_xml_tab7,"   <T1RXXXXG16D ROWNUM=\"%d\">%02d%02d%04d</T1RXXXXG16D>\n",nomer_str_tab7,dprik,mprik,gprik);//16

}
/*******************************/
/*Узнаём сумму начислений на зарплату*/
/*************************************/
double zarpensm1_snnz(int tabn,short mr,short gr,double *suma_esv_bol,GtkWidget *wpredok)
{
double suma=0.;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
*suma_esv_bol=0;
sprintf(strsql,"select kodz,sumap from Zarsocz where datz='%04d-%02d-01' and tabn=%d",gr,mr,tabn);

if(cur.make_cursor(&bd,strsql) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(0.);
 }
int kodz=0;
while(cur.read_cursor(&row) != 0)
 {
  kodz=atoi(row[0]);
  if(kodf_esv_bol == kodz || kodf_esv_bol_inv == kodz)
   *suma_esv_bol+=atof(row[1]);
  else
   suma+=atof(row[1]);
 }
return(suma);

}

/******************************************/
/******************************************/

gint zarpensm1w_r1(class zarpensm1w_r_data *data)
{
class iceb_u_str version_dok("H");
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row,row1;
SQLCURSOR cur,cur1;
int kolstr=0;
class iceb_u_str inn_or("");
class iceb_u_str inn("");
int metka_ukr_grom=1;
char imaf_tab5[64],imaf_xml_tab5[64],imaf_tab6[64],imaf_xml_tab6[64],imaf_tab7[64],imaf_xml_tab7[64],imaf_prot[64];
class iceb_u_str edrpou("");
class iceb_u_str fio("");
int nomer_str_tab5=0;
class iceb_u_str naim_kontr00(iceb_get_pnk("00",1,data->window));
static class iceb_u_str tabnom("");
int kd_ptv=0;
int nrm=0;



short mr,gr;
if(iceb_u_rsdat1(&mr,&gr,data->rk->datar.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s:%d.%d\n",gettext("Дата расчёта"),mr,gr);
iceb_printw(strsql,data->buffer,data->view);


class iceb_u_spisok stab6; /*код типа начисления|месяц.год*/
class iceb_u_double tab6_sumao;
class iceb_u_double tab6_sumavr;
class iceb_u_double tab6_sumaesv;

class iceb_u_spisok data_boln; /*Дата больничного*/
class iceb_u_double suma_data_boln; /*сумма больничного по датам*/
class iceb_u_double suma_data_boln_esv; /*сумма единого социального взноса с больничного по датам*/

class iceb_u_spisok data_berem; /*Дата помощи по беременности*/
class iceb_u_double suma_data_berem; /*сумма помощи по беременности по датам*/
class iceb_u_double suma_data_berem_esv; /*сумма единого социального взноса с помощи по беременности по датам*/

class zarpensm1_nastr nastr;

zarpensm1_read_nast(&nastr,data->window); /*Читаем настройки*/

/*чтение величины минимальной зарплаты и прожиточного минимума*/
class zar_read_tn1h nastr_mz;
zar_read_tn1w(1,mr,gr,&nastr_mz,data->window);

/*Чтение настроек для расчёта единого социального взноса*/
zarrnesvw(data->window);

sprintf(strsql,"select distinct tabn from Zarp where datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and prn='1' and suma <> 0.",
gr,mr,1,gr,mr,31);

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

if(iceb_sql_readkey("select kod from Kontragent where kodkon='00'",&row1,&cur1,data->window) == 1)
 {
  inn_or.new_plus(row1[0]);
 } 

class iceb_rnfxml_data rek_zag_nn;

iceb_rnfxml(&rek_zag_nn,data->window); /*чтение данных для xml файлов*/

int period_type=0;


FILE *ff_xml_tab6;

if(iceb_openxml(1,mr,gr,31,mr,gr,imaf_xml_tab6,iceb_getkoddok(1,mr,gr,8),1,&period_type,&rek_zag_nn,&ff_xml_tab6,data->window) != 0) /*открытие файла и получение имени файла*/
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_rnn_sap_xml(1,gr,mr,iceb_getkoddok(1,mr,gr,8),1,period_type,&rek_zag_nn,ff_xml_tab6); /*общие теги с шапкой документа*/

zarpensm1_hd4(inn_or.ravno(),naim_kontr00.ravno(),mr,gr,ff_xml_tab6); /*теги с шапкой таблицы*/

FILE *ff_xml_tab7;

if(iceb_openxml(1,mr,gr,31,mr,gr,imaf_xml_tab7,iceb_getkoddok(1,mr,gr,9),1,&period_type,&rek_zag_nn,&ff_xml_tab7,data->window) != 0) /*открытие файла и получение имени файла*/
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_rnn_sap_xml(1,gr,mr,iceb_getkoddok(1,mr,gr,9),1,period_type,&rek_zag_nn,ff_xml_tab7); /*общие теги с шапкой документа*/

zarpensm1_hd4(inn_or.ravno(),naim_kontr00.ravno(),mr,gr,ff_xml_tab7); /*теги с шапкой таблицы*/




sprintf(imaf_tab5,"E04T05_%d.lst",getpid());

sprintf(imaf_tab6,"E04T06_%d.lst",getpid());
class iceb_fopen ff_tab6;
if(ff_tab6.start(imaf_tab6,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(imaf_tab7,"E04T07_%d.lst",getpid());
class iceb_fopen ff_tab7;
if(ff_tab7.start(imaf_tab7,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(imaf_prot,"prot%d.lst",getpid());
class iceb_fopen ff_prot;
if(ff_prot.start(imaf_prot,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(ff_prot.ff != NULL)
 {
  fprintf(ff_prot.ff,"Коды не входящие в расчёт единого социального взноса:%s\n",knvr_esv_r.ravno());
  for(int nom=0; nom < nastr.kodtn.kolih(); nom++)
    fprintf(ff_prot.ff,"Коды начислений для кода типа начисления %d:%s\n",nastr.kodtn.ravno(nom),nastr.kodnah.ravno(nom));
  fprintf(ff_prot.ff,"Коды званий неатестованных:%s\n",nastr.kodzvna.ravno());
  fprintf(ff_prot.ff,"Коды табеля больничных:%s\n",nastr.kod_tab_bol.ravno());
  fprintf(ff_prot.ff,"Коды табеля без сохранения зарплаты:%s\n",nastr.kod_tab_bsz.ravno());
  fprintf(ff_prot.ff,"Коды табеля пребывания в трудовых отношениях:%s\n",nastr.kod_tab_kdtv.ravno());

  fprintf(ff_prot.ff,"Коды больничных:");
  if(kodbl != NULL)
    for(int nom=1; nom <= kodbl[0]; nom++)
      fprintf(ff_prot.ff,"%d ",kodbl[nom]);
 }

short dkm=1;
iceb_u_dpm(&dkm,&mr,&gr,5);

 


iceb_zagolov("Таблиця 6. Відомості про нарахування заробітку (доходу) застрахованим особам",ff_tab6.ff,data->window);
fprintf(ff_tab6.ff,"Звітний місяць:%d    рік:%d\n",mr,gr);

fprintf(ff_tab6.ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_tab6.ff,"\
  N  |Г|С|Номер облі|   Прізвище та ініціали ЗО    |Код |Тип |Місяць |  |  |  |  |Загальна  |Сума нарах|Сума різ|Сума утр|Сума нарах|Озна|Озна|\n\
     |р|т|кової карт|                              |кате|нара|та рік |  |  |  |  |сума нарах|увань заро|ниці між|иманого |ованого єд|ка  |ка  |\n\
     |о|а|ки застра-|                              |грії|хува|за який|  |  |  |  |ованої зар|бітку/дохо|розміром|єдиного |иного внес|наяв|наяв|\n\
     |м|т|хованої   |                              | ЗО |нь**|проведе|  |  |  |  |обітної пл|ду, в межа|мінімаль|внеску  |ку за звіт|ност|ност|\n\
     |а|ь|особи (ЗО)|                              |    |    |но нара|  |  |  |  |ати       |х максимал|ної заро|за звітн|ний місяць|і тр|і сп|\n\
     |д| |за ДРФО   |                              |    |    |хування|  |  |  |  |          |ьної велич|бітної  |ий місяц|          |удов|ецст|\n\
     |я| |   ДПА    |                              |    |    |       |  |  |  |  |          |ини на яку|плати та|ь       |          |ої к|ажу |\n\
     |н| |          |                              |    |    |       |  |  |  |  |          |нараховуют|фактично|        |          |нижк|(1-т|\n\
     |и| |          |                              |    |    |       |  |  |  |  |          |ься эдиний|нарахова|        |          |и   |ак,0|\n\
     |н| |          |                              |    |    |       |  |  |  |  |          |внесок    |ною заро|        |          |(1-т|-ні)|\n\
     | | |          |                              |    |    |       |  |  |  |  |          |          |бітною п|        |          |ак,0|    |\n\
     | | |          |                              |    |    |       |  |  |  |  |          |          |латою   |        |          |-ні)|    |\n");
fprintf(ff_tab6.ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");

double itogo_tab6[5];
memset(itogo_tab6,'\0',sizeof(itogo_tab6));



iceb_zagolov("Таблиця 7. Наявність підстав для обліку стажу окремим категоріям осіб відповідно до законодавства",ff_tab7.ff,data->window);
fprintf(ff_tab7.ff,"Звітний місяць:%d    рік:%d\n",mr,gr);

fprintf(ff_tab7.ff,"\
--------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_tab7.ff,"\
  N  |Г|Податковий|Код підста|   Прізвище та ініціали ЗО    |Поч|Кін|Кільк|Кількі|Норма|N нак|Дата наказ|Озна|\n\
     |р|номер або |ви для обл|                              |ато|нец|ість |сть го|трива|азу п|у про пров|ка  |\n\
     |о|серія та  |іку спецст|                              |к п|ь п|днів |дин,  |лості|ро пр|едення ате|СЕЗО|\n\
     |м|номер пасп|ажу       |                              |ері|ері|     |хвилин|робот|оведе|стації роб| Н  |\n\
     |а|орту      |          |                              |оду|оду|     |      |и для|ння а|очого місц|    |\n\
     |д|          |          |                              |   |   |     |      |її за|теста|я         |    |\n\
     |я|          |          |                              |   |   |     |      |рахув|ції р|          |    |\n\
     |н|          |          |                              |   |   |     |      |ання |обочо|          |    |\n\
     |и|          |          |                              |   |   |     |      |за по|го мі|          |    |\n\
     |н|          |          |                              |   |   |     |      |вний |сця  |          |    |\n\
     | |          |          |                              |   |   |     |      |місяц|     |          |    |\n\
     | |          |          |                              |   |   |     |      |ь    |     |          |    |\n");
fprintf(ff_tab7.ff,"\
--------------------------------------------------------------------------------------------------------------\n");

double suma_nah=0.;   

double suma=0.;
double suma_esv_v=0.; /*Cумма единого социального взноса не больничного*/
double suma_esv_v_bol=0.; /*Cумма единого социального взноса больничного*/

int nomer_str_tab6=0;
double suma_obh=0.;
class iceb_u_str metka_pola(""); /*чоловік Ч жінка Ж*/
int metka_otk=1; /*метка наличия трудовой книжки 0-нет трудовой 1-есть*/
int metka_exp=0; /*метка наличия спецстажа 0-нет 1-есть*/
int nomer_vsp=0;
int nomer_str_tab7=0;
int kodtipnah=0; /*Код типа начисления*/
//int nomer_str_tab5=0;
class iceb_u_str prizv("");
class iceb_u_str ima("");
class iceb_u_str othestvo("");
class iceb_u_str kodss("");
class iceb_u_str data_prik("");
class iceb_u_str nomer_prik("");
short dn=1;
short dk=1;
iceb_u_dpm(&dk,&mr,&gr,5);
short koldn=dk-dn+1;
class iceb_u_spisok t6_tabnom; /*Список табельных номеров по которым сделана запись в таблицу 6*/
int zo=1;
int metka_inv=0;
char data_zap[16]; /*месяц и год в счёт которых выплачено*/
int kd_nzp=0;
int tabn=0;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(tabnom.ravno(),row[0],0,0) != 0)
   continue;
  tabn=atoi(row[0]);
  /*сортировка по убыванию даты для того чтобы записи в счёт месяца расчёта попадались первыми*/
  sprintf(strsql,"select knah,suma,godn,mesn from Zarp where datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and tabn=%s and prn='1' and suma <> 0. order by godn desc,mesn desc,knah asc",
  gr,mr,1,gr,mr,31,row[0]);

  if(ff_prot.ff != NULL)
   {
    fprintf(ff_prot.ff,"\n\nТабельный номер %s\n",row[0]);
    fprintf(ff_prot.ff,"%s\n",strsql);
   }
  if(cur1.make_cursor(&bd,strsql) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  suma_obh=0.;
  suma=0.;
  stab6.free_class();
  tab6_sumao.free_class();  
  tab6_sumavr.free_class();  
  tab6_sumaesv.free_class();  

  data_boln.free_class();
  suma_data_boln.free_class();
  suma_data_boln_esv.free_class();

  data_berem.free_class();
  suma_data_berem.free_class();
  suma_data_berem_esv.free_class();

  while(cur1.read_cursor(&row1) != 0)
   {
    if(ff_prot.ff != NULL)
      fprintf(ff_prot.ff,"%s %s %s %s\n",row1[0],row1[1],row1[2],row1[3]);
    if(iceb_u_proverka(knvr_esv_r.ravno(),row1[0],0,1) == 0)
     {
      if(ff_prot.ff != NULL)
        fprintf(ff_prot.ff,"Не входит в расчёт\n");
      continue;
     }    

    suma_nah=atof(row1[1]);   
    kodtipnah=0;
    
    if(provkodw(kodbl,row1[0]) >= 0) /*больничные*/
     {
      if(ff_prot.ff != NULL)
       fprintf(ff_prot.ff,"Больничный\n");
       
      sprintf(strsql,"%s.%s",row1[3],row1[2]);
      
      if((nomer_vsp=data_boln.find(strsql)) < 0)
       data_boln.plus(strsql);
      suma_data_boln.plus(suma_nah,nomer_vsp);
      suma_data_boln_esv.plus(0.,nomer_vsp);
      continue;      
     }

    if(iceb_u_proverka(nastr.kod_nah_berem.ravno(),row1[0],0,1) == 0) /*сумма помощи по беременности и родам*/
     {
      if(ff_prot.ff != NULL)
       fprintf(ff_prot.ff,"Сумма помощи по беременности и родам\n");
       
      sprintf(strsql,"%s.%s",row1[3],row1[2]);
      
      if((nomer_vsp=data_berem.find(strsql)) < 0)
       data_berem.plus(strsql);
      suma_data_berem.plus(suma_nah,nomer_vsp);
      suma_data_berem_esv.plus(0.,nomer_vsp);
      continue;      
     }

    for(int nom=0; nom < nastr.kodtn.kolih(); nom++)
     {
      if(iceb_u_proverka(nastr.kodnah.ravno(nom),row1[0],0,1) == 0)
       {

        kodtipnah=nastr.kodtn.ravno(nom);
        if(ff_prot.ff != NULL)
          fprintf(ff_prot.ff,"Подходит для кода типа начисления %d\n",kodtipnah);
       }
     }


    sprintf(strsql,"%d|%s.%s",kodtipnah,row1[3],row1[2]);

    if((nomer_vsp=stab6.find(strsql)) < 0)
     stab6.plus(strsql);

    tab6_sumao.plus(suma_nah,nomer_vsp);

    tab6_sumavr.plus(0.,nomer_vsp);

    tab6_sumaesv.plus(0.,nomer_vsp);

   }



  sprintf(strsql,"select knah,suma from Zarp where datz >= '%04d-%02d-%02d' and \
datz <= '%04d-%02d-%02d' and tabn=%s and prn='2'and suma <> 0.",
  gr,mr,1,gr,mr,31,row[0]);

  if(cur1.make_cursor(&bd,strsql) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  int kod_zap=0;
  suma_esv_v=suma_esv_v_bol=0.;
  while(cur1.read_cursor(&row1) != 0)
   {
    if(ff_prot.ff != NULL)
     fprintf(ff_prot.ff,"%s %s\n",row1[0],row1[1]);    

    kod_zap=atoi(row1[0]);        

    if(kod_zap == kod_esv)
       suma_esv_v+=atof(row1[1])*-1;   

    if(kod_zap == kod_esv_bol || kod_zap == kod_esv_bol_inv)
       suma_esv_v_bol+=atof(row1[1])*-1;   

    if(kod_zap == kod_esv_inv)
       suma_esv_v+=atof(row1[1])*-1;   

    if(kod_zap == kod_esv_dog)
       suma_esv_v+=atof(row1[1])*-1;   
    if(ff_prot.ff != NULL)
     fprintf(ff_prot.ff,"suma_esv_v=%.2f\n",suma_esv_v);     
   }

  if(ff_prot.ff != NULL)
   fprintf(ff_prot.ff,"Единый соц.взнос = %.2f\n\
Единый соц.взнос с больничного=%.2f\n",suma_esv_v,suma_esv_v_bol);
/********************   
  if(suma_esv_v+suma_esv_v_bol == 0.)
   {
    if(ff_prot.ff != NULL)
     fprintf(ff_prot.ff,"Нет единой социальной выплаты!\n");
    continue;
   }
****************/
  double suma_vr_tab=0.;
  int kolih_nah=stab6.kolih();
  /*Определяем общюю сумму взятую в расчёт*/
  for(int nom=0; nom < kolih_nah; nom++)
   {  
    suma=tab6_sumao.ravno(nom);
    if(suma > nastr_mz.max_sum_for_soc)
     suma=nastr_mz.max_sum_for_soc;
    suma_vr_tab+=suma;
    tab6_sumavr.plus(suma,nom);
   }

  /*разбираемся с небольничными начилениями*/  
  suma=tab6_sumavr.suma();
  double proc_per=0.;
  double suma_per=0.;
  for(int nom=0; nom < kolih_nah; nom++)
   {
    suma=tab6_sumao.ravno(nom);

    proc_per=tab6_sumavr.ravno(nom)*100./suma_vr_tab;

    suma_per=suma_esv_v*proc_per/100.;
    tab6_sumaesv.plus(suma_per,nom);
    if(ff_prot.ff != NULL)
     {
      
      fprintf(ff_prot.ff,"Процент = %.2f*100./%.2f=%.2f\n",tab6_sumavr.ravno(nom),suma_vr_tab,proc_per);
      fprintf(ff_prot.ff,"Расчёт = %.2f*%.2f/100.=%.2f\n",suma_esv_v,proc_per,suma_per);
 
     }
   }

  /*разбираемся с больничными начислениями*/
  kolih_nah=data_boln.kolih();
  suma=suma_data_boln.suma();
  for(int nom=0; nom < kolih_nah; nom++)
   {
    proc_per=suma_data_boln.ravno(nom)*100./suma;
    suma_per=suma_esv_v_bol*proc_per/100.;
    suma_data_boln_esv.plus(suma_per,nom);
   }

  /*разбираемся с помощью по беременности и родам*/
  kolih_nah=data_berem.kolih();
  suma=suma_data_berem.suma();
  for(int nom=0; nom < kolih_nah; nom++)
   {
    proc_per=suma_data_berem.ravno(nom)*100./suma;
    suma_per=suma_esv_v_bol*proc_per/100.;
    suma_data_berem_esv.plus(suma_per,nom);
   }


  if(zarpensm1_readtn(mr,gr,row[0],nastr.kodzvna.ravno(),&fio,&ima,&othestvo,&inn,&prizv,&data_prik,&nomer_prik,&kodss,
  &metka_pola,&metka_otk,&metka_exp,&zo,&metka_inv,ff_prot.ff,data->window) != 0)
   continue;

  kd_nzp=0;
  kd_ptv=0;
  nrm=0;
  kd_nzp=zarpensm1_kdnp(atoi(row[0]),mr,gr,nastr.kod_tab_bsz.ravno(),ff_prot.ff,data->window);
  kd_ptv=zarpensm1_kdnp(atoi(row[0]),mr,gr,nastr.kod_tab_kdtv.ravno(),ff_prot.ff,data->window);


  /*Узнаём сумму начислений на зарплату*/
  double suma_narah_bol=0.;
  double sum_narah=zarpensm1_snnz(atoi(row[0]),mr,gr,&suma_narah_bol,data->window);
  
  itogo_tab6[4]+=sum_narah+suma_narah_bol;
  
  double sum_diff=0.;

  if(tab6_sumao.suma()+ suma_data_boln.suma()+suma_data_berem.suma() < nastr_mz.minzar)
   {
    sum_diff=nastr_mz.minzar-tab6_sumao.suma()- suma_data_boln.suma()-suma_data_berem.suma();
    /*проверяем принят/уволен*/
    sprintf(strsql,"select datn,datk,sovm from Kartb where tabn=%d",tabn);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      if(atoi(row1[2]) == 0) /*не совместитель*/
       {
        short dnr=0,mnr=0,gnr=0;
        short dkr=0,mkr=0,gkr=0;
        iceb_u_rsdat(&dnr,&mnr,&gnr,row1[0],2);
        iceb_u_rsdat(&dkr,&mkr,&gkr,row1[1],2);
        if(iceb_u_sravmydat(1,mr,gr,1,mnr,gnr) == 0 || iceb_u_sravmydat(1,mr,gr,1,mkr,gkr) == 0)
         sum_diff=0.;
        else
         {  
          if(zarprtnw(mr,gr,tabn,3,data->window) == 0)/*не работает по договору подряда*/
           {
            double sum_narahv=sum_narah*sum_diff/(tab6_sumao.suma()+sum_diff);
  
            zarpensm1_str_tab6(++nomer_str_tab6,mr,gr,inn.ravno(),fio.ravno(),metka_ukr_grom,13,metka_pola.ravno(),
            data_zap,0.,0.,0.,metka_otk,metka_exp,row[0],zo,0,0,0,0,nrm,sum_diff,sum_narahv,ff_tab6.ff,ff_xml_tab6);
            sum_narah-=sum_narahv;      
           }
         }
      }
     }
   }

  itogo_tab6[2]+=sum_diff;  
  /*не больничные начисления*/
  
  for(int nom=0; nom < stab6.kolih(); nom++)
   {
    if(ff_prot.ff != NULL)
     fprintf(ff_prot.ff,"tab6_sumao[%d]=%.2f\n\
tab6_sumavr[%d]=%.2f\n\
tab6_sumaesv[%d]=%.2f\n",
     nom,tab6_sumao.ravno(nom),
     nom,tab6_sumavr.ravno(nom),
     nom,tab6_sumaesv.ravno(nom));

    suma_obh=tab6_sumao.ravno(nom);
    itogo_tab6[0]+=suma_obh;

    double sum_narahv=sum_narah*suma_obh/tab6_sumao.suma();

    itogo_tab6[1]+=suma=tab6_sumavr.ravno(nom);
    itogo_tab6[3]+=suma_esv_v=tab6_sumaesv.ravno(nom);
    iceb_u_polen(stab6.ravno(nom),&kodtipnah,1,'|');      
    iceb_u_polen(stab6.ravno(nom),data_zap,sizeof(data_zap),2,'|');      

    zarpensm1_str_tab6(++nomer_str_tab6,mr,gr,inn.ravno(),fio.ravno(),metka_ukr_grom,kodtipnah,metka_pola.ravno(),
    data_zap,suma_obh,suma,suma_esv_v,metka_otk,metka_exp,row[0],zo,0,kd_nzp,kd_ptv,0,nrm,0.,sum_narahv,ff_tab6.ff,ff_xml_tab6);

    if(t6_tabnom.find(row[0]) < 0)
     t6_tabnom.plus(row[0]);
     
    kd_nzp=kd_ptv=0; /*обнуляем чтобы дни печатались только в первой записи ecли записей больше одной*/
   }
  
  /*больничные начисления*/
  for(int nom=0; nom < data_boln.kolih(); nom++)
   {
    itogo_tab6[0]+=suma_obh=suma_data_boln.ravno(nom);
    itogo_tab6[1]+=suma=suma_data_boln.ravno(nom);
    itogo_tab6[3]+=suma_esv_v=suma_data_boln_esv.ravno(nom);
    kodtipnah=0;

    if(metka_inv == 0)
      zo=29;
    else
      zo=36;
    
    short m,g;
    iceb_u_rsdat1(&m,&g,data_boln.ravno(nom));
    int kd_np=zarpensm1_kdnp(atoi(row[0]),m,g,nastr.kod_tab_bol.ravno(),ff_prot.ff,data->window);
    
    double suma_narah_bolv=suma_narah_bol*suma_obh/suma_data_boln.suma();
    
    zarpensm1_str_tab6(++nomer_str_tab6,mr,gr,inn.ravno(),fio.ravno(),metka_ukr_grom,kodtipnah,metka_pola.ravno(),
    data_boln.ravno(nom),suma_obh,suma,suma_esv_v,metka_otk,metka_exp,row[0],zo,kd_np,0,0,0,0,0.,suma_narah_bolv,ff_tab6.ff,ff_xml_tab6);


    if(t6_tabnom.find(row[0]) < 0)
     t6_tabnom.plus(row[0]);
     

   }

  /*cумма помощи по беременности и родам*/

  for(int nom=0; nom < data_berem.kolih(); nom++)
   {
    itogo_tab6[0]+=suma_obh=suma_data_berem.ravno(nom);
    itogo_tab6[1]+=suma=suma_data_berem.ravno(nom);
    itogo_tab6[3]+=suma_esv_v=suma_data_berem_esv.ravno(nom);
    kodtipnah=0;

    if(metka_inv == 0)
      zo=42;
    else
      zo=43;
    
    short m,g;
    iceb_u_rsdat1(&m,&g,data_berem.ravno(nom));
    int kd_berem=zarpensm1_kdnp(atoi(row[0]),m,g,nastr.kod_tab_berem.ravno(),ff_prot.ff,data->window);
    
    double suma_narah_bolv=suma_narah_bol*suma_obh/suma_data_berem.suma();
    
    zarpensm1_str_tab6(++nomer_str_tab6,mr,gr,inn.ravno(),fio.ravno(),metka_ukr_grom,kodtipnah,metka_pola.ravno(),
    data_berem.ravno(nom),suma_obh,suma,suma_esv_v,metka_otk,metka_exp,row[0],zo,0,0,0,kd_berem,0,0.,suma_narah_bolv,ff_tab6.ff,ff_xml_tab6);


    if(t6_tabnom.find(row[0]) < 0)
     t6_tabnom.plus(row[0]);
     

   }
   

  if(ff_prot.ff != NULL)
   fprintf(ff_prot.ff,"Код спецстажа=%s %s %s\n",kodss.ravno(),data_prik.ravno(),nomer_prik.ravno());
   
  if(kodss.ravno()[0] != '\0') /*Таблица 7*/
   {

    zarpensm1_str_tab7(++nomer_str_tab7,data_prik.ravno(),metka_ukr_grom,inn.ravno(),fio.ravno(),nomer_prik.ravno(),kodss.ravno(),dn,dk,mr,gr,row[0],prizv.ravno(),ima.ravno(),othestvo.ravno(),koldn,ff_tab7.ff,ff_xml_tab7,data->window);

 
 
   }
    
 }

/*Cмотрим табельные номера у которых введены календарные дни*/
sprintf(strsql,"select distinct tabn from Ztab where god=%d and mes=%d",gr,mr);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

if(ff_prot.ff != NULL)
  fprintf(ff_prot.ff,"\n\nПроверяем работников у которых небыло начилений но были введены календарные дни %d\n\
--------------------------------------------------------------------------------------------------------\n",kolstr);
while(cur.read_cursor(&row) != 0)
 {
  if(t6_tabnom.find(row[0]) >= 0)
   continue;
  if(iceb_u_proverka(tabnom.ravno(),row[0],0,0) != 0)
   continue;
  if(zarpensm1_readtn(mr,gr,row[0],nastr.kodzvna.ravno(),&fio,&ima,&othestvo,&inn,&prizv,&data_prik,&nomer_prik,&kodss,
  &metka_pola,&metka_otk,&metka_exp,&zo,&metka_inv,ff_prot.ff,data->window) != 0)
   continue;

  kd_nzp=zarpensm1_kdnp(atoi(row[0]),mr,gr,nastr.kod_tab_bsz.ravno(),ff_prot.ff,data->window);
  kd_ptv=zarpensm1_kdnp(atoi(row[0]),mr,gr,nastr.kod_tab_kdtv.ravno(),ff_prot.ff,data->window);

  sprintf(data_zap,"%02d.%d",mr,gr);  

  zarpensm1_str_tab6(++nomer_str_tab6,mr,gr,inn.ravno(),fio.ravno(),metka_ukr_grom,kodtipnah,metka_pola.ravno(),data_zap,0.,0.,0.,
  metka_otk,metka_exp,row[0],zo,0,kd_nzp,kd_ptv,0,nrm,0.,0.,ff_tab6.ff,ff_xml_tab6);
  
  if(kodss.getdlinna() > 1)
   {
    zarpensm1_str_tab7(++nomer_str_tab7,data_prik.ravno(),metka_ukr_grom,inn.ravno(),fio.ravno(),nomer_prik.ravno(),kodss.ravno(),dn,dk,mr,gr,row[0],prizv.ravno(),ima.ravno(),othestvo.ravno(),koldn,ff_tab7.ff,ff_xml_tab7,data->window);
   }
 }

class iceb_fioruk_rk kerivnik;
class iceb_fioruk_rk glavbuh;
iceb_fioruk(1,&kerivnik,data->window);
iceb_fioruk(2,&glavbuh,data->window);


fprintf(ff_tab6.ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_tab6.ff,"%*s:%10.2f %10.2f %8.2f %8.2f %10.2f\n",
iceb_u_kolbait(78,gettext("Итого")),
gettext("Итого"),
itogo_tab6[0],itogo_tab6[1],itogo_tab6[2],itogo_tab6[3],itogo_tab6[4]);


zarpensm1_kon_lst(nomer_str_tab6,kerivnik.inn.ravno(),kerivnik.fio.ravno(),glavbuh.inn.ravno(),glavbuh.fio.ravno(),ff_tab6.ff);


fprintf(ff_tab7.ff,"\
--------------------------------------------------------------------------------------------------------------\n");

zarpensm1_kon_lst(nomer_str_tab7,kerivnik.inn.ravno(),kerivnik.fio.ravno(),glavbuh.inn.ravno(),glavbuh.fio.ravno(),ff_tab7.ff);


    

short dt=0,mt=0,gt=0;
iceb_u_poltekdat(&dt,&mt,&gt);


fprintf(ff_xml_tab6,"   <R01G17>%.2f</R01G17>\n",itogo_tab6[0]);
fprintf(ff_xml_tab6,"   <R01G18>%.2f</R01G18>\n",itogo_tab6[1]);
fprintf(ff_xml_tab6,"   <R01G19>%.2f</R01G19>\n",itogo_tab6[2]);
fprintf(ff_xml_tab6,"   <R01G20>%.2f</R01G20>\n",itogo_tab6[3]);
fprintf(ff_xml_tab6,"   <R01G21>%.2f</R01G21>\n",itogo_tab6[4]);

zarpensm1_end_tab_xml(dt,mt,gt,&kerivnik,&glavbuh,ff_xml_tab6);/*концоквка таблицы*/

fclose(ff_xml_tab6);

zarpensm1_end_tab_xml(dt,mt,gt,&kerivnik,&glavbuh,ff_xml_tab7);/*концоквка таблицы*/

fclose(ff_xml_tab7);

iceb_podpis(ff_tab6.ff,data->window);
iceb_podpis(ff_tab7.ff,data->window);

ff_tab6.end();
ff_tab7.end();


time_t vrem;
time(&vrem);
struct tm *bf;
bf=localtime(&vrem);

char kodedrp[16];
memset(kodedrp,'\0',sizeof(kodedrp));
strncpy(kodedrp,inn_or.ravno(),10);
for(int nom=0; nom < 10; nom++)
 if(kodedrp[nom] == '\0')
  kodedrp[nom]='Z';
char datadok[512];
sprintf(datadok,"%02d%02d%04d",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);
char vremdok[512];
sprintf(vremdok,"%02d%02d%02d",bf->tm_hour,bf->tm_min,bf->tm_sec);


char imaf_tmp[64];
sprintf(imaf_tmp,"zarpensm1%d.tmp",getpid());

nomer_str_tab5=zarpensm1_t5(mr,gr,inn_or.ravno(),naim_kontr00.ravno(),nastr.kodpfu.ravno(),imaf_tab5,nastr.kodzvna.ravno(),imaf_xml_tab5,ff_prot.ff,data->window);



data->rk->imaf.plus(imaf_tab5);
data->rk->naimf.plus("Таблиця 5");
data->rk->imaf.plus(imaf_tab6);
data->rk->naimf.plus("Таблиця 6");

data->rk->imaf.plus(imaf_tab7);
data->rk->naimf.plus("Таблиця 7");



if(imaf_prot[0] != '\0')
 {
  data->rk->imaf.plus(imaf_prot);
  data->rk->naimf.plus("Протокол хода расчёта");
 }

for(int nomer=0; nomer < data->rk->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rk->imaf.ravno(nomer),3,data->window);

if(nomer_str_tab5 > 0)
 {
  data->rk->imaf.plus(imaf_xml_tab5);
  data->rk->naimf.plus("Таблиця 5 в форматі xml");
 }

data->rk->imaf.plus(imaf_xml_tab6);
data->rk->naimf.plus("Таблиця 6 в форматі xml");

if(nomer_str_tab7 > 0)
 {
  data->rk->imaf.plus(imaf_xml_tab7);
  data->rk->naimf.plus("Таблиця 7 в форматі xml");
 }
else
 unlink(imaf_xml_tab7);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
