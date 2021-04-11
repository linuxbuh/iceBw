/*$Id: muinv_r.c,v 1.27 2013/11/24 08:25:07 sasa Exp $*/
/*21.12.2017	21.11.2004	Белых А.И.	muinv_r.c
Расчёт инвентаризации в материальном учёте
*/
#include <math.h>
#include <errno.h>
#include "buhg_g.h"
#include "muinv.h"

class muinv_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class muinv_data *rk;
  class spis_oth *oth;
    
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  muinv_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   muinv_r_key_press(GtkWidget *widget,GdkEventKey *event,class muinv_r_data *data);
gint muinv_r1(class muinv_r_data *data);
void  muinv_r_v_knopka(GtkWidget *widget,class muinv_r_data *data);

extern SQL_baza bd;
extern int  kol_strok_na_liste; //Количесво строк на стандартном листе в ориентации портрет


extern double	okrg1;  /*Округление суммы*/
extern double	okrcn;  /*Округление цены*/

int muinv_r(class spis_oth *oth,class muinv_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
int gor=0;
int ver=0;
class muinv_r_data data;

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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать инвентаризационные ведомости"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(muinv_r_key_press),&data);

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

repl.plus(gettext("Распечатать инвентаризационные ведомости"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(muinv_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)muinv_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  muinv_r_v_knopka(GtkWidget *widget,class muinv_r_data *data)
{
//printf("muinv_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   muinv_r_key_press(GtkWidget *widget,GdkEventKey *event,class muinv_r_data *data)
{
// printf("muinv_r_key_press\n");
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
/*печать шапки страницы инвентаризационной ведомости для бюдженых организаций Наказ 572 от 17.06.2015р.*/
/******************************/
void muinv_h_572(int noml,int *kolstr,FILE *ff,GtkWidget *wpredok)
{
 
fprintf(ff,"%135s %s:%d\n","",gettext("Страница"),noml);

int kz=iceb_insfil("invmu572h.alx",ff,wpredok);

if(kolstr != NULL)
 *kolstr=*kolstr+kz+1;

}
/******************/
/*строка с итогами*/
/******************/
void muinv_r_itogo572(char *naim_itog,double kolih,double suma,int metka_fn,FILE *ff)
{
if(metka_fn == 1)
 fprintf(ff,"%*s|%11s|%12s|%12s|%11.11g|%12s|%12.2f|\n",
 iceb_u_kolbait(77,naim_itog),
 naim_itog,
 "",
 "",
 "",
 kolih,
 "",
 suma);
else
 fprintf(ff,"%*s|%11.11g|%12s|%12.2f|%11.11g|%12s|%12.2f|\n",
 iceb_u_kolbait(77,naim_itog),
 naim_itog,
 kolih,
 "",
 suma,
 kolih,
 "",
 suma);

}
/*********************************************************************/
/*Счётчик инвентаризационной ведомости с итогами на каждой странице*/
/********************************************************************/
int muinv_sh_572(int *kolstr,double *itogo,int *kol_na_liste,const char *naim,int metka_fn,FILE *ff,GtkWidget *wpredok)
{
int kol=2;
if(metka_fn == 0)
 kol=1;

*kolstr+=kol;

int kds=0; /*количество дополнительных строк*/
for(int nom=30; nom < iceb_u_strlen(naim);nom+=30)
  kds++;
  

*kolstr+=kds;

if(*kolstr <= kol_strok_na_liste-3)
 return(0);

muinv_r_itogo572(gettext("Итого"),itogo[0],itogo[1],metka_fn,ff);

double kk=*kol_na_liste;

fprintf(ff,"Кількість порядкових номерів:%s\n",iceb_u_nofwords(kk));
fprintf(ff,"Кількість одиниць:%s\n",iceb_u_nofwords(itogo[0]));

fprintf(ff,"\f");

*kolstr=iceb_insfil("invmu572h.alx",ff,wpredok)+kol+kds;

itogo[0]=itogo[1]=0.;
*kol_na_liste=0;

return(1);
}
/****************************************/
/*Распечатка по счетам*/
/*******************************/
void muinv_rps(class iceb_u_spisok *sp_shet,class iceb_u_double *kol_shet,class iceb_u_double *sum_shet,FILE *ff)
{
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str naim_shet("");
char strsql[1024];

for(int nom=0; nom < sp_shet->kolih(); nom++)
 {
  /*узнаём наименование счёта*/
  sprintf(strsql,"select nais from Plansh where ns='%s'",sp_shet->ravno(nom));
  if(iceb_sql_readkey(strsql,&row,&cur,NULL) == 1)
   {
    naim_shet.new_plus(row[0]);
   }
  else
   naim_shet.new_plus("");



  fprintf(ff,"%-*.*s %-*.*s %10.10g %12.2f\n",
  iceb_u_kolbait(10,sp_shet->ravno(nom)),
  iceb_u_kolbait(10,sp_shet->ravno(nom)),
  sp_shet->ravno(nom),
  iceb_u_kolbait(30,naim_shet.ravno()),
  iceb_u_kolbait(30,naim_shet.ravno()),
  naim_shet.ravno(),
  kol_shet->ravno(nom),
  sum_shet->ravno(nom));  
 }
}


/******************************************/
/******************************************/

gint muinv_r1(class muinv_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);


int kolstr=0;
sprintf(strsql,"select sklad,nomk,kodm,ei,shetu,cena,innom,datv,rnd from Kart order by sklad,shetu asc");
SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной карточки !"),data->window);

  gtk_widget_destroy(data->window);
  return(FALSE);
 }


char imaf[64];
FILE *ff;
class iceb_u_str dopstr("");
class iceb_u_str innom("");
class iceb_u_str regnom("");
double cena=0.;
double summa=0.;
double ostatok=0.;
SQL_str row,row1;
int nk=0;
class iceb_u_str ei("");
short  dv,mv,gv;
int kolstriv=0,kollistiv=0;
int nomerstr=0;
class iceb_u_str sklad("");
class iceb_u_str shetu("");
double  kolihsklad=0.;
double  sumasklad=0.;
double  kolihshet=0.;
double  sumashet=0.;
class iceb_u_str matot("");
class iceb_u_str mesqc("");
class iceb_u_str naish("");
class iceb_u_str artikul("");
short mos,dos,gos;
class iceb_u_str naim("");


iceb_u_rsdat(&dos,&mos,&gos,data->rk->datao.ravno(),1);
iceb_mesc(mos,1,&mesqc);


class iceb_tmptab tabtmp;
const char *imatmptab={"muinv"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

//Сортировка предпринята для того чтобы отсортировать и по наименованию

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
skl int not null,\
sh char(24) not null,\
naim char(112) not null,\
ei char(24) not null,\
cena double(15,6) not null,\
innom char(16) not null,\
datv DATE not null,\
rnd char(20) not null,\
articul char(32) not null,\
ost double(16,6) not null,\
INDEX(skl,sh,naim)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  


class ostatok ost;

float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[0],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->kodmat.ravno(),row[2],0,0) != 0)
     continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),row[4],0,0) != 0)
    continue;

  if(data->rk->metkainnom == 1)
   if(row[6][0] == '\0')
     continue;

  naim.new_plus("");
  artikul.new_plus("");
  /*Узнаем наименование материала*/
  sprintf(strsql,"select * from Material where kodm=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s %s",gettext("Не нашли наименование материала по коду"),row[2]);
    iceb_menu_soob(strsql,data->window);
    continue;
   }
  else
   {
    naim.new_plus(row1[2]);
    artikul.new_plus(row1[14]);
   }

  if(iceb_u_proverka(data->rk->grupa.ravno(),row1[1],0,0) != 0)
    continue;


  nk=atoi(row[1]);

  ei.new_plus(row[3]);
  ostkarw(1,1,gos,dos,mos,gos,atoi(row[0]),nk,&ost);

  if(ost.ostg[3] < -0.0000001)
   {

    sprintf(strsql,"%s !!!",gettext("Внимание"));
    repl.new_plus(strsql);
   
    sprintf(strsql,"%s:%s %s:%d",gettext("Склад"),
    row[0],gettext("Карточка"),nk);
    repl.ps_plus(strsql);
    
    repl.ps_plus(gettext("Отрицательный остаток"));
    iceb_menu_soob(&repl,data->window);
   }


  if(fabs(ost.ostg[3]) < 0.0000001 )
    continue;   
  

  sprintf(strsql,"insert into %s values (%d,'%s','%s','%s',%.10g,'%s','%s','%s','%s',%.10g)",
  imatmptab,
  atoi(row[0]),row[4],naim.ravno_filtr(),row[3],atof(row[5]),row[6],row[7],row[8],artikul.ravno(),ost.ostg[3]);

  iceb_sql_zapis(strsql,1,0,data->window);    
 

 }

