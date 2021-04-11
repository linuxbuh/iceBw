/*$Id: pvbanks_r.c,v 1.30 2014/08/31 06:19:21 sasa Exp $*/
/*25.01.2020	08.04.2004	Белых А.И.	pvbanks_r.c
Перечисление на карт-счета сумм сальдо по счёту
*/
#include <errno.h>
#include "buhg_g.h"
#include "pvbanks.h"


class pvbanks_r_data
 {
  public:
  
  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  pvbanks_rr *rek_r;
    
  
 };

gboolean   pvbanks_r_key_press(GtkWidget *widget,GdkEventKey *event,class pvbanks_r_data *data);
gint pvbanks_r1(class pvbanks_r_data *data);
void  pvbanks_r_v_knopka(GtkWidget *widget,class pvbanks_r_data *data);


extern SQL_baza bd;
extern short vplsh; /*0-двух порядковый план счетов 1-многпорядковый*/
extern short	startgodb; /*Стартовый год*/
extern int kol_strok_na_liste;

void pvbanks_r(class pvbanks_rr *data_rr)
{
char strsql[512];
int gor=0;
int ver=0;
class iceb_u_str soob;
short dk,mk,gk;

pvbanks_r_data data;

data.rek_r=data_rr;
data.name_window.plus(__FUNCTION__);


//iceb_u_rsdat(&data.dk,&data.mk,&data.gk,data.rek_r->datan.ravno(),1);
iceb_u_rsdat(&dk,&mk,&gk,data.rek_r->datan.ravno(),1);
  


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать документы для перечисления на карт-счета"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(pvbanks_r_key_press),&data);

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);
sprintf(strsql,"%s %s",gettext("Расчёт с использованием проводок"),data.rek_r->shet.ravno());

soob.new_plus(strsql);

sprintf(strsql,"%s %d.%d.%d",gettext("Дата сальдо"),
dk,mk,gk);
soob.ps_plus(strsql);

GtkWidget *label=gtk_label_new(soob.ravno());

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
gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(pvbanks_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)pvbanks_r1,&data);

