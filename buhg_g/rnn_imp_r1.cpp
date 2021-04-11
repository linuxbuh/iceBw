/*$Id: rnn_imp_r1.c,v 1.36 2014/07/31 07:08:26 sasa Exp $*/
/*23.05.2016	06.05.2008	Белых А.И.	rnn_imp_r1.c
Импорт полученных налоговых накладных в Реестр налоговых накладных
*/
#include "buhg_g.h"
#include "rnn_imp.h"
#include "rnnovdw.h"

class rnn_imp_r1_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class rnn_imp_data *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;
  
  rnn_imp_r1_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   rnn_imp_r1_key_press(GtkWidget *widget,GdkEventKey *event,class rnn_imp_r1_data *data);
gint rnn_imp_r11(class rnn_imp_r1_data *data);
void  rnn_imp_r1_v_knopka(GtkWidget *widget,class rnn_imp_r1_data *data);


extern SQL_baza bd;
extern short	startgod; /*Стартовый год*/
extern double   okrg1;
extern double	okrcn;
extern int      kol_strok_na_liste;
extern int kol_strok_na_liste_l;

int rnn_imp_r1(class rnn_imp_data *datark,GtkWidget *wpredok)
{
char strsql[1024];
int gor=0;
int ver=0;
class iceb_u_str repl;
class rnn_imp_r1_data data;
data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт налоговых накладных"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(rnn_imp_r1_key_press),&data);

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

repl.plus(gettext("Импорт налоговых накладных"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(rnn_imp_r1_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)rnn_imp_r11,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  rnn_imp_r1_v_knopka(GtkWidget *widget,class rnn_imp_r1_data *data)
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

gboolean   rnn_imp_r1_key_press(GtkWidget *widget,GdkEventKey *event,class rnn_imp_r1_data *data)
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

gint rnn_imp_r11(class rnn_imp_r1_data *data)
{
time_t vremn;
time(&vremn);
class rnnovd_rek ovd;
char strsql[2048];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str viddok("");

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


/*Чтение настроек для определения вида документа*/
rnnovdw(1,&ovd,data->window);

char bros[512];
short vt=0;

memset(bros,'\0',sizeof(bros));
class iceb_u_str imaf_matnast("matnast.alx");

iceb_poldan("Код группы возвратная тара",bros,imaf_matnast.ravno(),data->window);
vt=(short)atoi(bros);
class iceb_u_str kodop_storn; /*Коды операций сторнирования*/
iceb_poldan("Коды операций сторнирования для приходов",&kodop_storn,imaf_matnast.ravno(),data->window);

/*составляем список накладных имеющих код группы*/
sprintf(strsql,"select datd,nnn,mi,gr,inn from Reenn1 where datd >= '%04d-%02d-%02d' \
and datd <= '%04d-%02d-%02d' and mi > 0 and gr <> ''",gn,mn,dn,gk,mk,dk);
int kolstr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
#if 0
############################################################
class iceb_u_str mu11_12("");
class iceb_u_str usl11_12(""); 
class iceb_u_str uos11_12("");
double procent11_12=0.;

class iceb_u_str mu13_14("");
class iceb_u_str usl13_14(""); 
class iceb_u_str uos13_14("");

class iceb_u_str mu15_16("");
class iceb_u_str usl15_16(""); 
class iceb_u_str uos15_16("");

iceb_poldan("Материальный учёт колонки 11-12",&mu11_12,"rnn_nimp1.alx",data->window);
iceb_poldan("Учёт услуг колонки 11-12",&usl11_12,"rnn_nimp1.alx",data->window);
iceb_poldan("Учёт основных средств колонки 11-12",&uos11_12,"rnn_nimp1.alx",data->window);
iceb_poldan("Процент расчёта колонок 11-12",&procent11_12,"rnn_nimp1.alx",data->window);

iceb_poldan("Материальный учёт колонки 13-14",&mu13_14,"rnn_nimp1.alx",data->window);
iceb_poldan("Учёт услуг колонки 13-14",&usl13_14,"rnn_nimp1.alx",data->window);
iceb_poldan("Учёт основных средств колонки 13-14",&uos13_14,"rnn_nimp1.alx",data->window);

iceb_poldan("Материальный учёт колонки 15-16",&mu15_16,"rnn_nimp1.alx",data->window);
iceb_poldan("Учёт основных средств колонки 15-16",&usl15_16,"rnn_nimp1.alx",data->window);
iceb_poldan("Учёт услуг колонки 15-16",&uos15_16,"rnn_nimp1.alx",data->window);
############################################################3
#endif

class iceb_u_spisok spdok;
class iceb_u_int gr_dok;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s|%s|%s",row[0],row[1],row[2],row[4]);
  spdok.plus(strsql);
  gr_dok.plus(row[3]);
 }

/*составляем список накладных имеющих вид документа*/
sprintf(strsql,"select datd,nnn,mi,vd,inn from Reenn1 where datd >= '%04d-%02d-%02d' \
and datd <= '%04d-%02d-%02d' and mi > 0 and vd <> ''",gn,mn,dn,gk,mk,dk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
class iceb_u_spisok spdok_vd;
class iceb_u_spisok sviddok;

while(cur.read_cursor(&row) != 0)
 {
  sprintf(strsql,"%s|%s|%s|%s",row[0],row[1],row[2],row[4]);
  spdok_vd.plus(strsql);
  sviddok.plus(row[3]);
 }
class iceb_u_str sql_zapros;

//Удаляем все накладные загруженные за этот период
sprintf(strsql,"delete from Reenn1 where datd >= '%04d-%02d-%02d' \
and datd <= '%04d-%02d-%02d' and mi > 0",gn,mn,dn,gk,mk,dk);

sql_zapros.plus(strsql);

if(data->rk->inn.getdlinna() > 1)
 {
  sprintf(strsql," and inn='%s'",data->rk->inn.ravno());
  sql_zapros.plus(strsql);
 }
if(data->rk->nnn.getdlinna() > 1)
 {
  sprintf(strsql," and nnn='%s'",data->rk->nnn.ravno());
  sql_zapros.plus(strsql);
 }

iceb_sql_zapis(sql_zapros.ravno(),1,0,data->window);

sprintf(strsql,"%s\n",gettext("Материальный учёт"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"select datpnn,kontr,nomnn,sklad,nomd,datd,pn,kodop from Dokummat \
where datpnn >= '%d-%d-%d' and datpnn <= '%d-%d-%d' and tip=1 and \
nomnn <> '%s' order by datpnn asc",gn,mn,dn,gk,mk,dk,"\0");
//printw("%s\n",strsql);
//refresh();
SQLCURSOR cur1;
SQLCURSOR cur2;
SQL_str row1;
SQL_str row2;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s\n",gettext("В \"Материальном учете\" не найдено ни одного документа"));
  iceb_printw(strsql,data->buffer,data->view);
 }

float kolstr1=0;
class iceb_u_str metka(ICEB_MP_MATU);
short d,m,g;
int mnds=0;
double nds;
double suma,sumabn,sumkor;
double sum;
double summ[4];
double ndsm[4];

class iceb_u_str inn("");
class iceb_u_str nkontr("");
float pnds=0.;
short dv,mv,gv;
int storn=1;
class iceb_u_str kod_gr("");
int nomer_dok=0;
double kolih=0.;
//int nomer_kol=0;
while(cur.read_cursor(&row) != 0)
 {

  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->nnn.ravno(),row[2],0,0) != 0)
   continue;
  
  iceb_u_rsdat(&d,&m,&g,row[5],2);
  pnds=atof(row[6]);
  //Узнаём индивидуальный налоговый номер и наименование контрагента
  nkontr.new_plus("");
  sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[1]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(iceb_u_proverka(data->rk->inn.ravno(),row1[1],0,0) != 0)
     continue;
    if(row1[2][0] == '\0')
      nkontr.new_plus(row1[0]);
    else
      nkontr.new_plus(row1[2]);
    inn.new_plus(row1[1]);
   }
  else
    inn.new_plus("");

  sprintf(strsql,"%s %-6s %-6s %s %s %s\n",metka.ravno(),row[0],row[1],row[2],row[3],row[4]);
  iceb_printw(strsql,data->buffer,data->view);
