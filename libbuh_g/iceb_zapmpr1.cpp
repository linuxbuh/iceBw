/*$Id: iceb_zapmpr1.c,v 1.13 2013/08/13 06:10:14 sasa Exp $*/
/*28.03.2013	32.09.1998	Белых А.И.	iceb_zapmpr1.c
Запись проводок из пямяти в базу
*/
#include        <math.h>
#include        "iceb_libbuh.h"


void iceb_zapmpr1(const char *nn, //Номер документа
const char *kop, //Код операции
int skk,    //Код склада/подразделения
time_t vrem, //Время записи
const char *kto, //Метка подсистемы
short dd,short md,short gd, //Дата документа
int tipz, //1-приходный документа 2-расходный документ 0-не определено
class iceb_u_spisok *sp_prov, /*Список проводок*/
class iceb_u_double *sum_prov_dk, /*Суммы по дебету/кредиту проводов*/
FILE *ff_prot,
GtkWidget *wpredok)
{
short		i;
short		d,m,g;
class iceb_u_str datpr("");
class iceb_u_str sh(""),shk("");
class iceb_u_str kor1(""),kor2("");
class iceb_u_str kom("");
double		deb,kre;
double		bb;
int		kolpr=0;
int		metkaprov=0;
int kekv=0; //код классификации экономических затрат
/*
printw("\nzapmpr1-kp=%d nn=%s kto=%s kop=%s %d.%d.%d mprov-%s\n",
kp,nn,kto,kop,dd,md,gd,mprov);
*/
int kp=sp_prov->kolih();
if(kp == 0)
 return;
 
for(i=0 ; i < kp; i++)
 {

  iceb_u_polen(sp_prov->ravno(i),&datpr,1,'|');
  iceb_u_rsdat(&d,&m,&g,datpr.ravno(),0);

  iceb_u_polen(sp_prov->ravno(i),&sh,2,'|');

  iceb_u_polen(sp_prov->ravno(i),&shk,3,'|');

  iceb_u_polen(sp_prov->ravno(i),&kor1,4,'|');

  iceb_u_polen(sp_prov->ravno(i),&kor2,5,'|');

  iceb_u_polen(sp_prov->ravno(i),&kom,6,'|');
  iceb_u_polen(sp_prov->ravno(i),&kolpr,7,'|');

  iceb_u_polen(sp_prov->ravno(i),&metkaprov,8,'|');

  iceb_u_polen(sp_prov->ravno(i),&kekv,10,'|');
    
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s:%d.%d.%d %-4s %-4s %-4s %-4s %8.2f %8.2f %s\n",
   __FUNCTION__,d,m,g,sh.ravno(),shk.ravno(),kor1.ravno(),kor2.ravno(),sum_prov_dk->ravno(i*2),sum_prov_dk->ravno(i*2+1),kom.ravno());

//  sqlfiltr(kom,sizeof(kom));
  bb=iceb_sumvpr(sh.ravno(),shk.ravno(),dd,md,gd,nn,skk,kom.ravno_filtr(),vrem,metkaprov,kto);
 
  deb=sum_prov_dk->ravno(i*2);
  kre=sum_prov_dk->ravno(i*2+1);
  if(fabs(deb) > 0.009)
   deb-=bb;
  if(fabs(kre) > 0.009)
   kre-=bb;

  deb=iceb_u_okrug(deb,0.01);
  kre=iceb_u_okrug(kre,0.01);
         
  if(fabs(kre) > 0.009 || fabs(deb) > 0.009 )
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"%s:deb=%f kre=%f\n",__FUNCTION__,deb,kre);
    iceb_zapprov(metkaprov,g,m,d,sh.ravno(),shk.ravno(),kor1.ravno(),kor2.ravno(),kto,nn,kop,deb,kre,kom.ravno_filtr(),kolpr,skk,vrem,gd,md,dd,tipz,kekv,wpredok);
   }
 }

}
