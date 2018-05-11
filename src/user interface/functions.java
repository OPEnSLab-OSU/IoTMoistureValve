import java.lang.String;
import java.util.List;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

public class functions{
	//test function for multiple sensors
	public static void addVWC_start(int n) {
		variables.VWC_start.add(n);
	}
	//find the mode in the return object, it will not be used in the UI, only for early test
	public static String find_Mode(String i) {
		int j;
		for (j = 0; j <i.length(); j++) {
			if (i.charAt(j) == ':' ) {
				//it will either VWC mode that have a length of 3
				if (i.charAt(j+2) == 'V') {
					return i.substring(j+2,j+4);
				}else {   // or time/both mode that have a length of 4
					return i.substring(j+2,j+5);
				}
			}
		}
		return variables.mode.get(test.tabPanel.getSelectedIndex());
	}
	
	
	//find the date store in the list
	public static int find_date(String i) throws ParseException {
		int j;
		String up = " ";
		for (j = 0; j <i.length(); j++) {
			if (i.charAt(j) == ':' ) {
				up = i.substring(j+2,j+21);
				break;
			}
		}
		up = up.replace("T"," ");
		Date d1 = null;
		Date d2 = new Date();
		SimpleDateFormat Format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		d1 = Format.parse(up);
		String d3 = Format.format(d2);
		d2 = Format.parse(d3);
		d1.setTime(d1.getTime()-3600*7000);
		System.out.println(Format.format(d1)+"\n");
		System.out.println(Format.format(d2)+"\n");
		if (d2.getTime()-d1.getTime() > 900*1000) {
			return 0;
		}else {
			return 1;
		}
		

		//return variables.mode.get(test.tabPanel.getSelectedIndex());
	}
	//find value in the list and transfer it to integer
	public static int find_Value(String i) {
		int result = 0;
		int start,end,j;
		start = end = 0;
		int sign = 1;
		//find start number
		for (j = 0; j < i.length(); j++) {
			if (i.charAt(j) == '-')
				sign = -1;
			if (i.charAt(j) > '0' && i.charAt(j) <= '9') {
				start = j;
				break;
			}
			if (i.charAt(j) == '0') {
				return 0;
			}
		}
		// find end number
		for (j = start; j < i.length()-1; j++) {
			if (i.charAt(j+1) > '9' || i.charAt(j+1) < '0') {
				end = j;
			}
		}
		result = Integer.parseInt(i.substring(start,end));
		result = result * sign;
		return result;
	}
	//test code for error check
	public static boolean error_check(String i) {
		return true;
	}
	
	//find the value in the list, transfer it to double
	public static double find_fValue(String i) {
		if (i == null) {
			test.message.setText("Can't find seletced value");
			return 1;
		}
		double result = 0;
		int start,end,j;
		start = end = 0;
		//find start number
		for (j = 0; j < i.length(); j++) {
			if (i.charAt(j) > '0' && i.charAt(j) <= '9') {
				start = j;
				break;
			}
			if (i.charAt(j) == '0') {
				return 0;
			}
		}
		//find end number
		for (j = start; j < i.length()-1; j++) {
			if (i.charAt(j+1) > '9' || i.charAt(j+1) < '0') {
				end = j;
			}
		}
		//parse it
		result = Double.parseDouble(i.substring(start,end));
		return result;
	}
	public static void data_Update(List<String> list, int n) {
		//for (int j = 0; j < variables.sensor.size(); j++) {
		
		//update 
		for (int i = 0; i < list.size(); i++) {
			if (list.get(i).indexOf("key") != -1 && list.get(i).indexOf("vwc") != -1 && list.get(i).indexOf("vwc-high") == -1 && list.get(i).indexOf("vwc-low") == -1) {
				String tmp = list.get(i+5);
				variables.VWC = functions.find_fValue(tmp);
				break;
			}
			if (i == list.size()-1) {
				if (variables.error_message == 1)
					test.message.setText("<html><font color = 'red'>Can't find value,<br> please check Adafruit account</font></html>");
			}
		}		
		
		//update EC value
		for (int i = 0; i < list.size(); i++) {
			if (list.get(i).indexOf("key") != -1 && list.get(i).indexOf("elec-cond") != -1) {
				String tmp = list.get(i+5);
				variables.EC = functions.find_Value(tmp);
				break;
			}
			if (i == list.size()-1) {
				if (variables.error_message == 1)
					test.message.setText("Can't find value, please check Adafrui account");
			}
		}
		
		//update temp value
		for (int i = 0; i < list.size(); i++) {
			if (list.get(i).indexOf("key") != -1 && list.get(i).indexOf("temp") != -1) {
				String tmp = list.get(i+5);
				double temp = functions.find_fValue(tmp)*1.8+32;
				variables.temp = temp;
				break;
			}
			if (i == list.size()-1) {
				if (variables.error_message == 1)
					test.message.setText("Can't find value, please check Adafrui account");
			}
		}
		//}
	}

}