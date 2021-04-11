/*$Id: ost_skl_shetw_r.c,v 1.18 2013/09/26 09:46:52 sasa Exp $*/
/*23.05.2016	17.11.2008	Белых А.И.	ost_skl_shetw_r.c
Расчёт отчёта остатка 
*/
#include <errno.h>
#include "buhg_g.h"
#include "ost_skl_shetw.h"

class ost_skl_shetw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
    
  class ost_skl_shetw_data *rk;
  
  int voz; //0-все в порядке расчёт сделан 1-нет
  ost_skl_shetw_r_data()
   {
    voz=1;
   }
 };

gboolean   ost_skl_shetw_r_key_press(GtkWidget *widget,GdkEventKey *event,class ost_skl_shetw_r_data *data);
gint ost_skl_shetw_r1(class ost_skl_shetw_r_data *data);
void  ost_skl_shetw_r_v_knopka(GtkWidget *widget,class ost_skl_shetw_r_data *data);

extern SQL_baza bd;
extern short	vtara; /*Код группы возвратная тара*/
extern int      kol_strok_na_liste;

int ost_skl_shetw_r(class ost_skl_shetw_data *rek_ras,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_spisok repl_s;
class ost_skl_shetw_r_data data;
data.rk=rek_ras;
data.name_window.plus(__FUNCTION__);



data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт остатков по складам и счетам учёта"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(ost_skl_shetw_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Расчёт остатков по складам и счетам учёта"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(ost_skl_shetw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)ost_skl_shetw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  ost_skl_shetw_r_v_knopka(GtkWidget *widget,class ost_skl_shetw_r_data *data)
{
//printf("ost_skl_shetw_r_v_knopka\n");
if(data->voz == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   ost_skl_shetw_r_key_press(GtkWidget *widget,GdkEventKey *event,class ost_skl_shetw_r_data *data)
{
// printf("ost_skl_shetw_r_key_press\n");
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
/*Строка подчёркивания*/
/********************************/
void ost_skl_shet_str(int kol,int metka,FILE *ff)
{
//Полка над наименованием контрагента
if(metka == 0)
 fprintf(ff,"---------------------------");
if(metka == 1)
 fprintf(ff,"-----------------------------");

for(int ii=0; ii < kol+1; ii++)
 fprintf(ff,"---------------------");
 //          123456789|1234567890|
fprintf(ff,"\n");
}


/**********************************/
/*Проверка на условия поиска*/
/*********************************/

int ost_skl_shet_prov(SQL_str row,
const char *sklad,
const char *grup_mat,
const char *kodmat,
const char *shetz,
GtkWidget *wpredok)
{

if(iceb_u_proverka(sklad,row[0],0,0) != 0)
  return(1);
if(iceb_u_proverka(kodmat,row[2],0,0) != 0)
  return(1);
if(iceb_u_proverka(shetz,row[3],0,0) != 0)
  return(1);
if(grup_mat[0] != '\0') //Узнаём группу материалла
 {
  SQL_str row1;
  SQLCURSOR cur1;
  char strsql[512];
  sprintf(strsql,"select kodgr from Material where kodm=%s",row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),row[2]);
    iceb_menu_soob(strsql,wpredok);
    return(1);

   }
  if(iceb_u_proverka(grup_mat,row1[0],0,0) != 0)
   return(1);
 }     

return(0);
}

/*****************************/
/*Распечатка массивов по складам*/
/*****************************/
void ost_skl_shet_rm_skl(class iceb_u_int *sp_skl,
class iceb_u_spisok *sp_shetu,
class iceb_u_double *m_kolih,
class iceb_u_double *m_suma,
FILE *ff,
GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str naim("");
SQL_str row;
SQLCURSOR cur;

int kolih_shet=sp_shetu->kolih();
int kolih_skl=sp_skl->kolih();

ost_skl_shet_str(kolih_shet,0,ff);


fprintf(ff,"%-*.*s|%-*.*s|",
iceb_u_kolbait(5,gettext("Код")),
iceb_u_kolbait(5,gettext("Код")),
gettext("Код"),
iceb_u_kolbait(20,gettext("Склад")),
iceb_u_kolbait(20,gettext("Склад")),
gettext("Склад"));
//          12345 12345678901234567890
//Пишем первую строку со счетами
for(int ii=0; ii < kolih_shet; ii++)
 {
  //узнаём наименование 

  sprintf(strsql,"select nais from Plansh where ns='%s'",sp_shetu->ravno(ii));  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim.new_plus(row[0]);  
  else
   naim.new_plus("");

  sprintf(strsql,"%s %s",sp_shetu->ravno(ii),naim.ravno());

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,strsql),iceb_u_kolbait(20,strsql),strsql);
 }
fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,gettext("Итого")),iceb_u_kolbait(20,gettext("Итого")),gettext("Итого"));
fprintf(ff,"\n");

