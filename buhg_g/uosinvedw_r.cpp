/*$Id: uosinvedw_r.c,v 1.23 2013/09/26 09:46:57 sasa Exp $*/
/*22.07.2019	13.01.2008	Белых А.И.	uosinvedw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "uosinvedw.h"

class drag_met
 {
  public:
  class iceb_u_spisok dm_ei; /*Список драг металлов-единиц измерения*/
  class iceb_u_double ves_dm; /*Массив для итоговых весов по горизонтали*/
  class iceb_u_double ves_dmv; /*Массив для итоговых весов по вертикали*/
  class iceb_u_int sp_in; /*Список инвентарных номеров*/
  class iceb_u_double ves_hax; /*Шахматка основное средство-вес*/
 };

class uosinvedw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class uosinvedw_data *rk;
  class spis_oth *oth;
    
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  uosinvedw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };

gboolean   uosinvedw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosinvedw_r_data *data);
gint uosinvedw_r1(class uosinvedw_r_data *data);
void  uosinvedw_r_v_knopka(GtkWidget *widget,class uosinvedw_r_data *data);


extern SQL_baza bd;



extern int kol_strok_na_liste;
extern int kol_strok_na_liste_l;


int uosinvedw_r(class spis_oth *oth,class uosinvedw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class uosinvedw_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);
data.rk=datark;
data.oth=oth;

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
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(uosinvedw_r_key_press),&data);

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(uosinvedw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)uosinvedw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  uosinvedw_r_v_knopka(GtkWidget *widget,class uosinvedw_r_data *data)
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

gboolean   uosinvedw_r_key_press(GtkWidget *widget,GdkEventKey *event,class uosinvedw_r_data *data)
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


/*************************************/
/*итоги*/
/**************************************/
void usoinved_itog572(const char *nazv_itog,int kolih_uos_list,double *itogo_572,int metka_fn,FILE *ff)
{

if(metka_fn == 1) /*не заполнять не совпадает с терминальной версией*/
 fprintf(ff,"%*s|%4s|%13s|%6s|%4d|%13.2f|%12.2f|%12.2f|\n",
 iceb_u_kolbait(89,nazv_itog),
 nazv_itog,
 "",
 "",
 "",
 kolih_uos_list,
 itogo_572[0],
 itogo_572[1],
 itogo_572[0]-itogo_572[1]);
else
 fprintf(ff,"%*s|%4d|%13.2f|%6s|%4d|%13.2f|%12.2f|%12.2f|\n",
 iceb_u_kolbait(89,nazv_itog),
 nazv_itog,
 kolih_uos_list,
 itogo_572[0],
 "",
 kolih_uos_list,
 itogo_572[0],
 itogo_572[1],
 itogo_572[0]-itogo_572[1]);

}


/****************************************/
/*Распечатка по счетам*/
/*******************************/
void uosinved_rps(class iceb_u_spisok *sp_shet,class iceb_u_double *kol_shet,class iceb_u_double *sum_shet,FILE *ff)
{

for(int nom=0; nom < sp_shet->kolih(); nom++)
 {
  fprintf(ff,"%-*.*s %10.10g %12.2f\n",
  iceb_u_kolbait(10,sp_shet->ravno(nom)),
  iceb_u_kolbait(10,sp_shet->ravno(nom)),
  sp_shet->ravno(nom),
  kol_shet->ravno(nom),
  sum_shet->ravno(nom));  
 }
}

/****************************************/
/*Распечатка по драгоценным металлам*/
/****************************************/
int uosinved_rdm(short di,short mi,short gi,
class drag_met *dragm,
class iceb_u_str *imaf,
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
FILE *ff_dm;
char imaf_dm[64];
sprintf(imaf_dm,"indm%d.lst",getpid());
imaf->new_plus(imaf_dm);
if((ff_dm = fopen(imaf_dm,"w")) == NULL)
 {
   iceb_er_op_fil(imaf_dm,"",errno,wpredok);
  return(1);
 }

iceb_u_zagolov(gettext("Инвентаризационная опись драгоценных металлов"),0,0,0,di,mi,gi, iceb_get_pnk("00",0,wpredok),ff_dm);
class iceb_u_str liniq;

int kolih_dm=dragm->dm_ei.kolih();

liniq.plus("-----------------------------------------------------------------------");
for(int ii=0; ii < kolih_dm; ii++)
 {
  if(dragm->ves_dm.ravno(ii) == 0.)
   continue;
  liniq.plus("-----------");
 }
fprintf(ff_dm,"%s\n",liniq.ravno());
  
fprintf(ff_dm,gettext("\
Номер|Инвентарный|        Наименование          | Модель   |Заводской |"));

class iceb_u_str drm("");
class iceb_u_str naim_dm("");
for(int ii=0; ii < kolih_dm; ii++)
 {
  if(dragm->ves_dm.ravno(ii) == 0.)
   continue;
  iceb_u_polen(dragm->dm_ei.ravno(ii),&drm,1,'|');

  sprintf(strsql,"select naik from Uosdm where kod=%d",drm.ravno_atoi());

  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     naim_dm.new_plus(row[0]);    
  else
     naim_dm.new_plus("");
  sprintf(strsql,"%s %s",drm.ravno(),naim_dm.ravno());
  fprintf(ff_dm,"%-*.*s|",iceb_u_kolbait(10,strsql),iceb_u_kolbait(10,strsql),strsql);
 }
fprintf(ff_dm,"\n");

fprintf(ff_dm,gettext("\
 п-п | номер     |                              |          |  номер   |"));
/*
12345 12345678901 123456789012345678901234567890 1234567890 1234567890
*/
for(int ii=0; ii < kolih_dm; ii++)
 {
  if(dragm->ves_dm.ravno(ii) == 0.)
   continue;
  iceb_u_polen(dragm->dm_ei.ravno(ii),&drm,2,'|');
  fprintf(ff_dm,"%-*.*s|",iceb_u_kolbait(10,drm.ravno()),iceb_u_kolbait(10,drm.ravno()),drm.ravno());
 }

fprintf(ff_dm,"\n%s\n",liniq.ravno());
class iceb_u_str naim("");
class iceb_u_str model("");
class iceb_u_str zav_nomer("");
int nomer_pp_dm=0;
for(int nom_in=0; nom_in < dragm->sp_in.kolih(); nom_in++)
 {
  if(dragm->ves_dmv.ravno(nom_in) == 0.)
   continue;
 
  sprintf(strsql,"select naim,model,zavnom from Uosin where innom=%d",dragm->sp_in.ravno(nom_in));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    naim.new_plus(row[0]);
    model.new_plus(row[1]);
    zav_nomer.new_plus(row[2]);
   }
  else
   {
    zav_nomer.new_plus("");
    naim.new_plus("");
    model.new_plus("");
   }
  fprintf(ff_dm,"%5d|%-11d|%-*.*s|%-*.*s|%-*.*s|",
  ++nomer_pp_dm,
  dragm->sp_in.ravno(nom_in),
  iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(10,model.ravno()),iceb_u_kolbait(10,model.ravno()),model.ravno(),
  iceb_u_kolbait(10,zav_nomer.ravno()),iceb_u_kolbait(10,zav_nomer.ravno()),zav_nomer.ravno());
  for(int nom_dm=0; nom_dm < kolih_dm; nom_dm++)
   {
    if(dragm->ves_dm.ravno(nom_dm) == 0.)
     continue;
    fprintf(ff_dm,"%10.10g|",dragm->ves_hax.ravno(kolih_dm*nom_in+nom_dm));
   }
  fprintf(ff_dm,"\n");
  
 }

