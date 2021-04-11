/*$Id: dvtmcf2_r.c,v 1.27 2013/09/26 09:46:47 sasa Exp $*/
/*23.05.2016	22.11.2004	Белых А.И.	dvtmcf2_r.c
Расчёт движения товарно-материальных ценностей форма 2
*/
#include <math.h>
#include <errno.h>
#include "buhg_g.h"
#include "dvtmcf2.h"

class dvtmcf2_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class dvtmcf2_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  dvtmcf2_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   dvtmcf2_r_key_press(GtkWidget *widget,GdkEventKey *event,class dvtmcf2_r_data *data);
gint dvtmcf2_r1(class dvtmcf2_r_data *data);
void  dvtmcf2_r_v_knopka(GtkWidget *widget,class dvtmcf2_r_data *data);


extern SQL_baza bd;


extern int      kol_strok_na_liste;
extern int      kol_strok_na_liste_l;
extern double okrg1;

int dvtmcf2_r(class dvtmcf2_rr *datark,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
char strsql[512];
iceb_u_str repl;
class dvtmcf2_r_data data;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать движение товарно-материальных ценностей (форма 1)"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(dvtmcf2_r_key_press),&data);

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

repl.plus(gettext("Распечатать движение товарно-материальных ценностей (форма 1)"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,data.window));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(dvtmcf2_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)dvtmcf2_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  dvtmcf2_r_v_knopka(GtkWidget *widget,class dvtmcf2_r_data *data)
{
//printf("dvtmcf2_r_v_knopka\n");
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   dvtmcf2_r_key_press(GtkWidget *widget,GdkEventKey *event,class dvtmcf2_r_data *data)
{
// printf("dvtmcf2_r_key_press\n");
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}


/*********************/
/*вывод реквизитов поиска*/
/*************************/
void dvtmcf2_vrp(class dvtmcf2_rr *rp,int *kolstr,FILE *ff)
{

if(rp->pr == 1)
 {
  fprintf(ff,"%s\n",gettext("Только приходные документы"));
  *kolstr+=1;
 }
if(rp->pr == 2)
 {
  fprintf(ff,"%s\n",gettext("Только расходные документы"));
  *kolstr+=1;
 }

if(rp->metka_cena == 0)
 {
  fprintf(ff,"%s\n",gettext("Отчёт по учётным ценам"));
  *kolstr+=1;
 }
if(rp->metka_cena == 1)
 {
  fprintf(ff,"%s\n",gettext("Отчёт в ценах реализации"));
  *kolstr+=1;
 }
if(rp->sklad.getdlinna() > 1)
 {
   fprintf(ff,"%s:%s\n",gettext("Склад"),rp->sklad.ravno());
  *kolstr+=1;
 }
if(rp->kontr.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код контрагента"),rp->kontr.ravno());
  *kolstr+=1;
 }

if(rp->kodop.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Код операции"),rp->kodop.ravno());
  *kolstr+=1;
 }
if(rp->grupa.getdlinna() > 1)
 {
  fprintf(ff,"%s:%s\n",gettext("Группа"),rp->grupa.ravno());
  *kolstr+=1;
 }
if(rp->kodmat.ravno()[0] != 0)
 {
  fprintf(ff,"%s:%s\n",gettext("Материал"),rp->kodmat.ravno());
  *kolstr+=1;
 }
/**************
if(rp->naborop.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Набор операций"),rp->naborop.ravno());
  *kolstr+=1;
 }
***************/
if(rp->shet.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Счета учета"),rp->shet.ravno());
  *kolstr+=1;
 }

if(rp->nomdok.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("Документ"),rp->nomdok.ravno());
  *kolstr+=1;
 }
if(rp->nds.ravno()[0] != '\0')
 {
  fprintf(ff,"%s:%s\n",gettext("НДС"),rp->nds.ravno());
  *kolstr+=1;
 }
}
/**************************************/
/*Вывод шапки*/
/***************************/
void dvtmcf2_sap(int *sl, //Счетчик листов
int *kst, //Счетчик строк
FILE *ff)
{
*sl+=1;

fprintf(ff,"%*s N%d\n",iceb_u_kolbait(170,gettext("Лист")),gettext("Лист"),*sl);

 fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,gettext("\
 Код  |        Наименование               |   Номенкла-   |Един.|              |            |               |               | Д а т а  | Д а т а  | Код   |\n\
      |    товара или материалла          | турный номер  |изме-|   Ц е н а    | Количество |     Сумма     |Номер документа| выписки  |подтверж- |опера- |         К о н т р а г е н т\n\
      |                                   |               |рения|              |            |               |               |документа | дения    | ции   |\n"));

  fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
  *kst+=7; /*больше на 1*/

}


/*******/
/*Шапка*/
/*******/
void gsapp1(int *sl, //Счётчик листов
int *kst, //Счётчик строк
FILE *ff,
int metka_l) //0-портрет 1-ландшафт
{
*kst+=1;

if(metka_l == 0)
 if(*kst <= kol_strok_na_liste)
  return;
if(metka_l == 1)
 if(*kst <= kol_strok_na_liste_l)
  return;

fprintf(ff,"\f");
*kst=1;
 

dvtmcf2_sap(sl,kst,ff);

}

/*******************/
/*Распечатка итогов*/
/********************/

