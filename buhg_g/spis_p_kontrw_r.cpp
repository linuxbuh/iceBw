/*$Id: spis_p_kontrw_r.c,v 1.21 2013/09/26 09:46:55 sasa Exp $*/
/*23.05.2016	10.03.2006	Белых А.И.	spis_p_kontrw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "spis_p_kontrw.h"

class spis_p_kontrw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class spis_p_kontrw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  spis_p_kontrw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   spis_p_kontrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class spis_p_kontrw_r_data *data);
gint spis_p_kontrw_r1(class spis_p_kontrw_r_data *data);
void  spis_p_kontrw_r_v_knopka(GtkWidget *widget,class spis_p_kontrw_r_data *data);

int spis_p_kontr_provup(SQL_str row,const char *sklad,const char *grup_mat,const char *kodmat,const char *kontr,const char *kodop,const char *shetz,const char *nomdokp,int *kodmati,class iceb_u_str *ei,class iceb_u_str *kontr_v_dok,GtkWidget *wpredok);
void spis_p_kont_rekpoi(const char *sklad,const char *grup_mat,const char *kodmat,const char *kontr,const char *kodop,const char *shetz,const char *nomdokp,FILE *ff);
void spis_p_kontr_rasrr(class iceb_u_spisok *kodmat_ei,class iceb_u_spisok *spis_kontr,class iceb_u_double *m_kolih,class iceb_u_double *m_summa,FILE *ff,GtkWidget *wpredok);
void spis_p_kontr_rasrr2(class iceb_u_spisok *kodmat_ei,class iceb_u_spisok *spis_kontr,class iceb_u_double *m_kolih,class iceb_u_double *m_summa,double *itogo_k,double *itogo_s,FILE *ff,GtkWidget *wpredok);

extern SQL_baza bd;
extern double   okrg1;
extern double	okrcn;

int spis_p_kontrw_r(class spis_p_kontrw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class spis_p_kontrw_r_data data;
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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(spis_p_kontrw_r_key_press),&data);

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(spis_p_kontrw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)spis_p_kontrw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  spis_p_kontrw_r_v_knopka(GtkWidget *widget,class spis_p_kontrw_r_data *data)
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

gboolean   spis_p_kontrw_r_key_press(GtkWidget *widget,GdkEventKey *event,class spis_p_kontrw_r_data *data)
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

gint spis_p_kontrw_r1(class spis_p_kontrw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
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


SQL_str row;
SQLCURSOR cur;
int kolstr=0;
float kolstr1=0.;

int tipz=data->rk->prih_rash;


if(tipz != 0)
 sprintf(strsql,"select * from Zkart where datdp >= '%d-%d-%d' and datdp <= '%d-%d-%d' and tipz=%d",
 gn,mn,dn,gk,mk,dk,tipz);
else
 sprintf(strsql,"select * from Zkart where datdp >= '%d-%d-%d' and datdp <= '%d-%d-%d'",
 gn,mn,dn,gk,mk,dk);

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

FILE *fftmp;
char imaftmp[64];
sprintf(imaftmp,"skontr%d.tmp",getpid());

if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_spisok kodmat_ei;
class iceb_u_spisok spis_kontr;

class iceb_u_spisok kodmat_ei_p;
class iceb_u_spisok spis_kontr_p;

class iceb_u_str ei("");
class iceb_u_str kontr_v_dok("");

//Создаём нужные списки
int kolstr2=0;
kolstr1=0.;
int tipzz=0;
int kodmati=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  
  if(iceb_u_SRAV(row[2],"000",0) == 0) //Стартовый остаток пропускаем
   continue;
  
  tipzz=atoi(row[5]);

  if(spis_p_kontr_provup(row,data->rk->sklad.ravno(),data->rk->grup_mat.ravno(),data->rk->kodmat.ravno(),data->rk->kontr.ravno(),
  data->rk->kodop.ravno(),data->rk->shetu.ravno(),data->rk->nomdok.ravno(),&kodmati,&ei,&kontr_v_dok,data->window) != 0) 
   continue;
  
  if(iceb_u_SRAV(kontr_v_dok.ravno(),"00-",1) == 0)
   kontr_v_dok.new_plus("00");

  sprintf(strsql,"%d|%s",kodmati,ei.ravno());
    
  if(tipzz == 2)
   { 
    if(kodmat_ei.find(strsql) < 0)
     kodmat_ei.plus(strsql);

    if(spis_kontr.find_r(kontr_v_dok.ravno()) < 0)
      spis_kontr.plus(kontr_v_dok.ravno());
   }
  
  if(tipzz == 1)
   { 
    if(kodmat_ei_p.find(strsql) < 0)
     kodmat_ei_p.plus(strsql);

    if(spis_kontr_p.find_r(kontr_v_dok.ravno()) < 0)
     spis_kontr_p.plus(kontr_v_dok.ravno());
   }
   
  fprintf(fftmp,"%d|%s|%s|%s|%s|%d|\n",kodmati,ei.ravno(),kontr_v_dok.ravno(),row[6],row[7],tipzz);
  kolstr2++;
 }
fclose(fftmp);

if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
unlink(imaftmp);
//Создаём массивы для количества и суммы
class iceb_u_double m_kolih;
class iceb_u_double m_summa;

m_kolih.make_class(kodmat_ei.kolih()*spis_kontr.kolih());
m_summa.make_class(kodmat_ei.kolih()*spis_kontr.kolih());

class iceb_u_double m_kolih_p;
class iceb_u_double m_summa_p;

m_kolih_p.make_class(kodmat_ei_p .kolih()*spis_kontr_p.kolih());
m_summa_p.make_class(kodmat_ei_p.kolih()*spis_kontr_p.kolih());

int nomer_kodmat_ei;
int nomer_kontr;
double kolih,cena;

kolstr1=0.;
double suma;
char stroka[1024];
int kolih_km=kodmat_ei.kolih();
int kolih_km_p=kodmat_ei_p.kolih();

while(fgets(stroka,sizeof(stroka)-1,fftmp) != NULL)
 {
  iceb_pbar(data->bar,kolstr2,++kolstr1);    

  iceb_u_polen(stroka,&kodmati,1,'|');

  iceb_u_polen(stroka,&ei,2,'|');

  iceb_u_polen(stroka,&kontr_v_dok,3,'|');

  iceb_u_polen(stroka,&kolih,4,'|');

  iceb_u_polen(stroka,&cena,5,'|');

  iceb_u_polen(stroka,&tipzz,6,'|');

  suma=kolih*cena;
  
  sprintf(strsql,"%d|%s",kodmati,ei.ravno());
  if(tipzz == 2)
   {
    nomer_kodmat_ei=kodmat_ei.find(strsql);
    nomer_kontr=spis_kontr.find_r(kontr_v_dok.ravno());

    m_kolih.plus(kolih,nomer_kontr*kolih_km+nomer_kodmat_ei);
    m_summa.plus(suma,nomer_kontr*kolih_km+nomer_kodmat_ei);
   }  
  if(tipzz == 1)
   {
    nomer_kodmat_ei=kodmat_ei_p.find(strsql);
    nomer_kontr=spis_kontr_p.find_r(kontr_v_dok.ravno());

    m_kolih_p.plus(kolih,nomer_kontr*kolih_km_p+nomer_kodmat_ei);
    m_summa_p.plus(suma,nomer_kontr*kolih_km_p+nomer_kodmat_ei);
   }  
 }
fclose(fftmp);

FILE *ff;


char imaf[64];
sprintf(imaf,"skontr%d.lst",getpid());

data->rk->imaf.new_plus(imaf);
data->rk->naimf.new_plus(gettext("Расчёт списания/получения материаллов по контрагентам"));

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_zagolov(gettext("Расчёт списания/получения материаллов по контрагентам"),dn,mn,gn,dk,mk,gk,ff,data->window);

spis_p_kont_rekpoi(data->rk->sklad.ravno(),data->rk->grup_mat.ravno(),data->rk->kodmat.ravno(),data->rk->kontr.ravno(),data->rk->kodop.ravno(),data->rk->shetu.ravno(),data->rk->nomdok.ravno(),ff);

if(tipz == 0 || tipz == 2)
 {
  fprintf(ff,"\n%s\n",gettext("Расчёт списания материалов по контрагентам"));
  spis_p_kontr_rasrr(&kodmat_ei,&spis_kontr,&m_kolih,&m_summa,ff,data->window);
 }
if(tipz == 0 || tipz == 1)
 {
  fprintf(ff,"\n%s\n",gettext("Расчёт получения материалов по контрагентам"));
  spis_p_kontr_rasrr(&kodmat_ei_p,&spis_kontr_p,&m_kolih_p,&m_summa_p,ff,data->window);
 }
iceb_podpis(ff,data->window);

fclose(ff);

char imaf2[64];
sprintf(imaf2,"skontr2%d.lst",getpid());
data->rk->imaf.plus(imaf2);
data->rk->naimf.plus(gettext("Расчёт списания/получения материаллов по контрагентам"));

if((ff = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_zagolov(gettext("Расчёт списания/получения материаллов по контрагентам"),dn,mn,gn,dk,mk,gk,ff,data->window);

spis_p_kont_rekpoi(data->rk->sklad.ravno(),data->rk->grup_mat.ravno(),data->rk->kodmat.ravno(),data->rk->kontr.ravno(),data->rk->kodop.ravno(),data->rk->shetu.ravno(),data->rk->nomdok.ravno(),ff);

double itogo_k_p=0.;
double itogo_s_p=0.;
double itogo_k_r=0.;
double itogo_s_r=0.;

if(tipz == 0 || tipz == 2)
 {
  fprintf(ff,"\n%s\n",gettext("Расчёт списания материалов по контрагентам"));
  spis_p_kontr_rasrr2(&kodmat_ei,&spis_kontr,&m_kolih,&m_summa,&itogo_k_r,&itogo_s_r,ff,data->window);
 }
if(tipz == 0 || tipz == 1)
 {
  fprintf(ff,"\n%s\n",gettext("Расчёт поучения материалов по контрагентам"));
  spis_p_kontr_rasrr2(&kodmat_ei_p,&spis_kontr_p,&m_kolih_p,&m_summa_p,&itogo_k_p,&itogo_s_p,ff,data->window);
 }
iceb_podpis(ff,data->window);

fclose(ff);


iceb_printw(gettext("Получено"),data->buffer,data->view);
iceb_printw("\n",data->buffer,data->view);


sprintf(strsql,"%s:%10.10g\n",gettext("Количество"),itogo_k_p);
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s:%10.2f\n",gettext("Сумма"),itogo_s_p);
iceb_printw(strsql,data->buffer,data->view);

iceb_printw("-------------------------------\n",data->buffer,data->view);

iceb_printw(gettext("Списано"),data->buffer,data->view);
iceb_printw("\n",data->buffer,data->view);

sprintf(strsql,"%s:%10.10g\n",gettext("Количество"),itogo_k_r);
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s:%10.2f\n",gettext("Сумма"),itogo_s_r);
iceb_printw(strsql,data->buffer,data->view);


iceb_ustpeh(imaf,0,data->window);
iceb_ustpeh(imaf2,0,data->window);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/***********************************/
/*Проверка на условия поиска*/
/*******************************/

