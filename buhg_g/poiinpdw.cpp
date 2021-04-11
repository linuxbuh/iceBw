/* $Id: poiinpdw.c,v 1.6 2013/05/17 14:58:23 sasa Exp $ */
/*12.07.2015	28.07.1999	Белых А.И. 	poiinpdw.c
Поиск переменных данных на инвентарный номер
Если вернули 0 нашли настройку
             3 - нет ни одной записи

*/
#include	"buhg_g.h"

int poiinpdw(int in, //Инвентарный номер
short m,short g, //Дата
class poiinpdw_data *data,
GtkWidget *wpredok)
{
SQL_str         row;
char		strsql[512];

data->clear(); 
if(m != 0) 
  sprintf(strsql,"select * from Uosinp where innom=%d \
and (god < %d or (god = %d and mes <= %d)) order by god desc,mes desc limit 1",
  in,g,g,m);
else
  sprintf(strsql,"select * from Uosinp where innom=%d  order by god desc,mes desc limit 1",in);

class SQLCURSOR cur;

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
 return(3);


data->mz=(short)atoi(row[1]);
data->gz=(short)atoi(row[2]);
data->shetu.new_plus(row[3]);
data->hzt.new_plus(row[4]);
data->hau.new_plus(row[5]);
data->hna.new_plus(row[6]);
data->popkf=atof(row[7]);
data->soso=atoi(row[8]);
data->nomz.new_plus(row[9]);
data->hnaby.new_plus(row[12]);
data->popkfby=atof(row[13]);

/********
printw("popkfby=%f popkf=%f hna=%s shetu=%s\n",
*popkfby,*popkf,hna,shetu);
OSTANOV();
*********/

return(0);

}



