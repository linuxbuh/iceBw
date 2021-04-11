/*$Id: zarreadnw.c,v 1.13 2013/08/13 06:10:03 sasa Exp $*/
/*16.02.2016	23.01.2009	Белых А.И.	zarreadnw.c
Чтение настроек перед автоматическим расчётом начислений и удерданий*/
#include "buhg_g.h"
#include "read_nast_indw.h"

int read_nast_ind(short mr,short gr,class index_ua *data,GtkWidget *wpredok);
void zar_mpslw(GtkWidget *wpredok);

float pnds=20.;
class index_ua tabl_ind; /*Таблица для индексации зарплаты*/

void zarreadnw(short dr,short mr,short gr,
int metka_r,   //1-начисления 2-удержания 3-начисления и удержания 4-соц отчисления 5-проводки
GtkWidget *wpredok)
{

pnds=iceb_pnds(dr,mr,gr,wpredok);

/*Читаем таблицу разрядов-коэффициентов*/
zartarrozw(wpredok);
zartarroz1w(wpredok);

class iceb_u_str znah("");

read_nast_ind(mr,gr,&tabl_ind,wpredok);

/*Чтение кодов единого социального взноса*/
zarrnesvw(wpredok);
if(metka_r == 2 || metka_r == 3)
 zar_mpslw(wpredok);


}

