/*$Id: ukrprik_r.c,v 1.16 2013/09/26 09:46:56 sasa Exp $*/
/*23.05.2016	28.02.2008	Белых А.И.	ukrprik_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "ukrprik.h"

class ukrprik_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class ukrprik_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  ukrprik_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   ukrprik_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrprik_r_data *data);
gint ukrprik_r1(class ukrprik_r_data *data);
void  ukrprik_r_v_knopka(GtkWidget *widget,class ukrprik_r_data *data);


extern SQL_baza bd;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int ukrprik_r(class ukrprik_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class ukrprik_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка приказов на командировку"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(ukrprik_r_key_press),&data);

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

repl.plus(gettext("Распечатка приказов на командировку"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(ukrprik_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)ukrprik_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  ukrprik_r_v_knopka(GtkWidget *widget,class ukrprik_r_data *data)
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

gboolean   ukrprik_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrprik_r_data *data)
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

gint ukrprik_r1(class ukrprik_r_data *data)
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


int	kolstr=0;
char    imaf[64];
FILE	*ff;
class iceb_u_str nompr("");
short   d,m,g;
class iceb_u_str fio("");
int     kolprik=0;
short   dnp,mnp,gnp;
short   dkp,mkp,gkp;
class iceb_u_str fioruk("");
class iceb_u_str kodruk("");
short   metkaruk=0;
class iceb_u_str tabnom("");
class iceb_u_str dolgn("");
class iceb_u_str dolgnruk("");
class iceb_u_str tabnomruk("");

class iceb_u_str imaf_znast("zarnast.alx");

iceb_poldan("Табельный номер руководителя",&tabnomruk,imaf_znast.ravno(),data->window);

  
//Узнаем должность
sprintf(strsql,"select fio,dolg from Kartb where tabn=%d",tabnomruk.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  fioruk.new_plus(row[0]);
  dolgnruk.new_plus(row[1]);
 }


sprintf(strsql,"select kont,pnaz,organ,celk,datn,datk,nompr,datp from Ukrdok \
where datp >= '%d-%d-%d' and datp <= '%d-%d-%d' order by nompr asc",
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
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

kodruk.new_plus(ICEB_ZAR_PKTN);
kodruk.plus(tabnomruk.ravno());

sprintf(imaf,"pkr%d.tmp",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int nomer=0;
float kolstr1=0.;
SQL_str row1;
class SQLCURSOR curr;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_SRAV(kodruk.ravno(),row[0],0) == 0)
   {
    metkaruk++;
    continue;
   }

  if(iceb_u_SRAV(nompr.ravno(),row[6],0) != 0 || (kolstr1 == 1 && row[6][0] == '\0'))
   {
    if(kolprik != 0)
     {
      fprintf(ff,"\n%10s %s___________________%s\n"," ",dolgnruk.ravno(),fioruk.ravno());
      fprintf(ff,"\f");
     }

    fprintf(ff,"%s\n\n",iceb_get_pnk("00",0,data->window));
    fprintf(ff,"%25s%s %s\n"," ",gettext("ПРИКАЗ НОМЕР"),row[6]);

    iceb_u_rsdat(&d,&m,&g,row[7],2);

    fprintf(ff,"%25s%s %02d.%02d.%d%s\n\n"," ",gettext("от"),d,m,g,
    gettext("г."));

    fprintf(ff,"%s\n",gettext("Про командировку"));
    fprintf(ff,"%s:\n",gettext("Откомандировать"));

    nompr.new_plus(row[6]);
    nomer=0;    
    kolprik++;    
   }  

  iceb_u_rsdat(&dnp,&mnp,&gnp,row[4],2);
  iceb_u_rsdat(&dkp,&mkp,&gkp,row[5],2);

  dolgn.new_plus("");
    
  if(iceb_u_SRAV(ICEB_ZAR_PKTN,row[0],1) == 0)
   {
/***************   
    memset(tabnom,'\0',sizeof(tabnom));
    dlinnakont=strlen(row[0]);
    i2=0;
    for(i=dlinnapr; i < dlinnakont; i++)
      tabnom[i2++]=row[0][i];     
***************/
    tabnom.new_plus(iceb_u_adrsimv(iceb_u_strlen(ICEB_ZAR_PKTN),row[0]));
    //Узнаем должность
    sprintf(strsql,"select dolg from Kartb where tabn=%s",tabnom.ravno());
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
      dolgn.new_plus(row1[0]);

   }
  
  fio.new_plus("");
  //Читаем наименование контрагента
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
    fio.new_plus(row1[0]);
    
  fprintf(ff,"\n%d. %s %s\n",++nomer,fio.ravno(),dolgn.ravno());
  fprintf(ff,"%10s %s %s\n"," ",row[1],row[2]);
  fprintf(ff,"%10s %s %s %02d.%02d.%d%s %s %02d.%02d.%d%s\n",
  " ",row[3],
  gettext("с"),dnp,mnp,gnp,gettext("г."),
  gettext("по"),dkp,mkp,gkp,gettext("г."));
  fprintf(ff,"\n%10s%s________________________\n"," ",gettext("с приказом ознакомлен"));
 }