class iceb_u_str naim00("");
class iceb_u_str edrpou("");

if(iceb_sql_readkey("select naikon,kod from Kontragent where kodkon='00'",&row,&cur,data->window) == 1)
 {
  naim00.plus(row[0]);
  edrpou.plus(row[1]);  
 }


sprintf(strsql,"select * from %s order by skl asc,sh asc,naim asc",imatmptab);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(imaf,"iv%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }    

FILE *ffipsh;
char imafipsh[64];
sprintf(imafipsh,"ivs%d.lst",getpid());


if((ffipsh = fopen(imafipsh,"w")) == NULL)
 {
  iceb_er_op_fil(imafipsh,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

in_d12_startw(ffipsh);

char imaf_572[64];
sprintf(imaf_572,"invmu572_%d.lst",getpid());

class iceb_fopen fil_572;
if(fil_572.start(imaf_572,"w",data->window) != 0)
 return(1);

char imaf_572s[64];
sprintf(imaf_572s,"invmus572_%d.lst",getpid());

class iceb_fopen fil_572s;
if(fil_572s.start(imaf_572s,"w",data->window) != 0)
 return(1);

class iceb_u_spisok sp_shet;
class iceb_u_double sum_shet;
class iceb_u_double kol_shet;

class iceb_u_spisok sp_shet_i;
class iceb_u_double sum_shet_i;
class iceb_u_double kol_shet_i;

class iceb_u_str skladf("");
class iceb_u_str shetuf("");
class iceb_u_str cenaf("");
class iceb_u_str datvf("");
class iceb_u_str posada("");
int nomer_sheta=0;
class iceb_u_str suma_prop("");
int kolstr_572s=0;
double itogo_list[2];
memset(itogo_list,'\0',sizeof(itogo_list));
int kol_na_liste=0;
while(cur.read_cursor(&row) != 0)
 {

  skladf.new_plus(row[0]);
  shetuf.new_plus(row[1]);
  naim.new_plus(row[2]);
  ei.new_plus(row[3]);
  cenaf.new_plus(row[4]);
  innom.new_plus(row[5]);
  datvf.new_plus(row[6]);
  regnom.new_plus(row[7]);
  artikul.new_plus(row[8]);
  ostatok=atof(row[9]);
  
  if(iceb_u_SRAV(sklad.ravno(),skladf.ravno(),0) != 0)
   {
    
    if(sklad.ravno()[0] != '\0')
     {

      sapkk1ivw(&kolstriv,&kollistiv,ff);
      fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      sapkk1ivw(&kolstriv,&kollistiv,ff);
      fprintf(ff,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Итого по счёту")),gettext("Итого по счёту"),
      kolihshet,sumashet);

      sprintf(strsql,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(20,gettext("Итого по счёту")),gettext("Итого по счёту"),
      kolihshet,sumashet);
      
      iceb_printw(strsql,data->buffer,data->view);

      sapkk1ivw(&kolstriv,&kollistiv,ff);
      fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      sapkk1ivw(&kolstriv,&kollistiv,ff);
      fprintf(ff,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Итого по складу")),gettext("Итого по складу"),
      kolihsklad,sumasklad);

      sprintf(strsql,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(20,gettext("Итого по складу")),gettext("Итого по складу"),
      kolihsklad,sumasklad);
      iceb_printw(strsql,data->buffer,data->view);

      koncinvw(mesqc.ravno(),gos,ff);

      fprintf(ff,"\f");
     
      muinv_r_itogo572(gettext("Итого"),itogo_list[0],itogo_list[1],data->rk->metka_fn,fil_572s.ff);
      muinv_r_itogo572(gettext("Общий итог"),kolihsklad,sumasklad,data->rk->metka_fn,fil_572s.ff);
      muinv_r_itogo572(gettext("Итого"),kolihsklad,sumasklad,data->rk->metka_fn,fil_572.ff);


      fprintf(fil_572s.ff,"Кількість порядкових номерів:%s\n",iceb_u_nofwords(kol_na_liste));
      fprintf(fil_572s.ff,"Кількість одиниць:%s\n",iceb_u_nofwords(itogo_list[0]));

            
            
      fprintf(fil_572.ff,"ICEB_LST_END\n");
     
      muinv_rps(&sp_shet,&kol_shet,&sum_shet,fil_572s.ff);
      muinv_rps(&sp_shet,&kol_shet,&sum_shet,fil_572.ff);

      sp_shet.free_class();
      kol_shet.free_class();
      sum_shet.free_class();

      /*записываем реквизиты концовки документа*/
      class iceb_vstavfil_rk rhd;

      rhd.zap(1,1,49,160,iceb_u_nofwords(nomerstr));
      rhd.zap(7,1,80,160,iceb_u_nofwords(kolihsklad));
      iceb_u_preobr(sumasklad,&suma_prop,0);
      rhd.zap(9,1,62,160,suma_prop.ravno());
      rhd.zap(25,1,23,63,posada.ravno());
      rhd.zap(25,1,98,160,matot.ravno());
      iceb_vstavfil(&rhd,"invmu572k.alx",fil_572.ff,data->window);
      iceb_vstavfil(&rhd,"invmu572k.alx",fil_572s.ff,data->window);


      fprintf(fil_572.ff,"\fICEB_LST_START\n");
      fprintf(fil_572s.ff,"\f");
     }

    matot.new_plus("");
    sprintf(strsql,"select fmol,dolg from Sklad where kod=%s",
    skladf.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      repl.new_plus(gettext("Не найдено код склада"));
      repl.plus(" ");
      repl.plus(skladf.ravno());
      repl.plus(" !");
      iceb_menu_soob(&repl,data->window);      
      posada.new_plus("");
      matot.new_plus("");
     }
    else
     {
      matot.new_plus(row1[0]);
      posada.new_plus(row1[1]);
     }
    /*записываем реквизиты шапки документа*/
    class iceb_vstavfil_rk rhd;

    rhd.zap(1,1,0,100,naim00.ravno());
    rhd.zap(4,2,32,50,edrpou.ravno());
    rhd.zap(24,1,2,50,posada.ravno());
    rhd.zap(24,1,104,140,matot.ravno());
   

    /*вставляем заголовок файла*/
    iceb_vstavfil(&rhd,"invmu572z.alx",fil_572.ff,data->window);
    muinv_h_572(1,NULL,fil_572.ff,data->window);

    kolstr_572s=iceb_vstavfil(&rhd,"invmu572z.alx",fil_572s.ff,data->window);
    kolstr_572s+=iceb_insfil("invmu572h.alx",fil_572s.ff,data->window);
    kol_na_liste=0;
     
    kolstriv=0;
    sapvinw(matot.ravno(),mesqc.ravno(),gos,gos,&kolstriv,&kollistiv,ff);

    kolihsklad=sumasklad=0.;
    sklad.new_plus(skladf.ravno());
    shetu.new_plus("");
   }

  if(iceb_u_SRAV(shetu.ravno(),shetuf.ravno(),0) != 0)
   {
    if(shetu.ravno()[0] != '\0')
     {

      sapkk1ivw(&kolstriv,&kollistiv,ff);

      fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      sapkk1ivw(&kolstriv,&kollistiv,ff);
      fprintf(ff,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Итого по счёту")),gettext("Итого по счёту"),
      kolihshet,sumashet);

      sprintf(strsql,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(20,gettext("Итого по счёту")),gettext("Итого по счёту"),
      kolihshet,sumashet);

      iceb_printw(strsql,data->buffer,data->view);

     }
    naish.new_plus("");
    //Читаем наименование счета
    sprintf(strsql,"select nais from Plansh where ns='%s'",shetuf.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
      naish.new_plus(row1[0]);
                  
    sapkk1ivw(&kolstriv,&kollistiv,ff);
    fprintf(ff,"\n");
    sapkk1ivw(&kolstriv,&kollistiv,ff);
    fprintf(ff,"%s: %s %s\n",gettext("Счёт"),shetuf.ravno(),naish.ravno());

    sprintf(strsql,"%s: %s %s\n",gettext("Счёт"),shetuf.ravno(),naish.ravno());
    iceb_printw(strsql,data->buffer,data->view);

    kolihshet=sumashet=0.;
    shetu.new_plus(shetuf.ravno());
   }

  cena=cenaf.ravno_atof();
  cena=iceb_u_okrug(cena,okrcn);
  
  summa=ostatok*cena;
  summa=iceb_u_okrug(summa,okrg1);
  
  kolihsklad+=ostatok;
  sumasklad+=summa;
  kolihshet+=ostatok;
  sumashet+=summa;

  if((nomer_sheta=sp_shet.find(shetu.ravno())) < 0)
   sp_shet.plus(shetu.ravno());

  sum_shet.plus(summa,nomer_sheta);      
  kol_shet.plus(ostatok,nomer_sheta);      

  if((nomer_sheta=sp_shet_i.find(shetu.ravno())) < 0)
   sp_shet_i.plus(shetu.ravno());

  sum_shet_i.plus(summa,nomer_sheta);      
  kol_shet_i.plus(ostatok,nomer_sheta);      
  
  
  iceb_u_rsdat(&dv,&mv,&gv,datvf.ravno(),2);

  dopstr.new_plus("");
  
  if(innom.ravno()[0] != '\0')
   {
    if( regnom.ravno()[0] == '\0')
     {
//      sprintf(dopstr,"%s:%s",gettext("Инвентарный номер"),innom.ravno());
      dopstr.new_plus(gettext("Инвентарный номер"));
      dopstr.plus(":",innom.ravno());
      if(dv != 0)
       {
        memset(strsql,'\0',sizeof(strsql));
        sprintf(strsql," %s:%d.%d.%d%s",
        gettext("Дата ввода в эксплуатацию"),
        dv,mv,gv,gettext("г."));
        dopstr.plus(strsql);
       }
     }
    else
     {
//      sprintf(dopstr,"%s:%s",gettext("Регистрационный номер"),regnom.ravno());
      dopstr.new_plus(gettext("Регистрационный номер"));
      dopstr.plus(":",regnom.ravno());
      memset(strsql,'\0',sizeof(strsql));
      sprintf(strsql," %s:%s",gettext("Серия"),innom.ravno());
      dopstr.plus(strsql);
      if(dv != 0)
       {
        memset(strsql,'\0',sizeof(strsql));
        sprintf(strsql," %s:%d.%d.%d%s",
        gettext("Дата регистрации"),
        dv,mv,gv,gettext("г."));
        dopstr.plus(strsql);
       }
     }
   }
  if(artikul.ravno()[0] != '\0')
   {
    sprintf(strsql," %s:%s",gettext("Артикул"),artikul.ravno());
    dopstr.plus(strsql);
   }

  sapkk1ivw(&kolstriv,&kollistiv,ff);
  
  fprintf(ff,"%3d|%-*.*s|%-*s|%10.10g|%10.10g|%10.2f|%10s|%10s|%10s|%10s|\n",
  ++nomerstr,
  iceb_u_kolbait(40,naim.ravno()),iceb_u_kolbait(40,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(6,ei.ravno()),ei.ravno(),
  cena,ostatok,summa," "," "," "," ");

  if(dopstr.ravno()[0] != '\0')
   {
    sapkk1ivw(&kolstriv,&kollistiv,ff);
    fprintf(ff,"%s\n",dopstr.ravno());
   }
  sapkk1ivw(&kolstriv,&kollistiv,ff);
  fprintf(ff,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

  muinv_sh_572(&kolstr_572s,itogo_list,&kol_na_liste,naim.ravno(),data->rk->metka_fn,fil_572s.ff,data->window); /*считаем по две строки*/
  kol_na_liste++;  
  itogo_list[0]+=ostatok; /*Именно здесь*/
  itogo_list[1]+=summa;

  if(data->rk->metka_fn == 1)
   fprintf(fil_572s.ff,"%6d|%-*.*s|%-*.*s|%-*.*s|%-*.*s|%11s|%12s|%12s|%11.11g|%12.12g|%12.2f|\n",
   nomerstr,
   iceb_u_kolbait(10,shetuf.ravno()),
   iceb_u_kolbait(10,shetuf.ravno()),
   shetuf.ravno(),
   iceb_u_kolbait(30,naim.ravno()),
   iceb_u_kolbait(30,naim.ravno()),
   naim.ravno(),
   iceb_u_kolbait(20,innom.ravno()),
   iceb_u_kolbait(20,innom.ravno()),
   innom.ravno(),  
   iceb_u_kolbait(7,ei.ravno()),
   iceb_u_kolbait(7,ei.ravno()),
   ei.ravno(),
   "",
   "",
   "",
   ostatok,
   cena,
   summa);
  else
   fprintf(fil_572s.ff,"%6d|%-*.*s|%-*.*s|%-*.*s|%-*.*s|%11.11g|%12.12g|%12.2f|%11.11g|%12.12g|%12.2f|\n",
   nomerstr,
   iceb_u_kolbait(10,shetuf.ravno()),
   iceb_u_kolbait(10,shetuf.ravno()),
   shetuf.ravno(),
   iceb_u_kolbait(30,naim.ravno()),
   iceb_u_kolbait(30,naim.ravno()),
   naim.ravno(),
   iceb_u_kolbait(20,innom.ravno()),
   iceb_u_kolbait(20,innom.ravno()),
   innom.ravno(),  
   iceb_u_kolbait(7,ei.ravno()),
   iceb_u_kolbait(7,ei.ravno()),
   ei.ravno(),
   ostatok,
   cena,
   summa,
   ostatok,
   cena,
   summa);
  
  for(int nom=30; nom < iceb_u_strlen(naim.ravno()); nom+=30)
   {
    fprintf(fil_572s.ff,"%6s|%10s|%-*.*s|%20s|%7s|%11s|%12s|%12s|%11s|%12s|%12s|\n",
    "",
    "",
    iceb_u_kolbait(30,iceb_u_adrsimv(nom,naim.ravno())),
    iceb_u_kolbait(30,iceb_u_adrsimv(nom,naim.ravno())),
    iceb_u_adrsimv(nom,naim.ravno()),
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "");
   }    

  if(data->rk->metka_fn == 1)
   { 
    for(int nom=0; nom < 160; nom++)
     fprintf(fil_572s.ff,".");
    fprintf(fil_572s.ff,"\n");  
   }

  if(data->rk->metka_fn == 1)
   fprintf(fil_572.ff,"%6d|%-*.*s|%-*.*s|%-*.*s|%-*.*s|%11s|%12s|%12s|%11.11g|%12.12g|%12.2f|\n",
   nomerstr,
   iceb_u_kolbait(10,shetuf.ravno()),
   iceb_u_kolbait(10,shetuf.ravno()),
   shetuf.ravno(),
   iceb_u_kolbait(30,naim.ravno()),
   iceb_u_kolbait(30,naim.ravno()),
   naim.ravno(),
   iceb_u_kolbait(20,innom.ravno()),
   iceb_u_kolbait(20,innom.ravno()),
   innom.ravno(),  
   iceb_u_kolbait(7,ei.ravno()),
   iceb_u_kolbait(7,ei.ravno()),
   ei.ravno(),
   "",
   "",
   "",
   ostatok,
   cena,
   summa);
  else
   fprintf(fil_572.ff,"%6d|%-*.*s|%-*.*s|%-*.*s|%-*.*s|%11.11g|%12.12g|%12.2f|%11.11g|%12.12g|%12.2f|\n",
   nomerstr,
   iceb_u_kolbait(10,shetuf.ravno()),
   iceb_u_kolbait(10,shetuf.ravno()),
   shetuf.ravno(),
   iceb_u_kolbait(30,naim.ravno()),
   iceb_u_kolbait(30,naim.ravno()),
   naim.ravno(),
   iceb_u_kolbait(20,innom.ravno()),
   iceb_u_kolbait(20,innom.ravno()),
   innom.ravno(),  
   iceb_u_kolbait(7,ei.ravno()),
   iceb_u_kolbait(7,ei.ravno()),
   ei.ravno(),
   ostatok,
   cena,
   summa,
   ostatok,
   cena,
   summa);
  
  for(int nom=30; nom < iceb_u_strlen(naim.ravno()); nom+=30)
   fprintf(fil_572.ff,"%6s|%10s|%-*.*s|%20s|%7s|%11s|%12s|%12s|%11s|%12s|%12s|\n",
   "",
   "",
   iceb_u_kolbait(30,iceb_u_adrsimv(nom,naim.ravno())),
   iceb_u_kolbait(30,iceb_u_adrsimv(nom,naim.ravno())),
   iceb_u_adrsimv(nom,naim.ravno()),
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "");

  if(data->rk->metka_fn == 1)
   {
    for(int nom=0; nom < 160; nom++)
     fprintf(fil_572.ff,".");
    fprintf(fil_572.ff,"\n");  
   }
 }
fprintf(fil_572.ff,"ICEB_LST_END\n");
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%-*s %10.10g %10.2f\n",
iceb_u_kolbait(62,gettext("Итого по счёту")),gettext("Итого по счёту"),
kolihshet,sumashet);

sprintf(strsql,"%-*s %10.10g %10.2f\n",
iceb_u_kolbait(20,gettext("Итого по счёту")),gettext("Итого по счёту"),
kolihshet,sumashet);
iceb_printw(strsql,data->buffer,data->view);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%-*s %10.10g %10.2f\n",
iceb_u_kolbait(62,gettext("Итого по складу")),gettext("Итого по складу"),
kolihsklad,sumasklad);


sprintf(strsql,"%-*s %10.10g %10.2f\n",
iceb_u_kolbait(20,gettext("Итого по складу")),gettext("Итого по складу"),
kolihsklad,sumasklad);
iceb_printw(strsql,data->buffer,data->view);

/*Распечатка формы додаток 12*/
in_d12_itw(&sp_shet_i,&kol_shet_i,&sum_shet_i,ffipsh,data->window);

koncinvw(mesqc.ravno(),gos,ff);



fclose(ff);

muinv_r_itogo572(gettext("Итого"),itogo_list[0],itogo_list[1],data->rk->metka_fn,fil_572s.ff);
muinv_r_itogo572(gettext("Общий итог"),kolihsklad,sumasklad,data->rk->metka_fn,fil_572s.ff);
muinv_r_itogo572(gettext("Итого"),kolihsklad,sumasklad,data->rk->metka_fn,fil_572.ff);

fprintf(fil_572s.ff,"Кількість порядкових номерів:%s\n",iceb_u_nofwords(kol_na_liste));
fprintf(fil_572s.ff,"Кількість одиниць:%s\n",iceb_u_nofwords(itogo_list[0]));
muinv_rps(&sp_shet,&kol_shet,&sum_shet,fil_572s.ff);
muinv_rps(&sp_shet,&kol_shet,&sum_shet,fil_572.ff);


/*записываем реквизиты концовки документа*/
class iceb_vstavfil_rk rhd;

rhd.zap(1,1,49,160,iceb_u_nofwords(nomerstr));
rhd.zap(7,1,80,160,iceb_u_nofwords(kolihsklad));
iceb_u_preobr(sumasklad,&suma_prop,0);
rhd.zap(9,1,62,160,suma_prop.ravno());
rhd.zap(25,1,23,63,posada.ravno());
rhd.zap(25,1,98,160,matot.ravno());
iceb_vstavfil(&rhd,"invmu572k.alx",fil_572.ff,data->window);
iceb_vstavfil(&rhd,"invmu572k.alx",fil_572s.ff,data->window);


iceb_podpis(fil_572.ff,data->window);
fil_572.end();
iceb_podpis(fil_572s.ff,data->window);
fil_572s.end();


iceb_insfil("in_d12_end.alx",ffipsh,data->window);

iceb_podpis(ffipsh,data->window);
fclose(ffipsh);


data->oth->spis_imaf.plus(imaf);
data->oth->spis_naim.new_plus(gettext("Распечатка инвентаризационной ведомости"));

data->oth->spis_imaf.plus(imafipsh);
data->oth->spis_naim.plus(gettext("Дополнение 12"));

data->oth->spis_imaf.plus(imaf_572s);
data->oth->spis_naim.plus(gettext("Инвентаризационная опись бюджетных организаций с итогами"));

for(int nom=0; nom < data->oth->spis_imaf.kolih(); nom++)
 iceb_ustpeh(data->oth->spis_imaf.ravno(nom),3,data->window);

data->oth->spis_imaf.plus(imaf_572);
data->oth->spis_naim.plus(gettext("Инвентаризационная опись бюджетных организаций"));
iceb_ustpeh(imaf_572,3,&kolstr,data->window);
iceb_rnl(imaf_572,kolstr,"",&muinv_h_572,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
