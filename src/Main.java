/**
 *  Copyright 2012 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of the SMART
 *
 *  SMART is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SMART is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SMART. If not, see <http://www.gnu.org/licenses/>.
 */
 
package smart;

import java.awt.Point;
import java.io.*;
import java.nio.*;
import java.nio.channels.*;
import java.net.*;

/**
 
int port, width, height, paired, imgoff, dbgoff;
char args[4096];
...imagedata...

**/


public class Main {

    private static final int FirstFunc =          1;
    private static final int getRefresh =         FirstFunc+0;
    private static final int setRefresh =         FirstFunc+1;
    private static final int setTransparentColor= FirstFunc+2;
    private static final int setDebug =           FirstFunc+3;
    private static final int setGraphics =        FirstFunc+4;
    private static final int setEnabled =         FirstFunc+5;
    private static final int isActive =           FirstFunc+6;
    private static final int isBlocking =         FirstFunc+7;
    private static final int getMousePos =        FirstFunc+8;
    private static final int holdMouse =          FirstFunc+9;
    private static final int releaseMouse =       FirstFunc+10;
    private static final int holdMousePlus =      FirstFunc+11;
    private static final int releaseMousePlus =   FirstFunc+12;
    private static final int moveMouse =          FirstFunc+13;
    private static final int windMouse =          FirstFunc+14;
    private static final int clickMouse =         FirstFunc+15;
    private static final int clickMousePlus =     FirstFunc+16;
    private static final int isMouseButtonHeld =  FirstFunc+17;
    private static final int sendKeys =           FirstFunc+18;
    private static final int holdKey =            FirstFunc+19;
    private static final int releaseKey =         FirstFunc+20;
    private static final int isKeyDown =          FirstFunc+21;

    private static final int ExtraFuncs =         FirstFunc+22;
    private static final int Ping =               ExtraFuncs+0;
    private static final int Die =                ExtraFuncs+1;

    private static int id, port;
    private static ByteBuffer mem;
    private static ByteBuffer args;
    private static Client client;
    
    public static void setPort(int port) {
        mem.putInt(0*4,port);
    }
    
    public static void setID(int id) {
        mem.putInt(1*4,id);
    }
    
    public static int getPaired() {
        return mem.getInt(4*4);
    }
    
    public static void setPaired(int tid) {
        mem.putInt(4*4,tid);
    }
    
    public static void setImgOff(int off) {
        mem.putInt(5*4,off);
    }
    
    public static void setDbgOff(int off) {
        mem.putInt(6*4,off);
    }
    
    public static void setDims(int width, int height) {
        mem.putInt(2*4,width);
        mem.putInt(3*4,height);
    }
    
    public static native boolean checkAlive(int tid);
    
    public static native int getPID();
    
    public static native void copyGLBuffer(int x, int y, int w, int h, IntBuffer dest);
    
    public static void debug(Object wat) {
        System.out.println("SMART["+id+"]: " + wat);
    }
    
    private static int handle(int funid) {
        //debug("FunID: " + funid);
        switch (funid) {
            case getRefresh:
                args.putInt(0*4,client.getRefresh());
                break;
            case setRefresh:
                client.setRefresh(args.getInt(0*4));
                break;
            case setTransparentColor:
                client.transColor = args.getInt(0*4);
                break;
            case setDebug:
                client.setDebug(args.getInt(0*4)!=0);
                break;
            case setGraphics:
                client.setGraphics(args.getInt(0*4)!=0);
                break;
            case setEnabled:
                client.setEnabled(args.getInt(0*4)!=0);
                break;
            case isActive:
                args.putInt(0*4, client.active ? 1 : 0);
                break;
            case isBlocking:
                args.putInt(0*4, client.blocking ? 1 : 0);
                break;
            case getMousePos: {
                Point pt = client.getMousePos();
                args.putInt(0*4,pt.x);
                args.putInt(1*4,pt.y);
                } break;
            case holdMouse:
                client.holdMouse(args.getInt(0*4),args.getInt(1*4),args.getInt(2*4)!=0 ? 1 : 3);
                break;
            case releaseMouse:
                client.releaseMouse(args.getInt(0*4),args.getInt(1*4),args.getInt(2*4)!=0 ? 1 : 3);
                break;
            case holdMousePlus:
                client.holdMouse(args.getInt(0*4),args.getInt(1*4),args.getInt(2*4));
                break;
            case releaseMousePlus:
                client.releaseMouse(args.getInt(0*4),args.getInt(1*4),args.getInt(2*4));
                break;
            case moveMouse:
                client.moveMouse(args.getInt(0*4),args.getInt(1*4));
                break;
            case windMouse:
                client.windMouse(args.getInt(0*4),args.getInt(1*4));
                break;
            case clickMouse:
                client.clickMouse(args.getInt(0*4),args.getInt(1*4),args.getInt(2*4)!=0 ? 1 : 3);
                break;
            case clickMousePlus:
                client.clickMouse(args.getInt(0*4),args.getInt(1*4),args.getInt(2*4));
                break;
            case isMouseButtonHeld:
                args.putInt(0*4, client.isMouseButtonHeld(args.getInt(0*4)) ? 1 : 0);
                break;
            case sendKeys: {
                StringBuilder keys = new StringBuilder();
                byte c;
                args.position(8);
                while ((c=args.get()) != 0) keys.append((char)c);
                client.sendKeys(keys.toString(),args.getInt(0*4),args.getInt(1*4));
                } break;
            case holdKey:
                client.holdKey(args.getInt(0*4));
                break;
            case releaseKey:
                client.releaseKey(args.getInt(0*4));
                break;
            case isKeyDown:
                args.putInt(0*4,client.isKeyDown(args.getInt(0*4)) ? 1 : 0);
                break;
            case Ping:
                break;
            case Die:
                cleanup();
                break;
            default:
                return -1;
        }
        return funid;
    }
    