fprintf(ff,"\n%10s %s___________________%s\n"," ",dolgnruk.ravno(),fioruk.ravno());

if(metkaruk != 0)
 {
  short kolprik1=0;
  
  sprintf(strsql,"select kont,pnaz,organ,celk,datn,datk,nompr,datp from Ukrdok \
where datp >= '%d-%d-%d' and datp <= '%d-%d-%d' and kont='%s' order by nompr asc",
  gn,mn,dn,gk,mk,dk,kodruk.ravno());

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  
  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
    if(iceb_u_SRAV(nompr.ravno(),row[6],0) != 0 || (kolstr1 == 1 && row[6][0] == '\0'))
     {
      if(kolprik != 0 || kolprik1 != 0)
        fprintf(ff,"\f");

      if(kolprik1 != 0)
          fprintf(ff,"\n%10s %s___________________%s\n"," ",dolgnruk.ravno(),fioruk.ravno());

      fprintf(ff,"%s\n\n",iceb_get_pnk("00",0,data->window));
      fprintf(ff,"%25s%s %s\n"," ",gettext("ПРИКАЗ НОМЕР"),row[6]);

      iceb_u_rsdat(&d,&m,&g,row[7],2);

      fprintf(ff,"%25s%s %02d.%02d.%d%s\n\n"," ",gettext("от"),d,m,g,
      gettext("г."));

      fprintf(ff,"%s\n",gettext("Про командировку"));
      fprintf(ff,"%s:\n",gettext("Отбываю в командировку"));

      nompr.new_plus(row[6]);
      nomer=0;    
      kolprik1++;    
     }  
    iceb_u_rsdat(&dnp,&mnp,&gnp,row[4],2);
    iceb_u_rsdat(&dkp,&mkp,&gkp,row[5],2);
    
    fio.new_plus("");
    //Читаем наименование контрагента
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",row[0]);
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
      fio.new_plus(row1[0]);
      
    fprintf(ff,"\n%s %s\n",fio.ravno(),dolgnruk.ravno());
    fprintf(ff,"%10s %s %s\n"," ",row[1],row[2]);
    fprintf(ff,"%10s %s %s %02d.%02d.%d%s %s %02d.%02d.%d%s\n",
    " ",row[3],
    gettext("с"),dnp,mnp,gnp,gettext("г."),
    gettext("по"),dkp,mkp,gkp,gettext("г."));
   }
  fprintf(ff,"\n%10s %s___________________%s\n"," ",dolgnruk.ravno(),fioruk.ravno());
   
 }



fclose(ff);

data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Распечатка приказов на командировку"));
iceb_ustpeh(imaf,1,data->window);



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
