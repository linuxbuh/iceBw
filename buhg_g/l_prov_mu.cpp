/*$Id: l_prov_mu.c,v 1.47 2013/12/31 11:49:14 sasa Exp $*/
/*24.05.2016	18.08.2004	Белых А.И.	l_prov_mu.c
Работа с проводками для докумета в материальном учёте
*/
#include <errno.h>
#include        "buhg_g.h"
enum
{
 FK2,
 SFK2,
 FK3,
 SFK3,
 FK4,
 SFK4,
 FK5,
 SFK5,
 FK6,
 FK9,
 FK10,
 KOL_F_KL
};

enum
{
 COL_DATA,
 COL_SHET,
 COL_SHET_KOR,
 COL_KONTR,
 COL_DEBET,
 COL_KREDIT,
 COL_KOMENT,
 COL_DATA_VREM,
 COL_KTO,  
 COL_KEKV,
//неотображаемые данные
 COL_VREM,
 COL_KTOZ,
 COL_VAL,
 NUM_COLUMNS
};

class prov_mu_data
 {
  public:
  GtkWidget *label_kolstr;
  GtkWidget *label_sheta;
  GtkWidget *sw;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *radiobutton[3];

  GtkWidget *view;
  GtkTextBuffer *buffer;

  class iceb_u_str name_window;
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать
  int       kolzap;     //Количество записей
  int       voz;

  iceb_u_str zapros;
  class iceb_u_str metkasys;
  int sklad;
  iceb_u_str nomdok;
  short dd,md,gd;
  int tipz;
  int sklad1;
  int lnds;
  iceb_u_str nomdokp;
  double sumkor;
            
  iceb_u_str kontr;
  iceb_u_str kodop;
  
  iceb_u_str naim_kodop;
  iceb_u_str naim_kontr;
  iceb_u_str naim_sklad;      
  int metka_r;
  char imaf_prot[64];  
  //Выбранная проводка
  iceb_u_str datap_v;
  iceb_u_str shet_v;
  iceb_u_str shet_kor_v;
  iceb_u_str kontr_v;
  double     debet_v;
  double     kredit_v;
  iceb_u_str koment_v;
  time_t     vrem_v;
  int        ktoz_v;
  int        val_v;
  double     suma_deb;
  double     suma_kre;  
  int        kekv;
  short metka_avp; /*0-проводки автоматически не выполнялись 1-выполнялись*/  
  short metka_nsnp; /*метка наличия старой настройки проводок 0-нет 1-есть*/
  float pnds; //Процент НДС на момент создания документа
  prov_mu_data()
   {
    kekv=0;
    metka_r=0;
    treeview=NULL;
    snanomer=0;
    kolzap=0;
    voz=0;
    kl_shift=0;
    naim_kodop.new_plus("");
    naim_kontr.new_plus("");
    naim_sklad.new_plus("");
    metka_avp=0;
    metka_nsnp=0;
    metkasys.plus(ICEB_MP_MATU);
   }

 };
gboolean   prov_mu_key_press(GtkWidget *widget,GdkEventKey *event,class prov_mu_data *data);
void prov_mu_vibor(GtkTreeSelection *selection,class prov_mu_data *data);
void prov_mu_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class prov_mu_data *data);
void  prov_mu_knopka(GtkWidget *widget,class prov_mu_data *data);
void prov_mu_add_columns (GtkTreeView *treeview);
void prov_mu_create_list (class prov_mu_data *data);

void       prov_mu_radio0(GtkWidget *,class prov_mu_data *);

void prov_mu_sapka_menu(class prov_mu_data *data);
int prov_mu_prpvblok(class prov_mu_data *data);
void avtpromuw(int tipz,short dd,short md,short gd,const char *kop,const char *nn,int skk,const char *kor,double sumkor,const char *nn1,float,FILE *ff_prot,GtkWidget *wpredok);

extern SQL_baza  bd;

