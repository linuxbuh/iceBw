/* $Id: zaravprw.c,v 1.32 2013/08/13 06:10:03 sasa Exp $ */
/*22.12.2012	23.09.1998	Белых А.И.	zaravprw.c
Выполнение проводок для зароботной платы
*/
#include        "buhg_g.h"

void avtprozar(int tbn,short mp,short gp,FILE *ff_prot,GtkWidget *wpredok);
void zaravpr_oldw(int tbn,short mp,short gp,FILE *ff_prot,GtkWidget *wpredok);

extern SQL_baza bd;

void zaravprw(int tbn,short mp,short gp,FILE *ff_prot,GtkWidget *wpredok)
{

int metka_prov=0;
class iceb_u_str stroka("");
if(iceb_poldan("Автоматическое выполнение проводок с использованием таблицы",&stroka,"zarnast.alx",wpredok) == 0)
 if(iceb_u_SRAV(stroka.ravno(),"Вкл",1) == 0)
  metka_prov=1;
/*проверка на разрешение делать проводки внутри подпрограмм*/
if(metka_prov == 1)
  zaravpr_oldw(tbn,mp,gp,ff_prot,wpredok);
else
 avtprozar(tbn,mp,gp,ff_prot,wpredok);

}