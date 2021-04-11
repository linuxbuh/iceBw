/* $Id: rasshetzw.c,v 1.20 2014/08/31 06:19:22 sasa Exp $ */
/*03.04.2020	15.09.2000	Белых А.И.	rasshetzw.c
Расчёт начислений и удержаний заработной платы
*/
#include        <errno.h>
#include        "buhg_g.h"

void     zvaniew(int,short,short,int,const char *nah_only,GtkWidget*);
void	zarindexw(int,short,short,int,const char *nah_only,GtkWidget*);
void zarindexvw(int tn,short mp,short gp,int podr,const char *nah_only,GtkWidget *wpredok);
double   dolhnnw(int,short,short,int,const char *nah_only,GtkWidget*);
void     zarkmdw(int tn,short mp,short gp,int podr,const char *nah_only,int metka_r,FILE *ff,GtkWidget*);
double   podoh1w(int,short,short,int,const char *uder_only,GtkWidget*);
void     procentw(int,short,short,int,const char *nah_only,GtkWidget*);
void     dopdominw(int,short,short,int,double,const char *nah_only,GtkWidget*);
double   boln1w(int,short,short,int,const char *uder_only,GtkWidget*);
void     profsw(int,short,short,int,const char *uder_only,GtkWidget*);
void     kreditw(int,short,short,int,const char *uder_only,GtkWidget*);
void     alimw(int,short,short,int,const char *uder_only,GtkWidget*);
void     zaruozw(int,short,short,int,const char *uder_only,GtkWidget*);
void     zarvpnw(int,short,short,int,const char *uder_only,GtkWidget*);
void zardznw(int tn,short mp,short gp,int podr,const char *nah_only,GtkWidget *wpredok);
void zarvnw(int tabnom,short mes,short god,int podr,GtkWidget *wpredok);

extern short	kodindex;  /*Код индексации*/

void rasshetzw(int tabn,short mp,short gp,int kp,
int prn,  //1-только начисления 2-только удержания 3-все
const char *nah_only,
const char *uder_only,
GtkWidget *wpredok)
{
char strsql[1024];
double		koefpv;
/*
printw("rasshetz-%d %d.%d %d %d %d\n",tabn,mp,gp,sovm,kp,prn);
refresh();
*/

if(prn == 1 || prn == 3)
 {
  zar_pr_startw(1,tabn,mp,gp,wpredok);
  sprintf(strsql,"%s-Код начисления:%s\nКод удержания:%s\n",__FUNCTION__,nah_only,uder_only);
  zar_pr_insw(strsql,wpredok);




  koefpv=dolhnnw(tabn,mp,gp,kp,nah_only,wpredok); /*Должносной оклад*/

  zvaniew(tabn,mp,gp,kp,nah_only,wpredok); /*За звание*/

  procentw(tabn,mp,gp,kp,nah_only,wpredok); /*Процент*/

  zarindexw(tabn,mp,gp,kp,nah_only,wpredok); /*Расчёт индексации*/

  zarindexvw(tabn,mp,gp,kp,nah_only,wpredok); /*индексация на невовремя выплаченную зарплату*/

  dopdominw(tabn,mp,gp,kp,koefpv,nah_only,wpredok); /*Расчёт начисления до минимальной зарплаты*/
  
  zarkmdw(tabn,mp,gp,kp,nah_only,1,NULL,wpredok); //Расчёт командировочных
 
  zardznw(tabn,mp,gp,kp,nah_only,wpredok); /*Расчёт доплаты до средней зарплаты в командировке с учётом надбавки за награды*/
  zar_pr_endw(wpredok);    
 }


if(prn == 2 || prn == 3)
 {
  zar_pr_startw(2,tabn,mp,gp,wpredok);

  sprintf(strsql,"%s-Код начисления:%s\nКод удержания:%s\n",__FUNCTION__,nah_only,uder_only);
  zar_pr_insw(strsql,wpredok);



    podoh1w(tabn,mp,gp,kp,uder_only,wpredok); /*Расчёт подоходного налога,пенсионных,соцстраха*/

    zarvpnw(tabn,mp,gp,kp,uder_only,wpredok); /*возврат подоходного налога*/

    profsw(tabn,mp,gp,kp,uder_only,wpredok); /*Профсоюзный взнос*/

    kreditw(tabn,mp,gp,kp,uder_only,wpredok);  /*кредит */

    alimw(tabn,mp,gp,kp,uder_only,wpredok);    /*алименты*/

    zaruozw(tabn,mp,gp,kp,uder_only,wpredok); //удержание однодневного зароботка

    zarvnw(tabn,mp,gp,kp,wpredok); /*военный налог*/

    /* перерасчёт больничного делаем здесь так как будут перечитаны настройки по месяцам расчёта*/
    if(iceb_u_sravmydat(1,mp,gp,ICEB_DNDESV,ICEB_MNDESV,ICEB_GNDESV) >= 0)
      boln1w(tabn,mp,gp,kp,uder_only,wpredok); /*Перерасчёт подоходного из за больничных*/

  zar_pr_endw(wpredok);    
 }
}
