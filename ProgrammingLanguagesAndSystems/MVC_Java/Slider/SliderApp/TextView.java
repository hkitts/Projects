package SliderApp;

import javax.swing.JLabel;
import javax.swing.JPanel;
import java.awt.Font;

class TextView extends JPanel implements ModelView {

    private ValueModel model;
    private JLabel text;
    private String textValue;

    private Font textFont = new Font(Font.SANS_SERIF, Font.BOLD, 14);

    public TextView(ValueModel parentModel, int startValue){
        model = parentModel;
	    model.addObserver(this);

        textValue = "Value: " + Integer.toString(startValue);

        text = new JLabel(textValue);
        text.setFont(textFont);
        text.setHorizontalAlignment(JLabel.CENTER);

        add(text);        

    }

    public void update(int oldValue) {
        int value = model.getValue();
        String textValue = "Value: " + Integer.toString(value);
        text.setText(textValue);
    }

}