int spis_p_kontr_provup(SQL_str row,
const char *sklad,
const char *grup_mat,
const char *kodmat,
const char *kontr,
const char *kodop,
const char *shetz,
const char *nomdokp,
int *kodmati,
class iceb_u_str *ei,
class iceb_u_str *kontr_v_dok,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row1;
SQLCURSOR cur1;

if(iceb_u_proverka(sklad,row[0],0,0) != 0)
  return(1);
if(iceb_u_proverka(nomdokp,row[2],0,0) != 0)
  return(1);

sprintf(strsql,"select kontr,kodop from Dokummat where datd='%s' and sklad=%s and nomd='%s'",
row[4],row[0],row[2]);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
 {
  class iceb_u_str repl;
  repl.plus(gettext("Не найдена шапка документа !"));

  sprintf(strsql,"%s %s %s",row[4],row[0],row[2]);
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }


class iceb_u_str kontr_dok("");
kontr_dok.new_plus(row1[0]);
if(iceb_u_SRAV(kontr_dok.ravno(),"00-",1) == 0)
   kontr_dok.new_plus("00");

if(iceb_u_proverka(kontr,kontr_dok.ravno(),0,0) != 0)
  return(1);

if(iceb_u_proverka(kodop,row1[1],0,0) != 0)
  return(1);

kontr_v_dok->new_plus(kontr_dok.ravno());

//Читаем запись в документе

sprintf(strsql,"select kodm from Dokummat1 where datd='%s' and sklad=%s and nomd='%s' and \
nomkar=%s",row[4],row[0],row[2],row[1]);

if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
 {
  class iceb_u_str repl;

  sprintf(strsql,"%s !",gettext("Не найдена запись в документе"));
  repl.plus(strsql);

  sprintf(strsql,"%s %s %s %s",row[4],row[0],row[2],row[1]);
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }    
if(iceb_u_proverka(kodmat,row1[0],0,0) != 0)
 return(1);

*kodmati=atoi(row1[0]);

if(grup_mat[0] != '\0') //Узнаём группу материалла
 {
  sprintf(strsql,"select kodgr from Material where kodm=%s",row1[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    
    sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),row1[0]);
    iceb_menu_soob(strsql,wpredok);
    return(1);

   }
  if(iceb_u_proverka(grup_mat,row1[0],0,0) != 0)
   return(1);
 }     


