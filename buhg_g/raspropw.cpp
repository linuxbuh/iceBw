/* $Id: raspropw.c,v 1.19 2013/08/13 06:09:50 sasa Exp $ */
/*24.04.2018	13.06.2000	Белых А.И.	raspropw.c
Распечатка пропуска для вывоза полученного товара-материала
*/
#include        <errno.h>
#include	"buhg_g.h"

extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/
extern short    srtnk; /*0-не включена 1-включена сортировка записей в накладной*/

extern SQL_baza  bd;

void raspropw(short dd,short md,short gd,int skl,const char *nomdok,GtkWidget *wpredok)
{
SQL_str         row,rowtmp;
int		kolstr;
char		strsql[1024];
FILE            *f1;
class iceb_u_str kontr("");
int		tipz;
char		datop[32];
short		metz;
short		d,m,g;
class iceb_u_str bros("");
class iceb_u_str naiskl("");
char		imaf[64];
double		kratn;
class iceb_u_str ein("");
double		kolih,kolihi;
int		nomerz;
double		mest,mesti;
class iceb_u_str naim("");
time_t vrem_zap=0;
SQLCURSOR cur,curtmp;

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
gd,md,dd,skl,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"rasdpropw-%s",gettext("Не найден документ !")),
  repl.plus(strsql);
  repl.ps_plus(strsql);
  
  sprintf(strsql,"%s %d.%d.%d %s %s %s %d",
  gettext("Дата"),dd,md,gd,
  gettext("Документ"),nomdok,
  gettext("Слад"),skl);
  
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  
  return;
 }

kontr.new_plus(row[3]);
tipz=atoi(row[0]);
vrem_zap=atol(row[10]);

sprintf(strsql,"select nomerz,sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d",
gd,nomdok,skl);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

memset(datop,'\0',sizeof(datop));


if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  metz=atoi(row[0]);
  if(metz == 9)
   {
    iceb_u_rsdat(&d,&m,&g,row[1],0);
    if(d != 0 && iceb_u_sravmydat(d,m,g,dd,md,gd) < 0)
     sprintf(datop,"%02d.%02d.%d",d,m,g);
   }
 }

class iceb_tmptab tabtmp;
const char *imatmptab={__FUNCTION__};

if(sortdokmuw(dd,md,gd,skl,nomdok,imatmptab,&tabtmp,NULL,wpredok) != 0)
 return;


sprintf(strsql,"select * from %s",imatmptab);

if(srtnk == 1)
  sprintf(strsql,"select * from %s order by kgrm asc,naim asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }


memset(imaf,'\0',sizeof(imaf));
sprintf(imaf,"prp%ld.lst",vrem_zap);

if((f1 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
int kolstrok=0;
rashapw(tipz,&kolstrok,"00",kontr.ravno(),0,0,f1,wpredok);


/*Читаем наименование склада*/
sprintf(strsql,"select naik from Sklad where kod=%d",skl);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl; 
  repl.plus(gettext("Не найден склад"));
  repl.plus(" ");
  repl.plus(skl);
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);
 }
else
  naiskl.new_plus(row[0]);

fprintf(f1,"\n%s\n\n",naiskl.ravno());

fprintf(f1,"         %s N%s\n",
gettext("ПРОПУСК к расходной накладной"),nomdok);

memset(strsql,'\0',sizeof(strsql));
iceb_mesc(md,1,strsql);
fprintf(f1,"\
                      %d %s %d%s\n",
dd,strsql,gd,gettext("г."));

/*Читаем доверенность*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gd,nomdok,skl,1);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  fprintf(f1,gettext("Доверенность"));
  fprintf(f1," N");
  iceb_u_polen(row[0],&bros,1,'#');
  fprintf(f1," %s",bros.ravno());
  iceb_u_polen(row[0],&bros,2,'#');
  fprintf(f1," %s %s",gettext("от"),bros.ravno());
 }
fprintf(f1,"\n");


/*Читаем через кого*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gd,nomdok,skl,2);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  fprintf(f1,"Через кого: %s\n",row[0]);

fprintf(f1,"\x0F"); /*Включени ужатого режима*/

fprintf(f1,"\
--------------------------------------------------------------------------------\n");
fprintf(f1,gettext(" N |       Наименование продукции                 |Ед. |Количество| Кол. |Крат-|\n"));
fprintf(f1,gettext("   |              (товару)                        |изм.|          | м-ст |ность|\n"));
fprintf(f1,"\
--------------------------------------------------------------------------------\n");



kolihi=mesti=0.;
nomerz=0;
while(curtmp.read_cursor(&rowtmp) != 0)
 {

  naim.new_plus(rowtmp[1]);
  kratn=atoi(rowtmp[5]);
  ein.new_plus(rowtmp[6]);
  kolih=atof(rowtmp[9]);


  kolihi+=kolih;
  if(kratn != 0.)
    mest=kolih/kratn;
  else
    mest=0.;



  mest=iceb_u_okrug(mest,0.1);
  mesti+=mest;
  fprintf(f1,"%3d %-*.*s %-*s %10.10g %6.6g %4.4g\n",
  ++nomerz,
  iceb_u_kolbait(46,naim.ravno()),iceb_u_kolbait(46,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(4,ein.ravno()),ein.ravno(),
  kolih,mest,kratn);

 }
fprintf(f1,"\
--------------------------------------------------------------------------------\n");
fprintf(f1,"%*s:%10.10g %6.6g\n",
iceb_u_kolbait(55,gettext("Итого")),gettext("Итого"),kolihi,mesti);


fprintf(f1,"\n%s%s\n",gettext("\
Руководитель__________________        Главный  бухгалтер_______________\n\n"),
gettext("\
Отпустил______________________        Принял__________________________"));

iceb_podpis(f1,wpredok);
fclose(f1);

iceb_u_spisok imafil;
iceb_u_spisok naimf;
imafil.plus(imaf);
naimf.plus(gettext("Пропуск для вывоза материалов со склада"));

iceb_ustpeh(imaf,1,wpredok);
iceb_rabfil(&imafil,&naimf,wpredok);

}