void l_prov_mu(short dd,short md,short gd,
int sklad,
int sklad1,
int tipz,
const char *nomdok,
const char *nomdokp,
const char *kontr,
const char *kodop,
int lnds,
double sumkor,
float pnds,
GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class prov_mu_data data;
char strsql[512];
SQL_str row;
SQLCURSOR cur;
class iceb_u_str shet_suma;

sprintf(strsql,"select ns from Alx where fil='avtpro.alx' limit 1");
if(iceb_sql_readkey(strsql,wpredok) > 0)
   data.metka_nsnp=1;

prosprw(1,sklad,dd,md,gd,nomdok,tipz,lnds,sklad1,nomdokp,kodop,NULL,wpredok);
prosprw(2,sklad,dd,md,gd,nomdok,tipz,lnds,sklad1,nomdokp,kodop,&shet_suma,wpredok);

sprintf(data.imaf_prot,"mupr%d.tmp",getpid());

data.name_window.plus(__FUNCTION__);
data.tipz=tipz;
data.dd=dd;
data.md=md;
data.gd=gd;
data.nomdok.plus(nomdok);
data.sklad=sklad;
data.kontr.plus(kontr);
data.kodop.plus(kodop);
data.sklad1=sklad1;
data.lnds=lnds;
data.nomdokp.plus(nomdokp);
data.sumkor=sumkor;
data.pnds=pnds;
//Читаем наименование склада
sprintf(strsql,"select naik from Sklad where kod=%d",sklad);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.naim_sklad.new_plus(row[0]);

//Читаем наименование операции
if(tipz == 1)
  sprintf(strsql,"select naik from Prihod where kod='%s'",kodop);
if(tipz == 2)
  sprintf(strsql,"select naik from Rashod where kod='%s'",kodop);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.naim_kodop.new_plus(row[0]);

//Читаем наименование контрагента
sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",kontr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 data.naim_kontr.new_plus(row[0]);


data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

sprintf(strsql,"%s %s",iceb_get_namesystem(),
gettext("Работа с проводками (материальный учёт)"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(prov_mu_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


data.label_sheta=gtk_label_new(NULL);
iceb_label_set_style_text(data.label_sheta,"font-family=\"monospace\" color=\"red\"",shet_suma.ravno());

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);
gtk_box_pack_start (GTK_BOX (hbox),data.label_sheta,FALSE, FALSE, 0);


gtk_widget_show_all(hbox);

data.label_kolstr=gtk_label_new ("");


prov_mu_sapka_menu(&data);



gtk_box_pack_start (GTK_BOX (vbox2),data.label_kolstr,FALSE, FALSE, 0);

gtk_widget_show(vbox1);
gtk_widget_show(vbox2);


data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE, 0);

GtkWidget *hboxradio = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous(GTK_BOX(hboxradio),TRUE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

//Вставляем радиокнопки

data.radiobutton[0]=gtk_radio_button_new_with_label_from_widget(NULL,gettext("Дебет"));
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[0], TRUE, TRUE, 0);

g_signal_connect(data.radiobutton[0], "clicked",G_CALLBACK(prov_mu_radio0),&data);

data.radiobutton[1]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton[0]),gettext("Кредит"));
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[1], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton[1], "clicked",G_CALLBACK(prov_mu_radio0),&data);

data.radiobutton[2]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(data.radiobutton[1]),gettext("Все"));
gtk_box_pack_start (GTK_BOX (hboxradio),data.radiobutton[2], TRUE, TRUE, 0);
g_signal_connect(data.radiobutton[2], "clicked",G_CALLBACK(prov_mu_radio0),&data);

gtk_box_pack_start(GTK_BOX (vbox2), hboxradio, FALSE,FALSE, 0);
gtk_widget_show_all(hboxradio);

//Кнопки

