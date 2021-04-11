/*$Id: rasdok1w.c,v 1.29 2013/11/05 10:51:25 sasa Exp $*/
/*24.04.2018	09.07.2003	Белых А.И.	rasdok1w.c
Распечатка накладных с учетной ценой
*/
#include        <errno.h>
#include	"buhg_g.h"
extern int kol_strok_na_liste_l_u;

void	vztr1(short,short,short,int,const char*,double*,FILE*,int*,int*,int,GtkWidget*);
void    srasdok1(int,int*,int*,FILE*);
void    shrasdok1(int,int*,int*,FILE*);

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern short    srtnk; /*0-не включена 1-включена сортировка записей в накладной*/
extern short	vtara; /*Код группы возвратная тара*/
extern SQL_baza bd;
extern int kol_strok_na_liste;

void rasdok1w(short dg,short mg,short gg, //дата документа
int skl,  //Склад
const char *nomdok, //Номер документа
short lnds,  //Льготы по НДС*
double ves, //Вес
GtkWidget *wpredok)
{
class iceb_u_str kontr("");
class iceb_u_str kodop("");
int		tipz=0;
class iceb_u_str naimpr("");
char		strsql[1024];
class iceb_u_str bros("");
class iceb_u_str bros1("");
SQL_str		row,row1,rowtmp;
class iceb_u_str nomnn(""); /*Номер налоговой накладной*/
class iceb_u_str nn1(""); /*Номер встречного документа*/
short		mtpr=0; /*Метка первого и второго прохода*/
short		mtar=0; /*Метка наличия тары в накладной*/
class iceb_u_str kdtr(""); /*Коды тары*/
short		kgrm=0; /*код группы материала*/
class iceb_u_str naim("");
int             kodm,nk;
double		kratn,cenan;
short		vtar=0; /*Метка возвратной тары*/
short		mnt=0; /*Метка наличия тары*/
double		mest=0.;
int		i1;
int		kollist=0,kolstrok=0;
class iceb_u_str ein("");
double		kolih=0.;
double		cenareal=0.; //Цена реализации
double		suma=0.;
double		itogo=0.;
double		itogoreal=0.;
int		kolstr=0;
//double		sumkor=0.;
double		sumt=0.;
int		nomer=0;
class iceb_u_str artikul("");
class iceb_u_str kodkontr00("00");

SQLCURSOR cur,cur1,curtmp;


/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gg,mg,dg,skl,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Не найден документ !")),
  repl.plus(strsql);
  repl.ps_plus(strsql);
  
  sprintf(strsql,"%s %d.%d.%d %s %s %s %d",
  gettext("Дата"),dg,mg,gg,
  gettext("Документ"),nomdok,
  gettext("Слад"),skl);
  
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  
  return;
 }
float pnds=atof(row[13]);
kontr.new_plus(row[3]);
kodop.new_plus(row[6]);
tipz=atoi(row[0]);
nomnn.new_plus(row[5]);
nn1.new_plus(row[11]);
kodkontr00.new_plus(row[15]);
int mvnpp=0;
/*Читаем наименование операции*/
if(tipz == 1)
 bros.new_plus("Prihod");
if(tipz == 2)
 bros.new_plus("Rashod");

sprintf(strsql,"select naik,vido from %s where kod='%s'",
bros.ravno(),kodop.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден код операции"));
  repl.plus(" ");
  repl.plus(kodop.ravno());
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);  
 }
else
 {

  mvnpp=atoi(row[1]);
  naimpr.new_plus(row[0]);
  
 }

short kor=0;
iceb_poldan("Корректор межстрочного растояния",&kor,"matnast.alx",wpredok);
//kor=(short)iceb_u_atof(bros);

short ots=0;
iceb_poldan("Отступ от левого края",&ots,"matnast.alx",wpredok);
//ots=(short)iceb_u_atof(bros);

iceb_poldan("Код тары",&kdtr,"matnast.alx",wpredok);

