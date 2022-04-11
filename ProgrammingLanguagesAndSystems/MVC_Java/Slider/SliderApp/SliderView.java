package SliderApp;

import javax.swing.JPanel;
import javax.swing.JButton;
import javax.swing.JSlider;
import java.awt.Dimension;
import javax.swing.Box;
import java.awt.BorderLayout;
import javax.swing.DefaultBoundedRangeModel;
import javax.swing.JOptionPane;

class SliderView extends JPanel implements ModelView{

    private ValueModel model;

    int minValue;
    int maxValue;
    int startValue;
    int incrementValue;

    javax.swing.JFrame frame = this.frame;
    JSlider slider;
    JButton decrement;
    JButton increment;


    public SliderView(ValueModel parentModel, int minV,
    int maxV, int sV, int iV){
        
        model = parentModel;
        model.addObserver(this);

        minValue = minV;
        maxValue = maxV;
        startValue = sV;
        incrementValue = iV;

        setLayout(new java.awt.BorderLayout(5, 5));
        //setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        //************************ Decrement *************************
        decrement = new JButton("Decrement");
        add(decrement, BorderLayout.LINE_START);

        decrement.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                decrementActionPreformed(evt);
            }
        });

        //************************** Slider *************************
        DefaultBoundedRangeModel rangeModel = new DefaultBoundedRangeModel(startValue, 0, minValue, maxValue);
        slider = new JSlider(rangeModel);

        add(slider, BorderLayout.CENTER);

        slider.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                valueSliderHandler(evt);
            }
        });

        //************************* Increment ***************************

        increment = new JButton("Increment");
        add(increment, BorderLayout.LINE_END);

        increment.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                incrementActionPreformed(evt);
            }
        });

    }

    private void valueSliderHandler(javax.swing.event.ChangeEvent evt){
    int currentValue = slider.getValue();
    model.setValue(currentValue);
    }

    private void decrementActionPreformed(java.awt.event.ActionEvent evt){
        int currentValue = slider.getValue();
        int newValue = currentValue - incrementValue;
        if(newValue < minValue){
            JOptionPane.showMessageDialog(frame,
            "Cannot further decrement the value",
            "Decrement Error",
            JOptionPane.ERROR_MESSAGE);
        } else{
            slider.setValue(newValue);
        }

    }

    private void incrementActionPreformed(java.awt.event.ActionEvent evt){
        int currentValue = slider.getValue();
        int newValue = currentValue + incrementValue;
        if(newValue > maxValue){
            JOptionPane.showMessageDialog(frame,
            "Cannot further increment the value",
            "Increment Error",
            JOptionPane.ERROR_MESSAGE);
        } else{
            slider.setValue(newValue);
        }

    }

    public void update(int oldValue) {
        int value = model.getValue();
        slider.setValue(value);
    }

}