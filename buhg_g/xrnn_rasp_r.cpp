/*$Id: xrnn_rasp_r.c,v 1.34 2014/02/28 05:21:01 sasa Exp $*/
/*23.05.2016	05.05.2008	Белых А.И.	xrnn_rasp_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "xrnn_poiw.h"
#include "rnn_d5w.h"

class xrnn_rasp_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class xrnn_poi *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  xrnn_rasp_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   xrnn_rasp_r_key_press(GtkWidget *widget,GdkEventKey *event,class xrnn_rasp_r_data *data);
gint xrnn_rasp_r1(class xrnn_rasp_r_data *data);
void  xrnn_rasp_r_v_knopka(GtkWidget *widget,class xrnn_rasp_r_data *data);

int rasrnn_pw(class xrnn_poi *data,int period_type,FILE *ff,FILE *ff_prot,FILE *ff_xml,FILE *ff_dot,FILE *ff_xml_dot,
FILE *ff_xml_rozkor,
FILE *ff_lst_rozkor,
class rnn_d5 *dod5,
GtkWidget *bar,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);

int rasrnn_vw(class xrnn_poi *data,int period_type,FILE *ff,FILE *ff_prot,FILE *ff_xml,
FILE *ff_dot,FILE *ff_xml_dot,
FILE *ff_xml_rozkor,
FILE *ff_lst_rozkor,
class rnn_d5 *dod5,
//char *imaf_dbf,char *imaf_dbf_d,
GtkWidget *bar,GtkWidget *view,
GtkTextBuffer *buffer,GtkWidget *wpredok);

int rnnrd5w(short dn,short mn,short gn,short dk,short mk,short gk,char *imaf_xml,char *imaf_svod_naim,
char *imaf_xml_dot,char *imaf_svod_dot,double proc_dot,class rnn_d5 *dod5,GtkWidget *wpredok);

extern class iceb_rnfxml_data rek_zag_nn;
extern SQL_baza bd;
class iceb_u_str kat_for_nal_nak;

int xrnn_rasp_r(class xrnn_poi *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class xrnn_rasp_r_data data;
data.rk=datark;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать реестр налоговых накладных"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(xrnn_rasp_r_key_press),&data);

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

repl.plus(gettext("Распечатать реестр налоговых накладных"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(xrnn_rasp_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)xrnn_rasp_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  xrnn_rasp_r_v_knopka(GtkWidget *widget,class xrnn_rasp_r_data *data)
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

gboolean   xrnn_rasp_r_key_press(GtkWidget *widget,GdkEventKey *event,class xrnn_rasp_r_data *data)
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

/***********************************/
/*Шапка xml файла*/
/**************************/