//  printw("%s %s\n",inn,nkontr);
  
  /*Узнаем НДС документа*/
  mnds=0;
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=11",g,row[4],row[3]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
    mnds=atoi(row1[0]);

  if(mnds == 4)
   pnds=7.;
   
  nds=0.;

  if(mnds == 0)
   {
    /*Узнаем сумму НДС документа*/
    sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=6",g,row[4],row[3]);

    if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
      nds=atof(row1[0]);
   }
  /*Узнаём условие продажи - нужен виддокумента*/
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=7",g,row[4],row[3]);
  viddok.new_plus("");
  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(iceb_u_pole2(row1[0],':') == 0)
     {
        viddok.new_plus(row1[0]);
     }
    else
     iceb_u_polen(row1[0],&viddok,4,':');
   }
  
  dv=mv=gv=0;  
  /*Узнаём дату выписки документа*/
  sprintf(strsql,"select sodz from Dokummat2 where god=%d and nomd='%s' \
and sklad=%s and nomerz=14",g,row[4],row[3]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   iceb_u_rsdat(&dv,&mv,&gv,row1[0],1);
       
  sumzap1w(d,m,g,row[4],atoi(row[3]),&suma,&sumabn,mnds,vt,&sumkor,1,&kolih,data->window);
  storn=1;
  if(iceb_u_proverka(kodop_storn.ravno(),row[7],0,1) == 0)
   storn=-1;
  
  if(mnds == 0 && nds == 0.)
    nds=(suma+sumkor)*pnds/100.;
  
  sum=(suma+sumkor+nds+sumabn)*storn;
  if(nds != 0)
   nds*=storn;
   
  memset(summ,'\0',sizeof(summ));
  memset(ndsm,'\0',sizeof(ndsm));

  if(mnds == 0) /*основная ставка*/
   {
    summ[0]=sum;
    ndsm[0]=nds;
   }
  if(mnds == 1 || mnds == 2) /*0% на територии и вне Украины*/
   {
    summ[2]=sum;
    ndsm[2]=nds;
   }
  if(mnds == 3) /*освобождение от налогообложения*/
   {
    summ[3]=sum;
    ndsm[3]=nds;
   }      

  if(mnds == 4) /* 7% на мед препараты */
   {
    summ[1]=sum;
    ndsm[2]=nds;

   }
#if 0
#########################################################3  
  if(mnds < 3)
   {
    nomer_kol=0;
    if(iceb_u_proverka(mu11_12.ravno(),row[7],0,1) == 0)
     nomer_kol=1;
    if(iceb_u_proverka(mu13_14.ravno(),row[7],0,1) == 0)
     nomer_kol=2;
    if(iceb_u_proverka(mu15_16.ravno(),row[7],0,1) == 0)
     nomer_kol=3;
     
//    summ[nomer_kol]=sum;
//    ndsm[nomer_kol]=nds;
    if(nomer_kol != 1)
     {
      summ[nomer_kol]=sum;
      ndsm[nomer_kol]=nds;
     }
    else
     {
      double sum12=sum*procent11_12/100.;
      sum12=iceb_u_okrug(sum12,0.01);
      double sum11=sum-sum12;
      double nds12=nds*procent11_12/100.;
      nds12=iceb_u_okrug(nds12,0.01);
      double nds11=nds-nds12;
      
      summ[0]=sum11;
      ndsm[0]=nds11;
      summ[1]=sum12;
      ndsm[1]=nds12;
     }
   }
  else
   {
    if(iceb_u_SRAV("400000000000",inn.ravno(),0) == 0)/*�� ���� ��������� ���������*/
     {
      summ[1]=sum;
      ndsm[2]=0.;
     }    
    else
     {
      summ[0]=sum;
      ndsm[0]=nds;
     }
   }
########################################
#endif
  /*Ищем код группы*/
  sprintf(strsql,"%s|%s|1|%s",row[0],row[2],inn.ravno());
  if((nomer_dok=spdok.find(strsql)) >= 0)
     kod_gr.new_plus(gr_dok.ravno(nomer_dok));
  else
     kod_gr.new_plus("");
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(nds != 0. && inn.ravno()[0] == '\0')
   {
     class iceb_u_str repl;
     repl.plus(gettext("Материальный учёт"));
     sprintf(strsql,"%s:%d.%d.%d",gettext("Дата получения налоговой накладной"),d,m,g);
     repl.ps_plus(strsql);
     
     sprintf(strsql,"%s:%s",gettext("Номер налоговой накладной"),row[2]);
     repl.ps_plus(strsql);

     sprintf(strsql,"%s:%s/%s",gettext("Контрагент"),row[1],nkontr.ravno());
     repl.ps_plus(strsql);

     repl.ps_plus(gettext("Не ввели индивидуальный налоговый номер!"));
     iceb_menu_soob(&repl,data->window);
   }  

  /*определение вида документа*/
  if(viddok.getdlinna() <= 1)
   {
    rnn_imp_ovd(&viddok,row[7],&ovd.muvd,&ovd.muso);
    /*Смотрим был ли введён вид документа*/
    if(viddok.getdlinna() <= 1)
     {
      sprintf(strsql,"%s|%s|1|%s",row[0],row[2],inn.ravno());
      if((nomer_dok=spdok_vd.find(strsql)) >= 0)
       viddok.new_plus(sviddok.ravno(nomer_dok));
     }  
   }
  zapvree1w(d,m,g,row[2],summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
  inn.ravno(),nkontr.ravno(),1,row[5],row[4],"",kod_gr.ravno(),row[7],dv,mv,gv,viddok.ravno(),data->window);


 }

