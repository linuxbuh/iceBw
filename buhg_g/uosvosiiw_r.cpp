/*$Id: uosvosiiw_r.c,v 1.27 2013/09/26 09:46:57 sasa Exp $*/
/*19.06.2019	07.01.2008	Белых А.И.	uosvosiiw_r.c
Расчёт отчёта 
*/
#include <math.h>
#include <errno.h>
#include "buhg_g.h"
#include "uosvosiiw.h"
#define         KOLSTSH  6  /*Количество строк в шапке*/
#define STR1 8
#define STR2 10
#define STR3 11


class uosvosiiw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class uosvosiiw_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  int metka_r;  
  uosvosiiw_r_data()
   {
    metka_r=0;
    voz=1;
    kon_ras=1;
   }
 };
gboolean   uosvosiiw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvosiiw_r_data *data);
gint uosvosiiw_r1(class uosvosiiw_r_data *data);
void  uosvosiiw_r_v_knopka(GtkWidget *widget,class uosvosiiw_r_data *data);

void	sapkasord(short dd,short md,short gd,int nomlist,FILE *ffva);
void   hgrkd(FILE *ff);

extern SQL_baza bd;


extern short	startgoduos; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;
extern float    nemi; /*Необлагаемый минимум*/
extern char	*spgnu; //Список групп налогового учёта
extern short    metkabo; //Если равна 1-бюджетная организация

