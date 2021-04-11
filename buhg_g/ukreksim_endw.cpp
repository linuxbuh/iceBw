/*$Id: ukreksim_endw.c,v 1.10 2013/05/17 14:58:28 sasa Exp $*/
/*13.07.2015	25.01.2006	Белых А.И.	ukreksim_endw.c
Концовка формирования распечатки и дискеты для Укрэксимбанка
*/
#include "buhg_g.h"


void ukreksim_endw(int nomer,const char *imaftmp,char *imafdbf,
double sumai,const char *rukov,const char *glavbuh,FILE *fftmp,FILE *ffr,
GtkWidget *wpredok)
{
char strsql[512];

fputc(26, fftmp);
fprintf(ffr,"\
-----------------------------------------------------------------------\n");
fprintf(ffr,"%*s:%10.2f\n",iceb_u_kolbait(29,"Разом"),"Разом",sumai);
memset(strsql,'\0',sizeof(strsql));

iceb_u_preobr(sumai,strsql,0);

fprintf(ffr,"\nСумма прописом:\n%s\n",strsql);

fprintf(ffr,"\n\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",rukov,glavbuh);
iceb_podpis(ffr,wpredok); 
fclose(fftmp);
fclose(ffr);


sprintf(imafdbf,"OPER.DBF");

ukreksim_hw(imafdbf,nomer);

/*Сливаем два файла*/
iceb_cat(imafdbf,imaftmp,wpredok);
unlink(imaftmp);
}
