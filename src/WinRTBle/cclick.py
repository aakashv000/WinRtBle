#!/usr/bin/python2.7.6

__credits__=  "Dr.Anil prabhakar"
__date__=     " April/June 2018"
#######################Description #####################
'''
    This Class room clicker software is designed for interactive class room learning, This software is developed for getting data from msp430
    microcontroller through RF module and plot bar graph in GUI. This code is for Windows 10 supporting BLE.

'''
################Loding Modules###########################
import os
# import pprint
# import random
import time
import wx
# import sys
# import matplotlib.pyplot as plt
# import serial
import numpy as np
# import matplotlib
from pylab import figure, table, show, axis, title
#matplotlib.use('WXAgg')
from matplotlib.figure import Figure
from matplotlib.backends.backend_wxagg import \
    FigureCanvasWxAgg as FigCanvas, \
    NavigationToolbar2WxAgg as NavigationToolbar

###########################modified  ###########
#import Tkinter
#from Tkinter import *
#import thread
#from threading import Thread
# import traceback
#import tkMessageBox
# from bluepy.btle import Scanner, DefaultDelegate
# from bluepy.btle import UUID, Peripheral
# import time
#from tkFileDialog import askopenfilename
# import os, sys
import subprocess as sp


tot_devices=['e6:c6:15:3d:db:df','dc:8a:19:f8:14:aa', 'f4:36:67:20:f8:6d','fc:77:48:ab:85:c6','ee:46:b2:5f:3d:c4']
tot={'e6:c6:15:3d:db:df':'Click1','dc:8a:19:f8:14:aa':'Click2','f4:36:67:20:f8:6d':'Click3','fc:77:48:ab:85:c6':'Click4','ee:46:b2:5f:3d:c4':'Click5'} ##Name for each Device
found_devices=[]
avail_devices = []
#root=Tk()
global answer




def bt_add_from_ull( device_address_ull ):
    device_address_hex = hex( device_address_ull )[ 2: ]        # removed the '0x' from left end of hex address
    # print( device_address_hex )

    # device_address -> address of device in bluetooth address format as required
    device_address = str()
    for i in range( int( len(device_address_hex) / 2 ) ):
        device_address += device_address_hex[ i*2 : i*2 + 2 ] + ':'
    device_address = device_address[ : -1 ]                         # removed the extra ':' from right end
    # print( device_address )
    return device_address


def ull_from_bt_add( bt_add ):
    hex_add = bt_add.replace( ':', '' )
    ull_add = int( hex_add, 16 )
    return ull_add




