/*$Id: zar_rtabel_r.c,v 1.18 2013/09/26 09:47:00 sasa Exp $*/
/*23.05.2016	14.12.2006	Белых А.И.	zar_rtabel_r.c
Распечатка заготовки для заполнения табеля
*/
#include <errno.h>
#include "buhg_g.h"
#include "zar_rtabel.h"

class zar_rtabel_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class zar_rtabel_rek *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_rtabel_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_rtabel_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_rtabel_r_data *data);
gint zar_rtabel_r1(class zar_rtabel_r_data *data);
void  zar_rtabel_r_v_knopka(GtkWidget *widget,class zar_rtabel_r_data *data);

void gsaptb(int *sl,int *kol_strok,FILE *kaw);
void tabel_sap(int *kol_strok,FILE *kaw);


extern SQL_baza bd;

extern int kol_strok_na_liste;
extern short mmm,ggg;

int zar_rtabel_r(class zar_rtabel_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zar_rtabel_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка списка работников"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_rtabel_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

repl.plus(gettext("Распечатка списка работников"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,wpredok));
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno());

gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);


data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_rtabel_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zar_rtabel_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_rtabel_r_v_knopka(GtkWidget *widget,class zar_rtabel_r_data *data)
{
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   zar_rtabel_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_rtabel_r_data *data)
{
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}

/******************************************/
/******************************************/

gint zar_rtabel_r1(class zar_rtabel_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
int kolstr=0;
class iceb_clock sss(data->window);

SQL_str row;
class SQLCURSOR cur;

//читаем наименование подразделения
char podraz[112];
memset(podraz,'\0',sizeof(podraz));
sprintf(strsql,"select naik from Podr where kod=%d",data->rk->podr.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
 strncpy(podraz,row[0],sizeof(podraz)-1);



sprintf(strsql,"select tabn,fio,datk,podr,kateg,dolg from Kartb \
where podr=%d order by podr,tabn asc",data->rk->podr.ravno_atoi());

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не нашли ни одной записи !"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }
char imaf[56];
FILE *kaw;

sprintf(imaf,"tab%d.lst",getpid());
if((kaw = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


fprintf(kaw,"\
%s %s - %s\n\n\
                                                                                         %s\n\
                                                                            %s\n\
                                                                                     за %02d.%dр.\n\n",
iceb_get_pnk("00",0,data->window),
gettext("Подразделение"),
podraz,
gettext("ТАБЕЛЬ"),
gettext("учёта использования рабочего времени"),
mmm,ggg);
int kol_strok=6;

tabel_sap(&kol_strok,kaw);

int nst=0;
int sl=1;
float kolstr1=0;
int tabn;
int podr;
short d,m,g;
int kateg;
char dolg[112];

while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    
  tabn=atol(row[0]);

  /*Дата увольнения*/
  if(iceb_u_rsdat(&d,&m,&g,row[2],2) == 0)
    continue;


  /*Подразделение*/
  podr=atoi(row[3]);
  if(data->rk->podr.ravno_atoi() != 0)
   if(data->rk->podr.ravno_atoi() != podr)
     continue;

  /*Категория*/
  kateg=atoi(row[4]);

  /*Должность*/
  strncpy(dolg,row[5],sizeof(dolg)-1);


  nst++;

  sprintf(strsql,"%5d %s\n",tabn,row[1]);
  iceb_printw(strsql,data->buffer,data->view);

  gsaptb(&sl,&kol_strok,kaw);

  fprintf(kaw,"\
 %02d %02d %02d %-5d %-*.*s %-*.*s|   |  |    |   |   |   |   |   |  |   |      |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |\n\
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n",
  podr,nst,kateg,tabn,
  iceb_u_kolbait(30,row[1]),iceb_u_kolbait(30,row[1]),row[1],
  iceb_u_kolbait(15,dolg),iceb_u_kolbait(15,dolg),dolg);



 }
iceb_podpis(kaw,data->window);

fclose(kaw);
sprintf(strsql,"\n%s %d\n",
gettext("Количество листов"),sl);

iceb_printw(strsql,data->buffer,data->view);


data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Бланк для учёта отработанного времени"));

iceb_ustpeh(imaf,0,data->window);


data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/*********************/
/*печать шапки*/
/******************/
void tabel_sap(int *kol_strok,FILE *kaw)
{

*kol_strok+=10;

fprintf(kaw,"\
=============================================================================================================================================================================================================================================================\n");

fprintf(kaw,gettext("\
|  |Н |К |     |                              |               |   |  |    |Ночные |Вечерн.|Друг.виды оплаты |Пс|                                Д н и   м е с я ц а                                         |К |О |С |Б |Д |А |У |П |Го|  |Вп|Дв| Н/д |И |И |\n\
|У |о |а |     |                              |               |   |  |    |-------|-------|-----------------|ро|                                                                                            |о |т |п |о |р |д |ч |р |об|  |ыр|оы|-----|т |тн|\n\
|ч |мс|т | Та- |      Ф а м и л и я           |               |  о|  | Ч  |В о| Ч |В о| Ч |В о|Д | Ч |      |ив|--------------------------------------------------------------------------------------------|мо|п |р |л |у |м |е |о |ся|  |ха|пх|  |Пт|о |ое|\n\
|а |ет|ег|бель |          И м я               |  Должность    |В п|Д | а  |и п| а |и п| а |и п|н | а |      |зм|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |ав|у |а |ь |г |и |б |г |уз|  |оз|оо|  |ок|г |г |\n\
|с |рр| о| ный |      О т ч е с т в о         |               |и л|н | с  |д л| с |д л| с |д л|и | с | Сумма|не|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |нк|с |в |н |и |н |н |у |да|  |дд|лд|  |до|оя|оя|\n\
|т | о| р|номер|                              |               |д а|и | ы  |  а| ы |  а| ы |  а|  | ы | грн. |ас|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31|ди|к |к |и |е |и |ы |л |ан|  |нн|нн|  |рв| в| в|\n\
|о | к| и|     |                              |               |  т|  |    |  т|   |  т|   |  т|  |   |      |кт|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |и |  |и |ч |  |с |е |ы |рс|  |ы.|иы|  |оы| о| о|\n\
|к | и| я|     |                              |               |  ы|  |    |  ы|   |  ы|   |  ы|  |   |      | .|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |р |  |  |. |  |т |  |  |..|  |е |.е|  |се| к| к|\n"));

fprintf(kaw,"\
=============================================================================================================================================================================================================================================================\n");

}
/*******/
/*Шапка*/
/*******/
void            gsaptb(int *sl,int *kol_strok,FILE *kaw)
{
*kol_strok+=2;
if(*kol_strok <= kol_strok_na_liste)
 return;

fprintf(kaw,"\f");
*sl+=1;
*kol_strok=2;
tabel_sap(kol_strok,kaw);


}
