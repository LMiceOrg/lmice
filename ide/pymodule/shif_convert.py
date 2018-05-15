#!/usr/bin/env python
# -*- coding: utf-8 -*-

shif_csv_def = """
TradingDay,InstrumentID,UpdateTime,UpdateMillisec,LastPrice,
Volume,OpenPrice,OpenInterest,PreSettlementPrice,PreClosePrice,

PreOpenInterest,HighestPrice,LowestPrice,UpperLimitPrice,LowerLimitPrice,
BidPrice1,BidVolume1,AskPrice1,AskVolume1,BidPrice2,

BidVolume2,AskPrice2,AskVolume2,BidPrice3,BidVolume3,
AskPrice3,AskVolume3,BidPrice4,BidVolume4,AskPrice4,

AskVolume4,BidPrice5,BidVolume5,AskPrice5,AskVolume5,
AveragePrice,ExchangeID,ExchangeInstID,Tumover,ClosePrice,

SettlementPrice,PreDelta,CurrDelta,volumeleft,openinterestleft,Updatelocaltime
"""

Dummy_ChinaL1Msg="""
#pragma pack(push, 4)
typedef struct _Dummy_ChinaL1Msg
{
    inst8_t m_inst;     /*< char[8] */
    int64_t m_time;     /*< time in epoch micro seconds */
    double m_bid;
    double m_offer;
    int32_t m_bid_quantity;
    int32_t m_offer_quantity;
    int32_t m_volume;
    double m_notional;
    double m_limit_up;
    double m_limit_down;
}Dummy_ChinaL1Msg;
#pragma pack(pop)
"""

import struct
import time

class FutureSHIF:
    def load(self, name):
        self.name = name
    def test(self):
        t1 = time.time()
        self.shif_csv_to_ChinaL1Msg()
        t2=time.time()
        print "1=", t2-t1

    def shif_csv_to_ChinaL1Msg2(self):
        w=open("output.dat", "wb")
        f=open(self.name, "r")

        t1=time.time()

        cnt = 0

        data = ''
        while cnt < 1000000:
            blob = f.read(1024*32)
            if len(blob) == 0:
                break

            data += blob
            parsing = True

            while parsing:
                pos = data.find('\n')
                if(pos < 0):
                    parsing = False
                    break
                line = data[0:pos+1]

                data = data[pos+1:]
                if cnt == 0:
                    cnt += 1
                    continue
                self.parse_line(line, w)
                cnt += 1


    def shif_csv_to_ChinaL1Msg(self):

        colen = len(shif_csv_def.split(","))

        w=open("output.dat", "wb")

        f=open(self.name, "r")

        t1=time.time()

        # ignore first line
        line = f.readline()

        cnt = 0

        line=f.readline()
        while len(line)>0:
            self.parse_line(line, w)
            cnt = cnt + 1

#            if cnt > 1000000:
#                break

            line=f.readline()
        f.close()
        w.close()
        t2 = time.time()
        print "time:",  t2-t1, " count=", cnt
    def parse_line(self, line, w):
        try:
            #print line
            data = line.split(",")
            minst = data[1][:8]
            minst = minst+'\0'*(8-len(minst))
            mtime = 0
            if len(data[0]) == 0:
                mtime = int( float( time.mktime(time.strptime(" ".join((data[-1][:10], data[2])), "%Y-%m-%d %H:%M:%S")) ) * 1000000 + int(data[3])*1000 )
            else:
                mtime = int( float( time.mktime(time.strptime(" ".join((data[0], data[2])), "%Y%m%d %H:%M:%S")) ) * 1000000 + int(data[3])*1000 )
            mbid=float(data[15] if len(data[15])>0 else 0)
            moffer=float(data[17] if len(data[17])>0 else 0)
            mbid_quantity=int(float(data[16] if len(data[16])>0 else 0))
            moffer_quantity=int(float(data[18] if len(data[18])>0 else 0))
            mvolume=int(float(data[5] if len(data[5])>0 else 0))
            mnotional=float(data[38] if len(data[38])>0 else 0)
            mlimit_up=float(data[13] if len(data[13])>0 else 0)
            mlimit_down=float(data[14] if len(data[14])>0 else 0)

#            w.write( struct.pack("8sL2d3i3d", minst, mtime, mbid, moffer,
#                mbid_quantity, moffer_quantity, mvolume,
#                mnotional, mlimit_up, mlimit_down) )
        except:
            print line