void rasitogo(int metka,//  1-итого по группе
                                   //  2-итого по контр
                                   //  3-итого по склад
                                   //  4-итого по счёту
double mas[2],int *klst,const char *kod,
FILE *ff,short metoth,
int *sli,int metka_l,
class dvtmcf2_r_data *data)
{
char		strsql[1024];
class iceb_u_str naim("");
SQL_str         row;
SQLCURSOR       cur;
class iceb_u_str bros("");

if(mas[0] == 0.)
  return;

if(metoth == 1 && (metka == 1 || metka == 4))
  return;
if(metoth == 2 && metka == 2)
  return;


if(metka == 1)
 {
  sprintf(strsql,"select naik from Grup where kod=%s",kod);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !\n",gettext("Не найдена группа"),kod);
    iceb_printw(strsql,data->buffer,data->view);
   }
  else
   naim.new_plus(row[0]);
  sprintf(strsql,"%s %s %s",gettext("Итого по группе"),kod,naim.ravno());
 }

if(metka == 2)
 {
  if(kod[0] == '0' && iceb_u_polen(kod,&bros,2,'-') == 0)
   {
    sprintf(strsql,"select naik from Sklad where kod=%s",bros.ravno());
   }
  else
   {
    bros.new_plus(kod);
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kod);
   }

  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !\n",gettext("Не найден контрагент"),bros.ravno());
    iceb_printw(strsql,data->buffer,data->view);
   }
  else
   naim.new_plus(row[0]);
  sprintf(strsql,"%s %s %s",gettext("Итого по контрагенту"),kod,naim.ravno());
 }
if(metka == 3)
 {
  sprintf(strsql,"select naik from Sklad where kod=%s",kod);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %d %s!\n",gettext("Не найден склад"),atoi(kod),gettext("в списке складов"));
    iceb_printw(strsql,data->buffer,data->view);
   }
  else
   naim.new_plus(row[0]);
  sprintf(strsql,"%s %s %s",gettext("Итого по складу"),kod,naim.ravno());
 }

if(metka == 4)
 {
  sprintf(strsql,"select nais from Plansh where ns='%s'",kod);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s %s!\n",gettext("Не найден счёт"),kod,gettext("в плане счетов"));
    iceb_printw(strsql,data->buffer,data->view);
   }
  else
   naim.new_plus(row[0]);
  sprintf(strsql,"%s %s %s",gettext("Итого по счёту"),kod,naim.ravno());
 }

gsapp1(sli,klst,ff,metka_l);
fprintf(ff,"%*s:%12.12g %15.2f\n",iceb_u_kolbait(79,strsql),strsql,mas[0],mas[1]);

mas[0]=0.;
mas[1]=0.;

}                        

/*******************/
/*Распечатка отчёта*/
/*******************/

void  dvtmcf2ot(const char *imatmptab,class dvtmcf2_r_data *data,FILE *ff,FILE *ff_l)
{
class iceb_u_str bros("");
int		kodm;
short           dp,mp,gp;
short           d,m,g;
double		its,itk,br;
class iceb_u_str nomdok("");
class iceb_u_str poi("");
int		i;
double		itgrup[2];
double		itkontr[2];
double		itsklad[2];
double		itshet[2];
short           mvstr; /*Метка выдачи строки*/
int             sli=0,kst=0;
int             sli_l=0,kst_l=0;
char		strsql[2048];
double		kolih,cena;
class iceb_u_str shu(""),shuz("");
short		kgrm,kgrmz;
int             skl,skll,nk;
class iceb_u_str kop("");
class iceb_u_str kor(""),korzz("");
class iceb_u_str naim("");
char            nomn[128];
SQL_str         row1;
SQLCURSOR       cur1;
class iceb_u_str org("");
class iceb_u_str ei("");
float kolstr1=0.;
class iceb_u_str shetsp("");
its=itk=0;


if(data->rk->metka_sort == 0)/*Сортировка Счёт-склад-контрагент-группа-дата подтверждения*/ 
 {
  sprintf(strsql,"\n%s\n",gettext("Сортировка:Счёт-склад-контрагент-группа-дата подтверждения"));
  iceb_printw(strsql,data->buffer,data->view);
  sprintf(strsql,"select * from %s order by sh asc,skl asc,kor asc,kgm asc,dp asc",imatmptab);
 }

if(data->rk->metka_sort == 1) /*Сортировка склад-контрагент-дата подтверждения*/ 
 {
  sprintf(strsql,"\n%s\n",gettext("Сортировка:Cклад-контрагент-дата подтверждения"));
  iceb_printw(strsql,data->buffer,data->view);
  sprintf(strsql,"select * from %s order by skl asc,kor asc,dp asc",imatmptab);
 }

if(data->rk->metka_sort == 2) /*Сортировка:Счёт-cклад-группа*/
 {
  sprintf(strsql,"\n%s\n",gettext("Сортировка:Счёт-cклад-группа"));
  iceb_printw(strsql,data->buffer,data->view);
  sprintf(strsql,"select * from %s order by sh asc,skl asc,kgm asc",imatmptab);
 }

if(data->rk->metka_sort == 3) /*Сортировка:контрагент-наименование материала*/
 {
  sprintf(strsql,"\n%s\n",gettext("Сортировка:Контрагент-наименование материала"));
  iceb_printw(strsql,data->buffer,data->view);

  sprintf(strsql,"select * from %s order by kor asc,naim asc",imatmptab);
 }

int kolstr;
SQL_str row;
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  return;
 }


iceb_zagolov(gettext("Движение товарно-материальных ценностей"),data->rk->datan.ravno(),data->rk->datak.ravno(),ff,data->window);

