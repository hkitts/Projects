package SliderApp;

import javax.swing.JPanel;
import java.awt.BorderLayout;
import java.awt.Dimension;
import javax.swing.Box;

public class ValueMVC extends javax.swing.JFrame {

    ValueModel model;
    
    int minValue;
    int maxValue;
    int startValue;
    int incrementValue;

    public ValueMVC(String args[]) {

        //checks arg
        if(args.length != 4 ){
            System.out.println("arg: SliderApp minValue maxValue startValue incrementValue");
            System.exit(0);
        } else{
            //trys to get values
            try {
                minValue = Integer.parseInt(args[0]);
                maxValue = Integer.parseInt(args[1]);
                startValue = Integer.parseInt(args[2]);
                incrementValue = Integer.parseInt(args[3]);

                model = new ValueModel();

            } catch (NumberFormatException e) {
                System.out.println(e.getMessage() + "--should be an integer");
                System.exit(1);
            }

        }

	    initComponents();
    }

    private void initComponents() {

        JPanel border = new JPanel(); //all views are in this jpanel
        JPanel textView = new TextView(model, startValue);
        JPanel sliderView = new SliderView(model, minValue, maxValue, startValue, incrementValue);
        JPanel gaugeView = new GaugeView(model, minValue, maxValue, startValue, incrementValue);

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);

        //set up outside border
        getContentPane().setLayout(new java.awt.BorderLayout(15, 15));
        getContentPane().add(Box.createRigidArea(new Dimension(0,5)), BorderLayout.PAGE_START);
        getContentPane().add(Box.createRigidArea(new Dimension(5,0)), BorderLayout.LINE_START);
        getContentPane().add(Box.createRigidArea(new Dimension(5,0)), BorderLayout.LINE_END);
        getContentPane().add(Box.createRigidArea(new Dimension(0,5)), BorderLayout.PAGE_END);
        getContentPane().add(border, BorderLayout.CENTER);

        //set layout of the views
        border.setLayout(new java.awt.BorderLayout(15, 15));
        border.add(textView, BorderLayout.PAGE_START);
        border.add(sliderView, BorderLayout.CENTER);
        border.add(gaugeView, BorderLayout.PAGE_END);

        pack();
    }

    public static void main(String args[]) {
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                new ValueMVC(args).setVisible(true);
            }
        });
    }
}
