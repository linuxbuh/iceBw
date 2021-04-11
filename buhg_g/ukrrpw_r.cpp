/*$Id: ukrrpw_r.c,v 1.17 2013/09/26 09:46:56 sasa Exp $*/
/*18.05.2018	01.01.2008	Белых А.И.	ukrrpw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "ukrrpw.h"

class ukrrpw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class ukrrpw_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  ukrrpw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   ukrrpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrrpw_r_data *data);
gint ukrrpw_r1(class ukrrpw_r_data *data);
void  ukrrpw_r_v_knopka(GtkWidget *widget,class ukrrpw_r_data *data);


extern SQL_baza bd;




extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int ukrrpw_r(class ukrrpw_data *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class ukrrpw_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Реестр проводок по видам операций"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(ukrrpw_r_key_press),&data);

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

repl.plus(gettext("Реестр проводок по видам операций"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(), iceb_get_pnk("00",0,data.window));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(ukrrpw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);

gtk_text_buffer_create_tag(data.buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);

//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)ukrrpw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  ukrrpw_r_v_knopka(GtkWidget *widget,class ukrrpw_r_data *data)
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

gboolean   ukrrpw_r_key_press(GtkWidget *widget,GdkEventKey *event,class ukrrpw_r_data *data)
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
/****************************************/
/*  Шапка документа                     */
/****************************************/
void	saprppvo(
short dn,short mn,short gn,
short dk,short mk,short gk,
int *nlist,int *kolst,
FILE *ff)
{
fprintf(ff,"\
%s %02d.%02d.%4d%s %s %02d.%02d.%4d%s %30s%sN%d\n",
gettext("Период с"),
dn,mn,gn,
gettext("г."),
gettext("по"),
dk,mk,gk,
gettext("г."),
"",gettext("Лист"),
*nlist);

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
	Контрагент                              |Дата док. |Дата пров.|N документа|Nнал.н|  Счета   |       Сумма        |Комментарий\n"));

/*
123456789012345678901234567890123456789012345678 1234567890 1234567890 12345678901 123456 1234567890 12345678901234567890
*/

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------\n");
*nlist+=1;
*kolst+=4;
}

/***************************************/
/*Счетчик строк                        */
/***************************************/
void	shetrppvo(short dn,short mn,short gn,
short dk,short mk,short gk,
int *nlist,int *kolst,FILE *ff)
{
*kolst+=1;
if(*kolst < kol_strok_na_liste)
  return; 

fprintf(ff,"\f");
*kolst=1;
saprppvo(dn,mn,gn,dk,mk,gk,nlist,kolst,ff);

}

/**************************************/
/*Распечатка итога                    */
/**************************************/

void	itrppvo(int metka, //0-по контрагенту 1-по виду командировки 2-общий итог
const char	*kod,
short dn,short mn,short gn,
short dk,short mk,short gk,
int *nlist,int *kolst,
class iceb_u_spisok *PARSH,class iceb_u_double *kontr,FILE *ff)
{
SQLCURSOR curr;
double itog=0.;
int	kolih=PARSH->kolih();
class iceb_u_str naim("");
char	strsql[1024];
SQL_str row;
double	suma=0;
class iceb_u_str naimitog("");


if(kolih > 0)
 {
  if(metka == 0)
   {
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kod);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      naim.new_plus(row[0]);    
    naimitog.new_plus(gettext("Итого по контрагенту"));
   }     
  if(metka == 1)
   {
    sprintf(strsql,"select naik from Ukrvkr where kod='%s'",kod);
    if(sql_readkey(&bd,strsql,&row,&curr) == 1)
      naim.new_plus(row[0]);    
    naimitog.new_plus(gettext("Итого по виду командировки"));
   }
  if(metka == 2)
    naimitog.new_plus(gettext("Общий итог"));

  shetrppvo(dn,mn,gn,dk,mk,gk,nlist,kolst,ff);
  fprintf(ff,"%s %s:\n",naimitog.ravno(),naim.ravno());
 }
/*printw("kso- %d\n",kso);*/

for(int i=0; i< kolih ;i++)
 {
  suma=kontr->ravno(i);
  if(suma == 0.)
   continue;

  shetrppvo(dn,mn,gn,dk,mk,gk,nlist,kolst,ff);
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(20,PARSH->ravno(i)),PARSH->ravno(i), iceb_u_prnbr(suma));
  itog+=suma;
 }
