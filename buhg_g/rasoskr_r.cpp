/*$Id: rasoskr_r.c,v 1.42 2013/09/26 09:46:54 sasa Exp $*/
/*23.05.2016	22.10.2004	Белых А.И.	rasoskr_r.c
Расчёт отчёта остатка по карточкам
*/
#include <errno.h>
#include "buhg_g.h"
#include "rasoskr.h"

class rasoskr_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
    
  class rasoskr_data *rk;
  
  int voz; //0-все в порядке расчёт сделан 1-нет
  rasoskr_r_data()
   {
    voz=1;
   }
 };

gboolean   rasoskr_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasoskr_r_data *data);
gint rasoskr_r1(class rasoskr_r_data *data);
void  rasoskr_r_v_knopka(GtkWidget *widget,class rasoskr_r_data *data);

void nahalo_rasoskr(class rasoskr_r_data *data,short dos,short mos,short gos,int *kolstrlist,int mns,struct tm *bf,iceb_u_str *naiskl,FILE *ff1);
void 		sapkk1(short,short,short,FILE*,int*,int*,int);
void 		sapkk(short,short,short,int,FILE*,int);


extern SQL_baza bd;


extern double	nds1;
extern short	vtara; /*Код группы возвратная тара*/
extern double	okrcn;
 
extern double	okrg1;  /*Округление суммы*/
extern int      kol_strok_na_liste;

int rasoskr_r(class rasoskr_data *rek_ras,GtkWidget *wpredok)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_spisok repl_s;
class rasoskr_r_data data;
data.rk=rek_ras;
data.name_window.plus(__FUNCTION__);



