/*$Id: iceb_util.h,v 1.121 2014/06/30 06:35:58 sasa Exp $*/
/*22.01.2017	03.09.2003	Белых А.И.	iceb_util.h
*/
#include <libintl.h>
#include <stdio.h>
#include <nl_types.h>
#include <time.h>

#include "iceb_u_utf.h"
#include "iceb_u_int.h"
#include "iceb_u_double.h"
#include "iceb_u_spisok.h"
#include "iceb_u_str.h"
#include "iceb_u_file.h"

const char *iceb_u_double_to_char_zp(double suma,int drob);
int      iceb_u_pole3(const char *D,char *K,int N,char R);
int iceb_u_pole3(const char *D,class iceb_u_str *K,int N,char R);
int      iceb_u_pole2(const char *str,char raz);
short    iceb_u_pole1(const char*,const char*,char,short,int*);
int      iceb_u_pole(const char*,char*,int,char);
int      iceb_u_pole(const char*,class iceb_u_str*,int,char);
int      iceb_u_polen(const char*,char*,int,int,char);
int      iceb_u_polen(const char*,double*,int,char);
int      iceb_u_polen(const char*,float*,int,char);
int      iceb_u_polen(const char*,int*,int,char);
int      iceb_u_polen(const char*,long*,int,char);
int      iceb_u_polen(const char*,short*,int,char);
int      iceb_u_polen(const char*,class iceb_u_str*,int,char);
int      iceb_u_polen(const char*,class iceb_u_spisok*,int,char);
int 	iceb_u_SRAV(const char*,const char*,int);
int      iceb_u_proverka(const char*,const char*,int,int);
int      iceb_u_proverka(const char*,int,int,int);
int      iceb_u_strstrm(const char*,const char*);
const char *iceb_u_vremzap(char*);
const char *iceb_u_vremzap(time_t);
const char *iceb_u_kszap(char *kto,int metka_kod);
const char *iceb_u_kszap(int,int metka_kod);
void	iceb_u_zagolov(const char*,short,short,short,short,short,short,const char*,FILE*);
void	iceb_u_zagolov(const char *naim,const char *datan,const char *datak,const char *organ,FILE *ff);
void     iceb_u_startfil(FILE*);
int      iceb_u_poldan(const char*,char*,const char*);
int      iceb_u_poldan(const char*,class iceb_u_str*,const char*);
int      iceb_u_ps_minus(char*);
int      iceb_u_rsdat(short*,short*,short*,const char*,int);
int iceb_u_sravmydat(short,short,short,short,short,short);
int iceb_u_sravmydat(const char *data1,const char *data2);
int iceb_u_sravmydat(const char *data1,const char *data2,int metka);
int iceb_u_sravmydat(short d,short m,short g,const char *data2,int metka);
int      iceb_u_rstime(short *,short *,short *,const char *);
const char *iceb_u_datzap(const char*);
const char *iceb_u_datzap_mg(const char*);
double   iceb_u_atof(const char*);
void     iceb_u_dpm(short *d,short *m,short *g,short mt);
char     *iceb_u_prnbr(double);
double   iceb_u_okrug(double sh,double mt);
int iceb_u_print(const char *imaf,int kolkop,const char *progp,const char *name_printer,const char *kluh_peh,const char *kluh_name_pr,const char *kluh_kol_kop);
int      iceb_u_pstrz(char *stroka,int razmer,const char *stroka_plus);
int      iceb_u_period(short,short,short,short,short,short,int);
void     iceb_u_preobr(double,char *,short);
void     iceb_u_preobr(double,class iceb_u_str*,short);
int      iceb_u_rsdat1(short *m,short *g,char *str);
int      iceb_u_rsdat1(short *m,short *g,const char *str);
char     *iceb_u_kikz(int kto,time_t vremzap,nl_catd fils,int metka_kod);
time_t   iceb_u_vrem(iceb_u_str *data,iceb_u_str *vrem,int metka);
void     iceb_u_poltekdat(short*,short*,short*);
const char *iceb_u_poltekdat();
int      iceb_u_fgets_bps(char *stroka,int razmer,FILE *ff);
int iceb_u_fgets(class iceb_u_str *str,FILE *ff);
int 	iceb_u_vstavfil(const char *imafil,FILE *ff);
char     *iceb_u_vrem_tek(void);
time_t iceb_u_dat_to_sec(const char *data);
time_t iceb_u_dat_to_sec(short);
time_t iceb_u_dat_to_sec(short,short,short);
time_t iceb_u_dat_to_sec_end(const char *data);
time_t iceb_u_dat_to_sec_end(short);
time_t iceb_u_dat_to_sec_end(short,short,short);
void iceb_u_ud_lp(char *stroka);
int iceb_u_sravtime(short has1,short min1,short sek1,short has2,short min2,short sek2);
void    iceb_u_prnb(double nb,char *str);
void    iceb_u_prnb(double nb,class iceb_u_str *str);
void iceb_u_ud_simv(char *stroka,const char *simvol);
void iceb_u_prnb1(double nb,char *nb1);
void iceb_u_ekrs(char *stroka,int razmer,char ekran,const char *simv);
void iceb_u_sozmas(short **kod,char *st1);
void iceb_u_sozmas(short **kod,char *st1,FILE*);
void iceb_u_denrog(short *d,short *m,short *g,int kold);
time_t iceb_u_datetime_sec(const char *datetime);
time_t iceb_u_datetime_sec(short d,short m,short g,short has,short min,short sek);
const char *iceb_u_datetime_pr(char *datatime);
void iceb_u_tolower_str(char *stroka);
void iceb_u_toupper_str(char *stroka);
int iceb_u_srav_r(const char *stroka1,const char *stroka2,int metka);
void iceb_u_fplus(const char *stroka,class iceb_u_str *stroka1,FILE *ff);
void iceb_u_fplus(int metka_sp,const char *stroka,class iceb_u_spisok *nastr,FILE *ff);
void iceb_u_fio(const char *fio,class iceb_tu_str *fio_in);
int iceb_u_kol_str(int max_kol_std_na_liste,int ras_kol_str);
int iceb_u_kol_str_l(int max_kol_std_na_liste,int ras_kol_str);
int iceb_u_zvstr(class iceb_u_str *stroka,const char *obr,const char *zamena);
void iceb_u_cpbstr(char *strin,int dlstrin,const char *strout,const char *udstr);
void iceb_u_cpbstr(class iceb_u_str *strin,const char *strout,const char *udstr);
void iceb_u_cpbstr(class iceb_u_spisok *strin,const char *strout,const char *udstr);
int iceb_u_poltekgod();
int iceb_u_strlen(const char *stroka);
const char *iceb_u_adrsimv(int nomer_simv,const char *str);
int iceb_u_kolbait(int kolih_simv,const char *str);
int iceb_u_strncpy(char *tocop,const char *fromcop,int kolsimv);
const char *iceb_u_getlogin();
const char *iceb_u_sqlfiltr(const char *stroka);
const char *iceb_u_filtr_xml(const char *stroka);
const char *iceb_u_filtr_from_xml(const char *stroka);
const char *iceb_u_propis(double hs,int metka_1s);
const char *iceb_u_nofwords(double number);
void iceb_u_vstav(class iceb_u_str *stroka,const char *strin,int poz1,int poz2,int hag);
void iceb_u_vstav(class iceb_u_str *stroka,int strin,int poz1,int poz2,int hag);
void iceb_u_vstav(char *stroka,const char *strin,int poz1,int poz2,int hag,int kolbait_stroka);
void iceb_u_vstav(char *stroka,int strin,int poz1,int poz2,int hag,int kolbait_stroka);
void iceb_u_zvstrp(char *stroka,int poz_zam,const char *vstr,int dlinna_str);
void iceb_u_zsimv(char *stroka,int kolbait,int poz,const char *simvol);
int iceb_u_kolsimv(int kolbait_str,const char *stroka);
const char *iceb_u_tosqldat(const char *data);
void iceb_u_strspisok(const char *stroka,int razdelitel,class iceb_u_spisok *spisok);
const char *iceb_u_getcurrentdir();
const char *iceb_u_inttochar(int znah);
const char *iceb_u_tipz(const char *pm);
char *iceb_u_workid();

