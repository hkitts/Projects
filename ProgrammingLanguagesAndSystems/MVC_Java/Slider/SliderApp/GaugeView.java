package SliderApp;


import javax.swing.JPanel;
import javax.swing.BorderFactory;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseMotionAdapter;

class GaugeView extends JPanel implements ModelView{

    javax.swing.JFrame frame = this.frame;

    int minValue, maxValue, startValue, currentValue, incrementValue;
    int height;
    int width;
    double ratio;
    double angle;
    int release;
    int x1, x2, y1, y2;

    private ValueModel model;

    public GaugeView(ValueModel parentModel,int minV,
    int maxV, int sV, int iV){

    /* Set initial values and find ratio of current values to semi-circle */
    minValue = minV;
    maxValue = maxV;
    startValue = sV;
    incrementValue = iV;
    double temp = maxV - minV;
    ratio = 180 / (temp);
    currentValue = sV;
    angle = ratio * (currentValue - minValue);
    angle = Math.toRadians(angle);

    model = parentModel;
    model.addObserver(this);

    setBorder(BorderFactory.createLineBorder(Color.black)); 

    addMouseListener(new MouseAdapter(){
        public void mousePressed(MouseEvent e){
            moveLine(e.getX(), e.getY());
        }
        public void mouseReleased(MouseEvent e){
            release = 0;
        }
    });
    addMouseMotionListener(new MouseAdapter(){
        public void mouseDragged(MouseEvent e){
            moveLine(e.getX(), e.getY());
        }
    });

    }

    public Dimension getPreferredSize(){
        return new Dimension(200,100);
    }

    public void paintComponent(Graphics g){
        super.paintComponent(g);  
        paintArc(g);
        paintLine(g);
    }

    public void update(int oldValue) {
        int value = model.getValue();
        currentValue = value;
        angle = ratio * (currentValue - minValue);
        angle = Math.toRadians(angle);
        repaint();
    }

    //paints line if moved by mouse
    private void moveLine(int MouseX, int MouseY){
        double distance;
        double theta = 0;
        double xRef = 0;
        double yRef = 0;
        
        distance = Math.sqrt( Math.pow(MouseX - x2, 2) + Math.pow(MouseY - y2, 2) );
        if(MouseY <= 100 ){
            if(distance < 10 || release == 1){
                release = 1;
                xRef = 90.0;
                yRef = 15.0;
                xRef = MouseX - x1;
                yRef = y1 - MouseY;
                theta = Math.atan2(yRef, xRef);
                angle = theta;

                double temp = Math.toDegrees(angle);
                model.setValue((int)(temp/ratio + minValue));

                repaint();
            }
            else{
                release = 0;
            }
        }
    }

    //paints the arc
    private void paintArc(Graphics g){
        int x, y;
        
        //if the width is less than double the height
        if(getWidth() < getHeight()* 2){
            height = getWidth();
            width = getWidth();
            x = 0;
            y = 100 - (getWidth()/2);   
            
        }
        //if the double the height is more than the width
        else{
            width = getHeight() * 2;
            height = getHeight() * 2;
            x = (getWidth() /2 ) - height/2;
            y = 0;
        }

        //set the midpoint of the circle and starting point of the line
        x1 = getWidth() / 2;
        y1 = getHeight();

        g.setColor(Color.ORANGE);
        g.fillArc(x, y, width, height, 0, 180);   
    }

    //paints line if not moved by mouse
    public void paintLine(Graphics g){  
        double dcv, dminv, dmaxv;
        int radius;

        radius = width / 2;
   
        x2 = x1 + (int)(radius * Math.cos(angle));
        y2 = y1 - (int)(radius * Math.sin(angle));

        g.setColor(Color.BLUE);
        g.drawLine(x1, y1, x2, y2);
    }

}