iceb_zagolov(gettext("Движение товарно-материальных ценностей"),data->rk->datan.ravno(),data->rk->datak.ravno(),ff_l,data->window);
kst_l=kst=4;

dvtmcf2_vrp(data->rk,&kst,ff);

dvtmcf2_vrp(data->rk,&kst_l,ff_l);

//шапка документа
dvtmcf2_sap(&sli,&kst,ff);
dvtmcf2_sap(&sli_l,&kst_l,ff_l);

for(i=0; i < 2 ; i++)
 itgrup[i]=itkontr[i]=itsklad[i]=itshet[i]=0.;
  
kgrmz=skl=0;
mvstr=skll=0;
kolstr1=0;
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,kolstr1++);  
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  shu.new_plus(row[0]);
  skl=atoi(row[1]);
  kor.new_plus(row[2]);
  kgrm=atoi(row[3]);
  nk=atoi(row[13]);

  if(kgrmz != kgrm)
   {
    if(kgrmz != 0 && (data->rk->metka_sort == 0 || data->rk->metka_sort == 2))
     {
      bros.new_plus(kgrmz);
      rasitogo(1,itgrup,&kst,bros.ravno(),ff,data->rk->metka_sort,&sli,0,data);
      rasitogo(1,itgrup,&kst_l,bros.ravno(),ff_l,data->rk->metka_sort,&sli_l,1,data);
     }
    kgrmz=kgrm;
   }

  if(iceb_u_SRAV(korzz.ravno(),kor.ravno(),0) != 0)
   {
    if(korzz.ravno()[0] != '\0')
     {
      rasitogo(2,itkontr,&kst,korzz.ravno(),ff,data->rk->metka_sort,&sli,0,data);
      rasitogo(2,itkontr,&kst_l,korzz.ravno(),ff_l,data->rk->metka_sort,&sli_l,1,data);
     }
    korzz.new_plus(kor.ravno());
   }     

  if(skll != skl)
   {
    if(skll != 0)
     {
      bros.new_plus(kgrmz);
      rasitogo(1,itgrup,&kst,bros.ravno(),ff,data->rk->metka_sort,&sli,0,data);
      rasitogo(1,itgrup,&kst_l,bros.ravno(),ff_l,data->rk->metka_sort,&sli_l,1,data);

      rasitogo(2,itkontr,&kst,korzz.ravno(),ff,data->rk->metka_sort,&sli,0,data);
      rasitogo(2,itkontr,&kst_l,korzz.ravno(),ff_l,data->rk->metka_sort,&sli_l,1,data);

      bros.new_plus(skll);
      rasitogo(3,itsklad,&kst,bros.ravno(),ff,data->rk->metka_sort,&sli,0,data);
      rasitogo(3,itsklad,&kst_l,bros.ravno(),ff_l,data->rk->metka_sort,&sli_l,1,data);
                  
     } 
    skll=skl;
   } 
  if(iceb_u_SRAV(shuz.ravno(),shu.ravno(),0) != 0 )
   {
    if(shuz.ravno()[0] != '\0' && (data->rk->metka_sort == 0 || data->rk->metka_sort == 2))
     {
      bros.new_plus(kgrmz);
      rasitogo(1,itgrup,&kst,bros.ravno(),ff,data->rk->metka_sort,&sli,0,data);
      rasitogo(1,itgrup,&kst_l,bros.ravno(),ff_l,data->rk->metka_sort,&sli_l,1,data);

      rasitogo(2,itkontr,&kst,korzz.ravno(),ff,data->rk->metka_sort,&sli,0,data);
      rasitogo(2,itkontr,&kst_l,korzz.ravno(),ff_l,data->rk->metka_sort,&sli_l,1,data);

      bros.new_plus(skll);
      rasitogo(3,itsklad,&kst,bros.ravno(),ff,data->rk->metka_sort,&sli,0,data);
      rasitogo(3,itsklad,&kst_l,bros.ravno(),ff_l,data->rk->metka_sort,&sli_l,1,data);

      rasitogo(4,itshet,&kst,shuz.ravno(),ff,data->rk->metka_sort,&sli,0,data);
      rasitogo(4,itshet,&kst_l,shuz.ravno(),ff_l,data->rk->metka_sort,&sli_l,1,data);
     }
    shuz.new_plus(shu.ravno());
   }

  naim.new_plus(row[5]);
  kor.new_plus(row[2]);
  kodm=atoi(row[6]);
  ei.new_plus(row[7]);
  iceb_u_rsdat(&dp,&mp,&gp,row[4],2);
  iceb_u_rsdat(&d,&m,&g,row[10],2);
  kolih=atof(row[11]);  
  shetsp.new_plus(row[15]);
  kop.new_plus(row[8]);

  if(atoi(row[12]) == 1)
     kop.plus("+");
  if(atoi(row[12]) == 2)
     kop.plus("-");

  nomdok.new_plus(row[14]); 


  if(data->rk->metka_cena == 0) /*по учётным ценам*/
   {
    sprintf(strsql,"select cena from Kart where sklad=%d and nomk=%d",skl,nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      sprintf(strsql,"%s! %s %d %s %d\n",gettext("Не нашли карточку"),gettext("Склад"),skl,gettext("Номер карточки"),nk);
      iceb_printw(strsql,data->buffer,data->view);

      fprintf(ff,"%s",strsql);
      fprintf(ff_l,"%s",strsql);
      continue;
     }
   cena=atof(row1[0]);
   }
  else /*по ценам в документе*/
   {
    float pnds_dok=0.;
    sprintf(strsql,"select pn from Dokummat where datd='%s' and sklad=%d and nomd='%s'",row[4],skl,nomdok.ravno_filtr());
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
     {
      sprintf(strsql,"%s-%s\n%s=%s %s=%s %s=%d\n",__FUNCTION__,gettext("Не найден документ !"),
      gettext("Дата"),row[4],gettext("Номер документа"),row[2],gettext("Склад"),skl);
      iceb_printw(strsql,data->buffer,data->view);
      

      fprintf(ff,"%s",strsql);
      fprintf(ff_l,"%s",strsql);
      
      continue;
     }
    
    pnds_dok=atof(row1[0]);

    cena=0.;
    //Читаем цену в документе
    sprintf(strsql,"select cena from Dokummat1 where datd='%s' and sklad=%d and \
nomd='%s' and nomkar=%d",row[4],skl,nomdok.ravno_filtr(),nk);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
      cena=atof(row1[0]);
    short mnds=0;
    short d,m,g;
    iceb_u_rsdat(&d,&m,&g,row[4],2);

    //читаем метку НДС
    sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' and \
sklad=%d and nomerz=11",g,nomdok.ravno_filtr(),skl);

    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
      mnds=atoi(row1[0]);

    if((mnds == 0 || mnds == 4) && pnds_dok != 0. )
     {
      cena=cena+cena*pnds_dok/100.;    
      cena=iceb_u_okrug(cena,okrg1);
     }
   }



       
  mvstr=0;
  itgrup[0]+=kolih;  
  itkontr[0]+=kolih;
  itsklad[0]+=kolih;
  itshet[0]+=kolih;
  itk+=kolih;

  br=cena*kolih;
  br=iceb_u_okrug(br,0.01);
  itgrup[1]+=br;  
  itkontr[1]+=br;
  itsklad[1]+=br;
  itshet[1]+=br;
  its+=br;
  
  /*Узнаем наименование контрагента*/
  if(kor.ravno()[0] == '0' && iceb_u_polen(kor.ravno(),&bros,2,'-') == 0)
   {
    poi.new_plus(bros.ravno());   
    sprintf(strsql,"select naik from Sklad where kod=%s",poi.ravno());
   }
  else
   {
    poi.new_plus(kor.ravno());   
    sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",
    poi.ravno());
   }
  org.new_plus("");
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
     fprintf(ff,"%s %s !\n",gettext("Не найден код контрагента"),kor.ravno());
     fprintf(ff_l,"%s %s !\n",gettext("Не найден код контрагента"),kor.ravno());

     sprintf(strsql,"%s %s %s !\n%s %d %s %d %s %s\n",
     gettext("Не найден код контрагента"),kor.ravno(),poi.ravno(),
     gettext("Склад"),skl,
     gettext("Номер карточки"),
     nk,
     gettext("Номер документа"),
     nomdok.ravno());
     
     iceb_printw(strsql,data->buffer,data->view);

     fprintf(ff,"%s",strsql);
     fprintf(ff_l,"%s",strsql);
   }
  else
    org.new_plus(row1[0]);

 
  gsapp1(&sli,&kst,ff,0);
  gsapp1(&sli_l,&kst_l,ff_l,1);
  if(mvstr == 0)
   {
    sprintf(nomn,"%d.%s.%d.%d",skl,shu.ravno(),kgrm,nk);
     
    fprintf(ff,"\
%-6d %-*.*s %-*s %-*s %14.10g %12.12g %15.2f %-15s %02d.%02d.%d %02d.%02d.%d %-*s %s %s %s\n",
    kodm,
    iceb_u_kolbait(35,naim.ravno()),iceb_u_kolbait(35,naim.ravno()),naim.ravno(),
    iceb_u_kolbait(15,nomn),nomn,
    iceb_u_kolbait(5,ei.ravno()),ei.ravno(),
    cena,kolih,br,nomdok.ravno(),d,m,g,dp,mp,gp,
    iceb_u_kolbait(7,kop.ravno()),
    kop.ravno(),
    kor.ravno(),org.ravno(),shetsp.ravno());

    fprintf(ff_l,"\
%-6d %-*.*s %-*s %-*s %14.10g %12.12g %15.2f %-15s %02d.%02d.%d %02d.%02d.%d %-*s %s %s %s\n",
    kodm,
    iceb_u_kolbait(35,naim.ravno()),
    iceb_u_kolbait(35,naim.ravno()),
    naim.ravno(),
    iceb_u_kolbait(15,nomn),
    nomn,
    iceb_u_kolbait(5,ei.ravno()),
    ei.ravno(),
    cena,kolih,br,nomdok.ravno(),d,m,g,dp,mp,gp,
    iceb_u_kolbait(7,kop.ravno()),
    kop.ravno(),
    kor.ravno(),org.ravno(),shetsp.ravno());

    if(iceb_u_strlen(naim.ravno()) > 37)
     {
      gsapp1(&sli,&kst,ff,0);
      fprintf(ff,"%4s %s\n"," ",iceb_u_adrsimv(37,naim.ravno()));
      gsapp1(&sli_l,&kst_l,ff_l,1);
      fprintf(ff_l,"%4s %s\n"," ",iceb_u_adrsimv(37,naim.ravno()));
     }
   }
  else
   {
    fprintf(ff,"\
%6s %-35s %-15s %-5s %14.10g %12.12g %15.2f %-15s %02d.%02d.%d %02d.%02d.%d %-*s %s %s %s\n",
    " "," "," "," ",
    cena,kolih,br,nomdok.ravno(),d,m,g,dp,mp,gp,
    iceb_u_kolbait(7,kop.ravno()),
    kop.ravno(),
    kor.ravno(),org.ravno(),shetsp.ravno());
    
    fprintf(ff_l,"\
%6s %-35s %-15s %-5s %14.10g %12.12g %15.2f %-15s %02d.%02d.%d %02d.%02d.%d %-*s %s %s %s\n",
    " "," "," "," ",
    cena,kolih,br,nomdok.ravno(),d,m,g,dp,mp,gp,
    iceb_u_kolbait(7,kop.ravno()),
    kop.ravno(),
    kor.ravno(),org.ravno(),shetsp.ravno());
   }  
  
  mvstr++;

 }


