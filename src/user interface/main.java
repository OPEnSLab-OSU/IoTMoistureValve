import java.io.IOException;

class MainBoard implements Runnable{
	 private Thread board;
	 private String Name;
	 MainBoard(String name){
		 Name = name;
	 }
	 public void run() {
		 try {
			IO.load();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		 new variables();
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
	 MainBoard board = new MainBoard("test");
	 board.start();
	 
	 dataUpdate updator = new dataUpdate("data");
	 updator.start();
 }
}