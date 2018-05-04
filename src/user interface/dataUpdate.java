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
	 public void run() {
		 while(true) {
			 try {
			 	Thread.sleep(10*1000);
			 }
			 catch(InterruptedException e) {}
			 List<String> list= request.receiveRequest();
			 int x = 1;
			
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
				final String username = "kagamipion@gmail.com";
				final String password = "Kagamikami123";

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
					message.setFrom(new InternetAddress("kagamipion@gmail.com"));
					message.setRecipients(Message.RecipientType.TO,
						InternetAddress.parse("kagamipion@gmail.com"));
					message.setSubject("Testing Subject");
					message.setText("Dear XXX,"
						+ "\n\n Error.");

					Transport.send(message);

					System.out.println("Done");
					
					variables.email_check = 1;

				} catch (MessagingException e) {
					throw new RuntimeException(e);
				}
			}
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