sprintf(strsql,"%s\n",gettext("Учёт услуг"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"select datpnn,kontr,nomnn,podr,nomd,sumkor,\
nds,datd,sumnds,pn,kodop,datdp,osnov,uslpr from Usldokum where datpnn >= '%d-%d-%d' and \
datpnn <= '%d-%d-%d' and tp=1 and nomnn <> '%s' order by datpnn asc",
gn,mn,dn,gk,mk,dk,"\0");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  sprintf(strsql,"%s\n",gettext("В \"Учете услуг\" не найдено ни одного документа"));
  iceb_printw(strsql,data->buffer,data->view);
 }
kolstr1=0;
metka.new_plus(ICEB_MP_USLUGI);
while(cur.read_cursor(&row) != 0)
 {
  dv=mv=gv=0;
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->nnn.ravno(),row[2],0,0) != 0)
   continue;
   
  pnds=atof(row[9]);
  //Узнаём индивидуальный налоговый номер и наименование контрагента
  nkontr.new_plus("");

  sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[1]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(iceb_u_proverka(data->rk->inn.ravno(),row1[1],0,0) != 0)
     continue;
    if(row1[2][0] == '\0')
      nkontr.new_plus(row1[0]);
    else
      nkontr.new_plus(row1[2]);
    inn.new_plus(row1[1]);
   }
  else
    inn.new_plus("");
  sprintf(strsql,"%s %s %-6s %-6s %s %s\n",metka.ravno(),row[0],row[1],row[2],row[3],row[4]);
  iceb_printw(strsql,data->buffer,data->view);


  sumkor=atof(row[5]);
  mnds=atoi(row[6]);
  nds=0.;
  if(mnds == 0)
   nds=atof(row[8]);
   
  iceb_u_rsdat(&d,&m,&g,row[7],2);
  sumzap1uw(d,m,g,row[4],atoi(row[3]),&suma,&sumabn,mnds,1,data->window);

  sum=suma+sumkor+nds+sumabn;
