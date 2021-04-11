/*$Id: ukrshkz_r.c,v 1.17 2013/09/26 09:46:56 sasa Exp $*/
/*25.03.2020	28.02.2008	Белых А.И.	ukrshkz_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "ukrshkz.h"

class ukrshkz_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class ukrshkz_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  ukrshkz_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   ukrshkz_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrshkz_r_data *data);
gint ukrshkz_r1(class ukrshkz_r_data *data);
void  ukrshkz_r_v_knopka(GtkWidget *widget,class ukrshkz_r_data *data);


extern SQL_baza bd;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int ukrshkz_r(class ukrshkz_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class ukrshkz_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт ведомости по контрагентам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(ukrshkz_r_key_press),&data);

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

repl.plus(gettext("Расчёт ведомости по контрагентам"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(ukrshkz_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)ukrshkz_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  ukrshkz_r_v_knopka(GtkWidget *widget,class ukrshkz_r_data *data)
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

gboolean   ukrshkz_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrshkz_r_data *data)
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

gint ukrshkz_r1(class ukrshkz_r_data *data)
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
SQLCURSOR cur1;
int kolstr=0;

sprintf(strsql,"select datd,nomd,kont from Ukrdok where datao >= '%04d-%02d-%02d' and \
datao <= '%04d-%02d-%02d'",gn,mn,dn,gk,mk,dk);
if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
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
class iceb_u_spisok sp_shet; //Список счетов

while(cur1.read_cursor(&row1) != 0)
 {
  if(iceb_u_proverka(data->rk->kod_kontr.ravno(),row1[2],2,0) != 0)
   continue;
  sprintf(strsql,"select distinct shet,sn,snds from Ukrdok1 where datd='%s' and nomd='%s'",row1[0],row1[1]);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  while(cur.read_cursor(&row) != 0)
   {
    if(row[1][0] != '\0' && atof(row[2]) != 0.)
     {
  
      if(sp_shet.find(row[1]) < 0)
       sp_shet.plus(row[1]);
     }
    if(sp_shet.find(row[0]) < 0)
     sp_shet.plus(row[0]);
     
   }
 }

sprintf(strsql,"select kod from Ukrkras");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не введены коды затрат"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_u_spisok sp_kod_ras; //Список кодов расхода

while(cur.read_cursor(&row) != 0)
 sp_kod_ras.plus(row[0]);

class iceb_u_double m_kolih; //Массив с количеством
class iceb_u_double m_suma;  //Массив с суммами без НДС
class iceb_u_double m_nds;  //Массив с суммами НДС

m_kolih.make_class(sp_shet.kolih()*sp_kod_ras.kolih());
m_suma.make_class(sp_shet.kolih()*sp_kod_ras.kolih());
m_nds.make_class(sp_shet.kolih()*sp_kod_ras.kolih());

sprintf(strsql,"select datd,nomd,datao,kont from Ukrdok where datao >= '%04d-%02d-%02d' and \
datao <= '%04d-%02d-%02d' order by datao asc",gn,mn,dn,gk,mk,dk);
if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
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

char imaf_prot[64];
FILE *ff_prot;

sprintf(imaf_prot,"skkp%d.lst",getpid());
if((ff_prot = fopen(imaf_prot,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_prot,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

float kolstr1=0;
int nomer_sh;
int nomer_kr;
double kolih=0.;
double suma=0.;
int kolih_kod_ras=sp_kod_ras.kolih();
int kolstr2=0;
class iceb_u_str shet_nds("");
while(cur1.read_cursor(&row1) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->kod_kontr.ravno(),row1[3],2,0) != 0)
   continue;

  sprintf(strsql,"select kodr,shet,kolih,cena,snds,datd,nomd,sn from Ukrdok1 \
where datd='%s' and nomd='%s'",row1[0],row1[1]);

  if((kolstr2=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    return(1);
   }

  if(kolstr2 == 0)
   continue;
  while(cur.read_cursor(&row) != 0)
   {
            
    if(iceb_u_proverka(data->rk->shet.ravno(),row[1],1,0) != 0)
     if(iceb_u_proverka(data->rk->shet.ravno(),row[7],1,0) != 0)
       continue;

    if(iceb_u_proverka(data->rk->kod_zat.ravno(),row[0],0,0) != 0)
      continue;

    if(data->rk->kod_gr_zat.ravno()[0] != '\0')
     {
      SQL_str row2;
      SQLCURSOR cur2;
      //Узнаём код группы
      sprintf(strsql,"select kgr from Ukrkras where kod=%s",row[0]);
      if(iceb_sql_readkey(strsql,&row2,&cur2,data->window) == 1)
        if(iceb_u_proverka(data->rk->kod_gr_zat.ravno(),row2[0],0,0) != 0)
          continue;
     }

    fprintf(ff_prot,"%s %s %s %s %s %s %s %s %f\n",row1[2],row[5],row[6],row[0],row[1],row[2],row[3],row[4],atof(row[3])+atof(row[4]));
     
    nomer_sh=sp_shet.find(row[1]);
    nomer_kr=sp_kod_ras.find(row[0]);
    kolih=atof(row[2]);
    suma=atof(row[3]);

    m_kolih.plus(kolih,kolih_kod_ras*nomer_sh+nomer_kr);
    m_suma.plus(suma,kolih_kod_ras*nomer_sh+nomer_kr);
    
    if((suma=atof(row[4])) == 0.)
     continue;
    
    if(row[7][0] != '\0')
      shet_nds.new_plus(row[7]);
    else 
      shet_nds.new_plus(row[1]);

    nomer_sh=sp_shet.find(shet_nds.ravno());
    

    m_kolih.plus(kolih,kolih_kod_ras*nomer_sh+nomer_kr);
    m_nds.plus(suma,kolih_kod_ras*nomer_sh+nomer_kr);
          
   }
 }

char imaf[64];
FILE *ff;

sprintf(imaf,"skk%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_prot,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_zagolov(gettext("Расчёт ведомости по счетам-кодам затрат"),dn,mn,gn,dk,mk,gk,ff,data->window);

if(data->rk->shet.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Счёт"),data->rk->shet.ravno());
if(data->rk->kod_zat.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код затрат"),data->rk->kod_zat.ravno());
if(data->rk->kod_kontr.ravno()[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код контрагента"),data->rk->kod_kontr.ravno());
 

double is_kolih,is_suma,is_suma_nds;
double suma_nds=0.;
double itogo=0.;
class iceb_u_str naim("");
double itogo_all[3];
itogo_all[0]=itogo_all[1]=itogo_all[2]=0.;

for(int ss=0; ss < sp_shet.kolih(); ss++)
 {
  if(iceb_u_proverka(data->rk->shet.ravno(),sp_shet.ravno(ss),1,0) != 0)
    continue;

  is_kolih=is_suma=is_suma_nds=itogo=0.;
  
  sprintf(strsql,"select nais from Plansh where ns='%s'",sp_shet.ravno(ss));

  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");

  fprintf(ff,"\n%s:%s %s\n",gettext("Счёт"),sp_shet.ravno(ss),naim.ravno()); 
  fprintf(ff,"\
-------------------------------------------------------------------------------\n");
  fprintf(ff,gettext("\
Код| Наименование затрат          |Количество|С-ма б.НДС|С-ма НДС  |  Итого   |\n"));
  fprintf(ff,"\
-------------------------------------------------------------------------------\n");

  for(int kk=0; kk < kolih_kod_ras; kk++)
   {
    if(iceb_u_proverka(data->rk->kod_zat.ravno(),sp_kod_ras.ravno(kk),0,0) != 0)
      continue;
    
    kolih=m_kolih.ravno(kolih_kod_ras*ss+kk);
    suma=m_suma.ravno(kolih_kod_ras*ss+kk);
    suma_nds=m_nds.ravno(kolih_kod_ras*ss+kk);
  
    if(kolih == 0. && suma == 0. && suma_nds == 0.)
     continue;
    
    sprintf(strsql,"select naim from Ukrkras where kod=%s",sp_kod_ras.ravno(kk));

    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     naim.new_plus(row[0]);
    else
     naim.new_plus("");
   
    fprintf(ff,"%-3s %-*.*s %10.10g %10.2f %10.2f %10.2f\n",
    sp_kod_ras.ravno(kk),
    iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
    kolih,suma,suma_nds,suma+suma_nds); 

    is_kolih+=kolih;
    is_suma+=suma; 
    is_suma_nds+=suma_nds; 
    itogo+=suma+suma_nds;
    
    itogo_all[0]+=suma;
    itogo_all[1]+=suma_nds;
    itogo_all[2]+=suma+suma_nds;

   }
  fprintf(ff,"\
-------------------------------------------------------------------------------\n");
  fprintf(ff,"%*s %10.10g %10.2f %10.2f %10.2f\n",
  iceb_u_kolbait(34,gettext("Итого по счёту")),gettext("Итого по счёту"),
  is_kolih,is_suma,is_suma_nds,itogo);
  
 }

fprintf(ff,"\n%s\n\
%-*s:%11.2f\n\
%-*s:%11.2f\n\
%-*s:%11.2f\n",
gettext("Общий итог"),
iceb_u_kolbait(20,gettext("Cумма без НДС")),
gettext("Cумма без НДС"),
itogo_all[0],
iceb_u_kolbait(20,gettext("Cумма НДС")),
gettext("Cумма НДС"),
itogo_all[1],
iceb_u_kolbait(20,gettext("Сумма с НДС")),
gettext("Сумма с НДС"),
itogo_all[2]);

iceb_podpis(ff,data->window);

fclose(ff);
fclose(ff_prot);

data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Расчёт ведомости по счетам-кодам затрат"));
data->rk->imaf.plus(imaf_prot);
data->rk->naim.plus(gettext("Протокол хода расчёта"));

for(int nomer=0; nomer < data->rk->imaf.kolih(); nomer++)
 iceb_ustpeh(data->rk->imaf.ravno(nomer),3,data->window);

sprintf(strsql,"\n%s\n\
%-*s:%11.2f\n\
%-*s:%11.2f\n\
%-*s:%11.2f\n",
gettext("Общий итог"),
iceb_u_kolbait(20,gettext("Cумма без НДС")),
gettext("Cумма без НДС"),
itogo_all[0],
iceb_u_kolbait(20,gettext("Cумма НДС")),
gettext("Cумма НДС"),
itogo_all[1],
iceb_u_kolbait(20,gettext("Сумма с НДС")),
gettext("Сумма с НДС"),
itogo_all[2]);

gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace","foreground","red",NULL);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);


data->voz=0;
return(FALSE);

}