sprintf(strsql,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(prov_mu_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой проводки"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(strsql,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(prov_mu_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);

sprintf(strsql,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(prov_mu_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удалить выбранную запись"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(strsql,"%sF3 %s",RFK,gettext("Удалить"));
data.knopka[SFK3]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[SFK3], "clicked",G_CALLBACK(prov_mu_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK3],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[SFK3],gettext("Удалить все проводки"));
gtk_widget_set_name(data.knopka[SFK3],iceb_u_inttochar(SFK3));
gtk_widget_show(data.knopka[SFK3]);

sprintf(strsql,"F4 %s",gettext("Проводки"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(prov_mu_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Автоматическое выполнение проводок с использованием табличной настройки"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(strsql,"%sF4 %s",RFK,gettext("Протокол"));
data.knopka[SFK4]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK4],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK4], "clicked",G_CALLBACK(prov_mu_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK4],gettext("Просмотр протокола хода автоматического выполнения проводок"));
gtk_widget_set_name(data.knopka[SFK4],iceb_u_inttochar(SFK4));
gtk_widget_show(data.knopka[SFK4]);

sprintf(strsql,"F5 %s",gettext("Настройка"));
data.knopka[FK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[FK5], "clicked",G_CALLBACK(prov_mu_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK5],gettext("Настройка автоматического выполнения проводок"));
gtk_widget_set_name(data.knopka[FK5],iceb_u_inttochar(FK5));
gtk_widget_show(data.knopka[FK5]);

sprintf(strsql,"%sF5 %s",RFK,gettext("Печать"));
data.knopka[SFK5]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK5],TRUE,TRUE, 0);
g_signal_connect(data.knopka[SFK5], "clicked",G_CALLBACK(prov_mu_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[SFK5],gettext("Распечатка проводок"));
gtk_widget_set_name(data.knopka[SFK5],iceb_u_inttochar(SFK5));
gtk_widget_show(data.knopka[SFK5]);

sprintf(strsql,"F6 %s",gettext("Проводки"));
data.knopka[FK6]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(prov_mu_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Автоматическое выполнение проводок с использованием списка настройки"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(strsql,"F9 %s",gettext("Оплата"));
data.knopka[FK9]=gtk_button_new_with_label(strsql);
g_signal_connect(data.knopka[FK9], "clicked",G_CALLBACK(prov_mu_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK9],gettext("Переход в режим ввода и корректировки оплат к документу"));
gtk_widget_set_name(data.knopka[FK9],iceb_u_inttochar(FK9));
gtk_widget_show(data.knopka[FK9]);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE, 0);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(prov_mu_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);

gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

//gtk_widget_grab_focus(data.knopka[FK10]);

prov_mu_create_list(&data);

//gtk_window_maximize(GTK_WINDOW(data.window));
//gtk_window_fullscreen(GTK_WINDOW(data.window));
gtk_widget_show(data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void prov_mu_create_list (class prov_mu_data *data)
{
iceb_clock sss(data->window);
GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
char strsql[512];
int  kolstr=0;
SQL_str row;

data->kl_shift=0; //0-отжата 1-нажата  

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(prov_mu_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(prov_mu_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);


model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_LONG, 
G_TYPE_INT,  
G_TYPE_INT,
G_TYPE_INT);

iceb_u_str zapros;

if(data->metka_r == 2)
  sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,komen,ktoi,vrem,val,kekv \
from Prov where kto='%s' and pod=%d and nomd='%s' and \
datd='%04d-%02d-%02d' and oper='%s' and tz=%d order by datp asc",
  data->metkasys.ravno(),data->sklad,data->nomdok.ravno_filtr(),
  data->gd,data->md,data->dd,data->kodop.ravno_filtr(),data->tipz);

if(data->metka_r == 0)
  sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,komen,ktoi,vrem,val,kekv \
from Prov where kto='%s' and pod=%d and nomd='%s' and \
datd='%04d-%02d-%02d' and oper='%s' and tz=%d and deb <> 0. order by datp asc",
  data->metkasys.ravno(),data->sklad,data->nomdok.ravno_filtr(),
  data->gd,data->md,data->dd,data->kodop.ravno_filtr(),data->tipz);

if(data->metka_r == 1)
  sprintf(strsql,"select datp,sh,shk,kodkon,deb,kre,komen,ktoi,vrem,val,kekv \
from Prov where kto='%s' and pod=%d and nomd='%s' and \
datd='%04d-%02d-%02d' and oper='%s' and tz=%d and kre <> 0. order by datp asc",
  data->metkasys.ravno(),data->sklad,data->nomdok.ravno_filtr(),
  data->gd,data->md,data->dd,data->kodop.ravno_filtr(),data->tipz);

data->zapros.new_plus(strsql);


if((kolstr=cur.make_cursor(&bd,data->zapros.ravno())) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),data->zapros.ravno(),data->window);
  return;
 }