data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать остатки на карточках"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rasoskr_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Распечатать остатки на карточках"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(rasoskr_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)rasoskr_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rasoskr_r_v_knopka(GtkWidget *widget,class rasoskr_r_data *data)
{
if(data->voz == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   rasoskr_r_key_press(GtkWidget *widget,GdkEventKey *event,class rasoskr_r_data *data)
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

/**************************************/
/*шапка для отсортированого по наименованию*/
/********************************************/
void rasoskr_naim_sap(int nom_str,int *kolstr,FILE *ff,GtkWidget *wpredok)
{
if(kolstr != NULL)
 *kolstr+=4;

fprintf(ff,"%100s %s N%d\n","",gettext("Страница"),nom_str);

fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext(" Счёт  |Гр|Скл| Н/К |  Код |            Наименование             |    Цена   |НДС|  Е.и. | Остаток   |  Сумма   |К.дата исп|\n"));


fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------------------\n");
}
/***************************************************/
/*реквизиты поиска*/
/*****************************************************/
void rasosrk_rp(class rasoskr_data *poi,FILE *ff)
{

if(poi->sklad.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Склад"),poi->sklad.ravno());

if(poi->grupa.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Группа"),poi->grupa.ravno());

if(poi->shet.getdlinna() > 1)
  fprintf(ff,"%s:%s\n",gettext("Счёт учета"),poi->shet.ravno());
if(poi->kodmat.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Код материалла"),poi->kodmat.ravno());
if(poi->naim.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Наименование материалла"),poi->naim.ravno());
if(poi->invnom.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Инвентарный номер"),poi->invnom.ravno());

if(poi->nds.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("НДС"),poi->nds.ravno());

if(poi->kon_dat_is.ravno()[0] != '\0')
  fprintf(ff,"%s:%s\n",gettext("Конечная дата использования"),poi->kon_dat_is.ravno());

}


/******************************************/
/******************************************/

gint rasoskr_r1(class rasoskr_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);

int skl=0;
skl=data->rk->sklad.ravno_atoi();

SQLCURSOR cur,curtmp;
SQL_str   row,rowtmp;
SQL_str   row1;
SQLCURSOR cur1;

iceb_u_str matot;
matot.new_plus("");

iceb_u_str naiskl;
naiskl.new_plus("");

if(skl != 0)
 {
  sprintf(strsql,"select naik,fmol from Sklad where kod=%d",skl);
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    repl.new_plus(gettext("Не найден код склада"));
    repl.plus(" ");
    repl.plus(skl);
    repl.plus(" !");
    iceb_menu_soob(&repl,data->window);    
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  naiskl.new_plus(row[0]);
  matot.new_plus(row[1]); 
 }


/*Определяем количество складов в строке*/
int mns=iceb_u_pole2(data->rk->sklad.ravno(),',');
if(data->rk->sklad.getdlinna() > 1 && mns == 0)
  mns=1; /*Только один склад в списке*/
else
  mns=0;

int kolstr;
sprintf(strsql,"select * from Kart");
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


if(kolstr == 0)
 {
  repl.new_plus(gettext("Не найдено ни одной записи !"));
  iceb_menu_soob(&repl,data->window);    
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_tmptab tabtmp;
const char *imatmptab={"rasoskr"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
shu char(24) not null,\
kgrm int not null,\
skl int not null,\
nk int not null,\
km int not null,\
naim char(112) not null,\
cena double(15,6) not null,\
nds char(32) not null,\
ei char(24) not null,\
ost double(16,6) not null,\
suma double(16,6) not null,\
datki char(10) not null,\
innom char(56) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,data->window) != 0)
 {
  gtk_widget_destroy(data->window);
  return(FALSE);
 }  

time_t tmm;
time(&tmm);
struct tm *bf;
bf=localtime(&tmm);

if(skl != 0 && mns == 1)
 {
  sprintf(strsql,"%s\n\n%s N%s %s\n\
%s %s%s\n", iceb_get_pnk("00",0,data->window),gettext("Перечень карточек по складу"),
  data->rk->sklad.ravno(),naiskl.ravno(),
  gettext("Остатки на"),
  data->rk->data_ost.ravno(),
  gettext("г."));
 }
if(skl == 0)
 {
  sprintf(strsql,"%s\n\n%s\n\
%s %s%s\n", iceb_get_pnk("00",0,data->window),gettext("Перечень карточек по всем складам"),
  gettext("Остатки на"),
  data->rk->data_ost.ravno(),
  gettext("г."));
 }

iceb_printw(strsql,data->buffer,data->view);

char data_k_is[64];
float kolstr1=0.;
class iceb_u_str naim;
int kgrm=0;
int nk=0;
class iceb_u_str ei("");
class ostatok ost;
short dos,mos,gos;
iceb_u_rsdat(&dos,&mos,&gos,data->rk->data_ost.ravno(),1);
char		mnds=' ';
double bb;
class iceb_u_str invnom;
short d=0,m=0,g=0;
short dki=0,mki=0,gki=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[0],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->invnom.ravno(),row[15],0,0) != 0)
    continue;

   if(iceb_u_proverka(data->rk->kodmat.ravno(),row[2],0,0) != 0)
      continue;

  if(provndsw(data->rk->nds.ravno(),row) != 0)
    continue;

  if(iceb_u_proverka(data->rk->shet.ravno(),row[5],0,0) != 0)
    continue;

  if(data->rk->metka_innom == 1)
   if(row[15][0] == '\0')
     continue;

  naim.new_plus("");
  kgrm=0;
  /*Узнаем наименование материала*/
  sprintf(strsql,"select * from Material where kodm=%s",row[2]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    printf("Не нашли наименование материалу по коду %s\n",row[2]);
   }
  else
   {
    naim.new_plus(row1[2]);
    kgrm=atoi(row1[1]);
   }

  if(iceb_u_proverka(data->rk->naim.ravno(),row1[2],4,0) != 0)
    continue;
  if(iceb_u_proverka(data->rk->grupa.ravno(),row1[1],0,0) != 0)
    continue;

  iceb_u_rsdat(&d,&m,&g,row[18],2);
  memset(data_k_is,'\0',sizeof(data_k_is));
  if(dki != 0 )
   {
    if(d == 0)
     continue;
    if(iceb_u_sravmydat(d,m,g,dki,mki,gki) > 0)
     continue;    
   }
  if(d != 0)
   sprintf(data_k_is,"%02d.%02d.%04d",d,m,g);

  nk=atoi(row[1]);

  ei.new_plus(row[4]);
  ostkarw(1,1,gos,dos,mos,gos,atoi(row[0]),nk,&ost);

  if(ost.ostg[3] < -0.0000001)
   {
    repl.new_plus(gettext("Внимание"));
    repl.plus(" !!!");
    

    sprintf(strsql,"%s:%s %s:%d",gettext("Склад"),
    row[0],gettext("Карточка"),nk);
    repl.ps_plus(strsql);
    
    repl.ps_plus(gettext("Отрицательный остаток"));
    iceb_menu_soob(&repl,data->window);
   }

  if(data->rk->metka_ost == 1 || (data->rk->metka_ost == 0 && (ost.ostg[3] > 0.0000001 || ost.ostg[3] < -0.0000001 )))
   {
    if(row[3][0] == '0')
     mnds=' ';    
    if(row[3][0] == '1')
     mnds='+';    


    bb=ost.ostg[3]*atof(row[6]);
    bb=iceb_u_okrug(bb,okrg1);
    invnom.new_plus("");
    if(row[15][0] != '\0')
     {
      invnom.new_plus(row[15]);
      if(row[14][0] != '\0')
       {
        short d,m,g;
        iceb_u_rsdat(&d,&m,&g,row[14],2);
        sprintf(strsql," %d.%d.%d",d,m,g);
        invnom.plus(" ",strsql);
       }
     }

    sprintf(strsql,"insert into %s values ('%s',%d,%s,%d,%s,'%s',%s,'%c%2.2g','%s',%.10g,%.10g,'%s','%s')",
    imatmptab,
    row[5],kgrm,row[0],nk,row[2],
    naim.ravno_filtr(),
    row[6],
    mnds,
    atof(row[9]),
    ei.ravno(),
    ost.ostg[3],
    bb,
    data_k_is,
    invnom.ravno_filtr());
/**************
    sprintf(strsql,"insert into %s values ('%s',%d,%s,%d,%s,'%s',%s,'%c%2.2g','%s',%.10g,%.10g,'%s')",
    imatmptab,
    row[5],kgrm,row[0],nk,row[2],
    naim.ravno_filtr(),
    row[6],
    mnds,
    atof(row[9]),
    ei.ravno(),
    ost.ostg[3],bb,invnom.ravno_filtr());
**************/
    iceb_sql_zapis(strsql,1,0,data->window);    


   }

 }



/*Сортировка по группе материала*/
if(data->rk->metka_r == 0)
  sprintf(strsql,"select * from %s order by kgrm asc,skl asc,naim asc",imatmptab);

/*Сортировка по счёту учета материала*/
if(data->rk->metka_r == 1)
  sprintf(strsql,"select * from %s order by shu asc,skl asc,naim asc",imatmptab);
//Сортировка счет, группа
if(data->rk->metka_r == 2)
  sprintf(strsql,"select * from %s order by shu asc,kgrm asc,skl asc,naim asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


sprintf(strsql,"%s\n",gettext("Распечатываем"));
iceb_printw(strsql,data->buffer,data->view);



char imaf1[64];
FILE *ff1;
sprintf(imaf1,"sk%d_%d.lst",skl,getpid());

data->rk->imaf.new_plus(imaf1);
data->rk->naimf.new_plus(gettext("Распечатка остатков на карточках"));

if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imafpust[64];
FILE *ffp;
sprintf(imafpust,"skp%d_%d.lst",skl,getpid());

data->rk->imaf.plus(imafpust);
data->rk->naimf.plus(gettext("Распечатка остатков на карточках (пустографка)"));

if((ffp = fopen(imafpust,"w")) == NULL)
 {
  iceb_er_op_fil(imafpust,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imafiv[64];
sprintf(imafiv,"inv%d_%d.lst",skl,getpid());
FILE *ff2;

data->rk->imaf.plus(imafiv);
data->rk->naimf.plus(gettext("Распечатка инвентаризационной ведомости"));

if((ff2 = fopen(imafiv,"w")) == NULL)
 {
  iceb_er_op_fil(imafiv,"",errno,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }



d=dos; 
m=mos; 
g=gos;

iceb_u_dpm(&d,&m,&g,1); //Увеличить на день
class iceb_u_str mesqc("");
iceb_mesc(m,1,&mesqc);

int kolstriv=0;
int kollistiv=0;
sapvinw(matot.ravno(),mesqc.ravno(),g,gos,&kolstriv,&kollistiv,ff2);

int kolstrlist=0;
nahalo_rasoskr(data,dos,mos,gos,&kolstrlist,mns,bf,&naiskl,ff1);

int kolstrlist_p=0;
nahalo_rasoskr(data,dos,mos,gos,&kolstrlist_p,mns,bf,&naiskl,ffp);



class iceb_u_str shet("");
class iceb_u_str shetz("");
class iceb_u_str grup("");
class iceb_u_str grupz("");

double itogshk=0.,itogshs=0.;
double itk=0.,its=0.,itgk=0.,itgs=0.,itok=0.,itos=0.; 
int sklz=0;

kolstrlist+=4;
kolstrlist_p+=4;
int kollist=1;

sapkk(dos,mos,gos,kollist,ff1,0);
int kollist_p=1;
sapkk(dos,mos,gos,kollist_p,ffp,1);

int nomerstr=0;
char str[1024];
kolstr1=0.;
int skk;
class iceb_u_str naish;
class iceb_u_str dopstr;
class iceb_u_str innom;
class iceb_u_str regnom;
int kodmat=0;
double kolih=0.,summa=0.,cena=0.;
class iceb_u_str naim_mat;
class iceb_u_str nds_str("");

while(curtmp.read_cursor(&rowtmp) != 0)
 {
  iceb_pbar(data->bar,kolstrtmp,++kolstr1);    
  shet.new_plus(rowtmp[0]);
  grup.new_plus(rowtmp[1]);
  skk=atoi(rowtmp[2]);
  nk=atoi(rowtmp[3]);
  kodmat=atoi(rowtmp[4]);
  naim_mat.new_plus(rowtmp[5]);
  cena=atof(rowtmp[6]);
  nds_str.new_plus(rowtmp[7]);
  ei.new_plus(rowtmp[8]);
  kolih=atof(rowtmp[9]);
  summa=atof(rowtmp[10]);
  strncpy(data_k_is,rowtmp[11],sizeof(data_k_is)-1);
  invnom.new_plus(rowtmp[12]);
      

  if((data->rk->metka_r == 1 || data->rk->metka_r == 2) && iceb_u_SRAV(shetz.ravno(),shet.ravno(),0) != 0)
   {
    
    if(shetz.ravno()[0] != '\0')
     {
      if(data->rk->metka_r == 1)
       { 
        if(mns != 1)
         {
          sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
          fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------\n");
          sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
          fprintf(ff1,"%-*s %10.10g  %10.2f\n",
          iceb_u_kolbait(88,gettext("Итого по складу")),
          gettext("Итого по складу"),
          itok,itos);
         }
        itk+=itok;
        its+=itos;
        itok=itos=0.;
       }

      sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
      fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------\n");
      sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
      fprintf(ff1,"%-*s %10.10g  %10.2f\n",
      iceb_u_kolbait(88,gettext("Итого по счёту")),gettext("Итого по счёту"),
      itogshk,itogshs);

      sapkk1ivw(&kolstriv,&kollistiv,ff2);
      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n");

      sapkk1ivw(&kolstriv,&kollistiv,ff2);
      fprintf(ff2,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Итого по счёту")),gettext("Итого по счёту"),
      itogshk,itogshs);

      itogshk=itogshs=0.;

     }
    /*Определяем наименование счета*/
    naish.new_plus("");
    sprintf(strsql,"select nais from Plansh where ns='%s'",shet.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
      printf("Не найден счёт %s в плане счетов !\n",shet.ravno());         
     }
    else
     naish.new_plus(row1[0]);

    sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
    fprintf(ff1,"\n");
    sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
    fprintf(ff1,"%s: %s %s\n",gettext("Счёт"),shet.ravno(),naish.ravno());

    sapkk1ivw(&kolstriv,&kollistiv,ff2);
    fprintf(ff2,"\n");
    sapkk1ivw(&kolstriv,&kollistiv,ff2);
    fprintf(ff2,"%s: %s %s\n",gettext("Счёт"),shet.ravno(),naish.ravno());

    shetz.new_plus(shet.ravno());    
    sklz=0;
   }
      
  if((data->rk->metka_r == 0 || data->rk->metka_r == 2) && iceb_u_SRAV(grupz.ravno(),grup.ravno(),0) != 0)
   {
    
    if(grupz.ravno()[0] != '\0')
     {
      if(itok != 0. || itos != 0.)
       {
        sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
        fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------\n");
        sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
        fprintf(ff1,"%-*s %10.10g  %10.2f\n",
        iceb_u_kolbait(88,gettext("Итого по складу")),gettext("Итого по складу"),
        itok,itos);
        itk+=itok;
        its+=itos;
        itok=itos=0.;
       }

      sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
      fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------\n");
      sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
      fprintf(ff1,"%-*s %10.10g  %10.2f\n",
      iceb_u_kolbait(88,gettext("Итого по группе")),gettext("Итого по группе"),
      itgk,itgs);

      sapkk1ivw(&kolstriv,&kollistiv,ff2);
      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n");
      sapkk1ivw(&kolstriv,&kollistiv,ff2);
      fprintf(ff2,"%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Итого по группе")),gettext("Итого по группе"),
      itgk,itgs);

      itgk=itgs=0.; 


     }
    /*Узнаем наименование группы*/
    iceb_u_str naimgr;
    naimgr.new_plus("");
    sprintf(strsql,"select naik from Grup where kod=%s",grup.ravno());
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       printf("Нет группы %s\n",grup.ravno());
       naimgr.new_plus("Не найдена группа !!!");
     }
    else
     naimgr.new_plus(row1[0]);
     
    sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
    fprintf(ff1,"\n");
    sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
    fprintf(ff1,"%s: %s %s\n",gettext("Группа"),grup.ravno(),naimgr.ravno());

    sapkk1ivw(&kolstriv,&kollistiv,ff2);
    fprintf(ff2,"\n");
    sapkk1ivw(&kolstriv,&kollistiv,ff2);
    fprintf(ff2,"%s: %s %s\n",gettext("Группа"),grup.ravno(),naimgr.ravno());

    grupz.new_plus(grup.ravno());
    sklz=0;
   }


  if(sklz != skk)
   {
    if(sklz != 0 )
     {

      if(mns != 1)
       {
        sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
        fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------\n");
        sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
        fprintf(ff1,"%-*s %10.10g  %10.2f\n",
        iceb_u_kolbait(88,gettext("Итого по складу")),gettext("Итого по складу"),
        itok,itos);

       }      
      itk+=itok;
      its+=itos;
      itok=itos=0.;

      
     }
    iceb_u_str matot;
    matot.new_plus("");
    iceb_u_str naim_skl;
    naim_skl.new_plus("");
    /*Узнаем наименование Склада*/
    sprintf(strsql,"select naik,fmol from Sklad where kod=%d",
    skk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
       printf("Нет склада %d\n",skk);
       naim_skl.new_plus("Не найден склад !!!");
     }
    else
     {
      naim_skl.new_plus(row1[0]);
      matot.new_plus(row1[1]);
     }

    sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
    fprintf(ff1,"\n");
    sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
    fprintf(ff1,"%s: %d %s\n",
    gettext("Склад"),
    skk,naim_skl.ravno());
    sklz=skk;    


   }


  itgk+=kolih;
  itgs+=summa;

  itogshk+=kolih;
  itogshs+=summa;

  itok+=kolih;
  itos+=summa;
   

  //Вывод в файл с пустографкой
  sapkk1(dos,mos,gos,ffp,&kolstrlist_p,&kollist_p,1);

  fprintf(ffp,"%-*s|",iceb_u_kolbait(7,shet.ravno()),shet.ravno());
  fprintf(ffp,"%-2s|",grup.ravno());

  fprintf(ffp,"%-2d|",skk);

  fprintf(ffp,"%-5d|",nk);
  fprintf(ffp,"%-6d|",kodmat);
  fprintf(ffp,"%-*.*s|",iceb_u_kolbait(37,naim_mat.ravno()),iceb_u_kolbait(37,naim_mat.ravno()),naim_mat.ravno());

  fprintf(ffp," %-*s|",iceb_u_kolbait(6,ei.ravno()),ei.ravno());

  fprintf(ffp,"%10.10g |",kolih);

  /*Остаток по документам*/
  ostatok ost;
  ostdokw(1,1,gos,dos,mos,gos,skk,nk,&ost);
  fprintf(ffp,"%10.10g |",ost.ostg[3]);
  
  fprintf(ffp,"          |          |          |\n");

  sapkk1(dos,mos,gos,ffp,&kolstrlist_p,&kollist_p,1);
  fprintf(ffp,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");

  //Вывод в файл основной
  //Читаем инвентарный номер
  dopstr.new_plus("");
  innom.new_plus("");
  regnom.new_plus("");
  
  short dv=0,mv=0,gv=0;
  class iceb_u_str sklad("");
  class iceb_u_str nomkar("");  
  iceb_u_polen(str,&sklad,3,'|');
  iceb_u_polen(str,&nomkar,4,'|');

  sprintf(strsql,"select datv,innom,rnd from Kart where sklad=%s and nomk=%s",
  sklad.ravno(),nomkar.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur1) == 1)
   {
    iceb_u_rsdat(&dv,&mv,&gv,row[0],2);
    innom.new_plus(row[1]);
    regnom.new_plus(row[2]);
   }   

  if(innom.getdlinna() > 1)
   {
    if( regnom.getdlinna() <= 1)
     {
      sprintf(strsql,"%s:%s",gettext("Инвентарный номер"),innom.ravno());
      dopstr.new_plus(strsql);
      if(dv != 0)
       {
        sprintf(strsql," %s:%d.%d.%d%s",
        gettext("Дата ввода в эксплуатацию"),
        dv,mv,gv,gettext("г."));
        dopstr.plus(strsql);
       }
     }        
    else
     {
      sprintf(strsql,"%s:%s",gettext("Регистрационный номер"),regnom.ravno());
      dopstr.new_plus(strsql);
      sprintf(strsql," %s:%s",gettext("Серия"),innom.ravno());
      dopstr.plus(strsql);
      if(dv != 0)
       {
        sprintf(strsql," %s:%d.%d.%d%s",
        gettext("Дата регистрации"),
        dv,mv,gv,gettext("г."));
        dopstr.plus(strsql);
       }
     }
   }    


  sapkk1(dos,mos,gos,ff1,&kolstrlist,&kollist,0);
//  fprintf(ff1,"%s",str);

  fprintf(ff1,"%-*s |%2d|%-2d|%-5d|%-6d|%-*.*s|%10s ",
  iceb_u_kolbait(6,shet.ravno()),shet.ravno(),
  atoi(grup.ravno()),skk,nk,kodmat,
  iceb_u_kolbait(37,naim_mat.ravno()),iceb_u_kolbait(37,naim_mat.ravno()),naim_mat.ravno(),
  prcnw(cena));

  fprintf(ff1,"|%-3s| %-*s|%10.10g |%10.2f|%10s|%s\n",
  nds_str.ravno(),
  iceb_u_kolbait(6,ei.ravno()),ei.ravno(),
  kolih,summa,data_k_is,invnom.ravno());


  sapkk1ivw(&kolstriv,&kollistiv,ff2);
  
  fprintf(ff2,"%3d|%-*.*s|%-*s|%10.10g|%10.10g|%10.2f|%10s|%10s|%10s|%10s|\n",
  ++nomerstr,
  iceb_u_kolbait(40,naim_mat.ravno()),iceb_u_kolbait(40,naim_mat.ravno()),naim_mat.ravno(),
  iceb_u_kolbait(6,ei.ravno()),ei.ravno(),
  cena,kolih,summa," "," "," "," ");

  if(dopstr.getdlinna() > 1)
   {
    sapkk1ivw(&kolstriv,&kollistiv,ff2);
    fprintf(ff2,"%s\n",dopstr.ravno());
   }
  sapkk1ivw(&kolstriv,&kollistiv,ff2);
  fprintf(ff2,"\
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");


 }

itk+=itok;
its+=itos;

if(mns == 1)
 {
    if(data->rk->metka_r == 0)
     {
      fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",
      iceb_u_kolbait(88,gettext("Итого по группе")),gettext("Итого по группе"),
      itgk,itgs);

      fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",
      iceb_u_kolbait(88,gettext("Общий итог")),gettext("Общий итог"),
      itk,its);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Итого по группе")),gettext("Итого по группе"),
      itgk,itgs);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Общий итог")),gettext("Общий итог"),
      itk,its);
     }

    if(data->rk->metka_r == 1)
     {
      fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",
      iceb_u_kolbait(88,gettext("Итого по счёту")),gettext("Итого по счёту"),
       itogshk,itogshs);

      fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",
      iceb_u_kolbait(88,gettext("Общий итог")),gettext("Общий итог"),
      itk,its);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Итого по счёту")),gettext("Итого по счёту"),
       itogshk,itogshs);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",
      iceb_u_kolbait(62,gettext("Общий итог")),gettext("Общий итог"),itk,its);
     }

    if(data->rk->metka_r == 2)
     {

      fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",
      iceb_u_kolbait(88,gettext("Итого по складу")),gettext("Итого по складу"),itok,itos);

      fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_u_kolbait(88,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);

      fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_u_kolbait(88,gettext("Итого по счёту")),gettext("Итого по счёту"),itgk,itgs);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_u_kolbait(88,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);

      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",iceb_u_kolbait(62,gettext("Итого по счёту")),gettext("Итого по счёту"),itgk,itgs);

     }
 }

  if(mns != 1)
   {
     fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_u_kolbait(88,gettext("Итого по складу")),gettext("Итого по складу"),itok,itos);

    if(data->rk->metka_r == 0 || data->rk->metka_r == 2)
     {
      fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_u_kolbait(88,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);
      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",iceb_u_kolbait(62,gettext("Итого по группе")),gettext("Итого по группе"),itgk,itgs);
     }
    if(data->rk->metka_r == 1 || data->rk->metka_r == 2)
     {
      fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_u_kolbait(88,gettext("Итого по счёту")),gettext("Итого по счёту"),
       itogshk,itogshs);
      fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",iceb_u_kolbait(62,gettext("Итого по счёту")),gettext("Итого по счёту"),
       itogshk,itogshs);
     }
    fprintf(ff1,"\
--------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g  %10.2f\n",iceb_u_kolbait(88,gettext("Общий итог")),gettext("Общий итог"),itk,its);
    fprintf(ff2,"\
---------------------------------------------------------------------------------------------------------------------------------\n\
%-*s %10.10g %10.2f\n",iceb_u_kolbait(62,gettext("Общий итог")),gettext("Общий итог"),itk,its);
   }