int rasrnn_sap_xml(short dn,short mn,short gn,short dk,short mk,short gk,
char *imaf_xml,
char *imaf_xml_dot,
char *imaf_xml_rozkor,
int *period_type,double proc_dot,
FILE **ff_xml,
FILE **ff_xml_dot,
FILE **ff_xml_rozkor,
GtkWidget *wpredok)
{
int nomer_porcii=1;

if(iceb_openxml(dn,mn,gn,dk,mk,gk,imaf_xml,iceb_getkoddok(dn,mn,gn,2),nomer_porcii,period_type,&rek_zag_nn,ff_xml,wpredok) != 0)  /*Основной реестр*/
   return(1);

iceb_rnn_sap_xml(mn,gn,mk,iceb_getkoddok(dn,mn,gn,2),nomer_porcii,*period_type,&rek_zag_nn,*ff_xml); /*Основной реестр*/

fprintf(*ff_xml," <DECLARBODY>\n");
fprintf(*ff_xml,"   <HZY>%d</HZY>\n",gk);
fprintf(*ff_xml,"   <HZ>1</HZ>\n"); /*Звітний*/
fprintf(*ff_xml,"   <HNP>1</HNP>\n"); /*Номер порції реєстру*/
fprintf(*ff_xml,"   <HZKV></HZKV>\n");
fprintf(*ff_xml,"   <HZM>%d</HZM>\n",mk);
fprintf(*ff_xml,"   <HNAME>%s</HNAME>\n",iceb_u_filtr_xml(rek_zag_nn.naim_kontr.ravno()));
fprintf(*ff_xml,"   <HLOC>%s</HLOC>\n",iceb_u_filtr_xml(rek_zag_nn.adres.ravno()));
//fprintf(*ff_xml,"   <HTIN>%s</HTIN>\n",rek_zag_nn.innn);
fprintf(*ff_xml,"   <HNPDV>%s</HNPDV>\n",rek_zag_nn.innn.ravno());
fprintf(*ff_xml,"   <HNSPDV>%s</HNSPDV>\n",rek_zag_nn.nspnds.ravno());
fprintf(*ff_xml,"   <H01G01D>%02d%02d%04d</H01G01D>\n",dn,mn,gn);
fprintf(*ff_xml,"   <H01G02I>%04d</H01G02I>\n",gk); //архивный год
fprintf(*ff_xml,"   <H02G01D>%02d%02d%04d</H02G01D>\n",dk,mk,gk);
fprintf(*ff_xml,"   <H02G02S>1</H02G02S>\n"); //номер документа
fprintf(*ff_xml,"   <H03G02I>1</H03G02I>\n"); // количество страниц
fprintf(*ff_xml,"   <H03G01S>9</H03G01S>\n"); // Время хранения документа в годах

/*Расчёт корректировки сумм НДС к налоговой декларации с налога на добавленную стоимость додаток 1*/

if(iceb_openxml(dn,mn,gn,dk,mk,gk,imaf_xml_rozkor,iceb_getkoddok(dn,mn,gn,3),1,period_type,&rek_zag_nn,ff_xml_rozkor,wpredok) != 0)  /*Основной реестр*/
   return(1);

iceb_rnn_sap_xml(mn,gn,mk,iceb_getkoddok(dn,mn,gn,3),1,*period_type,&rek_zag_nn,*ff_xml_rozkor); /*Основной реестр*/


fprintf(*ff_xml_rozkor," <DECLARBODY>\n");
fprintf(*ff_xml_rozkor,"   <HZ>1</HZ>\n"); /*Звітний*/
fprintf(*ff_xml_rozkor,"   <HZY>%d</HZY>\n",gk);
fprintf(*ff_xml_rozkor,"   <HZM>%d</HZM>\n",mk);
fprintf(*ff_xml_rozkor,"   <HZYP xsi:nil=\"true\"></HZYP>\n");
fprintf(*ff_xml_rozkor,"   <HNAME>%s</HNAME>\n",iceb_u_filtr_xml(rek_zag_nn.naim_kontr.ravno()));
fprintf(*ff_xml_rozkor,"   <HTINJ>%s</HTINJ>\n",rek_zag_nn.innn.ravno());
fprintf(*ff_xml_rozkor,"   <HTIN>%s</HTIN>\n",rek_zag_nn.kod_edrpuo.ravno());
fprintf(*ff_xml_rozkor,"   <HNSPDV>%s</HNSPDV>\n",rek_zag_nn.nspnds.ravno());
fprintf(*ff_xml_rozkor,"   <HDDGVSD xsi:nil=\"true\"></HDDGVSD>\n");
fprintf(*ff_xml_rozkor,"   <HNDGVSD xsi:nil=\"true\"></HNDGVSD>\n");
//fprintf(*ff_xml_rozkor,"   <HNSPDV>01834282</HNSPDV>

if(proc_dot != 0.)
 {
  /*отличается от основного только порядковым номером документа*/
  
  if(iceb_openxml(dn,mn,gn,dk,mk,gk,imaf_xml_dot,iceb_getkoddok(dn,mn,gn,2),nomer_porcii+1,period_type,&rek_zag_nn,ff_xml_dot,wpredok) != 0) /*Дополнительный*/
     return(1);
  iceb_rnn_sap_xml(mn,gn,mk,iceb_getkoddok(dn,mn,gn,2),nomer_porcii+1,*period_type,&rek_zag_nn,*ff_xml_dot); /*Дополнительный*/
  
  fprintf(*ff_xml_dot," <DECLARBODY>\n");
  fprintf(*ff_xml_dot,"   <HZY>%d</HZY>\n",gk);
  fprintf(*ff_xml_dot,"   <HZ>1</HZ>\n"); /*Звітний*/
  fprintf(*ff_xml_dot,"   <HNP>1</HNP>\n"); /*Номер порції реєстру*/
  fprintf(*ff_xml_dot,"   <HZKV></HZKV>\n");
  fprintf(*ff_xml_dot,"   <HZM>%d</HZM>\n",mk);
  fprintf(*ff_xml_dot,"   <HNAME>%s</HNAME>\n",iceb_u_filtr_xml(rek_zag_nn.naim_kontr.ravno()));
  fprintf(*ff_xml_dot,"   <HLOC>%s</HLOC>\n",iceb_u_filtr_xml(rek_zag_nn.adres.ravno()));
  //fprintf(*ff_xml_dot,"   <HTIN>%s</HTIN>\n",rek_zag_nn.innn);
  fprintf(*ff_xml_dot,"   <HNPDV>%s</HNPDV>\n",rek_zag_nn.innn.ravno());
  fprintf(*ff_xml_dot,"   <HNSPDV>%s</HNSPDV>\n",rek_zag_nn.nspnds.ravno());
  fprintf(*ff_xml_dot,"   <H01G01D>%02d%02d%04d</H01G01D>\n",dn,mn,gn);
  fprintf(*ff_xml_dot,"   <H01G02I>%04d</H01G02I>\n",gk); //архивный год
  fprintf(*ff_xml_dot,"   <H02G01D>%02d%02d%04d</H02G01D>\n",dk,mk,gk);
  fprintf(*ff_xml_dot,"   <H02G02S>1</H02G02S>\n"); //номер документа
  fprintf(*ff_xml_dot,"   <H03G02I>1</H03G02I>\n"); // количество страниц
  fprintf(*ff_xml_dot,"   <H03G01S>9</H03G01S>\n"); // Время хранения документа в годах
 }



class iceb_u_str bros("");
if(iceb_poldan("Код группы возвратная тара",&bros,"matnast.alx",wpredok) == 0)
  rek_zag_nn.kod_gr_voz_tar=bros.ravno_atoi();




return(0);

}


