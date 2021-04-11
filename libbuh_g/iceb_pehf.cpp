/*$Id: iceb_pehf.c,v 1.12 2013/05/17 14:58:36 sasa Exp $*/
/*29.09.2014	14.10.1997	Белых А.И.	iceb_pehf.c
Печать файла на системный или приэкранный принтер
*/
#include	"iceb_libbuh.h"



void	iceb_pehf(const char *imaf, //Имя файла
short kk,  //0-запрос на количесто >0 введено количество
GtkWidget *wpredok)
{


iceb_print_operation(GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,imaf,wpredok);

}

