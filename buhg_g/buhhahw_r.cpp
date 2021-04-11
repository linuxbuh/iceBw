/*$Id: buhhahw_r.c,v 1.25 2014/06/30 06:35:54 sasa Exp $*/
/*10.07.2015	20.06.2006	Белых А.И.	buhhahw_r.c
Расчёт отчёта 
*/
#include <errno.h>
#include "buhg_g.h"
#include "buhhahw.h"

class buhhahw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class buhhahw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  buhhahw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   buhhahw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhhahw_r_data *data);
gint buhhahw_r1(class buhhahw_r_data *data);
void  buhhahw_r_v_knopka(GtkWidget *widget,class buhhahw_r_data *data);

int buhhah_rsh(class iceb_u_spisok *sheta,char *naim_oth);
void buhhah_sort(class iceb_u_spisok *sheta,GtkWidget*);
int buhhar_rs(short dn,short mn,short gn,short dk,short mk,short gk,int metka_ras,class SQLCURSOR *cur,int kolstr,class iceb_u_spisok *sheta,FILE *ff,FILE *ff1,GtkWidget *bar,GtkWidget *wpredok);

extern SQL_baza bd;
extern double   okrg1;
extern double	okrcn;

int buhhahw_r(class buhhahw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_str repl;
class buhhahw_r_data data;
int gor=0;
int ver=0;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Расчёт шахматки"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(buhhahw_r_key_press),&data);

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

repl.plus(gettext("Расчёт шахматки"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(buhhahw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)buhhahw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  buhhahw_r_v_knopka(GtkWidget *widget,class buhhahw_r_data *data)
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

gboolean   buhhahw_r_key_press(GtkWidget *widget,GdkEventKey *event,class buhhahw_r_data *data)
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

gint buhhahw_r1(class buhhahw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);

short dn,mn,gn;
short dk,mk,gk;

if(iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

sprintf(strsql,"%d.%d.%d%s => %d.%d.%d%s\n",
dn,mn,gn,gettext("г."),
dk,mk,gk,gettext("г."));

iceb_printw(strsql,data->buffer,data->view);




sprintf(strsql,"select sh,shk,deb from Prov where datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d' \
and deb <> 0. order by datp asc",gn,mn,dn,gk,mk,dk);

SQLCURSOR cur;
int kolstr=0;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
//printw("Количество проводок:%d\n",kolstr);

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи!"),data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr_alx=0;
sprintf(strsql,"select str from Alx where fil='buhhah.alx' order by ns asc");
if((kolstr_alx=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr_alx == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"buhhah.alx");
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }


class iceb_u_str variant("");

char imaf[64];
FILE *ff;

sprintf(imaf,"hax%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf1[64];
FILE *ff1;

sprintf(imaf1,"haxg%d.lst",getpid());
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

class iceb_u_str naim_oth("");
class iceb_u_str spis_sh("");
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  

  if(iceb_u_polen(row_alx[0],&variant,1,'|') != 0)
   continue;
  if(iceb_u_SRAV("Вариант расчёта",variant.ravno(),0) != 0)
   continue;

  iceb_u_polen(row_alx[0],&variant,2,'|');
  if(variant.ravno()[0] == '\0')
   continue;

  if(iceb_u_proverka(data->rk->variant.ravno(),variant.ravno(),0,0) != 0)
   continue;
  
  class iceb_u_spisok sheta;
  naim_oth.new_plus("");
  
  /*Читаем наименование отчёта*/
  cur_alx.read_cursor(&row_alx);
  iceb_u_polen(row_alx[0],&naim_oth,2,'|');
  
  /*Читаем список счетов*/
  cur_alx.read_cursor(&row_alx);
  iceb_u_polen(row_alx[0],&spis_sh,2,'|');
  iceb_fplus(0,spis_sh.ravno(),&sheta,&cur_alx);
  
  iceb_u_zagolov(naim_oth.ravno(),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ff);
  iceb_u_zagolov(naim_oth.ravno(),dn,mn,gn,dk,mk,gk,iceb_get_pnk("00",0,data->window),ff1);

  buhhar_rs(dn,mn,gn,dk,mk,gk,data->rk->metka_ras,&cur,kolstr,&sheta,ff,ff1,data->bar,data->window);

 }

iceb_podpis(ff,data->window);
fclose(ff);
iceb_podpis(ff1,data->window);
fclose(ff1);


data->rk->imaf.plus(imaf);
data->rk->naimf.plus(gettext("Отчёт с дебетами по вертикали"));
data->rk->imaf.plus(imaf1);
data->rk->naimf.plus(gettext("Отчёт с кредитами по вертикали"));

for(int nom=0; nom < data->rk->imaf.kolih();nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),3,data->window);

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
/************************************************/
/*Расчёт по варианту*/
/*************************************************/

int buhhar_rs(short dn,short mn,short gn,
short dk,short mk,short gk,
int metka_ras,
class SQLCURSOR *cur,
int kolstr,
class iceb_u_spisok *sheta,
FILE *ff,
FILE *ff1,
GtkWidget *bar,
GtkWidget *wpredok)
{
SQL_str row;
char strsql[512];


char imaf_tmp[64];
FILE *ff_tmp;

sprintf(imaf_tmp,"hax%d.tmp",getpid());
if((ff_tmp = fopen(imaf_tmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tmp,"",errno,wpredok);
  return(1);
 }

/*********
for(int i=0; i < sheta->kolih(); i++)
 printw("%s ",sheta->ravno(i));
printw("\n");
OSTANOV();
***************/
class iceb_u_spisok sheta_k;

float kolstr1=0;
int kol_zap=0;
//Создаём список счетов корреспондентов
cur->poz_cursor(0);
while(cur->read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(bar,kolstr,++kolstr1);    

  if(metka_ras == 1)
   if(sheta->find(row[1]) >= 0)
    continue;
    
  if(sheta->find(row[0],1) < 0)
   continue;
  
  if(sheta_k.find(row[1],1) < 0)
   sheta_k.plus(row[1]);       
  
  fprintf(ff_tmp,"%s|%s|%s|\n",row[0],row[1],row[2]);
  kol_zap++;
 }

fclose(ff_tmp);


if(kol_zap == 0)
 {
  unlink(imaf_tmp);
  iceb_menu_soob(gettext("Не найдено ни одной проводки со счётом из файла настройки !"),wpredok);
  return(1);
 }

buhhah_sort(&sheta_k,wpredok); //сортируем список счетов
 
if((ff_tmp = fopen(imaf_tmp,"r")) == NULL)
 {
  iceb_er_op_fil(imaf_tmp,"",errno,wpredok);
  return(1);
 }

unlink(imaf_tmp);



class iceb_u_double suma_d; //Массив сумм
suma_d.make_class(sheta->kolih()*sheta_k.kolih());

kolstr1=0;
char stroka[1024];
//Заполняем массив сумм по счетам
class iceb_u_str shet("");
class iceb_u_str shet_k("");
double suma;
int nomer_sh;
int nomer_shk;
int kol_sh_k=sheta_k.kolih();

while(fgets(stroka,sizeof(stroka),ff_tmp) != 0)
 {
//  strzag(LINES-1,0,kol_zap,++kolstr1);
  iceb_pbar(bar,kol_zap,++kolstr1);    
  iceb_u_polen(stroka,&shet,1,'|');
  iceb_u_polen(stroka,&shet_k,2,'|');
  iceb_u_polen(stroka,strsql,sizeof(strsql),3,'|');
  suma=atof(strsql);
  nomer_sh=sheta->find(shet.ravno(),1);
  nomer_shk=sheta_k.find(shet_k.ravno(),1);
  suma_d.plus(suma,nomer_sh*kol_sh_k+nomer_shk);
 }

fclose(ff_tmp);


buh_rhw(0,sheta,&sheta_k,&suma_d,ff); //распечатываем массив
buh_rhw(1,sheta,&sheta_k,&suma_d,ff1); //распечатываем массив



return(0);
}
/********************************/
/*Сортировка массива счетов*/
/*****************************/
void buhhah_sort(class iceb_u_spisok *sheta,GtkWidget *wpredok)
{
SQL_str rowtmp;
class SQLCURSOR curtmp;
char strsql[512];
class iceb_tmptab tabtmp;
const char *imatmptab={"buhhahw"};

char zaprostmp[512];
memset(zaprostmp,'\0',sizeof(zaprostmp));

sprintf(zaprostmp,"CREATE TEMPORARY TABLE %s (\
sh char(32) not null) ENGINE = MYISAM",imatmptab);


if(tabtmp.create_tab(imatmptab,zaprostmp,wpredok) != 0)
  return;
  

for(int i=0; i < sheta->kolih(); i++)
 {

  sprintf(strsql,"insert into %s values ('%s')", imatmptab,sheta->ravno(i));

  iceb_sql_zapis(strsql,1,0,wpredok);    
  
 }
sprintf(strsql,"select * from %s order by sh asc",imatmptab);
int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

sheta->free_class();

while(curtmp.read_cursor(&rowtmp) != 0)
 {
  sheta->plus(rowtmp[0]);
 }

}
