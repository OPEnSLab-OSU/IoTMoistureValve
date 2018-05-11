import java.text.ParseException;
import java.util.List;
import java.util.*;
import javax.mail.*;
import javax.mail.internet.*;

class dataUpdate implements Runnable{
	 private Thread update;
	 private String Name;
	 dataUpdate(String name){
		 Name = name;
	 }
	 //update the data every 15 minutes
	 public void run() {
		 while(true) {
			 try {
				 //sleep for 15 minutes
			 	Thread.sleep(10*1000*6*15);
			 }
			 catch(InterruptedException e) {}
			 List<String> list= request.receiveRequest();
			 int x = 1;
			//check the status of the hub by check the update time on the Adafruit.io
			 for (int i = 0; i < list.size(); i++) {
				if (list.get(i).indexOf("key") != -1 && list.get(i).indexOf("elec-cond") != -1) {
					String tmp = list.get(i+6);
					try {
						x = functions.find_date(tmp);
						break;
					} catch (ParseException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			}
			if (x == 0  && variables.email_check == 0) {
				// user name and password of the public used gmail account
				final String username = "IoTMoistureValve@gmail.com";
				final String password = "IoT123456";

				Properties props = new Properties();
				props.put("mail.smtp.auth", "true");
				props.put("mail.smtp.starttls.enable", "true");
				props.put("mail.smtp.host", "smtp.gmail.com");
				props.put("mail.smtp.port", "587");

				Session session = Session.getInstance(props,
				  new javax.mail.Authenticator() {
					protected PasswordAuthentication getPasswordAuthentication() {
						return new PasswordAuthentication(username, password);
					}
				  });

				try {

					Message message = new MimeMessage(session);
					
					message.setFrom(new InternetAddress("IoTMoistureValve@gmail.com"));
					message.setRecipients(Message.RecipientType.TO,
						InternetAddress.parse("kagamipion@gmail.com"));
					//subject of email
					message.setSubject("Testing Subject");
					//content of email
					message.setText("Dear XXX,"
						+ "\n\n Error.");
					
					Transport.send(message);
					//test code
					System.out.println("Done");
					
					variables.email_check = 1;

				} catch (MessagingException e) {
					throw new RuntimeException(e);
				}
			}
			//click refresh button to update
			//however, it should run another update function instead
			test.refresh.doClick();
			 
		 }
	 }
	 
	 public void start() {
		 if (update == null) {
			 update = new Thread (this, Name);
			 update.start();
		 }
	 }
 }