fprintf(ff,"     |                    |");

//строка разделения

for(int ii=0; ii < kolih_shet+1; ii++)
 {
  fprintf(ff,"---------------------");
 }
fprintf(ff,"\n");


fprintf(ff,"     |                    |");


for(int ii=0; ii < kolih_shet+1; ii++)
 {
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_u_kolbait(9,gettext("Количество")),
  iceb_u_kolbait(9,gettext("Количество")),
  gettext("Количество"),
  iceb_u_kolbait(10,gettext("Сумма")),
  iceb_u_kolbait(10,gettext("Сумма")),
  gettext("Сумма"));
 }
fprintf(ff,"\n");

ost_skl_shet_str(kolih_shet,0,ff);

//Распечатываем массив
for(int s_skl=0; s_skl < kolih_skl; s_skl++)
 {
  //узнаём наименование 

  sprintf(strsql,"select naik from Sklad where kod=%d",sp_skl->ravno(s_skl));  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim.new_plus(row[0]);  
  else
   naim.new_plus("");

  fprintf(ff,"%-5d|%-*.*s|",
  sp_skl->ravno(s_skl),
  iceb_u_kolbait(20,naim.ravno()),iceb_u_kolbait(20,naim.ravno()),naim.ravno());

  double itog_str_k=0.;
  double itog_str_s=0.;
  for(int s_shet=0; s_shet < kolih_shet; s_shet++)
   {
    double kolih=m_kolih->ravno(s_skl*kolih_shet+s_shet);
    double suma=m_suma->ravno(s_skl*kolih_shet+s_shet);

    if(kolih == 0.)
      fprintf(ff,"%9s|","");
    else
      fprintf(ff,"%9.9g|",kolih);

    if(suma == 0.)
      fprintf(ff,"%10s|","");
    else
      fprintf(ff,"%10.2f|",suma);
    itog_str_k+=kolih;
    itog_str_s+=suma;
   }
  fprintf(ff,"%9.9g|%10.2f|",itog_str_k,itog_str_s);
  fprintf(ff,"\n");    
   
 }

ost_skl_shet_str(kolih_shet,0,ff);

fprintf(ff,"%*s|",iceb_u_kolbait(26,gettext("Итого")),gettext("Итого"));

//Распечатка итоговой строки
double itogo_po_kol_kolih=0.;
double itogo_po_kol_suma=0.;
double itogo_k=0.;
double itogo_s=0.;

for(int ii=0; ii < kolih_shet; ii++)
 {
  itogo_po_kol_kolih=0.;
  itogo_po_kol_suma=0.;
  for(int skontr=0; skontr < kolih_skl; skontr++)
   {
    itogo_po_kol_kolih+=m_kolih->ravno(skontr*kolih_shet+ii);
    itogo_po_kol_suma+=m_suma->ravno(skontr*kolih_shet+ii);
   }
  fprintf(ff,"%9.9g|%10.2f|",itogo_po_kol_kolih,itogo_po_kol_suma);
  itogo_k+=itogo_po_kol_kolih;
  itogo_s+=itogo_po_kol_suma;  
 }
fprintf(ff,"%9.9g|%10.2f|",itogo_k,itogo_s);
fprintf(ff,"\n");

}

/*****************************/
/*Распечатка массивов по счетам*/
/*****************************/
void ost_skl_shet_rm_shet(class iceb_u_int *sp_skl,
class iceb_u_spisok *sp_shetu,
class iceb_u_double *m_kolih,
class iceb_u_double *m_suma,
FILE *ff,
GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str naim("");
SQL_str row;
SQLCURSOR cur;

int kolih_shet=sp_shetu->kolih();
int kolih_skl=sp_skl->kolih();

ost_skl_shet_str(kolih_skl,1,ff);


fprintf(ff,"%-*.*s|%-*.*s|",
iceb_u_kolbait(7,gettext("Счёт")),
iceb_u_kolbait(7,gettext("Счёт")),
gettext("Счёт"),
iceb_u_kolbait(20,gettext("Наименование")),
iceb_u_kolbait(20,gettext("Наименование")),
gettext("Наименование"));

//Пишем первую строку со счетами
for(int ii=0; ii < kolih_skl; ii++)
 {
  //узнаём наименование 
  sprintf(strsql,"select naik from Sklad where kod=%d",sp_skl->ravno(ii));  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim.new_plus(row[0]);  
  else
   naim.new_plus("");

  sprintf(strsql,"%d %s",sp_skl->ravno(ii),naim.ravno());

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,strsql),iceb_u_kolbait(20,strsql),strsql);
 }
fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,gettext("Итого")),iceb_u_kolbait(20,gettext("Итого")),gettext("Итого"));
fprintf(ff,"\n");

fprintf(ff,"       |                    |");

//строка разделения

for(int ii=0; ii < kolih_skl+1; ii++)
 {
  fprintf(ff,"---------------------");
 }
fprintf(ff,"\n");


fprintf(ff,"       |                    |");


for(int ii=0; ii < kolih_skl+1; ii++)
 {
  fprintf(ff,"%-*.*s|%-*.*s|",
  iceb_u_kolbait(9,gettext("Количество")),
  iceb_u_kolbait(9,gettext("Количество")),
  gettext("Количество"),
  iceb_u_kolbait(10,gettext("Сумма")),
  iceb_u_kolbait(10,gettext("Сумма")),
  gettext("Сумма"));
 }
fprintf(ff,"\n");

ost_skl_shet_str(kolih_skl,1,ff);

//Распечатываем массив
for(int s_shet=0; s_shet < kolih_shet; s_shet++)
 {
  //узнаём наименование 
  sprintf(strsql,"select nais from Plansh where ns='%s'",sp_shetu->ravno(s_shet));  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");

  fprintf(ff,"%-*s|%-*.*s|",
  iceb_u_kolbait(7,sp_shetu->ravno(s_shet)),sp_shetu->ravno(s_shet),
  iceb_u_kolbait(20,naim.ravno()),iceb_u_kolbait(20,naim.ravno()),naim.ravno());

  double itog_str_k=0.;
  double itog_str_s=0.;

  for(int s_skl=0; s_skl < kolih_skl; s_skl++)
   {
    double kolih=m_kolih->ravno(s_skl*kolih_shet+s_shet);
    double suma=m_suma->ravno(s_skl*kolih_shet+s_shet);

    if(kolih == 0.)
      fprintf(ff,"%9s|","");
    else
      fprintf(ff,"%9.9g|",kolih);

    if(suma == 0.)
      fprintf(ff,"%10s|","");
    else
      fprintf(ff,"%10.2f|",suma);
    itog_str_k+=kolih;
    itog_str_s+=suma;
   }
  fprintf(ff,"%9.9g|%10.2f|",itog_str_k,itog_str_s);
  fprintf(ff,"\n");    
   
 }

ost_skl_shet_str(kolih_skl,1,ff);

fprintf(ff,"%*s|",iceb_u_kolbait(28,gettext("Итого")),gettext("Итого"));

//Распечатка итоговой строки
double itogo_po_kol_kolih=0.;
double itogo_po_kol_suma=0.;
double itogo_k=0.;
double itogo_s=0.;

for(int s_skl=0; s_skl < kolih_skl; s_skl++)
 {
  itogo_po_kol_kolih=0.;
  itogo_po_kol_suma=0.;
  for(int s_shet=0; s_shet < kolih_shet; s_shet++)
   {
                                     
    itogo_po_kol_kolih+=m_kolih->ravno(s_skl*kolih_shet+s_shet);
    itogo_po_kol_suma+=m_suma->ravno(s_skl*kolih_shet+s_shet);
   }
  fprintf(ff,"%9.9g|%10.2f|",itogo_po_kol_kolih,itogo_po_kol_suma);
  itogo_k+=itogo_po_kol_kolih;
  itogo_s+=itogo_po_kol_suma;  
 }
fprintf(ff,"%9.9g|%10.2f|",itogo_k,itogo_s);
fprintf(ff,"\n");
}


