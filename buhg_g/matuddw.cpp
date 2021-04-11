/* $Id: matuddw.c,v 1.11 2013/05/17 14:58:22 sasa Exp $ */
/*19.10.2018    25.01.1994      Белых А.И.      matuddw.c
Удаляем в карточке запись или во всех карточках с указанным номером
документа
Если в документе не остаётся записей удаляем сам документ
Если запись на приход то необходимо проверить
какой остаток будет в карточке если запись удалить
Если остаток отрицательный то были проведены расходы с учетом
этого материалла
Если запись на расход то удаляем без проверок
Если вернули 0 - накладная удалена полностью
	     1 - удалена не полностью
*/
#include	"buhg_g.h"

extern SQL_baza  bd;
int            udmk10(int,short,short,short,const char[],int,int,GtkWidget*);

int matuddw(short mu, //0-записи во всех карточках 1-в конкретной карточке
short mprv, //*0-проверять на остаток 1-нет
short mun, //*0-Удалять накладную 1 - нет
int tipz,
short d,short m,short g,const char *nomdok,int skl,int nk,int kodm,GtkWidget *wpredok)
{
double          kol;
char		strsql[512];
long		kolstr;
long		kolust=0;
SQL_str         row,row1;
int		kodmm;


//printf("%s-Режим удаления %d.%d.%d\n",__FUNCTION__,d,m,g);


kolust=0;

sprintf(strsql,"select nomkar,kodm from Dokummat1 where \
datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
g,m,d,skl,nomdok);

if(mu == 1)
 sprintf(strsql,"select nomkar,kodm from Dokummat1 where \
datd='%d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and \
nomkar=%d",
  g,m,d,skl,nomdok,kodm,nk);

//printf("%s-%s\n",__FUNCTION__,strsql);

class SQLCURSOR cur;
class SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

/*
  printw("\nstrsql=%s\nkolstr=%ld\n",strsql,kolstr);
  OSTANOV();
*/
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
//  printf("%s-%s %s\n",__FUNCTION__,row[0],row[1]);
  
  nk=atoi(row[0]);
  kodmm=atoi(row[1]);
  if(nk == 0)
    goto vper1;
    
  if(tipz == 1 && mprv == 0 && nk != 0)
   {
    class ostatok data;
    ostkarw(1,1,g,31,m,g,skl,nk,&data);

    if((kol=readkolkw(skl,nk,d,m,g,nomdok,wpredok)) == 0)
      goto vpr;

    if(data.ostg[3]-kol < -0.000000001)
     {
      class iceb_u_str naimat("");
      sprintf(strsql,"select naimat from Material where kodm=%s",row[1]);
      if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
        naimat.new_plus(row1[0]);

      iceb_u_str repl;
      

      sprintf(strsql,"%s-%s %s",__FUNCTION__,row[1],naimat.ravno());
      repl.plus(strsql);
      repl.ps_plus(gettext("Склад"));
      repl.plus(":");
      repl.plus(skl);

      repl.plus(" ");
      repl.plus(gettext("Карточка"));
      repl.plus(":");
      repl.plus(nk);
      

      repl.plus(" ");
      repl.plus(gettext("Невозможно удалить приход !"));

      repl.ps_plus(gettext("Документ"));
      repl.plus(":");
      repl.plus(nomdok);
      repl.plus(" ");

      sprintf(strsql,"%s %d.%d.%d",gettext("Дата"),d,m,g);
      repl.plus(strsql);
      
      sprintf(strsql,"%s %f",gettext("Отрицательный остаток ! Удаление невозможно !"),data.ostg[3]-kol);
      repl.ps_plus(strsql);
      iceb_menu_soob(&repl,wpredok);
      return(1);
     }
//    printf("%s-1\n",__FUNCTION__);
    
    if(udmk10(tipz,d,m,g,nomdok,skl,nk,wpredok) != 0)
      return(1);
vpr:;
//    printf("%s-2\n",__FUNCTION__);
    udkarw(skl,nk,wpredok);

   }
  else
   {
    if(udmk10(tipz,d,m,g,nomdok,skl,nk,wpredok) != 0)
      return(1);
    udkarw(skl,nk,wpredok);
   }

  if(tipz == 1 && mprv == 1)
   {
    udkarw(skl,nk,wpredok);
   }


vper1:;

  sprintf(strsql,"delete from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
  g,m,d,skl,nomdok,kodmm,nk);
/*
  printw("\nstrsql=%s\nkolust=%ld\n",strsql,kolust);
  OSTANOV();
*/
  if(iceb_sql_zapis(strsql,1,0,wpredok) == 0)
   {
    kolust++;
    sprintf(strsql,"delete from Dokummat4 where datd='%d-%02d-%02d' and skl=%d and nomd='%s' and km=%d and nk=%d",
    g,m,d,skl,nomdok,kodmm,nk);
    iceb_sql_zapis(strsql,1,0,wpredok);
  }    
 }


/* Если в накладной удалена последняя запись
 то нужно удалить и саму накладную*/


if(mun == 0 && kolust == kolstr)
 {
  matudw(nomdok,d,m,g,skl,tipz,wpredok);
 } 

return(0);
}
/****************************/
/*Если вернули 1 доступ к базе толко на чтение*/
int udmk10(int tipz,short d,short m,short g,const char nomdok[],
int skl,int nk,GtkWidget *wpredok)
{
char		strsql[512];

sprintf(strsql,"delete from Zkart where datd='%d-%02d-%02d' \
and nomd='%s' and sklad=%d and nomk=%d",
g,m,d,nomdok,skl,nk);
/*
printw("\nstrsql=%s\n",strsql);
OSTANOV();
*/
if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_u_str repl;
   repl.plus(gettext("У вас нет полномочий для выполнения этой операции !"));
   iceb_menu_soob(&repl,wpredok);
   return(1); 
  }
 else
   iceb_msql_error(&bd,"matudd-udmk10-Ошибка удаления записи в карточке !",strsql,wpredok);
 }
return(0);
}