/******************************************/
/******************************************/

gint xrnn_rasp_r1(class xrnn_rasp_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);


short dn,mn,gn;
short dk,mk,gk;
iceb_u_rsdat(&dn,&mn,&gn,data->rk->datan.ravno(),1);
iceb_u_rsdat(&dk,&mk,&gk,data->rk->datak.ravno(),1);

sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(strsql,data->buffer,data->view);

iceb_rnfxml(&rek_zag_nn,data->window); /*чтение настроек для создания xml файлов*/




if(iceb_make_kat(&kat_for_nal_nak,"nal_nak_xml",data->window) != 0)
 {
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
 }

//Чистим каталог от файлов
iceb_delfil(kat_for_nal_nak.ravno(),"xml",data->window);

FILE *ff_xml;
char imaf_xml[56];

FILE *ff_lst_rozkor=NULL;
char imaf_lst_rozkor[56]; /*розрахунок коригування сумм ПДВ*/
sprintf(imaf_lst_rozkor,"rk%d.lst",getpid());

FILE *ff_xml_rozkor=NULL;
char imaf_xml_rozkor[56]; 

/************
  из списка выданных накладных делается Коригування податковых зобов'язань
  из списка полученных накладных делается Коригування податкового кредиту 
*************/

FILE *ff_xml_dot=NULL;
char imaf_xml_dot[56];

memset(imaf_xml_dot,'\0',sizeof(imaf_xml_dot));

int period_type=0;
if(rasrnn_sap_xml(dn,mn,gn,dk,mk,gk,imaf_xml,imaf_xml_dot,imaf_xml_rozkor,&period_type,data->rk->proc_dot.ravno_atof(),
&ff_xml,&ff_xml_dot,&ff_xml_rozkor,data->window) != 0)
 return(1);

