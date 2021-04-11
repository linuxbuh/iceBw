/*$Id:$*/
/*22.03.2017	21.10.2004	Белых А.И.	dooc_r.c
Расчёт дооценки
*/
#include "buhg_g.h"
#include "dooc.h"

class dooc_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
  
  class dooc_data *rk;
  
  
 };

gboolean   dooc_r_key_press(GtkWidget *widget,GdkEventKey *event,class dooc_r_data *data);
gint dooc_r1(class dooc_r_data *data);
void  dooc_r_v_knopka(GtkWidget *widget,class dooc_r_data *data);

extern SQL_baza bd;
extern double	nds1;
extern short	vtara; /*Код группы возвратная тара*/
extern double	okrcn;
 
void dooc_r(class dooc_data *rek_ras,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class dooc_r_data data;
int gor=0;
int ver=0;

data.rk=rek_ras;
data.name_window.plus(__FUNCTION__);



data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Дооценить документ"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(dooc_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Дооценить документ"));
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


gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(dooc_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)dooc_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  dooc_r_v_knopka(GtkWidget *widget,class dooc_r_data *data)
{
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   dooc_r_key_press(GtkWidget *widget,GdkEventKey *event,class dooc_r_data *data)
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

gint dooc_r1(class dooc_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
class iceb_u_str repl;

class iceb_u_str rnd("");
short dr,mr,gr;
iceb_u_rsdat(&dr,&mr,&gr,data->rk->datad.ravno(),1);

if(iceb_pbpds(mr,gr,data->window) != 0)
 {
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

SQL_str row;
SQLCURSOR cur;
//Проверяем есть ли код операции в списке приходов и расходов
sprintf(strsql,"select prov from Prihod where kod='%s'",data->rk->kodop.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  repl.new_plus(gettext("Не найден код операции прихода"));
  repl.plus(" ");
  repl.plus(data->rk->kodop.ravno());
  repl.plus(" !");
  
  iceb_menu_soob(&repl,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int prov1=atoi(row[0]);
sprintf(strsql,"select prov from Rashod where kod='%s'",data->rk->kodop.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  repl.new_plus(gettext("Не найден код операции расхода"));
  repl.plus(" ");
  repl.plus(data->rk->kodop.ravno());
  repl.plus(" !");
  
  iceb_menu_soob(&repl,data->window);

  gtk_widget_destroy(data->window);
  return(FALSE);
 }


int prov2=atoi(row[0]);

sprintf(strsql,"select nomkar,kodm,kolih,cena from Dokummat1 \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and tipz=1 \
and nomkar != 0 and voztar=0  order by kodm asc",
data->rk->gd,data->rk->md,data->rk->dd,data->rk->skl,data->rk->nomdok_c.ravno());
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи"),data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int nomkar[kolstr];
int kodm[kolstr];
double kolih[kolstr];
double kolihok[kolstr];
double suma[kolstr];
strcpy(strsql,"LOCK TABLES Kart READ,Zkart READ,Material READ,icebuser READ");
if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка блокировки таблицы !"),strsql,data->window);


int nomstr=0;
SQL_str row1;
SQLCURSOR cur1;
double kolihdok=0.;
double sumai=0.;
double kolihost=0.;
short ddd,mmm,ggg;
iceb_u_poltekdat(&ddd,&mmm,&ggg);
class ostatok oo;

while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(data->rk->kodmat.ravno(),row[1],0,0) != 0)
    continue;    

  sprintf(strsql,"select kodgr from Material where kodm=%s",row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   if(vtara != 0 && vtara == atoi(row1[0]))
     continue;

  nomkar[nomstr]=atoi(row[0]);
  kodm[nomstr]=atoi(row[1]);
  kolihdok+=kolih[nomstr]=atof(row[2]);
  sumai+=suma[nomstr]=kolih[nomstr]*atof(row[3]);
  ostkarw(1,1,ggg,ddd,mmm,ggg,data->rk->skl,nomkar[nomstr],&oo);
  kolihost+=kolihok[nomstr]=oo.ostg[3];  
  
  nomstr++;
 }


if(kolihdok - kolihost > 0.009)
 {
  iceb_u_spisok repp;
  
  repp.plus(gettext("Остаки на карточках не равны количеству в документе !"));

  sprintf(strsql,"%s:%d.%d.%d %s:%f != %s:%f",
  gettext("Дата"),
  ddd,mmm,ggg,gettext("Документ"),
  kolihdok,gettext("Карточки"),kolihost);

  repp.plus(strsql);
  
  repp.plus(gettext("Сделать дооценку на остатки ?"));

  if(iceb_menu_danet(&repp,2,data->window) == 2)
   {
    sprintf(strsql,"UNLOCK TABLES");
    if(sql_zap(&bd,strsql) != 0)
      iceb_msql_error(&bd,gettext("Ошибка разблокировки таблицы !"),strsql,data->window);

    gtk_widget_destroy(data->window);
    return(FALSE);
   }
 }


sprintf(strsql,"LOCK TABLES Dokummat WRITE,Dokummat1 WRITE,Dokummat2 WRITE,Kart WRITE,icebuser READ,Nalog READ");
if(sql_zap(&bd,strsql) != 0)
  iceb_msql_error(&bd,gettext("Ошибка блокировки таблицы !"),strsql,data->window);

//Определяем номера документов
class iceb_u_str sklad("");
sklad.new_plus(data->rk->skl);
iceb_u_str nomd1;
iceb_u_str nomd2;
iceb_nomnak(gr,sklad.ravno(),&nomd1,1,0,0,data->window);
iceb_nomnak(gr,sklad.ravno(),&nomd2,2,0,0,data->window);

sprintf(strsql,"00-%d",data->rk->skl);
class iceb_u_str kontr(strsql);

time_t tmm;
time(&tmm);
float pnds=iceb_pnds(dr,mr,gr,data->window);

if(zap_s_mudokw(1,dr,mr,gr,data->rk->skl,kontr.ravno(),nomd1.ravno(),"",data->rk->kodop.ravno(),0,prov1,nomd2.ravno(),0,0,0,1,"00",pnds,data->window) != 0)
 return(FALSE);

sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',3,'%s %s')",gr,sklad.ravno(),nomd1.ravno(),data->rk->nomdok_c.ravno(),data->rk->osnov.ravno());
iceb_sql_zapis(strsql,1,0,data->window);

if(zap_s_mudokw(2,dr,mr,gr,data->rk->skl,kontr.ravno(),nomd2.ravno(),"",data->rk->kodop.ravno(),0,prov2,nomd1.ravno(),0,0,0,1,"00",pnds,data->window) != 0)
 return(FALSE);


sprintf(strsql,"insert into Dokummat2 \
values (%d,%s,'%s',3,'%s %s')",gr,sklad.ravno(),nomd2.ravno(),data->rk->nomdok_c.ravno(),data->rk->osnov.ravno());
iceb_sql_zapis(strsql,1,0,data->window);

sprintf(strsql,"nomd1=%s nomd2=%s\n",nomd1.ravno(),nomd2.ravno());
iceb_printw(strsql,data->buffer,data->view);

double	cena=0.;
double	kof=0.;
kof=(sumai+data->rk->sumadooc.ravno_atof())/sumai;

sprintf(strsql,"kof=%f=(%.2f+%.2f)/%.2f\n",kof,sumai,data->rk->sumadooc.ravno_atof(),sumai);
iceb_printw(strsql,data->buffer,data->view);

class iceb_u_str shetu("");
class iceb_u_str ei("");
class iceb_u_str nds("");
class iceb_u_str mnds("");
class iceb_u_str krat("");
int	nomk=0;
double cenakar=0.;
short denv=0,mesv=0,godv=0;
class iceb_u_str innom("");
class iceb_u_str nomz("");
short deng,mesg,godg;

//Формируем записи приходного документа
for(int i=0 ; i < kolstr ; i++)
 {
  sprintf(strsql,"%4d %4d %8.8g %8.8g %8.8g\n",
  nomkar[i],kodm[i],kolih[i],kolihok[i],suma[i]);
  iceb_printw(strsql,data->buffer,data->view);
  
  if(kolih[i] == 0. || suma[i] == 0.)
    continue;

  cena=(suma[i]*kof)/kolih[i];
  
  sprintf(strsql,"cena=(%f*%f)/%f=%f\n",suma[i],kof,kolih[i],cena);
  iceb_printw(strsql,data->buffer,data->view);
  
  cena=iceb_u_okrug(cena,okrcn);
  
  shetu.new_plus("");
  ei.new_plus("");
  nds.new_plus("");
  mnds.new_plus("");
  krat.new_plus("");

  deng=mesg=godg=0;
  //  Читаем реквизиты карточки
  sprintf(strsql,"select shetu,ei,nds,krat,mnds,cena,innom,rnd,nomz,dat_god from \
Kart where sklad=%d and nomk=%d",data->rk->skl,nomkar[i]);
  if(sql_readkey(&bd,strsql,&row,&cur1) != 1)
   {
    sprintf(strsql,"Не нашли карточку %d на складе %d !\n",nomkar[i],data->rk->skl);
    iceb_printw(strsql,data->buffer,data->view);
    continue;
   }

  shetu.new_plus(row[0]);
  ei.new_plus(row[1]);
  nds.new_plus(row[2]);
  krat.new_plus(row[3]);
  mnds.new_plus(row[4]);
  cenakar=atof(row[5]);
  cenakar=iceb_u_okrug(cenakar,okrcn);
  innom.new_plus(row[6]);
  rnd.new_plus(row[7]);
  nomz.new_plus(row[8]);
  iceb_u_rsdat(&deng,&mesg,&godg,row[9],2);
   
  //Проверяем может карточка с введенными уникальными реквизитами уже есть на складе
  sprintf(strsql,"select nomk from Kart where kodm=%d and sklad=%d \
  and shetu='%s' and cena=%.10g and ei='%s' and nds=%s and cenap=%.10g \
  and krat=%s and mnds=%s",
  kodm[i],data->rk->skl,shetu.ravno(),cena,ei.ravno(),nds.ravno(),0.,krat.ravno(),mnds.ravno());

  /*
  printw("\n%s\n",strsql);
  OSTANOV();
  */
  if(sql_readkey(&bd,strsql,&row,&cur1) > 0)
   {
    nomk=atoi(row[0]);
    zapvdokw(dr,mr,gr,data->rk->skl,nomk,kodm[i],nomd1.ravno(),kolih[i],cena,
    ei.ravno(),nds.ravno_atof(),mnds.ravno_atoi(),0,1,nomkar[i],"",nomz.ravno(),0,"",data->window);
   }
  else
   {
    int nomk=nomkrw(data->rk->skl,data->window);
//    printw("nds=%s shbm=%s\n",nds,shetu.ravno());
//    refresh();
         
    if(zapkarw(data->rk->skl,nomk,kodm[i],cena,ei.ravno(),shetu.ravno(),krat.ravno_atof(),0.,0,
    nds.ravno_atof(),mnds.ravno_atoi(),0.,denv,mesv,godv,innom.ravno(),rnd.ravno(),nomz.ravno(),deng,mesg,godg,0,data->window) == 0)
      zapvdokw(dr,mr,gr,data->rk->skl,nomk,kodm[i],nomd1.ravno(),kolih[i],cena,
      ei.ravno(),nds.ravno_atof(),mnds.ravno_atoi(),0,1,nomkar[i],"",nomz.ravno(),0,"",data->window);
   }

  //Формируем запись расходного документа
  zapvdokw(dr,mr,gr,data->rk->skl,nomkar[i],kodm[i],nomd2.ravno(),kolih[i],cenakar,
  ei.ravno(),nds.ravno_atof(),mnds.ravno_atoi(),0,2,nomk,"",nomz.ravno(),0,"",data->window);

 }

sprintf(strsql,"UNLOCK TABLES");
if(sql_zap(&bd,strsql) != 0)
      iceb_msql_error(&bd,gettext("Ошибка разблокировки таблицы !"),strsql,data->window);


sprintf(strsql,"%s:%d.%d.%d\n",gettext("Дата"),dr,mr,gr);
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s: %s\n",gettext("Номер расходного документа"),nomd2.ravno());
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"%s: %s\n",gettext("Номер приходного документа"),nomd1.ravno());
iceb_printw(strsql,data->buffer,data->view);


gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);
return(FALSE);
}
