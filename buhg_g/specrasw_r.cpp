/*$Id: specrasw_r.c,v 1.20 2013/09/26 09:46:55 sasa Exp $*/
/*11.03.2017	17.05.2005	Белых А.И. 	specrasw_r.c
Расчёт прогноза реализации 
*/
#include <errno.h>
#include "buhg_g.h"
#include "specrasw.h"

class specrasw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class specrasw_rr *rk;
  class spis_oth *oth;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  specrasw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   specrasw_r_key_press(GtkWidget *widget,GdkEventKey *event,class specrasw_r_data *data);
gint specrasw_r1(class specrasw_r_data *data);
void  specrasw_r_v_knopka(GtkWidget *widget,class specrasw_r_data *data);


void rppvo1(int skl1,char *naiskl,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,
int *kst,
FILE *ff);

void rasmasop(iceb_u_spisok *spopr,iceb_u_double *mkm,iceb_u_double *mkm1_1r,iceb_u_double *mkm1_1k,
short tip,FILE *ff,class specrasw_r_data *data);


extern SQL_baza bd;
extern double	okrg1;  /*Округление суммы*/

int specrasw_r(class specrasw_rr *datark, class spis_oth *oth,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str repl;
class specrasw_r_data data;

data.rk=datark;
data.name_window.plus(__FUNCTION__);
data.oth=oth;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт количества материалов на изделие"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(specrasw_r_key_press),&data);

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

repl.plus(gettext("Расчёт количества деталей и материалов на изделие"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(specrasw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)specrasw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  specrasw_r_v_knopka(GtkWidget *widget,class specrasw_r_data *data)
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

gboolean   specrasw_r_key_press(GtkWidget *widget,GdkEventKey *event,class specrasw_r_data *data)
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
/*********************************************/
/*шапка по материалам*/
/**********************************************/
void specras_hm(FILE *ff)
{
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Код д.|    Наименование детали       |Ед.изм.|Код.м.|     Наименование материала   |Ед.изм.|Кол.деталей|Норма спи.|Кол.матер.|\n"));
/*
123456 123456789012345678901234567890 1234567 123456 123456789012345678901234567890 1234567 12345678901 1234567890 1234567890
*/

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------------\n");


}

/*********************************************/
/*шапка свода по материалам*/
/**********************************************/
void specras_hspm(FILE *ff)
{
fprintf(ff,"\
---------------------------------------------------------\n");
fprintf(ff,gettext("\
Код.м.|     Наименование материала   |Ед.изм.|Кол.матер.|\n"));
/*
123456 123456789012345678901234567890 1234567 123456 123456789012345678901234567890 1234567 12345678901 1234567890 1234567890
*/

fprintf(ff,"\
---------------------------------------------------------\n");


}

/******************************************/
/******************************************/

gint specrasw_r1(class specrasw_r_data *data)
{
iceb_clock sssk(data->window);
time_t vremn;
time(&vremn);
char strsql[2048];
SQL_str         row1;
FILE		*ff;
char		imaf[64];
int		kolndet; /*Количество наименований деталей*/
class iceb_u_str naim("");
int		sss;
float		kolstr1;
class iceb_u_str ei("");
int		kodm=0;
class iceb_u_spisok ssm; /*свод списания по материалам "код материала|единица измерения"*/
class iceb_u_double ssm_kol; /*количество материала*/
SQL_str row;
class SQLCURSOR cur;

if(data->rk->kolih.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Не введено количество !"),data->window);
  sssk.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


class iceb_razuz_data m_uz;
m_uz.kod_izd=data->rk->kod_izdel.ravno_atoi();
kolndet=iceb_razuz_kod(&m_uz,data->window);

SQLCURSOR cur1;
sprintf(strsql,"select naimat from Material where kodm=%d",data->rk->kod_izdel.ravno_atoi());
if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
 {
  sprintf(strsql,"specrasw_r-%s %d !",gettext("Не найден код материалла"),data->rk->kod_izdel.ravno_atoi());
  iceb_menu_soob(strsql,data->window);
  sssk.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  
else
 naim.new_plus(row1[0]);

sprintf(strsql,"%s %d %s\n\
%s:%.10g\n",
gettext("Расчёт входимости деталей изделие"),
data->rk->kod_izdel.ravno_atoi(),naim.ravno(),
gettext("Количество изделий"),
data->rk->kolih.ravno_atof());

iceb_printw(strsql,data->buffer,data->view);

char imaf_mat[64];
sprintf(imaf_mat,"specmatr%d.lst",data->rk->kod_izdel.ravno_atoi());
class iceb_fopen film;
if(film.start(imaf_mat,"w",data->window) != 0)
 {
  sssk.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(imaf,"specr%d.lst",data->rk->kod_izdel.ravno_atoi());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sssk.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

short ddd=0;
short mmm=0;
short ggg=0;
iceb_u_poltekdat(&ddd,&mmm,&ggg);

sprintf(strsql,"%s %d %s",
gettext("Расчёт входимости деталей в изделие"),
data->rk->kod_izdel.ravno_atoi(),naim.ravno());

iceb_zagolov(strsql,ff,data->window);

sprintf(strsql,"%s %d %s",
gettext("Расчёт списания материалов на изделие"),
data->rk->kod_izdel.ravno_atoi(),naim.ravno());

iceb_zagolov(strsql,film.ff,data->window);


fprintf(ff,"%s:%.10g\n",gettext("Количество изделий"),data->rk->kolih.ravno_atof());
fprintf(film.ff,"%s:%.10g\n",gettext("Количество изделий"),data->rk->kolih.ravno_atof());

specras_hm(film.ff);



fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
 Koд  |   Наименование   соcтавляющей          |Ед.из|Количество| Цена   |  Сумма   |  Остаток |  Разница |\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n");

double koliho=0.;
double kolihr=0.;
double cena=0.;
double suma=0.;
double isuma=0.;
double ostatok=0.;

kolstr1=0;
kolndet=m_uz.kod_det_ei.kolih();

gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace",NULL);

sprintf(strsql,"\
---------------------------------------------------------------------------------------------------------\n");
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,gettext("\
 Koд  |   Наименование   соcтавляющей          |Ед.из|Количество| Цена   |  Сумма   |  Остаток |  Разница |\n"));
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");


sprintf(strsql,"\
---------------------------------------------------------------------------------------------------------\n");
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
int nomer_v_sp=0;
int kolstr=0;
int metka_pz=0;
for(sss=0; sss < kolndet; sss++)
 {
  iceb_pbar(data->bar,kolndet,++kolstr1);
  
  iceb_u_polen(m_uz.kod_det_ei.ravno(sss),&kodm,1,'|');
  iceb_u_polen(m_uz.kod_det_ei.ravno(sss),&ei,2,'|');

  cena=suma=0.;
  if(m_uz.metka_mu.ravno(sss) == 0)
    sprintf(strsql,"select naimat,ei,cenapr from Material where kodm=%d",kodm);
  if(m_uz.metka_mu.ravno(sss) == 1)
    sprintf(strsql,"select naius,ei,cena from Uslugi where kodus=%d",kodm);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s-%s %d !",__FUNCTION__,gettext("Не найден код записи"),kodm);
    iceb_menu_soob(strsql,data->window);
    naim.new_plus("");
   }  
  else
   {
    naim.new_plus(row1[0]);
    if(row1[2] != NULL)
     cena=atof(row1[2]);
    if(cena != 0.)
     cena=iceb_u_okrug(cena,okrg1);
   }
  suma=koliho=kolihr=0.;
  kolihr=m_uz.kolih_det_ei.ravno(sss)*data->rk->kolih.ravno_atof();

  suma=cena*kolihr;
  if(suma != 0.)
   suma=iceb_u_okrug(suma,okrg1); 
  isuma+=suma;
  ostatok=0.;
  
  if(m_uz.metka_mu.ravno(sss) == 0)
   {
    koliho=ostdok1w(ddd,mmm,ggg,0,kodm);
    ostatok=koliho-kolihr;
   }
    
  sprintf(strsql,"%-6d %-*s %*s %10.10g %8.8g %10.2f %10.10g %10.10g\n",
  kodm,
  iceb_u_kolbait(40,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(5,ei.ravno()),ei.ravno(),
  kolihr,cena,suma,koliho,ostatok);

  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

  fprintf(ff,"%-6d %-*s %*s %10.10g %8.8g %10.2f %10.10g %10.10g\n",
  kodm,
  iceb_u_kolbait(40,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(5,ei.ravno()),ei.ravno(),
  kolihr,cena,suma,koliho,ostatok);
  


    if(m_uz.metka_mu.ravno(sss) == 1) /*для услуг списания материалов не может быть*/
    continue;
    
  /*списание материалов*/
  
  sprintf(strsql,"select km,kol,eim from Musnrm where kd=%d and vs=0",kodm);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
    continue;
   }
  
  fprintf(film.ff,"%6d %-*.*s %*s ",
  kodm,
  iceb_u_kolbait(30,naim.ravno()),
  iceb_u_kolbait(30,naim.ravno()),
  naim.ravno(),
  iceb_u_kolbait(7,ei.ravno()),
  ei.ravno());

  if(kolstr == 0)
   {
    fprintf(film.ff,"%s\n",gettext("В справочнике не найдена норма списания на эту деталь"));
    continue;
   }

  metka_pz=0;  

  while(cur.read_cursor(&row) != 0)
   {
    double norma=atof(row[1]);
    double kolih_mat=norma*kolihr;
    
    class iceb_u_str naim_mat("");
    sprintf(strsql,"select naimat from Material where kodm=%d",atoi(row[0]));
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     naim_mat.new_plus(row1[0]);
 
    if(metka_pz > 0)
     fprintf(film.ff,"%46s","");
     
    fprintf(film.ff,"%6s %-*.*s %*s %11.11g %10.10g %10.10g\n",             
    row[0],
    iceb_u_kolbait(30,naim_mat.ravno()),
    iceb_u_kolbait(30,naim_mat.ravno()),
    naim_mat.ravno(),
    iceb_u_kolbait(7,row[2]),
    row[2],
    kolihr,
    norma,
    kolih_mat);
    
    metka_pz++;

    sprintf(strsql,"%s|%s",row[0],row[2]);
    if((nomer_v_sp=ssm.find(strsql)) < 0)
     ssm.plus(strsql);

    ssm_kol.plus(kolih_mat,nomer_v_sp);


   }

 }
fprintf(film.ff,"\
------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(73,gettext("Итого")),gettext("Итого"),isuma);

sprintf(strsql,"%*s:%10.2f\n",iceb_u_kolbait(73,gettext("Итого")),gettext("Итого"),isuma);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

int kolnuz=m_uz.kod_uz.kolih();
if(kolnuz != 0)
 {
  sprintf(strsql,"\n%s:\n\
----------------------------------------------------------\n",
  gettext("Входимость узлов"));

  iceb_printw(strsql,data->buffer,data->view);

  fprintf(ff,"\n%s:\n\
-------------------------------------------------------------------------\n",
  gettext("Входимость узлов"));  
  kolstr1=0;
  for(sss=0; sss < kolnuz; sss++)
   {
    iceb_pbar(data->bar,kolnuz,++kolstr1);
    for(int metka=0; metka < 2; metka++)
     {
      if(metka == 0)
       sprintf(strsql,"select naimat,ei from Material where kodm=%d",m_uz.kod_uz.ravno(sss));
      if(metka == 1)
       sprintf(strsql,"select naius,ei from Usluge where kodus=%d",m_uz.kod_uz.ravno(sss));
      if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
       {
        if(metka == 1)
         {        
          sprintf(strsql,"%s %d !",gettext("Не найден код записи"),m_uz.kod_uz.ravno(sss));
          iceb_menu_soob(strsql,data->window);
         }
        naim.new_plus("");
        ei.new_plus("");
       }  
      else
       {
        naim.new_plus(row1[0]);
        ei.new_plus(row1[1]);
        break;
       }
     }

    sprintf(strsql,"%-6d %-*s %*s %10.10g\n",
    m_uz.kod_uz.ravno(sss),
    iceb_u_kolbait(40,naim.ravno()),naim.ravno(),
    iceb_u_kolbait(5,ei.ravno()),ei.ravno(),
    m_uz.kolih_uz.ravno(sss)*data->rk->kolih.ravno_atof());
    
    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

    fprintf(ff,"%-6d %-*s %*s %10.10g\n",
    m_uz.kod_uz.ravno(sss),
    iceb_u_kolbait(40,naim.ravno()),naim.ravno(),
    iceb_u_kolbait(5,ei.ravno()),ei.ravno(),
    m_uz.kolih_uz.ravno(sss)*data->rk->kolih.ravno_atof());
    
   }

 }

iceb_podpis(ff,data->window);
fclose(ff);
iceb_podpis(film.ff,data->window);
film.end();

/*распечатываем свод списания материалов*/
char imaf_ssm[64];
sprintf(imaf_ssm,"ssm%d.lst",data->rk->kod_izdel.ravno_atoi());
if(film.start(imaf_ssm,"w",data->window) != 0)
 {
  sssk.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s %d %s",
gettext("Свод списания материалов на изделие"),
data->rk->kod_izdel.ravno_atoi(),naim.ravno());

iceb_zagolov(strsql,film.ff,data->window);

fprintf(film.ff,"%s:%.10g\n",gettext("Количество изделий"),data->rk->kolih.ravno_atof());

specras_hspm(film.ff);

for(int nom=0; nom < ssm.kolih(); nom++)
 {
  iceb_u_polen(ssm.ravno(nom),&kodm,1,'|');
  iceb_u_polen(ssm.ravno(nom),&ei,2,'|');

  sprintf(strsql,"select naimat from Material where kodm=%d",kodm);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   naim.new_plus(row1[0]);
  else
   naim.new_plus("");
    
  
  fprintf(film.ff,"%6d %-*.*s %*s %10.10g\n",
  kodm,
  iceb_u_kolbait(30,naim.ravno()),
  iceb_u_kolbait(30,naim.ravno()),
  naim.ravno(),
  iceb_u_kolbait(7,ei.ravno()),
  ei.ravno(),
  ssm_kol.ravno(nom));

 }
fprintf(film.ff,"\
---------------------------------------------------------\n");

iceb_podpis(film.ff,data->window);
film.end();


data->oth->spis_imaf.plus(imaf);
data->oth->spis_naim.plus(gettext("Расчёт входимости деталей на изделие"));

data->oth->spis_imaf.plus(imaf_mat);
data->oth->spis_naim.plus(gettext("Расчёт списания материалов на изделие"));

data->oth->spis_imaf.plus(imaf_ssm);
data->oth->spis_naim.plus(gettext("Свод списания материалов на изделие"));

for(int nom=0; nom < data->oth->spis_imaf.kolih(); nom++)
 iceb_ustpeh(data->oth->spis_imaf.ravno(nom),3,data->window);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