if(itog != 0.)
 {
  shetrppvo(dn,mn,gn,dk,mk,gk,nlist,kolst,ff);
  fprintf(ff,"%-*s - %20s\n",iceb_u_kolbait(10,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(itog));

  shetrppvo(dn,mn,gn,dk,mk,gk,nlist,kolst,ff);
  fprintf(ff,"\
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n");
 }
kontr->clear_class();
}
/****************************************/
/*Итог по документа                     */
/****************************************/
void	itdok(double itogd,FILE *ff)
{
fprintf(ff,"%*s %20s\n",iceb_u_kolbait(100,gettext("Итого по документу:")),gettext("Итого по документу:"),iceb_u_prnbr(itogd));
}

/******************************************/
/******************************************/

gint ukrrpw_r1(class ukrrpw_r_data *data)
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

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");


int kolstr=0;

sprintf(strsql,"select datp,sh,shk,nomd,deb,datd,komen,oper from Prov \
where val=0 and \
datp >= '%04d-%02d-%02d' and \
datp <= '%04d-%02d-%02d' and kto='%s' and deb != 0.",
gn,mn,dn,gk,mk,dk,ICEB_MP_UKR);


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
/*Сортировка через временную таблицу делается так как сортировать нужно по коду контрагента
а он может отсутствовать в проводке*/

class iceb_tmptab tabtmp;
const char *imatmptab={"ukrrpw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
datp DATE not null,\
sh char(32) not null,\
shk char(32) not null,\
nomd char(32) not null,\
deb double(14,2) not null,\
datd DATE not null,\
komen char(255) not null,\
kodkon char(32) not null,\
oper char(32) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  

float kolstr1=0.;
class iceb_u_str vkom("");
class iceb_u_str kontr("");
SQL_str row1;
class SQLCURSOR curr;

short	dd,md,gd; //Дата документа
short	dp,mp,gp; //Дата проводки
class iceb_u_str nomdok("");
class iceb_u_str shet("");
class iceb_u_str shetk("");
double	deb=0.;
class iceb_u_str koment("");
int 	nlist=1;
int	kolst=0;
int	kom=0;


while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->shetu.ravno(),row[1],0,0) != 0)
    continue;
   
  sprintf(strsql,"select kont,vkom from Ukrdok where datd='%s' and \
nomd='%s'",row[5],row[3]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
   {
    sprintf(strsql,"Не найден документ N %s Дата=%s\n",
    row[3],row[5]);
    
    repl.new_plus(strsql);
    
    sprintf(strsql,"%s %s %s %s",gettext("Проводка"),
    row[1],row[2],row[3]);
    repl.plus(strsql);
    
    iceb_menu_soob(&repl,data->window);
    continue;
   }
  kontr.new_plus(row1[0]);
  vkom.new_plus(row1[1]);

  if(iceb_u_proverka(data->rk->kontr.ravno(),kontr.ravno(),0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->vidkom.ravno(),vkom.ravno(),0,0) != 0)
    continue;

  sprintf(strsql,"insert into %s values ('%s','%s','%s','%s',%s,'%s','%s','%s','%s')",
  imatmptab,
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],kontr.ravno(),vkom.ravno());  

  iceb_sql_zapis(strsql,1,0,data->window);    


 }


sprintf(strsql,"select * from %s order by oper asc,kodkon asc,nomd asc",imatmptab);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