fprintf(ff_dm,"%s\n",liniq.ravno());
fprintf(ff_dm,"%*s|",iceb_u_kolbait(70,gettext("Итого")),gettext("Итого"));
for(int ii=0; ii < kolih_dm; ii++)
 {
  if(dragm->ves_dm.ravno(ii) == 0.)
     continue;
  fprintf(ff_dm,"%10.10g|",dragm->ves_dm.ravno(ii));
 }
fprintf(ff_dm,"\n%s\n",liniq.ravno());

iceb_podpis(ff_dm,wpredok);
fclose(ff_dm);

return(0);

}

/******************/
/*Шапка распечатки*/
/******************/

void	uosinsap(int *kollist,int *kolst,FILE *ff)
{

*kollist+=1;
*kolst+=5;
fprintf(ff,"%65s %s N%d\n"," ",gettext("Лист"),*kollist);
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
Nп-п|Инвентарный|        Наименование          |Год |Дата ввода|  Модель  |   Бухучет     | Факт. наличие |    Разница    |\n\
    |  номер    |                              |вып.|в эксплуа.|          |Кол.|Стоимость |Кол.|Стоимость |Кол.|Стоимость |\n"));
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");
}
/***************/
/*Счетчик строк*/
/****************/
void  uosinvedsh(int *kolst,int *kollist,FILE *ff)
{

*kolst+=1;
if(*kolst >= kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kolst=0;
  uosinsap(kollist,kolst,ff);
 }
}
/**************************************/
/*Шапка для бюджетных организиций*/
/*********************************/

void uosinved_sbu(short dost,short most,short gost,int *kollist,int *kolst,FILE *ff)
{
*kollist+=1;
*kolst+=6;

fprintf(ff,"%80s %s N%d\n"," ",gettext("Лист"),*kollist);
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
 N  |        Наименование          |Год |            Номер             |По состоянию на %02d.%02d.%04dг.   |Отметка о  |\n\
з/п |                              |    |Инвентарный|заводсь- |паспорта| Факт. наличие |   Бухучет     |выбытии    |\n\
    |                              |    |           |  кий    |        |Кол.|Стоимость |Кол.|Стоимость |           |\n",
dost,most,gost);
//   123456789012345678901234567890 1234 12345678901 123456789 12345678 1234 1234567890 1234 1234567890
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------\n");

}
/***************/
/*Счетчик строк для бюджетной распечатки*/
/****************/
void  uosinvedsh_bu(short di,short mi,short gi,
int *kolst,int *kollist,FILE *ff)
{

*kolst+=1;
if(*kolst >= kol_strok_na_liste)
 {
  fprintf(ff,"\f");
  *kolst=1;
  uosinved_sbu(di,mi,gi,kollist,kolst,ff);
 }
}
/****************************************/
/*Первый лист бюджетного документа*/
/***********************************/