//Читаем карточку документа
sprintf(strsql,"select ei,shetu from Kart where sklad=%s and nomk=%s",row[0],row[1]);
if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
 {
  class iceb_u_str repl;
  sprintf(strsql,"%s !",gettext("Не найдена карточка материалла"));
  repl.plus(strsql);
  
  sprintf(strsql,"%s %s",row[0],row[1]);
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
 }
if(iceb_u_proverka(shetz,row1[1],0,0) != 0)
   return(1);
ei->new_plus(row1[0]);
return(0);
}
/*******************************/
/*строка подчёркивания*/
/*************************/
void spis_p_kontr_strp(int kolih_km,int metka,FILE *ff)
{
//Полка над наименованием контрагента
if(metka == 0)
 fprintf(ff,"------------------------------");
if(metka == 1)
  fprintf(ff,"-------------------------------------");
for(int ii=0; ii < kolih_km+1; ii++)
 fprintf(ff,"---------------------");
 //          123456789|1234567890|
fprintf(ff,"\n");
}

/*******************************************/
/*Распечатка результатов расчёта*/
/***********************************/
void spis_p_kontr_rasrr(class iceb_u_spisok *kodmat_ei,
class iceb_u_spisok *spis_kontr,
class iceb_u_double *m_kolih,
class iceb_u_double *m_summa,
FILE *ff,
GtkWidget *wpredok)
{

char strsql[512];
class iceb_u_str kodmat("");
class iceb_u_str naim("");
SQL_str row;
SQLCURSOR cur;
int kolih_km=kodmat_ei->kolih();


spis_p_kontr_strp(kolih_km,0,ff);


fprintf(ff,"  Код   |  Контрагент        |");
//Пишем первую строку с наименованиями материаллов
for(int ii=0; ii < kolih_km; ii++)
 {
  iceb_u_polen(kodmat_ei->ravno(ii),&kodmat,1,'|');
  
  //узнаём наименование материалла

  sprintf(strsql,"select naimat from Material where kodm=%s",kodmat.ravno());  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim.new_plus(row[0]);  
  else
   naim.new_plus("");
  sprintf(strsql,"%s %s",kodmat.ravno(),naim.ravno());

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,strsql),iceb_u_kolbait(20,strsql),strsql);
 }
fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,gettext("Итого")),iceb_u_kolbait(20,gettext("Итого")),gettext("Итого"));
fprintf(ff,"\n");

fprintf(ff,"        |                    |");

//Пишем вторую строку с единицами измерения

for(int ii=0; ii < kolih_km; ii++)
 {
  iceb_u_polen(kodmat_ei->ravno(ii),&kodmat,2,'|');

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,kodmat.ravno()),iceb_u_kolbait(20,kodmat.ravno()),kodmat.ravno());
 }
fprintf(ff,"%-20.20s|","");
fprintf(ff,"\n");

fprintf(ff,"        |                    |");

//строка разделения

for(int ii=0; ii < kolih_km+1; ii++)
 {
  fprintf(ff,"---------------------");
 }
fprintf(ff,"\n");


fprintf(ff,"        |                    |");


for(int ii=0; ii < kolih_km+1; ii++)
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

spis_p_kontr_strp(kolih_km,0,ff);
char kontr[54];
//Распечатываем массив
for(int skontr=0; skontr < spis_kontr->kolih(); skontr++)
 {
  if(iceb_u_SRAV(spis_kontr->ravno(skontr),"00-",1) == 0)
   strcpy(kontr,"00");
  else
   strcpy(kontr,spis_kontr->ravno(skontr));
  
  //узнаём наименование контрагента
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");

  fprintf(ff,"%-*s|%-*.*s|",
  iceb_u_kolbait(8,spis_kontr->ravno(skontr)),spis_kontr->ravno(skontr),
  iceb_u_kolbait(20,naim.ravno()),iceb_u_kolbait(20,naim.ravno()),naim.ravno());
  
  double itog_str_k=0.;
  double itog_str_s=0.;
  for(int ii=0; ii < kolih_km; ii++)
   {
    double kolih=m_kolih->ravno(skontr*kolih_km+ii);
    double suma=m_summa->ravno(skontr*kolih_km+ii);

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

spis_p_kontr_strp(kolih_km,0,ff);

fprintf(ff,"%*s|",iceb_u_kolbait(29,gettext("Итого")),gettext("Итого"));

//Распечатка итоговой строки
double itogo_po_kol_kolih=0.;
double itogo_po_kol_suma=0.;
double itogo_k=0.;
double itogo_s=0.;

for(int ii=0; ii < kolih_km; ii++)
 {
  itogo_po_kol_kolih=0.;
  itogo_po_kol_suma=0.;
  for(int skontr=0; skontr < spis_kontr->kolih(); skontr++)
   {
    itogo_po_kol_kolih+=m_kolih->ravno(skontr*kolih_km+ii);
    itogo_po_kol_suma+=m_summa->ravno(skontr*kolih_km+ii);
   }
  fprintf(ff,"%9.9g|%10.2f|",itogo_po_kol_kolih,itogo_po_kol_suma);
  itogo_k+=itogo_po_kol_kolih;
  itogo_s+=itogo_po_kol_suma;  
 }
fprintf(ff,"%9.9g|%10.2f|",itogo_k,itogo_s);
fprintf(ff,"\n");

}


/*******************************************/
/*Распечатка результатов расчёта*/
/***********************************/
void spis_p_kontr_rasrr2(class iceb_u_spisok *kodmat_ei,
class iceb_u_spisok *spis_kontr,
class iceb_u_double *m_kolih,
class iceb_u_double *m_summa,
double *itogo_k,
double *itogo_s,
FILE *ff,
GtkWidget *wpredok)
{

char strsql[512];
class iceb_u_str kodmat("");
class iceb_u_str naim("");
SQL_str row;
SQLCURSOR cur;
class iceb_u_str kontr("");
int kolih_kontr=spis_kontr->kolih();
int kolih_km=kodmat_ei->kolih();


spis_p_kontr_strp(kolih_kontr,1,ff);


fprintf(ff," Код |Наименование материалла|Ед.из.|");
//          12345 12345678901234567890123 123456     
//Пишем первую строку с наименованиями контрагентов
for(int ii=0; ii < kolih_kontr; ii++)
 {
  if(iceb_u_SRAV(spis_kontr->ravno(ii),"00-",1) == 0)
   kontr.new_plus("00");
  else
   kontr.new_plus(spis_kontr->ravno(ii));
  
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");

  sprintf(strsql,"%s %s",kontr.ravno(),naim.ravno());

  fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,strsql),iceb_u_kolbait(20,strsql),strsql);
 }