if(tipz == 2)
 {
  if(iceb_u_SRAV("00",kodkontr00.ravno(),0) == 0)
    kodkontr00.new_plus(iceb_getk00(0,kodop.ravno(),wpredok));
 }



class iceb_tmptab tabtmp;
const char *imatmptab={__FUNCTION__};

if(sortdokmuw(dg,mg,gg,skl,nomdok,imatmptab,&tabtmp,NULL,wpredok) != 0)
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
//sortnakw(dg,mg,gg,skl,nomdok,wpredok);

char imafdok[64];
FILE *ffdok;


sprintf(imafdok,"nsuc%d.lst",getpid());
if((ffdok = fopen(imafdok,"w")) == NULL)
 {
  iceb_er_op_fil(imafdok,"",errno,wpredok);
  return;
 }

rashapw(tipz,&kolstrok,kodkontr00.ravno(),kontr.ravno(),ots,kor,ffdok,wpredok);


class iceb_u_str naiskl("");
class iceb_u_str fmoll("");

/*Читаем наименование склада*/
sprintf(strsql,"select naik,fmol from Sklad where kod=%d",skl);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден код склада"));
  repl.plus(" ");
  repl.plus((int)skl);
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);  
 }
else
 {
  naiskl.new_plus(row[0]);
  fmoll.new_plus(row[1]);
 }
fprintf(ffdok,"\n%s\n",naiskl.ravno());
kolstrok+=2;

if(mvnpp == 1) /*Внутреннее перемещение*/
 {
  if(iceb_u_atof(kontr.ravno()) == 0. && iceb_u_pole(kontr.ravno(),&bros,2,'-') == 0 && kontr.ravno()[0] == '0')
   {
    int skl1=0;
    iceb_u_polen(kontr.ravno(),&skl1,2,'-');
//    int skl1=(int)iceb_u_atof(bros);
    /*Читаем наименование склада*/
    sprintf(strsql,"select naik,fmol from Sklad where kod=%d",skl1);
    class iceb_u_str fmol("");
    if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не найден код склада"));
      repl.plus(" ");
      repl.plus(skl1);
      repl.plus(" !");
      iceb_menu_soob(&repl,wpredok);  
      naiskl.new_plus("");
     }
    else
     {
      naiskl.new_plus(row[0]);
      fmol.new_plus(row[1]);
     }
    if(tipz == 1)
       fprintf(ffdok,"\n\
%s:\n%s %d <= %s %d %s\n\
%s <= %s\n",
       naimpr.ravno(),gettext("Склад"),skl,
       gettext("Склад"),skl1,naiskl.ravno(),fmoll.ravno(),fmol.ravno());
    if(tipz == 2)
       fprintf(ffdok,"\n\
%s:\n%s %d => %s %d %s\n\
%s => %s\n",
       naimpr.ravno(),gettext("Склад"),skl,
       gettext("Склад"),skl1,naiskl.ravno(),fmoll.ravno(),fmol.ravno());
    kolstrok+=3;

   }
  else
   {
    fprintf(ffdok,"\n%s\n",naimpr.ravno());
    kolstrok+=2;
   }
 }
else
 { 

  if(iceb_poldan("Наименование операции в накладной",&bros,"matnast.alx",wpredok) == 0)
   if(iceb_u_SRAV(bros.ravno(),"Вкл",1) == 0)
    {
     fprintf(ffdok,"\n%s\n",naimpr.ravno());
     kolstrok+=2;
    }   
 }

//sprintf(bros,"N%s",nomdok);  
bros.new_plus("N");
bros.plus(nomdok);
if(tipz == 2 && iceb_poldan("Перенос символа N",&bros,"matnast.alx",wpredok) == 0)
 {
  if(iceb_u_SRAV(bros.ravno(),"Вкл",1) == 0)
   {
    iceb_u_polen(nomdok,&bros,1,'-');
    iceb_u_polen(nomdok,&bros1,2,'-');
    bros.plus("-N");  
    bros.plus(bros1.ravno());  
    
   }       
  else
   {
//    sprintf(bros,"N%s",nomdok);  
    bros.new_plus("N");
    bros.plus(nomdok);
   }
 } 

