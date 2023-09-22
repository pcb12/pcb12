from machine import UART, I2C, Pin, WDT
from mfrc522 import MFRC522
from pico_i2c_lcd import I2cLcd
import time
import json
import _thread
import gc
import machine


class Meter:
    def __init__(self):
        self.dc = _thread.allocate_lock()
        self.wdt = WDT(timeout=8000)
        self.lcd = None
        try:
            self.lcd = I2cLcd(I2C(0, sda=Pin(12), scl=Pin(13), freq=400000), 0x27, 2, 16)
        except:
            self.lcd = I2cLcd(I2C(0, sda=Pin(12), scl=Pin(13), freq=400000), 0x3F, 2, 16)
            
        self.rd = MFRC522(spi_id=0, sck=2, miso=4, mosi=3, cs=1, rst=0)
        self.gprs = UART(0, baudrate=9600, rx=Pin(17), tx=Pin(16))
        self.op = Pin(27 , Pin.OUT)
        self.cp = Pin(26, Pin.OUT)
        self.old = Pin(6, Pin.OUT)
        self.cld = Pin(15, Pin.OUT)
        self.bld = Pin(25, Pin.OUT)
        self.motion = 0
        self.previous = 0
        self.ff = 0
        self.timer = time.time()
        self.lbo = self.lcd.backlight_on
        self.lbf = self.lcd.backlight_off
        self.pts = self.lcd.putstr
        self.lmt = self.lcd.move_to
        self.lcr = self.lcd.clear
        self.stage = 0
        self.iml = self.rj()["is_meter_locked"]
        self.url= self.rj()["url"]
        self.gw = self.gprs.write
        self.gr = self.gprs.read
        self.nfl = 0
        self.cstr = 1
        self.rfr = 251
        self.fld = ""
        self.lnt = 0
        self.reboot = 1
        self.cn_t = 0
        self.ecu = 0
        self.tsp = time.sleep
        self.pt = time.time()
        gc.enable()
        
        i = 0
        self.cv()
        self.pts("Tafadhali subiri")
        self.lmt(0, 1)
        se = 0
        while 0:
            self.wdt.feed()
            self.lmt(i, 1)
            self.pts("-")
            self.tsp(0.4)
            self.old.value(se)
            self.cld.value(se)
            se = not se
            self.gw("AT+COPS?\r")
            rpsl= ""
            try:
                rpsl= self.gr().decode().split()
            except:
                pass
            if (len(rpsl) >= 4):
                if rpsl[2] == "0":
                    pass
                else:
                    self.lcr()
                    self.pts("Mtandao: "+str(rpsl[2].split(",")[2].replace('"', "")))
                    self.gw("AT+CSQ\r")
                    self.wdt.feed()
                    self.tsp(2)
                    self.lmt(0, 1)
                    strs = self.gr().decode().split()
                    vl = int(strs[strs.index('+CSQ:')+1].split(",")[0]) / 31
                    vl = vl*100 
                    self.pts("Nguvu  : " + str("{:.2f}".format(vl))+"%")
                    self.wdt.feed()
                    self.tsp(3)
                    del strs, vl, rpsl
                    break
                
            if i < 16:
                self.lmt(i, 1)
                self.pts("_")
                i = i + 1
            else:
                i = 0
            del rpsl
        self.cv()
        self.gr()
        del i,se
        gc.collect()
        
        self.bld.value(1)
        self.lcr()
        self.pts("     RUWASA     ")
        self.lmt(0, 1)
        self.pts("Maji Kidigitali!")
        self.tsp(2)
        self.lcr()
        self.lbf()

    def rj(self):
        dt = None
        db_file = open("database.json", "r")
        dt = json.loads(db_file.read())
        db_file.close()
        del db_file
        gc.collect()
        return dt

    def wj(self, dt):
        res = False
        db_file = open("database.json", "w+")
        try:
            json.dump(dt, db_file)
            res = True
        except Exception as E:
            print(E)
        db_file.close()
        del db_file
        gc.collect()
        return res

    def cv(self):
        self.old.value(0)
        self.cld.value(1)
        self.op.value(0)
        self.cp.value(0)
        return 0

    def ov(self):
        self.old.value(1)
        self.cld.value(0)
        self.op.value(1)
        self.cp.value(0)
        return 0

    def hit(self, Pin):
        self.ff += 1
        

    
    def tmp(self, Pin):
        db = self.rj()
        self.iml = True
        db["is_meter_locked"] = self.iml
        swj = self.wj(db)
        self.cv()
        self.lbo()
        self.lcr()
        self.lmt(0, 0)
        self.pts(" MITA IMEFUNGWA")
        del db, swj
        return 0

    def handle_interrupt(self,pin):
      self.motion +=1
      
    def dw(self, uv, crd, db):
        self.ff = 0
        v = 0.0
        qc = 0
        upc = 0
        loop = True
        shw = True
        cvl = 0
        const = db["calibration_factor"]
        i = Pin(5, Pin.IN).irq(trigger=Pin.IRQ_RISING, handler=self.hit)
        self.lcr()
        self.previous = 0
        rtv = 0
        while loop:
            self.wdt.feed()
            (stat, tag_type) = self.rd.request(self.rd.REQIDL)
            if stat == self.rd.OK:
                (stat, uid) = self.rd.SelectTagSN()
                if stat == self.rd.OK:
                    rcd = uv - v
                    self.cv()
                    loop = False
                gc.collect()
            gc.collect()

            if time.time() - self.previous >= 1:
                self.previous = time.time()
                fcp = self.ff
                self.ff = 0
                v = ((fcp / const) / 60) + v
                rtv = v
                if fcp < 0.2:
                    qc += 1
                else:
                    qc = 0
                    upc += 1            
                    shw = True
                del fcp
                gc.collect()

            if upc > 10:
                db = self.rj()
                upc = 0
                db["users"][crd] = uv - v
                db["production"] += v
                cvl += v
                self.wj(db)
                del db
                gc.collect()
 
            if (v + 0.2) >= uv:
                self.lcr()
                self.lmt(0, 0)
                self.pts("salio limekwisha")
                rcd = uv - v
                self.cv()
                loop = False
                self.tsp(2)

            if qc >= 180:
                rcd = uv - v
                self.cv()
                loop = False

            if shw:
                self.lmt(0, 0)
                self.pts("SALIO :" + str("{:.1f}".format(uv-v)) + "L   ")
                self.lmt(0, 1)
                self.pts("CHOTWA:" + str("{:.2f}".format(v)) + "L  ")
                shw = False
            
        del (v, upc, loop, qc, const, shw)
        gc.collect()
        return rcd, cvl, rtv

    def pgd(self, dt):
        try:
            dt = json.loads(dt)
            db = self.rj()
            
            if "error" in dt:
                return 0
            
            db["response"] = {}
            db["transactions"] = {} 
            self.reboot = False
            self.ecu = 0
            dt = dt["body"]
            
            if "user_balance" in dt:
                for user in dt["user_balance"]:
                    if user in db["users"]:
                        db["users"][user] += dt["user_balance"][user]
                    else:
                        db["users"][user] = dt["user_balance"][user]
                db["response"]["users"] = dt["ids"]

            if "remove_user" in dt:
                for key in dt["remove_user"]:
                    del db["users"][key]
                db["response"]["removed_user"] = dt["remove_user"]
                
            if "server_ip" in dt:
                if len(dt["server_ip"]) > 5:
                    db["url"] = dt["server_ip"]
                db["response"]["server_ip"] = db["url"]

            if "lock_meter" in dt:
                db["is_meter_locked"] = dt["lock_meter"]
                self.iml = dt["lock_meter"]
                db["response"]["lock_meter"]  = db["is_meter_locked"]

            if "calibration_factor" in dt:
                if dt["calibration_factor"]  > 0:
                    db["calibration_factor"] = dt["calibration_factor"]
                db["response"]["calibration_factor"] = db["calibration_factor"]
                
            if "price_per_little" in dt:
                if dt["price_per_little"]  > 0:
                    db["ppl"] = dt["price_per_little"]
                db["response"]["price_per_little"] = db["ppl"]
                
            if "reset_production" in dt:
                db["production"] = 0
                db["response"]["production"] = db["production"]
            
            if "software-version" in dt:
                db["response"]["software-version"] = db["software-version"]
                    
            swj = self.wj(db)
            del (swj, db, dt)
            gc.collect()
            
        except Exception as E:
            print(E, "adding data")
        return 0

    def get_gprs(self):
        while 1:
            self.wdt.feed()
            self.dc.acquire()
            cms = [
                    "AT\r", "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r", "AT+SAPBR=3,1,\"APN\",\"internet\"\r", "AT+SAPBR=1,1\r", "AT+SAPBR=2,1\r", "AT+HTTPINIT\r", "AT+HTTPPARA=\"CID\",1\r",
                    'AT+HTTPPARA=\"URL\",\"'+self.url+'"\r',"AT+HTTPPARA=\"CONTENT\",\"application/json\"\r"," data-lenght","data-itself", "AT+HTTPACTION=1\r",
                    "AT+HTTPREAD\r", "AT+HTTPTERM\r", "AT+SAPBR=0,1\r"
                ]

            cl = len(cms)
            cd = [0.3, 6, 6, 6, 6, 6, 6, 4, 4, 6, 6, 10 ,5 ,5 , 5 * 60]

            if time.time() - self.pt >= cd[self.stage]:
                x = self.gr()
                print(x)
                
                if x ==  None:
                    self.gw("AT\r")
                    self.stage-=1
                    time.sleep(0.3)
                    
                if self.stage == 11:
                    try:
                        ida = x.decode().strip().split("\n")[3].split(",")[2]
                        self.lnt = int(ida)
                        self.nfl = int(self.lnt / self.rfr)
                        if self.lnt % self.rfr > 0:
                            self.nfl += 1
                            cms[12] = (
                                        "AT+HTTPREAD=" +
                                        str(0) + "," + str(self.rfr) + "\r"
                                    )
                        del ida
                    except:
                        pass

                if self.stage == 12:
                    try:
                        ida = x.decode().strip().split("\n")[2]
                        self.fld += ida
                        del ida
                        gc.collect()

                        if self.cstr < self.nfl:
                            self.stage -= 1

                            if self.cstr > 1:
                                cms[12] = (
                                        "AT+HTTPREAD="
                                        + str(self.cstr * self.rfr - 1)
                                        + ","
                                        + str(self.cstr * self.rfr + (self.rfr))
                                        + "\r"
                                )

                            else:
                                cms[12] = (
                                        "AT+HTTPREAD="
                                        + str(self.cstr * self.rfr)
                                        + ","
                                        + str(self.cstr * self.rfr + (self.rfr))
                                        + "\r"
                                )
                            self.cstr += 1

                        else:
                            self.pgd(self.fld)
                            cms[12] = "AT+HTTPREAD\r"
                            self.fld = ""
                            self.nfl = 0
                            self.lnt = 0
                            self.cstr = 1
                    except:
                        pass
                self.pt = time.time()
                self.stage += 1

                if self.stage == cl:
                    self.stage = 0
                    
                if self.stage == (cl-5) or self.stage == (cl-6):
                    db = self.rj()
                    err = list()
                    
                    if self.reboot:
                        if self.ecu <= 1:
                            err.append(900)
                        self.ecu+=1
                        
                    if self.ecu > 2:
                        err.append(800)
                    
                    if self.iml:
                        err.append(700)
                    
                    bdy = json.dumps({
                                    "apiKey": db["apiKey"],
                                    "deposits": db["transactions"],
                                    "production": db["production"],
                                    "error": err,
                                    "response" : db["response"]
                                    })
                    
                    if self.stage == (cl-5):
                        self.gw(bdy+"\r") 
                    else:
                        self.gw("AT+HTTPDATA=" + str(len(bdy))+ ",100000\r")

                    del bdy,db
                else:
                    self.gw(cms[self.stage])
                    
                del x
                gc.collect()
            del (cms, cd, cl)
            gc.collect()
            self.dc.release()
        return 0
    
    def run(self):
        self.cv()
