/*$Id: kasothw_r.c,v 1.49 2014/08/31 06:19:20 sasa Exp $*/
/*18.05.2018	07.08.2015	Белых А.И.	xukroppw_r.c
Расчёт отчёта 
*/
#include "buhg_g.h"
#include "xukropp.h"

class xukroppw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class xukropp_poi *rk;
  class spis_oth *oth;  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  xukroppw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   xukroppw_r_key_press(GtkWidget *widget,GdkEventKey *event,class xukroppw_r_data *data);
gint xukroppw_r1(class xukroppw_r_data *data);
void  xukroppw_r_v_knopka(GtkWidget *widget,class xukroppw_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;

int xukroppw_r(class xukropp_poi *datark,class spis_oth *oth,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class xukroppw_r_data data;

data.rk=datark;
data.oth=oth;
data.name_window.plus(__FUNCTION__);

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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(xukroppw_r_key_press),&data);

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(xukroppw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);

gtk_text_buffer_create_tag(data.buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);

//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)xukroppw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  xukroppw_r_v_knopka(GtkWidget *widget,class xukroppw_r_data *data)
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

gboolean   xukroppw_r_key_press(GtkWidget *widget,GdkEventKey *event,class xukroppw_r_data *data)
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
/************************************************/
/*шапка*/
/**************************************************/
void xukropp_shdok(int noml,int *kolstr,FILE *ff,GtkWidget *wpredok)
{
if(kolstr != NULL)
 *kolstr=*kolstr+3;
 
fprintf(ff,"%60s %s:%d\n","",gettext("Страница"),noml);

fprintf(ff,"\
-----------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Дата доку.|Ном.д.|Код рас.|Колич.|  Сумма   |Ед.из.|Контрагент поставщик\n"));

fprintf(ff,"\
-----------------------------------------------------------------------------------\n");

}

/**********************************/
void xukropp_rp(class xukropp_poi *poi,FILE *ff)
{
if(poi->metka_fop == 1)
 fprintf(ff,"%s\n",gettext("Только физические лица"));
if(poi->kodkr.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код командировочных расходов"),poi->kodkr.ravno()); 
}

/******************************************/
/******************************************/

gint xukroppw_r1(class xukroppw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);
SQL_str row,row1;
SQLCURSOR cur,cur1;
int kolstr=0;

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);

 }

sprintf(strsql,"select datd,nomd,kodr,kolih,cena,ediz,snds,kdrnn from Ukrdok1 where datd >= '%04d-%02d-%02d' and \
datd <= '%04d-%02d-%02d' and kdrnn <> ''",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_dok[64];
sprintf(imaf_dok,"xukropp%d.lst",getpid());

class iceb_fopen fil_dok;
if(fil_dok.start(imaf_dok,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
iceb_zagolov(gettext("Реестр командировочных расходов по поставщикам"),dn,mn,gn,dk,mk,gk,fil_dok.ff,data->window);

xukropp_rp(data->rk,fil_dok.ff);

xukropp_shdok(1,NULL,fil_dok.ff,data->window);

class iceb_u_int sp_kodkr; /*список кодов командировочных расходов*/
class iceb_u_spisok sp_kontr; /*список кодов контрагентов*/

class iceb_u_spisok sp_kontr_kodkr; /*список кодов контрагентов|кодов командировочных расходов*/
class iceb_u_double sp_suma; /*суммы по кодам контрагентов|командировочных расходов*/
int nom_v_sp=0;
double suma=0.;
float kolstr1=0;
double itogo_kolih=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->kodkr.ravno(),row[2],0,0) != 0)
   continue;

  if(data->rk->metka_fop == 1) /*только физические лица*/
   {
    sprintf(strsql,"select regnom from Kontragent where kodkon='%s'",row[7]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      if(row1[0][0] == '\0')
       continue;    
     }
   }

  itogo_kolih+=atof(row[3]);
  suma=atof(row[4])+atof(row[6]);

  if(sp_kodkr.find(row[2]) < 0)
   sp_kodkr.plus(row[2]);

  if(sp_kontr.find_r(row[7]) < 0)
   sp_kontr.plus(row[7]);

  sprintf(strsql,"%s|%s",row[7],row[2]);

  if((nom_v_sp=sp_kontr_kodkr.find_r(strsql)) < 0)
    sp_kontr_kodkr.plus(strsql);  
  sp_suma.plus(suma,nom_v_sp);
  

  sprintf(strsql,"%s %-*s %3s %5.5g %10.2f %-*s %s %s\n",
  iceb_u_datzap(row[0]),
  iceb_u_kolbait(10,row[1]),
  row[1],
  row[2],
  atof(row[3]),
  suma,
  iceb_u_kolbait(6,row[5]),
  row[5],
  row[7],
  iceb_get_pnk(row[7],0,data->window));  
  
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

  fprintf(fil_dok.ff,"%s %-*s %8s %6.6g %10.2f %-*s %-*s %s\n",
  iceb_u_datzap(row[0]),
  iceb_u_kolbait(6,row[1]),
  row[1],
  row[2],
  atof(row[3]),
  suma,
  iceb_u_kolbait(6,row[5]),
  row[5],
  iceb_u_kolbait(6,row[7]),
  row[7],
  iceb_get_pnk(row[7],0,data->window));  


 }
fprintf(fil_dok.ff,"\
-----------------------------------------------------------------------------------\n");
fprintf(fil_dok.ff,"%*s:%6.6g %10.2f\n",
iceb_u_kolbait(26,gettext("Итого")),
gettext("Итого"),
itogo_kolih,sp_suma.suma());