if(tipz == 1)
  fprintf(ffdok,"\n\
              %s %s %s %02d.%02d.%d%s\n",
  gettext("ПРИХОДНАЯ НАКЛАДНАЯ"),bros.ravno(),
  gettext("от"),dg,mg,gg,gettext("г."));

if(tipz == 2)
  fprintf(ffdok,"\n\
              %s %s %s %02d.%02d.%d%s\n",
  gettext("РАСХОДНАЯ НАКЛАДНАЯ"),bros.ravno(),
  gettext("от"),dg,mg,gg,gettext("г."));
kolstrok+=2;

/*Читаем доверенность*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,1);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  fprintf(ffdok,gettext("Доверенность"));
  fprintf(ffdok," N");
  iceb_u_polen(row[0],&bros,1,'#');
  fprintf(ffdok," %s",bros.ravno());
  iceb_u_polen(row[0],&bros,2,'#');
  fprintf(ffdok," %s %s\n",gettext("от"),bros.ravno());
  kolstrok++;
 }

/*Читаем через кого*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,2);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  fprintf(ffdok,"%s: %s\n",gettext("Через кого"),row[0]);
  kolstrok++;
 }

if(tipz == 2)
 {
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,3);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    fprintf(ffdok,"%s: %s\n",gettext("Основание"),row[0]);
    kolstrok++;
   }
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,9);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    short d,m,g;
    iceb_u_rsdat(&d,&m,&g,row[0],0);
//    if(d == 0 || iceb_u_SRAV1(g,m,d,gg,mg,dg) <= 0)
    if(d == 0 || iceb_u_sravmydat(d,m,g,dg,mg,gg) >= 0)
     {
      fprintf(ffdok,"\x1B\x45%s: %s\x1B\x45\x1b\x46\n",
      gettext("Конечная дата расчёта"),row[0]);
      kolstrok++;
     }
   }

  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,10);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)

  if(nomnn.ravno()[0] != '\0')
   {
    fprintf(ffdok,"%s: %s\n",
    gettext("Номер налоговой накладной"),nomnn.ravno());
    kolstrok++;
   }    
 }

if(nn1.ravno()[0] != '\0' && tipz == 1)
 {
  fprintf(ffdok,"%s: %s\n",
  gettext("Номер встречной накладной"),nn1.ravno());
  kolstrok++;
 }
if(nn1.ravno()[0] != '\0' && tipz == 2 && mvnpp > 0 )
 {
  fprintf(ffdok,"%s: %s\n",
  gettext("Номер встречной накладной"),nn1.ravno());
  kolstrok++;
 }

fprintf(ffdok,"\x1B\x4D"); /*12-знаков*/
//fprintf(ffdok,"\x0F");  /*Ужатый режим*/

srasdok1(tipz,&kollist,&kolstrok,ffdok);

