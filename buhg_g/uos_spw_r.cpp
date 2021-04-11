/*$Id: uos_spw_r.c,v 1.15 2013/09/26 09:46:56 sasa Exp $*/
/*23.05.2016	15.01.2008	Белых А.И.	uos_spw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "uos_spw.h"

class uos_spw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class uos_spw_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  int metka_r;  
  uos_spw_r_data()
   {
    metka_r=0;
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uos_spw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uos_spw_r_data *data);
gint uos_spw_r1(class uos_spw_r_data *data);
void  uos_spw_r_v_knopka(GtkWidget *widget,class uos_spw_r_data *data);

void	sapkasord(short dd,short md,short gd,int nomlist,FILE *ffva);
void   hgrkd(FILE *ff);

extern SQL_baza bd;
extern int      kol_strok_na_liste;

int uos_spw_r(int metka_r,class uos_spw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class uos_spw_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);
data.rk=datark;
data.metka_r=metka_r;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

if(metka_r == 1)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт движения основных средств по счетам списания"));
if(metka_r == 0)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт движения основных средств по счетам получения"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(uos_spw_r_key_press),&data);

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
if(metka_r == 1)
  repl.plus(gettext("Расчёт движения основных средств по счетам списания"));
if(metka_r == 0)
  repl.plus(gettext("Расчёт движения основных средств по счетам получения"));

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(uos_spw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)uos_spw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uos_spw_r_v_knopka(GtkWidget *widget,class uos_spw_r_data *data)
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

gboolean   uos_spw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uos_spw_r_data *data)
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

gint uos_spw_r1(class uos_spw_r_data *data)
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

iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window );


sprintf(strsql,"select datd,innom,nomd,podr,kodol,bs,iz,kodop,bsby,izby,shs from Uosdok1 \
where datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and tipz=%d and podt=1 order by shs,datd asc",
gn,mn,dn,gk,mk,dk,data->metka_r+1);

int kolstr=0;
class SQLCURSOR cur1;
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

FILE *ff;
char imaf[56];
sprintf(imaf,"uos_sp%d.lst",getpid());
if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
if(data->metka_r+1 == 1)
 iceb_zagolov(gettext("Движение основных средств по счетам приобретения"),dn,mn,gn,dk,mk,gk,ff,data->window);
if(data->metka_r+1 == 2)
 iceb_zagolov(gettext("Движение основных средств по счетам списания"),dn,mn,gn,dk,mk,gk,ff,data->window);
 
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Инв.номер |      Наименование            |Счёт спис.|Счёт учёта|Бал.с.н.у.|Износ н.у.|Бал.с.б.у.|Износ б.у.|Дата доку.|Номер доку|Подраздел.|Мат.отв.л.|Код оп.|Контраген.|\n"));

/*************
1234567890 123456789012345678901234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567 1234567890 
**********/

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");