if((ff_lst_rozkor=fopen(imaf_lst_rozkor,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_lst_rozkor,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

iceb_zagolov("Розрахунок коригування сум ПДВ до податкової\nдекларації з податку на додану вартість",dn,mn,gn,dk,mk,gk,ff_lst_rozkor,data->window);


FILE *ff;
char imaf[112];
sprintf(imaf,"rnn%d.lst",getpid());

if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

FILE *ff_dot=NULL;
char imaf_dot[112];
memset(imaf_dot,'\0',sizeof(imaf_dot));

if(data->rk->proc_dot.ravno_atof() != 0.)
 {
  sprintf(imaf_dot,"rnnd%d.lst",getpid());

  if((ff_dot=fopen(imaf_dot,"w")) == NULL)
   {
    iceb_er_op_fil(imaf_dot,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

 }
 
FILE *ff_prot;
char imaf_prot[112];
sprintf(imaf_prot,"rnnp%d.lst",getpid());

if((ff_prot=fopen(imaf_prot,"w")) == NULL)
 {
    iceb_er_op_fil(imaf_prot,"",errno,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
 }

class rnn_d5 dod5;


if(iceb_u_sravmydat(data->rk->datan.ravno(),"1.1.2011") < 0)
 {
  if(data->rk->pr_ras == 0 || data->rk->pr_ras == 1)
   {
    fprintf(ff_lst_rozkor,"\nКоригування податкового кредиту\n");

    if(rasrnn_pw(data->rk,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,
    ff_xml_rozkor,
    ff_lst_rozkor,
    &dod5,
    data->bar,data->view,data->buffer,data->window) == 0)

    if(data->rk->pr_ras == 0 )
     {
      fprintf(ff,"\f");
      if(ff_dot != NULL)
        fprintf(ff_dot,"\f");
     }
   }
  if(data->rk->pr_ras == 0 || data->rk->pr_ras == 2)
   {
    fprintf(ff_lst_rozkor,"\nКоригування податкових зобов'язань\n");
    rasrnn_vw(data->rk,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,
    ff_xml_rozkor,
    ff_lst_rozkor,
    &dod5,
    data->bar,data->view,data->buffer,data->window);
   }
 }

if(iceb_u_sravmydat(data->rk->datan.ravno(),"1.1.2011") >= 0)
 {
  if(data->rk->pr_ras == 0 || data->rk->pr_ras == 2)
   {
    fprintf(ff_lst_rozkor,"\nКоригування податкових зобов'язань\n");
    if(rasrnn_vw(data->rk,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,ff_xml_rozkor,ff_lst_rozkor,&dod5,data->bar,data->view,data->buffer,data->window) == 0)
    if(data->rk->pr_ras == 0 )
     {
      fprintf(ff,"\f");
      if(ff_dot != NULL)
        fprintf(ff_dot,"\f");
     }
   }
  if(data->rk->pr_ras == 0 || data->rk->pr_ras == 1)
   {
    fprintf(ff_lst_rozkor,"\nКоригування податкового кредиту\n");

    rasrnn_pw(data->rk,period_type,ff,ff_prot,ff_xml,ff_dot,ff_xml_dot,
    ff_xml_rozkor,
    ff_lst_rozkor,
    &dod5,
    data->bar,data->view,data->buffer,data->window);
   }
 }
iceb_podpis(ff_lst_rozkor,data->window);
fclose(ff_lst_rozkor);

class iceb_fioruk_rk rukov;
class iceb_fioruk_rk glbuh;
iceb_fioruk(1,&rukov,data->window);
iceb_fioruk(2,&glbuh,data->window);

short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

fprintf(ff_xml_rozkor,"   <HFILL>%02d%02d%d</HFILL>\n",dt,mt,gt);
fprintf(ff_xml_rozkor,"   <HBOS>%s</HBOS>\n",rukov.fio.ravno_filtr_xml());
fprintf(ff_xml_rozkor,"   <HBUH>%s</HBUH>\n",glbuh.fio.ravno_filtr_xml());
fprintf(ff_xml_rozkor,"   <HFO xsi:nil=\"true\"></HFO>\n");
fprintf(ff_xml_rozkor," </DECLARBODY>\n");
fprintf(ff_xml_rozkor,"</DECLAR>\n");
fclose(ff_xml_rozkor);


fprintf(ff_xml,"   <HFILL>%02d%02d%d</HFILL>\n",dt,mt,gt);
fprintf(ff_xml,"   <HBOS>%s</HBOS>\n",rukov.fio.ravno_filtr_xml());
fprintf(ff_xml,"   <HKBOS>%s</HKBOS>\n",rukov.inn.ravno());
fprintf(ff_xml,"   <HBUH>%s</HBUH>\n",glbuh.fio.ravno_filtr_xml());
fprintf(ff_xml,"   <HKBUH>%s</HKBUH>\n",glbuh.inn.ravno());
fprintf(ff_xml,"  </DECLARBODY>\n");
fprintf(ff_xml,"</DECLAR>\n");
fclose(ff_xml);
if(ff_xml_dot != NULL)
 {
  fprintf(ff_xml_dot,"   <HFILL>%02d%02d%d</HFILL>\n",dt,mt,gt);
  fprintf(ff_xml_dot,"   <HBOS>%s</HBOS>\n",rukov.fio.ravno_filtr_xml());
  fprintf(ff_xml_dot,"   <HKBOS>%s</HKBOS>\n",rukov.inn.ravno());
  fprintf(ff_xml_dot,"   <HBUH>%s</HBUH>\n",glbuh.fio.ravno_filtr_xml());
  fprintf(ff_xml_dot,"   <HKBUH>%s</HKBUH>\n",glbuh.inn.ravno());
  fprintf(ff_xml_dot,"  </DECLARBODY>\n");
  fprintf(ff_xml_dot,"</DECLAR>\n");
  fclose(ff_xml_dot);
 }
iceb_podpis(ff,data->window);
fclose(ff);

if(ff_dot != NULL)
 {
  iceb_podpis(ff_dot,data->window);
  fclose(ff_dot);

 }
iceb_podpis(ff_prot,data->window);
fclose(ff_prot);

char imaf_dod5_xml[112];
char imaf_svod_naim[64];

char imaf_dod5_xml_dot[112];
char imaf_svod_dot[64];
memset(imaf_dod5_xml_dot,'\0',sizeof(imaf_dod5_xml_dot));
memset(imaf_svod_dot,'\0',sizeof(imaf_svod_dot));

rnnrd5w(dn,mn,gn,dk,mk,gk,imaf_dod5_xml,imaf_svod_naim,imaf_dod5_xml_dot,imaf_svod_dot,data->rk->proc_dot.ravno_atof(),&dod5,data->window);


data->rk->imaf.plus(imaf);
data->rk->naim.plus("Реєстр податкових накладних (Загальний)");

if(imaf_dot[0] != '\0')
 {
  data->rk->imaf.plus(imaf_dot);
  data->rk->naim.plus("Реєстр податкових накладних (Переробного в-ва))");
 }

if(imaf_svod_dot[0] != '\0')
 {
  data->rk->imaf.plus(imaf_svod_dot);
  data->rk->naim.plus("Додаток 2");
 }
data->rk->imaf.plus(imaf_svod_naim);
data->rk->naim.plus("Додаток 5 з найменуванням контрагентів");

data->rk->imaf.plus(imaf_prot);
data->rk->naim.plus("Протокол расчёта");

data->rk->imaf.plus(imaf_lst_rozkor);
data->rk->naim.plus("Додаток 1");


for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);
 
data->rk->imaf.plus(imaf_xml);
data->rk->naim.plus("Реєстр податкових накладних (Загальний) в форматі xml");

data->rk->imaf.plus(imaf_xml_rozkor);
data->rk->naim.plus("Додаток 1 в форматі xml");

if(imaf_xml_dot[0] != '\0')
 {
  data->rk->imaf.plus(imaf_xml_dot);
  data->rk->naim.plus("Реєстр податкових накладних в форматі xml (Переробного в-ва)");
 }

data->rk->imaf.plus(imaf_dod5_xml);
data->rk->naim.plus("Додаток 5 в форматі xml");
if(imaf_dod5_xml_dot[0] != '\0')
 {
  data->rk->imaf.plus(imaf_dod5_xml_dot);
  data->rk->naim.plus("Додаток 2 в форматі xml");
 }



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