bros.new_plus(kgrmz);
rasitogo(1,itgrup,&kst,bros.ravno(),ff,data->rk->metka_sort,&sli,0,data);
rasitogo(1,itgrup,&kst_l,bros.ravno(),ff_l,data->rk->metka_sort,&sli_l,1,data);

rasitogo(2,itkontr,&kst,korzz.ravno(),ff,data->rk->metka_sort,&sli,0,data);
rasitogo(2,itkontr,&kst_l,korzz.ravno(),ff_l,data->rk->metka_sort,&sli_l,1,data);

bros.new_plus(skll);
rasitogo(3,itsklad,&kst,bros.ravno(),ff,data->rk->metka_sort,&sli,0,data);
rasitogo(3,itsklad,&kst_l,bros.ravno(),ff_l,data->rk->metka_sort,&sli_l,1,data);

rasitogo(4,itshet,&kst,shuz.ravno(),ff,data->rk->metka_sort,&sli,0,data);
rasitogo(4,itshet,&kst_l,shuz.ravno(),ff_l,data->rk->metka_sort,&sli_l,1,data);

fprintf(ff,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%*s:%12.12g %15.2f\n",iceb_u_kolbait(77,gettext("Общий итог")),gettext("Общий итог"),itk,its);

fprintf(ff_l,"\
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\
%77s:%12.12g %15.2f\n",gettext("Общий итог"),itk,its);


sprintf(strsql,"%s:\n\
%-*s %12.12g\n\
%-*s %12.2f%s\n",
gettext("Итого"),
iceb_u_kolbait(10,gettext("количество")),
gettext("количество"),
itk,
iceb_u_kolbait(10,gettext("сумма")),
gettext("сумма"),
its,
gettext("грн.")); 

iceb_printw(strsql,data->buffer,data->view);

}