sprintf(strsql,"%s: %10.10g %10.2f\n",
gettext("Общий итог"),itk,its);
iceb_printw(strsql,data->buffer,data->view);

koncinvw(mesqc.ravno(),gos,ff2);
iceb_podpis(ff1,data->window);
iceb_podpis(ffp,data->window);
iceb_podpis(ff2,data->window);

fclose(ff1);
fclose(ffp);

fclose(ff2);



/*распечатка остатков отсортированных по наименованию*/

sprintf(strsql,"select * from %s order by naim asc",imatmptab);

kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return(1);
 }
char imaf_naim[64];
sprintf(imaf_naim,"ostnm%d.lst",getpid());

class iceb_fopen fil_naim;
if(fil_naim.start(imaf_naim,"w",data->window) != 0)
 return(1);
 
iceb_zagolov(gettext("Остатки на карточках"),fil_naim.ff,data->window);

fprintf(fil_naim.ff,"%s %d.%d.%d %s %s\n",
gettext("Остатки вычислены на"),
dos,mos,gos,gettext("г."),
gettext("включительно"));

rasosrk_rp(data->rk,fil_naim.ff);


rasoskr_naim_sap(1,NULL,fil_naim.ff,data->window);

kolstr1=0;
while(curtmp.read_cursor(&rowtmp) != 0)
 {
  iceb_pbar(data->bar,kolstrtmp,++kolstr1);    
  fprintf(fil_naim.ff,"%-*s %2s %3s %-5s %-6s %-*.*s %11.11g %-3.3s %-*s %11.11g %10.2f %s %s\n",
  iceb_u_kolbait(7,rowtmp[0]),
  rowtmp[0],
  rowtmp[1],
  rowtmp[2],
  rowtmp[3],
  rowtmp[4],
  iceb_u_kolbait(37,rowtmp[5]),
  iceb_u_kolbait(37,rowtmp[5]),
  rowtmp[5],
  atof(rowtmp[6]),
  rowtmp[7],
  iceb_u_kolbait(7,rowtmp[8]),
  rowtmp[8],    
  atof(rowtmp[9]),
  atof(rowtmp[10]),
  iceb_u_datzap(rowtmp[11]),
  rowtmp[12]);
 }