fprintf(fil_dok.ff,"ICEB_LST_END\n");
iceb_podpis(fil_dok.ff,data->window);
fil_dok.end();


/*распечатываем свод*/
char imaf_svod[64];
sprintf(imaf_svod,"xukropps%d.lst",getpid());
if(fil_dok.start(imaf_svod,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_zagolov(gettext("Свод командировочных расходов по поставщикам"),dn,mn,gn,dk,mk,gk,fil_dok.ff,data->window);

xukropp_rp(data->rk,fil_dok.ff);

class iceb_u_str kod("");
class iceb_u_str naim("");

class iceb_u_str liniq("----------------------------------------------------");
for(int nom=0; nom <= sp_kodkr.kolih(); nom++)
 liniq.plus("-----------");

fprintf(fil_dok.ff,"%s\n",liniq.ravno());

fprintf(fil_dok.ff,"%-*.*s|%-*.*s|%-*.*s|",
iceb_u_kolbait(27,gettext("Контрагент поставщик услуг")),
iceb_u_kolbait(27,gettext("Контрагент поставщик услуг")),
gettext("Контрагент поставщик услуг"),
iceb_u_kolbait(12,gettext("Инд.нал.ном.")),
iceb_u_kolbait(12,gettext("Инд.нал.ном.")),
gettext("Инд.нал.ном."),
iceb_u_kolbait(10,gettext("Код ЕГРПОУ")),
iceb_u_kolbait(10,gettext("Код ЕГРПОУ")),
gettext("Код ЕГРПОУ"));

for(int nom=0; nom < sp_kodkr.kolih(); nom++)
 {
  sprintf(strsql,"select naim from Ukrkras where kod=%d",sp_kodkr.ravno(nom));
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   naim.new_plus(row[0]);
  else
   naim.new_plus("");

  sprintf(strsql,"%d %s",sp_kodkr.ravno(nom),naim.ravno());
  
  fprintf(fil_dok.ff,"%-*.*s|",
  iceb_u_kolbait(10,strsql),
  iceb_u_kolbait(10,strsql),
  strsql);
  
 }
fprintf(fil_dok.ff,"%-*.*s|\n",
iceb_u_kolbait(10,gettext("Итого")),
iceb_u_kolbait(10,gettext("Итого")),
gettext("Итого"));

fprintf(fil_dok.ff,"%s\n",liniq.ravno());

double itogo_str=0.;
double itogo_kol[sp_kodkr.kolih()];
class iceb_u_str inn("");
class iceb_u_str egrpou("");
for(int nom_kontr=0; nom_kontr < sp_kontr.kolih(); nom_kontr++)
 {
  kod.new_plus(sp_kontr.ravno(nom_kontr));
  sprintf(strsql,"select naikon,kod,innn from Kontragent where kodkon='%s'",kod.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   {
    naim.new_plus(row[0]);
    egrpou.new_plus(row[1]);
    inn.new_plus(row[2]);
   }
  else
   {
    naim.new_plus("");
    egrpou.new_plus("");
    inn.new_plus("");
   }
    
  fprintf(fil_dok.ff,"%-*s %-*.*s|%-*s|%-*s|",
  iceb_u_kolbait(6,kod.ravno()),
  kod.ravno(),
  iceb_u_kolbait(20,naim.ravno()),
  iceb_u_kolbait(20,naim.ravno()),
  naim.ravno(),
  iceb_u_kolbait(12,inn.ravno()),
  inn.ravno(),
  iceb_u_kolbait(10,egrpou.ravno()),
  egrpou.ravno());

  itogo_str=0.;
  for(int nom_kodkr=0; nom_kodkr < sp_kodkr.kolih(); nom_kodkr++)
   {
    sprintf(strsql,"%s|%d",kod.ravno(),sp_kodkr.ravno(nom_kodkr));
    if((nom_v_sp=sp_kontr_kodkr.find_r(strsql)) < 0)
     {
      fprintf(fil_dok.ff,"%10s|","");
     }
    else
     {
      fprintf(fil_dok.ff,"%10.2f|",sp_suma.ravno(nom_v_sp));
      itogo_str+=sp_suma.ravno(nom_v_sp);
      itogo_kol[nom_kodkr]+=sp_suma.ravno(nom_v_sp);
     }    

   }
  fprintf(fil_dok.ff,"%10.2f|\n",itogo_str);
 }

fprintf(fil_dok.ff,"%s\n",liniq.ravno());

fprintf(fil_dok.ff,"%*.*s|",
iceb_u_kolbait(51,gettext("Итого")),
iceb_u_kolbait(51,gettext("Итого")),
gettext("Итого"));

for(int nom=0; nom < sp_kodkr.kolih(); nom++)
 fprintf(fil_dok.ff,"%10.2f|",itogo_kol[nom]);


fprintf(fil_dok.ff,"%10.2f|\n",sp_suma.suma());

iceb_podpis(fil_dok.ff,data->window);

fil_dok.end();


data->oth->spis_imaf.plus(imaf_dok);
data->oth->spis_naim.plus(gettext("Реестр командировочных расходов по поставщикам"));

iceb_ustpeh(data->oth->spis_imaf.ravno(0),3,&kolstr,data->window);
iceb_rnl(data->oth->spis_imaf.ravno(0),kolstr,"",&xukropp_shdok,data->window);

data->oth->spis_imaf.plus(imaf_svod);
data->oth->spis_naim.plus(gettext("Свод командировочных расходов по поставщикам"));
iceb_ustpeh(data->oth->spis_imaf.ravno(1),3,data->window);





gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
