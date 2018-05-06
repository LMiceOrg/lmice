#!/usr/bin/env python
# -*- coding: utf-8 -*-

import xlrd
import sys
import os
import struct

product_mapping={
"铜":"cu",
"铝":"al",
"锌":"zn",
"铅":"pb",
"镍":"ni",
"锡":"sn",
"黄金":"au",
"白银":"ag",
"螺纹钢":"rb",
"线材":"wr",
"热轧卷板":"hc",
"燃料油":"fu",
"石油沥青":"bu",
"天然橡胶":"ru"
}

class Feature:
    def __init__(self,id, date, tuneover):
        self.product_id=id[:-4]
        self.date=date
        self.instrument_id =id
        self.tuneover=tuneover
def parseDayXl(name, features):
    if name[-4:] != ".csv":
        raise ValueError("Day csv file name must be data_yyyymmdd.csv, this one is %s" % name)
    f=open(name, "r")

    sud_dir, base_name = os.path.split(name)
    sdate = base_name[5:5+8]

    if not sdate.isdigit() or len(sdate) != 8:
        raise ValueError("Day csv name format should be date_yyyymmdd.csv, this one is %s." % base_name)

    instrument_id = ""

    for ln in f.readlines():
        row = ln.split(",")
        cell0 = row[0].decode("gb2312").strip()
        if cell0[:4]==u"商品名称":
            product_name = cell0[5:].strip()
            product_id = product_mapping[product_name.encode("utf-8")]
            #print product_name, product_id
        if cell0.isdigit():
            instrument_id =product_id + cell0
            price = float(row[6])
            volumn = float(row[9])
            tuneover = price*volumn

            if sdate not in features:
                features[sdate]={}
            if product_id not in features[sdate]:
                features[sdate][product_id]={}
            features[sdate][product_id][instrument_id]=tuneover
    f.close()


def parseYearXl(name, features):

    workbook = xlrd.open_workbook(name)
    sheet = workbook.sheet_by_index(0)
    names = sheet.row_values(2)

    instrument_id = ""
    for i in range(sheet.nrows -3):
        r = i+3
        row = sheet.row_values(r)
        date = row[1]
        tuneover = row[12]
        inst_id = row[0]
        if len(inst_id) >0 and len(inst_id)<5:
            continue
        if len(date) != 8:
            continue
        if row[0] != instrument_id and row[0] != "":
            instrument_id = row[0]
        else:
            row[0] = instrument_id
        feature = Feature(instrument_id, date, tuneover)
        product_id = feature.product_id
        if date not in features:
            features[date]={}
        if product_id not in features[date]:
            features[date][product_id]={}
        #print("%s -%s -%s: %f" % (date, product_id, instrument_id, tuneover))
        features[date][product_id][instrument_id]=tuneover
def parseXl(name, features):
    sub_dir, base_name = os.path.split(name)
    #print base_name
    if base_name.find("data") >= 0:
        parseDayXl(name, features)
    else:
        parseYearXl(name, features)

def schemebinarygen(name, features):
    f=open(name, "wb")
    '''
int32 --> alias count
    int32 --> inst count
    int64  --> alias
        int64 --> date
        int64 --> inst_id
    '''
    results = dict()
    dates = features.keys()
    dates.sort()
    for sdate in dates:
        products = features[sdate]
        for product in products.keys():
            insts = products[product]
            instlist=list()
            tunelist=list()
            for inst_id in insts.keys():
                tuneover = insts[inst_id]
                instlist.append(inst_id)
                tunelist.append(tuneover)
            slist=list()
            slist.extend(tunelist)
            slist.sort()
            ids=["01", "02", "03", "04"]


            for i in range(len(ids)):
                idx = len(slist) - i -1
                #print sdate, len(slist), idx, tunelist, insts, sdate, product
                v = slist[idx]
                alias = product + ids[i]
                tune_index = tunelist.index(v)
                inst_id = instlist[tune_index]
                balias = alias + (8-len(alias))*'\x00'
                bdate = sdate + (8 - len(sdate))*'\x00'
                binst_id = inst_id + (8-len(inst_id))*'\x00'
                if balias not in results:
                    results[balias]=list()
                results[balias].append(bdate+binst_id)
    alias_count = len(results.keys())
    f.write(struct.pack('i', alias_count))
    for balias in results.keys():
        result = results[balias]
        size = len(result)
        f.write(struct.pack('i', size) )
        f.write(balias)
        for item in result:
            f.write(item)
    f.close()

def schemegen(name, features):
    f=open(name, "w")
    f.write('''/** Copyright 2018 He Hao<hehaoslj@sina.com> */


#ifndef FUTUREMODEL_INCLUDE_SCHEME_H_
#define FUTUREMODEL_INCLUDE_SCHEME_H_


typedef struct _product_alias {
    char date[10];
    char alias[6];
    char instrument_id[8];
} product_alias;

''')
    f.write('product_alias product_aliases[] = {\n')
    dates = features.keys()
    dates.sort()
    for sdate in dates:
        #print "date = ", sdate, features.keys()
        products = features[sdate]
        for product in products.keys():
            insts = products[product]
            instlist=list()
            tunelist=list()
            for inst_id in insts.keys():
                tuneover = insts[inst_id]
                instlist.append(inst_id)
                tunelist.append(tuneover)
            slist=list()
            slist.extend(tunelist)
            slist.sort()
            ids=["01", "02", "03", "04"]
            f.write('    ')
            for i in range(len(ids)):
                idx = len(slist) - i -1
                #print sdate, len(slist), idx, tunelist, insts, sdate, product
                v = slist[idx]
                tune_index = tunelist.index(v)
                inst_id = instlist[tune_index]
                f.write('{"%s", "%s%s", "%s"}, ' % (sdate, product, ids[i], inst_id) )
            f.write('\n')
    f.write('    {"", "", ""}\n};\n#endif // FUTUREMODEL_INCLUDE_SCHEME_H_\n\n')
    f.close()

if __name__ == "__main__":
    names = sys.argv[1:]
    size = len(names)
    features = dict()
    for name in names:
        name_list = list()
        if os.path.isdir(name):
            for n in os.listdir(name):
                sub_name = os.path.join(name, n)
                if os.path.isdir(n):
                    continue
                elif n[0] in ('~', '.'):
                    continue
                elif n[-4:] in ('.xls', '.csv'):
                    name_list.append( sub_name )
        else:
            name_list.append(name)
        for nm in name_list:
            parseXl(nm, features)

    schemegen("scheme.h", features)
    schemebinarygen('scheme.bin', features)