#         tm = Pin(22, Pin.IN, Pin.PULL_UP)
#         tm.irq(trigger=Pin.IRQ_FALLING, handler=self.tmp)
        while 1:
            self.dc.acquire()
            self.wdt.feed()
            (stat, tag_type) = self.rd.request(self.rd.REQIDL)
            if stat == self.rd.OK:
                (stat, uid) = self.rd.SelectTagSN()
                if stat == self.rd.OK:
                    if not self.iml:
                        crd = str(int.from_bytes(bytes(uid), "little", False))
                        db = self.rj()
                        unts = 0
                        ppl = db["ppl"]
                        if crd in db["users"]:
                            slo = db["users"][crd]
                            self.lbo()
                            if (slo - 0.2) > 0:
                                self.lmt(0, 0)
                                self.pts("Kadi  " + str(crd))
                                self.lmt(0, 1)
                                self.pts("salio " + str(slo) + "L")
                                self.tsp(2)
                                self.ov()
                                x = self.dw(slo, crd, db)
                                nuc = x[0]
                                sub = x[1]
                                unts = x[2]
                                db["production"] = db["production"] + (slo - nuc) - sub
                                self.cv()
                                db["users"][crd] = nuc
                                db["transactions"][crd] = nuc
                                db["transactions"] = dict(
                                    sorted(db["transactions"].items())
                                )
                                swj = self.wj(db)
                                del (x, swj, nuc, db, sub)
                                gc.collect()

                            else:
                                self.lmt(0, 0)
                                self.pts("Kadi  " + str(crd))
                                self.lmt(0, 1)
                                self.pts("SALIO HALITOSHI!")
                                self.tsp(1.5)

                            del slo
                            self.lcr()
                            self.lmt(0, 0)
                            self.pts("MAJI: " + str("{:.1f}".format(unts)) + "L")
                            self.lmt(0, 1)
                            self.pts("PESA: " + str("{:.1f}".format(unts*ppl)) + "TZS")
                            self.tsp(3)
                            self.lcr()
                            self.lmt(0, 0)
                            self.pts("   KARIBU TENA   ")
                            self.lmt(0, 1)
                            self.pts("     ASANTE      ")
                            self.tsp(1)
                            self.lcr()
                            self.lbf()

                        else:
                            self.lbo()
                            self.lcr()
                            self.pts("Kadi " + str(crd))
                            self.lmt(0, 1)
                            self.pts("HAIJASAJILIWA")
                            self.tsp(3)
                            self.lcr()
                            self.lbf()
                        del crd
                        gc.collect()

                    else:
                        self.lbo()
                        self.lcr()
                        self.lmt(0, 1)
                        self.pts(" MITA IMEFUNGWA")
                        self.tsp(3)
                        self.lcr()
                        self.lbf()

                del (stat, uid)
                gc.collect()
            del tag_type
            gc.collect()
            self.dc.release()
        return 0

meter = Meter()
_thread.start_new_thread(meter.get_gprs, ())
meter.run()