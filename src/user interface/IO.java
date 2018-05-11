import java.io.*;

public class IO{
	public static void save() {
		try {
			FileWriter file = new FileWriter("log.txt");
			//over write the log.txt file, each sensor's mode settings = one line in the file
			for (int i = 0; i<variables.sensor.size(); i++) {
				file.write(variables.sensor.get(i));
				file.write(" ");
				file.write(variables.mode.get(i));
				file.write(" ");
				file.write(Integer.toString(variables.VWC_start.get(i)));
				file.write(" ");
				file.write(Integer.toString(variables.VWC_end.get(i)));
				file.write(" ");
				file.write(variables.Time.get(i));
				file.write(" ");
				file.write(variables.Time_m.get(i));
				file.write(" ");
				file.write(variables.period.get(i));
				file.write("\r\n");
			}
			
			//System.out.println("File is located at %s\n", file.getAbsolutePath());
			file.close();
			System.out.println("end");
		
		}catch (IOException e) {
			System.out.println("Unable to open file");
		}
	}

//when open the UI, load the settings store in the log.txt
public static void load() throws IOException {
	//check if the file exist (use the UI first time)
	File file1 = new File("log.txt");
	if (file1.exists()) {
		try {
			FileReader file = new FileReader("log.txt");
			
			BufferedReader buffert = new BufferedReader(new FileReader("log.txt"));
			if (buffert.readLine() == null) { // if the file is empty, set with default value
				variables.initial("1","VWC","20", "100", "0","0", "-1");
			}else { //or read the file line by line
				BufferedReader buffer = new BufferedReader(file);
				String strLine;
				while ((strLine = buffer.readLine())!= null) {
					String[] tokens = strLine.split(" ");
					
					for (int i = 0; i<tokens.length; i++) {
						System.out.println(tokens[i]);
					}
					System.out.println(tokens[5]);
					System.out.println("\n");
					
					if (tokens[1] == "time")
						variables.initial(tokens[0],tokens[1], tokens[2], tokens[3], tokens[4], tokens[5], tokens[6]);
					else if (tokens[1] == "VWC")
						variables.initial(tokens[0],tokens[1], tokens[2], tokens[3], tokens[4], tokens[5], tokens[6]);
					else
						variables.initial(tokens[0],tokens[1], tokens[2], tokens[3], tokens[4], tokens[5], tokens[6]);
				}
			}
			//System.out.println("File is located at %s\n", file.getAbsolutePath());
			file.close();
			System.out.println("load");
		
		}catch (IOException e) {
			System.out.println("Unable to open file");
		}
	
	}else {//create new file and set the default mode settings
		file1.createNewFile();
		variables.initial("1","VWC","20", "100", "0","0", "-1");
	}
}
}