//gtk_list_store_clear(model);

class iceb_u_str ss;

data->kolzap=0;
data->suma_deb=0.;
data->suma_kre=0.;

while(cur.read_cursor(&row) != 0)
 {
//  printf("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
  
//  if(prov_mu_prov_row(row,&data->rk,data->window) != 0)
//    continue;


//  if(iceb_u_SRAV(data->vvod.god.ravno(),row[0],0) == 0 && iceb_u_SRAV(data->vvod.shet.ravno(),row[1],0) == 0)
//    data->snanomer=data->kolzap;
  
  data->suma_deb+=atof(row[4]);
  //Кредит
  data->suma_kre+=atof(row[5]);
  

  if(atoi(row[10]) != 0)
    ss.new_plus(row[10]);
  else  
    ss.new_plus("");

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_DATA,iceb_u_datzap(row[0]),
  COL_KEKV,ss.ravno(),
  COL_SHET,row[1],
  COL_SHET_KOR,row[2],
  COL_KONTR,row[3],
  COL_DEBET,row[4],
  COL_KREDIT,row[5],
  COL_KOMENT,row[6],
  COL_DATA_VREM,iceb_u_vremzap(row[8]),
  COL_KTO,iceb_kszap(row[7],data->window),
  COL_VREM,atol(row[8]),
  COL_KTOZ,atoi(row[7]),
  COL_VAL,atoi(row[9]),
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

prov_mu_add_columns (GTK_TREE_VIEW (data->treeview));

if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK5]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK3]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK5]),TRUE);//Доступна
 }
if(data->metka_avp == 0)
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK4]),FALSE);//Недоступна
else
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK4]),TRUE);//Доступна

gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

prov_mu_sapka_menu(data);

gtk_widget_show(data->label_kolstr);


}

/*****************/
/*Создаем колонки*/
/*****************/

void prov_mu_add_columns(GtkTreeView *treeview)
{
GtkCellRenderer *renderer;


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата"), renderer,"text", COL_DATA,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счёт"), renderer,"text",COL_SHET,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
-1,gettext("Счёт кор."), renderer,"text", COL_SHET_KOR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кон-нт"), renderer,"text", COL_KONTR,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дебет"), renderer,"text", COL_DEBET,NULL);


renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кредит"), renderer,"text", COL_KREDIT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("КЭКР"), renderer,"text", COL_KEKV,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Коментарий"), renderer,"text", COL_KOMENT,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Дата и время записи"), renderer,"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),-1,
gettext("Кто записал"), renderer,"text", COL_KTO,NULL);


}

/****************************/
/*Выбор строки*/
/**********************/