gtk_main();


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  pvbanks_r_v_knopka(GtkWidget *widget,class pvbanks_r_data *data)
{
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   pvbanks_r_key_press(GtkWidget *widget,GdkEventKey *event,class pvbanks_r_data *data)
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
/*Выполнение проводок*/
/***************************/
void pvbanks_make_prov(const char *shet,
const char *shet_kor,
short dr,short mr,short gr,
int metkadk,
const char *kontr,
const char *fio,
double sum,
GtkWidget *wpredok)
{

double deb=0.;
double kre=0.;

if(metkadk == 0) /*Дебет*/
 kre=sum;
if(metkadk == 1) /*кредит*/
 deb=sum;

class iceb_u_str koment(gettext("Перечислено"));
koment.plus(" ",fio);

//sprintf(koment,"%s %s",gettext("Перечислено"),fio);
 
iceb_zapprov(0,gr,mr,dr,shet,shet_kor,kontr,kontr,"","","",deb,kre,koment.ravno(),2,0,time(NULL),0,0,0,0,0,wpredok);
}

/***************************/
/*Распечатка для банка ПИБ*/
/***************************/
void pvbavks_pib(iceb_u_spisok *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int kolih_simv,const char *pris,const char *grupk,
const char *shet,
const char *shet_kor,
short dr,short mr,short gr,
int kod_banka,
GtkWidget *wpredok)
{
int  kolstrkartr=0;
FILE *ffsp;
FILE *ffdis;
char imafsp[64];
char imafdis[64];
class iceb_u_str bankshet("");
class iceb_u_str fio("");
class iceb_u_str tabnom("");
double sumalist=0;
double sum=0;
double itogoo=0.;
SQLCURSOR cur1;


sprintf(imafsp,"pib%d.lst",getpid());
sprintf(imafdis,"pibd%d.txt",getpid());

if((ffsp = fopen(imafsp,"w")) == NULL)
 {
  iceb_er_op_fil(imafsp,"",errno,wpredok);
  return;
 }
hrvnks(ffsp,&kolstrkartr);

if((ffdis = fopen(imafdis,"w")) == NULL)
 {
  iceb_er_op_fil(imafdis,"",errno,wpredok);
  return;
 }
int kolh=0;
int kolkontr=KONT->kolih();
int kol_na_liste=0;
class iceb_u_str inn("");

for(int snom=0 ; snom < kolkontr; snom++)
 {
  
  if(pvbanks_kprovw(KONT,snom,pris,metkadk,&sum,&tabnom,deb,kre,kolih_simv,&fio,&bankshet,&inn,grupk,kod_banka,wpredok) != 0)
   continue;

  pib_itlw(&kolstrkartr,&sumalist,&kol_na_liste,rukov,glavbuh,ffsp);

  sumalist+=sum;
  itogoo+=sum;
  kolh++;
  
  pib_strw(kolh,bankshet.ravno(),sum,fio.ravno(),inn.ravno(),ffsp);    
  
  kol_na_liste++;
  
  fprintf(ffdis,"%-16s %8.2f %s\n",bankshet.ravno(),sum,fio.ravno());
  if(dr != 0) /*нужно делать проводки*/
    pvbanks_make_prov(shet,shet_kor,dr,mr,gr,metkadk,KONT->ravno(snom),fio.ravno(), sum,wpredok);
 }

pib_end_dokw(kol_na_liste,sumalist,itogoo,kolh,rukov,glavbuh,ffsp);

fclose(ffsp);
fclose(ffdis);

//Записываем шапку и концовку
hdisk(kolh,itogoo,imafdis);

class iceb_u_spisok imafo;
class iceb_u_spisok naimo;

imafo.plus(imafsp);
naimo.plus(gettext("Распечатка документов для перечисления на карт-счета"));

imafo.plus(imafdis);
naimo.plus(gettext("Документ в электронном формате"));
iceb_ustpeh(imafo.ravno(0),1,wpredok);


}
/***************************/
/*Распечатка для Укрсоцбанка*/
/***************************/
void pvbavks_uks(iceb_u_spisok *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int kolih_simv,const char *pris,const char *grupk,
const char *shet,
const char *shet_kor,
short dr,short mr,short gr,
int kod_banka,
GtkWidget *wpredok)
{
FILE *ff;
FILE *fftmp;
char imaf[64];
char imaftmp[64];
char imafzag[64];
class iceb_u_str bankshet("");
class iceb_u_str fio("");
class iceb_u_str tabnom("");
double sumalist=0;
double sum=0;
double itogoo=0.;
char strsql[512];
SQLCURSOR cur1;


sprintf(imaftmp,"ukrsoc%d.tmp",getpid());
if((fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,wpredok);
  return;
 }

sprintf(imaf,"ukrsoc%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

time_t          vrem;

time(&vrem);
struct tm *bf=localtime(&vrem);

fprintf(ff,"\
   Зведена відомість сум для зарахування на спеціальні карткові рахунки N_____\n\
------------------------------------------------------------------------------\n\
| Дата                             |             %02d.%02d.%04d                  |\n\
| Назва організації                |%-41.41s|\n\
| Назва та N філії АКБ \"Укрсоцбанк\"|Вінницька облана філія N050              |\n\
| Додаткова інформація             |                                         |\n\
------------------------------------------------------------------------------\n",
bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,iceb_get_pnk("00",0,wpredok));

fprintf(ff,"\
--------------------------------------------------------------------------------------\n\
 N  |     Прізвище, ім'я по батькові         | Картковий счёт |Сумма до зарахування|\n\
--------------------------------------------------------------------------------------\n"); 
int kolh=0;
int kolkontr=KONT->kolih();
class iceb_u_str inn("");
int nomerstr=0;

for(int snom=0 ; snom < kolkontr; snom++)
 {

  if(pvbanks_kprovw(KONT,snom,pris,metkadk,&sum,&tabnom,deb,kre,kolih_simv,&fio,&bankshet,&inn,grupk,kod_banka,wpredok) != 0)
   continue;

  sumalist+=sum;
  itogoo+=sum;
  kolh++;
    

  fprintf(ff,"%-4d %-*.*s %-19.19s %10.2f\n",
  ++nomerstr,
  iceb_u_kolbait(40,fio.ravno()),iceb_u_kolbait(40,fio.ravno()),fio.ravno(),
  bankshet.ravno(),
  sum);
  
  fprintf(fftmp,"@%s,0,%.2f\n",bankshet.ravno(),sum);

  if(dr != 0) /*нужно делать проводки*/
    pvbanks_make_prov(shet,shet_kor,dr,mr,gr,metkadk,KONT->ravno(snom),fio.ravno(),sum,wpredok);

 }
fprintf(ff,"\
--------------------------------------------------------------------------------------\n"); 

fprintf(ff,"\
                                                          Всього: %10.2f\n",itogoo);

fprintf(ff,"\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",rukov,glavbuh);

fprintf(ff,"\n\
Відмітки банку:\n\
Дата приймання Зведеної відомості на паперовому носії та магнітному носії_____________\n\
Посада та прізвище, ім'я та по батькові працівника Банку, який прийняв Зведену відомість\n\n\
_____________________________________________________\n\n\
_____________________________________________________\n\n\
підпис_______________________________________________\n");

iceb_podpis(ff,wpredok);

fclose(ff);
fclose(fftmp);

if((fftmp = fopen(imaftmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,wpredok);
  return;
 }

sprintf(imafzag,"ukrsoc%d.txt",getpid());
if((ff = fopen(imafzag,"w")) == NULL)
 {
  iceb_er_op_fil(imafzag,"",errno,wpredok);
  return;
 }

fprintf(ff,"@29244050010001,0,%.2f\n\
@302010,0,0,0.00\n",itogoo);

while(fgets(strsql,sizeof(strsql),fftmp) != NULL)
  fprintf(ff,"%s",strsql);
  
fclose(ff);
fclose(fftmp);
unlink(imaftmp);

class iceb_u_spisok imafo;
class iceb_u_spisok naimo;

imafo.plus(imaf);
naimo.plus(gettext("Распечатка документов для перечисления на карт-счета"));

imafo.plus(imafzag);
naimo.plus(gettext("Документ в электронном формате"));
iceb_ustpeh(imafo.ravno(0),1,wpredok);
iceb_rabfil(&imafo,&naimo,NULL);

}
/**************************************/
/*Для файлов в формате txt*/
/****************************************/

void pvbanks_txt(int kod_banka,iceb_u_spisok *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int kolih_simv,const char *pris,const char *grupk,
const char *shet,
const char *shet_kor,
short dr,short mr,short gr,
const char *kod00,
int kod_banka_tabl,
GtkWidget *wpredok)
{
char imaf_lst[64];
char imaf_csv_txt[64];
char imaf_dbf[64];
double sum=0.;
class iceb_u_str fio("");
class iceb_u_str bankshet("");
class iceb_u_str inn("");
class iceb_u_str tabnom("");
double sumai=0.;
short d_pp=0,m_pp=0,g_pp=0; /*Дата платёжки*/
//class iceb_u_str vidnahis("Заробітна плата та аванси");
class iceb_u_str edrpou("");
int kolkontr=KONT->kolih();

char imaftmp[64];
FILE *ff_lst;
FILE *ff_csv_txt;
FILE *ff_dbf;
memset(imaf_dbf,'\0',sizeof(imaf_dbf));

if(kod_banka == 7) /*Укрэксимбанк*/
  if(ukreksim_hapw(imaf_lst,imaftmp,&ff_csv_txt,&ff_lst,wpredok) != 0)
   return;

if(kod_banka == 8) /*хресщатик*/
 {
  for(int snom=0 ; snom < kolkontr; snom++)
   {
    if(pvbanks_kprovw(KONT,snom,pris,metkadk,&sum,&tabnom,deb,kre,kolih_simv,&fio,&bankshet,&inn,grupk,kod_banka_tabl,wpredok) != 0)
     continue;

    sumai+=sum;
   }
  if(zvb_hreshatik_startw(imaf_lst,imaf_csv_txt,sumai,&d_pp,&m_pp,&g_pp,&ff_csv_txt,&ff_lst,wpredok) != 0)
   return;

 }
 
if(kod_banka == 9) /*ibank2ua*/
 if(zvb_ibank2ua_startw(imaf_lst,imaf_csv_txt,kod00,&ff_csv_txt,&ff_lst,wpredok) != 0)
  return;
if(kod_banka == 16) /*ELpay*/
 if(zvb_elpay_startw(imaf_lst,imaf_csv_txt,imaf_dbf,&edrpou,kod00,&ff_csv_txt,&ff_lst,&ff_dbf,wpredok) != 0)
  return;

short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
int nomer=0;
sumai=0.;

for(int snom=0 ; snom < kolkontr; snom++)
 {

  if(pvbanks_kprovw(KONT,snom,pris,metkadk,&sum,&tabnom,deb,kre,kolih_simv,&fio,&bankshet,&inn,grupk,kod_banka_tabl,wpredok) != 0)
   continue;

  sumai+=sum;

  if(kod_banka == 7) /*Укрэксимбанк*/
    ukreksim_zstrw(ff_csv_txt,ff_lst,&nomer,sum,fio.ravno(),bankshet.ravno(),tabnom.ravno(),dt,mt,gt);

  if(kod_banka == 8) /*хресщатик*/
    zvb_hreshatik_strw(d_pp,m_pp,g_pp,&nomer,fio.ravno(),bankshet.ravno(),inn.ravno(),sum,ff_csv_txt,ff_lst);

  if(kod_banka == 9) /*ibank2ua*/
   zvb_ibank2ua_strw(&nomer,tabnom.ravno(),fio.ravno(),bankshet.ravno(),inn.ravno(),sum,ff_csv_txt,ff_lst);

  if(kod_banka == 16) /*ELpay*/
   zvb_elpay_strw(&nomer,tabnom.ravno(),fio.ravno(),bankshet.ravno(),inn.ravno(),sum,edrpou.ravno(),ff_csv_txt,ff_lst,ff_dbf);

  if(dr != 0) /*нужно делать проводки*/
      pvbanks_make_prov(shet,shet_kor,dr,mr,gr,metkadk,KONT->ravno(snom),fio.ravno(),sum,wpredok);

 }

if(kod_banka == 7) /*Укрэксимбанк*/
 {
  ukreksim_endw(nomer,imaftmp,imaf_csv_txt,sumai,rukov,glavbuh,ff_csv_txt,ff_lst,wpredok);
  iceb_ustpeh(imaf_lst,1,wpredok);
 }

if(kod_banka == 8) /*хресщатик*/
 {
  zvb_hreshatik_endw(sumai,nomer,ff_lst,ff_csv_txt,wpredok);
  iceb_ustpeh(imaf_lst,1,wpredok);
 }

if(kod_banka == 9) /*ibank2ua*/
 zvb_ibank2ua_endw(imaf_lst,sumai,ff_lst,ff_csv_txt,wpredok);

if(kod_banka == 16) /*ELpay*/
  zvb_elpay_endw(imaf_lst,imaf_csv_txt,imaf_dbf,sumai,nomer,ff_lst,ff_csv_txt,ff_dbf,wpredok);

class iceb_u_spisok imafo;
class iceb_u_spisok naimo;

imafo.plus(imaf_lst);
naimo.plus(gettext("Ведомость перечисления на карт-счета"));

imafo.plus(imaf_csv_txt);
naimo.plus(gettext("Документ в электронном формате"));
if(imaf_dbf[0] != '\0')
 {
  imafo.plus(imaf_dbf);
  naimo.plus(gettext("Документ в электронном формате dbf"));
 }
 

iceb_rabfil(&imafo,&naimo,NULL);

}
/**************************************/
/*Для банков у которых електронный формат dbf*/
/****************************************/

void pbanks_dbf(int metka_banka, /*1-ukrsib 2-ukrgaz*/
iceb_u_spisok *KONT,int metkadk,double *deb,double *kre,const char *glavbuh,const char *rukov,int kolih_simv,const char *pris,const char *grupk,
const char *shet,
const char *shet_kor,
short dr,short mr,short gr,
int kod_banka,
GtkWidget *wpredok)
{
char imaf_lst[64];
char imafdis[64];

int kolkontr=KONT->kolih();

char imaftmp[64];
FILE *ff_lst;
FILE *fftmp;
class iceb_u_str ope_tor("");
static class iceb_u_str branch("");
static class iceb_u_str zpkod("");
static class iceb_u_str rlkod("");

if(metka_banka == 4)
 if(ukrsib_startw(imaf_lst,imaftmp,&ope_tor,&fftmp,&ff_lst,wpredok) != 0)
  return;

if(metka_banka == 11)
 if(zvb_ukrgaz_startw(imaf_lst,imafdis,imaftmp,&fftmp,&ff_lst,wpredok) != 0)
  return;

if(metka_banka == 13)
 if(zvb_kievrus_startw(imaf_lst,imafdis,imaftmp,&fftmp,&ff_lst,wpredok) != 0)
  return;
if(metka_banka == 14)
 if(zvb_corp2_startw(imaf_lst,imafdis,imaftmp,&fftmp,&ff_lst,wpredok) != 0)
  return;

if(metka_banka == 15)
 if(zvb_privatbank_startw(imaf_lst,imafdis,imaftmp,&branch,&zpkod,&rlkod,&fftmp,&ff_lst,wpredok) != 0)
  return;

double sum=0.;
class iceb_u_str fio("");
class iceb_u_str bankshet("");
class iceb_u_str inn("");
class iceb_u_str tabnom("");
short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);
int nomer=0;
double sumai=0.;

for(int snom=0 ; snom < kolkontr; snom++)
 {

  if(pvbanks_kprovw(KONT,snom,pris,metkadk,&sum,&tabnom,deb,kre,kolih_simv,&fio,&bankshet,&inn,grupk,kod_banka,wpredok) != 0)
   continue;

  sumai+=sum;
  if(metka_banka == 4)
    ukrsib_strw(dt,mt,gt,sum,bankshet.ravno(),fio.ravno(),&nomer,tabnom.ravno(),ope_tor.ravno(),ff_lst,fftmp);
  if(metka_banka == 11)
    zvb_ukrgaz_strw(++nomer,fio.ravno(),bankshet.ravno(),sum,inn.ravno(),tabnom.ravno(),fftmp,ff_lst);
  if(metka_banka == 13)
    zvb_kievrus_strw(++nomer,fio.ravno(),bankshet.ravno(),sum,inn.ravno(),tabnom.ravno(),fftmp,ff_lst);
  if(metka_banka == 14)
    zvb_corp2_strw(++nomer,fio.ravno(),bankshet.ravno(),sum,inn.ravno(),tabnom.ravno(),fftmp,ff_lst);
  if(metka_banka == 15)
    zvb_privatbank_strw(++nomer,fio.ravno(),bankshet.ravno(),sum,inn.ravno(),tabnom.ravno(),branch.ravno(),zpkod.ravno(),rlkod.ravno(),fftmp,ff_lst);

  if(dr != 0) /*нужно делать проводки*/
      pvbanks_make_prov(shet,shet_kor,dr,mr,gr,metkadk,KONT->ravno(snom),fio.ravno(),sum,wpredok);
 }

if(metka_banka == 4)
 {
  ukrsib_endw(imafdis,imaftmp,sumai,nomer,rukov,glavbuh,ff_lst,fftmp,wpredok);
  iceb_ustpeh(imaf_lst,1,wpredok);
 }
if(metka_banka == 11)
 {
  zvb_ukrgaz_endw(imafdis,imaftmp,sumai,nomer,ff_lst,fftmp,wpredok);
  iceb_ustpeh(imaf_lst,1,wpredok);
 }
if(metka_banka == 13)
 {
  zvb_kievrus_endw(imafdis,imaftmp,sumai,nomer,ff_lst,fftmp,wpredok);
  iceb_ustpeh(imaf_lst,1,wpredok);
 }
if(metka_banka == 14)
  zvb_corp2_endw(imafdis,imaftmp,imaf_lst,sumai,nomer,ff_lst,fftmp,wpredok);

if(metka_banka == 15)
  zvb_privatbank_endw(imafdis,imaftmp,sumai,nomer,ff_lst,fftmp,wpredok);

class iceb_u_spisok imafo;
class iceb_u_spisok naimo;

imafo.plus(imaf_lst);
naimo.plus(gettext("Ведомость перечисления на карт-счета"));

imafo.plus(imafdis);
naimo.plus(gettext("Ведомость перечисления в электронном формате"));


iceb_rabfil(&imafo,&naimo,NULL);

}

/******************************************/
/******************************************/

gint pvbanks_r1(class pvbanks_r_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);
char strsql[512];
int kolstr;
class SQLCURSOR cur;
SQL_str row;
//printf("pvbanks_r1 data->prohod=%d %d %f \n",data->prohod,data->kolstr,data->kolstr1);
if((data->rek_r->kod_banka=zarbanksw(&data->rek_r->kod00,&data->rek_r->kod_banka_tabl,data->window)) <= 0)
 {
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int kolshet=iceb_u_pole2(data->rek_r->shet.ravno(),',');

//Составляем список всех контрагентов
if(kolshet == 0)
  sprintf(strsql,"select ns,kodkon from Skontr where ns='%s'",data->rek_r->shet.ravno());
else
  sprintf(strsql,"select ns,kodkon from Skontr");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int kolkontr=0;
class iceb_u_spisok KONT;
float kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(iceb_u_proverka(data->rek_r->shet.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[1],2,0) != 0)
    continue;
  if(KONT.find_r(row[1]) < 0)
   {
    kolkontr++;
    KONT.plus(row[1]);
   }
 }
double deb[kolkontr];
double kre[kolkontr];
memset(&deb,'\0',sizeof(deb));
memset(&kre,'\0',sizeof(kre));
short dk,mk,gk;
iceb_u_rsdat(&dk,&mk,&gk,data->rek_r->datan.ravno(),1);

short       gods=startgodb;
if(gods == 0)
 gods=gk;

//Читаем сальдо
if(kolshet == 0)
  sprintf(strsql,"select ns,kodkon,deb,kre from Saldo where kkk=1 and gs=%d and ns='%s'",gods,data->rek_r->shet.ravno());
else
  sprintf(strsql,"select ns,kodkon,deb,kre from Saldo where kkk=1 and gs=%d",gods);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
int nomer=0;
kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(iceb_u_proverka(data->rek_r->shet.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[1],2,0) != 0)
    continue;

  nomer=KONT.find_r(row[1]);
  deb[nomer]+=atof(row[2]);
  kre[nomer]+=atof(row[3]);
 }
//Читаем все проводки
if(kolshet == 0)
  sprintf(strsql,"select sh,kodkon,deb,kre from Prov where datp >= '%d-%d-%d' \
and datp <= '%d-%d-%d' and kodkon <> '' and sh='%s'",gods,1,1,gk,mk,dk,data->rek_r->shet.ravno());
else
  sprintf(strsql,"select sh,kodkon,deb,kre from Prov where datp >= '%d-%d-%d' \
and datp <= '%d-%d-%d' and kodkon <> ''",gods,1,1,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
kolstr1=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  if(iceb_u_proverka(data->rek_r->shet.ravno(),row[0],0,0) != 0)
    continue;
  if(iceb_u_proverka(data->rek_r->kontr.ravno(),row[1],2,0) != 0)
    continue;

  nomer=KONT.find_r(row[1]);
  deb[nomer]+=atof(row[2]);
  kre[nomer]+=atof(row[3]);
 }

gdite.close();

class iceb_u_str rukov("");
class iceb_u_str glavbuh("");
int kolih_simv;
SQL_str row1;
SQLCURSOR cur1;


kolih_simv=iceb_u_strlen(ICEB_ZAR_PKTN);
class iceb_u_str bros("");
iceb_poldan("Табельный номер руководителя",&bros,"zarnast.alx",data->window);
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    rukov.new_plus(row1[0]);
 } 