//  printw("sum=%f\n",sum);
//  OSTANOV();
  if(mnds == 0)
   {
    if(nds == 0.)
     {
      nds=(suma+sumkor)*pnds/100.;
      sum=suma+sumkor+nds+sumabn;
     }     
    else
     {
      //Пересчитываем обратным счетом базу налогообложения
      sum=nds*((100.+pnds)/pnds);
     }
   }

  memset(summ,'\0',sizeof(summ));
  memset(ndsm,'\0',sizeof(ndsm));

  if(mnds == 0) /*основная ставка*/
   {
    summ[0]=sum;
    ndsm[0]=nds;
   }
  if(mnds == 1 || mnds == 2) /*0% на територии и вне Украины*/
   {
    summ[2]=sum;
    ndsm[2]=nds;
   }
  if(mnds == 3) /*освобождение от налогообложения*/
   {
    summ[3]=sum;
    ndsm[3]=nds;
   }      

  /*mnds == 4 7% на мед препараты в услугах не может быть*/
#if 0
###################################################################3
  if(mnds < 3)
   {
    nomer_kol=0;
    if(iceb_u_proverka(usl11_12.ravno(),row[10],0,1) == 0)
     nomer_kol=1;
    if(iceb_u_proverka(usl13_14.ravno(),row[10],0,1) == 0)
     nomer_kol=2;
    if(iceb_u_proverka(usl15_16.ravno(),row[10],0,1) == 0)
     nomer_kol=3;

//    summ[nomer_kol]=sum;
//    ndsm[nomer_kol]=nds;
    
    if(nomer_kol != 1)
     {
      summ[nomer_kol]=sum;
      ndsm[nomer_kol]=nds;
     }
    else
     {
      double sum12=sum*procent11_12/100.;
      sum12=iceb_u_okrug(sum12,0.01);
      double sum11=sum-sum12;
      double nds12=nds*procent11_12/100.;
      nds12=iceb_u_okrug(nds12,0.01);
      double nds11=nds-nds12;
      
      summ[0]=sum11;
      ndsm[0]=nds11;
      summ[1]=sum12;
      ndsm[1]=nds12;
     }

   }
  else
   {
    if(iceb_u_SRAV("400000000000",inn.ravno(),0) == 0)/*�� ���� ��������� ���������*/
     {
      summ[0]=sum;
      ndsm[0]=0.;
     }    
    else
     {
      summ[1]=sum;
      ndsm[1]=nds;
     }
   }