/*********************************************/
/*Создание массивов счетов и групп материалов*/
/*********************************************/
void	sozshgr(iceb_u_int *GRUP, //Массив количества групп
iceb_u_spisok *SHET,        //Список счетов
int *kolgrup, //Количество групп
int *kolshet, //Количество счетов
class dvtmcf2_r_data *data)
{
SQL_str		row;
char		strsql[512];


sprintf(strsql,"select kod from Grup");

SQLCURSOR cur;
if((*kolgrup=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  GRUP->plus(atoi(row[0]),-1);
 }

sprintf(strsql,"select distinct shetu from Kart");

if((*kolshet=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  return;
 }

while(cur.read_cursor(&row) != 0)
 {
  SHET->plus(row[0]);
 }

}
/******************************************/
/*Распечатка массива группа-счет          */
/******************************************/

void rasgrsh(char *imaf,
class dvtmcf2_r_data *data,
short dn,short mn,short gn,
short dk,short mk,short gk,
class iceb_u_int *GRUP, //Массив количества групп
class iceb_u_spisok *SHET, //Список счетов
int kolgrup, //Количество групп
int kolshet, //Количество счетов
class iceb_u_double *sumshgr)
{
int	i,i1,i2;
int	nomgrup;
class iceb_u_str shet("");
char	strsql[512];
SQL_str	row;
SQLCURSOR cur;
class iceb_u_str naimgrup("");
class iceb_u_str stroka("");
double	itogkol=0.;
double  itogo=0.;

class iceb_fopen fil;
if(fil.start(imaf,"w",data->window) != 0)
 return;

iceb_zagolov(gettext("Движение товарно-материалных ценностей по группам и счетам"),dn,mn,gn,dk,mk,gk,fil.ff,data->window);

dvtmcf2_vrp(data->rk,&i1,fil.ff);

stroka.new_plus("-----------------------------------"); //Строка на наименованием

for(i1=0; i1 < kolshet+1;i1++)
 {
  itogkol=0.;
  for(i=0; i < kolgrup;i++)
   {
    itogkol+=sumshgr->ravno(i*kolshet+i1);
   }
  if(itogkol == 0.)
    continue;
  stroka.plus("-------------");
 }
fprintf(fil.ff,"%s\n",stroka.ravno());  

fprintf(fil.ff,"Код|  Наименование группы         |");

for(i1=0; i1 < kolshet;i1++)
 {
  itogkol=0.;
  for(i=0; i < kolgrup;i++)
   {
    itogkol+=sumshgr->ravno(i*kolshet+i1);
   }
  if(itogkol == 0.)
    continue;
  shet.new_plus(SHET->ravno(i1));
  fprintf(fil.ff,"%*s|",iceb_u_kolbait(12,shet.ravno()),shet.ravno());
 }
fprintf(fil.ff,"%*s|\n",iceb_u_kolbait(12,gettext("Итого")),gettext("Итого"));

fprintf(fil.ff,"%s\n",stroka.ravno());  

for(i=0; i < kolgrup; i++)
 {
  itogkol=0.;
  for(i1=0; i1 < kolshet;i1++)
      itogkol+=sumshgr->ravno(i*kolshet+i1);
  if(itogkol == 0.)
    continue;

  nomgrup=GRUP->ravno(i);  

  sprintf(strsql,"select naik from Grup where kod=%d",nomgrup);
  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    naimgrup.new_plus(row[0]);
  else
    naimgrup.new_plus("");
    
  fprintf(fil.ff,"%3d %-*.*s",nomgrup,iceb_u_kolbait(30,naimgrup.ravno()),iceb_u_kolbait(30,naimgrup.ravno()),naimgrup.ravno());    
  itogo=0.;
  for(i1=0; i1 < kolshet;i1++)
   {
    itogkol=0.;
    for(i2=0; i2 < kolgrup;i2++)
     {
      itogkol+=sumshgr->ravno(i2*kolshet+i1);
     }
    if(itogkol == 0.)
      continue;
    fprintf(fil.ff," %12.2f",sumshgr->ravno(i*kolshet+i1));
    itogo+=sumshgr->ravno(i*kolshet+i1);
   }

  fprintf(fil.ff," %12.2f\n",itogo);
 }

fprintf(fil.ff,"%s\n",stroka.ravno());  
fprintf(fil.ff,"%*s:",iceb_u_kolbait(33,gettext("Итого")),gettext("Итого"));
itogo=0.;
for(i1=0; i1 < kolshet; i1++)
 {
  itogkol=0.;
  for(i=0; i < kolgrup;i++)
   {
    itogkol+=sumshgr->ravno(i*kolshet+i1);
   }
  if(itogkol == 0.)
    continue;
  fprintf(fil.ff," %12.2f",itogkol);
  itogo+=itogkol;
 }

fprintf(fil.ff," %12.2f\n",itogo);

iceb_podpis(fil.ff,data->window);
fil.end();

}

/***********************************************/
/*Распечатка группа-счет-счет корреспондент    */
/***********************************************/
void rasgshshk(class dvtmcf2_r_data *data,
short dn,short mn,short gn,
short dk,short mk,short gk,
const char *imaf,
class iceb_u_spisok *GSHSHK,      //Список группа-счет-счет корреспондент
class iceb_u_double *SUMGSHSHK) //Суммы к списку  группа-счет-счет корреспондент
{
char	strsql[512];
SQL_str	row;
int	kolstr=0;
int	kolelem=0;
int	i=0;
short   metkavgr=0;
class iceb_u_str kod("");
class iceb_u_str shet("");
class iceb_u_str shetk("");
double	itoggr=0.;

class iceb_fopen fil;
if(fil.start(imaf,"w",data->window) != 0)
 return;

iceb_zagolov(gettext("Движение товарно-материалных ценностей по группам и счетам"),dn,mn,gn,dk,mk,gk,fil.ff,data->window);

dvtmcf2_vrp(data->rk,&i,fil.ff);

sprintf(strsql,"select kod,naik from Grup order by kod asc");

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,data->window);
  return;
 }
kolelem=GSHSHK->kolih();
while(cur.read_cursor(&row) != 0)
 {
  if(itoggr != 0)
   {
    fprintf(fil.ff,"%*s:%10.2f\n",iceb_u_kolbait(13,gettext("Итого")),gettext("Итого"),itoggr);
    itoggr=0.;
   }

  metkavgr=0;  
  for(i=0; i < kolelem; i++)
   {
    iceb_u_polen(GSHSHK->ravno(i),&kod,1,'#');
    if(iceb_u_SRAV(kod.ravno(),row[0],0) == 0)
     {
      if(metkavgr == 0)
       {
        fprintf(fil.ff,"\n%s %s\n",row[0],row[1]);

        fprintf(fil.ff,"\
------------------------------\n");
        fprintf(fil.ff,gettext("\
Дебет |Кредит|    Сумма      |\n"));
        fprintf(fil.ff,"\
------------------------------\n");

        metkavgr=1;
       }
      iceb_u_polen(GSHSHK->ravno(i),&shet,2,'#');
      iceb_u_polen(GSHSHK->ravno(i),&shetk,3,'#');
      if(shetk.ravno()[0] == '\0')
       shetk.new_plus("???");
      fprintf(fil.ff,"%-*s %-*s %10.2f\n",
      iceb_u_kolbait(6,shet.ravno()),shet.ravno(),
      iceb_u_kolbait(6,shetk.ravno()),shetk.ravno(),
      SUMGSHSHK->ravno(i));
      itoggr+=SUMGSHSHK->ravno(i);
     }            

   }  

 }

if(itoggr != 0)
  fprintf(fil.ff,"%*s:%10.2f\n",iceb_u_kolbait(13,gettext("Итого")),gettext("Итого"),itoggr);


iceb_podpis(fil.ff,data->window);
fil.end();

}




