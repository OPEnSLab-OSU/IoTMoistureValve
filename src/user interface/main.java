
import java.io.IOException;

class MainBoard implements Runnable{
	 private Thread board;
	 private String Name;
	 MainBoard(String name){
		 Name = name;
	 }
	 public void run() {
		 try { //load the mode settings
			IO.load();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		 //initial variables
		 new variables();
		 //create frame
		 new test();
	 }
	 
	 public void start() {
		 if (board == null) {
			 board = new Thread (this, Name);
			 board.start();
		 }
	 }
 }

public class main{
 public static void main(String[] args) {
	 //create thread that will create main frame
	 MainBoard board = new MainBoard("test");
	 board.start();
	 //background thread that will keep update data and check status of hub and valve
	 dataUpdate updator = new dataUpdate("data");
	 updator.start();
 }
}



/*reference
	chenjensen, A Java Dashboard, 11/26/2016
	https://github.com/Jensenczx/Dashboard/blob/master/Dashboard.java
*/

