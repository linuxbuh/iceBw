/*$Id: buhrpznpw_r.c,v 1.23 2014/06/30 06:35:54 sasa Exp $*/
/*23.05.2016	01.03.2007	Белых А.И.	buhrpznpw_r.c
Расчёт распределения административных затрат на доходы
*/
#include <errno.h>
#include "buhg_g.h"
#include "buhrpznpw.h"

class buhrpznpw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class buhrpznpw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  buhrpznpw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   buhrpznpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhrpznpw_r_data *data);
gint buhrpznpw_r1(class buhrpznpw_r_data *data);
void  buhrpznpw_r_v_knopka(GtkWidget *widget,class buhrpznpw_r_data *data);
int buhrpznpw_r_nast(class iceb_u_str *sp_shet_d,class iceb_u_str *sp_shet_z,class iceb_u_str *naim_ras,GtkWidget *wpredok);

extern SQL_baza bd;

int buhrpznpw_r(class buhrpznpw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class buhrpznpw_r_data data;
int gor=0;
int ver=0;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт распределения административних затрат на доходы"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(buhrpznpw_r_key_press),&data);

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

repl.plus(gettext("Расчёт распределения административних затрат на доходы"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,wpredok));
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
//gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_ALWAYS,GTK_POLICY_ALWAYS);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(buhrpznpw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)buhrpznpw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  buhrpznpw_r_v_knopka(GtkWidget *widget,class buhrpznpw_r_data *data)
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

gboolean   buhrpznpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhrpznpw_r_data *data)
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


/**********************************/
/*Проверка счёта*/
/*********************/
int buhrznp_prov(const char *sp_shet,const char *shet_prov,const char *shet_provk,class iceb_u_str *shet_vib)
{
class iceb_u_str shet_shetk("");
class iceb_u_str shet("");
class iceb_u_str shetk("");

int kol=iceb_u_pole2(sp_shet,',');
for(int ii=0; ii < kol; ii++)
 {
  
  iceb_u_polen(sp_shet,&shet_shetk,ii+1,',');
  if(shet_shetk.ravno()[0] == '\0')
   continue;
 
  shetk.new_plus("");
 
  if(iceb_u_polen(shet_shetk.ravno(),&shet,1,'>') != 0)
   shet.new_plus(shet_shetk.ravno());
  else
   iceb_u_polen(shet_shetk.ravno(),&shetk,2,'>');

  if(iceb_u_proverka(shet.ravno(),shet_prov,0,0) == 0)
   {
    if(shetk.ravno()[0] != '\0')
     {
      if(iceb_u_proverka(shetk.ravno(),shet_provk,0,0) == 0)
       {
        shet_vib->new_plus(shet_shetk.ravno());
        return(0);
       }
     }
    else
     {
      shet_vib->new_plus(shet_shetk.ravno());
      return(0);
     }
   }      
 }
return(1);
}

/******************************************/
/******************************************/

gint buhrpznpw_r1(class buhrpznpw_r_data *data)
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



class iceb_u_str doh_sheta;
class iceb_u_str zat_sheta;
class iceb_u_str naim_ras;

buhrpznpw_r_nast(&doh_sheta,&zat_sheta,&naim_ras,data->window);