void uosinved_pl(short di,short mi,short gi,
const char *ikod,
int *kollist,
int *kolstr,
FILE *ffbu)
{
*kollist=*kolstr=0;

fprintf(ffbu,"\
                                                            Додаток 1\n\
                                                            до інструкції з інвентаризації матеріальних цінностей,\n\
                                                            розрахунків та інших статей балансу бюджених установ.\n");
fprintf(ffbu,"%s\n",iceb_get_pnk("00",0,NULL));
fprintf(ffbu,"Індентифікаційний код за ЄДРПОУ %s\n",ikod);
                                                                          
fprintf(ffbu,"                                        ІНВЕТАРИЗАЦІЙНИЙ ОПИС ОСНОВНИХ ЗАСОБІВ\n");
fprintf(ffbu,"                                                  %02d.%02d.%04d\n",di,mi,gi);
fprintf(ffbu,"Основні засоби_______________________________________________________________________________\n\n");
fprintf(ffbu,"Місце знаходження____________________________________________________________________________\n");

fprintf(ffbu,"\
                          РАСПИСКА\n\
 К началу проведения инвентаризации все расходные и приходные документы\n\
на ценности сданы в бухгалтерию и все ценности, поступившие на мою\n\
(нашу) ответственность, оприходованы, а выбывшие списаны в расход.\n\n");

fprintf(ffbu,"\
______________________________   _____________________________   _______________________________\n\
   (посада, підпис)                   (посада, підпис)               (посада, підпис)\n\n\
______________________________   _____________________________   _______________________________\n\
(прізвище, ім'я, по батькові)    (прізвище, ім'я, по батькові)   (прізвище, ім'я, по батькові)\n\n");

fprintf(ffbu,"На підставі (розпорядження) від \"____\"_____________________р.\n\n");
fprintf(ffbu,"Виконано знімання фактичних залишків__________________________________________\n\n");
fprintf(ffbu,"що значаться на обліку станом на \"____\"____________________р.\n\n");
fprintf(ffbu,"Інвентаризація: розпочата \"___\"____________________р.\n\n");
fprintf(ffbu,"                закінчена \"___\"____________________p.\n");

fprintf(ffbu,"\n\nПри інвентаризіції встановлено наступне:\n");
*kolstr+=32;
uosinved_sbu(di,mi,gi,kollist,kolstr,ffbu);

}
/*******************************/
/*Концовка отчёта для бюджета*/
/*******************************/
void uosinved_end_bu(FILE *ffbu)
{
fprintf(ffbu,"\
Разом за описом : а) кількість порядкових номерів____________________________\n\
                                                          (прописом)\n\
                  б) загальна кількість лдиниць, фактично____________________\n\
                                                          (прописом)\n\
                  в) на суму, грн., фактично_________________________________\n\
                                                          (прописом)\n\n");
fprintf(ffbu,"\
Голова комісії ______________________  ___________________  ______________________________\n\
                  (посада)                 (підпис)         (прізвище, ім'я, по батькові)\n\n\
Члени комісії: ______________________  ___________________  ______________________________\n\
                  (посада)                 (підпис)         (прізвище, ім'я, по батькові)\n\n\
               ______________________  ___________________  ______________________________\n\
                  (посада)                 (підпис)         (прізвище, ім'я, по батькові)\n\n\
              ______________________  ___________________  ______________________________\n\
                  (посада)                 (підпис)         (прізвище, ім'я, по батькові)\n\n\
 Усі цінності, пойменовані в цьому інвентаризаційному описі з N__________ до N___________,\n\
перевірені комісією в натурі за моєї (нашої) присутності та внесені в опис, у зв'язку з чим\n\
претензій до інвентаризаційної комісії не маю (не маємо). Цінності, перелічені в описі,\n\
знаходяться на моєму (нашому) відповідальному зберіганні.\n\n\
Особа(и), відповідальна(і) за збереження основних засобів:\n\n\
\"____\"_____________________р.\n\n\
____________________________________________________________________________________________\n\n\
 Вказані у данному описі дані превірив ____________________  ______________________\n\
                                             (посада)            (підпис)\n\n\
\"____\"_____________________р.\n");

}
/*****************************/
/*Шапка */
/******************************/
void uosinved_sap(short mi,short gi,int podr,int kodotl,int *kollist,int *kolst,FILE *ff,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str naipod("");
class iceb_u_str fiootv("");
SQL_str row;
class SQLCURSOR cur1;
if(kodotl != 0)
 {
  sprintf(strsql,"select naik from Uosol where kod=%d",kodotl);
  if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден мат.-ответственный"),kodotl);
    iceb_menu_soob(strsql,wpredok);
   }
  else 
   fiootv.new_plus(row[0]);
 }

if(podr != 0)
 {
  sprintf(strsql,"select naik from Uospod where kod=%d",podr);
  if(iceb_sql_readkey(strsql,&row,&cur1,wpredok) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),podr);
    iceb_menu_soob(strsql,wpredok);
   }
  else 
    naipod.new_plus(row[0]);
 }

fprintf(ff,"%s\n\n",iceb_get_pnk("00",0,wpredok));
fprintf(ff,gettext("ИНВЕНТАРИЗАЦИОННАЯ ОПИСЬ N_____ ОСНОВНЫХ СРЕДСТВ.\n"));
fprintf(ff,"%s %s\n",gettext("Подразделение"),naipod.ravno());
fprintf(ff,gettext("\
                          РАСПИСКА\n\
 К началу проведения инвентаризации все расходные и приходные документы\n\
на ценности сданы в бухгалтерию и все ценности, поступившие на мою\n\
(нашу) ответственность, оприходованы, а выбывшие списаны в расход.\n\n"));

fprintf(ff,"\n\
%s __________________________%s\n\n",
gettext("Материально-ответственное лицо (лица)"),
fiootv.ravno());

*kolst=12;

*kolst+=iceb_insfil("inventuos.alx",ff,wpredok);

char mesqc[64];
memset(mesqc,'\0',sizeof(mesqc));
iceb_mesc(mi,1,mesqc);

fprintf(ff,"\n\
%*s:%*s \"______\" %s %d%s\n\
%15s %*s \"______\" %s %d%s\n",
iceb_u_kolbait(15,gettext("Инвентаризация")),gettext("Инвентаризация"),
iceb_u_kolbait(10,gettext("начата")),gettext("начата"),
mesqc,gi,
gettext("г."),
" ",
iceb_u_kolbait(10,gettext("окончена")),gettext("окончена"),
mesqc,gi,
gettext("г."));

fprintf(ff,"\n%s:\n",gettext("При инвентаризации установлено следующее"));

*kolst+=5;

*kollist=0;
uosinsap(kollist,kolst,ff);

}
/**********************************/
/*Итого по счёту*/
/*******************************/

void uosinved_ips(short di,short mi,short gi,
const char *shetuz,
int *kolst,int *kollist,int *kolst_bu,int *kollist_bu,
int *shethiksh,
double *balstsh,
FILE *ff,FILE *ffbu)
{
char bros[512];
//Счетчик строк
uosinvedsh(kolst,kollist,ff);
fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");
sprintf(bros,"%s %s",gettext("Итого по счёту"),shetuz);

uosinvedsh(kolst,kollist,ff);
fprintf(ff,"%*s:%4d %10.2f\n",iceb_u_kolbait(74,bros),bros,*shethiksh,*balstsh);

uosinvedsh_bu(di,mi,gi,kolst_bu,kollist_bu,ffbu);

fprintf(ffbu,"\
--------------------------------------------------------------------------------------------------------------------\n");
sprintf(bros,"%s %s",gettext("Итого по счёту"),shetuz);

uosinvedsh_bu(di,mi,gi,kolst_bu,kollist_bu,ffbu);
fprintf(ffbu,"%*s:%4s %10s %4d %10.2f\n",iceb_u_kolbait(71,bros),bros,"","",*shethiksh,*balstsh);
*balstsh=0.;
*shethiksh=0;
}