#####################################3
#endif
  /*Ищем код группы*/
  sprintf(strsql,"%s|%s|2|%s",row[0],row[2],inn.ravno());
  if((nomer_dok=spdok.find(strsql)) >= 0)
    kod_gr.new_plus(gr_dok.ravno(nomer_dok));
  else
    kod_gr.new_plus("");
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(nds != 0. && inn.ravno()[0] == '\0')
   {
     class iceb_u_str repl;
     repl.plus(gettext("Учёт услуг"));
     sprintf(strsql,"%s:%d.%d.%d",gettext("Дата получения налоговой накладной"),d,m,g);
     repl.ps_plus(strsql);
     
     sprintf(strsql,"%s:%s",gettext("Номер налоговой накладной"),row[2]);
     repl.ps_plus(strsql);

     sprintf(strsql,"%s:%s/%s",gettext("Контрагент"),row[1],nkontr.ravno());
     repl.ps_plus(strsql);

     repl.ps_plus(gettext("Не ввели индивидуальный налоговый номер!"));
     iceb_menu_soob(&repl,data->window);
   }  

  iceb_u_rsdat(&dv,&mv,&gv,row[11],2);
  /*определение вида документа*/
  viddok.new_plus("");
  if(iceb_u_pole2(row[13],':') == 0 && row[13][0] != '\0')
   {
      viddok.new_plus(row[13]);
   }
  else
   iceb_u_polen(row[13],&viddok,4,':');
  if(viddok.getdlinna() <= 1)
   {
    rnn_imp_ovd(&viddok,row[10],&ovd.uuvd,&ovd.uuso);
    if(viddok.getdlinna() <= 1)
     {
      sprintf(strsql,"%s|%s|2|%s",row[0],row[2],inn.ravno());
      if((nomer_dok=spdok_vd.find(strsql)) >= 0)
       viddok.new_plus(sviddok.ravno(nomer_dok));
     }  
   }
  zapvree1w(d,m,g,row[2],summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
  inn.ravno(),nkontr.ravno(),2,row[7],row[4],row[12],kod_gr.ravno(),row[10],dv,mv,gv,viddok.ravno(),data->window);

 }

sprintf(strsql,"%s\n",gettext("Учёт основных средств"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"select datpnn,kontr,podr,nomnn,nomd,\
nds,datd,pn,kodop,dvnn from Uosdok where datpnn >= '%04d-%d-%d' and \
datpnn <= '%04d-%d-%d' and tipz=1 and nomnn <> '%s' order by datpnn asc",
gn,mn,dn,gk,mk,dk,"\0");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  sprintf(strsql,"%s\n",gettext("В \"Учете основных средств\" не найдено ни одного документа"));
  iceb_printw(strsql,data->buffer,data->view);
 }
kolstr1=0;
metka.new_plus(ICEB_MP_UOS);
while(cur.read_cursor(&row) != 0)
 {
  dv=mv=gv=0;
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->nnn.ravno(),row[3],0,0) != 0)
   continue;
   
  pnds=atof(row[7]);
  //Узнаём индивидуальный налоговый номер и наименование контрагента
  nkontr.new_plus("");
  sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[1]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,data->window) == 1)
   {
    if(iceb_u_proverka(data->rk->inn.ravno(),row1[1],0,0) != 0)
     continue;
    if(row1[2][0] == '\0')
      nkontr.new_plus(row1[0]);
    else
      nkontr.new_plus(row1[2]);
    inn.new_plus(row1[1]);
   }
  else
    inn.new_plus("");
  sprintf(strsql,"%s %s %-6s %-6s %s %s\n",metka.ravno(),row[0],row[1],row[2],row[3],row[4]);
  iceb_printw(strsql,data->buffer,data->view);

  mnds=atoi(row[5]);
  iceb_u_rsdat(&d,&m,&g,row[6],2);
  sumzapuosw(d,m,g,row[4],&suma,&sumabn,mnds,1,data->window);
  nds=0.;
  if(mnds == 0)  
    nds=suma*pnds/100.;
  sum=suma+nds+sumabn;

  iceb_u_rsdat(&d,&m,&g,row[0],2);

  iceb_u_rsdat(&dv,&mv,&gv,row[9],2);
  
  memset(summ,'\0',sizeof(summ));
  memset(ndsm,'\0',sizeof(ndsm));

  if(mnds == 0) /*основная ставка*/
   {
    summ[0]=sum;
    ndsm[0]=nds;
   }
  if(mnds == 1 || mnds == 2) /*0% на територии и вне Украины*/
   {
    summ[2]=sum;
    ndsm[2]=nds;
   }
  if(mnds == 3) /*освобождение от налогообложения*/
   {
    summ[3]=sum;
    ndsm[3]=nds;
   }      

  /*mnds == 4 7% на мед препараты не может быть*/
