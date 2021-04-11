/*$Id: iceb_close_dserver.c,v 1.10 2013/05/17 14:58:34 sasa Exp $*/
/*16.08.2005	28.04.2005	Белых А.И.	iceb_close_dserver.c
Прекращение работы с сервером считывателя карточек
*/
#include "iceb_libbuh.h" //Обязятелен
#ifdef READ_CARD
extern tdcon           td_server; 
#endif

void iceb_close_dserver()
{

#ifdef READ_CARD
printf("Закрыта работа клиента\n");
td_close_server(td_server);
#endif

}