char imaf[64];
FILE *ff;
sprintf(imaf,"rpkr%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


if(data->rk->kontr.ravno()[0] != '\0')
 { 
  fprintf(ff,"%s: %s\n",gettext("Контрагент"),data->rk->kontr.ravno());
  kolst++;
 }

if(data->rk->vidkom.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Вид командировки"),data->rk->vidkom.ravno());
  kolst++;
 }

if(data->rk->shetu.ravno()[0] != '\0')
 {
  fprintf(ff,"%s: %s\n",gettext("Счёт"),data->rk->shetu.ravno());
  kolst++;
 }


class iceb_u_spisok PARSH; //Список пар счетов
class iceb_u_double oper; //Суммы по операциям
class iceb_u_double kont; //Суммы по конрагентам
class iceb_u_double itog; //Суммы по всему расчёту

class iceb_u_str vkom1("");
class iceb_u_str kontr1("");
class iceb_u_str shett("");
class iceb_u_str naikont("");
class iceb_u_str nomdok1("");
class iceb_u_str naivkom("");
double itogd=0.;

kolstr1=0.;

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  iceb_u_rsdat(&dp,&mp,&gp,row[0],2);

  shet.new_plus(row[1]);
  shetk.new_plus(row[2]);
  nomdok.new_plus(row[3]);
  deb=atof(row[4]);
  iceb_u_rsdat(&dd,&md,&gd,row[5],2);
  koment.new_plus(row[6]);

  kontr.new_plus(row[7]);

  vkom.new_plus(row[8]);

  if(iceb_u_SRAV(nomdok.ravno(),nomdok1.ravno(),0) != 0)
   {
    if(nomdok1.getdlinna() > 1)
     {
      shetrppvo(dn,mn,gn,dk,mk,gk,&nlist,&kolst,ff);
      itdok(itogd,ff);
     }
    nomdok1.new_plus(nomdok.ravno());
    itogd=0.;
   }

  if(iceb_u_SRAV(kontr.ravno(),kontr1.ravno(),0) != 0)
   {
    if(kontr1.ravno()[0] != '\0')
      itrppvo(0,kontr1.ravno(),dn,mn,gn,dk,mk,gk,&nlist,&kolst,&PARSH,&kont,ff);

    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
      naikont.new_plus(row1[0]);    
    else
      naikont.new_plus("");
    kom=1;
    kontr1.new_plus(kontr.ravno());
   }

  if(iceb_u_SRAV(vkom.ravno(),vkom1.ravno(),0) != 0)
   {
    if(vkom1.ravno()[0] != '\0')
     {
      itrppvo(1,vkom1.ravno(),dn,mn,gn,dk,mk,gk,&nlist,&kolst,&PARSH,&oper,ff);
      fprintf(ff,"\f");
      kolst=0;
     }    

    iceb_u_zagolov(gettext("Реестр проводок по виду командировок"),dn,mn,gn,dk,mk,gk, iceb_get_pnk("00",0,data->window),ff);
    kolst+=5;

    sprintf(strsql,"select naik from Ukrvkr where kod='%s'",vkom.ravno());
    if(sql_readkey(&bd,strsql,&row1,&curr) == 1)
     naivkom.new_plus(row1[0]);   
    else
     naivkom.new_plus("");
    fprintf(ff,"%s:%s %s\n",gettext("Вид командировки"),vkom.ravno(),naivkom.ravno());
    kolst++;

    saprppvo(dn,mn,gn,dk,mk,gk,&nlist,&kolst,ff);
    kom=1;
    vkom1.new_plus(vkom.ravno());
   }

  shett.new_plus(shet.ravno());
  shett.plus(",",shetk.ravno());
  int i=0;
  if((i=PARSH.find(shett.ravno())) == -1)
    PARSH.plus(shett.ravno());

  oper.plus(deb,i);
  kont.plus(deb,i);
  itog.plus(deb,i);
  itogd+=deb;

  sprintf(strsql,"%02d.%02d.%4d %02d.%02d.%4d %-*s %-*s %15.2f %d\n",
  dp,mp,gp,dd,md,gd,
  iceb_u_kolbait(11,nomdok.ravno()),nomdok.ravno(),
  iceb_u_kolbait(15,shett.ravno()),shett.ravno(),
  deb,kom);

  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

  shetrppvo(dn,mn,gn,dk,mk,gk,&nlist,&kolst,ff);
  if(kom == 1)
    {
     fprintf(ff,"%-*s %-*.*s %02d.%02d.%4d %02d.%02d.%4d %-*s %-6s %-*s %20s %s\n",
     iceb_u_kolbait(7,kontr.ravno()),kontr.ravno(),
     iceb_u_kolbait(40,naikont.ravno()),iceb_u_kolbait(40,naikont.ravno()),naikont.ravno(),
     dd,md,gd,dp,mp,gp,
     iceb_u_kolbait(11,nomdok.ravno()),nomdok.ravno(),
     "",
     iceb_u_kolbait(10,shett.ravno()),shett.ravno(),
     iceb_u_prnbr(deb),koment.ravno());
   }

  if(kom == 0)
   {
    fprintf(ff,"%48s %02d.%02d.%4d %02d.%02d.%4d %-*s %-6s %-*s %20s %s\n",
    " ",dd,md,gd,dp,mp,gp,
    iceb_u_kolbait(11,nomdok.ravno()),nomdok.ravno(),
    "",
    iceb_u_kolbait(10,shett.ravno()),shett.ravno(),
    iceb_u_prnbr(deb),koment.ravno());
   }
  kom=0;
   
 }

itdok(itogd,ff);
itrppvo(0,kontr1.ravno(),dn,mn,gn,dk,mk,gk,&nlist,&kolst,&PARSH,&kont,ff);
itrppvo(1,vkom1.ravno(),dn,mn,gn,dk,mk,gk,&nlist,&kolst,&PARSH,&oper,ff);
itrppvo(2,"",dn,mn,gn,dk,mk,gk,&nlist,&kolst,&PARSH,&itog,ff);

iceb_podpis(ff,data->window);

fclose(ff);

data->rk->imaf.plus(imaf);
data->rk->naim.plus(gettext("Реестр проводок по видам командировок"));
iceb_ustpeh(imaf,1,data->window);

for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);
 
gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
