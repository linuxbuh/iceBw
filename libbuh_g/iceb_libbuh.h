/*$Id: iceb_libbuh.h,v 1.296 2014/08/31 06:19:25 sasa Exp $*/
/*06.08.2019	07.09.2003	Белых А.И.	iceb_libbuhg.h
*/
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <libsql.h>
#include <iceb_util.h>
#include <nl_types.h>
#include "../iceBw.h"
#include "iceb_get_dev.h"
#include "iceb_gdite.h"
#include "iceb_razuz.h"
#include "iceb_fioruk.h"
#include "iceb_avp.h"
#include "iceb_tmptab.h"
#include "iceb_rnfxml.h"
#include "iceb_fopen.h"
#include "iceb_getkue.h"
#include "iceb_imp_xml_nn.h"
#include "iceb_vstavfil.h"
#include "iceb_vrvr.h"
#include "iceb_rnl.h"

#ifdef READ_CARD
#include        <torgdev_client.h>
extern int iceb_read_card(tdcon td_server,char *kod_kart,int razmer,int time_out,GtkWidget *wpredok);
#endif

#include "iceb_menu_vibknop.h"
#include "iceb_lock_tables.h"
#include "iceb_rsn.h"
#include "iceb_sql_flag.h"
#include "iceb_kassa.h"
#include "iceb_read_card_enter.h"

#define ICEB_UMASK 0111 /*Маска для каталогов всем читать и писать*/
#define ICEB_DATA_BAZ "15.05.2020" /*Дата последнего преобразования базы*/

//Располпжение окон на экране
#define ICEB_POS_CENTER GTK_WIN_POS_CENTER
//#define  ICEB_POS_CENTER GTK_WIN_POS_CENTER_ALWAYS
//#define  ICEB_POS_CENTER GTK_WIN_POS_CENTER_ON_PARENT

//Вид курсора
#define  ICEB_CURSOR       GDK_TOP_LEFT_ARROW //Обычный курсор
#define  ICEB_CURSOR_GDITE GDK_WATCH          //Курсор указывающий на то, что программа работает и надо подождать
#define  ICEB_KAT_ALX      ".iceB"             /*Имя каталога который создаётся в домашнем каталоге пользователя для настроечных файлов*/
#define  ICEB_IMAF_SIZW    "sizwin.alx"       /*Имя файла в котором будут записаны размеры окон*/
//Клавиша нажимаемая совместно с F-клавишами
//#define  ICEB_REG_L      GDK_Control_L
//#define  ICEB_REG_R      GDK_Control_R

//#define  ICEB_REG_L      GDK_Shift_L
//#define  ICEB_REG_R      GDK_Shift_R
#define  ICEB_REG_L      GDK_KEY_Shift_L
#define  ICEB_REG_R      GDK_KEY_Shift_R
#define  ICEB_PUT_NA_HTML "http://iceb.net.ua/dochtml/iceBw"
//Символ перед F клавишей при изменении регистра
#define  RFK  "S"

#define  SRIFT_RAVNOHIR "Monospace 10"
#include "iceb_clock.h"

const short ICEB_PS_GK =  1; /*Код подсистемы "Главная книга"*/
const short ICEB_PS_MU =  2; /*Код подсистемы "Материальный учёт"*/
const short ICEB_PS_PD =  3; /*Код подсистемы "Платёжные документы"*/
const short ICEB_PS_ZP =  4; /*Код подсистемы "Заработная плата"*/
const short ICEB_PS_UOS=  5; /*Код подсистемы "Учёт основных средств"*/
const short ICEB_PS_UU =  6; /*Код подсистемы "Учёт услуг"*/
const short ICEB_PS_UKO=  7; /*Код подсистемы "Учёт кассовых ордеров"*/
const short ICEB_PS_UKR=  8; /*Код подсистемы "Учёт командировочных расходов"*/
const short ICEB_PS_UPL=  9; /*Код подсистемы "Учёт путевых листов"*/
const short ICEB_PS_RNN= 10; /*Код подсистемы "Реестр налоговых накладных"*/
const short ICEB_PS_UD = 11; /*Код подсистемы "Учёт доверенностей"*/
const short ICEB_PS_ABD = 12; /*Код подсистемы "Администрирование баз данных"*/

const short ICEB_KOD_OP_PROG=99; /*Код оператора для действий выполненных программой без участия оператора*/
#define ICEB_LOGIN_OP_PROG "l99"

