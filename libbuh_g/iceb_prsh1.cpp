/*$Id: iceb_prsh1.c,v 1.10 2013/05/17 14:58:36 sasa Exp $*/
/*09.03.2016	16.10.2000	Белых А.И.	iceb_prsh1.c
Проверка счета

Если вернули 0 - все впорядке
	     1 - нет
*/
#include	"iceb_libbuh.h"


int iceb_prsh1(const char *shet,struct OPSHET *shetv,GtkWidget *wpredok)
{
int		kl;
char		bros[512];

if((kl=iceb_prsh(shet,shetv,wpredok)) == 0)
 {
  sprintf(bros,gettext("Нет счета %s в плане счетов !"),shet);
  iceb_menu_soob(bros,wpredok);
  return(1);
 }
if(kl == 2)
 {
  sprintf(bros,gettext("Счет %s имеет субсчета ! Введите нужный субсчет."),shet);
  iceb_menu_soob(bros,wpredok);
  return(2);
 }
if(kl == 4)
 {
  sprintf(bros,"%s %s %s!",gettext("Счет"),shet,gettext("заблокирован в плане счетов"));
  iceb_menu_soob(bros,wpredok);
  return(4);
 }
return(0);

}
/************************************/
int iceb_prsh1(const char *shet,GtkWidget *wpredok)
{
struct OPSHET shetv;
return(iceb_prsh1(shet,&shetv,wpredok));
}