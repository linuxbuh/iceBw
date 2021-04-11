/*$Id: cennikdw.c,v 1.16 2013/09/26 09:46:46 sasa Exp $*/
/*10.07.2015	07.05.2003	Белых А.И.	cennikdw.c
Распечатка ценников для документа
*/
#include        <errno.h>
#include	"buhg_g.h"

void	cennikw(short d,short m,short g,char *kodtv,char *naim,double cena,char *shrihkod,const char *organ,FILE *ff);

extern double	okrg1;  /*Округление суммы*/
extern SQL_baza  bd;

void   cennikdw(short dd,short md,short gd, //дата документа
int skl,  //Склад
const char *nomdok, //Номер документа
GtkWidget *wpredok)
{
char 	strsql[512];
SQL_str	row,row1;
int 	kolstr=0;


sprintf(strsql,"select distinct kodm from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s'",
gd,md,dd,skl,nomdok);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
  return;

FILE *ff;
char imaf[64];

sprintf(imaf,"cen%d.lst",getpid());

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

class iceb_u_str naim("");
double cena=0.;
class iceb_u_str kodtv("");
class iceb_u_str strihkod("");
time_t          tmm;
struct  tm      *bf;
time(&tmm);
bf=localtime(&tmm);


while(cur.read_cursor(&row) != 0)
 {
  cena=0.;
  naim.new_plus("");
  strihkod.new_plus("");
  kodtv.new_plus("");

  //Читаем цену продажи и наименование товара
  sprintf(strsql,"select naimat,strihkod,cenapr from Material where kodm=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    naim.new_plus(row1[0]);
    strihkod.new_plus(row1[1]);

    cena=atof(row1[2]);
    cena=iceb_u_okrug(cena,okrg1);
   }  
  kodtv.new_plus(row[0]);
  cennikw(bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900,kodtv.ravno(),naim.ravno(),cena,strihkod.ravno(),iceb_get_pnk("00",0,wpredok),ff);
 }

fclose(ff);

cenniksw(imaf,wpredok); //Преобразование в колонки

iceb_u_spisok imafils;
iceb_u_spisok naimf;

imafils.plus(imaf);
naimf.plus(gettext("Распечатка ценников для товаров"));

iceb_rabfil(&imafils,&naimf,wpredok);


}
