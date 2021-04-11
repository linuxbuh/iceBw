/*$Id:$*/
/*24.04.2018	10.03.2010	Белых А.И.	sortdokmuw.cpp
Создание временной таблицы для сортировки записей в документе
*/
#include "buhg_g.h"
extern SQL_baza bd;

int sortdokmuw(short dd,short mm,short gg,int skl,const char *nomdok,
const char *imatmptab,
class iceb_tmptab *tabtmp,
int *kol_znak_nomn,
GtkWidget *wpredok)
{
if(kol_znak_nomn != NULL)
*kol_znak_nomn=0;

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
kgrm int not null,\
naim char(112) not null,\
kodm int not null,\
nk int not null,\
cena double(17,7) not null,\
krat double(16,6) not null,\
ei char(24) not null,\
vtar int not null,\
shu char(24) not null,\
kolih double(16,6) not null,\
nomz char(24) not null,\
artikul char(24) not null,\
kt char(32) not null,\
mi TINYINT NOT NULL DEFAULT 0,\
kdstv VARCHAR(32) NOT NULL DEFAULT '') \
ENGINE = MYISAM",imatmptab);



if(tabtmp->create_tab(imatmptab,zaprostmp,wpredok) != 0)
  return(1);


double		krat;
class iceb_u_str naimat("");
int		kgrm;
class iceb_u_str ei("");
int		nk;
double		cena;
class iceb_u_str shu("");
short		vtar;
double		kolih;
SQL_str         row,row1;
char		strsql[512];
long		kolstr;
short		kodm;
class iceb_u_str nomz("");
class iceb_u_str artikul("");
class iceb_u_str kodtov("");
sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gg,mm,dd,skl,nomdok);

SQLCURSOR cur;
SQLCURSOR cur1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr <= 0)
  return(1);

int metka_imp_tov=0;
class iceb_u_str kdstv("");
while(cur.read_cursor(&row) != 0)
 {

  /*Читаем наименование материала*/
  kodtov.new_plus("");
  naimat.new_plus("");
  krat=0.;
  kgrm=0;
  sprintf(strsql,"select naimat,krat,kodgr,artikul,ku from Material where kodm=%s",row[4]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найден код материала"));
    repl.plus(" ");
    repl.plus(row[4]);
    repl.plus(" !");
    iceb_menu_soob(&repl,wpredok);
    artikul.new_plus("");
   }
  else
   {
    naimat.new_plus(row1[0]);
    if(row[16][0] != '\0')
     {
      naimat.plus(" ",row[16]);
     }
    artikul.new_plus(row1[3]);
    kodtov.new_plus(row[4]);
    krat=atof(row1[1]);
    kgrm=atoi(row1[2]);
   }

  kodm=atoi(row[4]);
  nk=atoi(row[3]);
  kolih=atof(row[5]);
  cena=atof(row[6]);
  ei.new_plus(row[7]);
  vtar=atoi(row[8]);
  nomz.new_plus(row[17]);
  metka_imp_tov=atoi(row[18]);
  kdstv.new_plus(row[19]);  
  
  shu.new_plus("**");  
  if(nk != 0)
   {
    sprintf(strsql,"select krat,shetu,innom,nomz from Kart where sklad=%d and nomk=%d",
    skl,nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     { 
      krat=atof(row1[0]);
      shu.new_plus(row1[1]);
      if(row1[2][0] != '\0')
       {
        naimat.plus(" N",row1[2]);
       }
      nomz.new_plus(row1[3]);
     }
   }


  sprintf(strsql,"insert into %s values (%d,'%s',%d,%d,%.10g,%.5g,'%s',%d,'%s',%.10g,'%s','%s','%s',%d,'%s')",
  imatmptab,
  kgrm,naimat.ravno_filtr(),kodm,nk,cena,krat,ei.ravno_filtr(),vtar,shu.ravno_filtr(),kolih,nomz.ravno_filtr(),artikul.ravno_filtr(),kodtov.ravno_filtr(),
  metka_imp_tov,kdstv.ravno_filtr());

  iceb_sql_zapis(strsql,1,0,wpredok);    

  if(kol_znak_nomn != NULL)
   {   
    sprintf(strsql,"%d.%s.%d.%d",skl,shu.ravno(),kodm,nk);
    if(iceb_u_strlen(strsql) > *kol_znak_nomn)
     *kol_znak_nomn=iceb_u_strlen(strsql);
   }

 }


return(0);



}