if(zat_sheta.getdlinna() <= 1)
 {
  iceb_menu_soob("Не введены \"Затратные счета\" !",data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(doh_sheta.getdlinna() <= 1)
 {
  iceb_menu_soob("Не введены \"Доходные счета\" !",data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int kolstr;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select sh,shk,deb from Prov where datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and val=0 and deb <> 0. order by sh asc",gn,mn,dn,gk,mk,dk);

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
class iceb_u_spisok sp_doh;
class iceb_u_spisok sp_zat;
class iceb_u_double sum_doh;
class iceb_u_double sum_zat;
int nomer=0;
double suma=0.;
class iceb_u_str shet_vib("");

while(cur.read_cursor(&row) != 0)
 {
  suma=atof(row[2]);
  shet_vib.new_plus("");
    
  if(buhrznp_prov(doh_sheta.ravno(),row[0],row[1],&shet_vib) == 0)
   {
    if((nomer=sp_doh.find(shet_vib.ravno())) < 0)
     sp_doh.plus(shet_vib.ravno());
    sum_doh.plus(suma,nomer);
   
    continue;
   }
  
  if(buhrznp_prov(zat_sheta.ravno(),row[0],row[1],&shet_vib) == 0)
   {
    if((nomer=sp_zat.find(shet_vib.ravno())) < 0)
     sp_zat.plus(shet_vib.ravno());
    sum_zat.plus(suma,nomer);
   
    continue;
   }
 } 


char imaf[64];
sprintf(imaf,"rznp%d.lst",getpid());
FILE *ff;
if((ff=fopen(imaf,"w")) == NULL) 
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(naim_ras.getdlinna() <= 1)
 iceb_zagolov(gettext("Расчёт распределения административных затрат на доходы"),dn,mn,gn,dk,mk,gk,ff,data->window);
else
 iceb_zagolov(naim_ras.ravno(),dn,mn,gn,dk,mk,gk,ff,data->window);

fprintf(ff,"%s:%s\n",gettext("Доходные счета"),doh_sheta.ravno());
fprintf(ff,"%s:%s\n",gettext("Затратные счета"),zat_sheta.ravno());

double itogo_doh=sum_doh.suma();

//создаём массив для коэффициентов

double koef[sp_zat.kolih()];
fprintf(ff,"\n%s\n",gettext("Расчёт коэффициентов"));

for(int ii=0; ii < sp_zat.kolih(); ii++)
 {
  if(itogo_doh != 0.)
    koef[ii]=sum_zat.ravno(ii)/itogo_doh;
  else
    koef[ii]=0.;
  
  fprintf(ff,"%-*s %.2f/%.2f=%f\n",
  iceb_u_kolbait(7,sp_zat.ravno(ii)),sp_zat.ravno(ii),sum_zat.ravno(ii),itogo_doh,koef[ii]);
 }

fprintf(ff,"\n");

class iceb_u_str liniq("----------------------");
for(int ii=0; ii < sp_zat.kolih()+1; ii++)
 liniq.plus("-----------");

fprintf(ff,"%s\n",liniq.ravno());

 
fprintf(ff,"%s",gettext("Счёт дохо.|Сумма дох.|"));
for(int ii=0; ii < sp_zat.kolih(); ii++)
 fprintf(ff,"%-*.*s|",iceb_u_kolbait(10,sp_zat.ravno(ii)),iceb_u_kolbait(10,sp_zat.ravno(ii)),sp_zat.ravno(ii));
fprintf(ff,"%-*.*s|\n",iceb_u_kolbait(10,gettext("Итого")),iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",liniq.ravno());
double itogo=0.;
for(int ii=0; ii < sp_doh.kolih() ; ii++)
 {
  fprintf(ff,"%-*.*s|%10.2f|",
  iceb_u_kolbait(10,sp_doh.ravno(ii)),iceb_u_kolbait(10,sp_doh.ravno(ii)),sp_doh.ravno(ii),sum_doh.ravno(ii));
  itogo=0.;
  for(int kk=0; kk < sp_zat.kolih() ; kk++)
   {
    suma=sum_doh.ravno(ii)*koef[kk];
    suma=iceb_u_okrug(suma,0.01);
    fprintf(ff,"%10.2f|",suma);
    itogo+=suma;
   }
  fprintf(ff,"%10.2f|\n",itogo);
 }

fprintf(ff,"%s\n",liniq.ravno());

fprintf(ff,"%*.*s|%10.2f|",
iceb_u_kolbait(10,gettext("Итого")),
iceb_u_kolbait(10,gettext("Итого")),
gettext("Итого"),

itogo_doh);
//Вычисляем итоги по колонкам
double itogo_ob=0.;

for(int ii=0; ii < sp_zat.kolih(); ii++)
 {
  itogo=0.;
  for(int kk=0; kk < sp_doh.kolih(); kk++)
   {
    suma=sum_doh.ravno(kk)*koef[ii];
    suma=iceb_u_okrug(suma,0.01);
    itogo+=suma;
   } 
  fprintf(ff,"%10.2f|",itogo);
  itogo_ob+=itogo;
 }
fprintf(ff,"%10.2f|\n",itogo_ob);

iceb_podpis(ff,data->window);
fclose(ff);

iceb_ustpeh(imaf,0,data->window);

data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Расчёт распределения административних затрат на доходы"));



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}


/*******************************/
/*Чтение файла настроек*/
/****************************/
int buhrpznpw_r_nast(class iceb_u_str *sp_shet_d,
class iceb_u_str *sp_shet_z,
class iceb_u_str *naim_ras,
GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
//char stroka1[10240];
class iceb_u_str stroka1("");
sprintf(strsql,"select str from Alx where fil='buhrznp.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"buhrznp.alx");
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;


  if(iceb_u_polen(row_alx[0],&stroka1,2,'|') != 0)
   continue;
 
  if(iceb_u_SRAV("Доходные счета",row_alx[0],1) == 0)
   {
    iceb_fplus(stroka1.ravno(),sp_shet_d,&cur_alx);
    continue;
   }
  if(iceb_u_SRAV("Затратные счета",row_alx[0],1) == 0)
   {
    iceb_fplus(stroka1.ravno(),sp_shet_z,&cur_alx);
    continue;
   }

  if(iceb_u_SRAV("Наименование расчёта",row_alx[0],1) == 0)
   {
    naim_ras->new_plus(stroka1.ravno());
    continue;
   }

 }
return(0); 

}