#if 0
###############################################################3
  if(mnds < 3)
   {
    nomer_kol=0;
    if(iceb_u_proverka(uos11_12.ravno(),row[8],0,1) == 0)
     nomer_kol=1;
    if(iceb_u_proverka(uos13_14.ravno(),row[8],0,1) == 0)
     nomer_kol=2;
    if(iceb_u_proverka(uos15_16.ravno(),row[8],0,1) == 0)
     nomer_kol=3;
//    summ[nomer_kol]=sum;
//    ndsm[nomer_kol]=nds;
    if(nomer_kol != 1)
     {
      summ[nomer_kol]=sum;
      ndsm[nomer_kol]=nds;
     }
    else
     {
      double sum12=sum*procent11_12/100.;
      sum12=iceb_u_okrug(sum12,0.01);
      double sum11=sum-sum12;
      double nds12=nds*procent11_12/100.;
      nds12=iceb_u_okrug(nds12,0.01);
      double nds11=nds-nds12;
      
      summ[0]=sum11;
      ndsm[0]=nds11;
      summ[1]=sum12;
      ndsm[1]=nds12;
     }
   }
  else
   {
    if(iceb_u_SRAV("400000000000",inn.ravno(),0) == 0)/*�� ���� ��������� ���������*/
     {
      summ[0]=sum;
      ndsm[0]=0.;
     }    
    else
     {
      summ[1]=sum;
      ndsm[1]=nds;
     }
   }
#################################################
#endif
  /*Ищем код группы*/
  sprintf(strsql,"%s|%s|3|%s",row[0],row[3],inn.ravno());
  if((nomer_dok=spdok.find(strsql)) >= 0)
    kod_gr.new_plus(gr_dok.ravno(nomer_dok));
  else
    kod_gr.new_plus("");  
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(nds != 0. && inn.ravno()[0] == '\0')
   {
     class iceb_u_str repl;
     repl.plus(gettext("Учёт основных средств"));
     sprintf(strsql,"%s:%d.%d.%d",gettext("Дата получения налоговой накладной"),d,m,g);
     repl.ps_plus(strsql);
     
     sprintf(strsql,"%s:%s",gettext("Номер налоговой накладной"),row[3]);
     repl.ps_plus(strsql);

     sprintf(strsql,"%s:%s/%s",gettext("Контрагент"),row[1],nkontr.ravno());
     repl.ps_plus(strsql);

     repl.ps_plus(gettext("Не ввели индивидуальный налоговый номер!"));
     iceb_menu_soob(&repl,data->window);
   }  

  /*определение вида документа*/
  rnn_imp_ovd(&viddok,row[8],&ovd.uosvd,&ovd.uosso);
  if(viddok.getdlinna() <= 1)
   {
    sprintf(strsql,"%s|%s|3|%s",row[0],row[3],inn.ravno());
    if((nomer_dok=spdok_vd.find(strsql)) >= 0)
     viddok.new_plus(sviddok.ravno(nomer_dok));
   }
  zapvree1w(d,m,g,row[3],summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
  inn.ravno(),nkontr.ravno(),3,row[6],row[4],"",kod_gr.ravno(),row[8],dv,mv,gv,viddok.ravno(),data->window);
 }

sprintf(strsql,"%s\n",gettext("Учёт командировочных расходов"));
iceb_printw(strsql,data->buffer,data->view);

