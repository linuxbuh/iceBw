/*$Id:$*/
/*22.03.2017	16.03.2016	Белых А.И.	iceb_nn9_start_s.c
Вставка сумм по документу в шапку документа
*/
#include "iceb_libbuh.h"

void iceb_nn9_start_s(const char *imaf,int lnds,double suma_dok,double suma_nds,double suma_vt,GtkWidget *wpredok)
{
char imaf_tmp[64];
char strsql[1024];
class iceb_u_str stroka("");
double suma[10];
memset(suma,'\0',sizeof(suma));

suma[0]=suma_dok;
suma[1]=suma_nds;
 
if(lnds == 0) /*20 процентов*/
 {
  suma[2]=suma_nds;
  suma[4]=suma_dok-suma_nds;
 }

if(lnds == 1) /*0 процентов на територии Украины*/
 {
  suma[7]=suma_dok;

 }

if(lnds == 2) /*0 процентов экспорт*/
 {
  suma[6]=suma_dok;
 }

if(lnds == 3) /*освобождено от НДС*/
 {
  suma[8]=suma_dok;
 }

if(lnds == 4)/*7 процентов*/
 {
  suma[3]=suma_nds;
  suma[5]=suma_dok;
 } 

/*возвратная тара*/
suma[9]=suma_vt;

class iceb_fopen fil_ff;
if(fil_ff.start(imaf,"r",wpredok) != 0)
 return;

sprintf(imaf_tmp,"nn9%d.tmp",getpid());
class iceb_fopen fil_tmp;

if(fil_tmp.start(imaf_tmp,"w",wpredok) != 0)
 return;
int nom_str=0;
int nom_sum=0;

while(iceb_u_fgets(&stroka,fil_ff.ff) == 0)
 {
//  printw("%s",stroka.ravno());
    
  switch(++nom_str)
   {
    case 28:
    case 30:
    case 32:
    case 34:
    case 36:
    case 38:
    case 40:
    case 42:
    case 44:
    case 46:
     if(suma[nom_sum] != 0.)
      {
       sprintf(strsql,"%17.2f",suma[nom_sum]);
       iceb_u_vstav(&stroka,strsql,163,181,1);
      }
     nom_sum++;
     break;
   }
  fprintf(fil_tmp.ff,"%s",stroka.ravno());
 }

fil_ff.end();
fil_tmp.end();
unlink(imaf);
rename(imaf_tmp,imaf);
}