fprintf(fil_naim.ff,"ICEB_LST_END\n");
iceb_podpis(fil_naim.ff,data->window);

fil_naim.end();



for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);

data->rk->imaf.plus(imaf_naim);
data->rk->naimf.plus(gettext("Распечатка остатков отсортированных по наименованию"));
iceb_ustpeh(imaf_naim,3,&kolstr,data->window);
iceb_rnl(imaf_naim,kolstr,"",&rasoskr_naim_sap,data->window);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;

return(FALSE);
}

/***************/
/*Счётчик строк*/
/****************/
void		sapkk1(short dos,short mos,short gos,
FILE *ff1,int *kolstrlist,int *kollist,
int metka_sap) 
{

*kolstrlist+=1;

if(*kolstrlist <= kol_strok_na_liste)
 return;

*kollist+=1;
fprintf(ff1,"\f");

*kolstrlist=4; //Размер шапки
*kolstrlist+=1; //СТРОКА которая должна быть выведена

sapkk(dos,mos,gos,*kollist,ff1,metka_sap);


}


/*************/
/*Шапка*/
/*************/

void		sapkk(short dos,short mos,short gos, //дата остатка
int kollist,FILE *ff1,int metka_sap)
{

if(metka_sap == 1)
 { 
  fprintf(ff1,"%100s %s N%d\n\
----------------------------------------------------------------------------------------------------------------------------------\n",
  " ",gettext("Лист"),kollist);
  fprintf(ff1,gettext("  Счёт |Гр|Ск| Н/К |  Код |            Наименование             |  Е.и. | Остаток   |Остаток док|          |          |          |\n"));

  fprintf(ff1,"\
----------------------------------------------------------------------------------------------------------------------------------\n");
 }

if(metka_sap == 0)
 {
 
fprintf(ff1,"%s:%02d.%02d.%04d%s %60s %s N%d\n\
---------------------------------------------------------------------------------------------------------------------------\n",
 gettext("Остатки вычислены на"),
 dos,mos,gos,gettext("г."),
" ",gettext("Лист"),kollist);
fprintf(ff1,gettext(" Счёт  |Гр|Ск| Н/К |  Код |            Наименование             |    Цена   |НДС|  Е.и. | Остаток   |  Сумма   |Дата к.исп|\n"));
fprintf(ff1,"\
---------------------------------------------------------------------------------------------------------------------------\n");
 }

}

