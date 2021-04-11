/*$Id: zar_pi_r.c,v 1.19 2013/09/26 09:47:00 sasa Exp $*/
/*23.05.2016	29.01.2007	Белых А.И.	zar_pi_r.c
Отчёт по пенсионерам и инвалидам
*/
#include <errno.h>
#include "buhg_g.h"

class zar_pi_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  const char *data_d;
  class iceb_u_spisok *imaf;
  class iceb_u_spisok *naimf;
    
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_pi_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_pi_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_pi_r_data *data);
gint zar_pi_r1(class zar_pi_r_data *data);
void  zar_pi_r_v_knopka(GtkWidget *widget,class zar_pi_r_data *data);

void  zar_sdpf_ps(short mr,short gr,class iceb_u_int *sppi,int *kolih_inv,GtkWidget *wpredok);

extern SQL_baza bd;


int zar_pi_r(const char *data_d,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zar_pi_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);

data.data_d=data_d;
data.imaf=imaf;
data.naimf=naimf;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать отчёт по пенсионерам и инвалидам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_pi_r_key_press),&data);

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

repl.plus(gettext("Распечатать отчёт по пенсионерам и инвалидам"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_pi_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zar_pi_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_pi_r_v_knopka(GtkWidget *widget,class zar_pi_r_data *data)
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

gboolean   zar_pi_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_pi_r_data *data)
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

gint zar_pi_r1(class zar_pi_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);


short mr,gr;
iceb_u_rsdat1(&mr,&gr,data->data_d);
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

FILE *ff;
char imaf[64];
sprintf(imaf,"sdpf%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int kolih_inv=0;
class iceb_u_int sppi;
zar_sdpf_ps(mr,gr,&sppi,&kolih_inv,data->window);

int kolih=sppi.kolih();

fprintf(ff,"\
                               Список працюючих пенсионерів та інвалідів по\n\
                               %s\n\
                                        Станом на %d.%d.%d\n",iceb_get_pnk("00",0,data->window),dt,mt,gt);
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 N  |  Прізвище, ім'я по батькові  |     Домашня адреса           |   ІПН    |Дата наро.|Дата прий.|З/п за %02d.%d\n",
 mr,gr);
/*
1234 123456789012345678901234567890 123456789012345678901234567890 1234567890 1234567890 1234567890
*/
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");
int nomer=0;
class iceb_u_str fio("");
double suma=0.;
short drg,mrg,grg;
short dp,mp,gp;
class iceb_u_str inn("");
SQL_str row;
SQLCURSOR cur;
class iceb_u_str adres("");
int kolstr=0;
double itogo=0.;
float kolstr1=0.;
double itogo_inv_pen=0;
for(int ii=0; ii < kolih; ii++)
 {
  iceb_pbar(data->bar,kolih,++kolstr1);    

  if(ii == 0 && kolih_inv > 0)
   fprintf(ff,"%s\n",gettext("Инвалиды"));

  if(ii == kolih_inv)
   {
    fprintf(ff,"%*s:%10.2f\n",
    iceb_u_kolbait(99,gettext("Итого по инвалидам")),gettext("Итого по инвалидам"),itogo_inv_pen);
    fprintf(ff,"%s\n",gettext("Пенсионеры"));
    itogo_inv_pen=0;
   }  

  sprintf(strsql,"select fio,datn,inn,adres,denrog from Kartb where tabn=%d",sppi.ravno(ii));
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),sppi.ravno(ii));
    iceb_menu_soob(strsql,data->window);
    continue;
   }  

  fio.new_plus(row[0]);
  iceb_u_rsdat(&dp,&mp,&gp,row[1],2);
  inn.new_plus(row[2]);
  adres.new_plus(row[3]);
  iceb_u_rsdat(&drg,&mrg,&grg,row[4],2);
  //Узнаём сумму начисленную
  sprintf(strsql,"select suma from Zarp where datz >= '%04d-%02d-%02d' \
and datz <= '%04d-%02d-%02d' and tabn=%d and prn='1'",
  gr,mr,1,gr,mr,31,sppi.ravno(ii));

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }
  suma=0.;  
  while(cur.read_cursor(&row) != 0)
   {
    suma+=atof(row[0]);    
   }

  sprintf(strsql,"%-5d %-*.*s %8.2f\n",sppi.ravno(ii),iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno(),suma);
  iceb_printw(strsql,data->buffer,data->view);

  if(suma == 0.)
   continue;
  itogo+=suma;
  itogo_inv_pen+=suma;  
  
  fprintf(ff,"%-4d %-*.*s %-*.*s %-10s %02d.%02d.%04d %02d.%02d.%04d %10.2f\n",
  ++nomer,
  iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno(),
  iceb_u_kolbait(30,adres.ravno()),iceb_u_kolbait(30,adres.ravno()),adres.ravno(),
  inn.ravno(),drg,mrg,grg,dp,mp,gp,suma);

  if(iceb_u_strlen(adres.ravno()) > 30)
    fprintf(ff,"%4s %30s %s\n","","",iceb_u_adrsimv(30,adres.ravno()));
  
 }

