/*$Id: vdnuw.h,v 1.8 2012-06-07 05:35:58 sasa Exp $*/
/*16.03.2017	22.08.2005	Белых А.И.	vdnuw.h
Реквизиты шапки документа на услуги
*/

class vdnuw_rek
 {
  public:
   //Ключевые реквизиты документа 
   short ds,ms,gs;
   class iceb_u_str nomdoks;
   int podrs;
   short tipz;
   class iceb_u_str kontrz; //Код контрагента первоначальный
   class iceb_u_str nom_nal_nakz; //Первоначальный номер налоговой накладной
   unsigned int kto_zap_hd;
   
   //Введённые в меню реквизиты
   class iceb_u_str kontr;
   class iceb_u_str datad;
   class iceb_u_str podrv;
   class iceb_u_str nomdokv;
   class iceb_u_str kodop;
   class iceb_u_str usl_prod;
   class iceb_u_str for_opl;
   class iceb_u_str nom_nal_nak;
   class iceb_u_str data_opl;
   class iceb_u_str osnov;
   class iceb_u_str nomdok_pos;
   class iceb_u_str data_vnp;
   class iceb_u_str data_pol_nal_nak;
   class iceb_u_str dover;
   class iceb_u_str data_dover;
   class iceb_u_str sherez;
   class iceb_u_str k00; /*код контрагента реквизиты которого берутся в распечатку*/   
   int lnds;    

   vdnuw_rek()
    {
     clear_rek();
     kontrz.plus("");
     nom_nal_nakz.plus("");
     k00.plus("00");
    }
   void clear_rek()
    {
     lnds=0;
     kontr.new_plus("");
     datad.new_plus("");
     podrv.new_plus("");
     nomdokv.new_plus("");
     kodop.new_plus("");
     usl_prod.new_plus("");
     for_opl.new_plus("");
     nom_nal_nak.new_plus("");
     data_opl.new_plus("");
     osnov.new_plus("");
     nomdok_pos.new_plus("");
     data_pol_nal_nak.new_plus("");
     dover.new_plus("");
     data_dover.new_plus("");
     sherez.new_plus("");
     data_vnp.new_plus("");
    }

 };