fprintf(ff,"%-*.*s|",iceb_u_kolbait(20,gettext("Итого")),iceb_u_kolbait(20,gettext("Итого")),gettext("Итого"));
fprintf(ff,"\n");


fprintf(ff,"     |                       |      |");

//строка разделения

for(int ii=0; ii < kolih_kontr+1; ii++)
 {
  fprintf(ff,"---------------------");
 }
fprintf(ff,"\n");


fprintf(ff,"     |                       |      |");


for(int ii=0; ii < kolih_kontr+1; ii++)
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

spis_p_kontr_strp(kolih_kontr,1,ff);
class iceb_u_str ei("");
//Распечатываем массив
for(int skontr=0; skontr < kodmat_ei->kolih(); skontr++)
 {
  iceb_u_polen(kodmat_ei->ravno(skontr),&kodmat,1,'|');
  iceb_u_polen(kodmat_ei->ravno(skontr),&ei,2,'|');


  sprintf(strsql,"select naimat from Material where kodm=%s",kodmat.ravno());  
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");

  fprintf(ff,"%-5s|%-*.*s|%-*.*s|",
  kodmat.ravno(),
  iceb_u_kolbait(23,naim.ravno()),iceb_u_kolbait(23,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(6,ei.ravno()),iceb_u_kolbait(6,ei.ravno()),ei.ravno());

  double itog_str_k=0.;
  double itog_str_s=0.;
  for(int ii=0; ii < kolih_kontr; ii++)
   {
    double kolih=m_kolih->ravno(ii*kolih_km+skontr);
    double suma=m_summa->ravno(ii*kolih_km+skontr);

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

spis_p_kontr_strp(kolih_kontr,1,ff);

fprintf(ff,"%*s|",iceb_u_kolbait(36,gettext("Итого")),gettext("Итого"));

//Распечатка итоговой строки
double itogo_po_kol_kolih=0.;
double itogo_po_kol_suma=0.;
*itogo_k=0.;
*itogo_s=0.;

for(int ii=0; ii < kolih_kontr; ii++)
 {
  itogo_po_kol_kolih=0.;
  itogo_po_kol_suma=0.;
  for(int skontr=0; skontr < kodmat_ei->kolih(); skontr++)
   {
    itogo_po_kol_kolih+=m_kolih->ravno(ii*kolih_km+skontr);
    itogo_po_kol_suma+=m_summa->ravno(ii*kolih_km+skontr);
   }
  fprintf(ff,"%9.9g|%10.2f|",itogo_po_kol_kolih,itogo_po_kol_suma);
  *itogo_k+=itogo_po_kol_kolih;
  *itogo_s+=itogo_po_kol_suma;  
 }
fprintf(ff,"%9.9g|%10.2f|",*itogo_k,*itogo_s);

fprintf(ff,"\n");


}



/*******************************/
/*Печать реквизитов поиска*/
/****************************/
void spis_p_kont_rekpoi(const char *sklad,
const char *grup_mat,
const char *kodmat,
const char *kontr,
const char *kodop,
const char *shetz,
const char *nomdokp,
FILE *ff)
{
if(sklad[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Склад"),sklad);
if(grup_mat[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Группа"),grup_mat);
if(kodmat[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код материалла"),kodmat);
if(kontr[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код контрагента"),kontr);
if(kodop[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код операции"),kodop);
if(shetz[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт учёта"),shetz);
if(nomdokp[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Номер документа"),nomdokp);

}