void prov_mu_vibor(GtkTreeSelection *selection,class prov_mu_data *data)
{
//printf("prov_mu_vibor\n");
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *shet_d;
gchar *shet_k;
gchar *datap;
gchar *debet;
gchar *kredit;
gchar *koment;
gchar *kontr;
gint ktoz;
glong vrem;
gint val;
gint nomer;
gchar *kekv;

gtk_tree_model_get(model,&iter,
COL_SHET,&shet_d,
COL_SHET_KOR,&shet_k,
COL_DATA,&datap,
COL_DEBET,&debet,
COL_KREDIT,&kredit,
COL_KONTR,&kontr,
COL_KOMENT,&koment,
COL_KTOZ,&ktoz,
COL_VREM,&vrem,
COL_VAL,&val,
COL_KEKV,&kekv,
NUM_COLUMNS,&nomer,-1);


data->shet_v.new_plus(shet_d);
data->shet_kor_v.new_plus(shet_k);
data->kontr_v.new_plus(kontr);
data->datap_v.new_plus(datap);
data->koment_v.new_plus(koment);
data->debet_v=atof(debet);
data->kredit_v=atof(kredit);
data->ktoz_v=ktoz;
data->vrem_v=vrem;
data->val_v=val;
data->snanomer=nomer;
data->kekv=atoi(kekv);

g_free(shet_d);
g_free(shet_k);
g_free(datap);
g_free(debet);
g_free(kredit);
g_free(kontr);
g_free(koment);
g_free(kekv);
/*
printf("%s %s %s %s %.2f %.2f %s %d\n",
data->datap_v.ravno(),
data->shet_v.ravno(),
data->shet_kor_v.ravno(),
data->kontr_v.ravno(),
data->debet_v,
data->kredit_v,
data->koment_v.ravno(),
data->snanomer);
*/
}