/******************************************/
/******************************************/

gint dvtmcf2_r1(class dvtmcf2_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[2048];
iceb_clock sss(data->window);




short dn,mn,gn;
short dk,mk,gk;
iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window);
SQLCURSOR cur;
SQLCURSOR cur1;
int kolstr;

sprintf(strsql,"select * from Zkart where datdp >= '%04d-%02d-%02d' and \
datdp <= '%04d-%02d-%02d' order by sklad,nomk,datdp asc",
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
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%s %d.%d.%d %s %d.%d.%d\n",
gettext("Период с"),
dn,mn,gn,
gettext("до"),
dk,mk,gk);

iceb_printw(strsql,data->buffer,data->view);


class iceb_u_int GRUP(0); //Массив количества групп
class iceb_u_spisok SHET(0);        //Список счетов
int	kolgrup=0;
int     kolshet=0;
class iceb_u_spisok GSHSHK(0);      //Список группа-счет-счет корреспондент
class iceb_u_double SUMGSHSHK(0); //Суммы к списку  группа-счет-счет корреспондент

//Создаем список счетов и массив групп
sozshgr(&GRUP,&SHET,&kolgrup,&kolshet,data);

class iceb_u_double sumshgr; //Массив сумм группа-счет
sumshgr.make_class(kolshet*kolgrup);

