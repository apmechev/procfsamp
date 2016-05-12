import re
import sys
import matplotlib.pyplot as plt
import numpy as np


class ProfData:
        def __init__(self):
            self.timestep=1000
            self.p_name="default"
            self.s_time=""


def main(ofile):
    io=[]
    mem=[]
    stat=[]
    outfile= open(ofile,'r')
    for line in outfile:
        if re.search("proc-io",line) is not None:
            io.append(line.split("$proc-io")[1].split())
        elif re.search("proc-mem",line) is not None:
            mem.append(line.split("$proc-mem")[1].split())
        elif re.search("proc-stat",line) is not None:
            stat.append(line.split("$proc-stat")[1].split())
    return np.asarray(io),np.asarray(mem,dtype='<U16'),np.asarray(stat)


def plot_all(fname):
    import os
    jiffytime=float(os.sysconf(os.sysconf_names['SC_CLK_TCK']))
    io,mem,stat=main(fname)
    print len(io),len(mem),len(stat)
    plt.figure(1)
    plt.subplot(111)
    x=np.arange(0,len(io[:,5])*100/1000.,100/1000.)
    rb,=plt.plot(x,io[:,4],'r')
    wb,=plt.plot(x,io[:,5],'k')
    plt.legend([rb,wb],["I/O Read Bytes","I/O Write Bytes"])
    plt.title("Read and Writen Bytes to Disk")
    plt.xlabel("Time in seconds")
    plt.show()
    rc,=plt.plot(x,io[:,0],'r')
    wc,=plt.plot(x,io[:,1],'k')
    plt.legend([rc,wc],["I/O Read_Char","I/O Write_Char"])
    plt.title("Requested Read and Write Chars by program")
    plt.xlabel("Time in seconds")
    plt.show()
    plt.plot(x,stat[:,5],'b')
    plt.xlabel("Time in Seconds")
    plt.ylabel("Number of Threads")
    plt.title("Number of Threads Spawned")
    plt.show()
    for i in range(len(mem)):
        mem[i,0]=float(mem[i,0])*4.
        mem[i,1]=float(mem[i,1])*4.
        mem[i,5]=float(mem[i,5])*4.
    fix,ax1=plt.subplots()
    msz,=ax1.plot(x,mem[:,0],"r")
    mrs,=ax1.plot(x,mem[:,1],"k")
    ds,=ax1.plot(x,mem[:,5],"g")
    plt.ylabel("Size in kB")

    ax2 = ax1.twinx()
    sp,=ax2.plot(x,mem[:,2],"b")
    ax2.set_ylabel('Shared Pages', color='b')
    for tl in ax2.get_yticklabels():
            tl.set_color('b')
    plt.legend([msz,mrs,sp,ds],["VMSize","VMRSS","Shared Pages","Data+Stack"],loc=9)
    plt.xlabel("time in Seconds")
    plt.title("Virtual Memory Statistics")
    plt.show()
    for i in range(len(stat)):
        stat[i,3]=float(stat[i,3])*jiffytime
        stat[i,4]=float(stat[i,4])*jiffytime
    fix,ax1=plt.subplots()
    stm,=ax1.plot(x,stat[:,3],"r")
    ax2 = ax1.twinx()
    plt.ylabel("System time",color='r')


    utm,=ax2.plot(x,stat[:,4],"k")
    ax2.set_ylabel('UserTime', color='k')
    plt.legend([stm,utm],["System time","User time"])
    plt.show()