fprintf(ff,"%*s:%10.2f\n",
 iceb_u_kolbait(99,gettext("Итого по пенсионерам")),gettext("Итого по пенсионерам"),itogo_inv_pen);

class iceb_u_str bros("");
class iceb_u_str imaf_nast("zarnast.alx");

iceb_poldan("Табельный номер руководителя",&bros,imaf_nast.ravno(),data->window);

class iceb_u_str fioruk("");
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,"Не знайдено табельного номера %s керівника !",bros.ravno());
    iceb_menu_soob(strsql,data->window);
   }
  else
   {
    fioruk.new_plus(row[0]);
   }
 } 

iceb_poldan("Табельный номер бухгалтера",&bros,imaf_nast.ravno(),data->window);
class iceb_u_str fiobuh("");

if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,"Не знайдено табельного номера %s головного бухгалтера!",bros.ravno());
    iceb_menu_soob(strsql,data->window);
   }
  else
   {
    fiobuh.new_plus(row[0]);

   }
 }


fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(99,gettext("Итого")),gettext("Итого"),itogo);
fprintf(ff,"\n\
                      %*s__________________%s\n\n",iceb_u_kolbait(30,gettext("Руководитель")),gettext("Руководитель"),fioruk.ravno());
fprintf(ff,"\
                      %*s__________________%s\n\n",iceb_u_kolbait(30,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),fiobuh.ravno());
                         
iceb_podpis(ff,data->window);
fclose(ff);




data->imaf->plus(imaf);
data->naimf->plus(gettext("Список работающих пенсионеров и инвалидов"));

iceb_ustpeh(imaf,1,data->window);

data->kon_ras=0;
data->voz=0;
gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

return(FALSE);

}
/*******************************/
/*получение списка пенсионеров и инвалидов*/
/*******************************************/

void  zar_sdpf_ps(short mr,short gr,class iceb_u_int *sppi,int *kolih_inv,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
*kolih_inv=0;
int metka_s=0;
for(int ii=0; ii < 2; ii++)
 {
  if(ii == 0)
   metka_s=2;
  if(ii == 1)
   metka_s=1;

  sprintf(strsql,"select tn,dnd,dkd,kom from Zargr where kg=%d order by dnd <= %04d-%02d-01",metka_s,gr,mr);
  if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return;
   }

  if(kolstr == 0)
   {
    return;
   }

  
  
  
  int tabn;

  while(cur_alx.read_cursor(&row_alx) != 0)
   {

    tabn=atoi(row_alx[0]);
    if(row_alx[2][0] != '\0')
     {
      short d=0,m=0,g=0;
      iceb_u_rsdat(&d,&m,&g,row_alx[2],2);
      if(iceb_u_sravmydat(1,mr,gr,1,m,g) > 0)
       continue;
     }
    sppi->plus(tabn,-1);

    if(ii == 0)
     *kolih_inv+=1;         
   }
 }



return;
}
