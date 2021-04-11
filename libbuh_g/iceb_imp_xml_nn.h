/*$Id:$*/
/*01.02.2015	27.01.2015	Белых А.И.	iceb_imp_xml_nn.h
Структура шапки налоговой накладной
*/

class iceb_imp_xml_nn_r
 {
  public:
   class iceb_u_str data_vnn; /*Дата выписки налоговой накладной*/

   class iceb_u_str nom_nn; /*Номер налоговой накладной*/
   class iceb_u_str kod_vd; /*код вида деятельности*/
   class iceb_u_str nom_fil; /*Номер филиала*/

   class iceb_u_str naim_prod; /*наименование продавца*/
   class iceb_u_str edrpuo_prod; /*ЕДРПУО продавца*/
   class iceb_u_str naim_pokup; /*наименование покупателя*/
   class iceb_u_str inn_prod;   /*индивидуальный налоговый номер продавца*/
   class iceb_u_str inn_pokup;  /*индивидуальный налоговый номер покупателя*/
   class iceb_u_str adres_prod; /*адрес продавца*/
   class iceb_u_str adres_pokup; /*адрес покупателя*/
   class iceb_u_str telef_prod;  /*телефон продавца*/
   class iceb_u_str telef_pokup; /*телефон покупателя*/

   class iceb_u_str ima_tablic;
   class iceb_tmptab tabtmp;
   class iceb_u_str kontr_prod; /*код контрагента продавца*/
   
   /*реквизиты которые вводятся в дополнительном меню*/
   class iceb_u_str datdok;
   class iceb_u_str sklad;
   class iceb_u_str nomdok;
   class iceb_u_str kod_op;
   class iceb_u_str shetu;
   int nds; /*метка ндс*/
   float pnds; /*процент НДС*/


   iceb_imp_xml_nn_r()
    {
     clear_data();
     ima_tablic.plus("");
    };

   void clear_data()
    {
     data_vnn.new_plus("");
     nom_nn.new_plus("");
     kod_vd.new_plus("");
     nom_fil.new_plus("");
     naim_prod.new_plus("");
     edrpuo_prod.new_plus("");
     naim_pokup.new_plus("");
     inn_prod.new_plus("");
     inn_pokup.new_plus("");
     adres_pokup.new_plus("");
     adres_prod.new_plus("");
     telef_prod.new_plus("");
     telef_pokup.new_plus("");
     kontr_prod.new_plus("");

     datdok.new_plus("");
     sklad.new_plus("");
     nomdok.new_plus("");
     nds=0;
     kod_op.new_plus("");
     shetu.new_plus("");
     pnds=0.;
    };
 };
 