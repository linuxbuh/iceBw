/*$Id: zar_snu_r.c,v 1.22 2013/09/26 09:47:00 sasa Exp $*/
/*23.05.2016	30.11.2006	Белых А.И.	zar_snu_r.c
Расчёт свода начислений/удержаний по работникам
*/
#include <math.h>
#include <errno.h>
#include "buhg_g.h"
#include "zar_snu.h"

class zar_snu_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class zar_snu_rek *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_snu_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_snu_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_snu_r_data *data);
gint zar_snu_r1(class zar_snu_r_data *data);
void  zar_snu_r_v_knopka(GtkWidget *widget,class zar_snu_r_data *data);

void svodnu_sort(class iceb_u_int *tabn,int koltb,GtkWidget *wpredok);

void schnu1(short mn,short gn,short mk,short gk,short dia,const char *kp,const char *tabnom,const char *nahis,const char *uder,const char *kategor,
char *imaf2,int kkol,class iceb_u_int *tn,int koltb,class iceb_u_int *nah_ud,int kon,class iceb_u_double *nao,GtkWidget *bar,GtkWidget *view,
GtkTextBuffer *buffer,GtkWidget *wpredok,FILE *kaw,FILE *ff);

extern SQL_baza bd;





extern char	*shetb; /*Бюджетные счета начислений*/
extern char	*shetbu; /*Бюджетные счета удержаний*/