sprintf(strsql,"select nomd,datd,datao,vkom from Ukrdok where datao >= '%04d-%d-%d' and \
datao <= '%04d-%d-%d' order by datao asc",gn,mn,dn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

if(kolstr == 0)
 {
  sprintf(strsql,"%s\n",gettext("В \"Учете командировочных расходов\" не найдено ни одного документа"));
  iceb_printw(strsql,data->buffer,data->view);
 }
kolstr1=0;
int kolstr2=0;
double strah_sbor=0.;
metka.new_plus(ICEB_MP_UKR);
while(cur.read_cursor(&row) != 0)
 {
//  sprintf(strsql,"%s %-6s %-6s %s\n",metka,row[0],row[1],row[2]);
  iceb_pbar(data->bar,kolstr,++kolstr1);    
//  strzag(lines-1,0,kolstr,++kolstr1);

  sprintf(strsql,"select cena,snds,kdrnn,ndrnn,dvnn,kodr,ss from Ukrdok1 \
where datd='%s' and nomd='%s' and kdrnn <> ''",row[1],row[0]);
  
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }  

  while(cur1.read_cursor(&row1) != 0)
   {
    dv=mv=gv=0;


    if(iceb_u_proverka(data->rk->nnn.ravno(),row1[3],0,0) != 0)
     continue;

    //Узнаём индивидуальный налоговый номер и наименование контрагента
    nkontr.new_plus("");
    sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row1[2]);
    
    if(iceb_sql_readkey(strsql,&row2,&cur2,data->window) == 1)
     {
      if(iceb_u_proverka(data->rk->inn.ravno(),row2[1],0,0) != 0)
       continue;
      if(row2[2][0] == '\0')
        nkontr.new_plus(row2[0]);
      else
        nkontr.new_plus(row2[2]);
      inn.new_plus(row2[1]);
     }
    else
      inn.new_plus("");
    sprintf(strsql,"%s %-6s %-6s %-6s %-6s\n",metka.ravno(),row1[0],row1[1],row1[2],row1[3]);
    iceb_printw(strsql,data->buffer,data->view);

    nds=atof(row1[1]);
    sum=atof(row1[0])+nds;
    strah_sbor=atof(row1[6]);
    memset(summ,'\0',sizeof(summ));
    memset(ndsm,'\0',sizeof(ndsm));
    if(nds != 0.)
     {
      summ[0]=sum;
      ndsm[0]=nds;
     }
    else
     {
      if(iceb_u_SRAV("400000000000",inn.ravno(),0) == 0)/*�� ���� ��������� ���������*/
       {
        summ[0]=sum;
        ndsm[0]=0.;
       }    
      else
        summ[1]=sum;
     }
    /*Ищем код группы*/
    sprintf(strsql,"%s|%s|4|%s",row[1],row[0],inn.ravno());
    if((nomer_dok=spdok.find(strsql)) >= 0)
       kod_gr.new_plus(gr_dok.ravno(nomer_dok));
    else
       kod_gr.new_plus(""); 
    iceb_u_rsdat(&d,&m,&g,row[2],2);
    iceb_u_rsdat(&dv,&mv,&gv,row1[4],2);

    if(nds != 0. && inn.ravno()[0] == '\0')
     {
       class iceb_u_str repl;
       repl.plus(gettext("Учёт командировочных расходов"));
       sprintf(strsql,"%s:%d.%d.%d",gettext("Дата получения налоговой накладной"),d,m,g);
       repl.ps_plus(strsql);
       
       sprintf(strsql,"%s:%s",gettext("Номер налоговой накладной"),row1[3]);
       repl.ps_plus(strsql);

       sprintf(strsql,"%s:%s/%s",gettext("Контрагент"),row1[2],nkontr.ravno());
       repl.ps_plus(strsql);

       repl.ps_plus(gettext("Не ввели индивидуальный налоговый номер!"));
       iceb_menu_soob(&repl,data->window);
     }  
    /*определение вида документа*/
    rnn_imp_ovd(&viddok,row1[5],&ovd.ukrvd,&ovd.ukrsr);
    if(viddok.getdlinna() <= 1)
     {
      sprintf(strsql,"%s|%s|4|%s",row[1],row[0],inn.ravno());
      if((nomer_dok=spdok_vd.find(strsql)) >= 0)
       viddok.new_plus(sviddok.ravno(nomer_dok));
     }    
    zapvree1w(d,m,g,row1[3],summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
    inn.ravno(),nkontr.ravno(),4,row[1],row[0],"",kod_gr.ravno(),row[3],dv,mv,gv,viddok.ravno(),data->window);
    if(strah_sbor != 0.)
     {
      memset(summ,'\0',sizeof(summ));
      memset(ndsm,'\0',sizeof(ndsm));
      summ[1]=strah_sbor;      
      zapvree1w(d,m,g,row1[3],summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
      inn.ravno(),nkontr.ravno(),4,row[1],row[0],"",kod_gr.ravno(),row[3],dv,mv,gv,viddok.ravno(),data->window);
     }
   }
 }

sprintf(strsql,"%s\n",gettext("Главная книга"));
iceb_printw(strsql,data->buffer,data->view);

class iceb_u_str imaf_nast("rnn_nast.alx");
class iceb_u_str shet_nds("");