/*******************************/
/*концовка распечатки*/
/*****************************/
void uosinved_konec(short mi,short gi,const char *shetuz,int *shethik,double *itogo,int *shethiksh,double *balstsh,FILE *ff,FILE *ffbu)
{
char bros[512];

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");

sprintf(bros,"%s %s",gettext("Итого по счёту"),shetuz);
fprintf(ff,"%*s:%4d %10.2f\n",iceb_u_kolbait(74,bros),bros,*shethiksh,*balstsh);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"%*s:%4d %10.2f\n",iceb_u_kolbait(74,gettext("Итого")),gettext("Итого"),*shethik,*itogo);


fprintf(ffbu,"\
--------------------------------------------------------------------------------------------------------------------\n");
sprintf(bros,"%s %s",gettext("Итого по счёту"),shetuz);
fprintf(ffbu,"%*s:%4s %10s %4d %10.2f\n",iceb_u_kolbait(71,bros),bros,"","",*shethiksh,*balstsh);

fprintf(ffbu,"\
--------------------------------------------------------------------------------------------------------------------\n");
fprintf(ffbu,"%*s:%4s %10s %4d %10.2f\n",iceb_u_kolbait(71,gettext("Итого")),gettext("Итого"),"","",*shethik,*itogo);

class iceb_u_str mesqc("");
iceb_mesc(mi,1,&mesqc);

koncinvw(mesqc.ravno(),gi,ff);

uosinved_end_bu(ffbu);
*balstsh=0.;
*shethik=0;
*shethiksh=0;
*itogo=0.;
}
/*************************/
/*Чтение записей по драг.металлам*/
/*********************************/
/*Если вернули 0-драг.металлы есть 1-нет*/
int uosinved_dm(int innom,class iceb_u_spisok *dm_ei,class iceb_u_double *ves,GtkWidget *wpredok)
{
int kolih=0;
class SQLCURSOR cur;
SQL_str row;
char strsql[512];
sprintf(strsql,"select kod,ei,ves from Uosindm where innom=%d",innom);

if((kolih=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
if(kolih == 0)
 return(1);
int nomer=0;
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s",row[0],row[1]);
  if((nomer=dm_ei->find(strsql)) < 0)
   {
    iceb_menu_soob("Не найдена нужная запись в списке!!!",wpredok);
    continue;
   }
  ves->plus(atof(row[2]),nomer);
 }
return(0);
}
/**********************************/
/*печать шапки страницы инвентаризационной ведомости для бюдженых организаций Наказ 572 от 17.06.2015р.*/
/******************************/
void uosinved_h_572(int noml,int *kolstr,FILE *ff,GtkWidget *wpredok)
{
 
fprintf(ff,"%145s %s:%d\n","",gettext("Страница"),noml);

int kz=iceb_insfil("invuos572h.alx",ff,wpredok);

if(kolstr != NULL)
 *kolstr=*kolstr+kz+1;

}
/*********************************************************************/
/*Счётчик инвентаризационной ведомости с итогами на каждой странице*/
/********************************************************************/
int uosinved_sh_572(int *kolstr,int *kolih_uos,double *itogo,const char *naim,int metka_fn,FILE *ff,GtkWidget *wpredok)
{
int kol=2;
if(metka_fn == 0)
 kol=1;
 
*kolstr+=kol;

int kds=0; /*количество дополнительных строк*/
for(int nom=30; nom < iceb_u_strlen(naim);nom+=30)
  kds++;

*kolstr+=kds;

//printf("%s %d %d\n",__FUNCTION__,*kolstr,kol_strok_na_liste_l-3);  

if(*kolstr <= kol_strok_na_liste_l-3)
 return(0);

usoinved_itog572(gettext("Итого"),*kolih_uos,itogo,metka_fn,ff);

fprintf(ff,"Кількість порядкових номерів:%s\n",iceb_u_nofwords(*kolih_uos));
fprintf(ff,"Кількість одиниць:%s\n",iceb_u_nofwords(*kolih_uos));

*kolih_uos=0;
itogo[0]=itogo[1]=0.;

fprintf(ff,"\f");

*kolstr=iceb_insfil("invuos572h.alx",ff,wpredok)+kol+kds;

return(1);
}


/******************************************/
/******************************************/

gint uosinvedw_r1(class uosinvedw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_clock sss(data->window);
SQL_str row,row1;
class SQLCURSOR cur;
printf("%s - %d\n",__FUNCTION__,kol_strok_na_liste_l);

char imaf[64];
memset(imaf,'\0',sizeof(imaf));
short di,mi,gi;

iceb_u_rsdat(&di,&mi,&gi,data->rk->datak.ravno(),1);

class SQLCURSOR cur1;


//читаем код
class iceb_u_str naim00("");
class iceb_u_str ikod("");
sprintf(strsql,"select naikon,kod from Kontragent where kodkon='00'");
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 {
  naim00.new_plus(row[0]);
  ikod.new_plus(row[1]);
 }
class drag_met dragm;

int kolih_dm=0;
/*Читаем списко драг.металлов*/
sprintf(strsql,"select distinct kod,ei from Uosindm");
if((kolih_dm=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s",row[0],row[1]);
  dragm.dm_ei.plus(strsql);
 }
if(kolih_dm > 0)
 dragm.ves_dm.make_class(kolih_dm);


int kolstr=0;
sprintf(strsql,"select innom,god,naim,model,pasp,zavnom,datvv from Uosin order by innom asc");
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

class iceb_tmptab tabtmp;
const char *imatmptab={"uosinvedw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
kmo int not null,\
shu char(32) not null,\
innom int not null,\
god smallint not null,\
naim char(112) not null,\
pod int not null,\
model char(112) not null,\
pasp char(112) not null,\
zavnom char(56) not null,\
datvv DATE not null,\
kla float(5,1) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  

class iceb_u_str naim("");
class iceb_u_str model("");
class iceb_u_str zav_nomer("");
class iceb_u_str nom_pasp("");
float kolstr1=0;
int innom=0;
int poddz=0,kodotl=0;
float kol_let_amort=0; /*количество лет амортизации*/
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6]);
  
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  if(iceb_u_proverka(data->rk->innom.ravno(),row[0],0,0) != 0)
   continue;
  innom=atoi(row[0]);

  if(poiinw(innom,di,mi,gi,&poddz,&kodotl,data->window) != 0) //Произошел расход или оно пришло позже даты
    continue;
    
  if(iceb_u_proverka(data->rk->podr.ravno(),poddz,0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->mat_ot.ravno(),kodotl,0,0) != 0)
    continue;

  class poiinpdw_data rekin;
  poiinpdw(innom,mi,gi,&rekin,data->window);

  if(iceb_u_proverka(data->rk->shetu.ravno(),rekin.shetu.ravno(),0,0) != 0)
    continue;

  kol_let_amort=0;

  if(data->rk->metka_bal_st == 1) /*налоговый учёт*/
   {
    /*Узнаём годовой процент амортизации*/
    sprintf(strsql,"select kof from Uosgrup where kod='%s'",rekin.hna.ravno());
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      if(atof(row1[0]) != 0.)
       kol_let_amort=100/atof(row1[0])*rekin.popkf;
      else 
       kol_let_amort=0.;
     }               
   }
  else /*бухгалтерский учёт*/
   {
    /*Узнаём годовой процент амортизации*/
    sprintf(strsql,"select kof from Uosgrup1 where kod='%s'",rekin.hnaby.ravno());
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      if(atof(row1[0]) != 0.)
       kol_let_amort=100/atof(row1[0])*rekin.popkfby;
      else
       kol_let_amort=0.;
     }               
   }
  dragm.sp_in.plus(row[0]);
  
  naim.new_plus(row[2]);
  model.new_plus(row[3]);
  nom_pasp.new_plus(row[4]);
  zav_nomer.new_plus(row[5]);
  
  sprintf(strsql,"insert into %s values (%d,'%s',%s,%s,'%s',%d,'%s','%s','%s','%s',%.1f)",
  imatmptab,
  kodotl,rekin.shetu.ravno(),row[0],row[1],naim.ravno_filtr(),poddz,model.ravno_filtr(),nom_pasp.ravno_filtr(),zav_nomer.ravno_filtr(),row[6],kol_let_amort);
  
  iceb_sql_zapis(strsql,1,0,data->window);    

 }