#define ICEB_MP_ZARP "ЗП" /*Метка проводки выполненной из подсистемы "Заработная плата"*/
#define ICEB_MP_MATU "МУ" /*Метка проводки выполненной из подсистемы "Материальный учёт"*/
#define ICEB_MP_UOS "УОС" /*Метка проводки выполненной из подсистемы "Учёт основных средств"*/
#define ICEB_MP_PPOR "ПЛТ" /*Метка проводки выполненной из подсистемы "Платёжные документы" для платёжных поручений*/
#define ICEB_MP_PTRE "ТРЕ" /*Метка проводки выполненной из подсистемы "Платёжные документы" для платёжных требований*/
#define ICEB_MP_KASA "КО" /*Метка проводки выполненной из подсистемы "Учёт кассовых ордеров"*/
#define ICEB_MP_USLUGI "УСЛ" /*Метка проводки выполненной из подсистемы "Учёт услуг"*/
#define ICEB_MP_UKR "УКР" /*Метка проводки выполненной из подсистемы "Учёт командировочных расходов"*/

#define ICEB_MP_GK "УКР" /*Метка подсистемы "Главная книга" в реестре налоговых накладных*/

#define ICEB_ZAR_PKTN "тн" /*приставка к табельному номеру*/

class spis_oth
 {
  public:
   class iceb_u_spisok spis_imaf; /*Список имён файлов с отчётами*/
   class iceb_u_spisok spis_naim; /*Список наименований отчётов*/
   class iceb_u_spisok spis_ms;   /*список имён файлов у которых метка сохранить*/  
   class iceb_u_spisok spis_mk;   /*список имён файлов у которых метка кодировки windows-1251*/
  void clear()
   {
    spis_imaf.free_class();
    spis_naim.free_class();
    spis_ms.free_class();
    spis_mk.free_class();
   }
 };


//Структура со всеми реквизитами счета
struct OPSHET
 {
  class iceb_u_str naim;//Наименование
  short tips;    // 0-активный 1-пассивный 2-активно-пассивный
  short vids;    // 0-счет 1-субсчет
  short saldo;   // 0-сальдо свернутое 3-сальдо развернутое
  short stat;    // 0-балансовый 1-не балансовый
  short val;     // 0-национальная валюта или номер валюты из справочника валют
 };

class akt_sverki //Для расчёта акта сверки по контрагентам
 {
  public:
   class iceb_u_spisok data_nomd; //Дата|номер документа
   class iceb_u_spisok koment;    //коментарий
   class iceb_u_double suma_deb; 
   class iceb_u_double suma_kre; 
   double start_saldo_deb;
   double start_saldo_kre;
   class iceb_u_str shet;  
   short dn,mn,gn;
     
  akt_sverki()
   {
    start_saldo_deb=start_saldo_kre=0.;
    shet.plus("");
    dn=mn=gn=0;
   }
  void clear()
   {
    start_saldo_deb=start_saldo_kre=0.;
    shet.new_plus("");
    suma_deb.free_class();
    suma_kre.free_class();
    data_nomd.free_class();
    koment.free_class();    
    dn=mn=gn=0;
   }
 };



int iceb_dirfajl(const char *name,int mi,class iceb_u_spisok *mfa,GtkWidget *wpredok);
void iceb_dirlst(const char *tfima,GtkWidget *wpredok);
void      iceb_msql_error(SQL_baza*,const char*,const char*,GtkWidget*);
void      iceb_menu_soob(iceb_u_spisok*,GtkWidget*);
void      iceb_menu_soob(iceb_u_str*,GtkWidget*);
void      iceb_menu_soob(const char*,GtkWidget*);
void      iceb_eropbaz(const char*,int,const char*,int);
void      iceb_infosys(const char*,const char*,const char*,short,short,short,const char*,iceb_u_str*,int);
void      iceb_readkey(int,char**,char**,char**,char**);
void      iceb_get_text_str(GtkWidget*,iceb_u_str*);
int       iceb_menu_danet(iceb_u_spisok*,int,GtkWidget*);
int       iceb_menu_danet(iceb_u_str*,int,GtkWidget*);
int       iceb_menu_danet(const char*,int,GtkWidget*);
gboolean  iceb_vihod(GtkWidget*,GdkEventKey*,int*);
void      iceb_podpis(FILE*,GtkWidget*);
void      iceb_podpis(uid_t,FILE*,GtkWidget*);
gboolean  iceb_key_release(GtkWidget *,GdkEventKey *,int *);
void iceb_rabfil(class spis_oth *oth,GtkWidget *wpredok);
void      iceb_rabfil(iceb_u_spisok*,iceb_u_spisok*,GtkWidget*);
void      iceb_ponp(const char*,const char*,GtkWidget*);
int       iceb_get_new_kod(const char*,int,GtkWidget*);
int       iceb_get_new_kod(const char*,const char*,int,GtkWidget*);

int       iceb_menu_vvod1(const char*,iceb_u_str*,int,const char *helpfil,GtkWidget*);
int       iceb_menu_vvod1(iceb_u_spisok*,iceb_u_str*,int,const char *helpfil,GtkWidget*);
int       iceb_menu_vvod1(const char*,char*,int,const char *helpfil,GtkWidget*);
int       iceb_menu_vvod1(iceb_u_str*,iceb_u_str*,int,const char *helpfil,GtkWidget*);
int       iceb_menu_vvod1(iceb_u_str*,char*,int,const char *helpfil,GtkWidget*);

