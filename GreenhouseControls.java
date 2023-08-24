//: innerclasses/GreenhouseControls.java
// This produces a specific application of the
// control system, all in a single class. Inner
// classes allow you to encapsulate different
// functionality for each type of event.
// From 'Thinking in Java, 4th ed.' (c) Bruce Eckel 2005
// www.BruceEckel.com. See copyright notice in CopyRight.txt.

import java.io.*;
import java.util.Calendar;
import java.util.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class GreenhouseControls extends Controller implements Serializable{
  private static final long serialVersionUID = 1L;
  private boolean light = false;
  private boolean water = false;
  private boolean fans = false;
  private String thermostat = "Day";
  private String eventsFile = "examples1.txt";  
  private boolean windowok=true;
  private boolean poweron=true;
  private int errorcode=0;

  //prints a Boolean variable as ON or OFF
  public void PrintBoolean(boolean b){
    if(b)
    System.out.println("ON");
    else
    System.out.println("OFF");
  }
  //Prints current state of Object i.e values of all the variables
  public void PrintState(){
    System.out.print("light=");
    PrintBoolean(light);
    System.out.print("water=");
    PrintBoolean(water);
    System.out.print("fans=");
    PrintBoolean(fans);
    System.out.println("thermostat="+thermostat);
    System.out.print("window=");
    if(windowok)
    System.out.println("window okay");
    else
    System.out.println("window malfunction");
    System.out.print("power=");
    if(poweron)
    System.out.println("power on");
    else
    System.out.println("power out");
  }

  public class LightOn extends Event {
    public LightOn(long delayTime) { super(delayTime); }
    public void action() {
      // Put hardware control code here to
      // physically turn on the light.
      light = true;
    }
    public String toString() { return "Light is on"; }
  }
  public class LightOff extends Event {
    public LightOff(long delayTime) { super(delayTime); }
    public void action() {
      // Put hardware control code here to
      // physically turn off the light.
      light = false;
    }
    public String toString() { return "Light is off"; }
  }
  public class WaterOn extends Event {
    public WaterOn(long delayTime) { super(delayTime); }
    public void action() {
      // Put hardware control code here.
      water = true;
    }
    public String toString() {
      return "Greenhouse water is on";
    }
  }
  public class WaterOff extends Event {
    public WaterOff(long delayTime) { super(delayTime); }
    public void action() {
      // Put hardware control code here.
      water = false;
    }
    public String toString() {
      return "Greenhouse water is off";
    }
  }
  //Functionality added to control Fans
  public class FansOn extends Event {
    public FansOn(long delayTime) { super(delayTime); }
    public void action() {
        // Put hardware control code here to
        // physically turn on the Fans.
        fans = true;
      }
    public String toString() { return "Fans are on"; }
  }
  public class FansOff extends Event {
    public FansOff(long delayTime) { super(delayTime); }
    public void action() {
        // Put hardware control code here to
        // physically turn off the Fans.
        fans = false;
      }
    public String toString() { return "Fans are off"; }
  }
  public class ThermostatNight extends Event {
    public ThermostatNight(long delayTime) {
      super(delayTime);
    }
    public void action() {
      // Put hardware control code here.
      thermostat = "Night";
    }
    public String toString() {
      return "Thermostat on night setting";
    }
  }
  public class ThermostatDay extends Event {
    public ThermostatDay(long delayTime) {
      super(delayTime);
    }
    public void action() {
      // Put hardware control code here.
      thermostat = "Day";
    }
    public String toString() {
      return "Thermostat on day setting";
    }
  }
  // An example of an action() that inserts a
  // new one of itself into the event list:
  public class Bell extends Event {
    int rings;
    //paremetrized constructor with delayTime only
    public Bell(long delayTime){ super(delayTime); }
    //paremetrized constructor with delayTime and arbitary rings
    public Bell(long delayTime,int rings) { 
      super(delayTime); 
      this.rings=rings;
      long prev_time=delayTime;
      //add bell events with a delay of 2000ms each
      for(int i=1;i<rings;i++){
        prev_time=prev_time+2000;
        addEvent(new Bell(prev_time));
      }
    }
    public void action() {
    //do nothing
    }
    public String toString() { return "Bing!"; }
  }

  //detects and reports if a window malfunctions 
  public class WindowMalfunction extends Event {
    public WindowMalfunction(long delayTime) { super(delayTime); }
    public void action(){
        // Put hardware monitoring code here to monitor windows
        windowok = false;
        errorcode = 1;
        try{
          throw new ControllerException();
        }catch(ControllerException exception){
          exception.Handle();
        }
      }
    public String toString() { return "Window Malfunction"; }
  }

  //detects and reports if a Power supply shuts down 
  public class PowerOut extends Event {
    public PowerOut(long delayTime) { super(delayTime); }
    public void action() {
        // Put hardware monitoring code here to monitor Power
        poweron = false;
        errorcode = 2;
        try{
          throw new ControllerException();
        }catch(ControllerException exception){
          exception.Handle();
        }
      }
    public String toString() { return "Power is Out"; }
  }

  //errorccode getter function
  public int getError(){
    return errorcode;
  }

  //Fixes Power shutdown and logs status
  class PowerOn implements Fixable {
    public void fix(){
      //Hardware code to fix the power supply
      poweron=true;
      errorcode=0;
    }
    public void log(){
      try{
        //writing data to fix.log file and the console 
        Writer filewriter = new FileWriter("fix.log.txt");
        String line="fixed error code:"+String.valueOf(2)+"\n";
        filewriter.write(line);
        System.out.print(line);
        //log time
        DateFormat df = new SimpleDateFormat("dd/MM/yy HH:mm:ss");
        Calendar calobj = Calendar.getInstance();
        line=df.format(calobj.getTime());
        line=line+"\n";
        filewriter.write(line);
        System.out.print(line);
        line="Power Restored!\n";
        filewriter.write(line);
        System.out.print(line);
        filewriter.flush();
        filewriter.close();
      }catch(FileNotFoundException e){
        System.out.println("File not found Exception"); 
      }catch(IOException ioException){
        System.out.println("I/O Exception"); 
      }
    }
  }

  //Fixes Window Malfunction and logs status
  class FixWindow implements Fixable {
    public void fix(){
      //Hardware code to fix the window
      windowok=true;
      errorcode=0;
    }
    public void log(){
      try{
        //writing data to fix.log file and the console 
        Writer filewriter = new FileWriter("fix.log.txt");
        String line="fixed error code:"+String.valueOf(1)+"\n";
        filewriter.write(line);
        System.out.print(line);
        //log time
        DateFormat df = new SimpleDateFormat("dd/MM/yy HH:mm:ss");
        Calendar calobj = Calendar.getInstance();
        line=df.format(calobj.getTime());
        line=line+"\n";
        filewriter.write(line);
        System.out.print(line);
        line="Window Fixed!\n";
        filewriter.write(line);
        System.out.print(line);
        filewriter.flush();
        filewriter.close();
      }catch(FileNotFoundException e){
        System.out.println("File not found Exception"); 
      }catch(IOException ioException){
        System.out.println("I/O Exception"); 
      }
    }
  }
  public class Restart extends Event {
    public Restart(long delayTime, String filename) {
      super(delayTime);
      eventsFile = filename;
    }

    public void action(){
      //reads data from a file using scanner
  File example = new File(eventsFile);
  try{
    Scanner scanner = new Scanner (example);
    while(scanner.hasNextLine()){
        String line = scanner.nextLine();
        String temp="";
        long delaytime;
        int rings=0;

        //regex to validate expression has even
        Pattern pattern = Pattern.compile("Event=");
        Matcher matcher = pattern.matcher(line);
        
        if(matcher.find()){
          //store everythign after event word
          temp=line.substring(matcher.end());
        }
        else{
          System.out.println("Invalid File Format!");
          System.exit(0);
        }
        //split string to extract event name and time
        String[] parts=temp.split("(?=,)");
        delaytime=Long.parseLong(parts[1].substring(6));
        if(parts.length==3){
          //in case of third variable: rings of bell
          rings=Integer.parseInt(parts[2].substring(7));
        }
        //get name by comparing strings and add that event to the list
        if(parts[0].equals("LightOn"))
        addEvent(new LightOn(delaytime));
        else if(parts[0].equals("LightOff"))
        addEvent(new LightOff(delaytime));
        else if(parts[0].equals("WaterOn"))
        addEvent(new WaterOn(delaytime));
        else if(parts[0].equals("WaterOff"))
        addEvent(new WaterOff(delaytime));
        else if(parts[0].equals("FansOn"))
        addEvent(new FansOn(delaytime));
        else if(parts[0].equals("FansOff"))
        addEvent(new FansOff(delaytime));
        else if(parts[0].equals("ThermostatDay"))
        addEvent(new ThermostatDay(delaytime));
        else if(parts[0].equals("ThermostatNight")){
          addEvent(new ThermostatNight(delaytime));
        }
        else if(parts[0].equals("Bell")){
          if(rings==0)
          addEvent(new Bell(delaytime));
          else if(rings>0)
          addEvent(new Bell(delaytime,rings));
        }
        else if(parts[0].equals("PowerOut"))
        addEvent(new PowerOut(delaytime));
        else if(parts[0].equals("WindowMalfunction"))
        addEvent(new WindowMalfunction(delaytime));
        else if(parts[0].equals("Terminate"))
        addEvent(new Terminate(delaytime));
        else{
          System.out.println("Invalid File Format!\n");
        }
    }
    scanner.close();
  }catch(FileNotFoundException e){
    System.out.println("File not found Exception\n");
  }
  }
    public String toString() {
      return "Restarting system";
    }
  }

  //returns an appropriate fixable object based on errorcode provided
  public Fixable getFixable(int errorcode){
    Fixable fix_obj=null;
    if(errorcode==1)
    fix_obj=new FixWindow();
    else if(errorcode==2)
      fix_obj=new PowerOn();
      
    return fix_obj;
  }

  public class Terminate extends Event {
    public Terminate(long delayTime) { super(delayTime); }
    public void action() { System.exit(0); }
    public String toString() { return "Terminating";  }
  }

  //emergency shutdown
  public void shutdown(){
    try{
      //logs all data to console and file error.log.txt
      Writer filewriter = new FileWriter("error.log.txt");
      String line="error code:"+String.valueOf(errorcode)+"\n";
      filewriter.write(line);
      System.out.print(line);
      //log time
      DateFormat df = new SimpleDateFormat("dd/MM/yy HH:mm:ss");
      Calendar calobj = Calendar.getInstance();
      line=df.format(calobj.getTime());
      line=line+"\n";
      filewriter.write(line);
      System.out.print(line);
      if(errorcode==1)
      line="Window Malfunction\n";
      if(errorcode==2)
      line="Power Out\n";
      filewriter.write(line);
      filewriter.flush();
      filewriter.close();
      //save entire greenhouse object in dump.out
      FileOutputStream fileOut = new FileOutputStream("dump.out");
      ObjectOutputStream objectOut = new ObjectOutputStream(fileOut);
      objectOut.writeObject(this);
      objectOut.close();
      fileOut.close();
    }catch(FileNotFoundException e){
      System.out.println("File not found Exception"); 
    }catch(IOException ioException){
      System.out.println("I/O Exception"); 
    }
    System.exit(0);
  }

  //Class ControllerException:
  //Handles exception by performing an emergency shutdown
  public class ControllerException extends Exception{
    public void Handle(){
		  System.out.println("Initiating emergency shut down");
      shutdown();
	  }  
  }

  public static void printUsage() {
    System.out.println("Correct format: ");
    System.out.println("  java GreenhouseControls -f <filename>, or");
    System.out.println("  java GreenhouseControls -d dump.out");
  }

  public static void main(String[] args) {
	try {
	    String option = args[0];
	    String filename = args[1];

	    if ( !(option.equals("-f")) && !(option.equals("-d")) ) {
		System.out.println("Invalid option");
		printUsage();
	    }

	    GreenhouseControls gc = new GreenhouseControls();
      //for loading from file
	    if (option.equals("-f"))  {
		  gc.addEvent(gc.new Restart(0,filename));
      gc.run();
	    }
      //for loading from object
      else if(option.equals("-d")){
        Restore restorer=new Restore();
        gc=restorer.RestoreState(filename);
        gc.run();
      }

	}
	catch (ArrayIndexOutOfBoundsException e) {
	    System.out.println("Invalid number of parameters");
	    printUsage();
	}
}
} ///:~
