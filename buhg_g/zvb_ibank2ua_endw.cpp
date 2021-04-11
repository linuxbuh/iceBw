/*$Id: zvb_ibank2ua_endw.c,v 1.6 2013/05/17 14:58:31 sasa Exp $*/
/*25.01.2020	18.09.2008	Белых А.И.	zvb_ibank2ua_endw.c
Концовка формирования данных для зачисления на карт-счета для системы iBank2 UA
*/
#include "buhg_g.h"


void zvb_ibank2ua_endw(const char *imaf_lst,double sumai,FILE *ffr,FILE *ff,GtkWidget *wpredok)
{

zvb_end_lstw(imaf_lst,sumai,ffr,wpredok);




class iceb_u_str shet_bp("29242000000110");


fprintf(ff,"\
AMOUNT=%.2f\n\
PAYER_BANK_ACCOUNT=%s\n\
COMMISSION_PERCENT=0.00\n",
sumai,
shet_bp.ravno());

fclose(ff);


}