int zar_snu_r(class zar_snu_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zar_snu_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать свод начислений и удержаний по работникам"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_snu_r_key_press),&data);

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

repl.plus(gettext("Распечатать свод начислений и удержаний по работникам"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(), iceb_get_pnk("00",0,wpredok));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_snu_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zar_snu_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_snu_r_v_knopka(GtkWidget *widget,class zar_snu_r_data *data)
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

gboolean   zar_snu_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_snu_r_data *data)
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

gint zar_snu_r1(class zar_snu_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
class iceb_clock sss(data->window);

short mn,gn;
short mk,gk;
iceb_u_rsdat1(&mn,&gn,data->rk->datan.ravno());
iceb_u_rsdat1(&mk,&gk,data->rk->datak.ravno());
if(mk == 0)
 {
  mk=mn;
  gk=gn;
 }




sprintf(strsql,"%s %d.%d %s %d.%d\n",
gettext("Период с"),mn,gn,
gettext("по"),mk,gk);

iceb_printw(strsql,data->buffer,data->view);

/*Определяем количество начислений и удержаний*/

int kon=0,kou=0;
sprintf(strsql,"select kod from Nash");
class SQLCURSOR cur;
if((kon=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kon == 0)
 {
  iceb_menu_soob(gettext("Не введены начисления !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_u_int nah_ud;
SQL_str row;
while(cur.read_cursor(&row) != 0)
  nah_ud.plus(atoi(row[0]),-1);


memset(strsql,'\0',sizeof(strsql));
strcpy(strsql,"select kod from Uder");

if((kou=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kou == 0)
 {
  iceb_menu_soob(gettext("Не введены удержания !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

while(cur.read_cursor(&row) != 0)
  nah_ud.plus(atoi(row[0]),-1);

sprintf(strsql,"%s:%d\n",
gettext("Количество начислений и удержаний"),nah_ud.kolih());
iceb_printw(strsql,data->buffer,data->view);

/*Узнаем количество людей*/
if(gn != gk)
   sprintf(strsql,"select distinct tabn from Zarn where \
(god > %d and god < %d) or \
(god = %d and mes >= %d) or \
(god = %d and mes <= %d) order by tabn asc",
gn,gk,gn,mn,gk,mk);
else
   sprintf(strsql,"select distinct tabn from Zarn where god=%d and \
mes >= %d and mes <= %d order by tabn asc",gn,mn,mk);
int koltb=0;
if((koltb=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(koltb == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
 

//basic_array<int, true> tn(koltb);
class iceb_u_int tn;
tn.make_class(koltb);

int i=0;
while(cur.read_cursor(&row) != 0)
  tn.plus(atoi(row[0]),i++);

if(data->rk->metka_vr == 1)
 svodnu_sort(&tn,koltb,data->window);

sprintf(strsql,"%s:%d\n",gettext("Количество табельных номеров"),koltb);
iceb_printw(strsql,data->buffer,data->view);

class iceb_u_double nao;
nao.make_class((nah_ud.kolih()+3)*koltb);
char imaf[64];
char imaf2[64];
memset(imaf2,'\0',sizeof(imaf2));

sprintf(imaf,"nu%d.lst",getpid());
FILE *kaw;
if((kaw = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf1[64];
sprintf(imaf1,"nuf%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

short dk;
iceb_u_dpm(&dk,&mk,&gk,5);
iceb_u_zagolov(gettext("Свод начислений и удержаний"),1,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ff);

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext(" П/н| Т/н |Фамилия , Имя , Отчество      |Сальдо на нач.|  Начислено   |   Удержано   |Сальдо конечное|\n"));

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");

iceb_u_zagolov(gettext("Свод начислений и удержаний"),1,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),kaw);


if(data->rk->podr.ravno()[0] == '\0')
 fprintf(kaw,"%s\n",gettext("По всем подразделениям"));
else
  fprintf(kaw,"%s:%s\n",gettext("Код подразделения"),data->rk->podr.ravno());
if(data->rk->tabnom.ravno()[0] != '\0')
  fprintf(kaw,"%s:%s\n",gettext("Табельный номер"),data->rk->tabnom.ravno());
if(data->rk->kod_nah.ravno()[0] != '\0')
  fprintf(kaw,"%s:%s\n",gettext("Коды начисления"),data->rk->kod_nah.ravno());
if(data->rk->kod_ud.ravno()[0] != '\0')
  fprintf(kaw,"%s:%s\n",gettext("Код удержания"),data->rk->kod_ud.ravno());
if(data->rk->kod_kat.ravno()[0] != '\0')
  fprintf(kaw,"%s:%s\n",gettext("Код категории"),data->rk->kod_kat.ravno());

int kkol=data->rk->kolih_kol.ravno_atoi();
schnu1(mn,gn,mk,gk,0,data->rk->podr.ravno(),data->rk->tabnom.ravno(),data->rk->kod_nah.ravno(),data->rk->kod_ud.ravno(),
data->rk->kod_kat.ravno(),imaf2,kkol,&tn,koltb,&nah_ud,kon,&nao,data->bar,data->view,data->buffer,data->window,kaw,ff);

iceb_podpis(kaw,data->window);
iceb_podpis(ff,data->window);

fclose(kaw);
fclose(ff);

iceb_ustpeh(imaf,0,data->window);
iceb_ustpeh(imaf1,0,data->window);
iceb_ustpeh(imaf2,0,data->window);

data->rk->imaf.plus(imaf);
data->rk->imaf.plus(imaf1);
data->rk->imaf.plus(imaf2);
data->rk->naimf.plus(gettext("Свод начислений и удержаний"));
data->rk->naimf.plus(gettext("Свод начислений и удержаний (свёрнутый)"));
data->rk->naimf.plus(gettext("Начисленный доход по месяцам"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),0,data->window);


data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/***************/
/*Шапка таблицы*/
/***************/
void sappknu(long kka, //Количество начислений и удержаний
class iceb_u_str *st,
long pn, //Позиция начала
long pk, //Позиция конца
long konu, //Количество людей
class iceb_u_int *nah_ud,
int kon, //количество начислений
class iceb_u_double *nao,
FILE *kaw,
GtkWidget *wpredok)
{
int             i,i1,i2;
class iceb_u_str nai("");
double          br2;
SQL_str         row1;
char		strsql[512];
SQLCURSOR curr;

/*Формируем строку подчеркивания*/
/*Полка над наименованием*/
st->new_plus("------------------------------------------");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
     br2+=nao->ravno(i1);
   }
  if(fabs(br2) > 0.01 )
    st->plus("-------------");
  else
   i2++;
 }
/*Полка над ИТОГО*/
if(i == kka)
  st->plus("-------------");

fprintf(kaw,"\n%s\n",st->ravno());

fprintf(kaw,"П.н.| Т/Н |%-*s",iceb_u_kolbait(30,gettext("Фамилия")),gettext("Фамилия"));

i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
     br2+=nao->ravno(i1);
   }
  if(fabs(br2) < 0.01)
   {
    i2++;
    continue;
   }
  if(i == kon+1)
   {
    fprintf(kaw,"| %-*.*s",iceb_u_kolbait(11,gettext("Итого начис.")),iceb_u_kolbait(11,gettext("Итого начис.")),gettext("Итого начис."));
    continue;
   }
  if(i == kka-1)
   {
    fprintf(kaw,"| %-*.*s",iceb_u_kolbait(11,gettext("Итого удерж.")),iceb_u_kolbait(11,gettext("Итого удерж.")),gettext("Итого удерж."));
    continue;
   }
  if(i != 0)
   {

    if(i <= kon)
     {
      sprintf(strsql,"select naik from Nash where kod=%d",nah_ud->ravno(i-1));
     }
    else
     {
      sprintf(strsql,"select naik from Uder where kod=%d",nah_ud->ravno(i-2));
     }
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      if(i <= kon)
       sprintf(strsql,"%s %d !",gettext("Не найден код начисления"),nah_ud->ravno(i-1));
      else
       sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),nah_ud->ravno(i-2));
      
      iceb_menu_soob(strsql,wpredok);
      
      nai.new_plus("");
     }
    else
      nai.new_plus(row1[0]);

    fprintf(kaw,"| %-*.*s",iceb_u_kolbait(11,nai.ravno()),iceb_u_kolbait(11,nai.ravno()),nai.ravno());
   }
  else
    fprintf(kaw,"| %-*.*s",iceb_u_kolbait(11,gettext("Сальдо нач.")),iceb_u_kolbait(11,gettext("Сальдо нач.")),gettext("Сальдо нач."));

 }
if(i == kka)
  fprintf(kaw,"| %-*.*s|\n",iceb_u_kolbait(11,gettext("Сальдо кон.")),iceb_u_kolbait(11,gettext("Сальдо кон.")),gettext("Сальдо кон."));
else
  fprintf(kaw,"|\n");

fprintf(kaw,"    |     |%-30s"," ");
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
     br2+=nao->ravno(i1);
   }
  if(fabs(br2) > 0.01)
   {
    if(i == kon+1)
     {
      fprintf(kaw,"| %-11s","**********");
      continue;
     }
    if(i == kka-1)
     {
      fprintf(kaw,"| %-11s","**********");
      continue;
     }
    if(i != 0)
     {
      if(i <= kon)
        fprintf(kaw,"| %-11d",nah_ud->ravno(i-1));
      else
        fprintf(kaw,"| %-11d",nah_ud->ravno(i-2));
      
     }
    else
      fprintf(kaw,"| %-11s","**********");
   }
  else
    i2++;
 }

if(i == kka)
  fprintf(kaw,"| %-11.11s|\n"," ");
else
  fprintf(kaw,"|\n");

fprintf(kaw,"%s\n",st->ravno());
/*
printw("Закончили шапку\n");
refresh();
*/
}
/*******************************************/
/*Распечатка шапки для начислений по месяцу*/
/*******************************************/
void rasmesn1(short mn,short mk,short gg,class iceb_u_str *st,FILE *ff)
{
short		i;

/*Полка над наименованием*/
st->new_plus("------------------------------------------");
for(i=mn; i <= mk; i++)
 st->plus("---------");

/*Полка над Итого*/
st->plus("-----------");

fprintf(ff,"%s\n",st->ravno());

fprintf(ff,"П.н.| Т/Н |%-*.*s|",iceb_u_kolbait(30,gettext("Фамилия")),iceb_u_kolbait(30,gettext("Фамилия")),gettext("Фамилия"));
for(i=mn; i <= mk; i++)
 fprintf(ff," %02d.%d|",i,gg);
fprintf(ff,"%*s|\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"));

fprintf(ff,"%s\n",st->ravno());
 

}

/***********************************/
/*Распечатка помесячных начислений*/
/***********************************/
void rasmesn(short mn,short gn,short mk,short gk,short kolm,
class iceb_u_int *mdat,class iceb_u_double *mnah,char *imaf,class iceb_u_int *tn,int koltb,const char *kp,const char *kategor,const char *tabnom,
GtkWidget *wpredok)
{
FILE		*ff;
short		m,g;
short		mnn=0,mkk=0;
short		kol,i,i1,ip=0;
short		pm;
class iceb_u_str st("");
double		itogs,it;
time_t		tmm;
class iceb_u_str fio("");
SQL_str         row1;
char		strsql[512];
SQLCURSOR curr;
time(&tmm);

sprintf(imaf,"rspn%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
iceb_u_startfil(ff);


short dk;

iceb_u_dpm(&dk,&mk,&gk,5);

iceb_u_zagolov(gettext("Свод начислений"),1,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,wpredok),ff);


if(kp[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код подразделения"),kp);

if(tabnom[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),tabnom);
 

if(kategor[0] != '\0')
 fprintf(ff,"%s:%s\n",gettext("Код категории"),kategor);

fprintf(ff,"%s %d\n",gettext("Количество месяцев"),kolm);

pm=0;
for(g=gn; g<=gk ; g++)
 {
   if(g == gn )
     mnn=mn;
   if(gn != gk && g == gk)
    mnn=1;

  if(g == gn && gn == gk)
   mkk=mk;
  if(g == gn && gn != gk)
   mkk=12;
  if(g == gk )
   mkk=mk;
  if(g != gk  && g != gn)
   mkk=12;
  
  rasmesn1(mnn,mkk,g,&st,ff);
  int nomer=0;
  for(kol=0; kol < koltb; kol++)
   {
    sprintf(strsql,"%d",tn->ravno(kol));
    
    if(iceb_u_proverka(tabnom,strsql,0,0) != 0)
      continue;
    /*Проверяем были ли начисления*/
    sprintf(strsql,"select fio,podr,kateg from Kartb where tabn=%d",tn->ravno(kol));
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      sprintf(strsql,"%s %d /%d/%d\n",gettext("Не найден табельный номер"),tn->ravno(kol),kol,koltb);
      iceb_menu_soob(strsql,wpredok);
      fio.new_plus("");
     }
    else
      fio.new_plus(row1[0]);

    if(iceb_u_proverka(kp,row1[1],0,0) != 0)
      continue;
    if(iceb_u_proverka(kategor,row1[2],0,0) != 0)
      continue;

    //Проверяем есть ли начисления   
    m=mnn;
    ip=pm;   
    itogs=0;
    for( ;     ; m++)
     {

      if(g == gk && m > mk)
       {
        break;
       }
      if(m == 13)
       {
        m=1;
        break;
       }
      itogs+=mnah->ravno(kol*kolm+ip);
      if(itogs != 0.)
       break;
      ip++;
     }
    if(itogs == 0.)
     continue;

    fprintf(ff,"%4d|%5d|%-*.*s|",++nomer,tn->ravno(kol),iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno());
   
    m=mnn;
    ip=pm;   
    itogs=0;
    for( ;     ; m++)
     {

      if(g == gk && m > mk)
       {
        fprintf(ff,"%10.2f|\n",itogs);
        break;
       }
      if(m == 13)
       {
        m=1;
        fprintf(ff,"%10.2f|\n",itogs);
        break;
       }
      fprintf(ff,"%8.2f|",mnah->ravno(kol*kolm+ip));
      itogs+=mnah->ravno(kol*kolm+ip);
      ip++;
     }
   
   }

  /*Горизонтальное Итого*/
  fprintf(ff,"%s\n",st.ravno());
  fprintf(ff,"%*s|",iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"));
  it=0.;
  for(i=pm; i <= pm+(mkk-mnn) ;i++)
   {
    itogs=0.;
    for(i1=0; i1 < koltb; i1++)
      itogs+=mnah->ravno(i1*kolm+i);
    fprintf(ff,"%8.2f|",itogs);
    it+=itogs;
   }
  fprintf(ff,"%10.2f|\n%s\n",it,st.ravno());
  pm=ip;
 }

iceb_podpis(ff,wpredok);
fclose(ff);

}
/********/
/*Поиск */
/********/
void schnu1(short mn,short gn,short mk,short gk,short dia,
const char *kp, //Коды подразделений
const char *tabnom, //Табельные номера
const char *nahis, //Коды начислений
const char *uder, //Коды удержаний
const char *kategor, //коды категорий
char *imaf2,
int kkol,
class iceb_u_int *tn,int koltb,
class iceb_u_int *nah_ud,
int kon, //количество начислений
class iceb_u_double *nao,
GtkWidget *bar,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok,
FILE *kaw,
FILE *ff)
{
short		kolm; /*Количество месяцев в периоде*/
class iceb_u_int mdat; /*массив дат*/
class iceb_u_double mnah; /*Массив начислений*/
unsigned short	porn; /*Порядковый номер*/
int             i,i1,i2;
double          br1,br2;
class iceb_u_str st("");
long            skn;
int             kkk=0;
double          sn;
long            ii;
short           m,g;
double		it1,it2,it3,it4;
short		dd,mm,gg;
short		prn,knah;
double		sym;
class iceb_u_str fio("");
int		tbn;
long		kolstr,kolstr2;
float kolstr1=0;
SQL_str         row,row1;
char		strsql[512];
short		metka;
double		saldb;

kolm=iceb_u_period(1,mn,gn,1,mk,gk,1);
if(kolm > 0)
 {
  mnah.make_class(kolm*koltb);
  mdat.make_class(kolm*2);

  m=mn;
  i=0;
  for(g=gn; g<=gk ; g++)
   for( ;     ; m++)
    {
     if(g == gk && m > mk)
       break;
     if(m == 13)
      {
       m=1;
       break;
      }
     mdat.plus(m,i);
     mdat.plus(g,i+1);
     i+=2;
    }

 }


porn=0;
g=gn;
it1=it2=it3=it4=0.;
int kka=nah_ud->kolih();
class SQLCURSOR cur;
class SQLCURSOR cur1;
for(m=mn; ; m++)
 {
  if(m==13)
   {
    m=1;
    g++;
   }
  if(g>gk)
    break;
  if(g == gk && m > mk)
    break;

  sprintf(strsql,"%d.%d%s\n",m,g,gettext("г."));
  iceb_printw(strsql,buffer,view);
  
  sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and mes=%d",g,m);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    break;
   }

  if(kolstr == 0)
    continue;

    
  kolstr1=0;
  while(cur.read_cursor(&row) != 0)
   {
    iceb_pbar(bar,kolstr,++kolstr1);    

    if(iceb_u_proverka(kp,row[1],0,0) != 0)
      continue;
    if(iceb_u_proverka(tabnom,row[0],0,0) != 0)
      continue;
    if(iceb_u_proverka(kategor,row[2],0,0) != 0)
      continue;

    tbn=atol(row[0]);
    skn=0;
    for(i=0 ; i < koltb ; i++)
     if(tn->ravno(i) == tbn)
      {
       skn=i;
       break;
      }

    if(i == koltb)
     {
      sprintf(strsql,gettext("Не нашли табельный номер %d в массиве !"),tbn);
      iceb_menu_soob(strsql,wpredok);
      continue;
     }

    /*Читаем сальдо*/
    if(m == mn && g == gn)
      nao->plus(zarsaldw(1,m,g,tbn,&saldb,wpredok),skn*(kka+3));

    sprintf(strsql,"select datz,prn,knah,suma from Zarp where datz >= '%d-%02d-01' and \
datz <= '%d-%02d-31' and tabn=%d and suma <> 0. order by prn,knah asc",
    g,m,g,m,tbn);
    if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
      continue;
     }

    if(kolstr2 == 0)
     {
      continue;
     }

    while(cur1.read_cursor(&row1) != 0)
     {
      prn=atoi(row1[1]);
      if(prn == 1 && nahis[0] == '-')
         continue;
      if(prn == 1 && nahis[0] != '\0')
       if(iceb_u_proverka(nahis,row1[2],0,0) != 0)
         continue;

      if(prn == 2 && uder[0] == '-')
         continue;
      if(prn == 2 && uder[0] != '\0')
       if(iceb_u_proverka(uder,row1[2],0,0) != 0)
         continue;

      iceb_u_rsdat(&dd,&mm,&gg,row1[0],2);
      knah=atoi(row1[2]);
      sym=atof(row1[3]);     

      if(prn == 1)
       {
	for(i=0 ; i < kon ; i++)
	  if(nah_ud->ravno(i) == knah)
	   {
	    kkk=i+1; /*С учетом колонки сальдо*/
	    break;
	   }
	nao->plus(sym,(skn*(kka+3))+kon+1);
        if(kolm > 0)
         {
          for(i=0 ; i < kolm; i++)
            if(mdat.ravno(i*2) == mm && mdat.ravno(i*2+1) == gg)
             {
              mnah.plus(sym,skn*kolm+i);
              break;
             }
         }
       }
      if(prn == 2)
       {
	for(i=kon; i < kka ; i++)
	  if(nah_ud->ravno(i) == knah)
	   {
	    kkk=i+2; /*С учетом колонки сальдо и итого начислено*/
	    break;
	   }
	nao->plus(sym,(skn*(kka+3))+kka+2);
       }

      nao->plus(sym,(skn*(kka+3))+kkk);
     }

   }
 }

rasmesn(mn,gn,mk,gk,kolm,&mdat,&mnah,imaf2,tn,koltb,kp,kategor,tabnom,wpredok);

/*Распечатываем*/

sprintf(strsql,"%s\n-------------------------\n",
gettext("Распечатываем"));

iceb_printw(strsql,buffer,view);

SQLCURSOR curr;
sn=0.;
for(ii=0 ; ii < kka+3; ii += kkol)
 {
//  printw("ii-%d kka-%d korr-%d kkol=%d\n",ii,kka,korr,kkol);
  /*Проверяем если все равно нулю то печатаем следующюю группу столбиков*/
  i2=0; br2=0.;
  for(i=ii; i<ii+kkol+i2 && i < kka+3;i++)
   {
    metka=0;
    br1=0.;
    for(skn=i;skn< (kka+3)*(koltb-1)+ii+kkol && skn<(kka+3)*koltb; skn+=(kka+3))
     {
      if(fabs(nao->ravno(skn)) > 0.009)
       {
        metka=1;
        break;
       }      
     }
    if(metka == 0)
     {
      i2++;
     }
    else
     {
      br2++;
     }
   }

  if(br2 == 0.)
    continue;   

  
  sappknu(kka+3,&st,ii,ii+kkol,koltb,nah_ud,kon,nao,kaw,wpredok);

  br1=0.;
  for(i=0 ; i<koltb;i++)
   {
    /*Проверяем если нет равных нолю позиций печатаем*/
    br1=0.;
    for(skn=0;skn< kka+3 ; skn++)
     {
      if(fabs(nao->ravno(skn+i*(kka+3))) > 0.009)
       {
        br1++;
        break;
       }
     }

    if(br1 == 0)
      continue;

    sprintf(strsql,"select fio from Kartb where tabn=%d",tn->ravno(i));
    if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
     {
      sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tn->ravno(i));
      iceb_menu_soob(strsql,wpredok);
      fio.new_plus("");
     }
    else
      fio.new_plus(row1[0]);

    porn++;
    fprintf(kaw,"%4d %5d %-*.*s",porn,tn->ravno(i),iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno());

    if(ii == 0)
     {
      fprintf(ff,"%4d %5d %-*.*s",porn,tn->ravno(i),iceb_u_kolbait(30,fio.ravno()),iceb_u_kolbait(30,fio.ravno()),fio.ravno());
      fprintf(ff," %14.2f",nao->ravno(i*(kka+3)));
      fprintf(ff," %14.2f",nao->ravno(i*(kka+3)+kon+1));
      fprintf(ff," %14.2f",nao->ravno(i*(kka+3)+kka+2));
      fprintf(ff," %14.2f\n",nao->ravno(i*(kka+3))+ nao->ravno(i*(kka+3)+kon+1) + nao->ravno(i*(kka+3)+kka+2));
      it1+=nao->ravno(i*(kka+3));
      it2+=nao->ravno(i*(kka+3)+kon+1);
      it3+=nao->ravno(i*(kka+3)+kka+2);
      it4+=nao->ravno(i*(kka+3))+ nao->ravno(i*(kka+3)+kon+1) + nao->ravno(i*(kka+3)+kka+2);
     }
    /*Предыдущая информация*/
    br1=0.;
    if(ii > 0 )
     {
      for(skn=i*(kka+3); skn<i*(kka+3)+ii && skn < (kka+3)*koltb ;skn++)
       {
	/*Пропускаем итого удержано*/
	if(skn == i*(kka+3)+kka+2 )
	  continue;
	/*Пропускаем итого начислено*/
	if(skn == i*(kka+3)+kon+1)
	  continue;
	 br1+=nao->ravno(skn);
       }
     }
    i2=0;
    for(skn=i*(kka+3)+ii; skn<i*(kka+3)+ii+kkol+i2 && skn < i*(kka+3)+(kka+3) ;skn++)
     {

      /*Если колонка нулевая не печатать*/
      metka=0;
      br2=0.;
      for(i1=skn-(i*(kka+3)); i1< (kka+3)*koltb ; i1+=(kka+3))
       {
	 br2+=nao->ravno(i1);
        if(fabs(nao->ravno(i1)) > 0.009)
          metka=1;
       }
      if(metka == 1)
       {
	fprintf(kaw," %12.2f",nao->ravno(skn));
       }
      else
       {
	i2++;
       }
      /*Пропускаем итого удержано*/
      if(skn == i*(kka+3)+kka+2 )
	continue;
      /*Пропускаем итого начислено*/
      if(skn == i*(kka+3)+kon+1)
        continue;

      br1+=nao->ravno(skn);
     }
    if(skn == i*(kka+3)+kka+3)
     {
      fprintf(kaw," %12.2f\n",br1);
     }
    else
      fprintf(kaw,"\n");
   }
  fprintf(kaw,"%s\n",st.ravno());
  fprintf(kaw,"%-*s",iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"));
  porn=0;
  i2=0;
  for(i=ii; i<ii+kkol+i2 && i < (kka+3) ;i++)
   {
    metka=0;
    br1=0.;
    for(skn=i;skn < (kka+3)*(koltb-1)+ii+kkol+i2 && skn< (kka+3)*koltb ; skn+=(kka+3))
     {
      br1+=nao->ravno(skn);
      if(fabs(nao->ravno(skn)) > 0.009)
        metka=1;
     }
    if(metka == 1)
       fprintf(kaw," %12.2f",br1);
    else
      i2++;

    /*Пропускаем итого удержано*/
    if(i == kka+2 )
     {
      continue;
     }
    /*Пропускаем итого начислено*/
    if(i == kon+1)
     {
      continue;
     } 
    sn+=br1;
   }
  if(i == kka+3)
   {
    fprintf(kaw," %12.2f\n",sn);
   }
  else
   {
    fprintf(kaw,"\n");
   }
  fprintf(kaw,"%s\n",st.ravno());

  if(i == kka+3)
   break;
  ii+=i2;
 }

fprintf(ff,"\
-------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%-*s %14.2f %14.2f %14.2f %14.2f",
iceb_u_kolbait(41,gettext("Итого")),gettext("Итого"),it1,it2,it3,it4);

}

/*****************************/
/*Сортировка табельных номеров по фамилии*/
/**********************************/

void svodnu_sort(class iceb_u_int *tabn,int koltb,GtkWidget *wpredok)
{


class iceb_tmptab tabtmp;
const char *imatmptab={"zarsnu"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
tbn int not null,\
fio char(112) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,wpredok) != 0)
 {
  return;
 }  

char strsql[512];
SQL_str row;
SQLCURSOR cur;
for(int i=0 ; i < koltb; i++)
 {
  sprintf(strsql,"select fio from Kartb where tabn=%d",tabn->ravno(i));
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    continue;
   }
   
//  fprintf(ff,"%d|%s|\n",tabn[i],row[0]);

  sprintf(strsql,"insert into %s values (%d,'%s')",
  imatmptab,
  tabn->ravno(i),iceb_u_sqlfiltr(row[0]));  

  iceb_sql_zapis(strsql,1,1,wpredok);    
     
 }
int kolstr=0;
sprintf(strsql,"select * from %s order by fio asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
/***
  iceb_u_polen(stroka,strsql,sizeof(strsql),1,'|');
  tabn[nomer++]=atol(strsql);  
*****/
  tabn->new_plus(atoi(row[0]),nomer++);
  
 }


}