/*******************************/
/*Печать реквизитов поиска*/
/****************************/
void ost_skl_shet_rekpoi(const char *sklad,
const char *grup_mat,
const char *kodmat,
const char *shetz,
FILE *ff)
{
if(sklad[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Склад"),sklad);
if(grup_mat[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Группа"),grup_mat);
if(kodmat[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код материалла"),kodmat);
if(shetz[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),shetz);

}


/******************************************/
/******************************************/

gint ost_skl_shetw_r1(class ost_skl_shetw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_u_str repl;
class iceb_clock sss(data->window);


short dk,mk,gk;

if(iceb_u_rsdat(&dk,&mk,&gk,data->rk->data_ost.ravno(),1) != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


SQL_str row;
SQLCURSOR cur;
int kolstr=0;
float kolstr1=0;


sprintf(strsql,"select sklad,nomk,kodm,shetu,cena from Kart");

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
FILE *fftmp;
char imaftmp[64];
sprintf(imaftmp,"ost_skl_shet%d.tmp",getpid());
if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


//Создаём нужные списки
class iceb_u_int sp_skl;
class iceb_u_spisok sp_shetu;
int kolstr2=0;
kolstr1=0;
class ostatok ost;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(ost_skl_shet_prov(row,data->rk->sklad.ravno(),data->rk->grupa.ravno(),data->rk->kodmat.ravno(),data->rk->shet.ravno(),data->window) != 0)
   continue;

  ostkarw(1,1,gk,dk,mk,gk,atoi(row[0]),atoi(row[1]),&ost);
  if(ost.ostg[3] == 0.)
    continue;  

  if(sp_shetu.find(row[3]) < 0)
   sp_shetu.plus(row[3]);

  if(sp_skl.find(row[0]) < 0)
   sp_skl.plus(row[0]);

  fprintf(fftmp,"%s|%s|%s|%10.10g|\n",row[0],row[3],row[4],ost.ostg[3]);
  kolstr2++;  
 }
fclose(fftmp);

class iceb_u_double m_kolih;
class iceb_u_double m_suma;

m_kolih.make_class(sp_shetu.kolih()*sp_skl.kolih());
m_suma.make_class(sp_shetu.kolih()*sp_skl.kolih());

int nom_shet=0;
int nom_skl=0;
double suma=0.;
int kol_shet=sp_shetu.kolih();
cur.poz_cursor(0);
kolstr1=0;
if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
unlink(imaftmp);

char stroka[1024];
class iceb_u_str sklads("");
class iceb_u_str shet("");
class iceb_u_str cena("");
class iceb_u_str kolih("");
kolstr1=0;
while(fgets(stroka,sizeof(stroka)-1,fftmp) != NULL)
 {
  iceb_pbar(data->bar,kolstr2,++kolstr1);    
  iceb_u_polen(stroka,&sklads,1,'|');
  iceb_u_polen(stroka,&shet,2,'|');
  iceb_u_polen(stroka,&cena,3,'|');
  iceb_u_polen(stroka,&kolih,4,'|');

  nom_skl=sp_skl.find(sklads.ravno());
  nom_shet=sp_shetu.find(shet.ravno());

  suma=kolih.ravno_atof()*cena.ravno_atof();
  m_kolih.plus(kolih.ravno_atof(),nom_skl*kol_shet+nom_shet);
  m_suma.plus(suma,nom_skl*kol_shet+nom_shet);
 }
fclose(fftmp);


FILE *ff;

char imaf[64];
sprintf(imaf,"ostskl%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



iceb_zagolov(gettext("Расчёт остатков по складам и счетам учёта"),0,0,0,dk,mk,gk,ff,data->window);

ost_skl_shet_rekpoi(data->rk->sklad.ravno(),data->rk->grupa.ravno(),data->rk->kodmat.ravno(),data->rk->shet.ravno(),ff);

ost_skl_shet_rm_skl(&sp_skl,&sp_shetu,&m_kolih,&m_suma,ff,data->window);

iceb_podpis(ff,data->window);

fclose(ff);

char imaf2[50];
sprintf(imaf2,"ostshet%d.lst",getpid());

if((ff = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


//fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля*/
//fprintf(ff,"\x1B\x4D"); /*12-знаков*/
//fprintf(ff,"\x0F");  /*Ужатый режим*/

iceb_zagolov(gettext("Расчёт остатков по складам и счетам учёта"),0,0,0,dk,mk,gk,ff,data->window);

ost_skl_shet_rekpoi(data->rk->sklad.ravno(),data->rk->grupa.ravno(),data->rk->kodmat.ravno(),data->rk->shet.ravno(),ff);

ost_skl_shet_rm_shet(&sp_skl,&sp_shetu,&m_kolih,&m_suma,ff,data->window);

iceb_podpis(ff,data->window);

fclose(ff);



data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчёт остатков по складам и счетам учёта"));
data->rk->imaf.plus(imaf2);
data->rk->naimf.plus(gettext("Расчёт остатков по складам и счетам учёта"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);
}