if(iceb_poldan("Счёт расчётов по НДС",&shet_nds,imaf_nast.ravno(),data->window) == 0)
 {
  if(shet_nds.getdlinna() > 1)
   {

    sprintf(strsql,"select datp,sh,shk,kodkon,deb,komen from Prov where datp >= '%04d-%d-%d' and \
    datp <= '%04d-%d-%d' and sh='%s' and deb <> 0. and komen like 'нн%%' order by datp asc",gn,mn,dn,gk,mk,dk,shet_nds.ravno());

    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);

    if(kolstr == 0)
     {  
      sprintf(strsql,"%s\n",gettext("В \"Главной книге \" не найдено нужных проводок"));
      iceb_printw(strsql,data->buffer,data->view);
     }
    kolstr1=0;
    metka.new_plus(gettext("ГК"));
    char nomer_nn[64];
    class iceb_u_str koment;
    while(cur.read_cursor(&row) != 0)
     {
    //  printw("%s %-6s %-6s %s\n",metka,row[0],row[1],row[2]);
    //  strzag(lines-1,0,kolstr,++kolstr1);
      iceb_pbar(data->bar,kolstr,++kolstr1);    
      dv=mv=gv=0;
      //Узнаём индивидуальный налоговый номер и наименование контрагента
      nkontr.new_plus("");
      sprintf(strsql,"select naikon,innn,pnaim from Kontragent where kodkon='%s'",row[3]);
      
      if(iceb_sql_readkey(strsql,&row2,&cur2,data->window) == 1)
       {
        if(iceb_u_proverka(data->rk->inn.ravno(),row2[1],0,0) != 0)
         continue;
        if(row2[2][0] == '\0')
          nkontr.new_plus(row2[0]);
        else
          nkontr.new_plus(row2[2]);
        inn.new_plus(row2[1]);
       }
      else
        inn.new_plus("");      
      iceb_u_rsdat(&d,&m,&g,row[0],2);
      nds=atof(row[4]);
      
      /*Номер налоговой накладной*/
      if(iceb_u_polen(row[5],strsql,sizeof(strsql),1,' ') != 0)
       {
        sprintf(strsql,"Не правильно введён комментарий!\n");
        iceb_printw(strsql,data->buffer,data->view);
        continue;
       }
       
      strncpy(nomer_nn,iceb_u_adrsimv(2,strsql),sizeof(nomer_nn)-1);

      if(iceb_u_proverka(data->rk->nnn.ravno(),nomer_nn,0,0) != 0)
       continue;      

      sprintf(strsql,"%s %s %-6s %-6s %-6s %10s %s\n",metka.ravno(),iceb_u_datzap(row[0]),row[1],row[2],row[3],row[4],row[5]);
      iceb_printw(strsql,data->buffer,data->view);

      /*Дата выписки налоговой накладной*/
      if(iceb_u_polen(row[5],strsql,sizeof(strsql),2,' ') != 0)
       {
        sprintf(strsql,"В комментарии не найдена дата выписки налоговой накладной !\n");
        iceb_printw(strsql,data->buffer,data->view);
        continue;
       }
      if(iceb_u_rsdat(&dv,&mv,&gv,strsql,1) != 0)
       {
        
        iceb_printw("В комментарии не правильно введена дата выписки налоговой накладной!\n",data->buffer,data->view);
        iceb_printw(strsql,data->buffer,data->view);
        continue;
       }     
      if(iceb_u_polen(row[5],&viddok,3,' ') != 0)
       viddok.new_plus("ПН");
      memset(summ,'\0',sizeof(summ));
      memset(ndsm,'\0',sizeof(ndsm));

      sum=nds*100./20.+nds;
      summ[0]=iceb_u_okrug(sum,0.01);
      ndsm[0]=nds;
      
      koment.new_plus(row[1]);
      koment.plus(" ");
      koment.plus(row[2]);
      koment.plus(" ");
      koment.plus(row[5]);

      /*Ищем код группы*/
      sprintf(strsql,"%s|%s|5|%s",row[0],nomer_nn,inn.ravno());
      if((nomer_dok=spdok.find(strsql)) >= 0)
        kod_gr.new_plus(gr_dok.ravno(nomer_dok));
      else
        kod_gr.new_plus("");           
      zapvree1w(d,m,g,nomer_nn,summ[0],ndsm[0],summ[1],ndsm[1],summ[2],ndsm[2],summ[3],ndsm[3],
      inn.ravno(),nkontr.ravno(),5,"","",koment.ravno(),kod_gr.ravno(),"",dv,mv,gv,viddok.ravno(),data->window);

     }
   }
 }

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