int uosvosiiw_r(int metka_r,class uosvosiiw_data *datark,GtkWidget *wpredok)
{
char strsql[1024];
class iceb_u_str repl;
class uosvosiiw_r_data data;
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

if(metka_r == 0)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать ведомость износа и остаточной стоимости (налоговый учёт)"));
if(metka_r == 1)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать ведомость износа и остаточной стоимости (бух. учёт)"));

//sprintf(strsql,"%s %s",_config.system_name(),gettext("Распечатка ведомости аморт. отчислений для налогового учёта"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(uosvosiiw_r_key_press),&data);

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
if(metka_r == 0)
  repl.plus(gettext("Распечатать ведомость износа и остаточной стоимости (налоговый учёт)"));
if(metka_r == 1)
  repl.plus(gettext("Распечатать ведомость износа и остаточной стоимости (бух. учёт)"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(uosvosiiw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);
gtk_text_buffer_create_tag(data.buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)uosvosiiw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosvosiiw_r_v_knopka(GtkWidget *widget,class uosvosiiw_r_data *data)
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

gboolean   uosvosiiw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosvosiiw_r_data *data)
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
/****************************************************/

void itogi(const char *sh,
double *str_suma1,
double *str_suma2,
double *str_suma3,
short mt,  //0-по подразделению 1- по группе 2-общий итог 3-по счету учёта
int *kst,
FILE *ff,FILE *ffhpz,FILE *ff_f,FILE *ff_fi,FILE *ff_fis,
GtkWidget *view,
GtkTextBuffer *buffer)
{
char		bros[2048];
char		str[4096];

memset(str,'\0',sizeof(str));
memset(str,'-',164);

if(mt == 2)
 {
  sprintf(bros,gettext("Общий итог"));
 }

if(mt == 1)
 {
  if(sh[0] == '\0')
   return;
  sprintf(bros,"%s %s",gettext("Итого по группе"),sh);
  fprintf(ffhpz,"%s\n\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f \
%15.2f %12.2f %12.2f\n",str," ",iceb_u_kolbait(30,bros),bros," ",
  str_suma1[0],
  str_suma1[1],
  str_suma1[2],
  str_suma1[3],
  str_suma1[4],
  str_suma1[5],
  str_suma1[6],
  str_suma1[7]);
 }

double suma=0.;
for(int nom=0; nom < STR1; nom++)
   suma+=str_suma1[nom];

if(mt == 0)
 {
  if(suma < 0.0001)
   return;
  sprintf(bros,"%s %s",gettext("Итого по подразделению"),sh);
 }

if(mt == 3)
 {
  if(suma < 0.0001)
   return;
  sprintf(bros,"%s %s",gettext("Итого по счёту"),sh);
 }

fprintf(ff,"%s\n\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f \
%15.2f %12.2f %12.2f\n",str," ",iceb_u_kolbait(30,bros),bros," ",
str_suma1[0],
str_suma1[1],
str_suma1[2],
str_suma1[3],
str_suma1[4],
str_suma1[5],
str_suma1[6],
str_suma1[7]);

fprintf(ff_f,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_f,"\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f\
 %15.2f %12.2f %12.2f\n"," ",iceb_u_kolbait(30,bros),bros," ",
str_suma2[0],
str_suma2[1],
str_suma2[2],
str_suma2[3],
str_suma2[4],
str_suma2[5],
str_suma2[6],
str_suma2[7],
str_suma2[8],
str_suma2[9]);


fprintf(ff_fi,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_fi,"\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f\
 %15.2f %12.2f %12.2f\n"," ",iceb_u_kolbait(30,bros),bros," ",
str_suma2[0],
str_suma2[1],
str_suma2[2],
str_suma2[3],
str_suma2[4],
str_suma2[5],
str_suma2[6],
str_suma2[7],
str_suma2[8],
str_suma2[9]);

if(mt != 0)
 {
  fprintf(ff_fis,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(ff_fis,"\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f %10.2f \
%15.2f %12.2f %12.2f\n"," ",iceb_u_kolbait(30,bros),bros," ",
  str_suma2[0],
  str_suma2[1],
  str_suma2[2],
  str_suma2[3],
  str_suma2[4],
  str_suma2[5],
  str_suma2[6],
  str_suma2[7],
  str_suma2[8],
  str_suma2[9],
  str_suma2[10]);
 }
 
if(mt == 2 || mt == 3)
  fprintf(ffhpz,"%s\n\
%11s %-*s %10s %15.2f %12.2f %12.2f %12.2f %12.2f \
%15.2f %12.2f %12.2f\n",str," ",iceb_u_kolbait(30,bros),bros," ",
  str_suma1[0],
  str_suma1[1],
  str_suma1[2],
  str_suma1[3],
  str_suma1[4],
  str_suma1[5],
  str_suma1[6],
  str_suma1[7]);

*kst=*kst+2;

sprintf(str,"\n%s:\n\
%-*s:%15.2f %15.2f %15.2f\n\
%-*s:%15.2f %15.2f\n\
%-*s:%15.2f %15.2f %15.2f\n",
bros,
iceb_u_kolbait(26,gettext("Баланс. ст./Износ/Ост.ст.")),
gettext("Баланс. ст./Износ/Ост.ст."),
str_suma1[0],
str_suma1[1],
str_suma1[2],
iceb_u_kolbait(26,gettext("Измение бал. ст./Износ")),
gettext("Измение бал. ст./Износ"),
str_suma1[3],
str_suma1[4],
iceb_u_kolbait(26,gettext("Баланс. ст./Износ/Ост.ст.")),
gettext("Баланс. ст./Износ/Ост.ст."),
str_suma1[5],
str_suma1[6],
str_suma1[7]);

iceb_printw(str,buffer,view,"naim_shrift");

}
/********/
/*Шапка*/
/*******/

void haphna(short dn,short mn,short gn,
short dk,short mk,short gk,
int sl, //Счетчик листов
int *kst,  //Счетчик строк
int skl,const char *nai,
short mt,  //0-по подразделению 1-по материально-ответств
FILE *ff,
FILE *ff_f,
FILE *ff_fi,FILE *ff_fis)
{
char		str[312];
char		bros[312];

if(mt == 0)
 sprintf(bros,"%s %d %s",gettext("Подразделение"),skl,nai);

if(mt == 1)
 sprintf(bros,"%s %d %s",gettext("Мат.-ответственный"),skl,nai);

if(mt == 2)
 sprintf(bros,"%s",nai);

memset(str,'\0',sizeof(str));
memset(str,'-',164);

fprintf(ff,"\
%-*s %s N%d\n\%s\n",
iceb_u_kolbait(150,bros),bros,
gettext("Лист"),
sl,str);

fprintf(ff,gettext("\
Инвентарный|     Наименование             |          |  Балансовая   |   Износ    | Остаточная | Изменение  |    Износ   |   Балансова   |    Износ   | Остаточная |\n\
  номер    |                              |  Номер   | стоимость на  |    на      |стоимость на| балансовой |    за      | стоимость  на |    на      |стоимость на|\n\
           |                              |          | %02d.%02d.%dг.  |%02d.%02d.%dг.|%02d.%02d.%dг.| стоимости  |   период   |  %02d.%02d.%dг. |%02d.%02d.%dг.|%02d.%02d.%dг.|\n"),
dn,mn,gn,dn,mn,gn,dn,mn,gn,dk,mk,gk,dk,mk,gk,dk,mk,gk);

fprintf(ff,"%s\n",str);

if(ff_f != NULL)
 {
  fprintf(ff_f,"\
%-*s %s N%d\n",
  iceb_u_kolbait(150,bros),bros,
  gettext("Лист"),
  sl);

  fprintf(ff_f,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff_f,"\
Инвентарный|     Наименование             |          |  Балансовая   |   Износ    | Остаточная | Приход     |  Расход    |Амортизац.|Изменение |   Балансовая  |   Износ    | Остаточная |\n\
  номер    |                              |  Номер   | стоимасть на  |    на      |стоимость на| балансовой | балансовой |   за     |износа за | стоимость  на |    на      |стоимость на|\n\
           |                              |          | %02d.%02d.%dг.  |%02d.%02d.%dг.|%02d.%02d.%dг.| стоимости  | стоимости  | период   |  период  |  %02d.%02d.%dг. |%02d.%02d.%dг.|%02d.%02d.%dг.|\n",
  dn,mn,gn,dn,mn,gn,dn,mn,gn,dk,mk,gk,dk,mk,gk,dk,mk,gk);


  fprintf(ff_f,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
 }

if(ff_fi != NULL)
 {
  fprintf(ff_fi,"\
%-*s %s N%d\n",
  iceb_u_kolbait(150,bros),bros,
  gettext("Лист"),
  sl);

  fprintf(ff_fi,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff_fi,"\
Инвентарный|     Наименование             |          |  Балансовая   |   Износ    | Остаточная | Приход     |  Расход    |Амортизац.|Изменение |   Балансовая  |   Износ    | Остаточная |\n\
  номер    |                              |  Номер   | стоимость на  |    на      |стоимость на| балансовой | балансовой |   за     |износа за | стоимость  на |    на      |стоимость на|\n\
        x   |                              |          | %02d.%02d.%dг.  |%02d.%02d.%dг.|%02d.%02d.%dг.| стоимости  | стоимости  | период   |  период  |  %02d.%02d.%dг. |%02d.%02d.%dг.|%02d.%02d.%dг.|\n",
  dn,mn,gn,dn,mn,gn,dn,mn,gn,dk,mk,gk,dk,mk,gk,dk,mk,gk);


  fprintf(ff_fi,"\
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
 }

if(ff_fis != NULL)
 {
  fprintf(ff_fis,"\
%-*s %s N%d\n",
  iceb_u_kolbait(150,bros),bros,
  gettext("Лист"),
  sl);

  fprintf(ff_fis,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(ff_fis,"\
Инвентарный|     Наименование             |          |  Балансовая   |   Износ    | Остаточная | Приход     |  Расход    |Амортизац.|Изменение |Изменение |   Балансовая  |   Износ    | Остаточная |\n\
  номер    |                              |  Номер   | стоимость на  |    на      |стоимость на| балансовой | балансовой |   за     |износа за |остаточной| стоимость  на |    на      |стоимость на|\n\
	   |                              |          | %02d.%02d.%dг.  |%02d.%02d.%dг.|%02d.%02d.%dг.| стоимости  | стоимости  | период   |  период  |стоимости |  %02d.%02d.%dг. |%02d.%02d.%dг.|%02d.%02d.%dг.|\n",
  dn,mn,gn,dn,mn,gn,dn,mn,gn,dk,mk,gk,dk,mk,gk,dk,mk,gk);


  fprintf(ff_fis,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
 }
 
*kst=*kst+KOLSTSH;

}

/*******/
/*Счетчик строк*/
/*******/
void gsapphna(short dn,short mn,short gn,
short dk,short mk,short gk,
int *sl,  //Счетчик листов
int *kst, //Счетчик строк
int skl,const char *nai,
short mt,   //0-по подразделению 1-по материально-ответств
FILE *ff,FILE *ff_f)
{

*kst=*kst+1;

if(*kst <= kol_strok_na_liste_l)
 return;

*sl=*sl+1;
*kst=1;
fprintf(ff,"\f");
fprintf(ff_f,"\f");

haphna(dn,mn,gn,dk,mk,gk,*sl,kst,skl,nai,mt,ff,ff_f,NULL,NULL);


}

/**********************************/
/*Начало отчётов*/
/***********************************/

void uosvosii_start(short dn,short mn,short gn,
short dk,short mk,short gk,
int *kst,
class uosvosiiw_data *poi,
FILE *ff,
GtkWidget *wpredok)
{

*kst=5;

iceb_zagolov(gettext("Ведомость износа и остаточной стоимости основных средств"),dn,mn,gn,dk,mk,gk,ff,wpredok);

*kst+=1;

if(poi->viduh == 0)
 {
  fprintf(ff,"%s.\n",gettext("Налоговый учёт"));
 }
if(poi->viduh == 1)
 {
  fprintf(ff,"%s.\n",gettext("Бухгалтерский учёт"));
 }

int klst=0;
if(poi->podr.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Подразделение"),poi->podr.ravno());
   iceb_printcod(ff,"Uospod","kod","naik",0,poi->podr.ravno(),&klst);
  *kst+=klst+1;
 }

if(poi->grupa_nu.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа налогового учёта"),poi->grupa_nu.ravno());
  iceb_printcod(ff,"Uosgrup","kod","naik",1,poi->grupa_nu.ravno(),&klst);
  *kst+=klst+1;
 }
if(poi->grupa_bu.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Группа бухгалтерского учёта"),poi->grupa_bu.ravno());
  iceb_printcod(ff,"Uosgrup1","kod","naik",1,poi->grupa_bu.ravno(),&klst);
  *kst+=klst+1;
 }
if(poi->mat_ot.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Мат.-ответственный"),poi->mat_ot.ravno());
  iceb_printcod(ff,"Uosol","kod","naik",1,poi->mat_ot.ravno(),&klst);
  *kst+=klst+1;
 }
if(poi->hau.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр аналитического учета"),poi->hau.ravno());
  iceb_printcod(ff,"Uoshau","kod","naik",0,poi->hau.ravno(),&klst);
  *kst+=klst+1;
 }
if(poi->shetu.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Счёт"),poi->shetu.ravno());
  *kst+=1;
 }

if(poi->innom.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Инвентарный номер"),poi->innom.ravno());
  *kst+=1;

 }
 
if(poi->hzt.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Шифр затрат"),poi->hzt.ravno());
  *kst+=1;
 }

if(poi->sost_ob == 0)
 {
  fprintf(ff,"%s: %s\n",gettext("Состояние объекта"),gettext("все основные средства"));
  *kst+=1;
 }
if(poi->sost_ob == 1)
 {
  fprintf(ff,"%s: %s\n",gettext("Состояние объекта"),gettext("только те для которых амортизация считатся"));
  *kst+=1;
 }
if(poi->sost_ob == 2)
 {
  fprintf(ff,"%s: %s\n",gettext("Состояние объекта"),gettext("только те для которых амортизация не считатся"));
  *kst+=1;
 }

if(poi->metka_ost == 1)
 {
  fprintf(ff,"%s\n",gettext("Основные средства с нулевою остаточной стоимостью"));
  *kst+=1;
 }
if(poi->metka_ost == 2)
 {
  fprintf(ff,"%s\n",gettext("Основные средства с ненулевой остаточной стоимостью"));
  *kst+=1;
 }
*kst+=1;
if(poi->metka_mat_podr == 0)
 fprintf(ff,"%s\n",gettext("По подразделениям"));
if(poi->metka_mat_podr == 1)
 fprintf(ff,"%s\n",gettext("По материально-ответственным"));

*kst+=1;
if(poi->metka_sort == 0)
 fprintf(ff,"%s\n",gettext("Сортировка по группам"));
if(poi->metka_sort == 1)
 fprintf(ff,"%s\n",gettext("Сортировать по счетам учёта"));

}

/****************************/
/*проверка реквизитов поиска*/
/*****************************/
int uosvosii_rek(int in,
const char *naim,
short mn,short gn,
short mk,short gk,
int poddz,
int kodotl,
class uosvosiiw_data *poi,
const char *imatmptab,
GtkWidget *wpredok)
{
class poiinpdw_data rekin;
if(poiinpdw(in,mn,gn,&rekin,wpredok) != 0) /*на начало периода ещё не было*/
 {
  poiinpdw(in,mk,gk,&rekin,wpredok); /*смотрим реквизиты на конец периода*/
 }
   

if(poi->viduh == 0) //Пропускаем все основные средства если для них амортизазия считается в целом по группе
 if(uosprovgrw(rekin.hna.ravno(),wpredok) == 1)
    return(1);

if(poi->viduh == 0 ) //Налоговый учёт
 {
  if(rekin.soso == 0 || rekin.soso == 3) //амортизация считается
   if(poi->sost_ob == 2)
    return(1);
  if(rekin.soso == 1 || rekin.soso == 2) //амортизация не считаестя
   if(poi->sost_ob == 1)
    return(1);
  
 }
if(poi->viduh == 1 ) //Бух.учёт
 {
  if(rekin.soso == 0 || rekin.soso == 2) //амортизация считается
   if(poi->sost_ob == 2)
    return(1);
  if(rekin.soso == 1 || rekin.soso == 3) //амортизация не считаестя
   if(poi->sost_ob == 1)
    return(1);
  
 }
if(iceb_u_proverka(poi->shetu.ravno(),rekin.shetu.ravno(),0,0) != 0)
  return(1);

if(iceb_u_proverka(poi->hzt.ravno(),rekin.hzt.ravno(),0,0) != 0)
  return(1);

if(iceb_u_proverka(poi->grupa_nu.ravno(),rekin.hna.ravno(),0,0) != 0)
  return(1);

if(iceb_u_proverka(poi->grupa_bu.ravno(),rekin.hnaby.ravno(),0,0) != 0)
  return(1);

if(iceb_u_proverka(poi->hau.ravno(),rekin.hau.ravno(),0,0) != 0)
  return(1);

char strsql[1024];
sprintf(strsql,"insert into %s values ('%s',%d,%d,'%s',%d,'%s','%s','%s')",
imatmptab,
rekin.shetu.ravno(),kodotl,in,iceb_u_sqlfiltr(naim),poddz,rekin.nomz.ravno(),rekin.hna.ravno(),rekin.hnaby.ravno());
//printf("%s-%s\n",__FUNCTION__,strsql);
iceb_sql_zapis(strsql,1,1,wpredok);

return(0);
}
/*********************************/
/*определение где и с какими реквизитами числится основное средство на указанную дату*/
/*************************************/
int uosvosii_poi(int in,
const char *naim,
short dn,short mn,short gn,
short dk,short mk,short gk,
class uosvosiiw_data *poi,
const char *imatmptab,
GtkWidget *wpredok)
{
SQL_str row,row1;
class SQLCURSOR cur,cur1;
int kolstr=0;
char strsql[1024];
int poddz=0;
int kodotl=0;

if(poiinw(in,dn,mn,gn,&poddz,&kodotl,wpredok) == 0) 
 {
  if(iceb_u_proverka(poi->podr.ravno(),poddz,0,0) == 0)
   if(iceb_u_proverka(poi->mat_ot.ravno(),kodotl,0,0) == 0)
    {
     uosvosii_rek(in,naim,mn,gn,mk,gk,poddz,kodotl,poi,imatmptab,wpredok);
    }
 }

/*просмотриваем период -может быть куча внутренних перемещений*/

sprintf(strsql,"select datd,podr,nomd from Uosdok1 where \
datd >= '%04d-%d-%d' and datd <= '%04d-%d-%d' and innom=%d and podt=1 \
order by datd asc, tipz desc",gn,mn,dn,gk,mk,dk,in);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
short d,m,g;
while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(poi->podr.ravno(),row[1],0,0) != 0)
   continue;
  poddz=atoi(row[1]);
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  /**************Читаем документ*/
  sprintf(strsql,"select kodol from Uosdok where datd='%s' and \
  nomd='%s'",row[0],row[2]);

  //printw("%s\n",strsql);

  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    sprintf(strsql,"Не найден документ N%s от %s !",row[2],row[0]);
    iceb_menu_soob(strsql,wpredok);
    continue;
   }
  kodotl=atoi(row1[0]);
  if(iceb_u_proverka(poi->mat_ot.ravno(),row1[0],0,0) != 0)
   continue;

  uosvosii_rek(in,naim,mn,gn,mk,gk,poddz,kodotl,poi,imatmptab,wpredok);
 }




return(0);
}



/******************************************/
/******************************************/

gint uosvosiiw_r1(class uosvosiiw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;

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

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");




sprintf(strsql,"select innom,naim from Uosin");
int kolstr=0;
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
class iceb_fopen fil_prot;
char imaf_prot[64];
sprintf(imaf_prot,"uospr%d.lst",getpid());
if(fil_prot.start(imaf_prot,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_tmptab tabtmp;
const char *imatmptab={"uosvosiiw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

if(data->rk->metka_mat_podr == 0) //По подразделениям
 sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shu char(24) not null,\
kmo int not null,\
innom int not null,\
naim char(112) not null,\
pod int not null,\
nomz char(56) not null,\
hna char(32) not null,\
hnaby char(32) not null,\
unique(pod,innom)) ENGINE = MYISAM",imatmptab);
else
 sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shu char(24) not null,\
kmo int not null,\
innom int not null,\
naim char(112) not null,\
pod int not null,\
nomz char(56) not null,\
hna char(32) not null,\
hnaby char(32) not null,\
unique(kmo,innom)) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  



float kolstr1=0;
int in=0;
class poiinpdw_data rekin;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->innom.ravno(),row[0],0,0) != 0)
    continue;

  in=atoi(row[0]);

  uosvosii_poi(in,row[1],dn,mn,gn,dk,mk,gk,data->rk,imatmptab,data->window);

  
 }


if(data->rk->metka_mat_podr == 0) //По подразделениям
 {
  if(data->rk->metka_sort == 0)
   {
    if(data->rk->viduh == 0) //налоговый учёт
      sprintf(strsql,"select * from %s order by hna asc,pod asc,innom asc",imatmptab);
    if(data->rk->viduh == 1) //бух. учёт
      sprintf(strsql,"select * from %s order by hnaby asc,pod asc,innom asc",imatmptab);
   }
  if(data->rk->metka_sort == 1)
      sprintf(strsql,"select * from %s order by shu asc,pod asc,innom asc",imatmptab);
 }
if(data->rk->metka_mat_podr == 1) //По материально-ответственным
 {
  if(data->rk->metka_sort == 0)
   {
    if(data->rk->viduh == 0) //налоговый учёт
      sprintf(strsql,"select * from %s order by hna asc,kmo asc,innom asc",imatmptab);
    if(data->rk->viduh == 1) //бух.учёт
      sprintf(strsql,"select * from %s order by hnaby asc,kmo asc,innom asc",imatmptab);
   }
  if(data->rk->metka_sort == 1)
      sprintf(strsql,"select * from %s order by shu asc,kmo asc,innom asc",imatmptab);

 }

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }

char imaf[64];
if(data->rk->metka_mat_podr == 0)
 sprintf(imaf,"iznosp%d.lst",getpid());
if(data->rk->metka_mat_podr == 1)
 sprintf(imaf,"iznosm%d.lst",getpid());

class iceb_fopen fil_ff;
if(fil_ff.start(imaf,"w",data->window) != 0)
 return(1);

char imaf_csv[64];
sprintf(imaf_csv,"iznos%d.csv",getpid());
class iceb_fopen fil_ff_csv;
if(fil_ff_csv.start(imaf_csv,"w",data->window) != 0)
 return(1);

char imaf_f[64];

if(data->rk->metka_mat_podr == 0)
 sprintf(imaf_f,"viiosp%d.lst",getpid());
if(data->rk->metka_mat_podr == 1)
 sprintf(imaf_f,"viiosm%d.lst",getpid());
class iceb_fopen fil_f;
if(fil_f.start(imaf_f,"w",data->window) != 0)
 return(1);

char imaf_fi[64];

if(data->rk->metka_mat_podr == 0)
 sprintf(imaf_fi,"viiospi%d.lst",getpid());
if(data->rk->metka_mat_podr == 1)
 sprintf(imaf_fi,"viiosmi%d.lst",getpid());
class iceb_fopen fil_fi;
if(fil_fi.start(imaf_fi,"w",data->window) != 0)
 return(1);
 

//Отчёт без итогов по подразделению, только по шифрам затрат
char imaf_fis[64];

if(data->rk->metka_mat_podr == 0)
 sprintf(imaf_fis,"visn%d.lst",getpid());
if(data->rk->metka_mat_podr == 1)
 sprintf(imaf_fis,"visb%d.lst",getpid());
class iceb_fopen fil_fis;
if(fil_fis.start(imaf_fis,"w",data->window) != 0)
 return(1);

char imafhpz[64];
if(data->rk->metka_mat_podr == 0)
 sprintf(imafhpz,"iznospi%d.lst",getpid());
if(data->rk->metka_mat_podr == 1)
 sprintf(imafhpz,"iznosmi%d.lst",getpid());
class iceb_fopen fil_ffhpz;
if(fil_ffhpz.start(imafhpz,"w",data->window) != 0)
 return(1);
int kst=0;

uosvosii_start(dn,mn,gn,dk,mk,gk,&kst,data->rk,fil_ff.ff,data->window);
uosvosii_start(dn,mn,gn,dk,mk,gk,&kst,data->rk,fil_ff_csv.ff,data->window);
uosvosii_start(dn,mn,gn,dk,mk,gk,&kst,data->rk,fil_ffhpz.ff,data->window);
uosvosii_start(dn,mn,gn,dk,mk,gk,&kst,data->rk,fil_f.ff,data->window);
uosvosii_start(dn,mn,gn,dk,mk,gk,&kst,data->rk,fil_fi.ff,data->window);

kst=0;
uosvosii_start(dn,mn,gn,dk,mk,gk,&kst,data->rk,fil_fis.ff,data->window);

 
int kstt=0;
haphna(dn,mn,gn,dk,mk,gk,1,&kstt,0,"",data->rk->metka_mat_podr,fil_ffhpz.ff,NULL,fil_fi.ff,fil_fis.ff);

fprintf(fil_ff_csv.ff,"%s|%s|%s|%s %d.%d.%d|%s %d.%d.%d|%s %d.%d.%d|\
%s|%s|%s %d.%d.%d|%s %d.%d.%d|%s %d.%d.%d|\n",
gettext("Инвентарный номер"),
gettext("Наименование"),
gettext("Номер"),
gettext("Балансовая стоимость на"),
dn,mn,gn,
gettext("Износ на"),
dn,mn,gn,
gettext("Остаточная стоимость на"),
dn,mn,gn,
gettext("Изменение балансовой стоимости"),
gettext("Износ за период"),
gettext("Балансовая стоимость на"),
dk,mk,gk,
gettext("Износ на"),
dk,mk,gk,
gettext("Остаточная стоимость на"),
dk,mk,gk);

int mvsi=0,mvsh=0;
in=0;
int sli=1;
int podz=0;
kolstr1=0;
double iznos_per[2];
memset(&iznos_per,'\0',sizeof(iznos_per));

double str_suma1[STR1]; /*строка с суммами для вывода*/
double ithz_suma1[STR1];
double itpd_suma1[STR1];
double itsh_suma1[STR1];
double itall_suma1[STR1];

double str_suma2[STR2]; /*строка с суммами для вывода*/
double ithz_suma2[STR2];
double itpd_suma2[STR2];
double itsh_suma2[STR2];
double itall_suma2[STR2];

double ithz_suma3[STR3];
double itpd_suma3[STR3];
double itsh_suma3[STR3];
double itall_suma3[STR3];

memset(str_suma1,'\0',sizeof(str_suma1));
memset(ithz_suma1,'\0',sizeof(ithz_suma1));
memset(itpd_suma1,'\0',sizeof(itpd_suma1));
memset(itsh_suma1,'\0',sizeof(itsh_suma1));
memset(itall_suma1,'\0',sizeof(itall_suma1));

memset(str_suma2,'\0',sizeof(str_suma2));
memset(ithz_suma2,'\0',sizeof(ithz_suma2));
memset(itpd_suma2,'\0',sizeof(itpd_suma2));
memset(itsh_suma2,'\0',sizeof(itsh_suma2));
memset(itall_suma2,'\0',sizeof(itall_suma2));

memset(ithz_suma3,'\0',sizeof(ithz_suma3));
memset(itpd_suma3,'\0',sizeof(itpd_suma3));
memset(itsh_suma3,'\0',sizeof(itsh_suma3));
memset(itall_suma3,'\0',sizeof(itall_suma3));
class iceb_u_str hna("");
class iceb_u_str hna1("");
int pod=0;
class iceb_u_str naimos("");
class iceb_u_str nomz("");
double pr1=0.,pr2=0.;
double pr11=0.,pr22=0.;
class iceb_u_str bros("");
SQL_str row1;
class SQLCURSOR cur1;
class iceb_u_str naipod("");
class iceb_u_str naigrup("");
int kolos=0;
int kodmo=0;
int kodot=0;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  sprintf(strsql,"%s %s %s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7]);
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

  if(data->rk->metka_sort == 0)
   {
    if(data->rk->viduh == 0) //налоговый учёт
     hna.new_plus(row[6]);
    if(data->rk->viduh == 1) //бух.учёт
     hna.new_plus(row[7]);
   }
  if(data->rk->metka_sort == 1)
    hna.new_plus(row[0]);

  if(data->rk->metka_mat_podr == 0) //По подразделениям
   {
//    podrp=pod=atoi(row[4]);
      kodot=atoi(row[4]);
   }
  if(data->rk->metka_mat_podr == 1) //По материально-ответственным
   {
    kodot=atoi(row[1]);
//    pod=atoi(row[1]);
//    podrp=atoi(row[4]);
   }
  pod=atoi(row[4]);
  kodmo=atoi(row[1]);
  in=atoi(row[2]);
  naimos.new_plus(row[3]);
  nomz.new_plus(row[5]);
  memset(iznos_per,'\0',sizeof(iznos_per));

  double bs_prih_za_per=0.;
  double bs_rash_za_per=0.;
  
  class bsiz_data data_per;
  
  if(data->rk->metka_mat_podr == 0) //По подразделениям
    bsizpw(in,pod,0,dn,mn,gn,dk,mk,gk,&data_per,fil_prot.ff,data->window);
  else
    bsizpw(in,0,kodmo,dn,mn,gn,dk,mk,gk,&data_per,fil_prot.ff,data->window);
  
  if(data->rk->viduh == 0)
   {
    pr1=data_per.nu.bs_nah_per; //Балансовая стоимость на начало периода
    pr2=data_per.nu.izmen_iznosa_nah_per+data_per.nu.amort_nah_per;  //Износ на начало периода
      
    pr11=data_per.nu.bs_kon_per; //Балансовая стоимость на конец периода
    
    pr22=data_per.nu.izmen_iznosa_kon_per+data_per.nu.amort_kon_per;  //Износ на конец периода
    
    iznos_per[0]+=data_per.nu.amort_raznica;     //Амортизация за период
    iznos_per[1]+=data_per.nu.izmen_iznosa_raznica; //Изменение износа за период
    bs_prih_za_per=data_per.nu.bs_prih_za_per;
    bs_rash_za_per=data_per.nu.bs_rash_za_per;
   }
  
  if(data->rk->viduh == 1)
   {
    pr1=data_per.bu.bs_nah_per; //Балансовая стоимость на начало периода
    pr2=data_per.bu.izmen_iznosa_nah_per+data_per.bu.amort_nah_per;  //Износ на начало периода
    
    pr11=data_per.bu.bs_kon_per; //Балансовая стоимость на конец периода
    
    pr22=data_per.bu.izmen_iznosa_kon_per+data_per.bu.amort_kon_per;  //Износ на конец периода
    
    iznos_per[0]+=data_per.bu.amort_raznica;     //Амортизация за период
    iznos_per[1]+=data_per.bu.izmen_iznosa_raznica; //Изменение износа за период
    bs_prih_za_per=data_per.bu.bs_prih_za_per;
    bs_rash_za_per=data_per.bu.bs_rash_za_per;

   }
    
    
  
  if(fabs(pr1) < 0.01 && fabs(pr11) < 0.01) //Значит уже продан или списан или передан в другое подразделение
   {
    continue;
   }
  
//  printf("%s\n",__FUNCTION__);
  
  if(data->rk->metka_mat_podr == 0 && data->rk->mat_ot.ravno()[0] != '\0') /*По подразделениям*/
   {
    //Может быть движение внутри подразделения по разным материально-ответственным лицам
    //в этом случае балансовая стоимость не равна нулю
    //Проверяем числился ли у этого мат. ответственного
    int pp=0,kk=0;
    if(poiinw(in,dk,mk,gk,&pp,&kk,data->window) != 0)
      continue;
    sprintf(strsql,"%d",kk);
    if(iceb_u_proverka(data->rk->mat_ot.ravno(),strsql,0,0) != 0)
      continue;
   }

  if(podz != kodot)
   {
    if(podz != 0)
     {
      bros.new_plus(podz);
      itogi(bros.ravno(),itpd_suma1,itpd_suma2,itpd_suma3,0,&kst,fil_ff.ff,fil_ffhpz.ff,fil_f.ff,fil_fi.ff,fil_fis.ff,data->view,data->buffer);
      memset(itpd_suma1,'\0',sizeof(itpd_suma1));
      memset(itpd_suma2,'\0',sizeof(itpd_suma2));
      memset(itpd_suma3,'\0',sizeof(itpd_suma3));
     }

    if(data->rk->metka_mat_podr == 0)
     {
      /*Читаем наименование подразделения*/
      sprintf(strsql,"select naik from Uospod where kod=%d",kodot);
      if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
       {
        iceb_beep();
        sprintf(strsql,"%s %d !\n",gettext("Не найден код подразделения"),kodot);
        
        iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
    
        fprintf(fil_ff.ff,"%s %d !\n",gettext("Не найден код подразделения"),kodot);
        naipod.new_plus("");
       }
      else
        naipod.new_plus(row1[0]);
     }

    if(data->rk->metka_mat_podr == 1)
     {
      /*Читаем мат.-ответственного*/
      sprintf(strsql,"select naik from Uosol where kod=%d",kodot);
      if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
       {
        iceb_beep();
        sprintf(strsql,"%s %d !\n",gettext("Не найден мат.-ответственный"),kodot);
        
        iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
        fprintf(fil_ff.ff,"%s %d !\n",gettext("Не найден мат.-ответственный"),kodot);
        naipod.new_plus("");
       }
      else
        naipod.new_plus(row1[0]);
     }

    mvsh++;
             
    podz=kodot;

    if(data->rk->metka_mat_podr == 0)
     sprintf(strsql,"\n%s - %d/%s\n",gettext("Подразделение"),podz,naipod.ravno());

    if(data->rk->metka_mat_podr == 1)
     sprintf(strsql,"\n%s - %d/%s\n",gettext("Мат.-ответственный"),podz,naipod.ravno());

    iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
   }

  if(iceb_u_SRAV(hna1.ravno(),hna.ravno(),0) != 0)
   {
    if(hna1.ravno()[0] != '\0')
     {
      if(data->rk->metka_sort == 0)
       {
        itogi(hna1.ravno(),ithz_suma1,ithz_suma2,ithz_suma3,1,&kst,fil_ff.ff,fil_ffhpz.ff,fil_f.ff,fil_fi.ff,fil_fis.ff,data->view,data->buffer);
        memset(ithz_suma1,'\0',sizeof(ithz_suma1));
        memset(ithz_suma2,'\0',sizeof(ithz_suma2));
        memset(ithz_suma3,'\0',sizeof(ithz_suma3));

       }

      if(data->rk->metka_sort == 1)
       {
        itogi(hna1.ravno(),itsh_suma1,itsh_suma2,itsh_suma3,3,&kst,fil_ff.ff,fil_ffhpz.ff,fil_f.ff,fil_fi.ff,fil_fis.ff,data->view,data->buffer);
        memset(itsh_suma1,'\0',sizeof(itsh_suma1));
        memset(itsh_suma2,'\0',sizeof(itsh_suma2));
        memset(itsh_suma3,'\0',sizeof(itsh_suma3));
       }
     }
    
    if(data->rk->metka_sort == 0)
     {
      /*Уэнаем наименование шифра нормы аморт. отчисления*/
      if(iceb_u_SRAV(hna.ravno(),"???",0) != 0)
       {
        if(data->rk->viduh == 0)
          sprintf(strsql,"select naik from Uosgrup where kod='%s'",hna.ravno());
        if(data->rk->viduh == 1)
          sprintf(strsql,"select naik from Uosgrup1 where kod='%s'",hna.ravno());

        if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
         {
          iceb_beep();
          sprintf(strsql,"%s %s !\n",gettext("Не найдено группу"),hna.ravno());
          iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
          fprintf(fil_ff.ff,"%s %s !\n",gettext("Не найдено группу"),hna.ravno());
          naigrup.new_plus("");
         }
        else
          naigrup.new_plus(row1[0]);
       }
      else
       naigrup.new_plus(gettext("Не введена в карточку группа"));
      sprintf(strsql,"\n%s %s \"%s\"\n",gettext("Группа"),hna.ravno(),naigrup.ravno());
      iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
     }
    if(data->rk->metka_sort == 1)
     {
      sprintf(strsql,"select nais from Plansh where ns='%s'",hna.ravno());
      if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
        naigrup.new_plus(row1[0]);
      else
        naigrup.new_plus("");
     
      sprintf(strsql,"\n%s %s \"%s\"\n",
      gettext("Счёт учёта"),hna.ravno(),naigrup.ravno());
      iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
     }
    mvsi++;


    hna1.new_plus(hna.ravno());

   }

  if(mvsh > 0 || mvsi > 0) 
   {  
    if(kst > kol_strok_na_liste_l-KOLSTSH-5)
     {
      fprintf(fil_ff.ff,"\f");
      fprintf(fil_f.ff,"\f");
      sli++;
      kst=0;
     }
    if(mvsi > 0)
     {
      if(data->rk->metka_sort == 0)
       {
        fprintf(fil_ff.ff,"\n%s %s \"%s\"\n",gettext("Группа"),hna.ravno(),naigrup.ravno());
        fprintf(fil_f.ff,"\n%s %s \"%s\"\n",gettext("Группа"),hna.ravno(),naigrup.ravno());
       }
      if(data->rk->metka_sort == 1)
       {
        fprintf(fil_ff.ff,"\n%s %s \"%s\"\n",
        gettext("Счёт учёта"),hna.ravno(),naigrup.ravno());
        fprintf(fil_f.ff,"\n%s %s \"%s\"\n",
        gettext("Счёт учёта"),hna.ravno(),naigrup.ravno());
       }
      mvsi=0;
      kst+=2;
     } 
    haphna(dn,mn,gn,dk,mk,gk,sli,&kst,kodot,naipod.ravno(),data->rk->metka_mat_podr,fil_ff.ff,fil_f.ff,NULL,NULL);
    mvsh=0;
   }
  double izn=pr2;
  double bsn=pr1;
  double osts=bsn-izn;

  if(osts >  0.009 && data->rk->metka_ost == 1)
   continue;

  if(osts ==  0. && data->rk->metka_ost == 2)
   continue;


  double izpr=pr22-pr2;


  if(fabs(bsn+izn+osts+pr11-pr1+pr22-pr2+bsn+pr11-pr1+izn+pr22+pr2+(bsn+pr11-pr1)+(izn+pr22-pr2)) > 0.009)
   {
    gsapphna(dn,mn,gn,dk,mk,gk,&sli,&kst,kodot,naipod.ravno(),data->rk->metka_mat_podr,fil_ff.ff,fil_f.ff);
    
    memset(str_suma1,'\0',sizeof(str_suma1));
    str_suma1[0]=bsn;
    str_suma1[1]=izn;
    str_suma1[2]=osts;
    str_suma1[3]=pr11-pr1;
    str_suma1[4]=izpr;
    str_suma1[5]=bsn+pr11-pr1;
    str_suma1[6]=izn+pr22-pr2;
    str_suma1[7]=(bsn+pr11-pr1)-(izn+pr22-pr2);
    for(int nom=0; nom < STR1; nom++)
     {
      ithz_suma1[nom]+=str_suma1[nom];
      itpd_suma1[nom]+=str_suma1[nom];
      itsh_suma1[nom]+=str_suma1[nom];
      itall_suma1[nom]+=str_suma1[nom];
     }

    

    fprintf(fil_ff.ff,"\
%11d %-*.*s %-*.*s %15.2f %12.2f %12.2f %12.2f %12.2f %15.2f %12.2f %12.2f\n",
    in,
    iceb_u_kolbait(30,naimos.ravno()),iceb_u_kolbait(30,naimos.ravno()),naimos.ravno(),
    iceb_u_kolbait(10,nomz.ravno()),iceb_u_kolbait(10,nomz.ravno()),nomz.ravno(),
    str_suma1[0],
    str_suma1[1],
    str_suma1[2],
    str_suma1[3],
    str_suma1[4],
    str_suma1[5],
    str_suma1[6],
    str_suma1[7]);

    fprintf(fil_ff_csv.ff,"\
%d|%s|%s|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f\n",
    in,
    naimos.ravno(),
    nomz.ravno(),
    str_suma1[0],
    str_suma1[1],
    str_suma1[2],
    str_suma1[3],
    str_suma1[4],
    str_suma1[5],
    str_suma1[6],
    str_suma1[7]);

    memset(str_suma2,'\0',sizeof(str_suma2));
    str_suma2[0]=bsn;
    str_suma2[1]=izn;
    str_suma2[2]=osts;
    str_suma2[3]=bs_prih_za_per;
    str_suma2[4]=bs_rash_za_per;
    str_suma2[5]=iznos_per[0];
    str_suma2[6]=iznos_per[1];
    str_suma2[7]=bsn+pr11-pr1;
    str_suma2[8]=izn+pr22-pr2;
    str_suma2[9]=(bsn+pr11-pr1)-(izn+pr22-pr2);
    for(int nom=0; nom < STR2; nom++)
     {
      ithz_suma2[nom]+=str_suma2[nom];
      itpd_suma2[nom]+=str_suma2[nom];
      itsh_suma2[nom]+=str_suma2[nom];
      itall_suma2[nom]+=str_suma2[nom];
     }

    
    int nom1=0;
    for(int nom=0; nom < STR2; nom++)
     {

      if(nom1 == 7)
       {
        itall_suma3[nom1]+=bs_prih_za_per-bs_rash_za_per;
        ithz_suma3[nom1]+=bs_prih_za_per-bs_rash_za_per;
        itsh_suma3[nom1]+=bs_prih_za_per-bs_rash_za_per;
        itpd_suma3[nom1]+=bs_prih_za_per-bs_rash_za_per;
        nom1++;
        nom--;
       }
      else
       {
        itall_suma3[nom1]+=str_suma2[nom];
        ithz_suma3[nom1]+=str_suma2[nom];
        itsh_suma3[nom1]+=str_suma2[nom];
        itpd_suma3[nom1]+=str_suma2[nom];
        nom1++;
       }
     }

    fprintf(fil_f.ff,"\
%11d %-*.*s %-*.*s %15.2f %12.2f %12.2f %12.2f %12.2f %10.2f %10.2f %15.2f %12.2f %12.2f\n",
    in,
    iceb_u_kolbait(30,naimos.ravno()),iceb_u_kolbait(30,naimos.ravno()),naimos.ravno(),
    iceb_u_kolbait(10,nomz.ravno()),iceb_u_kolbait(10,nomz.ravno()),nomz.ravno(),
    str_suma2[0],
    str_suma2[1],
    str_suma2[2],
    str_suma2[3],
    str_suma2[4],
    str_suma2[5],
    str_suma2[6],
    str_suma2[7],
    str_suma2[8],
    str_suma2[9]);

    

    if(iceb_u_strlen(naimos.ravno()) > 30)
     {
      gsapphna(dn,mn,gn,dk,mk,gk,&sli,&kst,kodot,naipod.ravno(),data->rk->metka_mat_podr,fil_ff.ff,fil_f.ff);
      fprintf(fil_ff.ff,"%11s %s\n","",iceb_u_adrsimv(30,naimos.ravno()));
      fprintf(fil_f.ff,"%11s %s\n","",iceb_u_adrsimv(30,naimos.ravno()));
     }
    kolos++;
   }
 }
bros.new_plus(kodot);
itogi(bros.ravno(),itpd_suma1,itpd_suma2,itpd_suma3,0,&kst,fil_ff.ff,fil_ffhpz.ff,fil_f.ff,fil_fi.ff,fil_fis.ff,data->view,data->buffer);

if(data->rk->metka_sort == 0)
 itogi(hna1.ravno(),ithz_suma1,ithz_suma2,ithz_suma3,1,&kst,fil_ff.ff,fil_ffhpz.ff,fil_f.ff,fil_fi.ff,fil_fis.ff,data->view,data->buffer);

if(data->rk->metka_sort == 1)
 itogi(hna1.ravno(),itsh_suma1,itsh_suma2,itsh_suma3,3,&kst,fil_ff.ff,fil_ffhpz.ff,fil_f.ff,fil_fi.ff,fil_fis.ff,data->view,data->buffer);

/*Итог Общий*/
itogi("",itall_suma1,itall_suma2,itall_suma3,2,&kst,fil_ff.ff,fil_ffhpz.ff,fil_f.ff,fil_fi.ff,fil_fis.ff,data->view,data->buffer);


sprintf(strsql,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

fprintf(fil_ff.ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(fil_f.ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(fil_fi.ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(fil_fis.ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);
fprintf(fil_ffhpz.ff,"\n%s:%d\n",gettext("Количество основных средств"),kolos);

iceb_podpis(fil_ff.ff,data->window);
iceb_podpis(fil_ff_csv.ff,data->window);
iceb_podpis(fil_f.ff,data->window);
iceb_podpis(fil_fi.ff,data->window);
iceb_podpis(fil_fis.ff,data->window);
iceb_podpis(fil_ffhpz.ff,data->window);
iceb_podpis(fil_prot.ff,data->window);

fil_ff.end();
fil_ff_csv.end();
fil_f.end();
fil_fi.end();
fil_fis.end();
fil_ffhpz.end();
fil_prot.end();

data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Ведомость износа и остаточной стоимости основных средств"));
data->rk->imaf.plus(imafhpz);
data->rk->naim.plus(gettext("Общий итог"));
data->rk->imaf.plus(imaf_f);
data->rk->naim.plus(gettext("Ведомость с расшифровкой изменения балансовой стоимости"));
data->rk->imaf.plus(imaf_fi);
data->rk->naim.plus(gettext("Общие итоги по ведомости с расшифровкой"));
data->rk->imaf.plus(imaf_fis);
data->rk->naim.plus(gettext("Общие итоги по ведомости с расшифровкой+остаточная бал.стоимость"));
data->rk->imaf.plus(imaf_prot);
data->rk->naim.plus(gettext("Протокол хода расчёта"));

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),0,data->window);

data->rk->imaf.plus(imaf_csv);
data->rk->naim.plus(gettext("Ведомость износа для загрузки в электронные таблицы"));

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);

}