int       iceb_parol(int,GtkWidget*);
int       iceb_parol(int,class iceb_u_spisok*,GtkWidget*);
int       iceb_parol(int metkap,class iceb_u_spisok *text_menu,class iceb_u_str *parol_voz,GtkWidget *wpredok);
int       iceb_parol(class iceb_u_str *parol,GtkWidget *wpredok);
int	 iceb_sql_zapis(const char *,int,int,GtkWidget*);
void      iceb_er_op_fil(const char *,const char *,int,GtkWidget*);
void      iceb_pbar(GtkWidget *,int,gfloat);
int       iceb_dikont(const char,const char *,const char *,const char *);
int iceb_vizred(const char *,GtkWidget*);
int iceb_vizred(const char *imaf,const char *editor,GtkWidget *wpredok);
void      iceb_spks(const char*,const char*,GtkWidget*);

int       iceb_rsdatp(short *,short *,short *,const char *,short *,short *,short *,const char *,GtkWidget*);
int       iceb_rsdatp(const char *datn,const char *datk,GtkWidget*);
int       iceb_rsdatp(const char *datn,class iceb_u_str *datk,GtkWidget*);
int iceb_rsdatp1(short *mn,short *gn,const char *datan,short *mk,short *gk,const char *datak,GtkWidget *wpredok);

