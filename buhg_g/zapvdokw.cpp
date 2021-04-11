/* $Id: zapvdokw.c,v 1.10 2013/08/13 06:09:59 sasa Exp $ */
/*22.03.2017    26.09.1997      Белых А.И.      zapvdokw.c
Запись в документ
*/
#include	"buhg_g.h"

//extern SQL_baza  bd;

int  zapvdokw(short dd,short md,short gd,int skll,int nkk,
int km,const char *nnn,double kol,double cen,
const char *ei,double nds,int mnds,int voztar,int tipz,
int nomkarp,//номер карточки парного документа
const char *shet, //Счет списания для расходных документов
const char *nomz, //Номер заказа
int metka_imp_tov, /*метка импортного товара 0-не импорт 1-импорт*/
const char *kdstv, /*код діяльності сільськогосподарського товаровиробника*/
GtkWidget *wpredok)
{
extern double	okrcn;  /*Округление цены*/
char		strsql[1024];

cen=iceb_u_okrug(cen,okrcn);
nds=iceb_u_okrug(nds,0.01);


sprintf(strsql,"insert into Dokummat1 \
values ('%04d-%02d-%02d',%d,'%s',%d,%d,%.10g,%.10g,'%s',%d,%.10g,%d,%d,%ld,\
%d,%d,'%s','%s','%s',%d,'%s')",
gd,md,dd,skll,nnn,nkk,km,kol,cen,ei,voztar,nds,mnds,iceb_getuid(wpredok),time(NULL),
tipz,nomkarp,shet,"",nomz,metka_imp_tov,kdstv);

if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
  return(1);

return(0);
}