class iceb_tmptab tabtmp;
const char *imatmptab={"dvtmcf2"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
sh char(24) not null,\
skl int not null,\
kor char(24) not null,\
kgm int not null,\
dp DATE not null,\
naim char(112) not null,\
km int not null,\
ei char(24) not null,\
kop char(24) not null,\
cena double(15,6) not null,\
dv DATE not null,\
kol double(16,6) not null,\
tip int not null,\
nk int not null,\
nomd char(24) not null,\
shs char(24) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  return(1);
 }  

/*Открываем заранее, чтобы иметь возможность выгрузить ошибки*/
char imaf[64];
class iceb_fopen fil;
sprintf(imaf,"dv%d.lst",getpid());
if(fil.start(imaf,"w",data->window) != 0)
 return(1);

class iceb_fopen fil_l;
char imaf_l[64];
sprintf(imaf_l,"dv2l%d.lst",getpid());
if(fil_l.start(imaf_l,"w",data->window) != 0)
 return(1);

float kolstr1=0.;
float pnds_dok=0.;
class iceb_u_str nomdok("");
SQL_str row,row1;
int skl=0;
int nk=0;
class iceb_u_str kor("");
class iceb_u_str kop("");
int kodm=0;
class iceb_u_str shu("");
class iceb_u_str ei("");
double cena=0.;
int kgrm=0;
class iceb_u_str naim("");
class iceb_u_str shetsp("");
while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);  
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  skl=atoi(row[0]);
  nk=atoi(row[1]);
  nomdok.new_plus(row[2]);
  
  if(iceb_u_proverka(data->rk->sklad.ravno(),row[0],0,0) != 0)
    continue;  

  if(data->rk->pr != 0)
   if(data->rk->pr != atoi(row[5]))
       continue;

  if(iceb_u_proverka(data->rk->nomdok.ravno(),row[2],0,0) != 0)
    continue;  

  pnds_dok=0.;
  /*Читаем накладную*/
  sprintf(strsql,"select kontr,kodop,pn from Dokummat \
where datd='%s' and sklad=%d and nomd='%s'",
  row[4],skl,row[2]);
  
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s-%s\n%s-%s %s-%s %s-%d\n",
    __FUNCTION__,
    gettext("Не найден документ"),
    gettext("Дата"),
    row[4],
    gettext("Номер документа"),
    row[2],
    gettext("Склад"),
    skl);

    iceb_printw(strsql,data->buffer,data->view);
    
    fprintf(fil.ff,"%s",strsql);
    
    fprintf(fil_l.ff,"%s",strsql);
    
    continue;
   }
  
  kor.new_plus(row1[0]);
  kop.new_plus(row1[1]);
  pnds_dok=atof(row1[2]);
  
  if(iceb_u_proverka(data->rk->kontr.ravno(),kor.ravno(),0,0) != 0)
    continue;  

  if(iceb_u_proverka(data->rk->kodop.ravno(),kop.ravno(),0,0) != 0)
    continue;  

  /*Читаем карточку материалла*/
  sprintf(strsql,"select kodm,shetu,ei,cena,mnds,nds from Kart where sklad=%d and nomk=%d",skl,nk);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    sprintf(strsql,"%s! %s %d %s %d\n",
    gettext("Не нашли карточку"),
    gettext("Склад"),
    skl,
    gettext("Номер карточки"),
    nk);

    iceb_printw(strsql,data->buffer,data->view);

    fprintf(fil.ff,"%s",strsql);
    fprintf(fil_l.ff,"%s",strsql);
    continue;
   }
  double nds=atof(row1[5]);

  if(data->rk->nds.ravno()[0] != '\0')
   {
    //Карточки с нулевым НДС
    if(data->rk->nds.ravno()[0] == '0' && atof(row1[5]) != 0.)
        continue;
    //Карточки с НДС нужного значения
    if(data->rk->nds.ravno()[0] != '+' && data->rk->nds.ravno()[0] != '0' && (fabs(nds - atof(data->rk->nds.ravno())) > 0.009 || atoi(row1[4]) != 0))
        continue;
    //Карточки в цене которых включено НДС нужного значения
    if(data->rk->nds.ravno()[0] == '+' && data->rk->nds.ravno()[1] != '\0' && (fabs(nds - atof(data->rk->nds.ravno())) > 0.009 || atoi(row1[4]) != 1) )
        continue;
    //Карточки в цене которых НДС любого значения
    if(data->rk->nds.ravno()[0] == '+' && data->rk->nds.ravno()[1] == '\0' &&  atoi(row1[4]) != 1 )
        continue;
   }

  kodm=atoi(row1[0]);
  shu.new_plus(row1[1]);
  ei.new_plus(row1[2]);
  if(data->rk->metka_cena == 0)
    cena=atof(row1[3]);
  else
   {
    cena=0.;
    //Читаем цену в документе
    sprintf(strsql,"select cena from Dokummat1 where datd='%s' and sklad=%d and \
nomd='%s' and nomkar=%d",row[4],skl,nomdok.ravno_filtr(),nk);
    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
      cena=atof(row1[0]);
    short mnds=0;
    short d,m,g;
    iceb_u_rsdat(&d,&m,&g,row[4],2);

    //читаем метку НДС
    sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' and \
sklad=%d and nomerz=11",g,nomdok.ravno_filtr(),skl);

    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
      mnds=atoi(row1[0]);

    if((mnds == 0 || mnds == 4) && pnds_dok != 0. )
     {
      cena=cena+cena*pnds_dok/100.;    
      cena=iceb_u_okrug(cena,okrg1);
     }
   }

  if(data->rk->kodmat.ravno()[0] != '\0')
   {

    if(iceb_u_proverka(data->rk->kodmat.ravno(),kodm,0,0) != 0)
        continue;  
   }

  if(iceb_u_proverka(data->rk->shet.ravno(),shu.ravno(),1,0) != 0)
    continue;  

  /*Узнаем наименование материалла*/
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%d",kodm);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) != 1)
   {

    sprintf(strsql,"%s %d !\n",gettext("Не найден код материалла"),kodm);
    iceb_printw(strsql,data->buffer,data->view);

    fprintf(fil.ff,"%s",strsql);
    fprintf(fil_l.ff,"%s",strsql);
    continue;
   }

  naim.new_plus(row1[0]);
  kgrm=atoi(row1[1]);
  
  if(iceb_u_proverka(data->rk->grupa.ravno(),row1[1],0,0) != 0)
    continue;  

  shetsp.new_plus("");

  //Читаем дополнение к наименованию материалла
  sprintf(strsql,"select shet,dnaim from Dokummat1 where datd='%s' and sklad=%s \
and nomd='%s' and kodm=%d and nomkar=%s",
  row[4],row[0],row[2],kodm,row[1]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    shetsp.new_plus(row1[0]);
    naim.plus(" ",row1[1]);
   }  
  else
   {
    sprintf(strsql,"%s!\n",gettext("Не найдена запись в документе"));
    iceb_printw(strsql,data->buffer,data->view);
   }


  sprintf(strsql,"insert into %s values ('%s',%d,'%s',%d,'%s','%s',%d,'%s','%s',%.10g,'%s',%.10g,%d,%d,'%s','%s')",
  imatmptab,
  shu.ravno(),skl,kor.ravno(),kgrm,row[3],naim.ravno_filtr(),kodm,ei.ravno(),kop.ravno(),cena,row[4],atof(row[6]),atoi(row[5]),
  nk,row[2],shetsp.ravno());

  iceb_sql_zapis(strsql,1,0,data->window);    
  
  int nomgrup=GRUP.find(kgrm);
  int nomshet=SHET.find(shu.ravno());
  sumshgr.plus(cena*atof(row[6]),nomgrup*kolshet+nomshet);
  int nomel=0;  
  sprintf(strsql,"%d#%s#%s",kgrm,shu.ravno(),shetsp.ravno());
  if((nomel=GSHSHK.find(strsql)) >= 0) //Есть в списке
   {
    SUMGSHSHK.plus(cena*atof(row[6]),nomel);
   }
  else //Нет в списке
   {
    GSHSHK.plus(strsql);
    SUMGSHSHK.plus(cena*atof(row[6]),-1);
   }
 }