naz:;
while(curtmp.read_cursor(&rowtmp) != 0)
 {

  kgrm=atoi(rowtmp[0]);
  naim.new_plus(rowtmp[1]);
  kodm=atoi(rowtmp[2]);
  nk=atoi(rowtmp[3]);
  cenan=atof(rowtmp[4]);
  cenan=iceb_u_okrug(cenan,okrcn);
  kratn=atoi(rowtmp[5]);
  ein.new_plus(rowtmp[6]);
  vtar=atoi(rowtmp[7]);
  kolih=atof(rowtmp[9]);
  artikul.new_plus(rowtmp[11]);
  
  if(vtar == 1)
   {
    mnt++;
    continue;
   } 

  mest=0;

  if(vtara != 0 && kgrm == vtara)
   {
    mnt++;
    continue;
   } 

  bros.new_plus(kgrm);
  if(kdtr.ravno()[0] != '\0' && mtpr == 0 )
    if(iceb_u_pole1(kdtr.ravno(),bros.ravno(),',',0,&i1) == 0 || iceb_u_SRAV(kdtr.ravno(),bros.ravno(),0) == 0)
     {  
      mtar++;
      continue;
     }


  if(kdtr.ravno()[0] != '\0' && mtpr == 1)
    if(iceb_u_pole1(kdtr.ravno(),bros.ravno(),',',0,&i1) != 0)
     if(iceb_u_SRAV(kdtr.ravno(),bros.ravno(),0) != 0)
       continue;
  mest=0.;
  if(kratn != 0.)
   {
    mest=kolih/kratn;
    mest=iceb_u_okrug(mest,0.1);
   }

  suma=cenan*kolih;
  suma=iceb_u_okrug(suma,okrg1);

  itogo+=suma;
  

  cenareal=0.;

  if(tipz == 1)
   {
    //Читаем цену реализации в списке товаров
    if(mvnpp == 1) /*Внутреннее перемещение*/
     {
      //Читаем учетную цену в карточке
      sprintf(strsql,"select cenap from Kart where sklad=%d and nomk=%d",skl,nk);
      if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
         cenareal=atof(row[0]);    
     }
    else
     {
      sprintf(strsql,"select cenapr from Material where kodm=%d",kodm);
      if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
       cenareal=atof(row[0]);    
     }
   }
  if(tipz == 2)
   {
    //Читаем учетную цену в карточке
    sprintf(strsql,"select cena,cenap from Kart where sklad=%d and nomk=%d",skl,nk);
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     {
      if(mvnpp == 1) /*Внутреннее перемещение*/
       cenareal=atof(row[1]);    
      else
       cenareal=atof(row[0]);    
     }    
   }
  double suma1=cenareal*kolih;  
  suma1=iceb_u_okrug(suma1,okrg1);

  itogoreal+=suma1;

  if(artikul.ravno()[0] != '\0')
   {
    sprintf(strsql," %s:%s",gettext("Артикул"),artikul.ravno());
    naim.plus(strsql);
   }

  shrasdok1(tipz,&kollist,&kolstrok,ffdok);

  fprintf(ffdok,"%2d %5d %-*.*s %-*.*s %8s %10.10g",
  ++nomer,
  kodm,
  iceb_u_kolbait(25,naim.ravno()),iceb_u_kolbait(25,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(4,ein.ravno()),iceb_u_kolbait(4,ein.ravno()),ein.ravno(),
  iceb_prcn(cenan),kolih);
  
  fprintf(ffdok," %10s %8.2f",
  iceb_prcn(suma),cenareal);

  rasdokkr(kolih,mest,kratn,ffdok);
  if(iceb_u_strlen(naim.ravno()) > 25)
   {
    shrasdok1(tipz,&kollist,&kolstrok,ffdok);
    fprintf(ffdok,"%2s %5s %s\n"," "," ",iceb_u_adrsimv(25,naim.ravno()));
   }  

 }
 
mtpr++;
/*
printw("mtpr=%d kdtr.ravno()=%s mtar=%d\n",mtpr,kdtr.ravno(),mtar);
OSTANOV();
*/
if(mtpr == 1 && kdtr.ravno()[0] != '\0' && mtar != 0) /*Распечатываем отдельно тару*/
 {
  kolstrok++;

  fprintf(ffdok,"\
- - - - - - - - - - - - - - - - - - - - %s - -- - - - - - - - - - - - - - - - - - - - - \n",
    gettext("Т а р а"));

  curtmp.poz_cursor(0);
  goto naz;
 }


/*Распечатываем услуги*/
sprintf(strsql,"select * from Dokummat3 where \
datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",gg,mg,dg,skl,nomdok);
/*printw("\nstrsql=%s\n",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
class iceb_u_str naiusl("");

while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[4]);
  cenan=atof(row[5]);
  cenan=iceb_u_okrug(cenan,okrcn);

  naiusl.new_plus("");
  if(atoi(row[10]) != 0)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[10]);
    printf("strsql=%s\n",strsql);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
       naiusl.new_plus(row1[0]);
   }  

  if(naiusl.ravno()[0] == '\0')
   naiusl.new_plus(row[7]);  
  else
   {
    naiusl.plus(" ",row[7]);
   }
  
  if(kolih > 0)
    suma=cenan*kolih;
  else
    suma=cenan;
  
  suma=iceb_u_okrug(suma,okrg1);

  shrasdok1(tipz,&kollist,&kolstrok,ffdok);

  fprintf(ffdok,"%2d %5s %-*.*s %-*.*s %8s %10.10g %8s\n",
  ++nomer," ",
  iceb_u_kolbait(25,naiusl.ravno()),iceb_u_kolbait(25,naiusl.ravno()),naiusl.ravno(),
  iceb_u_kolbait(4,row[3]),iceb_u_kolbait(4,row[3]),row[3],
  iceb_prcn(cenan),kolih,iceb_prcn(suma));
  if(iceb_u_strlen(naiusl.ravno()) > 25)
   {
    shrasdok1(tipz,&kollist,&kolstrok,ffdok);
    fprintf(ffdok,"%2s %5s %s\n"," "," ",iceb_u_adrsimv(25,naiusl.ravno()));
   }  

  itogo+=suma;

 }
 
fprintf(ffdok,"\
-------------------------------------------------------------------------------------------\n");
fprintf(ffdok,"\
%*s: %10s",iceb_u_kolbait(58,gettext("Итого")),gettext("Итого"),iceb_prcn(itogo));
fprintf(ffdok,"(%8s)\n",iceb_prcn(itogoreal));

double itogo1=itogo;
if(mvnpp == 0) /*Внешнее перемещение*/
 {
  short           nnds=0; //0-накладная 1-накладная с ценой НДС 2-накладная с номером заказа
  int dlinna=58;
  itogo1=dok_end_muw(tipz,dg,mg,gg,nomdok,skl,dlinna,itogo,pnds,lnds,mnt,nnds,ffdok,wpredok);
 }
else
 { 
  sumt=0.;
  if(mnt != 0)
     vztr1(dg,mg,gg,skl,nomdok,&sumt,ffdok,&kollist,&kolstrok,tipz,wpredok);
  itogo1+=sumt;
 }

memset(strsql,'\0',sizeof(strsql));
iceb_u_preobr(itogo1,strsql,0);

fprintf(ffdok,"%s:%s\n",gettext("Сумма прописью"),strsql);

fprintf(ffdok,"\n%s%s\n",
        gettext("Руководитель__________________        Главный  бухгалтер_______________\n\n"),
        gettext("Отпустил______________________        Принял__________________________"));

iceb_podpis(ffdok,wpredok);

fclose(ffdok);

iceb_u_spisok imafils;
iceb_u_spisok naimf;
imafils.plus(imafdok);
naimf.plus(gettext("Распечатка накладной с ценами учета/реализации"));
iceb_ustpeh(imafdok,1,wpredok);

iceb_rabfil(&imafils,&naimf,wpredok);

}
/*******************************************/
/* Шапка документа                         */
/*******************************************/