int       iceb_rsdatp_str(iceb_u_str *datan,iceb_u_str *datak,GtkWidget *wpredok);
void      iceb_zagacts(short dn,short mn,short gn,short dk,short mk,short gk,const char *kodkontr,const char *naimkont,FILE *ff);
void	 iceb_konact(FILE *ff,GtkWidget*);
void	 iceb_zagsh(const char *shet,FILE *ff,GtkWidget*);
void      iceb_salorksl(double dd, double kk,double ddn,double kkn,double debm,double krem,FILE *ff1,FILE *ffact);
void      iceb_printw(const char *stroka,GtkTextBuffer *buffer,GtkWidget *view);
void iceb_printw(const char *stroka,GtkTextBuffer *buffer,GtkWidget *view,const char *name_tag);
void      iceb_printw_vr(time_t vrem_n,GtkTextBuffer *buffer,GtkWidget *view);
int       iceb_prsh(const char*,OPSHET *,GtkWidget*);
int       iceb_prsh1(const char *shet,OPSHET *shetv,GtkWidget *wpredok);
int       iceb_prsh1(const char *shet,GtkWidget *wpredok);
int       iceb_l_kontr(int,iceb_u_str *,iceb_u_str *,GtkWidget  *);
int       iceb_l_kontrsh(int,const char*,iceb_u_str *,iceb_u_str *,GtkWidget *);
int iceb_f_redfil(const char *imafil,int,GtkWidget *wpredok);
int iceb_f_redfil(const char *imafil,int,const char *editor,GtkWidget *wpredok);
int       iceb_l_gkontr(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int       iceb_l_plansh(int,iceb_u_str *,iceb_u_str *,GtkWidget  *);
int iceb_prblm(short m,short g,const char *imaf,const char *soobstr,GtkWidget *wpredok);
int       iceb_zapprov(int,int,int,int,const char *,const char *,const char *,const char *,const char *,const char *,const char *,double,double,const char *,short,int,time_t,short,short,short,int,int,GtkWidget *);
void      iceb_menu_vibknop(GtkWidget *widget,struct vibknop_data *data);
int       iceb_menu_mv(iceb_u_str *titl,iceb_u_str *zagolovok,iceb_u_spisok *punkt_menu,int,GtkWidget *wpredok);
int       iceb_menu_mv(const char *titl,const char *zagolovok,iceb_u_spisok *punkt_menu,int,GtkWidget *wpredok);
int       iceb_vibrek(int,const char *tablica,iceb_u_str *kod,GtkWidget *wpredok);
int       iceb_vibrek(int,const char *tablica,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int       iceb_vibrek(int,const char *tablica,char *kod,int,GtkWidget *wpredok);
int       iceb_udprov(int,short,short,short,const char *,const char *,time_t,double,double,const char *,short,GtkWidget *);
int       iceb_udprov(int,const char*,const char *,const char *,time_t,double,double,const char *,short,GtkWidget *);
int       iceb_klient_pvu(const char *kodkl,GtkWidget *wpedok);
void      iceb_klient_kor(const char *skod,const char *nkod);
int       iceb_sql_readkey(const char *zapros,SQL_str *row,SQLCURSOR *cur,GtkWidget *wpredok);
int       iceb_sql_readkey(const char *zapros,GtkWidget *wpredok);
void      iceb_printcod(FILE *,const char *,const char *,const char *,int,const char *,int *);
int       iceb_rsn(class SYSTEM_NAST *data,GtkWidget *wpredok);
//void      iceb_l_rsfr(GtkWidget *wpredok);
int       iceb_rmenuf(char *imafil,iceb_u_spisok *kol1,iceb_u_spisok *kol2,GtkWidget *wpredok);
int       iceb_l_spisokm(iceb_u_spisok *kol1,iceb_u_spisok *kol2,iceb_u_spisok *naim_kol,iceb_u_str*,int nom_str,GtkWidget *wpredok);
int   iceb_smenabaz(int mopen,GtkWidget *wpredok);
int   iceb_smenabaz_m(iceb_u_str *imabaz,iceb_u_str *host);
void      iceb_l_printcap(GtkWidget *wpredok);
int       iceb_runsystem(char *komanda,char*,GtkWidget *wpredok);
int       iceb_l_grupmat(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int       iceb_l_ei(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int       iceb_l_forop(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int       iceb_l_sklad(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int       iceb_l_opmup(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int       iceb_l_opmur(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);

int       iceb_print(const char *imaf,int kolkop,const char *name_print,const char *kluh_peh,GtkWidget *wpredok);
int       iceb_print(const char *imaf,GtkWidget *wpredok);

void      iceb_str_poisk(iceb_u_str *stroka,const char *data,const char *naim);
void      iceb_refresh(void);
void      iceb_kodkon(char *kodkon,char *str);
void      iceb_kodkon(class iceb_u_str *kodkon,char *str);
int       iceb_rstimep_str(iceb_u_str *vremn,iceb_u_str *vremk,GtkWidget*);
void      iceb_mesc(short mes,short skl,char *naz);
void      iceb_mesc(short mes,short skl,class iceb_u_str *naz);
char      *iceb_prcn(double cn);
int       iceb_udprgr(const char *kto,short dd,short md,short gd,const char *nn,int pod,int tipz,GtkWidget *wpredok);
int       iceb_provinnom(const char *innom,GtkWidget *wpredok);
int       iceb_invnomer(GtkWidget *wpredok);

void      iceb_nomnak(short g,const char *skll,iceb_u_str *ndk,int tz,int metka,int,GtkWidget *wpredok);
void      iceb_nomnak(const char*,const char *skll,iceb_u_str *ndk,int tz,int metka,int,GtkWidget *wpredok);

int iceb_nomdok(short god,const char *tabl,GtkWidget *wpredok);

int       iceb_00_skl(const char *kontr);
void      iceb_00_kontr(const char *kontr_old,iceb_u_str *kontr_new);
void      iceb_get_menu(GtkWidget *widget,int *data);

int iceb_vprov(const char *metkasys,const char *shet_d,const char *shet_k,const char *suma,const char *data_prov,const char *koment,
  const char *nomdok,const char *kodop,int sklad,short dd,short md,short gd,const char *kontr,time_t vremz,int ktozap,int pods,int tipz,int val,int kekv,GtkWidget *wpredok);

int iceb_provsh(class iceb_u_str *kor,const char *shp4,struct OPSHET *shetv,int metka,int metkanbs,GtkWidget *wpredok);
double iceb_sumvpr(const char *sh,const char *shk,short d,short m,short g,const char *nn,int skk,const char *kom,time_t vrem,int metkaprov,const char *kto);
void iceb_zapmpr(short g,short m,short d,const char *sh,const char *shk,const char *kor1,const char *kor2,double deb,double kre,const char *kom,int kolpr,int kekv,class iceb_u_spisok*,class iceb_u_double*,FILE*);
void iceb_zapmpr1(const char *nn,const char *kop,int skk,time_t vrem,const char *kto,short dd,short md,short gd,int,class iceb_u_spisok*,class iceb_u_double*,FILE*,GtkWidget *wpredok);
void	iceb_raspprov(const char *zapros,short dd,short md,short gd,const char *nomd,int pods,GtkWidget *wpredok);
void	iceb_pehf(const char *imaf,short kk,GtkWidget *wpredok);
void	iceb_kasord1(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,double suma,const char *fio,const char *osnov,const char *dopol,const char*,GtkWidget *wpredok);

void	iceb_kasord2(const char *imaf,const char *nomd,short dd,short md,short gd,const char *shet,const char *shetkas,
  double suma,const char *fio,const char *osnov,const char *dopol,const char *dokum,const char *hcn,FILE *ff,GtkWidget *wpredok);

int iceb_nalndoh(GtkWidget *wpredok);
int iceb_razuz(class iceb_razuz_data *data,GtkWidget*);
int iceb_razuz1(class iceb_razuz_data *data,GtkWidget*);
int iceb_razuz_kod(class iceb_razuz_data *data,GtkWidget*);
int	iceb_getnkas(int autoid,KASSA *kasr);
int	iceb_errorkas(int errn,GtkWidget *wpredok);
int	iceb_errorkas(KASSA *kasr, char *str,GtkWidget *wpredok);
int      iceb_cmd(KASSA *kasr, char *cmd,GtkWidget *wpredok);
int      iceb_p_kol_strok_list(GtkWidget*);
int  iceb_file_selection(iceb_u_str *imaf_v,GtkWidget *wpredok);
int      iceb_nastsys(void);
int  iceb_kolstrok(int);
int  iceb_mous_klav(char *nadpis,iceb_u_str *stroka_data,int max_dlinna_str,int metka_r,int metka_parol,int,GtkWidget *wpredok);
int iceb_exit(int);
void    iceb_rab_kas(struct KASSA *kasr,int);
int iceb_mariq_vs(int metka,struct KASSA *kasr);
int iceb_mariq_zvs(int metka,double suma,int vidnal,struct KASSA *kasr,GtkWidget *wpredok);
int	iceb_mariq_slvi(struct KASSA *kasr,double *suma,int metka);
int iceb_calendar(short *dv,short *mv,short *gv,GtkWidget *wpredok);
int iceb_calendar1(class iceb_u_str *mes_god,GtkWidget *wpredok);
int iceb_calendar(iceb_u_str*,GtkWidget *wpredok);
void iceb_start(int argc,char **argv);
void iceb_read_card_enter(class iceb_read_card_enter_data *data);
int iceb_connect_dserver(const char*,const char*);
void iceb_close_dserver(void);
int iceb_perzap(int metka,GtkWidget *wpredok);
int iceb_get_dev(const char *imafil);
int   	iceb_nomnalnak(short mes,short god,GtkWidget*);
int	iceb_provnomnnak(short mes,short god,const char *nomnalnak,GtkWidget *wpredok);
void iceb_snanomer(int kolzap,int *snanomer,GtkWidget *treeview);
void iceb_gdite(class iceb_gdite_data *data,int,GtkWidget *wpredok);
void iceb_prosf(const char *imaf,GtkWidget*);
int iceb_menu_mes_god(class iceb_u_str *mes_god,const char *nadpis,const char *imaf_nast,GtkWidget *wpredok);
int iceb_spis_komp(const char *imaf,GtkWidget *wpredok);
int   iceb_l_subbal(int metka_rr,iceb_u_str *kod,iceb_u_str *naim,GtkWidget *wpredok);
int iceb_menu_import(iceb_u_str *imafz,const char *zagolov,const char *imaf_nast,const char *imaf_html,GtkWidget *wpredok);
void iceb_start_rf();

void iceb_pm_pr(GtkWidget **opt,int metka_pr);

void iceb_pm_nds(const char *data,GtkWidget **opt,int nds);
void iceb_pm_nds(short d,short m,short g,GtkWidget **opt,int nds);

void iceb_pm_vibor(class iceb_u_spisok *strmenu,GtkWidget **opt,int nomer_str);
void iceb_pm_vibor_enter(class iceb_u_spisok *strmenu,GtkWidget **opt,const char *text_in_enter);
int iceb_prov_iz(int kod,GtkWidget *wpredok);
int iceb_prospp(const char *imaf,GtkWidget*);
void iceb_salorok(const char *kontr,GtkWidget *wpredok);
void iceb_akt_sv(class akt_sverki *akt_svr,FILE *ff,FILE *ff_bi);
void iceb_hasv(char *shet,FILE *ff,GtkWidget *wpredok);
int iceb_pbp(int metka_ps,short dd,short md,short gd,const char *nomdok,int pod,int tipz,const char *soob,GtkWidget *wpredok);
int iceb_pbp(const char *kto,short dd,short md,short gd,const char *nomdok,int pod,int tipz,const char *soob,GtkWidget *wpredok);
int iceb_pbpds(short mes,short god,GtkWidget *wpredok);
int iceb_pbpds(short mes,short god,int podsystem,GtkWidget *wpredok);
int iceb_pbpds(const char *data,GtkWidget *wpredok);
int iceb_pbsh(short mes,short god,const char *shet,const char *shetkor,const char *repl,GtkWidget *wpredok);
int iceb_pbsh(const char *data,const char *shet,const char *shetkor,const char *repl,GtkWidget *wpredok);
int iceb_delfil(const char *katalog,const char *rashir,GtkWidget *wpredok);
int iceb_fioruk(int metka,class iceb_fioruk_rk *rek,GtkWidget *wpredok);
void iceb_mpods(int metka_ps,char *kto);
int iceb_mpods(char *kto);
int iceb_pvkdd(int metka_ps,short dd,short md,short gd,short mk,short gk,int pod,const char *nomdok,int tipz,GtkWidget *wpredok);
void iceb_uionp(GtkWidget *wpredok);
int iceb_pvglkni(short mes,short god,GtkWidget *wpredok);
int iceb_pvglkni(const char *mesgod,GtkWidget *wpredok);
int iceb_make_kat(class iceb_u_str *putnakat,const char *podkat,GtkWidget *wpredok);
void iceb_fsystem(const char *imaf_nast,const char *zamena,GtkWidget *wpredok);
void iceb_avp(class iceb_avp *avp,const char *imaf_nast,class iceb_u_spisok *sp_prov,class iceb_u_double *sum_prov_dk,FILE *ff_prot,GtkWidget *wpredok);
int  iceb_poi_kontr(class iceb_u_str *kontr,class iceb_u_str *naim_kontr,GtkWidget *wpredok);
int iceb_ustpeh(const char *imaf,int orient,GtkWidget *wpredok);
int iceb_ustpeh(const char *imaf,int orient,int *kolstr,GtkWidget *wpredok);
int iceb_sortkk(int metkasort,class iceb_u_spisok *skontr,class iceb_u_spisok *skontr_sort,GtkWidget *wpredok);
int iceb_cp(const char *imaf_out,const char *imaf_in,int metka,GtkWidget *wpredok);
int iceb_cat(const char *imaf1,const char *imaf2,GtkWidget *wpredok);

int iceb_poldan(const char *strpoi,char *find_dat,const char *imaf,GtkWidget *wpredok);
int iceb_poldan(const char *strpoi,class iceb_u_str *find_dat,const char *imaf,GtkWidget *wpredok);
int iceb_poldan(const char *strpoi,class iceb_u_str *find_dat,const char *imaf,int metka_ps,GtkWidget *wpredok);
int iceb_poldan(const char *strpoi,int *kk,const char *imaf,GtkWidget *wpredok);
int iceb_poldan(const char *strpoi,short *kk,const char *imaf,GtkWidget *wpredok);
int iceb_poldan(const char *strpoi,double *kk,const char *imaf,GtkWidget *wpredok);

int iceb_poldan_vkl(const char *strpoi,const char *imaf,GtkWidget *wpredok);
int iceb_perecod(int metka_kod,const char *imaf,GtkWidget*);
int iceb_pnnnp(const char *datann,const char *datavnn,const char *kodkontr,const char *nomnn,const char *datadok,const char *nomdok,int metka_ps,GtkWidget *wpredok);
int iceb_vperecod(const char *imaf,GtkWidget *wpredok);
int iceb_dir_select(class iceb_u_str *imadir,const char *titl,GtkWidget *wpredok);
int iceb_menu_start(int nomstr_menu,const char *version,const char *dataver,const char *naim_pods,const char *fioop,const char *naim_kontr00,int nom_op,class iceb_u_spisok *menustr,const char *cvet_fona);
void iceb_about();
int   iceb_l_smenabaz(class iceb_u_str *imabazv,int metka,GtkWidget *wpredok);
void iceb_font_selection(GtkWidget *wpredok);
void iceb_l_blok(GtkWidget *wpredok);
const char *iceb_getnps();
int iceb_getuid(GtkWidget *wpredok);
int iceb_prr(GtkWidget *wpredok);
int iceb_prr(int podsystem,GtkWidget *wpredok);
int iceb_l_blok_prov(GtkWidget *wpredok);
int iceb_pblok(short mes,short god,int kodps,GtkWidget *wpredok);
const char *iceb_getfioop(GtkWidget *wpredok);
void iceb_alxin(const char *imaf,GtkWidget *wpredok);
int iceb_alxout(const char *imaf,GtkWidget *wpredok);
int iceb_alxout(const char *imaf,const char *imafin,GtkWidget *wpredok);
int iceb_prn(int podsystem,GtkWidget *wpredok);
int iceb_prn(GtkWidget *wpredok);

const char *iceb_kszap(int kodop,GtkWidget *wpredok);
const char *iceb_kszap(const char *kodop,GtkWidget *wpredok);

int iceb_print_operation(GtkPrintOperationAction operation_action,const char *filname,GtkWidget *wpredok);
int iceb_insfil(const char *imaf,FILE *ff,GtkWidget *wpredok);
double iceb_pnds(short d,short m,short g,GtkWidget *wpredok);
double iceb_pnds(const char *data,GtkWidget *predok);
double iceb_pnds(GtkWidget *wpredok);
int iceb_alxzag(const char *put_alx,int metka,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok) ;
const char *iceb_kikz(char *kto,char *vremzap,GtkWidget *wpredok);
const char *iceb_kikz(int kto,time_t vremzap,GtkWidget *wpredok);
void iceb_fplus(const char *stroka,class iceb_u_str *stroka1,class SQLCURSOR *cur);
void iceb_fplus(int metka_sp,const char *stroka,class iceb_u_spisok *nastr,class SQLCURSOR *cur);
void iceb_sozmas(short **kod,char *st1,class SQLCURSOR *cur);
int iceb_vixod(short d,short m,short g,GtkWidget *wpredok);
void iceb_vkk00(GtkWidget *wpredok);
void iceb_vk00(class iceb_u_str *k00,GtkWidget *wpredok);
void       kontr_korkod(const char *kods,const char *kodn,GtkWidget *wpredok);
int        kontr_prov_row(SQL_str row,class kontr_data *data);
int iceb_getuslp(const char *kodkon,class iceb_u_str *uslprod,GtkWidget *wpredok);
int iceb_getuslp(int un_nom_zap,class iceb_u_str *uslprod,GtkWidget *wpredok);
int iceb_getuslp(const char *kodkon,class iceb_u_str *uslprod,int metka,GtkWidget *wpredok);
int iceb_getuslp(int un_nom_zap,class iceb_u_str *uslprod,int metka,GtkWidget *wpredok);
int iceb_getuslp_m(const char *kodkon,class iceb_u_str *usl_prod,GtkWidget *wpredok);
int   iceb_l_kontdog(int metka_rr,const char *kodkon,GtkWidget *wpredok);
void iceb_rnfxml(class iceb_rnfxml_data *rek_zag_nn,GtkWidget *wpredok);

int iceb_openxml(short dn,short mn,short gn,short dk,short mk,short gk,char *imaf_xml,const char *kod_dok,int nomer_dok_v_pakete,int *period_type,class iceb_rnfxml_data *rek_zag_nn,FILE **ff_xml,GtkWidget *wpredok);
int iceb_openxml(const char *datan,const char *datak,char *imaf_xml,const char *kod_dok,int nomer_dok_v_pakete,int *period_type,class iceb_rnfxml_data *rek_zag_nn,FILE **ff_xml,GtkWidget *wpredok);

int iceb_openxml(short dn,short mn,short gn,short dk,short mk,short gk,char *imaf_xml,const char *tip_dok,const char *pod_tip_dok,const char *nomer_versii_dok,int nomer_dok_v_pakete,int *period_type,class iceb_rnfxml_data *rek_zag_nn,FILE **ff_xml,GtkWidget *wpredok);
int iceb_openxml(const char *datan,const char *datak,char *imaf_xml,const char *tip_dok,const char *pod_tip_dok,const char *nomer_versii_dok,int nomer_dok_v_pakete,int *period_type,class iceb_rnfxml_data *rek_zag_nn,FILE **ff_xml,GtkWidget *wpredok);

int iceb_menu_vibkat(class iceb_u_spisok *zapros,class iceb_u_str *stroka,GtkWidget *wpredok);
int iceb_menu_vibkat(const char *zapros,class iceb_u_str *stroka,GtkWidget *wpredok);
const char *iceb_get_pnk(const char *kodkontr,int metka,GtkWidget *wpredok);
const char *iceb_get_namesystem();
const char *iceb_get_namebase();
void iceb_locale();

int iceb_nalnaks_v(class iceb_u_str *vid_dog,class iceb_u_str *dat_dog,class iceb_u_str *nom_dog,class iceb_u_str *vid_pokup,class iceb_u_str *vkl_erpn,class iceb_u_str *zal_prod,class iceb_u_str *tip_prih,class iceb_u_str *kopiq,GtkWidget *wpredok);

int iceb_nn7_start(const char *imaf,short dd,short md,short gd,short dnn,short mnn,short gnn,const char *nomnn,const char *osnov,const char *uspr,const char *frop,const char *kpos,int tipz,int metka_kop,const char *kodkontr00,const char *kod_vid_deqt,FILE **ff,GtkWidget *wpredok);
void iceb_nn7_end(int tipz,short tipnn,double itogo,double sumkor,const char *naimnal,float procent,float pnds,const char *imaf_nastr,const char *kodop,double okrg1,double suma_voz_tar,const char *kod_kontr,FILE *ff,GtkWidget *wpredok);
void iceb_nn7_str(int metka_str,int tipnn,const char *razdel,const char *datop,const char *naim,const char *ei,double kolih,double cena,double suma,const char *kodtov,FILE *ff);
void iceb_nn7_hap(int nomstr,int *kolstr,FILE *ff,GtkWidget *wpredok);


const char *iceb_getk00(int metka,const char *kodop,GtkWidget *wpredok);
void iceb_zagolov(const char *naim,FILE *ff,GtkWidget *wpredok);
void iceb_zagolov(const char *naim,short dn,short mn,short gn,short dk,short mk,short gk,FILE *ff,GtkWidget *wpredok);
void iceb_zagolov(const char *naim,const char *datan,const char *datak,FILE *ff,GtkWidget *wpredok);

void iceb_rnn_sap_xml(short mn,short gn,short mk,const char *kod_dok,int pnd,int period_type,class iceb_rnfxml_data *rek_zag_nn,FILE *ff_xml);
void iceb_rnn_sap_xml(const char *datan,const char *datak,const char *kod_dok,int pnd,int period_type,class iceb_rnfxml_data *rek_zag_nn,FILE *ff_xml);

void iceb_rnn_sap_xml(short mn,short gn,short mk,const char *tip_dok,const char *podtip,int type_ver,int pnd,const char *imaf,int period_type,class iceb_rnfxml_data *rek_zag_nn,FILE *ff_xml);
void iceb_rnn_sap_xml(const char *datan,const char *datak,const char *tip_dok,const char *podtip,int type_ver,int pnd,const char *imaf,int period_type,class iceb_rnfxml_data *rek_zag_nn,FILE *ff_xml);
int iceb_dumpbazout(const char *namebases,const char *login,const char *parol,const char *hostname,GtkWidget *wpredok);
int iceb_menu_selectfil(class iceb_u_str *imafz,const char *zagolov,GtkWidget *wpredok);
int iceb_system(const char *komanda,GtkTextBuffer *buffer,GtkWidget *view);
void iceb_blokps();
void iceb_optimbazw(const char *imabaz,GtkWidget *wpredok);
int iceb_getnkontr(int nom_start,GtkWidget *wpredok);
void iceb_zageks(const char *naim,FILE *ff,GtkWidget *wpredok);
void iceb_lnom();
int iceb_prov_vdb(const char *namebases,GtkWidget *wpredok);

int iceb_rnl(const char *imaf,int orient,const char *obr,void (*hapka)(int nom,int *kostr,FILE *FF,GtkWidget *wpredok),GtkWidget *wpredok);
int iceb_rnl(const char *imaf,class iceb_rnl_c *rh,void (*hapka1)(class iceb_rnl_c *rh,int *nom_str,FILE *ff),GtkWidget *wpredok) ;

const char *iceb_getkoddok(short d,short m,short g,int metka);
int iceb_open_fil_nn_xml(char *imaf_xml,int pnd,int tipz,short mn,short gn,class iceb_rnfxml_data *rek_zag_nn,FILE **ff_xml,GtkWidget *wpredok);
int iceb_sizw(const char *name_ok,gint gor,gint vert,GtkWidget *wpredok);
int iceb_sizww(const char *name_ok,GtkWidget *window);
int iceb_sizwr(const char *name_ok,int *gor,int *ver);
int iceb_imp_xml_nn(const char *imaf_xml,class iceb_imp_xml_nn_r *rk,GtkWidget *view,GtkTextBuffer *buffer,GtkWidget *wpredok);
void iceb_label_set_text_color(GtkWidget *label,const char *text,const char *cvet);
void iceb_label_set_style_text(GtkWidget *label,const char *style_text,const char *text);
void iceb_beep();
gboolean iceb_activate_link (GtkWidget   *label,const gchar *uri,gpointer     data);
int iceb_vstavfil(class iceb_vstavfil_rk *rek,const char *naimfil,FILE *ff,GtkWidget *wpredok);

int iceb_get_kkfname(const char *naim,const char *kodkon,class iceb_u_str *kkontr,int *nom_kod_kontr,GtkWidget *wpredok);
int iceb_pvkup(int kodop_v_prov,GtkWidget *wpredok);
int iceb_zkvsk(const char *strok,int *nom_kod_kontr,FILE *ff_prot,GtkWidget *wpredok);

int iceb_get_rtn(int tabnom,class iceb_fioruk_rk *rek,GtkWidget *wpredok);

void iceb_nn9_end(int tipz, short lnds,const char *imaf_nastr,const char *kodop,const char *kod_kontr,FILE *ff,GtkWidget *wpredok);
int iceb_nn9_start(const char *imaf,short dd,short md,short gd,short dnn,short mnn,short gnn,const char *nomnn,const char *kpos,int tipz,const char *kodkontr00,FILE **ff,GtkWidget *wpredok);
void iceb_nn9_start_s(const char *imaf,int lnds,double suma_dok,double suma_nds,double suma_vt,GtkWidget *wpredok);
void iceb_nn9_str(int nomer_str,int metka_str,const char *naim,const char *ei,double kolih,double cena,double suma,const char *kodtov,int metka_imp,const char *ku,const char *kdstv,FILE *ff,GtkWidget *wpredok);
void iceb_nn9_hap(int nomstr,int *kolstr,FILE *ff,GtkWidget *wpredok);

void iceb_nn10_zag_xml(int tipz,const char *nomdok,short dnp,short mnp,short gnp,short dd,short md,short gd,const char *kontr00,const char *kontr,const char *vid_dok,FILE *ff_xml,GtkWidget *wpredok);
void iceb_nn10_str_xml(int nomer,double kolih,double cena,double suma,const char *ei,const char *naim_mat,const char *kodtov,const char *kod_stavki,const char *kod_lgoti,int metka_imp_tov,const char *ku,const char *kdstv,double spnv,FILE *ff_xml,GtkWidget *wpredok);
void iceb_nn10_end_xml(const char *kodop,const char *imaf_nastr,int lnds,double sumdok,double suma_nds,double suma_vt,FILE *ff_xml,GtkWidget *wpredok);

int iceb_kor_kod_kontr(const char *old_kod,const char *new_kod,GtkWidget *wpredok);
const char *iceb_get_kodstavki(int lnds);
const char *iceb_get_fiotbn(int tabnom,GtkWidget *wpredok);
int iceb_vrvr(const char *imaf,class iceb_vrvr *rv,FILE *ff,GtkWidget *wpredok);
const char *iceb_get_edrpou(const char *kod_kontr,GtkWidget *wpredok);

