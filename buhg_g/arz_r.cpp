/*$Id: arz_r.c,v 1.26 2014/07/31 07:08:24 sasa Exp $*/
/*16.06.2018	08.11.2006	Белых А.И.	arz_r.c
Автоматическая розноска заработной платы
*/
#include <errno.h>
#include "buhg_g.h"
#include "arz.h"

class arz_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class arz_rek *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  arz_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   arz_r_key_press(GtkWidget *widget,GdkEventKey *event,class arz_r_data *data);
gint arz_r1(class arz_r_data *data);
void  arz_r_v_knopka(GtkWidget *widget,class arz_r_data *data);

void strsen(int nomer,int tabn,const char *inn,const char *bankshet,const char *fio,double bbr,FILE *ff1);
double zarpv(short mr,short gr,int tabnom,double *sumab,GtkWidget *wpredok);

extern SQL_baza bd;
extern int      kol_strok_na_liste;
extern iceb_u_str shrift_ravnohir;
extern short	*obud; /*Обязательные удержания*/
extern char	*shetbu; /*Бюджетные счета удержаний*/
extern short mmm,ggg;
extern double   okrg; /*Округление*/
extern char	*shetb; /*Бюджетные счета начислений*/

int arz_r(class arz_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class arz_r_data data;
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

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Автоматическая разноска зароботной платы"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(arz_r_key_press),&data);

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

repl.plus(gettext("Автоматическая разноска зароботной платы"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(arz_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)arz_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  arz_r_v_knopka(GtkWidget *widget,class arz_r_data *data)
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

gboolean   arz_r_key_press(GtkWidget *widget,GdkEventKey *event,class arz_r_data *data)
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
/*Шапка                           */
/**********************************/

void hpzrs(int nom,int *kolstr,FILE *ff1,GtkWidget *wpredok)
{

if(kolstr != NULL)
 *kolstr+=4;

fprintf(ff1,"\
--------------------------------------------------------------------------------------\n");

fprintf(ff1,gettext("\
Ном.|Табел.|Идентефик.|     Номер          |   Фамилия Имя Отчество       |  Сумма   |\n\
п/п |номер |  номер   | карточного счёта   |                              |          |\n"));
//   123456            12345678901234567890 123456789012345678901234567890 

fprintf(ff1,"\
--------------------------------------------------------------------------------------\n");


}

/******************************************/
/******************************************/

gint arz_r1(class arz_r_data *data)
{
struct  tm      *bf;
time_t vremn;
time(&vremn);
bf=localtime(&vremn);
char strsql[1024];
iceb_u_str repl;
class iceb_clock sss(data->window);
class iceb_u_str bros("");
class iceb_u_str shet_per(""),shet_per_b("");
class iceb_u_str shet_v_kas(""),shet_v_kas_b("");
int		metkaras=0; //0-все 1-только хозрасчёт 2-только бюджет
class iceb_u_str koment("***");

koment.plus(" ",data->rk->koment.ravno());

short mr,gr;

iceb_u_rsdat1(&mr,&gr,data->rk->data.ravno());

double proc_vipl=data->rk->proc_vip.ravno_atof();

class SQLCURSOR curr;
SQL_str row;
short dp=0,mp=0,gp=0;
int kodper=0;
int kod_v_kas=0;
class iceb_u_str imaf_nast("zarnast.alx");
if(data->rk->data_per.getdlinna() > 1)
 {
  if(iceb_u_rsdat(&dp,&mp,&gp,data->rk->data_per.ravno(),1) != 0)
   { 
    iceb_menu_soob(gettext("Неправильно введена дата выплаты !"),data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   } 
  /*Читаем код перечисления*/
  if(iceb_poldan("Код перечисления зарплаты на счет",&kodper,imaf_nast.ravno(),data->window) != 0)
   {
    iceb_menu_soob(gettext("Не найден код перечисления зарплаты на счёт !"),data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  if(iceb_poldan("Код получения зарплаты в кассе",&kod_v_kas,imaf_nast.ravno(),data->window) != 0)
   {
    iceb_menu_soob(gettext("Не найден код получения зарплаты в кассе !"),data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

  if(kod_v_kas == 0 && kodper == 0)
   {
    iceb_menu_soob(gettext("Код получения зарплаты в кассе и код перечисления на счёт равны нолю !"),data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

   
  //Читаем счёт перечисления на карт-счёт
  sprintf(strsql,"select shet,shetb from Uder where kod=%d",kodper);
  if(iceb_sql_readkey(strsql,&row,&curr,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),kodper);
    iceb_menu_soob(strsql,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }
  shet_per.new_plus(row[0]);
  shet_per_b.new_plus(row[1]);

  //*Читаем счёт 
  sprintf(strsql,"select shet,shetb from Uder where kod=%d",kod_v_kas);
  if(iceb_sql_readkey(strsql,&row,&curr,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код удержания"),kod_v_kas);
    iceb_menu_soob(strsql,data->window);
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }
  shet_v_kas.new_plus(row[0]);
  shet_v_kas_b.new_plus(row[1]);

  if(shetbu != NULL)
   {
    //Проверяем есть ли счета в плане счетов
    sprintf(strsql,"select ns from Plansh where ns='%s'",shet_per.ravno());
    int i;
    if((i=sql_readkey(&bd,strsql,&row,&curr)) != 1)
     {
      if(shet_per.getdlinna() > 1)
       sprintf(strsql,gettext("Нет счёта %s в плане счетов !"),shet_per.ravno());
      else
       sprintf(strsql,"%s %d %s!",gettext("Для кода удержания"),kodper,gettext("не введён хозрасчётный счёт в справочнике удержаний"));
       

      iceb_menu_soob(strsql,data->window);
      sss.clear_data();
      gtk_widget_destroy(data->window);
      return(FALSE);
     }

    sprintf(strsql,"select ns from Plansh where ns='%s'",shet_per_b.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      if(shet_per_b.getdlinna() > 1)
       sprintf(strsql,gettext("Не найден счёт %s в плане счетов !"),shet_per_b.ravno());
      else
       sprintf(strsql,"%s %d %s!",gettext("Для кода удержания"),kodper,gettext("не введён хозрасчётный счёт в справочнике удержаний"));
      

      iceb_menu_soob(strsql,data->window);
      sss.clear_data();
      gtk_widget_destroy(data->window);
      return(FALSE);
     }

    //Проверяем есть ли счета в плане счетов
    sprintf(strsql,"select ns from Plansh where ns='%s'",shet_v_kas.ravno());
    if((i=sql_readkey(&bd,strsql,&row,&curr)) != 1)
     {
      if(shet_v_kas.getdlinna() > 1)
       sprintf(strsql,gettext("Нет счёта %s в плане счетов !"),shet_v_kas.ravno());
      else
       sprintf(strsql,"%s %d %s!",gettext("Для кода удержания"),kod_v_kas,gettext("не введён хозрасчётный счёт в справочнике удержаний"));
      
      iceb_menu_soob(strsql,data->window);
      sss.clear_data();
      gtk_widget_destroy(data->window);
      return(FALSE);
     }

    sprintf(strsql,"select ns from Plansh where ns='%s'",shet_v_kas_b.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      if(shet_v_kas_b.getdlinna() > 1)
       sprintf(strsql,gettext("Нет счёта %s в плане счетов !"),shet_v_kas_b.ravno());
      else
       sprintf(strsql,"%s %d %s!",gettext("Для кода удержания"),kod_v_kas,gettext("не введён бюджетный счёт в справочнике удержаний"));

      iceb_menu_soob(strsql,data->window);
      sss.clear_data();
      gtk_widget_destroy(data->window);
      return(FALSE);
     }

    iceb_u_str titl;
    iceb_u_str zagolovok;
    iceb_u_spisok punkt_m;

    titl.plus(gettext("Выбор"));


    zagolovok.plus(gettext("Выберите нужное"));


    punkt_m.plus(gettext("Хозрасчёт и бюджет"));//0
    punkt_m.plus(gettext("Только хозрасчёт"));//1
    punkt_m.plus(gettext("Только бюджет"));//2


    metkaras=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);

    if(metkaras == 3 || metkaras == -1)
     {
      iceb_menu_soob(&repl,data->window);
      sss.clear_data();
      gtk_widget_destroy(data->window);
      return(FALSE);
     }
     
   
   }
  
  if(kodper <= 0)
   {
    shet_per.new_plus(shet_v_kas.ravno());
    shet_per_b.new_plus(shet_v_kas_b.ravno());
    kodper=kod_v_kas;
   }

  if(kod_v_kas <= 0)
   {
    shet_v_kas.new_plus(shet_per.ravno());
    shet_v_kas_b.new_plus(shet_per_b.ravno());
    kod_v_kas=kodper;
   }
 }

int uder=2; /*Удержания не выполнять*/
if(dp != 0)
 {

  uder=iceb_menu_danet(gettext("Сделать разноску ? Вы уверены ?"),2,data->window);
  if(iceb_pbpds(mp,gp,data->window) != 0)
   {
    sss.clear_data();
    gtk_widget_destroy(data->window);
    return(FALSE);
   }

 }
 

sprintf(strsql,"%s:%.2f\n",gettext("Максимальная сумма виплати"),data->rk->max_sum.ravno_atof());  
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s\n",gettext("Сортируем записи"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"select tabn,podr,kateg from Zarn where god=%d and mes=%d order by tabn asc",gr,mr);

class SQLCURSOR cur;
int kolstr2=0;
if((kolstr2=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr2 == 0)
 {
  iceb_menu_soob(gettext("Не найжено ни одной записи !"),data->window);

  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

char imafprot[64];
sprintf(imafprot,"send%d.lst",getpid());

FILE *ff1;
if((ff1 = fopen(imafprot,"w")) == NULL)
 {
  iceb_er_op_fil(imafprot,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


iceb_u_zagolov(gettext("Список рaзнесённых сумм"),0,0,0,0,mr,gr,iceb_get_pnk("00",0,NULL),ff1);

//Вывод шапки
hpzrs(1,NULL,ff1,data->window);

float kolstr1=0.;
int podr=0;
double sal=0.,itogo=0.,itogoo=0.;
int nomer=0;
kolstr1=0;
double viplata=0.;
double vid_zar=0.; //Уже выданная зарплата при расчёте с учётом сальдо
double vid_zar_b=0.; //Уже выданная бюджетная зарплата при расчёте с учётом сальдо
int kod_zap=0;
class iceb_u_str shet_zap("");
class iceb_u_str shet_zap_b("");
class SQLCURSOR cur1;
SQL_str row1;
int tabb;
class iceb_u_str inn("");
class iceb_u_str fio("");
class iceb_u_str bankshet("");
double saldb=0.;
double itsal=0.;
int kolstr=0;
double  in1=0,inah=0,nahshet=0,iu=0,ins=0,ius=0,ius1=0,iub=0,iusb=0.;
int prn=0;
int knah=0;
short mz,gz;
double sym=0;
double bbr=0.;
double iusb1=0.;
class ZARP zp;
double bb=0.;
double itogo_shet=0.;
double itogo_b_shet=0.;

gtk_text_buffer_create_tag(data->buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr2,++kolstr1);    

  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->podr.ravno(),row[1],0,0) != 0)
    continue;

  if(iceb_u_proverka(data->rk->kod_kat.ravno(),row[2],0,0) != 0)
    continue;

  if(data->rk->kod_grup_pod.ravno()[0] != '\0')
   {
    sprintf(strsql,"select grup from Podr where kod=%s",row[1]);
    if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
     if(iceb_u_proverka(data->rk->kod_grup_pod.ravno(),row1[0],0,0) != 0)
       continue;
   }

  tabb=atol(row[0]);
  podr=atoi(row[1]);



  sprintf(strsql,"select fio,bankshet,inn from Kartb where tabn=%d",tabb);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabb);
    iceb_menu_soob(strsql,data->window);
    continue;
   }
  fio.new_plus(row1[0]);
  inn.new_plus(row1[2]);
  bankshet.new_plus(row1[1]);
  if(bankshet.ravno()[0] == '\0')
   {
    shet_zap.new_plus(shet_v_kas.ravno());
    shet_zap_b.new_plus(shet_v_kas_b.ravno()); //это пока не сделано
    kod_zap=kod_v_kas;
   }
  else
   {
    shet_zap.new_plus(shet_per.ravno());
    shet_zap_b.new_plus(shet_per_b.ravno());
    kod_zap=kodper;
   }   

  sal=0.;
  /*Читаем сальдо*/

  sal=zarsaldw(1,mr,gr,tabb,&saldb,data->window);
  itsal+=sal;
  if(sal < 0. && data->rk->metka_saldo == 1)
   {
    class iceb_u_str repl;
    repl.plus(gettext("Внимание ! Отрицательное сальдо !"));
    sprintf(strsql,"%s:%d %s",gettext("Табельный номер"),tabb,fio.ravno());
    repl.ps_plus(strsql);
    iceb_menu_soob(strsql,data->window);
   }


  sprintf(strsql,"select datz,prn,knah,suma,godn,mesn,shet \
from Zarp where datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and \
tabn=%d and suma != 0. order by prn,knah asc",  gr,mr,gr,mr,tabb);

  if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr == 0)
   {
    if(data->rk->metka_saldo == 0 && sal > 0.)
     {
      sprintf(strsql,"%d %s %.2f\n%s",tabb,fio.ravno(),sal,gettext("Нет ни начислений ни удержаний! Пропускаем!"));
      iceb_menu_soob(strsql,data->window);
     }  
   
    continue;
   }
  in1=inah=nahshet=iu=ins=ius=ius1=iub=iusb=0.;

  vid_zar=0.; //Уже выданная зарплата при расчёте с учётом сальдо
  vid_zar_b=0.; //Уже выданная бюджетная зарплата при расчёте с учётом сальдо
  int im=0;
  while(cur1.read_cursor(&row1) != 0)
   {

/*
    printw("%s %s %s %s %s %s %s %s\n",
    row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7]);
*/
    
    prn=atoi(row1[1]);

    if(prn == 1)
     {
      if(iceb_u_proverka(data->rk->kod_nah.ravno(),row1[2],0,1) == 0)
        continue;
     }

    knah=atoi(row1[2]);
    sym=atof(row1[3]);
    gz=atoi(row1[4]);
    mz=atoi(row1[5]);

    if(prn == 1)
     {
      in1+=sym;
      inah+=sym;
      ins+=sym;
      if(iceb_u_proverka(shetb,row1[6],0,1) == 0)
         nahshet+=sym;
     }

    if(prn == 2)
     {
      iu+=sym;
      if(iceb_u_proverka(shetbu,row1[6],0,1) == 0)
         iub+=sym;
      
      if(gz == gr && mz == mr)
       {
	ius+=sym;
        if(iceb_u_proverka(shetbu,row1[6],0,1) == 0)
          iusb+=sym;
       }
      else
       {
        if(provkodw(obud,knah) >= 0)
         {
          ius+=sym;
          if(iceb_u_proverka(shetbu,row1[6],0,1) == 0)
            iusb+=sym;
         }
       }


     }


    im++;

   }

  if(im > 0 || sal != 0.)
   {



    if(data->rk->metka_saldo == 0)
     {
      //При расчёте ведомости этой проверки на выданную зарплату нет так как
      //нам нужно получить ведомость без учёта уже выданной зарплаты
      vid_zar=zarpv(mr,gr,tabb,&iusb1,data->window);
      bbr=in1+iu+sal+vid_zar;
//      printw("%f+%f+%f+%f=%f\n",in1,iu,sal,vid_zar,bbr);

     }
    if(data->rk->metka_saldo == 1)
     {
      ius1=prbperw(mr,gr,bf->tm_mon+1,bf->tm_year+1900,tabb,&iusb1,0,data->window);
      bbr=in1+ius+ius1;
     }

    if(bbr > 0.009)
     {


      viplata=bbr;
      if(proc_vipl != 100.)
       {
        viplata=bbr*proc_vipl/100.;
        viplata=iceb_u_okrug(viplata,okrg);
        }

      if(data->rk->max_sum.ravno_atof() > 0.009) 
       if(viplata > data->rk->max_sum.ravno_atof())
        viplata=data->rk->max_sum.ravno_atof();

      sprintf(strsql,"%4d %-5d %8.2f %-20s %s\n",
      podr,tabb,viplata,bankshet.ravno(),fio.ravno());

      iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

      itogo+=viplata;
      if(bankshet.getdlinna() <= 1)
       itogo_b_shet+=viplata;
      else
       itogo_shet+=viplata;
                   
      strsen(++nomer,tabb,inn.ravno(),bankshet.ravno(),fio.ravno(),viplata,ff1);

      if(dp != 0 && uder == 1)
       {
        /*Проверяем записан ли стандартный набор*/
        provzzagw(mp,gp,tabb,2,podr,data->window);
/*****
        zp.dz=dp;
        zp.mesn=mr; zp.godn=gr;
        zp.nomz=0;
        zp.podr=podr;
********/
        zp.tabnom=tabb;
        zp.prn=2;
        zp.knu=kod_zap;
        zp.dz=dp;
        zp.mz=mp;
        zp.gz=gp;
        zp.mesn=mr; zp.godn=gr;
        zp.nomz=0;
        zp.podr=podr;

       }

      bb=0.;     

      if(shetbu != NULL && data->rk->max_sum.ravno_atof() < 0.01 && shet_zap_b.ravno()[0] != '\0')
       {
       
        if(data->rk->metka_saldo == 0 ) /*С учетом сальдо*/
         {
          bb=bbr-(nahshet+iub+saldb+vid_zar_b);

         }               
       
        if(data->rk->metka_saldo == 1 ) /*Без учета сальдо*/
         {
          bb=in1-nahshet+(ius-iusb)+(ius1-iusb1);
         }

        bb=bbr-bb;
        if(bb != 0.)
         {
          fprintf(ff1,"%4s %-5s %8.2f/%.2f %s/%s\n",
          " "," ",bb,bbr-bb,shet_zap_b.ravno(),shet_zap.ravno());
         }       

        if(metkaras == 0 || metkaras == 2)
        if(dp != 0 && uder == 1)
        if(bb > 0.)
         {
          strcpy(zp.shet,shet_zap_b.ravno());
          viplata=bb;
          if(proc_vipl != 100.)
           {
            viplata=bb*proc_vipl/100.;
            viplata=iceb_u_okrug(viplata,okrg);
            }

          if(data->rk->max_sum.ravno_atof() > 0.009) 
           if(viplata > data->rk->max_sum.ravno_atof())
            viplata=data->rk->max_sum.ravno_atof();
         
          strncpy(zp.shet,shet_zap_b.ravno(),sizeof(zp.shet)-1);
          
  
          zapzarpw(&zp,viplata*-1,dp,mr,gr,0,shet_zap_b.ravno(),koment.ravno(),0,podr,"",data->window);
         }
       }
      if(metkaras == 0 || metkaras == 1)
      if(dp != 0 && uder == 1)
       {
        bb=bbr-bb;
     
        /*Записываем код перечисления*/
        if(bb > 0.)
         {
//          printw("%d.%d.%d %d %d.%d %f\n",dp,mp,gp,tabb,mr,gr,bb);
          strcpy(zp.shet,shet_zap.ravno());
          viplata=bb;
          if(proc_vipl != 100.)
           {
            viplata=bb*proc_vipl/100.;
            viplata=iceb_u_okrug(viplata,okrg);
           }

          if(data->rk->max_sum.ravno_atof() > 0.009) 
           if(viplata > data->rk->max_sum.ravno_atof())
            viplata=data->rk->max_sum.ravno_atof();

          strncpy(zp.shet,shet_zap.ravno(),sizeof(zp.shet)-1);

          zapzarpw(&zp,viplata*-1,dp,mr,gr,0,shet_zap.ravno(),koment.ravno(),0,podr,"",data->window);
          

         }
       }
      //Делаем проводки
      zaravprw(tabb,mp,gp,NULL,data->window);

     }

   }


 }
fprintf(ff1,"ICEB_LST_END\n");


itogoo+=itogo;

//Вывод концовки файла
fprintf(ff1,"\
--------------------------------------------------------------------------------------\n");
fprintf(ff1,"%*s:%10.2f\n",iceb_u_kolbait(74,gettext("Итого")),gettext("Итого"),itogoo);

fprintf(ff1,"%*s:%11.2f\n",
iceb_u_kolbait(20,gettext("На карточки")),
gettext("На карточки"),itogo_shet);

fprintf(ff1,"%*s:%11.2f\n",
iceb_u_kolbait(20,gettext("В кассу")),
gettext("В кассу"),itogo_b_shet);


iceb_podpis(ff1,data->window);


fclose(ff1);

int orient=0;
iceb_ustpeh(imafprot,3,&orient,data->window);
iceb_rnl(imafprot,orient,NULL,&hpzrs,data->window);


sprintf(strsql,"%*s:%11.2f\n",
iceb_u_kolbait(20,gettext("На карточки")),
gettext("На карточки"),itogo_shet);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

sprintf(strsql,"%*s:%11.2f\n",
iceb_u_kolbait(20,gettext("В кассу")),
gettext("В кассу"),itogo_b_shet);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");


sprintf(strsql,"%*s:%11.2f\n",
iceb_u_kolbait(20,gettext("Общий итог")),
gettext("Общий итог"),itogoo);
iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

data->rk->imaf.plus(imafprot);
data->rk->naimf.plus(gettext("Список разнесеных сумм"));
for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}

/*************/
/*Вывод строки*/
/**************/

void strsen(int nomer,int tabn,const char *inn,const char *bankshet,const char *fio,double bbr,FILE *ff1)
{

fprintf(ff1,"%4d %-6d %-10s %-20s %-*.*s %10.2f\n",
nomer,tabn,inn,bankshet,
iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,
bbr);

}
