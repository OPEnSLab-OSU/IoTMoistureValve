import java.util.List;
import java.util.ArrayList;

public class variables{
	//Adafruit.io AIO key
	public static String AIO_key = "c3b8ceca3231410ab47418540810c1fe";
	//Adafruit.io group
	public static String group = "soil-data";
	
	public static String email = "kagamipion@gmail.com";
	public static int error_message = 1;
	
	public static int input_type = -1;
	
	public static int email_check = 0;
	
	public static String on_off = "OFF";
	
	public static final int VWC_MIN = 0;
	public static final int VWC_MAX = 100;
	
	public static final int TEMP_MIN = 0;
	public static final int TEMP_MAX = 100;
	
	public static final int EC_MIN = 0;
	public static final int EC_MAX = 100;
	//soil statics
	//VWC
	public static double VWC = 20;
	//EC
	public static int EC = 0;
	//temperature
	public static double temp = 20;
	
	
	//mode
	public static List<String> mode = new ArrayList<String>();
	
	//sensor name
	public static List<String> sensor = new ArrayList<String>();
	//sensor.add("1");
	
	//mode setting
	//VWC mode start value
	public static List<Integer> VWC_start = new ArrayList<Integer>();
	//VWC mode end value
	public static List<Integer> VWC_end = new ArrayList<Integer>();
	//time mode start time
	public static List<String> Time = new ArrayList<String>();
	public static List<String> Time_m = new ArrayList<String>();
	//time mode period
	public static List<String> period = new ArrayList<String>();
	
	public variables() {
		
	}
	
	public static void initial(String id,String m, String vs, String ve, String st, String stm, String p) {
		sensor.add(id);
		mode.add(m);
		VWC_start.add(Integer.parseInt(vs));
		VWC_end.add(Integer.parseInt(ve));
		
		Time.add(st);
		Time_m.add(stm);
		period.add(p);
	}
	public static void Insert(Integer i,String m, String vs, String ve, String st, String stm, String p) {
		mode.add(i,m);
		VWC_start.add(i,Integer.parseInt(vs));
		VWC_end.add(i,Integer.parseInt(ve));
		
		Time.add(i,st);
		Time_m.add(i,stm);
		period.add(i,p);
	}
	public static void Remove(int i) {
		sensor.remove(i);
		mode.remove(i);
		VWC_start.remove(i);
		VWC_end.remove(i);
		
		Time.remove(i);
		Time_m.remove(i);
		period.remove(i);
	}
	
}