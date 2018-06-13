import java.net.*;
import java.io.*;
import java.util.*;

public class request{
	 /*public static void main(String[] args) {
	 
	 //request.sendRequest();
	   request.receiveRequest();
	   //request.checkdate(); //this should be a thread
	   while(true) {
		   break;
	   }
	 }*/
	 public static void checkdate() {
		 
	 }
	public static String sendRequest() {
		StringBuffer jsonString = new StringBuffer();
    try {
    	//String input = new
    	String url = "https://io.adafruit.com/api/groups/";
    	url += variables.group;
		url += "/send.json?x-aio-key=";
		url += variables.AIO_key;
		int i = test.tabPanel.getSelectedIndex();
		
		url += "&txtbox=";
		
		//url += Integer.toString(i);
		
		//mode option
		if (Objects.equals(variables.mode.get(i),"time")) {
			url += Integer.toString(1);
		}
		if (Objects.equals(variables.mode.get(i),"VWC")) {
			url += Integer.toString(2);
		}
		if (Objects.equals(variables.mode.get(i),"both")) {
			url += Integer.toString(3);
		}
		
		//VWC/both mode
		if (Objects.equals(variables.mode.get(i),"VWC") ||Objects.equals(variables.mode.get(i),"both")) {
			url+="/";
			url += Integer.toString(variables.VWC_start.get(i));
			url+="/";
			url += Integer.toString(variables.VWC_end.get(i));
		}else {           // not VWC/both mode
			url+="/";
			url += Integer.toString(0);
			url+="/";
			url += Integer.toString(0);
		}
		
		//time/both mode
		if (Objects.equals(variables.mode.get(i),"time")|| Objects.equals(variables.mode.get(i),"both")) {
			url+="/";
			int tmp = Integer.parseInt(variables.Time.get(i));
			tmp = tmp * 60;
			int m = Integer.parseInt(variables.Time_m.get(i));
			tmp = tmp + m;
			url+=Integer.toString(tmp);
			url+="/";
			tmp = Integer.parseInt(variables.period.get(i));
			tmp = tmp*60;
			url+=Integer.toString(tmp);
		}else {             //not time/both mode
			url+="/";
			url += Integer.toString(0);
			url+="/";
			url += Integer.toString(0);
		}
		url += "/";
		//default value for change
		url += Integer.toString(5);
		
		
		/*
		 if (recuring_check == 1){
		 
			url +="/";
			url +="1";
		 }else{
		 	url +="/";
		 	url +="0";
		 }
		 */
		 
		System.out.println(variables.mode.get(i));	
    		
        URL add = new URL(url);
        HttpURLConnection connection = (HttpURLConnection) add.openConnection();

        connection.setRequestMethod("GET");
        connection.setRequestProperty("Accept", "application/json");
        connection.setRequestProperty("Content-Type", "application/json; charset=UTF-8");
        
        int responseCode = connection.getResponseCode();
		System.out.println("\nSending 'GET' request to URL : " + url);
		System.out.println("Response Code : " + responseCode);
        
		/*BufferedReader in = new BufferedReader(
		        new InputStreamReader(connection.getInputStream()));
		String inputLine;

		while ((inputLine = in.readLine()) != null) {
			jsonString.append(inputLine);
		}
		in.close();
		*/
        connection.disconnect();
        //System.out.println(jsonString.toString());
    } catch (Exception e) {
            throw new RuntimeException(e.getMessage());
    }
    return jsonString.toString();
	}
	
	public static String Request_t() {
		StringBuffer jsonString = new StringBuffer();
    try {
    	//String input = new
    	String url = "https://io.adafruit.com/api/groups/";
    		url += variables.group;
    		url += "/send.json?x-aio-key=";
    		url += variables.AIO_key;
    		url += "&on_off=";
    		url += variables.on_off;
    		if (variables.on_off == "ON")
    			variables.on_off = "OFF";
    		else
    			variables.on_off = "ON";
    		//url += variables.mode;
    		//url += "&Elec_Cond";
    		//url += Integer.toString(1);
    		//url += "=";
    		//url += Integer.toString(variables.VWC);
    		
    		//System.out.println(variables.group);
    		
        URL add = new URL(url);
        HttpURLConnection connection = (HttpURLConnection) add.openConnection();

        connection.setRequestMethod("GET");
        connection.setRequestProperty("Accept", "application/json");
        connection.setRequestProperty("Content-Type", "application/json; charset=UTF-8");
        
        int responseCode = connection.getResponseCode();
		System.out.println("\nSending 'GET' request to URL : " + url);
		System.out.println("Response Code : " + responseCode);
        
		BufferedReader in = new BufferedReader(
		        new InputStreamReader(connection.getInputStream()));
		String inputLine;

		while ((inputLine = in.readLine()) != null) {
			jsonString.append(inputLine);
		}
		in.close();

        connection.disconnect();
        //System.out.println(jsonString.toString());
    } catch (Exception e) {
            throw new RuntimeException(e.getMessage());
    }
    return jsonString.toString();
	}
	
	public static List<String> receiveRequest() {
		List<String> list = new ArrayList<String>();
    try {
    	String url = "https://io.adafruit.com/api/groups/soil-data/receive.json?x-aio-key=c3b8ceca3231410ab47418540810c1fe";
        URL add = new URL(url);
        HttpURLConnection connection = (HttpURLConnection) add.openConnection();

        connection.setRequestMethod("GET");
        connection.setRequestProperty("Accept", "application/json");
        connection.setRequestProperty("Content-Type", "application/json; charset=UTF-8");
        
        int responseCode = connection.getResponseCode();
		System.out.println("\nSending 'GET' request to URL : " + url);
		System.out.println("Response Code : " + responseCode);
		BufferedReader in = new BufferedReader(
		        new InputStreamReader(connection.getInputStream()));
		String inputLine;

		while ((inputLine = in.readLine()) != null) {
			list.add(inputLine);
			//System.out.println(inputLine);
		}
		in.close();
		
        connection.disconnect();
        /*for (i = 0; i < list.size(); i++) {
        	System.out.println(list.get(i));
        }*/
        //System.out.println(list.get(1));
    } catch (Exception e) {
            throw new RuntimeException(e.getMessage());
    }
    //return jsonString.toString();
    return list;
	}
}