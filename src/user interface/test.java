//Imports are listed in full to show what's being used
//could just import javax.swing.* and java.awt.* etc..
import javax.imageio.ImageIO;
import javax.swing.*;
import javax.swing.border.Border;
import java.util.Date;
import java.text.DateFormat;
import java.text.Format;
import java.awt.event.*;
import java.awt.*;
import java.util.List;
import java.util.Objects;
import java.util.Dictionary;

import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import java.util.Calendar;
import org.jdesktop.swingx.*;

import java.awt.geom.Line2D;


public class test {
	
	
	public static JFrame guiFrame = new JFrame();
	public static int tab_l = 46;
	public static int pos_a = 43;
	
	//menu
	public static JMenuBar menuBar;
	
	
	//tab
	public static JTabbedPane tabPanel;
	
	public static JLabel message;
	//displayboard
	//VWC board
	public static dashboard VWCboard = new dashboard();
	//temperature board
	public static dashboard tempboard = new dashboard();
	//EC board
	public static dashboard ECboard = new dashboard();
	public static JPanel cpanel = new JPanel();
	
	//user account panel
	public static JPanel account_panel;
	//refresh button
	public static JButton refresh;
	
	//control panel of sensors
	public static JPanel tabp;
	public static JTabbedPane tab = new JTabbedPane();
	
	//mode panel
	public static JPanel sPanel;
	public static JPanel ModePanel = new JPanel();
	public static JRadioButton rdbtnVwc;
		
	public static JRadioButton rdbtnTime;
	public static JRadioButton rdbtnBoth;
	//vwc mode board
	public static JPanel VM_panel;
	public static JLabel VWC_sL = new JLabel("Start");
	public static JLabel VWC_eL = new JLabel("End");
	public static JTextField VWC_label,VWC_eLabel;
	public static JSlider VWC;
	public static JSlider VWC_end;
    
	//time mode panel
	public static JPanel time_panel;
	public static JComboBox start_h;
	public static JComboBox start_m;
	public static JComboBox period;
	
	//real-time mode / data based mode identifier
	public static int r_d_mode = 0;
	public static JPanel real_panel = new JPanel();
	
	
	
	public static JButton acc_button = new JButton("AIO_key");
	
	public static JButton Applybutton;
	
	public static JButton real_on_off;
	