if(kolih_dm > 0)
 {
  dragm.ves_hax.make_class(kolih_dm*dragm.sp_in.kolih());
  dragm.ves_dmv.make_class(dragm.sp_in.kolih());
 } 

sprintf(strsql,"select * from %s order by kmo asc,shu asc,innom asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_spisok sp_shet;
class iceb_u_double sum_shet;
class iceb_u_double kol_shet;

class iceb_u_spisok sp_shet_i;
class iceb_u_double sum_shet_i;
class iceb_u_double kol_shet_i;
class iceb_u_str shetuz("");
class iceb_u_str shetu("");
short dvv,mvv,gvv;
int kod_otv_z=-1; //может равняться нулю
int nomer_sheta=0;
kolstr1=0.;
double balst=0.;
class iceb_u_str god("");
int		shethik=0,shethiksh=0;
double itogo=0.;
double balstsh=0.;
int		kolst=0,kollist=0;
class iceb_u_str naimshet("");
class iceb_u_str fio_mo("");


char imafbu[64];

sprintf(imafbu,"invb%d.lst",getpid());
FILE *ffbu;
if((ffbu = fopen(imafbu,"w")) == NULL)
 {
  iceb_er_op_fil(imafbu,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int kollist_bu=0;
int kolst_bu=0;

sprintf(imaf,"inv%d.lst",getpid());
FILE *ff;
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_ipsh[64];
FILE *ffipsh;
sprintf(imaf_ipsh,"dod%d.lst",getpid());
if((ffipsh = fopen(imaf_ipsh,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_ipsh,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

in_d12_startw(ffipsh);
char imaf_572[64];
sprintf(imaf_572,"io572_%d.lst",getpid());

class iceb_fopen fil_572;/*инвентаризационная опись наказ Міністерства фінансів України від 17.06.2015 N 572*/
if(fil_572.start(imaf_572,"w",data->window) != 0)
 return(1);

char imaf_572s[64];
sprintf(imaf_572s,"io572s_%d.lst",getpid());

class iceb_fopen fil_572s;/*инвентаризационная опись наказ Міністерства фінансів України від 17.06.2015 N 572*/
if(fil_572s.start(imaf_572s,"w",data->window) != 0)
 return(1);


double itogo_572[2];
memset(itogo_572,'\0',sizeof(itogo_572));
double itogo_572l[2];
memset(itogo_572l,'\0',sizeof(itogo_572l));
int nom_zap572=0;
double iznos=0.;

kolstr1=0;
int kolstr_572s=0;
int kolih_uos_list=0;
class iceb_u_str suma_prop("");
class iceb_u_str dolg("");
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  kodotl=atoi(row[0]);
  shetu.new_plus(row[1]);
  innom=atoi(row[2]);
  god.new_plus(row[3]);
  naim.new_plus(row[4]);
  poddz=atoi(row[5]);
  model.new_plus(row[6]);
  nom_pasp.new_plus(row[7]);
  zav_nomer.new_plus(row[8]);
  iceb_u_rsdat(&dvv,&mvv,&gvv,row[9],2);

  if(kod_otv_z != kodotl)
   {
    if(kod_otv_z != -1)
     {
      uosinved_konec(mi,gi,shetuz.ravno(),&shethik,&itogo,&shethiksh,&balstsh,ff,ffbu);


      usoinved_itog572(gettext("Итого"),kolih_uos_list,itogo_572l,data->rk->metka_fn,fil_572s.ff);
      usoinved_itog572(gettext("Общий итог"),nom_zap572,itogo_572,data->rk->metka_fn,fil_572s.ff);
      usoinved_itog572(gettext("Итого"),nom_zap572,itogo_572,data->rk->metka_fn,fil_572.ff);

      fprintf(fil_572s.ff,"Кількість порядкових номерів:%s\n",iceb_u_nofwords(kolih_uos_list));
      fprintf(fil_572s.ff,"Кількість одиниць:%s\n",iceb_u_nofwords(kolih_uos_list));
      kolih_uos_list=0;

            
      /*вставляем концовку ведомости*/

      fprintf(fil_572.ff,"ICEB_LST_END\n");

      uosinved_rps(&sp_shet,&kol_shet,&sum_shet,fil_572s.ff);
      uosinved_rps(&sp_shet,&kol_shet,&sum_shet,fil_572.ff);

      sp_shet.free_class();
      kol_shet.free_class();
      sum_shet.free_class();

      class iceb_vstavfil_rk rhd;
      rhd.zap(1,1,49,160,iceb_u_nofwords(nom_zap572));
      rhd.zap(7,1,80,160,iceb_u_nofwords(nom_zap572));
      iceb_u_preobr(itogo_572[0],&suma_prop,0);
      rhd.zap(9,1,62,160,suma_prop.ravno());
      rhd.zap(25,1,30,60,dolg.ravno());
      rhd.zap(25,1,97,160,fio_mo.ravno());

      iceb_vstavfil(&rhd,"invuos572k.alx",fil_572.ff,data->window);
      iceb_vstavfil(&rhd,"invuos572k.alx",fil_572s.ff,data->window);

      fprintf(ff,"\f");
      fprintf(ffbu,"\f");
      fprintf(fil_572.ff,"\fICEB_LST_START\n");
      nom_zap572=0;
      memset(itogo_572,'\0',sizeof(itogo_572));
      memset(itogo_572l,'\0',sizeof(itogo_572l));
      
     }
    uosinved_pl(di,mi,gi,ikod.ravno(),&kollist_bu,&kolst_bu,ffbu);
    uosinved_sap(mi,gi,poddz,kodotl,&kollist,&kolst,ff,data->window);

    sprintf(strsql,"select naik,dolg from Uosol where kod=%d",kodotl);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
     {
      fio_mo.new_plus(row1[0]);
      dolg.new_plus(row1[1]);
     }
    else
     {
      fio_mo.new_plus("");
      dolg.new_plus("");
     }     

    class iceb_vstavfil_rk rhd;
    rhd.zap(1,1,0,100,naim00.ravno());
    rhd.zap(4,2,32,50,ikod.ravno());
    rhd.zap(29,1,2,60,dolg.ravno());
    rhd.zap(29,1,107,140,fio_mo.ravno());

    /*вставляем заголовок файла*/
    iceb_vstavfil(&rhd,"invuos572z.alx",fil_572.ff,data->window);
    iceb_vstavfil(&rhd,"invuos572z.alx",fil_572s.ff,data->window);

    fprintf(fil_572.ff,"\fICEB_LST_START\n"); /*заголовок и шапка страницы не помещаются на одном листе*/
    uosinved_h_572(1,NULL,fil_572.ff,data->window);

    fprintf(fil_572s.ff,"\f"); /*заголовок и шапка страницы не помещаются на одном листе*/
    kolstr_572s=iceb_insfil("invuos572h.alx",fil_572s.ff,data->window);
    
    kod_otv_z=kodotl;
    
   }

  if(iceb_u_SRAV(shetu.ravno(),shetuz.ravno(),0) != 0)
   {
    if(shetuz.ravno()[0] != '\0')
     {
      if(balstsh != 0.)
        uosinved_ips(di,mi,gi,shetuz.ravno(),&kolst,&kollist,&kolst_bu,&kollist_bu,&shethiksh,&balstsh,ff,ffbu);

     }
    sprintf(strsql,"select nais from Plansh where ns='%s'",shetu.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) > 0)
      naimshet.new_plus(row1[0]);
    else
      naimshet.new_plus("");

    //Счетчик строк
    uosinvedsh(&kolst,&kollist,ff);
    fprintf(ff,"%s:%s %s\n",gettext("Счёт"),shetu.ravno(),naimshet.ravno());

    uosinvedsh_bu(di,mi,gi,&kolst_bu,&kollist_bu,ffbu);
    fprintf(ffbu,"%s:%s %s\n",gettext("Счёт"),shetu.ravno(),naimshet.ravno());
  
    shethiksh=0;
    balstsh=0.;
    shetuz.new_plus(shetu.ravno());
   }

  class bsizw_data bal_st;
  bsizw(innom,poddz,di,mi,gi,&bal_st,NULL,data->window);

  if(data->rk->metka_bal_st == 1)
   {
    balst=bal_st.bs+bal_st.sbs;
    iznos=bal_st.siz+bal_st.iz+bal_st.iz1;
   }
  if(data->rk->metka_bal_st == 0)
   {
    balst=bal_st.bsby+bal_st.sbsby;
    iznos=bal_st.sizby+bal_st.izby+bal_st.iz1by;
   }
//  if(balst == 0.)
//    continue;

  //Счетчик строк
  uosinvedsh(&kolst,&kollist,ff);

  fprintf(ff,"%4d %-11d %-*.*s %4s %02d.%02d.%04d %-*.*s %4s %10.2f\n",
  ++shethik,innom,
  iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
  god.ravno(),dvv,mvv,gvv,
  iceb_u_kolbait(10,model.ravno()),iceb_u_kolbait(10,model.ravno()),model.ravno(),
  "1",balst);

  uosinvedsh_bu(di,mi,gi,&kolst_bu,&kollist_bu,ffbu);

  fprintf(ffbu,"%4d %-*.*s %4s %-11d %-*.*s %-*.*s %4s %10s %4s %10.2f\n",
  shethik,
  iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
  god.ravno(),
  innom,
  iceb_u_kolbait(9,zav_nomer.ravno()),iceb_u_kolbait(9,zav_nomer.ravno()),zav_nomer.ravno(),
  iceb_u_kolbait(8,nom_pasp.ravno()),iceb_u_kolbait(8,nom_pasp.ravno()),nom_pasp.ravno(),
  "","","1",balst);


  uosinved_sh_572(&kolstr_572s,&kolih_uos_list,itogo_572l,naim.ravno(),data->rk->metka_fn,fil_572s.ff,data->window); /*считаем по 2 строки*/
  nom_zap572++;
  kolih_uos_list++;  

  if(data->rk->metka_fn == 1)
   fprintf(fil_572s.ff,"%6d|%-*.*s|%-10s|%-14d|%-*.*s|%-*.*s|%-*.*s|%4s|%13s|%6s|%4d|%13.2f|%12.2f|%12.2f|%6.6s|\n",
   nom_zap572,
   iceb_u_kolbait(30,naim.ravno()),
   iceb_u_kolbait(30,naim.ravno()),
   naim.ravno(),
   god.ravno(),
   innom,
   iceb_u_kolbait(10,zav_nomer.ravno()),
   iceb_u_kolbait(10,zav_nomer.ravno()),
   zav_nomer.ravno(),
   iceb_u_kolbait(8,nom_pasp.ravno()),
   iceb_u_kolbait(8,nom_pasp.ravno()),
   nom_pasp.ravno(),
   iceb_u_kolbait(5,gettext("Шт")),
   iceb_u_kolbait(5,gettext("Шт")),
   "Шт",
   "",
   "",
   "",
   1,
   balst,
   iznos,
   balst-iznos,
   row[10]);   
  else
   fprintf(fil_572s.ff,"%6d|%-*.*s|%-10s|%-14d|%-*.*s|%-*.*s|%-*.*s|%4s|%13.13g|%6s|%4d|%13.2f|%12.2f|%12.2f|%6.6s|\n",
   nom_zap572,
   iceb_u_kolbait(30,naim.ravno()),
   iceb_u_kolbait(30,naim.ravno()),
   naim.ravno(),
   god.ravno(),
   innom,
   iceb_u_kolbait(10,zav_nomer.ravno()),
   iceb_u_kolbait(10,zav_nomer.ravno()),
   zav_nomer.ravno(),
   iceb_u_kolbait(8,nom_pasp.ravno()),
   iceb_u_kolbait(8,nom_pasp.ravno()),
   nom_pasp.ravno(),
   iceb_u_kolbait(5,gettext("Шт")),
   iceb_u_kolbait(5,gettext("Шт")),
   "Шт",
   "1",
   balst,
   "",
   1,
   balst,
   iznos,
   balst-iznos,
   row[10]);   
   
  for(int nom=30; nom < iceb_u_strlen(naim.ravno()); nom+=30)
    {
     fprintf(fil_572s.ff,"%6s|%-*.*s|%10s|%14s|%10s|%8s|%5s|%4s|%13s|%6s|%4s|%13s|%12s|%12s|%6s|\n",
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
     "",
     "",
     "",
     "",
     "",
     "");
    }    
  if(data->rk->metka_fn == 1)
   {
    for(int nom=0; nom < 173;nom++)
     fprintf(fil_572s.ff,".");
    fprintf(fil_572s.ff,"\n");
   }
   
  if(data->rk->metka_fn == 1)   
   fprintf(fil_572.ff,"%6d|%-*.*s|%-10s|%-14d|%-*.*s|%-*.*s|%-*.*s|%4s|%13s|%6s|%4d|%13.2f|%12.2f|%12.2f|\n",
   nom_zap572,
   iceb_u_kolbait(30,naim.ravno()),
   iceb_u_kolbait(30,naim.ravno()),
   naim.ravno(),
   god.ravno(),
   innom,
   iceb_u_kolbait(10,zav_nomer.ravno()),
   iceb_u_kolbait(10,zav_nomer.ravno()),
   zav_nomer.ravno(),
   iceb_u_kolbait(8,nom_pasp.ravno()),
   iceb_u_kolbait(8,nom_pasp.ravno()),
   nom_pasp.ravno(),
   iceb_u_kolbait(5,gettext("Шт")),
   iceb_u_kolbait(5,gettext("Шт")),
   "Шт",
   "",
   "",
   "",
   1,
   balst,
   iznos,
   balst-iznos);   
  else
   fprintf(fil_572.ff,"%6d|%-*.*s|%-10s|%-14d|%-*.*s|%-*.*s|%-*.*s|%4s|%13.13g|%6s|%4d|%13.2f|%12.2f|%12.2f|\n",
   nom_zap572,
   iceb_u_kolbait(30,naim.ravno()),
   iceb_u_kolbait(30,naim.ravno()),
   naim.ravno(),
   god.ravno(),
   innom,
   iceb_u_kolbait(10,zav_nomer.ravno()),
   iceb_u_kolbait(10,zav_nomer.ravno()),
   zav_nomer.ravno(),
   iceb_u_kolbait(8,nom_pasp.ravno()),
   iceb_u_kolbait(8,nom_pasp.ravno()),
   nom_pasp.ravno(),
   iceb_u_kolbait(5,gettext("Шт")),
   iceb_u_kolbait(5,gettext("Шт")),
   "Шт",
   "1",
   balst,
   "",
   1,
   balst,
   iznos,
   balst-iznos);   
  
  for(int nom=30; nom < iceb_u_strlen(naim.ravno()); nom+=30)
   fprintf(fil_572.ff,"%6s|%-*.*s|%10s|%14s|%10s|%8s|%5s|%4s|%13s|%6s|%4s|%13s|%12s|%12s|%6s|\n",
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
   "",
   "",
   "",
   "",
   "",
   "");
  if(data->rk->metka_fn == 1)
   {
    for(int nom=0; nom < 173;nom++)
      fprintf(fil_572.ff,".");
    fprintf(fil_572.ff,"\n");
   }
  if(iceb_u_strlen(naim.ravno()) > 30)
   {
    //Счетчик строк
    uosinvedsh(&kolst,&kollist,ff);
    fprintf(ff,"%4s %-11s %s\n"," "," ",iceb_u_adrsimv(30,naim.ravno()));

    uosinvedsh_bu(di,mi,gi,&kolst_bu,&kollist_bu,ffbu);
    fprintf(ffbu,"%4s %s\n","",iceb_u_adrsimv(30,naim.ravno()));

   }
  if(kolih_dm > 0)
   {
    class iceb_u_double ves;
    ves.make_class(kolih_dm);
    if(uosinved_dm(innom,&dragm.dm_ei,&ves,data->window) == 0)
     {

      int nomer_os=dragm.sp_in.find(innom);      
      for(int ii=0; ii < kolih_dm; ii++)
       {
        dragm.ves_dm.plus(ves.ravno(ii),ii);
        dragm.ves_hax.plus(ves.ravno(ii),kolih_dm*nomer_os+ii);
        dragm.ves_dmv.plus(ves.ravno(ii),nomer_os);
       }

     }    
   }
  if((nomer_sheta=sp_shet.find(shetu.ravno())) < 0)
   sp_shet.plus(shetu.ravno());
  kol_shet.plus(1,nomer_sheta);
  sum_shet.plus(balst,nomer_sheta);

  if((nomer_sheta=sp_shet_i.find(shetu.ravno())) < 0)
   sp_shet_i.plus(shetu.ravno());
  kol_shet_i.plus(1,nomer_sheta);
  sum_shet_i.plus(balst,nomer_sheta);

  itogo_572[0]+=balst;
  itogo_572[1]+=iznos;
  itogo_572l[0]+=balst;
  itogo_572l[1]+=iznos;
  
  itogo+=balst;
  balstsh+=balst;
  shethiksh++;
 }
fprintf(fil_572.ff,"ICEB_LST_END\n");
uosinved_konec(mi,gi,shetuz.ravno(),&shethik,&itogo,&shethiksh,&balstsh,ff,ffbu);

/*Распечатка формы додаток 12*/
in_d12_itw(&sp_shet_i,&kol_shet_i,&sum_shet_i,ffipsh,data->window);


iceb_insfil("in_d12_end.alx",ffipsh,data->window);

iceb_podpis(ff,data->window);
iceb_podpis(ffbu,data->window);
iceb_podpis(ffipsh,data->window);

fclose(ff);
fclose(ffbu);
fclose(ffipsh);

usoinved_itog572(gettext("Итого"),kolih_uos_list,itogo_572l,data->rk->metka_fn,fil_572s.ff);
usoinved_itog572(gettext("Общий итог"),nom_zap572,itogo_572,data->rk->metka_fn,fil_572s.ff);
usoinved_itog572(gettext("Итого"),nom_zap572,itogo_572,data->rk->metka_fn,fil_572.ff);

fprintf(fil_572s.ff,"Кількість порядкових номерів:%s\n",iceb_u_nofwords(kolih_uos_list));
fprintf(fil_572s.ff,"Кількість одиниць:%s\n",iceb_u_nofwords(kolih_uos_list));

      

uosinved_rps(&sp_shet,&kol_shet,&sum_shet,fil_572s.ff);
uosinved_rps(&sp_shet,&kol_shet,&sum_shet,fil_572.ff);

class iceb_vstavfil_rk rhd;
rhd.zap(1,1,49,160,iceb_u_nofwords(nom_zap572));
rhd.zap(7,1,80,160,iceb_u_nofwords(nom_zap572));
iceb_u_preobr(itogo_572[0],&suma_prop,0);
rhd.zap(9,1,62,160,suma_prop.ravno());
rhd.zap(25,1,30,60,dolg.ravno());
rhd.zap(25,1,97,160,fio_mo.ravno());

iceb_vstavfil(&rhd,"invuos572k.alx",fil_572.ff,data->window);
iceb_vstavfil(&rhd,"invuos572k.alx",fil_572s.ff,data->window);

iceb_podpis(fil_572.ff,data->window);
iceb_podpis(fil_572s.ff,data->window);

fil_572.end();
fil_572s.end();

class iceb_u_str imaf_dragm;

if(kolih_dm > 0)
  uosinved_rdm(di,mi,gi,&dragm,&imaf_dragm,data->window);

data->oth->spis_imaf.plus(imaf);
data->oth->spis_naim.plus(gettext("Инвентаризационная опись (форма 1)"));

data->oth->spis_imaf.plus(imafbu);
data->oth->spis_naim.plus(gettext("Инвентаризационная опись (форма 2)"));

data->oth->spis_imaf.plus(imaf_ipsh);
data->oth->spis_naim.plus(gettext("Дополнение 12"));

if(kolih_dm > 0)
 {
  data->oth->spis_imaf.plus(imaf_dragm.ravno());
  data->oth->spis_naim.plus(gettext("Инвентаризационная опись драг. металлов"));
 }

data->oth->spis_imaf.plus(imaf_572s);
data->oth->spis_naim.plus(gettext("Инвентаризационная опись бюджетных организаций с итогами"));

for(int nom=0; nom < data->oth->spis_imaf.kolih(); nom++)
 iceb_ustpeh(data->oth->spis_imaf.ravno(nom),3,data->window);


data->oth->spis_imaf.plus(imaf_572);
data->oth->spis_naim.plus(gettext("Инвентаризационная опись бюджетных организаций"));
iceb_ustpeh(imaf_572,3,&kolstr,data->window);
iceb_rnl(imaf_572,kolstr,"",&uosinved_h_572,data->window);



gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