void srasdok1(int tipz,int *kollist,int *kolstrok,FILE *ffdok)
{

*kollist+=1;
*kolstrok+=5;

fprintf(ffdok,"%82s%sN%d\n","",gettext("Лист"),*kollist);

fprintf(ffdok,"\
-------------------------------------------------------------------------------------------\n");

if(tipz == 1)
 {
  fprintf(ffdok,gettext("\
N |Код  |Наименование товара,     |Ед. |Цена пр-|Количество|  Сумма   |  Цена  |Кол.|Крат-|\n"));
  fprintf(ffdok,gettext("\
  |ма-ла|      материала          |изм.|бретения|          |          |реализац|м-ст|ность|\n"));
 }

if(tipz == 2)
 {
  fprintf(ffdok,gettext("\
N |Код  |Наименование товара,     |Ед. |  Цена  |Количество|  Сумма   |  Цена  |Кол.|Крат-|\n"));
  fprintf(ffdok,gettext("\
  |ма-ла|      материала          |изм.|реализац|          |          |учетная |м-ст|ность|\n"));
 }

fprintf(ffdok,"\
-------------------------------------------------------------------------------------------\n");

}
/****************************/
/*Счетчик                   */
/****************************/
void   shrasdok1(int tipz,int *kollist,int *kolstrok,FILE *ffdok)
{
*kolstrok+=1;
if(*kolstrok <= kol_strok_na_liste_l_u)
 return;

fprintf(ffdok,"\f");
*kolstrok=0;
srasdok1(tipz,kollist,kolstrok,ffdok);
*kolstrok+=1; //Плюс следующая за шапкой строка

}
/*****************/
/*Распечатка тары*/
/*****************/