iceb_poldan("Табельный номер бухгалтера",&bros,"zarnast.alx",data->window);
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio from Kartb where tabn=%s",bros.ravno());
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    glavbuh.new_plus(row1[0]);
 }

short dr,mr,gr;
iceb_u_rsdat(&dr,&mr,&gr,data->rek_r->datap.ravno(),1);
switch(data->rek_r->kod_banka)
 {
  case 1: /*Проминвестбанк*/
    pvbavks_pib(&KONT,data->rek_r->metka_r,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod_banka_tabl,data->window);
    break;

  case 3: /*Укрсоцбанк*/
    pvbavks_uks(&KONT,data->rek_r->metka_r,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod_banka_tabl,data->window);
    break;

  case 4: /*Укрсиббанк*/
    pbanks_dbf(data->rek_r->kod_banka,&KONT,data->rek_r->metka_r,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod_banka_tabl,data->window);
    break;

  case 7: /*Укрэксимбанк*/
    pvbanks_txt(data->rek_r->kod_banka,&KONT,data->rek_r->metka_r,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod00.ravno(),data->rek_r->kod_banka_tabl,data->window);
    break;

  case 8: /*хресщатик*/
    pvbanks_txt(data->rek_r->kod_banka,&KONT,data->rek_r->metka_r,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod00.ravno(),data->rek_r->kod_banka_tabl,data->window);
    break;

  case 9: /*ibank2ua*/
    pvbanks_txt(data->rek_r->kod_banka,&KONT,data->rek_r->metka_r,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod00.ravno(),data->rek_r->kod_banka_tabl,data->window);
    break;

  case 11: /*Укргазбанк*/
    pbanks_dbf(data->rek_r->kod_banka,&KONT,data->rek_r->metka_r,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod_banka_tabl,data->window);
    break;

  case 13: /*Киевская русь*/
    pbanks_dbf(data->rek_r->kod_banka,&KONT,data->rek_r->metka_r,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod_banka_tabl,data->window);
    break;

  case 14: /*Система CORP2 "Державний ощадний банк України*/
    pbanks_dbf(data->rek_r->kod_banka,&KONT,data->rek_r->metka_r,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod_banka_tabl,data->window);
    break;

  case 15: /*Приватбанк*/
    pbanks_dbf(data->rek_r->kod_banka,&KONT,data->rek_r->metka_r,deb,kre,glavbuh.ravno(),rukov.ravno(),kolih_simv,ICEB_ZAR_PKTN,data->rek_r->grupk.ravno(),data->rek_r->shet.ravno(),data->rek_r->shet_kor.ravno(),dr,mr,gr,data->rek_r->kod_banka_tabl,data->window);
    break;

  default:
   sprintf(strsql,"%s %d!",gettext("Не известный номер банка"),data->rek_r->kod_banka);
   iceb_menu_soob(strsql,data->window);
   break;  
 }

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);
return(FALSE);

}