dvtmcf2ot(imatmptab,data,fil.ff,fil_l.ff);

iceb_podpis(fil.ff,data->window);
fil.end();
iceb_podpis(fil_l.ff,data->window);
fil_l.end();


char	imafshgr[64];
sprintf(imafshgr,"grsh%d.lst",getpid());

rasgrsh(imafshgr,data,dn,mn,gn,dk,mk,gk,&GRUP,&SHET,kolgrup,kolshet,&sumshgr);

char	imafgshshk[64];

sprintf(imafgshshk,"grshk%d.lst",getpid());

rasgshshk(data,dn,mn,gn,dk,mk,gk,imafgshshk,&GSHSHK,&SUMGSHSHK);

sprintf(strsql,"%s %s %s",imaf,imafshgr,imafgshshk);


data->rk->oth.spis_imaf.plus(imaf_l);
data->rk->oth.spis_naim.plus(gettext("Движение товарно-материальных ценностей"));
iceb_ustpeh(data->rk->oth.spis_imaf.ravno(0),2,data->window);

data->rk->oth.spis_imaf.plus(imaf);
data->rk->oth.spis_naim.plus(gettext("Движение товарно-материальных ценностей с итогами"));
iceb_ustpeh(data->rk->oth.spis_imaf.ravno(1),1,data->window);

data->rk->oth.spis_imaf.plus(imafshgr);
data->rk->oth.spis_naim.plus(gettext("Движение материальных ценностей по группам и счетам учёта"));
iceb_ustpeh(data->rk->oth.spis_imaf.ravno(2),3,data->window);

data->rk->oth.spis_imaf.plus(imafgshshk);
data->rk->oth.spis_naim.plus(gettext("Движение материальных ценностей по группам и счетам списания/получения"));
iceb_ustpeh(data->rk->oth.spis_imaf.ravno(3),3,data->window);


 
data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}