SQL_str row1;
float kolstr1=0;
long in_nom;
short d,m,g;
class iceb_u_str naim_uos("");
double bs,iz,bsbu,izbu;
double ibs=0.,iiz=0.,ibsbu=0.,iizbu=0.;
double ibs_shet=0.,iiz_shet=0.,ibsbu_shet=0.,iizbu_shet=0.;
int podr=0;
int kodol=0;
class iceb_u_str podr_char("");
class iceb_u_str kodol_char("");
class iceb_u_str kontr("");
class iceb_u_str shet_sp("");
class iceb_u_str shet_sp_zap("");

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->shet_sp.ravno(),row[10],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->kod_op.ravno(),row[7],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->podr.ravno(),row[3],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->mat_ot.ravno(),row[4],0,0) != 0)
    continue;
  
  sprintf(strsql,"select kontr from Uosdok where datd='%s' and nomd='%s'",row[0],row[2]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   kontr.new_plus(row1[0]);
  else
   kontr.new_plus("");
   
  if(data->rk->kontr.getdlinna() > 1)
   {
    if(iceb_u_proverka(data->rk->kontr.ravno(),kontr.ravno(),0,0) != 0)
      continue;    
   }
  in_nom=atol(row[1]);
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  class poiinpdw_data rekin;
  poiinpdw(in_nom,m,g,&rekin,data->window);

  if(iceb_u_proverka(data->rk->shetu.ravno(),rekin.shetu.ravno(),0,0) != 0)
    continue;
  
  poiinw(atol(row[1]),d,m,g,&podr,&kodol,data->window);
  
  sprintf(strsql,"select naim from Uosin where innom=%s",row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim_uos.new_plus(row1[0]);
  else
   naim_uos.new_plus("");

  podr_char.new_plus(podr);
  sprintf(strsql,"select naik from Uospod where kod=%d",podr);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   podr_char.plus(row1[0]);

  kodol_char.new_plus(kodol);
  sprintf(strsql,"select naik from Uosol where kod=%d",kodol);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   kodol_char.plus(row1[0]);
  
  shet_sp.new_plus(row[10]);
  if(row[10][0] == '\0')
   shet_sp.new_plus("???");  
  
  bs=atof(row[5]);
  iz=atof(row[6]);  
  bsbu=atof(row[8]);
  izbu=atof(row[9]);  
  
  ibs+=bs;
  iiz+=iz;
  ibsbu+=bsbu;
  iizbu+=izbu;

  ibs_shet+=bs;
  iiz_shet+=iz;
  ibsbu_shet+=bsbu;
  iizbu_shet+=izbu;
    
  if(iceb_u_SRAV(shet_sp_zap.ravno(),shet_sp.ravno(),0) != 0)
   {
    if(shet_sp_zap.ravno()[0] != '\0')
     {
      sprintf(strsql,"%s %s",gettext("Итого по счёту"),shet_sp_zap.ravno());
      fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f\n",
      iceb_u_kolbait(63,strsql),strsql,
      ibs_shet,iiz_shet,ibsbu_shet,iizbu_shet);
      
      ibs_shet=0.;
      iiz_shet=0.;
      ibsbu_shet=0.;
      iizbu_shet=0.;
      
     }
    shet_sp_zap.new_plus(shet_sp.ravno());
   }

  fprintf(ff,"%10s %-*.*s %-*s %-*s %10.2f %10.2f %10.2f %10.2f %02d.%02d.%04d %-10s %-10.10s %-10.10s %-*s %s\n",
  row[1],
  iceb_u_kolbait(30,naim_uos.ravno()),iceb_u_kolbait(30,naim_uos.ravno()),naim_uos.ravno(),
  iceb_u_kolbait(10,shet_sp.ravno()),shet_sp.ravno(),
  iceb_u_kolbait(10,rekin.shetu.ravno()),rekin.shetu.ravno(),
  bs,iz,bsbu,izbu,d,m,g,row[2],podr_char.ravno(),kodol_char.ravno(),
  iceb_u_kolbait(7,row[7]),row[7],
  kontr.ravno());

 }

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
sprintf(strsql,"%s %s",gettext("Итого по счёту"),shet_sp_zap.ravno());
fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f\n",iceb_u_kolbait(63,strsql),strsql,ibs_shet,iiz_shet,ibsbu_shet,iizbu_shet);
fprintf(ff,"%*s %10.2f %10.2f %10.2f %10.2f\n",iceb_u_kolbait(63,gettext("Итого")),gettext("Итого"),ibs,iiz,ibsbu,iizbu);

iceb_podpis(ff,data->window);

fclose(ff);

iceb_ustpeh(imaf,0,data->window);

data->rk->imaf.plus(imaf);

if(data->metka_r+1 == 1)
  data->rk->naim.plus(gettext("Расчёт движения основных средств по счетам получения"));
if(data->metka_r+1 == 2)
  data->rk->naim.plus(gettext("Расчёт движения основных средств по счетам списания"));







gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