/********************************************/
/*Начало распечаток*/
/************************/
void nahalo_rasoskr(class rasoskr_r_data *data,
short dos,short mos,short gos,
int *kolstrlist,
int mns,
struct tm *bf,
iceb_u_str *naiskl,
FILE *ff1)
{
if(data->rk->sklad.getdlinna() > 1)
 {
  if(data->rk->sklad.ravno_atoi() != 0 && mns == 1)
   {
    fprintf(ff1,"%s\n\n%s %s %s\n\
%s %d.%d.%d%s  %s - %d:%d\n",
iceb_get_pnk("00",0,data->window),
    gettext("Перечень карточек по складу"),
    data->rk->sklad.ravno(),naiskl->ravno(),
    gettext("По состоянию на"),
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
    gettext("г."),
    gettext("Время"),
    bf->tm_hour,bf->tm_min);
    *kolstrlist=4;
   }
  else
   {
    fprintf(ff1,"%s\n\n%s %s\n\
%s %d.%d.%d%s  %s - %d:%d\n",
iceb_get_pnk("00",0,data->window),
    gettext("Перечень карточек по складу"),
    data->rk->sklad.ravno(),
    gettext("По состоянию на"),
    bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
    gettext("г."),
    gettext("Время"),
    bf->tm_hour,bf->tm_min);
    *kolstrlist=4;
   }
 }

if(data->rk->sklad.getdlinna() <= 1)
 {
  fprintf(ff1,"%s\n\n%s\n\
%s %d.%d.%d%s  %s - %d:%d\n",
iceb_get_pnk("00",0,data->window),
  gettext("Перечень карточек по всем складам"),
  gettext("По состоянию на"),
  bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,
  gettext("г."),
  gettext("Время"),
  bf->tm_hour,bf->tm_min);
  *kolstrlist=4;
 }

if(data->rk->grupa.getdlinna() > 1)
 {
  fprintf(ff1,"%s:%s\n",gettext("Группа"),data->rk->grupa.ravno());
  *kolstrlist+=1;;
 }

if(data->rk->shet.getdlinna() > 1)
 {
  fprintf(ff1,"%s:%s\n",gettext("Счета учета"),data->rk->shet.ravno());
  *kolstrlist+=1;;
 }
if(data->rk->kodmat.getdlinna() > 1)
 {
  fprintf(ff1,"%s:%s\n",gettext("Код материала"),data->rk->kodmat.ravno());
  *kolstrlist+=1;;
 }
if(data->rk->nds.getdlinna()  > 1 )
 {
  fprintf(ff1,"%s:%s\n",gettext("НДС"),data->rk->nds.ravno());
  *kolstrlist+=1;;
 }
 
if(data->rk->metka_ost == 0)
 {
  fprintf(ff1,"%s\n",gettext("Карточки с нулевыми остатками не показаны !"));
  *kolstrlist+=1;;
 }
if(data->rk->metka_r == 0)
 {
  fprintf(ff1,"%s\n",gettext("Сортировка по группам материалов"));
  *kolstrlist+=1;;
 }
if(data->rk->metka_r == 1)
 {
  fprintf(ff1,"%s\n",gettext("Сортировка по счетам учета материалов"));
  *kolstrlist+=1;;
 }
if(data->rk->metka_r == 2)
 {
  fprintf(ff1,"%s\n",gettext("Сортировка по счетам, группам"));
  *kolstrlist+=1;;
 }
if(data->rk->metka_innom == 1)
 {
  fprintf(ff1,"%s\n",gettext("Только карточки с инвентарными номерами !"));
  *kolstrlist+=1;;
 }

if(data->rk->naim.getdlinna() > 1)
 {
  fprintf(ff1,"%s:%s\n",gettext("Наименование материала"),data->rk->naim.ravno());
  *kolstrlist+=1;;
 }
if(data->rk->invnom.getdlinna() > 1)
 {
  fprintf(ff1,"%s:%s\n",gettext("Инвентарный номер"),data->rk->invnom.ravno());
  *kolstrlist+=1;;
 }

}