void vztr1(short dd,short md,short gd,int skl,const char *nomdok,
double *sumt,FILE *ffdok,int *kollist,int *kolstrok,int tipz,GtkWidget *wpredok)
{
double		itogo;
SQL_str         row,row1;
char		strsql[512];
long		kolstr;
double		kolih,cenan;
int		nk,kgrm;
class iceb_u_str naim("");
short		vtr;
int		i;
double		cenareal=0.;
int		kodm=0;
double		suma=0.;

sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,skl,nomdok);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 { 
  return;
 } 
SQLCURSOR cur1;
itogo=0.;
i=0;
while(cur.read_cursor(&row) != 0)
 {
/*
  printw("%s %s %s %s %s %s %s %s %s %s %s\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],
  row[10]);
*/
  
  /*Читаем наименование материала*/
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%s",
  row[4]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
    {
     printf("%s-Не нашли наименование материала по коду %s\n",__FUNCTION__,row[4]);
     naim.new_plus("");
    }
  else
   {
    naim.new_plus(row1[0]);
    if(row[16][0] != '\0')
     {
      naim.plus(" ",row[16]);
     }
   }
  kgrm=atoi(row1[1]);
  vtr=atoi(row[8]);

  if(vtr == 0. && (vtara != 0 && kgrm != vtara))
    continue;
  if(vtr == 0 && vtara == 0 )
    continue;

  nk=atoi(row[3]);
  kodm=atoi(row[4]);
  kolih=atof(row[5]);
  cenan=atof(row[6]);
  cenan=iceb_u_okrug(cenan,okrcn);

/*
  printw("mk2.kodm=%d %.2f vt=%d %d\n",mk2.kodm,mk2.nds,vt,mk8.kgrm);
  refresh();  
*/
  
  suma=cenan*kolih;
  suma=iceb_u_okrug(suma,okrg1);
  i++;
  if(i == 1)
   {   
//    shrasdok1(tipz,kollist,kolstrok,ffdok);
    fprintf(ffdok,"%s:\n",gettext("Оборотная тара"));
   }

//  shrasdok1(tipz,kollist,kolstrok,ffdok);

  cenareal=0.;
  if(tipz == 1)
   {
    //Читаем цену реализации в списке товаров
    sprintf(strsql,"select cenapr from Material where kodm=%d",kodm);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     cenareal=atof(row1[0]);    
   }
  if(tipz == 2)
   {
    //Читаем учетную цену в карточке
    sprintf(strsql,"select cena from Kart where sklad=%d and nomk=%d",skl,nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     cenareal=atof(row1[0]);    
    
   }

  fprintf(ffdok,"%8d %-*.*s %-*s %10s %10.10g",
  kodm,
  iceb_u_kolbait(40,naim.ravno()),iceb_u_kolbait(40,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(4,row[7]),row[7],
  iceb_prcn(cenan),kolih);
  
  fprintf(ffdok," %10s %10.2f\n",
  iceb_prcn(suma),cenareal);
  if(iceb_u_strlen(naim.ravno()) > 40)
   {
    fprintf(ffdok,"%8s %s\n"," ",iceb_u_adrsimv(40,naim.ravno()));
   }  


  itogo+=suma;

 }
*sumt=itogo;
}