class BarsFrame(wx.Frame):                                        #Creating bar graph frame for 20 users As of now we have 5 devices only
    title = 'Class Room Clicker'
    def btscan(self,event):
        print('scan_button_pressed')
        global avail_devices
        global found_devices

        ############################################################################
        # class ScanDelegate(DefaultDelegate):
        #     def _init_(self):
        #         DefaultDelegate._init_(self)
        #     def handleDiscovery(self, dev, isNewDev, isNewData):
        #         if isNewDev:
        #             print ("Discovered device", dev.addr)
        #         elif isNewData:
        #             print ("Received new data from", dev.addr )
        # scanner = Scanner().withDelegate(ScanDelegate())
        # devices = scanner.scan(10.0)
        # for dev in devices:
        #     for (adtype, desc, value) in dev.getScanData():
        #         #print ("  %s = %s" % (desc, value) )
        #         if 'RFduino' in value:      #Filter Only RFduino Device
        #             found_devices.append(dev.addr)

        discover_exe = 'modules_exe\\discover.exe'
        child_process = sp.Popen( discover_exe, stdout = sp.PIPE )
        output_bytes = child_process.communicate()[0]
        # print( output_bytes )
        output_str = output_bytes.decode('utf-8')
        # print( output_str )

        # split into separate device addresses
        device_list_str = output_str.splitlines()
        print( device_list_str )

        device_list_ull = [ int( i ) for i in device_list_str ]         # list of device addresses in unsigned long long format

        found_devices = [ bt_add_from_ull( i ) for i in device_list_ull ]

        #####################################################

        print('Found Devices are :',found_devices )
        print('Total Devices list are \n',tot_devices )
        # print('scanned devices list are \n',found_devices )
        avail_devices = list( set(tot_devices) & set(found_devices) )
        print('Available Devices are \n',avail_devices )
        wx.MessageBox('Scan completed', 'Info',
                wx.OK | wx.ICON_INFORMATION)


    def __init__(self):#Initialize variables
        wx.Frame.__init__(self, None, -1, self.title)

        #Labels creat for table

        self.labelc = [ 'A', 'B', 'C', 'D', 'LOP','Vmon']
        self.labelr = ['ID', '1', '2', '3', '4', '5', '6', '7', '8', '9',
              '10', '11', '12', '13', '14', '15', '16', '17', '18', '19','20']
        self.chars = [['' for c in range(6)] for r in range(21)]
        self.colors = [[(0.95, 0.95, 0.95) for c in range(6)] for r in range(21)]
        self.qqq=1


        self.wloop=1
        self.totg = [0, 0, 0, 0]
        self.ao=[0]*20
        self.bo=[0]*20
        self.co=[0]*20
        self.do=[0]*20
        self.vv=[0]*20
        self.lastopt=[0]*20


        self.create_menu()
        self.create_status_bar()
        self.create_main_panel()
        self.textbox.SetValue(' '.join(map(str, 't(sec)')))
        self.textbox2.SetValue(' '.join(map(str, 'question')))



        self.draw_figure()

    def serread(self,event):#creating file,read till timeout and write to CRC.log file
        self.logfile=open("CRC.log","a")                   #log file is created
        self.logfile.write("\n##")
        self.logfile.write( time.strftime("%d/%m/%Y"))
        self.logfile.write("   ")

        self.logfile.write( time.strftime("%H:%M:%S"))
        self.logfile.write("##\n")
        self.logfile.write("#Q")
        self.logfile.write(str(self.qqq))
        self.logfile.write(":")

        self.logfile.write(self.textbox2.GetValue())
        self.logfile.write("?")
        #print ((time.strftime("%d/%m/%Y")))
        #print ((time.strftime("%H:%M:%S")))
        num_user = float(self.textbox.GetValue())
        #global ser
        #self.question=self.textbox2.GetValue()
        #print(self.question)
        global avail_devices
        try:


            ####################################
            print ( avail_devices )
        except Exception:
            answer=0
        except:
            print ("Device was not connected")

        timeout = int(time.time()) + int(num_user)
        while(self.wloop==1):
            if(time.time()>timeout):
                self.wloop=0
            else:
            #    print (avail_devices )
                for i in range(len(avail_devices)):
                    # print (avail_devices[i])
                    #####################################################
                    # read_uuid = UUID(0x2221)
                    # write_wuuid = UUID(0x2222)
                    # p = Peripheral(avail_devices[i],"random")
                    # ch = p.getCharacteristics(uuid=read_uuid)[0]
                    # chw = p.getCharacteristics(uuid=write_wuuid)[0]

                    # val=0
                    # if (ch.supportsRead()):
                    #     val=0
                    #     answer=''
                    #     val=chw.write('ff') #command for read the data from the device.
                    #     time.sleep(2)
                    #     val = ch.read()
                    #     value = ord(val)

                    ##### NOTE: as coded in exe file, the exe sleeps for 1 sec

                    answer = ''

                    address_ull = ull_from_bt_add( avail_devices[ i ] )

                    connectAndRead_exe = 'modules_exe\\ConnectAndRead.exe'
                    child_process = sp.Popen( [ connectAndRead_exe, str( address_ull ) ], stdout = sp.PIPE )
                    output_bytes = child_process.communicate()[0]
                    # print( output_bytes )
                    output_str = output_bytes.decode('utf-8')
                    # print( output_str )                       # is of type '3\r\n'

                    value = int( output_str )

                    ####################################

                    if value==1:
                        answer= 'A'
                    elif value==2:
                        answer= 'B'
                    elif value==3:
                        answer= 'C'
                    elif value==4:
                        answer= 'D'
                    else:
                        answer='0'
                    print (tot.get(avail_devices[i]),answer )



                    volt=float(4.4)/10
                    #self.data = [int(self.serdata[0:2]), str(self.serdata[2]),volt]
                    self.data = [int(i), str(answer),volt]   #Device name, Option, volt
                    #print (self.data[0] )
                    #print(type(self.data[1]))
                    if(self.data[1]=='A'):
                        self.ao[((self.data[0])-1)]=self.ao[((self.data[0])-1)]+1
                        self.lastopt[((self.data[0])-1)]=self.data[1]
                        self.vv[((self.data[0])-1)]=self.data[2]

                    if(self.data[1]=='B'):
                        self.bo[((self.data[0])-1)]=self.bo[((self.data[0])-1)]+1
                        self.lastopt[((self.data[0])-1)]=self.data[1]
                        self.vv[((self.data[0])-1)]=self.data[2]

                    if(self.data[1]=='C'):
                        self.co[((self.data[0])-1)]=self.co[((self.data[0])-1)]+1
                        self.lastopt[((self.data[0])-1)]=self.data[1]
                        self.vv[((self.data[0])-1)]=self.data[2]

                    if(self.data[1]=='D'):
                        self.do[((self.data[0])-1)]=self.do[((self.data[0])-1)]+1
                        self.lastopt[((self.data[0])-1)]=self.data[1]
                        self.vv[((self.data[0])-1)]=self.data[2]

        self.atot=sum(self.ao)
        self.btot=sum(self.bo)
        self.ctot=sum(self.co)
        self.dtot=sum(self.do)
        self.totg=[self.atot,self.btot,self.ctot,self.dtot]
        #self.ser.flushInput()
        #self.ser.close()
        self.draw_figure()
        #print('10sec')
        print(self.bo)
                #print ("Reopen this gui" )
        for kkkk in range(0,20):
            lev=kkkk
            self.logfile.write("\n")
            self.logfile.write(str(lev+1))
            self.logfile.write(",")
            self.logfile.write(str(self.lastopt[kkkk]))
        #self.logfile.write("\n")


        self.logfile.close()
        self.qqq=self.qqq+1

    def create_menu(self):###Menu bar is created
        self.menubar = wx.MenuBar()

        menu_file = wx.Menu()
        m_expt = menu_file.Append(-1, "&Save plot\tCtrl-S", "Save plot to file")
        self.Bind(wx.EVT_MENU, self.on_save_plot, m_expt)
        menu_file.AppendSeparator()
        m_exit = menu_file.Append(-1, "E&xit\tCtrl-X", "Exit")
        self.Bind(wx.EVT_MENU, self.on_exit, m_exit)

        menu_help = wx.Menu()
        m_about = menu_help.Append(-1, "&About\tF1", "About the classe room clicker")
        self.Bind(wx.EVT_MENU, self.on_about, m_about)

        self.menubar.Append(menu_file, "&File")
        self.menubar.Append(menu_help, "&Help")
        self.SetMenuBar(self.menubar)

    def create_main_panel(self):# Crearing the elements of the GUI

        self.panel = wx.Panel(self)

        # Create the mpl Figure and FigCanvas objects.
        # 5x4 inches, 100 dots-per-inch
        #
        self.dpi = 100
        self.fig = Figure((5.0, 4.0), dpi=self.dpi)
        self.canvas = FigCanvas(self.panel, -1, self.fig)
        self.textbox2 = wx.TextCtrl(
            self.panel,
            size=(900,-1),
            style=wx.TE_PROCESS_ENTER)

        self.axes = self.fig.add_subplot(111)

        # Bind the 'pick' event for clicking on one of the bars
        #
        self.canvas.mpl_connect('pick_event', self.on_pick)



        #self.Bind(wx.EVT_TEXT_ENTER, self.on_text_enter, self.textbox2)
        self.drawbutton1 = wx.Button(self.panel, -1, "Scan")
        self.Bind(wx.EVT_BUTTON,  self.btscan, self.drawbutton1)
        self.textbox = wx.TextCtrl(
            self.panel,
            size=(60,-1),
            style=wx.TE_PROCESS_ENTER)
        self.Bind(wx.EVT_TEXT_ENTER, self.on_text_enter, self.textbox)
        self.drawbutton = wx.Button(self.panel, -1, "Start")
        self.Bind(wx.EVT_BUTTON, self.serread, self.drawbutton)
        self.attbutton = wx.Button(self.panel, -1, "Init")
        self.Bind(wx.EVT_BUTTON, self.init, self.attbutton)
        self.donebutton = wx.Button(self.panel, -1, "Result")
        self.Bind(wx.EVT_BUTTON, self.result, self.donebutton)
        self.closebutton = wx.Button(self.panel, -1, "Close")
        self.Bind(wx.EVT_BUTTON, self.closebtt, self.closebutton)
        self.plotlopbut = wx.Button(self.panel, -1, "L_opt")
        self.Bind(wx.EVT_BUTTON, self.plotlop, self.plotlopbut)
        self.totbt = wx.Button(self.panel, -1, "Total")
        self.Bind(wx.EVT_BUTTON, self.totfun, self.totbt)
        #self.cb_grid = wx.CheckBox(self.panel, -1,
        #    "Show Grid",
        #    style=wx.ALIGN_RIGHT)
        #self.Bind(wx.EVT_CHECKBOX, self.on_cb_grid, self.cb_grid)

        #self.slider_label = wx.StaticText(self.panel, -1,
        #    "width")
        #self.slider_width = wx.Slider(self.panel, -1,
        #    value=20,
        #    minValue=1,
        #    maxValue=100,
        #   style=wx.SL_AUTOTICKS | wx.SL_LABELS)
        #self.slider_width.SetTickFreq(10, 1)
        #self.Bind(wx.EVT_COMMAND_SCROLL_THUMBTRACK, self.on_slider_width, self.slider_width)




        self.toolbar = NavigationToolbar(self.canvas)



        #
        # Layout with box sizers
        #
        self.qbox = wx.BoxSizer(wx.HORIZONTAL)
        self.vbox = wx.BoxSizer(wx.VERTICAL)
        flags = wx.ALIGN_LEFT | wx.ALL | wx.ALIGN_CENTER_VERTICAL

        self.vbox.Add(self.canvas, 1, wx.TOP | wx.GROW)
        self.vbox.Add(self.toolbar, 0, wx.EXPAND)
        self.vbox.AddSpacer(10)

        self.hbox = wx.BoxSizer(wx.HORIZONTAL)


        self.qbox.AddSpacer(10)

        self.hbox.Add(self.drawbutton1, 0, border=3, flag=flags)
        self.hbox.Add(self.textbox, 0, border=3, flag=flags)
        self.hbox.Add(self.drawbutton, 0, border=3, flag=flags)
        self.hbox.Add(self.donebutton, 0, border=3, flag=flags)
        self.hbox.Add(self.totbt, 0, border=3, flag=flags)




        self.hbox.Add(self.plotlopbut, 0, border=3, flag=flags)
        self.hbox.Add(self.attbutton, 0, border=3, flag=flags)
        self.hbox.Add(self.closebutton, 0, border=3, flag=flags)
        self.qbox.Add(self.textbox2, 0, border=3, flag=flags)


        #self.hbox.Add(self.cb_grid, 0, border=3, flag=flags)
        self.hbox.AddSpacer(30)
        #self.hbox.Add(self.slider_label, 0, flag=flags)
        #self.hbox.Add(self.slider_width, 0, border=3, flag=flags)

        self.vbox.Add(self.hbox, 0, flag = wx.ALIGN_LEFT | wx.TOP)
        self.vbox.Add(self.qbox, 0, flag = wx.ALIGN_LEFT | wx.TOP)
        self.panel.SetSizer(self.vbox)
        self.vbox.Fit(self)

    def totfun(self,event):#Button event for raw option
        self.draw_figure()
    def init(self,event):# To reuse the GUI Every variables need to be reinit
        self.chars = [['' for c in range(6)] for r in range(21)]
        self.wloop=1
        self.totg = [0, 0, 0, 0]
        self.ao=[0]*20
        self.bo=[0]*20
        self.co=[0]*20
        self.do=[0]*20
        self.vv=[0]*20
        self.lastopt=[0]*20


        app.frame = BarsFrame()
        print ("att")
        x = range(4)
        #xy=['a','b','c','d']
        ind = np.arange(4)
        width=0
        self.totg=[0,0,0,0]
        # clear the axes and redraw the plot anew
        #
        self.axes.clear()
        #self.axes.xticks(range(len(xy)),xy,size='small')
        #self.axes.grid(self.cb_grid.IsChecked())

        self.axes.bar(left=x,height=self.totg,
            width=1,
            align='center',color='y')
        self.axes.set_title('Histogram')
        self.axes.set_xlabel('Options')
        self.axes.set_ylabel('level')
        self.axes.set_xticks(ind+width)
        self.axes.set_xticklabels( ('A', 'B', 'C', 'D') )
        #self.ser.close()
        #ab=self.axes.set_xticklabels(['.','A', '.', 'B','.','C', '.','D'])

        self.canvas.draw()
    def plotlop(self,event):# To find the last option given by all the 20 users
        print('enter')
        x = range(4)
        self.lastoptA=0
        self.lastoptB=0
        self.lastoptC=0
        self.lastoptD=0
        #xy=['a','b','c','d']
        ind = np.arange(4)
        width=0

        # clear the axes and redraw the plot anew
        #
        self.axes.clear()


        for ii in range(0,20):
            if(self.lastopt[ii]=='A'):
                self.lastoptA=self.lastoptA+1
            if(self.lastopt[ii]=='B'):
                self.lastoptB=self.lastoptB+1
            if(self.lastopt[ii]=='C'):
                self.lastoptC=self.lastoptC+1
            if(self.lastopt[ii]=='D'):
                self.lastoptD=self.lastoptD+1

        self.lastoptt=[self.lastoptA,self.lastoptB,self.lastoptC,self.lastoptD]

        #self.axes.xticks(range(len(xy)),xy,size='small')
        #self.axes.grid(self.cb_grid.IsChecked())

        self.axes.bar(left=x,height=self.lastoptt, width=.35, align='center',color='y')
        self.axes.set_title('Histogram')
        self.axes.set_xlabel('Options')
        self.axes.set_ylabel('level')
        self.axes.set_xticks(ind+width)
        self.axes.set_xticklabels( ('A', 'B', 'C', 'D') )

        #ab=self.axes.set_xticklabels(['.','A', '.', 'B','.','C', '.','D'])

        self.canvas.draw()

    def closebtt(self,event):# Closs serial port and other events
        #global ser
        self.Close(True)
        #self.ser.close()


    def create_status_bar(self):#status bar
        self.statusbar = self.CreateStatusBar()

    def draw_figure(self):#Redraws the figure


        #str = self.textbox.GetValue()
        #self.data = map(int, str.split())

        x = range(4)
        #xy=['a','b','c','d']
        ind = np.arange(4)
        width=0

        # clear the axes and redraw the plot anew
        #
        self.axes.clear()
        #self.axes.xticks(range(len(xy)),xy,size='small')
        #self.axes.grid(self.cb_grid.IsChecked())

        self.axes.bar(left=x,height=self.totg, width=.35,  align='center',color='y')
        self.axes.set_title('Histogram')
        self.axes.set_xlabel('Options')
        self.axes.set_ylabel('level')
        self.axes.set_xticks(ind+width)
        self.axes.set_xticklabels( ('A', 'B', 'C', 'D') )

        #ab=self.axes.set_xticklabels(['.','A', '.', 'B','.','C', '.','D'])
        # global ser
        self.canvas.draw()



    def on_cb_grid(self, event):# Grid draw
        self.draw_figure()

    def on_slider_width(self, event):#slider
        self.draw_figure()

    def on_draw_button(self, event):# draw button
        self.figure(figsize=(8, 4), dpi=120)
        #self.data = [3, 1, 0, 1]
        self.draw_figure()
    def result(self, event):# Present the result of 20 user in a table
        #print ("result" )
        for x3 in range(0, 20):

            self.chars[x3+1][0] = self.ao[x3]
        for xx in range(0, 20):

            self.chars[xx+1][1] = self.bo[xx]

        for yy in range(0, 20):

            self.chars[yy+1][2] = self.co[yy]

        for zz in range(0, 20):

            self.chars[zz+1][3] = self.do[zz]

        for x1 in range(0, 20):

            self.chars[x1+1][4] = self.lastopt[x1]

        for x4 in range(0, 20):

            self.chars[x4+1][5] = self.vv[x4]

        for x5 in range(0, 20):
            if(self.vv[x5]>=2.6):

                self.colors[x5+1][5]=(0, 1, 0)
            elif((self.vv[x5]<2.6) and (self.vv[x5]>1)):
                self.colors[x5+1][5]=(1, 0, 0)
            else:
                self.colors[x5+1][5]=(.95, .95, 0.95)



        #for x2 in range(0, 20):

        #    self.chars[x2][5] = self.lastopt[x2]
        #self.chars[19][4] = '0'

        lightgrn = (0.5, 0, .9)
        title('Result')
        tab = table(cellText=self.chars,
                    rowLabels=self.labelr,
                    colLabels=self.labelc,
                    rowColours=[lightgrn]*21,
                    colColours=[lightgrn]*6,
                    cellColours=self.colors,
                    cellLoc='center',
                    loc='upper left')


        axis('off')
        show()

    def on_pick(self, event):#Pick single bar for info

        box_points = event.artist.get_bbox().get_points()
        msg = "You've clicked on a bar with coords:\n %s" % box_points

        dlg = wx.MessageDialog(
            self,
            msg,
            "Click!",
            wx.OK | wx.ICON_INFORMATION)

        dlg.ShowModal()
        dlg.Destroy()

    def on_text_enter(self, event):# This function will be called when the text event occer to display in GUI
        self.draw_figure()

    def on_save_plot(self, event):# To save the plot
        file_choices = "PNG (*.png)|*.png"

        dlg = wx.FileDialog(
            self,
            message="Save plot as...",
            defaultDir=os.getcwd(),
            defaultFile="plot.png",
            wildcard=file_choices,
            style=wx.SAVE)

        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            self.canvas.print_figure(path, dpi=self.dpi)
            self.flash_status_message("Saved to %s" % path)

    def on_exit(self, event):#  Exit button
        self.Destroy()

    def on_about(self, event):#Help menu
        msg = """ Class Room Clicker:

         * This Application is created for intrective class room learning


        """
        dlg = wx.MessageDialog(self, msg, "About", wx.OK)
        dlg.ShowModal()
        dlg.Destroy()

    def flash_status_message(self, msg, flash_len_ms=1500):# Life time of status message
        self.statusbar.SetStatusText(msg)
        self.timeroff = wx.Timer(self)
        self.Bind(
            wx.EVT_TIMER,
            self.on_flash_status_off,
            self.timeroff)
        self.timeroff.Start(flash_len_ms, oneShot=True)

    def on_flash_status_off(self, event):
        self.statusbar.SetStatusText('')


if __name__ == '__main__':
    app = wx.App()
    app.frame = BarsFrame()
    app.frame.Show()
    app.MainLoop()
