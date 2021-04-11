/*$Id: rozkor_r.c,v 1.29 2013/09/26 09:46:54 sasa Exp $*/
/*23.05.2016	20.10.2004	Белых А.И.	rozkor_r.c
Расчёт оформления возврата
*/
#include <errno.h>
#include "buhg_g.h"
#include "rozkor.h"

class rozkor_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
  
  class rozkor_data *rk;
  short metka_fil; //0-расчёт выполнен 1-нет
  
  iceb_u_spisok imafil;
  iceb_u_spisok naimfil;
    
  rozkor_r_data()
   {
    metka_fil=1;
   }
  

 };

gboolean   rozkor_r_key_press(GtkWidget *widget,GdkEventKey *event,class rozkor_r_data *data);
gint rozkor_r1(class rozkor_r_data *data);
void  rozkor_r_v_knopka(GtkWidget *widget,class rozkor_r_data *data);

extern SQL_baza bd;
 
void rozkor_r(class rozkor_data *rek_ras,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_spisok repl_s;
class rozkor_r_data data;
data.rk=rek_ras;
data.name_window.plus(__FUNCTION__);



data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Оформить возврат"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rozkor_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Оформить возврат"));
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


gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(rozkor_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)rozkor_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.metka_fil == 0)
 iceb_rabfil(&data.imafil,&data.naimfil,wpredok);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rozkor_r_v_knopka(GtkWidget *widget,class rozkor_r_data *data)
{
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rozkor_r_key_press(GtkWidget *widget,GdkEventKey *event,class rozkor_r_data *data)
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

gint rozkor_r1(class rozkor_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;

SQLCURSOR cur;
SQL_str row;

/*Проверяем есть ли такая накладная*/
sprintf(strsql,"select datd,sklad,nomd,kontr from Dokummat where \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomnn='%s'",
data->rk->god_nal_nak.ravno_atoi(),
data->rk->god_nal_nak.ravno_atoi(),
data->rk->nomer_nal_nak.ravno());

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"Не найден документ с номером налоговой накладной %s !",
  data->rk->nomer_nal_nak.ravno());
  iceb_menu_soob(strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
short di,mi,gi;

iceb_u_rsdat(&di,&mi,&gi,row[0],2);
if(iceb_u_sravmydat(data->rk->dd,data->rk->md,data->rk->gd,di,mi,gi) < 0)
 {
  sprintf(strsql,"Дата налоговой накладной %d.%d.%d больше даты возврата %d.%d.%d !",
  di,mi,gi,data->rk->dd,data->rk->md,data->rk->gd);

  iceb_menu_soob(strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);

 }
class iceb_u_str nin(row[2]);
class iceb_u_str kontr(row[3]);



//Узнаем с каким НДС
sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%s and \
nomd='%s' and nomerz=11",data->rk->god_nal_nak.ravno_atoi(),row[1],row[2]);
int lnds1=0;
if(sql_readkey(&bd,strsql,&row,&cur) > 0)
   lnds1=atoi(row[0]);


/*Читаем условие продажи*/
class iceb_u_str uslpr("");

sprintf(strsql,"select sodz from Dokummat2 where god=%d and \
nomd='%s' and sklad=%d and nomerz=7",data->rk->god_nal_nak.ravno_atoi(),nin.ravno(),data->rk->skl);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  uslpr.new_plus(row[0]);


/*Читаем форму оплаты*/
class iceb_u_str forop("");
sprintf(strsql,"select sodz from Dokummat2 where god=%d and \
nomd='%s' and sklad=%d and nomerz=8",data->rk->god_nal_nak.ravno_atoi(),nin.ravno(),data->rk->skl);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  forop.new_plus(row[0]);
if(forop.ravno()[0] != '\0')
 {
  sprintf(strsql,"select naik from Foroplat where kod='%s'",forop.ravno());

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    forop.new_plus(row[0]);
  else
    forop.new_plus("");
 }

class iceb_u_str nai(""),nai1(""); 
class iceb_u_str inn(""),inn1("");
class iceb_u_str adr(""),adr1("");
class iceb_u_str telef(""),telef1("");
class iceb_u_str nspdv(""),nspdv1("");
 
/*Читаем реквизиты организации свои */
sprintf(strsql,"select naikon,adres,innn,nspnds,telef from Kontragent \
where kodkon='00'");
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {

  repl.new_plus(gettext("Не найден код контрагента 00 !"));
  iceb_menu_soob(&repl,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


nai.new_plus(row[0]);
adr.new_plus(row[1]);
inn.new_plus(row[2]);
nspdv.new_plus(row[3]);
telef.new_plus(row[4]);

sprintf(strsql,"select naikon,adres,innn,nspnds,telef from Kontragent \
where kodkon='%s'",kontr.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код контрагента !"),kontr.ravno());
  iceb_menu_soob(strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

nai1.new_plus(row[0]);
adr1.new_plus(row[1]);
inn1.new_plus(row[2]);
nspdv1.new_plus(row[3]);
telef1.new_plus(row[4]);

int kolstr;
/*Читаем документ*/
sprintf(strsql,"select kodm,nomkar,kolih,ei from Dokummat1 where sklad=%d and nomd='%s' \
and datd='%d-%d-%d'",data->rk->skl,data->rk->nomdok_c.ravno(),data->rk->gd,data->rk->md,data->rk->dd);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  repl.new_plus(gettext("Не найдено ни одной записи !"));
  iceb_menu_soob(&repl,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

SQL_str row_alx;
class SQLCURSOR cur_alx;

sprintf(strsql,"select str from Alx where fil='dokrozr.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


char imaf[512];
sprintf(imaf,"rozvoz%d.lst",getpid());
FILE *ff;
data->imafil.new_plus(imaf);
data->naimfil.new_plus("Розсчёт коригування кількісних і вартісних показників до податкавої накладної");

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

/*Начинаем формирование нового документа*/
fprintf(ff,"\x1B\x33%c",28); /*Уменьшаем межстрочный интервал*/
class iceb_u_str str("");
//memset(str,'\0',sizeof(str));

cur_alx.read_cursor(&row_alx);
fprintf(ff,"%s",row_alx[0]);

cur_alx.read_cursor(&row_alx);
fprintf(ff,"%s",row_alx[0]);

cur_alx.read_cursor(&row_alx);
fprintf(ff,"%s",row_alx[0]);
str.new_plus(row_alx[0]);


sprintf(strsql,"%s / %s",data->rk->nomer_ras.ravno(),data->rk->nomer_nal_nak.ravno());
iceb_u_vstav(&str,strsql,121,144,1);
fprintf(ff,"%s",str.ravno());

cur_alx.read_cursor(&row_alx);
fprintf(ff,"%s",row_alx[0]);

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
sprintf(strsql,"%d.%d.%d",di,mi,gi);
iceb_u_vstav(&str,strsql,78,88,1);
iceb_u_vstav(&str,data->rk->nomer_nal_nak.ravno(),94,105,1);
iceb_u_vstav(&str,data->rk->data_dogovora.ravno(),124,135,1);
iceb_u_vstav(&str,data->rk->nomer_dogovora.ravno(),140,154,1);
fprintf(ff,"%s",str.ravno());

cur_alx.read_cursor(&row_alx);
fprintf(ff,"%s",row_alx[0]);

cur_alx.read_cursor(&row_alx);
fprintf(ff,"%s",row_alx[0]);

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_u_vstav(&str,nai.ravno(),36,75,1);
iceb_u_vstav(&str,nai1.ravno(),155,193,1);
fprintf(ff,"%s",str.ravno());

for(int i=0; i < 3 ; i++)
 {
  cur_alx.read_cursor(&row_alx);
  fprintf(ff,"%s",row_alx[0]);
 }

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_u_vstav(&str,inn.ravno(),13,57,4);
iceb_u_vstav(&str,inn1.ravno(),139,183,4);
fprintf(ff,"%s",str.ravno());

for(int i=0; i < 2 ; i++)
 {
  cur_alx.read_cursor(&row_alx);
  fprintf(ff,"%s",row_alx[0]);
 }

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_u_vstav(&str,adr.ravno(),25,64,1);
iceb_u_vstav(&str,adr1.ravno(),120,159,1);
fprintf(ff,"%s",str.ravno());

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_u_vstav(&str,telef.ravno(),17,56,1);
iceb_u_vstav(&str,telef1.ravno(),150,189,1);
fprintf(ff,"%s",str.ravno());

cur_alx.read_cursor(&row_alx);
fprintf(ff,"%s",row_alx[0]);

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_u_vstav(&str,nspdv.ravno(),45,64,1);
iceb_u_vstav(&str,nspdv1.ravno(),138,157,1);
fprintf(ff,"%s",str.ravno());

cur_alx.read_cursor(&row_alx);
fprintf(ff,"%s",row_alx[0]);

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_u_vstav(&str,uslpr.ravno(),20,49,1);
fprintf(ff,"%s",str.ravno());

for(int i=0; i < 2 ; i++)
 {
  cur_alx.read_cursor(&row_alx);
  fprintf(ff,"%s",row_alx[0]);
 }

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_u_vstav(&str,forop.ravno(),30,69,1);
fprintf(ff,"%s",str.ravno());

cur_alx.read_cursor(&row_alx);
str.new_plus(row_alx[0]);
iceb_u_vstav(&str,data->rk->kto_v_ras.ravno(),40,69,1);
fprintf(ff,"%s",str.ravno());

fprintf(ff,"\x1B\x33%c",20); /*Уменьшаем межстрочный интервал*/
for(int i=0; i < 19 ; i++)
 {
  cur_alx.read_cursor(&row_alx);
  fprintf(ff,"%s",row_alx[0]);
 }
fprintf(ff,"\x1B\x33%c",28); /*Уменьшаем межстрочный интервал*/

sprintf(strsql,"Номер накладной %s Номер налоговой накладной %s\n",
nin.ravno(),data->rk->nomer_nal_nak.ravno());

iceb_printw(strsql,data->buffer,data->view);

fprintf(ff,"\x1B\x2D\x31"); /*Включить подчеркивание*/
SQLCURSOR cur1;
SQL_str row1;
double it1=0.,it2=0.;
float kolstr1=0.;
int kodm;
int nk;
double kolih;
class iceb_u_str ei("");
class iceb_u_str naimat("");

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  kodm=atoi(row[0]);
  nk=atoi(row[1]);
  kolih=atof(row[2]);
  ei.new_plus(row[3]);
      
  /*Узнаем наименование материалла*/
  sprintf(strsql,"select naimat from Material where kodm=%s",
  row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найдено код материалла"),row[0]);
    iceb_menu_soob(strsql,data->window);
    naimat.new_plus("");
   }
  else
    naimat.new_plus(row1[0]);


  sprintf(strsql,"%4d %4d %-*.*s %.8g\n",
  kodm,nk,
  iceb_u_kolbait(30,naimat.ravno()),iceb_u_kolbait(30,naimat.ravno()),naimat.ravno(),
  kolih);
  iceb_printw(strsql,data->buffer,data->view);

  /*Проверяем есть ли материал в исходной накладной и если есть то 
  сколько по количеству*/
  sprintf(strsql,"select kolih,cena from Dokummat1 where \
sklad=%d and nomd='%s' and datd='%d-%d-%d' and kodm=%d and nomkar=%d",
data->rk->skl,nin.ravno(),gi,mi,di,kodm,nk);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s\n",gettext("Нет такого материалла в исходной накладной !"));
    iceb_printw(strsql,data->buffer,data->view);
    continue;
   }


  double kolih1=atof(row1[0]);
  double cenaz=atof(row1[1]);

  if(kolih1-kolih < 0.)
   {
    sprintf(strsql,"%s (%.8g) !\n",gettext("В исходной накладной количество меньше чем в приходной"),kolih1);
    iceb_printw(strsql,data->buffer,data->view);
    continue;
   } 


  double bb1=kolih*cenaz*(-1);
  bb1=iceb_u_okrug(bb1,0.01);
  it1+=bb1;
  
  double bb=kolih*cenaz*data->rk->pnds/100.*(-1);
  bb=iceb_u_okrug(bb,0.01);
  it2+=bb;     
  if(lnds1 == 0)
   fprintf(ff,"|%02d.%02d.%d|%-*.*s|%-*.*s|%-*s|%10.2f|%10.2f|%10s|\
%10s|%10.2f|%10s|%10s|%11.2f|%11.2f|%11s|%11s|\n",
   data->rk->dd,data->rk->md,data->rk->gd,
   iceb_u_kolbait(20,data->rk->prihina.ravno()),iceb_u_kolbait(20,data->rk->prihina.ravno()),data->rk->prihina.ravno(),
   iceb_u_kolbait(30,naimat.ravno()),iceb_u_kolbait(30,naimat.ravno()),naimat.ravno(),
   iceb_u_kolbait(5,ei.ravno()),ei.ravno(),
   kolih*(-1),cenaz," "," ",
   bb1," "," ",bb,bb," "," ");

  if(lnds1 == 3)
   fprintf(ff,"|%02d.%02d.%d|%-*.*s|%-*.*s|%-*s|%10.2f|%10.2f|%10s|\
%10s|%10s|%10s|%10.2f|%11s|%11s|%11s|%11s|\n",
   data->rk->dd,data->rk->md,data->rk->gd,
   iceb_u_kolbait(20,data->rk->prihina.ravno()),iceb_u_kolbait(20,data->rk->prihina.ravno()),data->rk->prihina.ravno(),
   iceb_u_kolbait(30,naimat.ravno()),iceb_u_kolbait(30,naimat.ravno()),naimat.ravno(),
   iceb_u_kolbait(5,ei.ravno()),ei.ravno(),
   kolih*(-1),cenaz," "," ",
   " "," ",bb1," "," "," "," ");


/*
|    1     |          2         |               3              |  4  |    5     |    6     |    7     |    8     |     9    |    10    |    11    |    12     |     13    |     14    |    15     |
*/     
   
 }
if(lnds1 == 0)
  fprintf(ff,"%*s: %10.2f %21s %11.2f %11.2f\n",
  iceb_u_kolbait(112,gettext("Итого")),gettext("Итого"),it1," ",it2,it2);

if(lnds1 == 3)
  fprintf(ff,"%*s: %10.2f %11s %11s\n",
  iceb_u_kolbait(134,gettext("Итого")),gettext("Итого"),it1," "," ");

fprintf(ff,"\x1B\x2D\x30"); /*Выключить подчеркивание*/

int shet=1; //счетчик строк
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  str.new_plus(row_alx[0]);
  if(shet == 3)
    iceb_u_vstav(&str,data->rk->kto_v_ras.ravno(),160,189,1);
  if(shet == 8)
   {
    sprintf(strsql,"%d.%d.%d",data->rk->dd,data->rk->md,data->rk->gd);
    iceb_u_vstav(&str,strsql,30,41,1);
    iceb_u_vstav(&str,data->rk->nomdok_c.ravno(),48,59,1);
    sprintf(strsql,"%d.%d.%d",di,mi,gi);
    iceb_u_vstav(&str,strsql,89,101,1);
    iceb_u_vstav(&str,data->rk->nomer_nal_nak.ravno(),106,117,1);
   }
  fprintf(ff,"%s",str.ravno());
  shet++;
 }  
  
fclose(ff);
iceb_ustpeh(imaf,3,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);
data->metka_fil=0;
return(FALSE);
}