 public test()
 {
//main frame
	sPanel = new JPanel();
	sPanel.setBackground(SystemColor.window);
	sPanel.setBounds(0, 328, 376, 348);
	guiFrame.add(sPanel);
	message = new JLabel("");
	sPanel.setLayout(null);
	 
	 
	display_board();
	tab_board();	 
	tm_board();
 
	vm_board();
	mode_p();
	create_new_frame();
	 
	JMenuBar menuBar = new JMenuBar();
	menuBar.setBounds(0, 0, 130, 34);
	guiFrame.add(menuBar);
	
	JMenu mnNewMenu_2 = new JMenu("File");
	menuBar.add(mnNewMenu_2);
	
	JMenuItem mntmClose = new JMenuItem("Close");
	mntmClose.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
			System.exit(JFrame.EXIT_ON_CLOSE);
		}
	});
	JMenuItem mntmEmail = new JMenuItem("Email");
	mnNewMenu_2.add(mntmEmail);
	mntmEmail.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent arg0) {
			variables.input_type = 1;
			account_panel.setVisible(true);
		}
	});
	
	JMenuItem mntmAiokey = new JMenuItem("AIO key");
	mnNewMenu_2.add(mntmAiokey);
	mnNewMenu_2.add(mntmClose);
	mntmAiokey.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent arg0) {
			variables.input_type = 0;
			account_panel.setVisible(true);
		}
	});
	
	JMenu mnNewMenu = new JMenu("Edit");
	menuBar.add(mnNewMenu);
	
	JMenuItem mntmNewMenuItem = new JMenuItem("Add sens");
	mntmNewMenuItem.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
			JComponent panel1 = new JPanel();
			int n=2;
			for (int i = 0; i<variables.sensor.size(); i++) {
				n = i+1;
				System.out.println(n + " " + variables.sensor.get(i));
				if (Integer.parseInt(variables.sensor.get(i)) != n) {
					variables.sensor.add(i,Integer.toString(n));
					break;
				}
				if (i+1 == variables.sensor.size()) {
					n++;
					System.out.println(n);
					variables.sensor.add(Integer.toString(n));
					break;
				}	
			}
			tabPanel.insertTab("V"+Integer.toString(n),null,panel1,null,n-1);
			variables.Insert(variables.sensor.indexOf(Integer.toString(n)),"VWC","20", "100", "0","0", "-1");
		}
	});
	mnNewMenu.add(mntmNewMenuItem);
	
	JMenuItem mntmDeleteSensor = new JMenuItem("Delete sensor");
	mntmDeleteSensor.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
			int index = tabPanel.getSelectedIndex();
			tabPanel.remove(index);
			System.out.println(index);
			variables.Remove(index);
		}
	});
	mnNewMenu.add(mntmDeleteSensor);
	 // mode change button
	 // will switch to another mode once click it
	 
	
	
	JMenu mnNewMenu_3 = new JMenu("Mode");
	menuBar.add(mnNewMenu_3);
	
	
	JMenuItem mntmSwitchMode = new JMenuItem("Switch Mode");
	mntmSwitchMode.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
			if (r_d_mode == 1) {
				 r_d_mode = 0;
				 Applybutton.setEnabled(true);
				 real_on_off.setEnabled(false);
			 }
			 else {
				 r_d_mode = 1;
				 real_on_off.setEnabled(true);
				 Applybutton.setEnabled(false);
			 }
			 //request.Request_t();
			 guiFrame.repaint();
		}
	});
	mnNewMenu_3.add(mntmSwitchMode);
	
	
	JMenu mnNewMenu_4 = new JMenu("View");
	menuBar.add(mnNewMenu_4);
	JMenuItem mntmCEmail = new JMenuItem("Check Email");
	mntmCEmail.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
			message.setText(variables.email);
		}
	});
	mnNewMenu_4.add(mntmCEmail);
	
	JMenuItem mntmCError = new JMenuItem("Close error message");
	mntmCError.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
			if (variables.error_message == 1) {
				variables.error_message = 0;
				mntmCError.setText("Open error message");
				message.setText("Error notification closed");
			}else if (variables.error_message == 0) {
				variables.error_message = 1;
				mntmCError.setText("Close error message");
				message.setText("Error notification opened");
			}
		}
	});
	mnNewMenu_4.add(mntmCError);
	/*
	 JButton tabc = new JButton("Data");
	 //JLabel lblTitle = new JLabel("var1");
	 tabc.addActionListener( new ActionListener()
	 {
		 public void actionPerformed(ActionEvent e)
		 {
			 // Create a method named "createFrame()", and set up an new frame there
			 if (r_d_mode == 1) {
				 r_d_mode = 0;
				 tabc.setText("Data");
				 Applybutton.setEnabled(true);
				 real_on_off.setEnabled(false);
			 }
			 else {
				 tabc.setText("Real-time");
				 r_d_mode = 1;
				 real_on_off.setEnabled(true);
				 Applybutton.setEnabled(false);
			 }
			 //request.Request_t();
			 guiFrame.repaint();
		 }	
	 });
	 tabc.setBackground(Color.WHITE);
	 tabc.setBounds(400,450,80,30);
	 guiFrame.add(tabc);
 	*/
	 //real-time mode on-off button
	 real_on_off = new JButton("ON");
	 real_on_off.setEnabled(false);
	 real_on_off.addActionListener( new ActionListener()
	 {
		 public void actionPerformed(ActionEvent e)
		 {
			 // Create a method named "createFrame()", and set up an new frame there
			 if (variables.on_off == "ON")
				 real_on_off.setText("OFF");
			 else
				 real_on_off.setText("ON");
			 request.Request_t();
			 guiFrame.repaint();
		 }	
	 });
	 real_on_off.setBackground(Color.WHITE);
	 real_on_off.setBounds(450,450,60,30);
	 guiFrame.add(real_on_off);
	 //tab
      
	 
	JPanel dlPanel = new JPanel();
	dlPanel.setBounds(0, 250, 640, 55);
	dlPanel.setBackground(new Color(135, 206, 235));
	guiFrame.add(dlPanel);
	dlPanel.setLayout(null);
	
	JLabel lblNewLabel_1 = new JLabel("VWC");
	lblNewLabel_1.setBounds(98, 17, 73, 21);
	dlPanel.add(lblNewLabel_1);
	
	JLabel lblNewLabel_2 = new JLabel("Temp");
	lblNewLabel_2.setBounds(300, 17, 73, 21);
	dlPanel.add(lblNewLabel_2);
	
	JLabel lblNewLabel_3 = new JLabel("EC");
	lblNewLabel_3.setBounds(507, 17, 73, 21);
	dlPanel.add(lblNewLabel_3);
	


 
 
	 //refresh.setBorderPainted(false);
	refresh = new JButton(" ");
	refresh.setContentAreaFilled(false);
	refresh.setBorder(null);
	refresh.setFocusPainted(false);
 
	 //Apply button, apply data change to Adafruit.io
	 
	refresh.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent event) {
			message.setText("");
			List<String> list= request.receiveRequest();
			int index = tabPanel.getSelectedIndex();
			functions.data_Update(list,index+1);
			/*for (int i = 0; i < list.size(); i++) {
				if (list.get(i).indexOf("key") != -1 && list.get(i).indexOf("VWC") != -1 && list.get(i).indexOf("vwc_high") == -1 && list.get(i).indexOf("vwc-low") == -1) {
					//System.out.println(list.get(list.get(i).indexOf(i+5));
					String temp = list.get(i+5);
					variables.VWC = functions.find_fValue(temp);
					//VWC.setValue(variables.VWC);
					//VWCboard.setValue(Double.toString(variables.VWC));
					guiFrame.repaint();
				}
			}*/
			VWCboard.setValue(Double.toString(variables.VWC));
			ECboard.setValue(Integer.toString(variables.EC));
			tempboard.setValue(Double.toString(variables.temp));
			
			account_panel.setVisible(false);
			guiFrame.repaint();
		}
	 });
	 try {
		    Image img = ImageIO.read(getClass().getResource("refresh_icon2.png"));
		    refresh.setIcon(new ImageIcon(img));
		  } catch (Exception ex) {
		    System.out.println(ex);
		  }
	refresh.setBounds(581, 0, 52, 33);

	
	
	Applybutton = new JButton( "Apply");
	Applybutton.setContentAreaFilled(false);
	Applybutton.setFocusPainted(false);
 	Applybutton.addActionListener(new ActionListener()
	 {
	 @Override
	 public void actionPerformed(ActionEvent event)
	 {
		 int vwc = VWC.getValue();
		 variables.VWC = vwc;
		 System.out.println(variables.VWC);
		 request.sendRequest();
		 IO.save();
		 message.setText("Successfully changed!");
		 /*
		 if (variables.mode.get(tabPanel.getSelectedIndex()) == "VWC") {
			 int vwc_s = VWC.getValue();
			 int vwc_e = VWC_end.getValue();
			 variables.VWC_start.set(tabPanel.getSelectedIndex(), vwc_s);
			 variables.VWC_end.set(tabPanel.getSelectedIndex(), vwc_e);
		 }else if (variables.mode.get(tabPanel.getSelectedIndex()) == "time") {
		 }
		 */

	 }
	 });
 //guiFrame.add(listPanel, BorderLayout.CENTER);
 	
 	
 	
 Applybutton.setBounds(90, 280, 126, 33);
 sPanel.add(Applybutton);
 guiFrame.add(refresh);

 
 acc_button.setBackground(Color.WHITE);
 acc_button.setBounds(650,390,104,39);
 guiFrame.add(acc_button);
 
 message.setBounds(380,600,280,60);
 message.setFont(new Font("Serif", Font.PLAIN, 15));
 guiFrame.add(message);
 
 //make sure the program exits when the frame closes
 guiFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
 guiFrame.setResizable(false);
 guiFrame.setTitle("Test");
 
 //This will center the JFrame in the middle of the screen
 guiFrame.setLocationRelativeTo(null);

 //make sure the JFrame is visible
 guiFrame.setSize(654, 737);
 //guiFrame.getContentPane().setBackground(new Color(107,106,104));
 guiFrame.getContentPane().setBackground(Color.white);
 guiFrame.setLayout(null);
 guiFrame.setVisible(true);
 
 }
 
 
 //tab panel
 //one change the panel, the other components in the frame won't change
 //so it's only use for distinguish different sensor
 
 public void tab_board() {
	 tabPanel = new JTabbedPane(JTabbedPane.TOP);
	 tabPanel.setBounds(0, 31, 784, 26);
	 //JComponent panel2 = makeTextPanel("panel #2");
	 //tabPanel.addTab("se2",panel2);
	 String x = "V";
     for (int i = 0; i < variables.sensor.size(); i++) {
    	 JComponent panel2 = new JPanel();
    	 tabPanel.addTab(x+variables.sensor.get(i),panel2);
     }
     tabPanel.addChangeListener(new ChangeListener() 
     {
    	 public void stateChanged(ChangeEvent ee) {
    		 refresh.doClick();
    		 start_h.setSelectedItem(variables.Time.get(tabPanel.getSelectedIndex()));
    		 start_m.setSelectedItem(variables.Time_m.get(tabPanel.getSelectedIndex()));
    		 period.setSelectedItem(variables.period.get(tabPanel.getSelectedIndex()));
    		 VWC.setValue(variables.VWC_start.get(tabPanel.getSelectedIndex()));
    		 VWC_end.setValue(variables.VWC_end.get(tabPanel.getSelectedIndex()));
    		 if (Objects.equals(variables.mode.get(tabPanel.getSelectedIndex()),"time")) {
    			 rdbtnBoth.setSelected(false);
    			 rdbtnVwc.setSelected(false);
    			 rdbtnTime.setSelected(true);
    		 }else if (Objects.equals(variables.mode.get(tabPanel.getSelectedIndex()),"VWC")) {
    			 rdbtnBoth.setSelected(false);
    			 rdbtnVwc.setSelected(true);
    			 rdbtnTime.setSelected(false);
    		 }else if (Objects.equals(variables.mode.get(tabPanel.getSelectedIndex()),"both")) {
    			 rdbtnBoth.setSelected(true);
    			 rdbtnVwc.setSelected(false);
    			 rdbtnTime.setSelected(false);
    		 }
    	 }
     });
     
    
	 guiFrame.add(tabPanel);
 }
 
 
 
 //Enter AIO-key for user account
 //will ignore if input is empty(misclick or cancel)
 //will print error message if length isn't equal to 32(AIO-key length)
 
 public static void create_new_frame() {
	
	 account_panel = new JPanel();
     account_panel.setLayout(new BoxLayout(account_panel, BoxLayout.Y_AXIS));
     account_panel.setOpaque(true);
     JTextField input = new JTextField(10);
     JButton button = new JButton("Enter");
     Border blackline = BorderFactory.createLineBorder(Color.black);
     button.setBackground(Color.WHITE);
     button.setAlignmentX(Component.CENTER_ALIGNMENT);
     input.setText(null);
     button.addActionListener(new ActionListener() {
    	 public void actionPerformed(ActionEvent e) {
    		 String temp = input.getText();
    		 //acc_button.setVisible(true);
    		 
    		 if (variables.input_type == 0) {	
    		 	if (temp.length() == 0) {}
    			 	//ignore it
    		 	else if (temp.length() != 32) {}
    			 	//print error message
    		 	else
    		 		variables.AIO_key = temp;
    		 }else if (variables.input_type == 1){
    			 if (temp.length() == 0) {}
 			 	//ignore it
    			 else
    				 variables.email = temp;
    		 }
    	     input.setText("");
    		 account_panel.setVisible(false);
    	 }
     });
     account_panel.setBorder(blackline);
     account_panel.setBackground(Color.WHITE);
     account_panel.add(input);
     account_panel.add(button);
     account_panel.setBounds(376, 320, 255, 60);
     account_panel.setVisible(false);
     input.requestFocus();
     guiFrame.add(account_panel);
 }
 public static void addSensor() {
	// Field
 }
 
 
 //3 dash board, each refers to one soil data
 //all sensors use same dashboards
 public static void display_board() {
	//set the type
	 VWCboard.setType("circle");
	 VWCboard.setUnit(" ");
	 VWCboard.setValue(Double.toString(variables.VWC));
	 VWCboard.setFrom(0);
	 VWCboard.setTo(100);
	 VWCboard.setMajor(10);
	 VWCboard.setMinor(5);

	 //dashboard.setForeground(Color.BLUE);
	 //dashboard.setBackground(Color.WHITE);
	 //dashboard font color
	 VWCboard.setTextColor(Color.black);
	 //dashboard data color
	 VWCboard.setValueColor(Color.BLACK);
	 //dashboard pointer color
	 VWCboard.setPointerColor(Color.BLUE);
	 //dashboard short point color
	 VWCboard.setMajorScaleColor(Color.BLACK);
	 //dashboard long point color
	 VWCboard.setMinorScaleColor(Color.DARK_GRAY);

	 //set size for dashboard
	 VWCboard.setPreferredSize(new Dimension(169, 169));
	 
	 
	 
	 //set the type
	 tempboard.setType("circle");
	 tempboard.setUnit(" ");
	 tempboard.setValue(Double.toString(variables.temp));
	 tempboard.setFrom(10);
	 tempboard.setTo(130);
	 tempboard.setMajor(10);
	 tempboard.setMinor(5);


	 tempboard.setTextColor(Color.black);
	 tempboard.setValueColor(Color.BLACK);
	 tempboard.setPointerColor(Color.BLUE);
	 tempboard.setMajorScaleColor(Color.BLACK);
	 tempboard.setMinorScaleColor(Color.DARK_GRAY);

	 tempboard.setPreferredSize(new Dimension(169, 169));
	 
	 
	 
	 //set the type
	 ECboard.setType("circle");
	 ECboard.setUnit(" ");
	 ECboard.setValue(Integer.toString(variables.EC));
	 ECboard.setFrom(0);
	 ECboard.setTo(100);
	 ECboard.setMajor(10);
	 ECboard.setMinor(5);

	 
	 ECboard.setTextColor(Color.black);
	 ECboard.setValueColor(Color.BLACK);
	 ECboard.setPointerColor(Color.BLUE);
	 ECboard.setMajorScaleColor(Color.BLACK);
	 ECboard.setMinorScaleColor(Color.DARK_GRAY);

	 ECboard.setPreferredSize(new Dimension(169, 169));
	 
	//panel for data display
	 
	 FlowLayout layout = new FlowLayout();
	 cpanel.setLayout(layout);

	 cpanel.add(VWCboard);
	 cpanel.add(Box.createRigidArea(new Dimension(10,0)));
	 cpanel.add(tempboard);
	 cpanel.add(Box.createRigidArea(new Dimension(10,0)));
	 cpanel.add(ECboard);
	 
	 cpanel.setBackground(Color.WHITE);
	 
	 cpanel.setComponentOrientation(ComponentOrientation.LEFT_TO_RIGHT);
	 Border blackline = BorderFactory.createLineBorder(Color.black);
	 //cpanel.setBorder(blackline);
	//soil statics panel
	 cpanel.setBounds(0, 65, 640, 190);
	 
	 List<String> list= request.receiveRequest();
		functions.data_Update(list,1);
		/*for (int i = 0; i < list.size(); i++) {
			if (list.get(i).indexOf("key") != -1 && list.get(i).indexOf("VWC") != -1 && list.get(i).indexOf("vwc_high") == -1 && list.get(i).indexOf("vwc-low") == -1) {
				//System.out.println(list.get(list.get(i).indexOf(i+5));
				String temp = list.get(i+5);
				variables.VWC = functions.find_fValue(temp);
				//VWC.setValue(variables.VWC);
				//VWCboard.setValue(Double.toString(variables.VWC));
				guiFrame.repaint();
			}
		}*/
	VWCboard.setValue(Double.toString(variables.VWC));
	ECboard.setValue(Integer.toString(variables.EC));
	tempboard.setValue(Double.toString(variables.temp));
	guiFrame.repaint();
	 
	 
	 
	 guiFrame.add(cpanel);
 }
 //panel for time mode
 
 
 
   //time mode
   //user can choose the start time in a day
   //user can choose the period, (how long will valve open)
   public static void tm_board() {
	 //calender
	  time_panel = new JPanel();
	  //hours in a day
	  String[] hours = {"0","1","2","3","4","5","6","7","8","9","10","11"
			  ,"12","13","14","15","16","17","18","19","20","21","22","23"};
	  String[] per = {"1","2","3","4","5","6","7","8","9","10","11"
			  ,"12","13","14","15","16","17","18","19","20","21","22","23","24"};
	  String[] minutes = new String[60];
	  for (int i = 0; i< 60; i++) {
		  minutes[i] = Integer.toString(i);
	  }
	  JLabel start_time = new JLabel("start:");
	  start_h = new JComboBox(hours);
	  JLabel end_time = new JLabel("period:");
	  start_m = new JComboBox(minutes);
	  period = new JComboBox(per);
	  start_h.setBackground(Color.WHITE);
	  start_h.setSelectedItem(variables.Time.get(tabPanel.getSelectedIndex()));
	  start_m.setBackground(Color.WHITE);
	  start_m.setSelectedItem(variables.Time_m.get(tabPanel.getSelectedIndex()));
	  period.setBackground(Color.WHITE);
	  period.setSelectedItem(variables.period.get(tabPanel.getSelectedIndex()));
	  Border blackline = BorderFactory.createLineBorder(Color.black);
	 
	  ItemListener s_itemListener = new ItemListener() {
	       public void itemStateChanged(ItemEvent itemEvent) {
	         int state = itemEvent.getStateChange();
	         if (state == ItemEvent.SELECTED) {
	        	 //String time = itemEvent.getItem().toString();
	      	   	 variables.Time.set(tabPanel.getSelectedIndex(),  itemEvent.getItem().toString());
	      	  
	      	   //System.out.println("Item: " + itemEvent.getItem());
	         }
	       }
	  };
	  start_h.addItemListener(s_itemListener);
	  
	  ItemListener m_itemListener = new ItemListener() {
	       public void itemStateChanged(ItemEvent itemEvent) {
	         int state = itemEvent.getStateChange();
	         if (state == ItemEvent.SELECTED) {
	        	 //String time = itemEvent.getItem().toString();
	      	   	 variables.Time_m.set(tabPanel.getSelectedIndex(),  itemEvent.getItem().toString());
	      	  
	      	   //System.out.println("Item: " + itemEvent.getItem());
	         }
	       }
	  };
	  start_m.addItemListener(m_itemListener);
	  
	  
	  ItemListener e_itemListener = new ItemListener() {
	       public void itemStateChanged(ItemEvent itemEvent) {
	         int state = itemEvent.getStateChange();
	         if (state == ItemEvent.SELECTED) {
	        	 String period = itemEvent.getItem().toString();
	      	   	 variables.period.set(tabPanel.getSelectedIndex(),  period);
	      	  
	      	   //System.out.println("Item: " + itemEvent.getItem());
	         }
	       }
	  };
	  period.addItemListener(e_itemListener);
	  
	  
	  time_panel.setBackground(Color.WHITE);
	  start_time.setBounds(13, 180, 73, 21);
	  end_time.setBounds(13, 221, 73, 21);
	  start_h.setBounds(90, 180, 68, 29);
	  start_m.setBounds(160, 180, 68, 29);
	  period.setBounds(90, 217, 68, 29);
	  //time_panel.add(picker);
	  sPanel.add(start_time);
	  sPanel.add(start_h);
	  sPanel.add(end_time);
	  sPanel.add(period);
	  sPanel.add(start_m);
	 // time_panel.setBorder(blackline);
	  
	  guiFrame.add(time_panel);
   }
   
   //VWC mode, 2 sliders
   //start value will always lower than end value
   public static void vm_board() {
	   VM_panel = new JPanel();
	   JPanel vs_p = new JPanel();
	   JPanel ve_p = new JPanel();
	   vs_p.setLayout(new BoxLayout(vs_p,BoxLayout.X_AXIS));
	   ve_p.setLayout(new BoxLayout(ve_p,BoxLayout.X_AXIS));
	   VM_panel.setLayout(new BoxLayout(VM_panel,BoxLayout.Y_AXIS));
	   VWC = new JSlider(JSlider.HORIZONTAL, variables.VWC_MIN,variables.VWC_MAX,variables.VWC_start.get(0));
	   VWC_end = new JSlider(JSlider.HORIZONTAL, variables.VWC_MIN,variables.VWC_MAX,variables.VWC_end.get(0));

	   //label for VWC start value
	   VWC_label = new JTextField() {
	  	 @Override public void setBorder(Border border) {
	  		 
	  	 }
	   };
	   
	   VWC_label.setEditable(false);
	   VWC_label.setBackground(Color.WHITE);
	   VWC_label.setText(Integer.toString(variables.VWC_start.get(0)));
	   
	  //Slider for VWC
	  
	  //VWC.setMajorTickSpacing(10);
	  VWC.setPaintTicks(false);
	  VWC.setBackground(Color.WHITE);
	  //VWC.setInverted(true);

	  ChangeListener vListener = new ChangeListener() {
	  	 public void stateChanged(ChangeEvent event) {
	  		 JSlider source = (JSlider) event.getSource();
	         VWC_label.setText("" + source.getValue());
	         int vs = VWC.getValue();
	         int ve = VWC_end.getValue();
	         if (ve-vs < 1) {
	        	 if (vs == 100) {
	        		 VWC_end.setValue(100);
	        		 variables.VWC_start.set(tabPanel.getSelectedIndex(), vs);
	        		 variables.VWC_end.set(tabPanel.getSelectedIndex(), 100);
	        	 }
	        	 else {
	        		 VWC_end.setValue(vs+1);
	        		 variables.VWC_start.set(tabPanel.getSelectedIndex(), vs);
	        		 variables.VWC_end.set(tabPanel.getSelectedIndex(), vs+1);
	        	 }
	         }else {
	        	 variables.VWC_start.set(tabPanel.getSelectedIndex(), vs);
	         }
	         
	  	 }
	  };

	  VWC.addChangeListener(vListener);
	   
	   // label for VWC mode end value
	   VWC_eLabel = new JTextField() {
	  	 @Override public void setBorder(Border border) {
	  		 
	  	 }
	   };
	   VWC_eLabel.setEditable(false);
	   VWC_eLabel.setBackground(Color.WHITE);
	   VWC_eLabel.setText(Integer.toString(variables.VWC_end.get(0)));
	   
	   
	   //VWC_end.setMajorTickSpacing(10);
	   VWC_end.setPaintTicks(false);
	   VWC_end.setBackground(Color.WHITE);

	   ChangeListener eListener = new ChangeListener() {
	  	 public void stateChanged(ChangeEvent event) {
	  		 JSlider source = (JSlider) event.getSource();
	          VWC_eLabel.setText("" + source.getValue());
	          int vs = VWC.getValue();
	          int ve = VWC_end.getValue();
	          if (ve-vs < 1) {
	        	  if (ve == 0) {
	        		  VWC.setValue(0);
	        	  	  variables.VWC_start.set(tabPanel.getSelectedIndex(), 0);
	        		  variables.VWC_end.set(tabPanel.getSelectedIndex(), ve);
	        	  }else {
	        		  VWC.setValue(ve-1);
	        		  variables.VWC_start.set(tabPanel.getSelectedIndex(), ve-1);
	        		  variables.VWC_end.set(tabPanel.getSelectedIndex(), ve);
	        	  }
	          }else {
	        	  variables.VWC_end.set(tabPanel.getSelectedIndex(), ve);
	          }
	  	 }
	  };
	  
	   VWC_end.addChangeListener(eListener);
	   vs_p.add(VWC_sL);
	   vs_p.add(VWC);
	   vs_p.add(VWC_label);
	   vs_p.setBackground(Color.WHITE);
	   ve_p.add(VWC_eL);
	   ve_p.add(Box.createRigidArea(new Dimension(7,0)));
	   ve_p.add(VWC_end);
	   ve_p.add(VWC_eLabel);
	   ve_p.setBackground(Color.WHITE);
	   VM_panel.add(vs_p);
	   VM_panel.add(ve_p);
	   //Border blackline = BorderFactory.createLineBorder(Color.black);
	   //VM_panel.setBorder(blackline);
	   VM_panel.setBounds(0, 46, 376, 117);
	   VM_panel.setBackground(Color.WHITE);
	   sPanel.add(VM_panel);
   }
   
   
   //data based mode change in mode control panel
   //when current mode changes, the layout of mode control panel
   //will also change
   public static void mode_p() {
	 //panel for mode choice

	   
	   //ModePanel.add(comboLbl);
	   //ModePanel.add(mode);
	   
	   
	   JLabel lblNewLabel = new JLabel("mode");
	   lblNewLabel.setForeground(SystemColor.textHighlight);
	   ModePanel.add(lblNewLabel);
		
	   rdbtnVwc = new JRadioButton("VWC");
	   ModePanel.add(rdbtnVwc);
	   rdbtnVwc.setBackground(Color.WHITE);
		
	   rdbtnTime = new JRadioButton("time");
	   ModePanel.add(rdbtnTime);
	   rdbtnTime.setBackground(Color.WHITE);
		
	   rdbtnBoth = new JRadioButton("both");
	   ModePanel.add(rdbtnBoth);
	   rdbtnBoth.setBackground(Color.WHITE);
	   
	   rdbtnVwc.addActionListener(new ActionListener() {
	        @Override
	        public void actionPerformed(ActionEvent e) {
	            String mode = rdbtnVwc.getText();
	            variables.mode.set(tabPanel.getSelectedIndex(),  mode);
	            rdbtnTime.setSelected(false);
	            rdbtnBoth.setSelected(false);
	        }
	    });
	   
	   rdbtnTime.addActionListener(new ActionListener() {
	        @Override
	        public void actionPerformed(ActionEvent e) {
	        	String mode = rdbtnTime.getText();
	            variables.mode.set(tabPanel.getSelectedIndex(),  mode);
	            rdbtnVwc.setSelected(false);
	            rdbtnBoth.setSelected(false);

	        }
	    });
	   
	   rdbtnBoth.addActionListener(new ActionListener() {
	        @Override
	        public void actionPerformed(ActionEvent e) {
	        	String mode = rdbtnBoth.getText();
	            variables.mode.set(tabPanel.getSelectedIndex(),  mode);
	            rdbtnVwc.setSelected(false);
	            rdbtnTime.setSelected(false);

	        }
	    });
	   //mode.setSelectedItem(variables.mode.get(tabPanel.getSelectedIndex()));
	   
	   if (variables.mode.get(tabPanel.getSelectedIndex()) == "VWC") {
		   rdbtnVwc.setSelected(true);
		   rdbtnBoth.setSelected(false);
           rdbtnTime.setSelected(false);
	   }else if (variables.mode.get(tabPanel.getSelectedIndex()) == "time") {
		   rdbtnVwc.setSelected(false);
		   rdbtnBoth.setSelected(false);
           rdbtnTime.setSelected(true);
	   }else {
		   rdbtnVwc.setSelected(false);
		   rdbtnBoth.setSelected(true);
           rdbtnTime.setSelected(false);
	   }
	   //create action for refresh button

	     //variables.l1 = new JLabel("123");
	     //guiFrame.add(variables.l1);
	   ModePanel.setBounds(0, 0, 300, 46);
	   ModePanel.setBackground(Color.white);
	   sPanel.add(ModePanel);
   }
   
   // text panel for tabs
   protected JComponent makeTextPanel(String text) {
       JPanel panel = new JPanel(new GridBagLayout());
       JLabel filler = new JLabel(text);
       JButton button = new JButton("x");
       panel.setOpaque(false);
       //filler.setHorizontalAlignment(JLabel.CENTER);
       GridBagConstraints gbc = new GridBagConstraints();
       gbc.gridx = 0;
       gbc.gridy = 0;
       gbc.weightx = 1;
       //panel.setLayout(new GridLayout(1, 1));
      panel.add(filler,gbc);
       //panel.setLayout(new GridLayout(1, 1));
       panel.add(button,gbc);
       return panel;
   }
   
 
}