    public static void main(String[] exec_args) {
        if (exec_args.length != 9) System.exit(1);
         
        String path = new File(exec_args[0]).getAbsolutePath();
        System.load(path);
        String root = exec_args[1];
        String params = exec_args[2];
        int width = Integer.parseInt(exec_args[3]);
        int height = Integer.parseInt(exec_args[4]);
        String initseq = exec_args[5];
        String useragent = exec_args[6];
		
		String pluginspath = exec_args[7];
		String pluginsinfo = exec_args[8];
		
		if (pluginsinfo.length() > 0) {	
		    String[] pluginslist = pluginsinfo.split("[,]+");
			for (int i = 0; i < pluginslist.length; i++) {
				File pluginsfile = new File(pluginspath + "/" + pluginslist[i].trim());
				if (pluginsfile.isFile()) {
					System.load(pluginsfile.getAbsolutePath());
					debug("Smart_"+pluginslist[i].trim()+" Successfully Loaded!\n");
				} else {
					debug("File Missing: Smart_"+pluginslist[i].trim()+" Failed To Load!\n");
				}
			}
		}
		
        
        int nVars = 7;
        int argsSpace = 4096;
        int imgLen = width*height*4;
        
        int size = nVars*4 + argsSpace + imgLen*2;
        int imgoff = nVars*4 + argsSpace;
        int dbgoff = imgoff + imgLen;
        
        try {
            id = getPID();
            debug("Starting remote " + id);
            File f = new File("SMART."+id);
            if (f.exists()) f.delete();
            f.createNewFile();
            f.deleteOnExit();
            
            RandomAccessFile fra = new RandomAccessFile(f,"rwd"); //possibly just rw, might need rws
            fra.setLength(size);
            fra.getChannel();
            
            FileChannel chan = fra.getChannel();
            mem = chan.map(FileChannel.MapMode.READ_WRITE,0,size);
            mem.order(ByteOrder.LITTLE_ENDIAN);
            mem.position(nVars*4);
            args = mem.slice();
            args.order(ByteOrder.LITTLE_ENDIAN);
            setImgOff(imgoff);
            mem.position(imgoff);
            ByteBuffer img = mem.slice();
            setDbgOff(dbgoff);
            mem.position(dbgoff);
            ByteBuffer dbg = mem.slice();
            mem.rewind();
            
            setDims(width,height);
            
            ServerSocket listen = new ServerSocket(0);
            port = listen.getLocalPort();
            
            setID(id);
            setPort(port);
            setPaired(0);
            
            client = new Client(img,dbg,width,height,root,params,initseq,useragent,id);
            
            while (client.active) {
                Socket ctrl = listen.accept();
                debug("Socket connected");
                ctrl.setSoTimeout(500);
                InputStream in = ctrl.getInputStream();
                OutputStream out = ctrl.getOutputStream();
                debug("Remote TID: "+getPaired());
                while (getPaired() != 0 && checkAlive(getPaired())) {
                    try {
                        out.write(handle(in.read()));
                    } catch (SocketTimeoutException e) { 
                    } catch (Exception e) {
                        e.printStackTrace();
                        break;
                    }
                }
                debug("Socket disconnected; unpairing");
                setPaired(0);
                in.close();
                out.close();
                ctrl.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    public static void cleanup() {
        System.exit(0);
    }
    
}