/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  prov_mu_knopka(GtkWidget *widget,class prov_mu_data *data)
{
iceb_u_str shetd;
iceb_u_str shetk;
iceb_u_str suma;
FILE *ff_prot=NULL;

int knop=atoi(gtk_widget_get_name(widget));

data->kl_shift=0; //Сбрасываем нажатый сшифт так как при после запуска нового меню он не сбрасывается

switch (knop)
 {

  case GDK_KEY_plus: //это почемуто не работает Нажата клавиша плюс на основной клавиатуре
  case GDK_KEY_KP_Add: //Нажата клавиша "+" на дополнительной клавиатуре
  case FK2:
    if(prov_mu_prpvblok(data) != 0)
     return;

    if(iceb_vprov(data->metkasys.ravno(),"","","","","",data->nomdok.ravno(),data->kodop.ravno(),data->sklad,
    data->dd,data->md,data->gd,data->kontr.ravno(),data->vrem_v,data->ktoz_v,1,data->tipz,0,0,data->window) != 0)
         return;  
    
    prov_mu_create_list(data);

    return;  

  case SFK2:
    if(data->kolzap == 0)
     return;
    if(prov_mu_prpvblok(data) != 0)
     return;
    shetd.new_plus(data->shet_v.ravno());
    shetk.new_plus(data->shet_kor_v.ravno());
    suma.new_plus(data->debet_v);
    if(data->debet_v == 0)
     {
      shetd.new_plus(data->shet_kor_v.ravno());
      shetk.new_plus(data->shet_v.ravno());
      suma.new_plus(data->kredit_v);
     }
    
    if(iceb_vprov(data->metkasys.ravno(),shetd.ravno(),shetk.ravno(),
    suma.ravno(),data->datap_v.ravno(),data->koment_v.ravno(),
    data->nomdok.ravno(),data->kodop.ravno(),data->sklad,
    data->dd,data->md,data->gd,data->kontr.ravno(),data->vrem_v,data->ktoz_v,1,data->tipz,data->val_v,data->kekv,data->window) != 0)
         return;  
    
    prov_mu_create_list(data);

    return;  

  case FK3:
    if(data->kolzap == 0)
     return;
    if(prov_mu_prpvblok(data) != 0)
     return;
    shetd.new_plus(gettext("Удалить запись ? Вы уверены ?"));
    if(iceb_menu_danet(&shetd,2,data->window) != 1)
     return;

    iceb_udprov(data->val_v,data->datap_v.ravno(),data->shet_v.ravno_filtr(),
    data->shet_kor_v.ravno_filtr(),data->vrem_v,
    data->debet_v,data->kredit_v,data->koment_v.ravno_filtr(),2,data->window);

    prov_mu_create_list(data);
    
    return;  

  case SFK3: 
    if(data->kolzap == 0)
     return;
  
    if(prov_mu_prpvblok(data) != 0)
     return;

  
    shetd.new_plus(gettext("Удалить все проводки ? Вы уверены ?"));
    if(iceb_menu_danet(&shetd,2,data->window) != 1)
     return;
  
    if(iceb_udprgr(data->metkasys.ravno(),data->dd,data->md,data->gd,data->nomdok.ravno(),data->sklad,data->tipz,data->window) != 0)
     return;

    prov_mu_create_list(data);
    return;  
    
  
  case FK4:
    if(prov_mu_prpvblok(data) != 0)
     return;
    if((ff_prot = fopen(data->imaf_prot,"w")) == NULL)
     {
      iceb_er_op_fil(data->imaf_prot,"",errno,data->window);
      ff_prot=NULL;
     }

    if(data->metka_nsnp == 1)
      avtpromuw(data->tipz,data->dd,data->md,data->gd,data->kodop.ravno(),
      data->nomdok.ravno(),data->sklad,data->kontr.ravno(),data->sumkor,data->nomdokp.ravno(),data->pnds,ff_prot,data->window);
    else
    avtpromu1(data->tipz,data->dd,data->md,data->gd,data->nomdok.ravno(),ff_prot,data->window);
    
    if(ff_prot != NULL)
     {
      fclose(ff_prot);
      data->metka_avp=1;
     }
    prov_mu_create_list(data);
    return;  

  case SFK4:/*Просмотр протокола выполнения проводок*/
    iceb_prosf(data->imaf_prot,data->window);
    return;  
  
  case FK5:
    menuvnpw(1,data->tipz,data->window);
    return;  

  case SFK5:
    iceb_raspprov(data->zapros.ravno(),data->dd,data->md,data->gd,data->nomdok.ravno(),1,data->window);
    return;  
  

  case FK6:
    if(prov_mu_prpvblok(data) != 0)
     return;
    if((ff_prot = fopen(data->imaf_prot,"w")) == NULL)
     {
      iceb_er_op_fil(data->imaf_prot,"",errno,data->window);
      ff_prot=NULL;
     }


    avtpromu1(data->tipz,data->dd,data->md,data->gd,data->nomdok.ravno(),ff_prot,data->window);

    if(ff_prot != NULL)
      fclose(ff_prot);

    prov_mu_create_list(data);
    return;  
  
    
    return;  

  case FK9:
   
    l_opldok(1,data->dd,data->md,data->gd,data->nomdok.ravno(),data->sklad,
    skoplmuw(data->dd,data->md,data->gd,data->nomdok.ravno(),data->sklad,data->window),
    data->window);
   
    return;  
  
    
  case FK10:
    
    prosprw(0,data->sklad,data->dd,data->md,data->gd,data->nomdok.ravno(),data->tipz,data->lnds,data->sklad1,data->nomdokp.ravno(),data->kodop.ravno(),NULL,data->window);
    iceb_sizww(data->name_window.ravno(),data->window);
    unlink(data->imaf_prot);    
    gtk_widget_destroy(data->window);
    return;
 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   prov_mu_key_press(GtkWidget *widget,GdkEventKey *event,class prov_mu_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    if(data->kl_shift == 1) //Нажата
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");
    else
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
    if(data->kl_shift == 1) //Нажата
      g_signal_emit_by_name(data->knopka[SFK3],"clicked");
    else
      g_signal_emit_by_name(data->knopka[FK3],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    if(data->kl_shift == 1) //Нажата
      g_signal_emit_by_name(data->knopka[SFK4],"clicked");
    else
      g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_F5:
    if(data->kl_shift == 1) //Нажата
      g_signal_emit_by_name(data->knopka[SFK5],"clicked");
    else
    g_signal_emit_by_name(data->knopka[FK5],"clicked");
    return(TRUE);

  case GDK_KEY_F6:
    g_signal_emit_by_name(data->knopka[FK6],"clicked");
    return(TRUE);

  case GDK_KEY_F9:
    g_signal_emit_by_name(data->knopka[FK9],"clicked");
    return(TRUE);



  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("prov_mu_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  default:
    printf("Не выбрана клавиша !\n");
    break;
 }

return(TRUE);
}
/****************************/
/*Выбор строки*/
/**********************/
void prov_mu_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class prov_mu_data *data)
{
g_signal_emit_by_name(data->knopka[SFK2],"clicked");
}

/***************/
/*Шапка меню*/
/***************/

void prov_mu_sapka_menu(class prov_mu_data *data)
{
iceb_u_str stroka;

stroka.new_plus(gettext("Работа с проводками (материальный учёт)"));
stroka.ps_plus(gettext("Склад"));

stroka.plus(":");
stroka.plus(data->sklad);
stroka.plus(" ");
stroka.plus(data->naim_sklad.ravno());

stroka.ps_plus(gettext("Номер документа"));
stroka.plus(":");
stroka.plus(data->nomdok.ravno());

stroka.plus(" ");
stroka.plus(gettext("Дата"));
stroka.plus(":");
stroka.plus(data->dd);
stroka.plus(".");
stroka.plus(data->md);
stroka.plus(".");
stroka.plus(data->gd);



char strsql[512];
sprintf(strsql," %s:%d",gettext("Количество записей"),
data->kolzap);
stroka.plus(strsql);


stroka.ps_plus(gettext("Контрагент"));
stroka.plus(":");
stroka.plus(data->kontr.ravno());
stroka.plus("/");
stroka.plus(data->naim_kontr.ravno());

stroka.ps_plus(gettext("Операция"));
stroka.plus(":");
stroka.plus(data->kodop.ravno());
stroka.plus("/");
stroka.plus(data->naim_kodop.ravno());

stroka.plus(" ");
if(data->tipz == 1)
 {
  stroka.plus("(");
  stroka.plus(gettext("Приход"));
  stroka.plus(")");
 }
if(data->tipz == 2)
 {
  stroka.plus("(");
  stroka.plus(gettext("Расход"));
  stroka.plus(")");
 }
stroka.ps_plus(gettext("Сумма выполненных проводок"));
stroka.plus(":");
stroka.plus(data->suma_deb);
stroka.plus("/");
stroka.plus(data->suma_kre);

gtk_label_set_text(GTK_LABEL(data->label_kolstr),stroka.ravno());

}
/******************************/
/*Обработчик нажатия радиокнопок*/
/**********************************/

void  prov_mu_radio0(GtkWidget *widget,class prov_mu_data *data)
{

if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton[0])) == TRUE)
 {
  if(data->metka_r == 0)
   return;
  data->metka_r=0;
 }
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton[1])) == TRUE)
 {
  if(data->metka_r == 1)
   return;
  data->metka_r=1;
 }
if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->radiobutton[2])) == TRUE)
 {
  if(data->metka_r == 2)
   return;
  data->metka_r=2;
 }

prov_mu_create_list(data);

}
/**************************/
/*Проверка блокировки*/
/************************/

int prov_mu_prpvblok(class prov_mu_data *data)
{
if(iceb_pbpds(data->md,data->gd,data->window) != 0)
 return(1);

if(data->kolzap == 0)
 return(0);


short d1,m1,g1;
iceb_u_rsdat(&d1,&m1,&g1,data->datap_v.ravno(),1);
if(iceb_pvglkni(m1,g1,data->window) != 0)
 return(1);

return(0);
}
